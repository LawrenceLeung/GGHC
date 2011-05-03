This readme is for the Jigsaw Renaissance Great Global Hackerspace Challenge
(GGHC). The project is for a device that allows groups to make music
collaboratively using natural gestures. The product is tentatively called
Jigbox.

=== Getting Started:

These instructions assume you are developing in Windows and have a Value-line
Discovery board connected via USB. The value-line can be used as the
development environment OR as the programmer for the Olimex board.

Install the Atollic TrueStudio Lite edition.

(These instructions assume that you clone the git repository into your
directory "C:\Projects". Change that part of the path if you didn't.)

Clone the Github repository at: https://github.com/LawrenceLeung/GGHC

   C:\Projects > git clone git://github.com/LawrenceLeung/GGHC.git

Open a new workspace in Atollic, and import the project from  C:\Projects\GGHC\Project\Atollic

Open the Atollic Project Properties, and go to the page for "Resource/Linked
Resource", then to the tab for "Path Variables".

Change the value of the "GGHC_SRC" variable to C:\Projects\GGHC\firmware

If you're using a processor other than the STM32F103RGT6, you should change
that setting in Atollic.  Of course, the code may not work well in that case.

To change the processor type, you will have to:
  - go to the project properties, under "C/C++ General", "Paths and Symbols", and choose the "Symbols" tab.
  - change the #define of STM32F10X_XL to one appropriate for your hardware (for instance, STM32F10X_MD_VL for the STM32F100RB on the Value Line Discovery board).
  - select the "Source Location" tab, then the "/Jigbox/GGHC_SRC" folder; then hit the "Edit Filter..." button.
  - edit the entry for Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/TrueSTUDIO/startup_stm32f10x_md_vl.s
    (if you're using the VL Discovery board), and change it to have a filename of "startup_stm32f10x_xl.s"

