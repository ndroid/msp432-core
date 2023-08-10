# MSP432 Cores for use with Arduino Board Manager

This repository is to maintain and update cores for the MSP432P4xx Red Launchpad boards by Texas Instruments. This allows development for TI LaunchPads using the Arduino IDE (using [Arduino Boards Manager](https://support.arduino.cc/hc/en-us/articles/360016119519-Add-boards-to-Arduino-IDE)) rather than the [Energia IDE](http://energia.nu). The official Energia release provided support for a single MSP432 Launchpad, the MSP432P401R Red Launchpad board. This repository will provide an MSP432 core compatible with the **Arduino Boards Manager** which maintains and extends support for the **MSP432P401R** and adds support for the **MSP432P4111 Launchpad** board. In addition, this project extends the core to provide some bug fixes and new features, as detailed below.



## Background

### What Is Energia?

Energia is a fork/port of Arduino originally developed in 2012 for the TI Launchpads, or boards with MCUs from Texas Instruments such as the Stellaris or Tiva C Series. As reported on the [Energia](http://github.com/energia/Energia) project page in 2022, Energia is no longer maintained. The final release is 1.8.10E23, published in December 2019. Though it is no longer under active development, the processor cores developed for Energia may be used with the **Arduino IDE** using the **Arduino Boards Manager**.

A strong effort to maintain availability of the Energia tools and support their transition to Arduino has been provided by the [TI_Platform_Cores_For_Arduino](https://github.com/Andy4495/TI_Platform_Cores_For_Arduino) repository. 

### What Are the LaunchPads Supported?

Energia provided native support for numourous LaunchPad boards, including MSP430F series, MSP430G series, MSP432 series, Stellaris and Tiva C series. This project maintains the platform core for only the LaunchPad with **MSP432P401R**, with added support for the **MSP432P4111**.

### Energia Application Libraries and BoosterPacks Support

The Energia IDE includes several libraries at the application level of the IDE, and includes others in the platform cores. Some of the application level libraries are now available independently for use with Arduino. Two popular libraries can be found here:

- [LCD_SharpBoosterPack_SPI](https://github.com/Andy4495/LCD_SharpBoosterPack_SPI)
- [OneMsTaskTimer](https://github.com/Andy4495/OneMsTaskTimer)

This MSP432 core includes the following libraries:

* Educational BoosterPack MKII
* Servo 
* IR remote (NEC transmit and receive)
* SPI
* Wire (I2C)

### Energia Resources

* [Website](http://energia.nu) - Full documentation including tutorials, reference, pins maps, FAQ and much more!
    * [Download](http://energia.nu/download/) - Download a compiled application for Linux, Mac OS X, or Windows.
    * [GitHub repository](http://github.com/energia/Energia) 
* [Wiki](https://github.com/energia/Energia/wiki) - Find basic information and tutorials here.
* [LaunchPad forum at 43oh](http://forum.43oh.com/forum/28-energia/) - A community around the original LaunchPad  but also the newer ones.



## Adding the MSP432 Launchpad Boards into Arduino IDE

1. Open the Arduino Preferences pane.
2. Click on the box next to the text field labeled `Additional Boards Manager URLs`.
3. Add the following URL (on a line of its own) to the list:
  `https://raw.githubusercontent.com/ndroid/msp432-core/main/package_msp432_index.json`
4. Click OK to close the window and OK to close the Preferences pane
5. Open `Tools->Board->Boards Manager...` menu item
6. Select the board platform you wish to install:
    - Use the search box at the top to make it easier to find the board (i.e., "MSP432")
    - Hover the mouse over the board platform you want, and click "Install"
7. Once the board package is installed, you can select the board you want with the `Tools->Board` menu.


## Installing Launchpad USB Drivers 

If you have not previously developed with the Energia IDE or Code Composer Studio, then you may need to install the LaunchPad drivers for Windows if developing on Windows. The zip file with the driver installer can be downloaded from [Energia Driver Package](http://s3.amazonaws.com/energiaUS/files/energia_drivers.zip). 

More information can be found in the Energia guide [Installing the LaunchPad drivers](https://energia.nu/guide/install/windows/).


<br>

# Change Log

__`version 5.29.4`__

<ul> Initial release, adapted from msp432r version 5.29.1 core </ul>

* features
    * added MSP432P4111 LaunchPad board
    * added support for Serial2 and Serial3
    * modified Servo library to use Timer32 to avoid using Timer_A which may conflict with PWM output requests (e.g. analogWrite and IRremote)
        * attempts to allocate Timer32_2 first (will attempt Timer32_1 if T32_2 is unavailable) 
    * added MultipleServos example in Servo library
    * added TinyIRremote library to support IR NEC encoding receive and transmit applications
        * multiple receivers supported without timer usage with IRreceiver class
        * multiple transmitters supported with a single Timer_A module with IRsender class

* added features to support defined Arduino features
    * added definition of LED_BUILTIN for compatibility with Arduino examples
    * added analogWriteResolution implementation (supported by some Arduino boards)
    * added availableForWrite to Serial implementation 
    * added pulseInLong implementation
    * added implementation of HardwareSerial configuration using predefined SERIAL_XYZ constants
        * supports setting data bits, parity and stop bits in addition to baud rate

* bug fixes
    * corrected A22 pin assignment (now correctly assigned to pin 60)
    * corrected potential array index out-of-bounds reference in Servo library
    * added value range verifications in Servo library methods
    * corrected display format of strings for values between -1.0 and 0 in EduBPMKII_Screen library
    * corrected indexing error in wiring_analog.c which prevented more than one PWM output being assigned using Timer_A2
    * corrected one-off error in conditional in wiring_analog.c which reduced max PWM count by one
    * corrected duplicate definition of Board_WIFI and Board_WATCHDOG


__`version 5.29.2`__

* revision of version 5.29.0 to support MSP432P401R on Arduino (provided by TI Platform Cores For Arduino [repository](https://github.com/Andy4495/TI_Platform_Cores_For_Arduino))

__`version 5.29.1`__

* last official Energia release for MSP432P401R board

__`version 5.29.0-beta1`__

* last developmental release for MSP432P4111 board

<br>

# 

## Repository Contents

### Package Index JSON File

This file is included in the repository to provide a permanent link for the Arduino Board Manager. It specifies the supported boards, the required cores and tools, and links to those resources.

The Package Index file names need to follow the convention specified in the Arduino [Package Index Specification](https://arduino.github.io/arduino-cli/0.21/package_index_json-specification/). Specifically, the file name needs be of the form `package_YOURNAME_PACKAGENAME_index.json`. The prefix `package_` and suffix `_index.json` are mandatory, while the choice of `YOURNAME_PACKAGENAME` is left to the packager.

| Package Index File               | MSP Board Version  | Notes |
| ------------------               | ------             | ----- |
| `package_msp432_index.json`      | 5.29.4  | MSP432P4xx boards only, use with local Arduino IDE/CLI |



### Board Package Files

Located in the **cores** directory. These files are referenced by the package index json files. This directory will host package releases, beginning with version 5.29.4.

- `msp432-5.29.4.tar.bz2`

### Board Platform Compiler and Tool Versions

The tools are specific to the board package platform and version. These are currently linked in the package index json file using the original Energia links. They have been saved in a separate respository should those links become unusable in the future.

| Board Version  | Compiler                         | dslite     | ino2cpp |
| -------------- | --------                         | ------     | ------- |
| MSP432 5.29.4  | arm-none-eabi-gcc 8.3.1-20190703 | 9.3.0.1863 | 1.0.7   |

| Tool Download Links              |              |             |             |
| :------------------------------- | ------------ | ----------- | ----------- |
| arm-none-eabi-gcc 8.3.1-20190703 | [Wndows][1]  | [MacOS][2]  | [Linux][3]  |
| dslite 9.3.0.1863                | [Wndows][4]  | [MacOS][5]  | [Linux][6]  |
| ino2cpp 1.0.7                    | [Wndows][7]  | [MacOS][8]  | [Linux][9]  |


[1]: https://s3.amazonaws.com/energiaUS/tools/windows/gcc-arm-none-eabi-8.3.1-20190703-windows.tar.bz2
[2]: https://s3.amazonaws.com/energiaUS/tools/macosx/gcc-arm-none-eabi-8.3.1-20190703-mac.tar.bz2
[3]: https://s3.amazonaws.com/energiaUS/tools/linux64/gcc-arm-none-eabi-8.3.1-20190703-x86_64-pc-linux-gnu.tar.bz2
[4]: https://s3.amazonaws.com/energiaUS/tools/windows/dslite-9.3.0.1863-i686-mingw32.tar.bz2
[5]: https://s3.amazonaws.com/energiaUS/tools/macosx/dslite-9.3.0.1863-x86_64-apple-darwin.tar.bz2
[6]: https://s3.amazonaws.com/energiaUS/tools/linux64/dslite-9.3.0.1863-i386-x86_64-pc-linux-gnu.tar.bz2
[7]: https://s3.amazonaws.com/energiaUS/tools/windows/ino2cpp-1.0.7-i686-mingw32.tar.bz2
[8]: http://s3.amazonaws.com/energiaUS/tools/macosx/ino2cpp-1.0.7-x86_64-apple-darwin.tar.bz2
[9]: https://s3.amazonaws.com/energiaUS/tools/linux64/ino2cpp-1.0.7-x86_64-pc-linux-gnu.tar.bz2


## References

- Energia IDE [application](https://energia.nu/) and source code [repo](https://github.com/energia/Energia)
- Energia MSP432 core [repo](https://github.com/energia/msp432r-core)
- Arduino [Platform Specification](https://arduino.github.io/arduino-cli/0.21/platform-specification/)
- Arduino [Package Index JSON Specification](https://arduino.github.io/arduino-cli/0.21/package_index_json-specification/)
- Arduino instructions for [installing cores](https://docs.arduino.cc/learn/starting-guide/cores)
- TI Platform Cores For Arduino [repository](https://github.com/Andy4495/TI_Platform_Cores_For_Arduino)
- Unofficial list of Arduino 3rd Party [Board Manager URLs](https://github.com/arduino/Arduino/wiki/Unofficial-list-of-3rd-party-boards-support-urls)


## License

The majority of the files in this repo are either a copy or a derivation of Energia platform cores, which are licensed under the GNU [Lesser General Public License v2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html) per [Energia](https://github.com/energia/Energia/blob/master/license.txt). For consistency, the non-Energia derived software and files in this repository are also released released under LGPL v2.1. See the file `LICENSE.txt` in this repository.
