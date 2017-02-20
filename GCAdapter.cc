#include <iostream>
#include <mutex>
#include <future>
#include <atomic>
#include <nan.h>
#include <bitset>
#include <iomanip>
#include "GCAdapter.h"
#include "Flag.h"

using namespace std;

libusb_device_handle *device_handle = nullptr;
libusb_context *context = nullptr;

uint8_t controller_payload[37];
uint8_t controller_payload_swap[37];

atomic<int> controller_payload_size = { 0 };

thread adapter_thread;
GCAdapter::Flag adapter_thread_running;

mutex main_mutex;

uint8_t endpoint_out = 0;
uint8_t endpoint_in = 0;

NAN_METHOD(Setup) {
	auto return_value = Nan::New<v8::Number>(0);
    if(info.Length()>0) {
        Nan::ThrowError("Load() must not have arguments");
        return;
    }
	libusb_init(&context);
	libusb_device **list;
	ssize_t count = libusb_get_device_list(context, &list);
	bool case_test = false;
	for (int i = 0; i < count && !case_test; i++) {
		libusb_device *device = list[i];
		case_test = IsAccessible(device);
		if (case_test) {
			return_value = Nan::New<v8::Number>(AddAdapter(device));
		}
	}
	libusb_free_device_list(list, 1);
    info.GetReturnValue().Set(return_value);
}
bool IsAccessible(libusb_device *dev) {
	int return_value=0,kernel_value=0;
	libusb_device_descriptor descriptor;
	return_value = libusb_get_device_descriptor(dev, &descriptor);
	if (return_value < 0) {
		cout << "Error getting descriptor of USB device. Error code: " << return_value << endl;
		return false;
	}

	if (descriptor.idVendor == GAMECUBE_VID && descriptor.idProduct == GAMECUBE_PID) {
		cout << "Found GameCube Adapter" << endl;
	}
	else {
		return false;
	}
	return_value = libusb_open(dev, &device_handle);
	switch (return_value) {
		case 0:
			cout << "This adapter seems reachable. Trying to reach..." << endl;
			break;
		case LIBUSB_ERROR_ACCESS:
			cout << "LIBUSB_ERROR_ACCESS: gca-node does not have access to this adapter." << endl;
			return false;
			break;
		default:
			cout << "gca-node couldn't open this adapter. Error code: " << return_value << endl;
			return false;
			break;
	}
	return_value = libusb_kernel_driver_active(device_handle, 0);
	if (return_value == 1) {
		kernel_value = libusb_detach_kernel_driver(device_handle, 0);
		if (kernel_value != 0) {
			cout << "gca-node can't attach the kernel of this adapter. Error code:" << return_value << endl;
			return false;
		}
	}
	if (return_value == 0 || kernel_value == 0) {
		return_value = libusb_claim_interface(device_handle, 0);
		if (return_value != 0) {
			cout << "gca-node couldn't claim interface 0 of adapter. Error code:" << return_value << endl;
		}
		return return_value == 0;
	}
	return false;
}
int AddAdapter(libusb_device *dev) {
    int endpoint_number = 0;
	libusb_config_descriptor *config = nullptr;
	libusb_get_config_descriptor(dev, 0, &config);
	for (uint8_t iface=0; iface < config->bNumInterfaces; iface++) {
		const libusb_interface *interfaceContainer = &config->interface[iface];
		for (int i = 0; i < interfaceContainer->num_altsetting; i++) {
			const libusb_interface_descriptor *idesc = &interfaceContainer->altsetting[i];
			for (uint8_t epoint = 0; epoint < idesc->bNumEndpoints; epoint++) {
				const libusb_endpoint_descriptor *endpoint = &idesc->endpoint[epoint];
                endpoint_number++;
				if (endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN) {
					endpoint_in = endpoint->bEndpointAddress;
				}
				else {
					endpoint_out = endpoint->bEndpointAddress;
				}
			}
		}
	}

    return endpoint_number;
}
NAN_METHOD(Load) {
    if(info.Length()>0) {
        Nan::ThrowError("Process() must not have arguments");
        return;
    }
	int payload_size = 0, tmp = 0;
	uint8_t payload = 0x13;
	auto return_value = Nan::New<v8::Number>(libusb_interrupt_transfer(device_handle, endpoint_out, &payload, sizeof(payload), &tmp, 16));
	info.GetReturnValue().Set(return_value);
}
NAN_METHOD(Request) {
	if (info.Length()>0) {
		Nan::ThrowError("Load() must not have arguments");
		return;
	}
	adapter_thread_running.Set(true);
	adapter_thread = thread(Read);
	if (adapter_thread_running.TestAndClear()) {
		adapter_thread.join();
	}
	auto return_string = PollBytes(controller_payload);
	auto return_value = Nan::New<v8::String>(return_string).ToLocalChecked();

	info.GetReturnValue().Set(return_value);
}
NAN_METHOD(Stop) {
	int code = 0;
	if (adapter_thread_running.TestAndClear()) {
		adapter_thread.join();
	}
	if (device_handle) {
		code = libusb_release_interface(device_handle, 0);
		libusb_close(device_handle);
		device_handle = nullptr;
	}
	auto return_value = Nan::New<v8::Number>(code);
	info.GetReturnValue().Set(return_value);
}
void Read() {
	int payload_size = 0;
	int code = libusb_interrupt_transfer(device_handle, endpoint_in, controller_payload_swap, sizeof(controller_payload_swap), &payload_size, 16);

	lock_guard<mutex> lock(main_mutex);
	swap(controller_payload_swap, controller_payload);
	controller_payload_size.store(payload_size);

	this_thread::yield();
}
string PollBytes(uint8_t *results)
{
	stringstream return_value;
	//This is pretty messy. Must clean up.
	unsigned int header = results[0];
	unsigned int unknown = results[1];
	bitset<8> x(unknown);
	unsigned int buttonA = GetNthBit(results[2], 1);
	unsigned int buttonB = GetNthBit(results[2], 2);
	unsigned int buttonX = GetNthBit(results[2], 3);
	unsigned int buttonY = GetNthBit(results[2], 4);
	unsigned int padLeft = GetNthBit(results[2], 5);
	unsigned int padRight = GetNthBit(results[2], 6);
	unsigned int padDown = GetNthBit(results[2], 7);
	unsigned int padUp = GetNthBit(results[2], 8);

	unsigned int buttonL = GetNthBit(results[3], 4);
	unsigned int buttonR = GetNthBit(results[3], 3);
	unsigned int buttonZ = GetNthBit(results[3], 2);
	unsigned int buttonStart = GetNthBit(results[3], 1);
	//TODO: Are axes ints, floats or doubles?
	float mainStickX = results[4]/128.0f-1;
	float mainStickY = results[5]/128.0f-1;

	float cStickX = results[6]/128.0f -1;
	float cStickY = results[7]/128.0f -1;

	float triggerL = results[8]/255.0f;
	float triggerR = results[9]/255.0f;

	return_value.precision(5);

	return_value << "GCA Header = 0x" << hex << header << dec << endl;
	return_value << "Unknown value = " << x << endl;
	return_value << "A = " << buttonA << ", B = " << buttonB << ", X = " << buttonX << ", Y =" << buttonY << endl;
	return_value << "UP = " << padUp << ", DOWN = " << padDown << ", LEFT = " << padLeft << ", RIGHT =" << padRight << endl;
	return_value << "L = " << buttonL << ", R = " << buttonR << ", Z = " << buttonZ << ", START =" << buttonStart << endl << endl;
	return_value << "Stick Horiz = " << mainStickX << ", Stick Verti =" << mainStickY << endl;
	return_value << "C-Stick Horiz = " << cStickX << ", C-Stick Verti =" << cStickY << endl;
	return_value << "L Axis = " << triggerL << ", R Axis =" << triggerR << endl;

	return return_value.str();
}

unsigned int GetNthBit(uint8_t number,int n) {
	unsigned int bit = (unsigned)(number & (1 << n - 1));
	return bit >> n-1;
}
NAN_METHOD(Process) {
	if (info.Length()>0) {
		Nan::ThrowError("Process() must not have arguments");
		return;
	}
	adapter_thread_running.Set(true);
	adapter_thread = thread(Read);
	if (adapter_thread_running.TestAndClear()) {
		adapter_thread.join();
	}
	Nan::HandleScope arr_scope;
	v8::Handle<v8::Array> arr = Nan::New<v8::Array>();
	for (int i = 0; i < 4; i++) {
		arr->Set(i,GetGamepadStatus(controller_payload, i+1));
	}
	info.GetReturnValue().Set(arr);
}
NAN_METHOD(RawData) {
	if (info.Length()>0) {
		Nan::ThrowError("RawData() must not have arguments");
		return;
	}
	adapter_thread_running.Set(true);
	adapter_thread = thread(Read);
	if (adapter_thread_running.TestAndClear()) {
		adapter_thread.join();
	}
	stringstream return_value;
	return_value << "[";
	for (int i = 0; i < 10; i++) {
		return_value << bitset<8>(controller_payload[i]) << ",";
	}
	return_value << bitset<8>(controller_payload[10]) << "]";
	auto chain = Nan::New<v8::String>(return_value.str()).ToLocalChecked();
	info.GetReturnValue().Set(chain);
}

v8::Local<v8::Object> GetGamepadStatus(uint8_t * results, int port)
{
	using namespace v8;
	using namespace Nan;
	Local<Object> status = New<Object>();

	status->Set(New<String>("connected").ToLocalChecked(), New<Boolean>(GetNthBit(results[1 * port], 5)));

	status->Set(New<String>("buttonA").ToLocalChecked(), New<Boolean>(GetNthBit(results[2 * port], 1)));
	status->Set(New<String>("buttonB").ToLocalChecked(), New<Boolean>(GetNthBit(results[2 * port], 2)));
	status->Set(New<String>("buttonX").ToLocalChecked(), New<Boolean>(GetNthBit(results[2 * port], 3)));
	status->Set(New<String>("buttonY").ToLocalChecked(), New<Boolean>(GetNthBit(results[2 * port], 4)));

	status->Set(New<String>("padLeft").ToLocalChecked(), New<Boolean>(GetNthBit(results[2 * port], 5)));
	status->Set(New<String>("padRight").ToLocalChecked(), New<Boolean>(GetNthBit(results[2 * port], 6)));
	status->Set(New<String>("padDown").ToLocalChecked(), New<Boolean>(GetNthBit(results[2 * port], 7)));
	status->Set(New<String>("padUp").ToLocalChecked(), New<Boolean>(GetNthBit(results[2 * port], 8)));

	status->Set(New<String>("buttonL").ToLocalChecked(), New<Boolean>(GetNthBit(results[3 * port], 4)));
	status->Set(New<String>("buttonR").ToLocalChecked(), New<Boolean>(GetNthBit(results[3 * port], 3)));
	status->Set(New<String>("buttonZ").ToLocalChecked(), New<Boolean>(GetNthBit(results[3 * port], 2)));
	status->Set(New<String>("buttonStart").ToLocalChecked(), New<Boolean>(GetNthBit(results[3 * port], 1)));

	status->Set(New<String>("mainStickHorizontal").ToLocalChecked(), New<Number>(results[4 * port]/128.0 - 1));
	status->Set(New<String>("mainStickVertical").ToLocalChecked(), New<Number>(results[5 * port]/128.0 - 1));

	status->Set(New<String>("cStickHorizontal").ToLocalChecked(), New<Number>(results[6 * port]/128.0 -1));
	status->Set(New<String>("cStickVertical").ToLocalChecked(), New<Number>(results[7 * port]/128.0 -1));

	status->Set(New<String>("triggerL").ToLocalChecked(), New<Number>(results[8 * port]/256.0));
	status->Set(New<String>("triggerR").ToLocalChecked(), New<Number>(results[9 * port]/256.0));

	return status;
}

NAN_MODULE_INIT(Bridge) {
    NAN_EXPORT(target,Load);
    NAN_EXPORT(target,Setup);
	NAN_EXPORT(target,Request);
	NAN_EXPORT(target, Process);
	NAN_EXPORT(target, Stop);
	NAN_EXPORT(target, RawData);
}

NODE_MODULE(gca_node,Bridge)
