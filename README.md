# E-39 Indoor Positioning System (IPS)

**Project Status:** Ready to manufacture

If you are new to GitHub and would like to create a derivative of this project, please have a look at GitHub instructions to [fork a repo](https://help.github.com/en/articles/fork-a-repo)

## Description

Out of our passion to solve the issue of time waste in the robot creation process, ARC was born. The ARC software platform reduces the time it takes to get from idea to implementation. Since 2011 we have been listening to the challenges robot builders encounter and continue to provide them with an ever expanding toolbox of solutions. Join us on this journey!

We didn't stop at the software, we also made time saving hardware solutions. This IPS reference design is one of them. The IPS has two cameras that alternate frames. One camera has a normal lens with an Infrared blocking filter and the other has an infrared sensitive lens with a daylight blocking filter. A unique feature of this design is that it uses wide angle lenses and can detect the IPS TX at many different angles. We want to share these files with you so you can create your own version for your community and customers to enjoy!

**Features:**
- Combined with the IPS TX, this system can be used for night vision
- SWD Programming port
- Two types of communication modes: USB or EZ-B camera port
- Switch for USB or EZ-B mode
- Replaceable Camera modules
- Replaceable Wide angle lenses
- On-board 3.3V, 2.8V and 1.5V voltage regulation
- 5V tolerant I/O pins
- RGB status indicator
- ARC skill available
- Voltage requirement: 3.2-16VDC (3.3V (EZ-B) or 5V (USB) Typical)
- Current draw: 200mA
- Dimensions: 37.7(W) x 51.5(L) x 16(H) (mm)
- Weight: 11g

**Major components:** 
- STM32F407VET6 32-bit Cortex-M4 ARM microcontroller (custom firmware provided)
- 2 x HDF3M-811 OV2640 2 Megapixel Camera
- TS3DV20812RHHR 1:2 Multiplexer

**Manufacturing notes:** 
1. Supplier: programs custom firmware into the STM32F407VET6 at their facility before sending to manufacturer
2. Supplier: provides pre-built HDF3M-811 OV2640 Cameras with custom ribbon length
3. Supplier: provides HDF3M-811 OV2640 Camera with custom IR filter and Wide angle lens 
4. Supplier: provides HDF3M-811 OV2640 Camera with custom Daylight filter and Wide angle lens 
5. Manufacturer: Dual side placement and soldering of SMT components
6. Manufacturer: Single Side soldering of THT components

This project requires the IPS Transmitter: https://github.com/synthiam/E-44_IPS_Transmitter

## Contents

[**Documentation:**](https://github.com/synthiam/E-39_Indoor_Positioning_System/tree/master/E-39%20Documentation) Schematic PDF, Datasheet PDF, BOM, Assembly PDFs

[**Hardware:**](https://github.com/synthiam/E-39_Indoor_Positioning_System/tree/master/E-39%20Hardware) Altium PCB design File, Altium SCH Design File, Gerbers

[**Firmware:**](https://github.com/synthiam/E-39_Indoor_Positioning_System/tree/master/E-39%20Firmware) Code, Compiled Binary

*Altium Libraries are also available <a href="https://github.com/synthiam/Synthiam_Altium_Librairies">here</a>*

## Photos

<p align="left">
<img src="https://live.staticflickr.com/65535/46962806124_cfa921f407_k.jpg" width="683" height="383">
<img src="https://live.staticflickr.com/65535/47752093741_de2e21d5d2_k.jpg" width="683" height="383">
<img src="https://live.staticflickr.com/65535/32808700827_8a51697d4c_k.jpg" width="683" height="383"></p>

## ARC Skill

The ARC skill for this reference design is very easy to use! Simply click on the area you'd like the robot to move to and it moves there! 

Link: https://synthiam.com/Software/Manual/Indoor-Positioning-System-17492

<a href="https://synthiam.com/Software/Manual/Indoor-Positioning-System-17492"><img src="E-39.gif" width="683" height="383"></a>

## Contact

For profit use of these files requires written consent. Contact partners@synthiam.com. For everyone else, party on!

Synthiam Website: https://synthiam.com

## License

This project is released under the following licenses:

**Hardware:** Creative Commons Plus Attribution-NonCommercial 4.0 International (CC+ BY-NC 4.0)

**Firmware:** Apache 2.0 + “Commons Clause” License Condition v1.0

Please see [LICENSE.md](https://github.com/synthiam/E-39_Indoor_Positioning_System/blob/master/LICENSE.md) for license details.

<a href="https://synthiam.com"><img src="https://live.staticflickr.com/65535/47791527651_358dffb302_m.jpg"></a>
