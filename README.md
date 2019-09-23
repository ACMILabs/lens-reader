# Kiosk IV tap reader

The Kiosk IV tap reader runs on a Raspberry Pi 4, with an IDTech Kiosk IV reader and a strip of Adafruit DotStar LEDs attached, and is deployed via Balena.

The file `runner.py` runs the `idtech` C app in the background, and processes the output of the app. The C app simply continuously prints discovered NFC tag reads to `stdout`.

The Python script processes the output and makes 'tap' calls to XOS, and controls the lights attached to the RPi.

# Configuration values

## Environment (set in Balena or using defaults in the code)

```
AUTH_TOKEN=(secret)                 # the token to use for XOS API communication
BLINKA_FORCEBOARD=RASPBERRY_PI_4B   # Configuration for the Blinka lights
BLINKA_FORCECHIP=BCM2XXX
LABEL=1                             # The label ID of this reader
XOS_URL=https://museumos-prod.acmi.net.au/api/taps/ # The URL to use for API messages

DEBUG=false             # set to 'true' to use the DEBUG build of the idtech C code
SENTRY_ID=(secret)      # the ID to use for posting exceptions to Sentry
DEVICE_NAME=(set by Balena and used for READER_NAME)
READER_MODEL=(set by Balena)

TAP_OFF_TIMEOUT=0.5                 # seconds to wait without an NFC present before concluding the tag has been removed.
LEDS_BRIGHTNESS=1.0                 # how bright to make the LED strip
LEDS_BREATHE_TIME=5.0               # seconds to cycle between IN and OUT colours.
LEDS_BREATHE_COLOUR_OUT=36,26,10    # RGB Dim warm white
LEDS_BREATHE_COLOUR_IN=95,70,26     # RGB Medium warm white
LEDS_SUCCESS_COLOUR=255,238,202     # RGB Bright warm white
LEDS_FAILED_COLOUR=137,0,34         # RGB Medium red
LEDS_SIGNAL_TIMES=0.3,0.5,0.6       # Seconds to take for success/failed colour fade in, auto-hold (if needed), fade out.
```

# Compiling the `idtech` application (in C)

The `c_project` folder contains the source code for a simple application that configures the IDTech Kiosk IV reader and prints discovered NFC tags to `stdout`. To compile:

Ensure `make` is installed; either with `xcode-select --install` on OS X, or `apt-get install build-essential` on Raspberry Pi.

1. run `make all` in either the `c_project/Debug` or the `c_project/Release` folder.
2. Ensure the `libIDTechSDK.so` file - or a symlink to it - is in the path.
3. In the command line, run `./idtech` and test with an NFC tag.
4. To use the compiled result in the Python script, move it to `idtech` or `idtech_debug` in the `bin/arm_32` (Raspberry Pi) or `bin/mac` (OS X) folders.

# To deploy to Balena devices:

`balena push e__tap-reader-pi-4`

You can see the status of deployed devices on [the Balena dashboard](https://dashboard.balena-cloud.com/devices/dc0627e5962af329de637af277da9b3a/summary).

Note that the Docker image is hard-coded to use a 32-bit Raspberry Pi Python image, which runs inside the 64-bit BalenaOS. This is because if we don't then `libIDTechSDK.so` file is ignored as 'not compatible' during compilation and execution.