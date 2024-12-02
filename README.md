# Lens reader

The Lens reader runs on a Raspberry Pi 4, with an IDTech Kiosk IV reader and a strip of Adafruit DotStar LEDs attached, and is deployed via Balena.

![Lens Reader CI](https://github.com/ACMILabs/lens-reader/workflows/Lens%20Reader%20CI/badge.svg)

The file `runner.py` runs the `idtech` C app in the background, and processes the output of the app. The C app simply continuously prints discovered NFC tag reads to `stdout`.

The Python script processes the output and POSTs 'tap' API calls to `TARGET_API_ENDPOINT`, and controls the lights attached to the RPi.

# Features

* Read a nearby NFC tag and post a tap message to a given API endpoint
* Light up LED strips in nominated colour in response to a tap
* Light up LED strips in an error colour in case of error
* Allow LED strips to be controlled via API from an external source
* Can be configured, deployed and rebooted via Balena
* Optionally scan a ticket barcode instead of an NFC tag

# Hardware

* Raspberry Pi 4 Model B, 2GB RAM
* Official power supply
* IDTech Vivopay Kiosk IV integrated RFID reader
* 10cm USB-to-micro-USB cable (with 90-degree plugs for space economy)
* 16GB SD Card
* 20cm Segment of Pololu 3088  SK9822/APA102C-Based LED Strip 5m
* Light diffusing surface
* Ethernet cable
* DFRobot DHT22 Temperature and Humidity sensor
* Housing

## Optional barcode reader

* SparkFun DE2120 barcode scanner

To setup the barcode reader, set the environment variable: `READER_MODEL=Sparkfun DE2120`

Then scan this barcode to enable USB mode:

<img src="de2120-usbcom-mode.png">

# Configuration values

## Environment (set in Balena or using defaults in the code)

```
AUTH_TOKEN=(secret)                 # the token to use for XOS API communication
BLINKA_FORCEBOARD=RASPBERRY_PI_4B   # Configuration for the Blinka lights
BLINKA_FORCECHIP=BCM2XXX
LABEL=1                             # The label ID of this reader
# The URL to use for API messages. Maker Moments intercept the messages and so use a different URL.
TARGET_API_ENDPOINT=https://xos.acmi.net.au/api/

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
LEDS_CONTROL_OVERRIDE=false         # set to 'true' to override lens tap LED responses with LED API control commands
```

# Tap API calls
The following is the kind of message POSTed by the Lens Reader hardware to `TARGET_API_ENDPOINT`.
```
       {
            "atr": "a5292afe",
            "uid": "98cf3848549405",
            "experience_id": null,
            "tap_datetime": "2020-01-08T16:47:39.700188+11:00",
            "maker_moment": null,
            "label": 1,
            "data": {
                "lens_reader": {
                    "reader_ip": "172.17.0.3",
                    "mac_address": "24:2A:C1:10:00:3",
                    "reader_name": "nfc-3",
                    "reader_model": null
                }
            }
        }
```

## For maker moments

Each maker moment will provide its own `TARGET_API_ENDPOINT` URL to receive tap messages (with null `maker_moment` and `experience_id` values as above) at `<TARGET_API_ENDPOINT>/taps/`. The maker moment software will populate `maker_moment` and `experience_id` values and POST the resulting tap to the XOS taps API endpoint at https://xos.acmi.net.au/api/taps/.

* `maker_moment` is a constant integer which determines the maker moment that was tapped. XOS team can advise this value for each maker moment.
* `experience_id` is a unique string that identifies the experience which was just saved. The ID can be used at retrieval time to e.g. locate specific media and metadata on cloud storage. In other words, all media and metadata should be identifiable from constants and the `experience_id`.

The default LED behaviour is never to interrupt a lens tap, but should a maker moment need to override the LED control while a lens is still on the lens reader, `LEDS_CONTROL_OVERRIDE` must be set `true`.

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

# Lights API

The lights API allows controlling the lens reader's LEDs. This API is exposed by the device so the full endpoint URL would be `http://<device-ip>:8082/api/lights/`.

## POST `/api/lights/`

This endpoint allows toggling the lights on/off with an RGB colour, a ramp time and a fade percentage.

To turn the lights on, set the `cross_fade` value to a float greater than 0, up to 1.

To turn the lights off, set the `cross_fade` value to 0.

**Note**: When the `cross_fade` value is greater than `0`, Taps are not sent to XOS.

### Headers
```
Content-Type: application/json
```

### Body
```
{
    "rgb_value": [int, int, int],
    "ramp_time": float,
    "cross_fade": float
}
```
- `rgb_value` is a list of three values representing an RGB colour. Each value in the list should be 0-255.
- `ramp_time` is a float that represents the time in seconds that the LEDs take to go from idle to the specified colour.
- `cross_fade` is a float 0-1 that represents the fade percentage of the specified colour. 0.0 = No effect; 1.0 = Total override.

### Response

200 on success

400 on invalid values in the body

409 if trying to turn on while LEDs are already on or trying to turn off while LEDs are off.

### Example

Override the colour of the LEDs, and set them to a specific colour (which also prevents Taps being sent to XOS):

```
curl -X POST -H "Content-Type: application/json" -d '{
    "rgb_value": [4,99,7],
    "ramp_time": 2.5,
    "cross_fade": 1.0
}' "http://172.16.80.225:8082/api/lights/"
```

Return the LEDs to standard lens reader breathe animation.

```
curl -X POST -H "Content-Type: application/json" -d '{
    "rgb_value": [4,99,7],
    "ramp_time": 2.5,
    "cross_fade": 0.0
}' "http://172.16.80.225:8082/api/lights/"
```
