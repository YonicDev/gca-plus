![gca-node logo][logo]

gca-node is a NodeJS addon that adds Nintendo&reg; Wii U GameCube&trade; Adapter native support for NodeJS applications.

## Usage

**gca-node currently only builds on Windows 7+ 64-bit and Linux**, although there is planned support for Linux and Mac OS.
As of now, it can only be used on NW.js.

### Windows

#### Prerequisites
  * **Visual Studio 2015** with standard C++ tools.
  * **Node 7.5.0** or higher, with npm installed.
  * **Zadig**: You must use it to replace the HID driver installed on Windows for the adapter to a generic WinUSB one.
  
### Building

 1. Download or clone the gca-node repository.
 2. Install `node-gyp` globally (`npm install node-gyp -g`)
 3. Open the CLI inside the repository and execute `node-gyp rebuild`.

### Linux
Tested on Arch Linux and Ubuntu, but it seems to be working for almost any Linux distro with a fairly recent linux kernel.

#### Prereqisites
When using your system's package manager. Use the specific commands for your distro.

 * **Make**. It can be already installed on your system. If not, install it from your system's package manager.
 * **libusb-1.0**. You can install it from your system's package manager. If specified, please install the -dev packages.
 * **Node 7.5.0** or higher, with npm installed.

#### Building

 1. Download or clone the gca-node repository.
 2. Install `node-gyp` globally (`sudo npm install node-gyp -g`)
 3. Open the CLI inside the repository and execute `node-gyp rebuild`.

#### Configuration
Like Windows, Linux distros require some special configuration. Linux fully supports the adapter's built-in drivers, but an udev rule must be added in order to allow access for gca-node.

 1. Go to `/lib/udev/rules.d/`.
 2. Edit `XX-gcadapter.rules`, if there isn't one, create a file with the following `51-gcadapter.rules`.

Here are the contents of the file:
`SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", ATTRS{idVendor}=="057e", ATTRS{idProduct}=="0337", MODE="0666"`

### test.js
Once everything has been configured, you may run a test program by running `node test.js`.

### NW.js
This is common for all operating systems.

1. Download NW.js SDK flavor.
2. Add `gca-node` to your project's `package.json` dependencies.
3. Copy the included batch program for your required OS:
    * **[Windows]:** `configure.bat`
    * **[Linux]:** `configure.sh`
4. Open the CLI inside your NW.js project, and run the copied configure file to install the addon.
5. **[Tip]:** Whenever you need to use gca-node, use an alias for require so that other NodeJS tools like webpack do not mistake it as a NodeJS module.

   ```
    var native_require = eval('require');
    gca_node = native_require('gca-node.node');
   ```

## gca-node API
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
  * **What is the difference between gca-node and gca+?**
     *  gca-node and the upcoming sister project [gca+][1] share almost the same source code. gca-node is designed for NodeJS applications, and gca+ is designed for C++ applications such as Unreal Engine.
  * **Will gca-node support rumble?**
     * gca-node 2.0 will be able to send rumble commands to the controllers.
  * **Will gca-node support third party GameCube Controllers?**
     * This has not been tested, but in theory it is very likely that third party GameCube controllers are compatible with gca-node. However, their extra features (i.e. turbo mode) will most likely be not supported due to the adapter's specifications.
  * **Will gca-node support third party GameCube USB Adapters?**
     * It is very unlikely due to the structure of gca-node as of now.
  * **Will gca-node support connection with Game Boy Advance with GBA Link?**
     * No. Unfortunately, the specifications of the adapter make it incompatible with the GBA Link. Even if it were to be compatible, remotely interfacing with the Game Boy Advance is currently impossible.
  * **When will gca-node be cross-platform?**
     * You can test cross-platforming support on the crossplatform branch, though it is expected to be on the master branch on gca-node 1.4.x.
  * **Will gca-node br available for 32-bit platforms?**
     * gca-node 1.5.x will start experimenting with 32-bit platforms.
  * **Will gca-node support [electron][2] and apm?**
     * Probably not. Testings with electron have reported problems due to using a different version of NodeJS and incomaptibilities with apm. We will investigate on a workaround during gca-node 2.x.
  * **[Windows] Why is it necessary to use just Zadig? Can't I just use [ElMassivo's USB GameCube Adapter][3] instead?** 
     * *ElMassivo's USB GameCube Adapter must remain inactive while gca-node is running*, as it claims the only interface available of the adapter. Without any other free interfaces, gca-node is unable to use the adapter, and viceversa.
     * However, you can install ElMassivo's USB GameCube Adapter and not use it, since Zadig is a component of this program.
  * **Why is gca-node better than HTML5 Gamepad API?**
     * HTML5 Gamepad API requires ElMassivo's USB GameCube Adapter, which does not bring all native capabilities that gca-node offers, like real-time input or rumble support.
     * Chromium's support for Nintendo&reg; Wii U GameCube&trade; Adapter is currently incomplete and glitchy.

[logo]: http://i.imgur.com/quWt3jK.png
[1]: https://github.com/yonicstudios/gca-plus
[2]: https://github.com/electron/electron
[3]: http://m4sv.com/page/wii-u-gcn-usb-driver