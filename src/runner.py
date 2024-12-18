#!/usr/bin/python

""" Kiosk IV tap reader running on a Raspberry Pi """

import json
import os
import re
import subprocess
from datetime import datetime
from queue import PriorityQueue
from threading import Thread, Timer
from time import sleep, time

import de2120_barcode_scanner
import requests
import sentry_sdk
import serial
from flask import Flask, request

from src.utils import IP_ADDRESS, IS_OSX, MAC_ADDRESS, TZ, env_to_tuple, log

try:
    import board

    import src.adafruit_dotstar as dotstar

except (NotImplementedError, ModuleNotFoundError):  # this doesn't compile install
    pass


# Constants defined in environment. Changes here should be documented in README.

# whether to use the DEBUG version of the idtech C code
DEBUG = os.getenv('DEBUG', 'false').lower() == 'true'
IS_LOCAL_ENV = os.getenv('IS_LOCAL_ENV', 'false') == 'true'
TARGET_API_ENDPOINT = os.getenv('TARGET_API_ENDPOINT', 'http://localhost:8000/api/')
TARGET_TAPS_ENDPOINT = os.getenv('TARGET_TAPS_ENDPOINT', f'{TARGET_API_ENDPOINT}taps/')
XOS_TAPS_ENDPOINT = os.getenv('XOS_TAPS_ENDPOINT', 'https://xos.acmi.net.au/api/taps/')
AUTH_TOKEN = os.getenv('AUTH_TOKEN', '')
XOS_LABEL_ID = os.getenv('XOS_LABEL_ID', '1')
SENTRY_ID = os.getenv('SENTRY_ID')

DEVICE_NAME = os.getenv('BALENA_DEVICE_NAME_AT_INIT', 'DD-00')
READER_MODEL = os.getenv('READER_MODEL', 'IDTech Kiosk IV')

TAP_OFF_TIMEOUT = float(os.getenv('TAP_OFF_TIMEOUT', '0.5'))  # seconds
LEDS_BRIGHTNESS = float(os.getenv('LEDS_BRIGHTNESS', '1.0'))
# seconds to cycle between IN and OUT colours.
LEDS_BREATHE_TIME = float(os.getenv('LEDS_BREATHE_TIME', '5.0'))
LEDS_BREATHE_COLOUR_OUT = env_to_tuple('LEDS_BREATHE_COLOUR_OUT', '36,26,10')  # Dim warm white
LEDS_BREATHE_COLOUR_IN = env_to_tuple('LEDS_BREATHE_COLOUR_IN', '95,70,26')  # Medium warm white
LEDS_SUCCESS_COLOUR = env_to_tuple('LEDS_SUCCESS_COLOUR', '255,238,202')  # Bright warm white
LEDS_FAILED_COLOUR = env_to_tuple('LEDS_FAILED_COLOUR', '149,8,106')  # Purple
LEDS_SUCCESS_TAP_COLOUR = env_to_tuple('LEDS_SUCCESS_COLOUR', '41,255,120')  # Blue
# up ramp_on, auto-hold (if needed), down ramp_on
LEDS_SIGNAL_TIMES = env_to_tuple('LEDS_SIGNAL_TIMES', '0.3,0.5,0.6')
LEDS_IN_STRING = int(os.getenv('LEDS_IN_STRING', '12'))  # Number of LEDs to light up
# Allow LED API POST to override Lens response
LEDS_CONTROL_OVERRIDE = os.getenv('LEDS_CONTROL_OVERRIDE', 'false').lower() == 'true'

TAP_SEND_RETRY_SECS = int(os.getenv('TAP_SEND_RETRY_SECS', '5'))

XOS_FAILED_RESPONSE_CODES = [400, 404]  # 400 Lens UID not found in XOS
TAP_SUCCESS_RESPONSE_CODES = [200, 201]  # 200 Maker Moments, 201 XOS

ONBOARDING_LEDS_API = os.getenv('ONBOARDING_LEDS_API')
ONBOARDING_LEDS_DATA_SUCCESS = os.getenv('ONBOARDING_LEDS_DATA_SUCCESS')
ONBOARDING_LEDS_DATA_FAILED = os.getenv('ONBOARDING_LEDS_DATA_FAILED')
ONBOARDING_LEDS_USERNAME = os.getenv('ONBOARDING_LEDS_USERNAME', '')
ONBOARDING_LEDS_PASSWORD = os.getenv('ONBOARDING_LEDS_PASSWORD', '')
# Ignore UIDs less than or equal to this length
UID_IGNORE_LENGTH = int(os.getenv('UID_IGNORE_LENGTH', '8'))

if IS_OSX:
    FOLDER = './bin/mac/'
else:
    FOLDER = './bin/arm_32/'

if DEBUG:
    CMD = ['./idtech_debug']  # this will run silently
else:
    CMD = ['./idtech']  # this will produce 2 beeps from the reader when run

# Setup Sentry
sentry_sdk.init(SENTRY_ID)

app = Flask(__name__)  # pylint: disable=invalid-name


class LEDControllerThread(Thread):
    # pylint: disable=too-many-instance-attributes
    """
    A thread that cycles the LED colours as a gaussian-ish 'breathe' state,
    until interrupted with a ramp_on in/ramp_on out to another colour.
    """

    @staticmethod
    def ease(current_time, beginning_value, value_change, duration):
        # Penner's easeInCubic
        current_time /= duration
        return value_change * (current_time ** 3) + beginning_value

    def __init__(self):
        super().__init__()
        self.current_colour = [0, 0, 0]  # the colour LEDs will be set to each fram
        self.breathe_colour = [0, 0, 0]  # the current colour of the breathing animation

        self.ramp_target_colour = [0, 0, 0]  # the target colour of the ramping animation
        self.ramp_target = 0.0
        self.ramp_duration = None
        self.ramp_time0 = None
        self.blocked_by = None

        self.onboarding_authentication_token = ''
        self.onboarding_authentication_expiry_time = 0
        self.send_onboarding_leds_sentry_exception = True

        if not IS_LOCAL_ENV:
            self.leds = dotstar.DotStar(
                board.SCK,
                board.MOSI,
                LEDS_IN_STRING,
                brightness=LEDS_BRIGHTNESS
            )
        else:
            self.leds = None

    def set_leds(self, colour):
        """
        If appropriate libraries are installed and working, give the LEDs a specified colour.
        Else print the colour they would get (for development).
        """
        colour = [min(max(int(i), 0), 255) for i in colour]
        if self.leds:
            self.leds.fill((*colour, LEDS_BRIGHTNESS))
        else:
            print(f'Setting LEDs to {list(colour)}')

    def ramp_on(self, target_colour, duration_s):
        """Set values to produce a fade to the ramp colour"""
        # print("RAMPING TO %s" % str(target_colour))
        self.ramp_target_colour = target_colour
        self.ramp_target = 1.0
        self.ramp_duration = duration_s
        self.ramp_time0 = time()

    def ramp_off(self, duration_s):
        """Set values to produce a fade out from the ramp colour"""
        # print("RAMPING OFF")
        self.ramp_duration = duration_s
        self.ramp_target = 0.0
        self.ramp_time0 = time()

    def _calculate_breathe_colour(self, time_):
        # pylint: disable=invalid-name
        # cubic approximation to gauss from
        # http://www.iquilezles.org/www/articles/functions/functions.htm
        x = time_ % LEDS_BREATHE_TIME
        # returns a ramp_on value between 0 (breathe out) and 1 (breathe in)
        w = LEDS_BREATHE_TIME / 2
        if x < w:
            x = (-x + w) / w
        else:
            x = (x - w) / w
        ramp_val = 1.0 - x * x * (3.0 - 2.0 * x)
        for i in range(3):
            self.breathe_colour[i] = LEDS_BREATHE_COLOUR_OUT[i] + ramp_val * \
                (LEDS_BREATHE_COLOUR_IN[i] - LEDS_BREATHE_COLOUR_OUT[i])

    def _calculate_ramp_proportion(self):
        """
        If we're ramping up or down, calculate where we are in that process
        (from 0 to 1 and back again, depending on the value of target_proprotion)
        """
        if self.ramp_time0 is not None:
            # we're ramping
            time_ = time() - self.ramp_time0
            if time_ >= self.ramp_duration:
                # we're done ramping
                self.ramp_time0 = None
                return self.ramp_target

            # we're not done ramping
            if self.ramp_target > 0.5:
                # we're ramping up
                return LEDControllerThread.ease(
                    time_, 0.0, self.ramp_target, self.ramp_duration
                )

            # we're ramping down
            return LEDControllerThread.ease(
                time_, 1.0, self.ramp_target - 1.0, self.ramp_duration
            )

        # we're not ramping
        return self.ramp_target

    def update_leds(self, start_time):
        """
        Updates the LEDs with a colour proportional to a target
        colour or colours in the breathe animation.
        """
        time_ = time() - start_time
        self._calculate_breathe_colour(time_)
        ramp_proportion = self._calculate_ramp_proportion()

        # mix the breathe and ramp_on colours according to the current ramp_on amount
        for i in range(3):
            self.current_colour[i] = ramp_proportion * self.ramp_target_colour[i] + \
                (1.0 - ramp_proportion) * self.breathe_colour[i]

        # output the colours
        self.set_leds(self.current_colour)

    def run(self):
        """
        Infinite loop to constantly update LEDs.
        """
        start_time = time()
        while True:
            self.update_leds(start_time)
            sleep(1.0 / 60)

    def success_on(self):
        """
        Ramp on animation for a successful Lens read.
        """
        self.ramp_on(LEDS_SUCCESS_COLOUR, LEDS_SIGNAL_TIMES[0])

    def success_off(self):
        """
        Ramp off animation for a successful Lens read.
        """
        self.ramp_off(LEDS_SIGNAL_TIMES[-1])

    def success(self):
        """
        Ramp on animation for a sucessful Tap response from XOS/Maker Moments.
        """
        # Turn on building LEDs
        if ONBOARDING_LEDS_API:
            Thread(target=self.success_onboarding_lights).start()

        # Turn on Lens Reader LEDs
        self.ramp_on(LEDS_SUCCESS_TAP_COLOUR, LEDS_SIGNAL_TIMES[0])
        sleep(LEDS_SIGNAL_TIMES[1])
        self.ramp_off(LEDS_SIGNAL_TIMES[-1])

    def failed(self):
        """
        Triggers a predefined set of colours to indicate a failed Tap response
        from XOS/Maker Moments.
        """
        # Turn on building LEDs
        if ONBOARDING_LEDS_API:
            Thread(target=self.failed_onboarding_lights).start()

        # Turn on Lens Reader LEDs
        for _ in range(3):
            self.ramp_on(LEDS_FAILED_COLOUR, 0.1)
            sleep(LEDS_SIGNAL_TIMES[-1])
            self.ramp_on(LEDS_BREATHE_COLOUR_IN, 0.1)
            sleep(LEDS_SIGNAL_TIMES[-1])

        self.ramp_off(LEDS_SIGNAL_TIMES[-1])

    def toggle_lights(self, rgb_value, ramp_time, cross_fade):
        self.ramp_target_colour = rgb_value
        self.ramp_target = cross_fade
        self.ramp_duration = ramp_time
        self.ramp_time0 = time()

    def success_onboarding_lights(self):
        """
        Set the onboarding lights to success.
        """
        self.post_to_onboarding_lights(ONBOARDING_LEDS_DATA_SUCCESS)

    def failed_onboarding_lights(self):
        """
        Set the onboarding lights to failed.
        """
        self.post_to_onboarding_lights(ONBOARDING_LEDS_DATA_FAILED)

    def post_to_onboarding_lights(self, data):
        """
        If ONBOARDING_LEDS_API is set, send an API call to turn on the onboarding lights
        with the data handed in.
        """
        if ONBOARDING_LEDS_API and data:
            try:
                data = json.loads(data)
                response = requests.post(
                    url=f'{ONBOARDING_LEDS_API}api/trigger',
                    json=data,
                    timeout=5,
                    headers={
                        'Authorization': f'Bearer {self.onboarding_authentication_token}'
                    }
                )
                response.raise_for_status()
                log(
                    f'Sent onboarding LED request {ONBOARDING_LEDS_API}api/trigger {data}, '
                    f'response: {response.status_code}'
                )
            except json.decoder.JSONDecodeError as exception:
                log(f'Failed to parse JSON from string {data}\n{str(exception)}')
                sentry_sdk.capture_exception(exception)
            except (
                requests.exceptions.ConnectionError,
                requests.exceptions.Timeout
            ) as connection_error:
                log(
                    f'Failed to post onboarding lights to '
                    f'{ONBOARDING_LEDS_API}api/trigger: {data}\n'
                    f'{str(connection_error)}'
                )
                sentry_sdk.capture_exception(connection_error)
            except requests.HTTPError as exception:
                log(
                    f'Failed to trigger onboarding lights '
                    f'{ONBOARDING_LEDS_API}api/trigger '
                    f'with error {str(exception)}'
                )
                sentry_sdk.capture_exception(exception)

    def onboarding_authentication_daemon(self):
        """
        If ONBOARDING_LEDS_TOKEN_API is set, start a daemon to update
        authentication token before it expires
        """
        if ONBOARDING_LEDS_API:
            while True:
                self.update_onboarding_authentication()
                sleep(30)

    def update_onboarding_authentication(self):
        """
        Update onbording lens token if it is about to expire
        """
        now = time()
        time_till_expiry = self.onboarding_authentication_expiry_time - now

        # less then 45 seconds to expiry
        if time_till_expiry < 45:
            authentication_creds = {
                "user": ONBOARDING_LEDS_USERNAME,
                "password": ONBOARDING_LEDS_PASSWORD
            }
            try:
                response = requests.post(
                    url=f'{ONBOARDING_LEDS_API}token',
                    json=authentication_creds,
                    timeout=5,
                )
                response.raise_for_status()
                authentication_information = response.json()
                expires_in = authentication_information['expires_in']
                access_token = authentication_information['access_token']
                self.onboarding_authentication_expiry_time = time() + expires_in
                self.onboarding_authentication_token = access_token
            except (
                requests.exceptions.ConnectionError,
                requests.exceptions.Timeout,
                requests.HTTPError,
                KeyError
            ) as connection_error:
                log(
                    f'Failed to get authenticationToken from {ONBOARDING_LEDS_API}token\n'
                    f'{str(connection_error)}'
                )
                if self.send_onboarding_leds_sentry_exception:
                    sentry_sdk.capture_exception(connection_error)
                    self.send_onboarding_leds_sentry_exception = False


class TapManager:  # pylint: disable=too-many-instance-attributes
    """
    TapManager processes continuous messages from the idtech C code,
    in order to detect tap on and tap off events.

    Tap On events send a message to a nominated XOS address,
    and light up the reader's LEDs.

    Tap Off events are currently only logged.
    """

    def __init__(self):
        # Tap init
        self.last_id = None
        self.last_id_time = time()
        self.last_id_failed = None
        self.tap_off_timer = None
        self.leds = LEDControllerThread()
        self.queue = PriorityQueue()
        self.post_to_sentry = True
        self.barcode_scanner = None

    def create_tap(self, lens_id):
        """
        Create a tap from a lens ID
        """
        return {
            'lens': {
                'uid': lens_id
            },
            'tap_datetime': datetime.now(TZ).isoformat(),
            'label': XOS_LABEL_ID,
            'data': {
                'lens_reader': {
                    'device_name': DEVICE_NAME,
                    'mac_address': MAC_ADDRESS,
                    'reader_ip': IP_ADDRESS,
                    'reader_model': READER_MODEL,
                }
            }
        }

    def send_tap_or_requeue(self):
        """
        Get the topmost tap from the queue and try to send it.
        If XOS is unreachable, put the tap back in the queue.
        """
        try:
            _, tap, endpoint, api_key = self.queue.get()
            headers = {'Authorization': 'Token ' + api_key}
        except TypeError as exception:
            log(f'Failed to get tap from queue: {exception}')
            if self.post_to_sentry:
                sentry_sdk.capture_exception(exception)
                self.post_to_sentry = False
            log(f'Waiting for {TAP_SEND_RETRY_SECS} seconds to retry')
            sleep(TAP_SEND_RETRY_SECS)
            return 1

        try:
            response = requests.post(url=endpoint, json=tap, headers=headers, timeout=5)
            if response.status_code in TAP_SUCCESS_RESPONSE_CODES:
                try:
                    data = response.json()
                    log(
                        f'XOS Tap created: {data["id"]}, Lens: {data["lens_short_code"]}, '
                        f'Collectible: {data["collectible"]["id"]}'
                    )
                except (json.decoder.JSONDecodeError, KeyError):
                    log(response.text)
                self.last_id_failed = False

                if not self.leds.blocked_by and ONBOARDING_LEDS_API:
                    self.leds.success()
                    self.last_id_failed = None
                self.post_to_sentry = True
                return 0

            if response.status_code in XOS_FAILED_RESPONSE_CODES:
                log(response.text)
                self.last_id_failed = True
                # XOS returned a failed tap after the visitor tapped off,
                # so show the failed LED state asynchronously.
                # Possible UX problem: the visitor walks away before the LEDs
                # show the failed state.
                self.failed_led_response()
                return 1

            log(response.text)
            self.last_id_failed = True
            # XOS returned an failed tap response not in the XOS_FAILED_RESPONSE_CODES
            # after the visitor tapped off, so show the failed LED state asynchronously.
            # Possible UX problem: the visitor walks away before the LEDs
            self.failed_led_response()
            if self.post_to_sentry:
                sentry_sdk.capture_message(response.text)
                self.post_to_sentry = False
            self.queue.put((tap['tap_datetime'], tap, endpoint, api_key))
            log(f'Waiting for {TAP_SEND_RETRY_SECS} seconds to retry')
            sleep(TAP_SEND_RETRY_SECS)
            return 1

        except (
            requests.exceptions.ConnectionError,
            requests.exceptions.ReadTimeout,
            requests.exceptions.Timeout,
        ) as connection_error:
            log(f'Failed to post tap message to {endpoint}: {tap}\n{str(connection_error)}')
            self.failed_led_response()
            if self.post_to_sentry:
                sentry_sdk.capture_exception(connection_error)
                self.post_to_sentry = False
            self.queue.put((tap['tap_datetime'], tap, endpoint, api_key))
            log(f'Waiting for {TAP_SEND_RETRY_SECS} seconds to retry')
            sleep(TAP_SEND_RETRY_SECS)
            return 1

    def failed_led_response(self):
        """
        In response to a failed Tap, set the LEDs if they're not blocked.
        """
        if not self.leds.blocked_by:
            self.leds.blocked_by = 'xos'
            self.leds.failed()
            self.leds.blocked_by = None
            self.last_id_failed = None

    def send_taps(self):
        """
        Continuously try to send the taps
        """
        while True:
            self.send_tap_or_requeue()

    def tap_on(self):
        log(' Tap On: ', self.last_id)
        # turn leds on only if not being used
        if not self.leds.blocked_by:
            tap = self.create_tap(self.last_id)
            self.queue.put((tap['tap_datetime'], tap, TARGET_TAPS_ENDPOINT, AUTH_TOKEN))
            if not ONBOARDING_LEDS_API:
                self.leds.blocked_by = 'tap'
            self.leds.success_on()
        else:
            log('Tap blocked by: ', self.leds.blocked_by)

    def tap_off(self):
        log('Tap Off: ', self.last_id)
        if self.leds.blocked_by == 'tap':
            # if blocked by a tap, turn LEDs off
            self.leds.success_off()
            if self.last_id_failed:
                # XOS has returned a failed tap before the visitor tapped off,
                # so show the failed LED state now.
                self.leds.failed()
            elif self.last_id_failed is not None and ONBOARDING_LEDS_API:
                # XOS has returned a success tap before the visitor tapped off,
                # so show the success LED state now.
                self.leds.success()
            self.last_id = None
            self.last_id_failed = None
            self.leds.blocked_by = None
        elif self.leds.blocked_by in ('remote', 'xos'):
            # if blocked by another LED command, leave LEDs alone, and only reset
            # last lens id so we can still receive new tap on events
            self.last_id = None
        elif ONBOARDING_LEDS_API:
            self.last_id = None

    def _reset_tap_off_timer(self):
        # reset the tap-off timer for this ID
        if self.tap_off_timer:
            self.tap_off_timer.cancel()
            del self.tap_off_timer
        self.tap_off_timer = Timer(TAP_OFF_TIMEOUT, self.tap_off)
        self.tap_off_timer.start()

    def _byte_string_to_lens_id(self, byte_string):
        """
        Convert an id of the form 04:04:A5:2C:F2:2A:5E:80 (as output from the C app)
        to 04a52cf22a5e80 by:
            - stripping ':' and whitespace
            - removing first byte
            - lowercasing
        """
        return ''.join(byte_string.strip().split(':')[1:]).lower()

    def read_line(self, line):
        """
        This is called continuously while an NFC tag is present.
        """
        lens_id = ''
        barcode_reader = 'de2120' in READER_MODEL.lower()
        if barcode_reader:
            lens_id = line.replace('https://lens.acmi.net.au/', '')
        else:
            lens_id = self._byte_string_to_lens_id(line)

        if len(lens_id) > UID_IGNORE_LENGTH or barcode_reader:
            if lens_id != self.last_id:
                # send a tap-off message if needed
                if self.tap_off_timer and self.tap_off_timer.is_alive():
                    self.tap_off_timer.cancel()
                    self.tap_off()
                self.last_id = lens_id
                self.tap_on()
            self._reset_tap_off_timer()
        else:
            log(f'Ignoring {len(lens_id)} character UID: {lens_id}')

    def process_taps(self):
        """
        Read the lines from the C interface or barcode reader and processes taps.
        """
        if 'de2120' in READER_MODEL.lower():  # pylint: disable=too-many-nested-blocks
            scan_buffer = None
            self.turn_on_barcode_scanner()
            reported = False
            while True:
                try:
                    if self.barcode_scanner:
                        scan_buffer = self.barcode_scanner.read_barcode()
                        if scan_buffer:
                            barcode = str(scan_buffer).replace('\r', '').replace('\x06', '')
                            if barcode:
                                log(f'Code found: {barcode}')
                                barcode = self.fix_double_barcode_scan(barcode)
                                self.read_line(barcode)
                                scan_buffer = None
                    sleep(0.02)
                except (
                    OSError,
                    serial.serialutil.SerialException,
                    UnicodeDecodeError,
                ) as exception:
                    log(f'ERROR: {READER_MODEL} reading barcode - {exception}')
                    if not reported:
                        sentry_sdk.capture_exception(exception)
                        reported = True
        else:
            log(f'{READER_MODEL} connected...')
            shell = True
            with subprocess.Popen(CMD, cwd=FOLDER, shell=shell, stdout=subprocess.PIPE) as popen:
                byte_string_re = r'([0-9a-fA-F]{2}:?)+'

                while True:
                    # wait for the next line from the C interface
                    # (if there are no NFCs there will be no lines)
                    line = popen.stdout.readline().decode('utf-8')

                    # see if it is a tag read
                    if re.match(byte_string_re, line):
                        # We have an ID.
                        self.read_line(line)

    def turn_on_barcode_scanner(self):  # pylint: disable=too-many-statements
        """
        Turn on the Sparkfun DE2120 barcode scanner.
        """
        if 'de2120' in READER_MODEL.lower():
            try:
                if not self.barcode_scanner:
                    self.barcode_scanner = de2120_barcode_scanner.DE2120BarcodeScanner()
                try:
                    self.barcode_scanner.USB_mode('VIC')
                    sleep(1.0)
                except (serial.serialutil.SerialException, TypeError) as exception:
                    log(f'ERROR: {READER_MODEL} failed setting USB mode with: {exception}')
                    sleep(1.0)
                try:
                    self.barcode_scanner.enable_motion_sense()
                    sleep(1.0)
                except (serial.serialutil.SerialException, TypeError) as exception:
                    log(f'ERROR: {READER_MODEL} failed setting motion mode with: {exception}')
                    sleep(1.0)
                try:
                    self.barcode_scanner.enable_continuous_read(3)
                    sleep(1.0)
                except (serial.serialutil.SerialException, TypeError) as exception:
                    log(f'ERROR: {READER_MODEL} failed setting continuous read: {exception}')
                    sleep(1.0)
                try:
                    self.barcode_scanner.light_on()
                    sleep(1.0)
                except (serial.serialutil.SerialException, TypeError) as exception:
                    log(f'ERROR: {READER_MODEL} failed setting light on: {exception}')
                    sleep(1.0)
                try:
                    self.barcode_scanner.reticle_on()
                    sleep(1.0)
                except (serial.serialutil.SerialException, TypeError) as exception:
                    log(f'ERROR: {READER_MODEL} failed setting scan line on: {exception}')
                    sleep(1.0)
                try:
                    self.barcode_scanner.enable_decode_beep()
                    sleep(1.0)
                except (serial.serialutil.SerialException, TypeError) as exception:
                    log(f'ERROR: {READER_MODEL} failed setting beep on: {exception}')
                    sleep(1.0)
                try:
                    if not self.barcode_scanner.begin():
                        log(f"ERROR: {READER_MODEL} isn't connected...")
                        return
                    log(f'{READER_MODEL} connected...')
                except (serial.serialutil.SerialException, TypeError) as exception:
                    log(f"ERROR: {READER_MODEL} failed begin() with: {exception}")
                    sleep(1)

            except (OSError, serial.serialutil.SerialException) as exception:
                log(f'ERROR: {READER_MODEL} turning on - {exception}')
                sentry_sdk.capture_exception(exception)

    def turn_off_barcode_scanner(self):
        """
        Turn off the Sparkfun DE2120 barcode scanner.
        """
        if 'de2120' in READER_MODEL.lower():
            try:
                if not self.barcode_scanner:
                    self.barcode_scanner = de2120_barcode_scanner.DE2120BarcodeScanner()
                try:
                    self.barcode_scanner.enable_manual_trigger()
                    sleep(1.0)
                except (serial.serialutil.SerialException, TypeError) as exception:
                    log(f'ERROR: {READER_MODEL} failed setting manual scan mode with: {exception}')
                    sleep(1.0)
                try:
                    self.barcode_scanner.light_off()
                    sleep(1.0)
                except (serial.serialutil.SerialException, TypeError) as exception:
                    log(f'ERROR: {READER_MODEL} failed setting light off: {exception}')
                    sleep(1.0)
                try:
                    self.barcode_scanner.reticle_off()
                except (serial.serialutil.SerialException, TypeError) as exception:
                    log(f'ERROR: {READER_MODEL} failed setting scan line off: {exception}')
                    sleep(1.0)
                try:
                    self.barcode_scanner.disable_decode_beep()
                    sleep(1.0)
                except (serial.serialutil.SerialException, TypeError) as exception:
                    log(f'ERROR: {READER_MODEL} failed setting beep on: {exception}')
                    sleep(1.0)

            except (OSError, serial.serialutil.SerialException) as exception:
                log(f'ERROR: {READER_MODEL} turning off - {exception}')
                sentry_sdk.capture_exception(exception)

    def turn_on_barcode_beep(self):
        """
        Turn on the Sparkfun DE2120 barcode beep.
        """
        if 'de2120' in READER_MODEL.lower():
            try:
                if not self.barcode_scanner:
                    self.barcode_scanner = de2120_barcode_scanner.DE2120BarcodeScanner()
                try:
                    self.barcode_scanner.enable_decode_beep()
                except (serial.serialutil.SerialException, TypeError) as exception:
                    log(f'ERROR: {READER_MODEL} failed setting beep on: {exception}')

            except (OSError, serial.serialutil.SerialException) as exception:
                log(f'ERROR: {READER_MODEL} turning beep off - {exception}')
                sentry_sdk.capture_exception(exception)

    def turn_off_barcode_beep(self):
        """
        Turn off the Sparkfun DE2120 barcode beep.
        """
        if 'de2120' in READER_MODEL.lower():
            try:
                if not self.barcode_scanner:
                    self.barcode_scanner = de2120_barcode_scanner.DE2120BarcodeScanner()
                try:
                    self.barcode_scanner.disable_decode_beep()
                except (serial.serialutil.SerialException, TypeError) as exception:
                    log(f'ERROR: {READER_MODEL} failed setting beep off: {exception}')

            except (OSError, serial.serialutil.SerialException) as exception:
                log(f'ERROR: {READER_MODEL} turning beep off - {exception}')
                sentry_sdk.capture_exception(exception)

    def fix_double_barcode_scan(self, barcode):
        """
        Return a single barcode if the reader scans it twice.
        """
        if barcode and len(barcode) % 20 == 0:
            barcode = barcode[0:20]
            log(f'Code updated: {barcode}')
            return barcode
        return barcode


@app.route('/api/taps/', methods=['POST'])
def taps_endpoint():
    """
    Endpoint to accept Lens Taps which are then forwarded on to XOS via the tap queue.

    :param request_data: Tap data as JSON.
    :type request_data: json
    :return: Success is set True unless the request_data is invalid
    """
    try:
        request_data = dict(request.get_json())
        api_key = request.headers.get('Authorization')
        assert request_data['lens']['uid']
        assert api_key
        tap_manager.queue.put((
            request_data['tap_datetime'],
            request_data,
            XOS_TAPS_ENDPOINT,
            api_key.replace('Token ', ''),
        ))
        return json.dumps({'success': True}), 201
    except KeyError:
        return json.dumps(
            {'success': False, 'error': 'Did you include a Lens UID and tap datetime?'},
        ), 400
    except AssertionError:
        return json.dumps(
            {'success': False, 'error': 'Did you include an API key?'},
        ), 401


@app.route('/api/lights/', methods=['POST'])
def toggle_lights():
    """
    Endpoint to turn on the lights with a given RGB value and for a given time.

    :param rgb_value: A list of three integers (0-255) making up an RGB combination.
    :type rgb_value: list
    :param ramp_time: A float that indicates the ramp on/off time in seconds.
    :type ramp_time: float
    :param cross_fade: A float from 0 to 1 that indicates the ramp target value.
    :type cross_fade: float
    :return: Success or error message

    """
    request_data = dict(request.get_json())
    try:
        rgb_value = request_data['rgb_value']
        ramp_time = float(request_data['ramp_time'])
        cross_fade = float(request_data['cross_fade'])

        if LEDS_CONTROL_OVERRIDE and tap_manager.leds.blocked_by == 'tap' and cross_fade > 0.0:
            # If a Maker Moment needs to control LEDs while a Lens is sitting on the Lens Reader
            # the Lens Reader must have LEDS_CONTROL_OVERRIDE set true to enable
            # the reader to force tap-off, and control the LEDs
            tap_manager.tap_off()
            tap_manager.turn_on_barcode_beep()
        else:
            # Normal behaviour, don't interrupt a Lens tap/response, so:
            # ensure we are turning off a previous remote toggle
            # or turning on if nothing is using the leds
            assert (tap_manager.leds.blocked_by == 'remote' and cross_fade == 0.0) or \
                (not tap_manager.leds.blocked_by and cross_fade > 0.0)

        if cross_fade > 0.0:
            # block taps and LED events
            tap_manager.leds.blocked_by = 'remote'
            tap_manager.turn_off_barcode_beep()
        else:
            # reset ready for more taps
            tap_manager.last_id = None
            tap_manager.leds.success_off()
            tap_manager.leds.blocked_by = None
            tap_manager.turn_on_barcode_beep()

        tap_manager.leds.toggle_lights(rgb_value, ramp_time, cross_fade)
        return 'Leds toggled successfully.', 200

    except (SyntaxError, TypeError, ValueError):
        response = {'error': 'Invalid rgb_value, ramp_time or cross_fade value.'}
        return json.dumps(response), 400
    except AssertionError:
        response = {'error': 'Cannot perform action.'}
        return json.dumps(response), 409


def byte_string_to_lens_id(byte_string):
    """
    Convert 04:04:A5:2C:F2:2A:5E:80 to 04a52cf22a5e80 by:
    - stripping ':' and whitespace
    - removing first byte
    - lowercasing
    """
    return ''.join(byte_string.strip().split(':')[1:]).lower()


tap_manager = TapManager()  # pylint: disable=invalid-name


def main():
    """Launcher."""
    print(f'XOS Lens Reader ({READER_MODEL})')

    if ONBOARDING_LEDS_API:
        print('----------------------')
        print(f'Onboarding LEDs API: {ONBOARDING_LEDS_API}')
        print(f'Onboarding LEDs Data Success: {ONBOARDING_LEDS_DATA_SUCCESS}')
        print(f'Onboarding LEDs Data Failed: {ONBOARDING_LEDS_DATA_FAILED}')
        print('----------------------')

    if not TARGET_TAPS_ENDPOINT == XOS_TAPS_ENDPOINT:
        print('----------------------')
        print(f'Target Taps endpoint: {TARGET_TAPS_ENDPOINT}')
        print(f'Target XOS endpoint: {XOS_TAPS_ENDPOINT}')
        print('----------------------')

    tap_manager.leds.start()

    # Start thread to read and queue taps.
    Thread(target=tap_manager.process_taps).start()

    # Start thread to update authentication
    Thread(target=tap_manager.leds.onboarding_authentication_daemon).start()

    # Start thread to send taps.
    Thread(target=tap_manager.send_taps).start()

    # Start a Flask server to expose /api/lights/ endpoint
    app.run(host='0.0.0.0', port=8082)


if __name__ == "__main__":
    main()
