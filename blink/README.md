Instructions to run our blink program:

0. Install ubuntu. Learn how to use it.
1. Install SCons using apt-get
2. Download the xc16 microchip compiler for your architecture from microchip.com/compilers
3. Run the installer in a terminal. Accept the defaults, except if they will do dumb things.
	POTENTIAL PITFALL: if you get a segmentation fault, run the xc16 installer with the '--mode text' flag.
4. Download the bootloader code from elecanisms.olin.edu/internal/bootloader.zip Unzip this into somewhere you can remember. 
5. Clone our git repo, eschneider1992/elecanisms, somewhere near the unzipped bootloader code.
55. Reset the elecanisms board holding switch1
6. To build our code, navigate into the blink folder and run 'scons'. This will also load the compiled hex file onto the elecanisms board. 
7. Reset the board