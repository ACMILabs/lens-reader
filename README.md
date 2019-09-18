# Kiosk IV tap reader

A Kiosk IV tap reader running on a Raspberry Pi deployed via Balena.

The file `runner.py` runs the `idtech` C app in the background, and processes the output of the app. The C app simply continuously prints discovered NFC tag reads to `stdout`.

The Python script processes the output and makes 'tap' calls to XOS, and controls the lights attached to the RPi.

# Compiling the `idtech` application (in C)

The `c_project` folder contains the source code for a simple application that configures the IDTech Kiosk IV reader and prints discovered NFC tags to `stdout`.

## To compile on Mac

1. Install Eclipse for C/C++ and the OS X developer command-line tools
2. When Eclipse starts, choose "Import Existing Project" and choose the `c_project` folder.
3. In the Build Configuration (hammer) drop-down, choose Debug or Release.
4. Click the Build Button. This will compile the `idtech` executable in the folder that matches the Build Configuration.
5. Ensure the `libIDTechSDK.dylib` file - or a symlink to it - is in the path or the same folder as the executable.
6. In the command line, run `./idtech` and test with an NFC tag.

## To compile on Raspberry Pi

1. Install libusb with `apt-get install libusb-dev`
2. run `make all` in either the `Debug` or the `Release` folder.
3. Ensure the `libIDTechSDK.so` file - or a symlink to it - is in the path or the same folder as the executable.
5. In the command line, run `./idtech` and test with an NFC tag.