This readme is for the Jigsaw Renaissance Great Global Hackerspace Challenge (GGHC). The project is for a device that allows groups to make music collaboratively using natural gestures. The product is tentatively called Jigbox.

Getting Started:
These instructions assume you are developing in Windows and have a Value-line Discovery board connected via USB. The value-line can be used as the development environment OR as the programmer for the Olimex board.
Download and install ST Link and IAR Embedded Workbench. The IAR Workbench must be registered with IAR and they will then give you a serial number and a key.

Get the source code from Github at:
https://github.com/LawrenceLeung/GGHC

To work on the code just load the workspace into IAR Embedded workbench IDE
File | Open | Workspace...

The code is based on the Olimex examples, that can be downloaded from: http://www.olimex.com/dev/stm32-103stk.html

The base example chosen was the AudioDevice. If you need code from one of the other examples you should be able to merge the folders / files
.\App
.\Module
main.c

To load the code to your target check that the correct build configuration is selected for your board. The drop down at the top of the Workspace pane should show the options:
Dscvr 103RB flash debug
Dscvr 103RG flash debug
Olimx flash debug
Select the one that matches your dev board. The standard Discover board should use "Dscvr 103RB flash debug" while the ones that Ned has replaced the processor on should use "Dscvr 103RG flash debug".
