Instructions to run our blink program:

0. Install ubuntu. Learn how to use it.
1. Install SCons using apt-get
2. Download the xc16 microchip compiler for your architecture from microchip.com/compilers
3. Run the installer in a terminal. Accept the defaults, except if they will do dumb things.
	33. POTENTIAL PITFALL: if you get a segmentation fault, run the xc16 installer with the '--mode text' flag.
	34. POTENTIAL PITFALL: If you run the .run file from the terminal and nothing appears, try installing the 32-bit libraries... lib32z1, lib32ncurses5, lib32bz2-1.0, openjdk-7-jre. sudo apt-get install libc6:i386 libx11-6:i386 libxext6:i386 libstdc++6:i386 libexpat1:i386
4. Download the bootloader code from elecanisms.olin.edu/internal/bootloader.zip Unzip this into somewhere you can remember. 
	44. Follow the instructions in the bootloader readme to install the correct USB devices.
	45. Protip: use lsusb to see attached USB devices. If you see a device with 6666:4321, your board is in bootloader mode and ready to accept code.
5. Clone our git repo, eschneider1992/elecanisms, somewhere near the unzipped bootloader code.
55. Press the reset button while holding switch1, release the reset button, then release switch1. You can run lsusb to confirm your board is ready.
6. To build our code, navigate into the blink folder and run 'scons'. This will also load the compiled hex file onto the elecanisms board. Our code is a binary counter with the 3 LEDs.
