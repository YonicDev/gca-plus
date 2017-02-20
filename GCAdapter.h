#ifdef _WIN32
#include "libusb.h"
#else
#include <libusb-1.0/libusb.h>
#endif
#include <sstream>

#pragma once

int AddAdapter(libusb_device *dev);
NAN_METHOD(Load);
NAN_METHOD(Setup);
NAN_METHOD(Request);
NAN_METHOD(Process);
NAN_METHOD(Stop);
NAN_METHOD(RawData);
void Read();
bool IsAccessible(libusb_device *dev);
std::string PollBytes(uint8_t *results);
unsigned int GetNthBit(uint8_t number, int n);
v8::Local<v8::Object> GetGamepadStatus(uint8_t * results, int port);

const uint16_t GAMECUBE_VID = 0x057E;
const uint16_t GAMECUBE_PID = 0x0337;
