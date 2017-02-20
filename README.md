![gca+ logo][logo]

gca+ is the base C++ code of the [gca-node][1] NodeJS addon which adds Nintendo&reg; Wii U GameCube&trade; Adapter native support for C++ applications.

Unreal Engine support is yet to be added.

## Prerequisites

**gca+ is only currently supported on Windows Vista+ 64-bit and Linux**, although there is planned support Mac OS and 32-bit systems.

### Windows

  * A built version of **libusb-1.0** (static)
  * **Zadig**: You must use it to replace the HID driver installed on Windows for the adapter to a generic WinUSB one.

### Linux
Tested on Arch Linux and Ubuntu, but it seems to be working for almost any Linux distro with a fairly recent linux kernel.

When using your system's package manager. Use the specific commands for your distro.

 * **Make**. It can be already installed on your system. If not, install it from your system's package manager.
 * **libusb-1.0**. You can install it from your system's package manager. If specified, please install the -dev packages.


## Configuration

### Linux

Linux fully supports the adapter's built-in drivers, but an udev rule must be added in order to allow access for gca+.

 1. Go to `/lib/udev/rules.d/`.
 2. Edit `XX-gcadapter.rules`, if there isn't one, create a file with the following `51-gcadapter.rules`.

The file must contain the following:

`SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", ATTRS{idVendor}=="057e", ATTRS{idProduct}=="0337", MODE="0666"`


## gca+ API
To be added on Wiki pages.

### Setup()
Detects the first Nintendo&reg; Wii U GameCube&trade; Adapter connected in your computer.

Returns 0 if the setup has detected one.

### Load()
Claims the interface to be used on the NodeJS application.

Returns 0 if an interface has been succesfully claimed.

### Process()
Returns an array of 4 objects with the current status of each port of the adapter.

Each object contains a status for each port, whether if there is a GameCube&trade; Controller connected or not.

### Request()
Returns debug information about the current status of the GameCube controllers.

### RawData()
Returns raw binary data of the status of the Adapter.

### Unload()
Releases the interface so that it can be used by other applications.

Returns 0 if succesful.

### Stop()
Closes the Nintendo&reg; Wii U GameCube&trade; Adapter so that it can be safely unplugged.

Returns 0 if succesful.

## FAQ
  * **What is the difference between gca+ and gca-node?**
     *  gca+ and the sister project [gca-node][1] share almost the same source code. gca-node is designed for NodeJS applications, and gca+ is designed for C++ applications such as Unreal Engine.
  * **Will gca+ support rumble?**
     * gca+ 2.0 will be able to send rumble commands to the controllers.
  * **Will gca+ support third party GameCube Controllers?**
     * This has not been tested, but in theory it is very likely that third party GameCube controllers are compatible with gca+. However, their extra features (i.e. turbo mode) will most likely be not supported due to the adapter's specifications.
  * **Will gca+ support third party GameCube USB Adapters?**
     * It is very unlikely due to the structure of gca+ as of now.
  * **Will gca+ support connection with Game Boy Advance with GBA Link?**
     * No. Unfortunately, the specifications of the adapter make it incompatible with the GBA Link. Even if it were to be compatible, remotely interfacing with the Game Boy Advance is currently impossible.
  * **When will gca+ support Mac OS?**
     * It is expected to support Mac OS 10.9 in gca+ 1.2.x.
  * **Will gca+ br available for 32-bit platforms?**
     * gca+ 1.3.x will start experimenting with 32-bit platforms.
  * **Will gca+ support Unity?**
     * We will investigate support in Unity in gca+ 2.x.
  * **Will gca+ support CRYENGINE?**
     * Yes, although currently there are no plans on support in 1.x.
  * **[Windows] Why is it necessary to use just Zadig? Can't I just use [ElMassivo's USB GameCube Adapter][2] instead?** 
     * *ElMassivo's USB GameCube Adapter must remain inactive while gca+ is running*, as it claims the only interface available of the adapter. Without any other free interfaces, gca+ is unable to use the adapter, and viceversa.
     * However, you can install ElMassivo's USB GameCube Adapter and not use it, since Zadig is a component of this program, although **this is not recommended.**
  * **Why is gca+ better than vJoy?**
     * Applications with gca+ unlock all the potential of the GameCube Controller&trade;, and allow them to interface with the Adapter natively, unlike vJoy.
     * Although vJoy is compatible with a fair amount of applications and games, it produces a mere virtualization of the Adapter's ports and thus it is not native, so some features like rumble and real-time inputs are lost.
     * gca+ is cross-platform. vJoy is Windows only.

[logo]: http://i.imgur.com/FkTDjc4.png
[1]: https://github.com/yonicstudios/gca-node
[2]: http://m4sv.com/page/wii-u-gcn-usb-driver