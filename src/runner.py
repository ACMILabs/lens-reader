#!/usr/bin/python

""" Kiosk IV tap reader running on a Raspberry Pi """

import math
import os
import re
import subprocess
import sys
from datetime import datetime
from threading import Timer, Thread, Event
from time import sleep, time
from copy import deepcopy

import requests
import sentry_sdk
from utils import IP_ADDRESS, IS_OSX, MAC_ADDRESS, TZ, envtotuple, log

try:
  import adafruit_dotstar as dotstar
except ModuleNotFoundError: # this doesn't compile install
  dotstar = None

try:
  import board
except (NotImplementedError, ModuleNotFoundError):
  board = None


# Constants defined in environment. Changes here should be documented in README.
DEBUG = os.getenv('DEBUG', 'false').lower() == "true" # whether to use the DEBUG version of the idtech C code
TARGET_API_ENDPOINT = os.getenv('TARGET_API_ENDPOINT', 'http://localhost:8000/api/')
TARGET_TAPS_ENDPOINT = os.getenv('TARGET_TAPS_ENDPOINT', f'{TARGET_API_ENDPOINT}taps/')
AUTH_TOKEN = os.getenv('AUTH_TOKEN', '')
XOS_LABEL_ID = os.getenv('XOS_LABEL_ID', '1')
SENTRY_ID = os.getenv('SENTRY_ID')

DEVICE_NAME = os.getenv('DEVICE_NAME')
READER_MODEL = os.getenv('READER_MODEL')
READER_NAME = DEVICE_NAME or 'nfc-' + IP_ADDRESS.split('.')[-1] # not user-settable

TAP_OFF_TIMEOUT = float(os.getenv('TAP_OFF_TIMEOUT', '0.5')) # seconds
LEDS_BRIGHTNESS = float(os.getenv('LEDS_BRIGHTNESS', '1.0'))
LEDS_BREATHE_TIME = float(os.getenv('LEDS_BREATHE_TIME', '5.0')) # seconds to cycle between IN and OUT colours.
LEDS_BREATHE_COLOUR_OUT = envtotuple('LEDS_BREATHE_COLOUR_OUT', "36,26,10")  # Dim warm white
LEDS_BREATHE_COLOUR_IN = envtotuple('LEDS_BREATHE_COLOUR_IN',  "95,70,26")  # Medium warm white
LEDS_SUCCESS_COLOUR = envtotuple('LEDS_SUCCESS_COLOUR', "255,238,202")  # Bright warm white
LEDS_FAILED_COLOUR = envtotuple('LEDS_FAILED_COLOUR', "137,0,34")  # Medium red
LEDS_SIGNAL_TIMES = envtotuple('LEDS_SIGNAL_TIMES', "0.3,0.5,0.6") # up ramp_on, auto-hold (if needed), down ramp_on
LEDS_IN_STRING = int(os.getenv('LEDS_IN_STRING', '12')) # Number of LEDs to light up

if IS_OSX:
  FOLDER = "../bin/mac/"
else:
  FOLDER = "../bin/arm_32/"

if DEBUG:
  CMD = ["./idtech_debug"] # this will run silently
else:
  CMD = ["./idtech"] # this will produce 2 beeps from the reader when run

# Setup Sentry
sentry_sdk.init(SENTRY_ID)


class LEDControllerThread(Thread):
  """
  A thread that cycles the LED colours as a gaussian-ish 'breathe' state, until interrupted with a ramp_on in/ramp_on out to another colour.
  """

  @staticmethod
  def ease(t, b, c, d):
    # Penner's easeInCubic
    # t: current time, b: beginning value, c: change in value, d: duration
    t = t / d
    r = c * t * t * t + b
    return r

  def __init__(self):
    super(LEDControllerThread, self).__init__()
    self.current_colour = [0,0,0] # the colour LEDs will be set to each fram
    self.breathe_colour = [0,0,0] # the current colour of the breathing animation

    self.ramp_target_colour = [0,0,0] # the target colour of the ramping animation
    self.ramp_target = 0.0
    self.ramp_duration = None
    self.ramp_time0 = None

    if board and dotstar:
      self.LEDS = dotstar.DotStar(board.SCK, board.MOSI, LEDS_IN_STRING, brightness=LEDS_BRIGHTNESS)
    else:
      self.LEDS = None

  def set_leds(self, colour):
    colour = [min(max(int(i), 0),255) for i in colour]
    if self.LEDS:
      self.LEDS.fill((*colour, LEDS_BRIGHTNESS))
    else:
      print("Setting LEDs to %s" % list(colour))

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

  def _calculate_breathe_colour(self, t):
    # cubic approximation to gauss from http://www.iquilezles.org/www/articles/functions/functions.htm
    x = t % LEDS_BREATHE_TIME
    # returns a ramp_on value between 0 (breathe out) and 1 (breathe in)
    w = LEDS_BREATHE_TIME / 2
    if (x<w):
      x = (-x + w) / w
    else:
      x = (x - w) / w
    ramp_val = 1.0 - x * x * (3.0 - 2.0 * x)
    for i in range(3):
      self.breathe_colour[i] = LEDS_BREATHE_COLOUR_OUT[i] + ramp_val * (LEDS_BREATHE_COLOUR_IN[i] - LEDS_BREATHE_COLOUR_OUT[i])

  def _calculate_ramp_proportion(self):
    """
    If we're ramping up or down, calculate where we are in that process (from 0 to 1 and back again, depending on the value of target_proprotion)
    """
    if self.ramp_time0 is not None:
      # we're ramping
      t = time() - self.ramp_time0
      if t >= self.ramp_duration:
        # we're done ramping
        self.ramp_time0 = None
        return self.ramp_target
      else:
        # we're not done ramping
        if self.ramp_target > 0.5:
          # we're ramping up
          return LEDControllerThread.ease(t, 0.0, self.ramp_target, self.ramp_duration)
        else:
          # we're ramping down
          return LEDControllerThread.ease(t, 1.0, self.ramp_target-1.0, self.ramp_duration)
    else:
      # we're not ramping
      return self.ramp_target

  def run(self):
    t0 = time()
    while True:
      t = time() - t0
      self._calculate_breathe_colour(t)
      ramp_proportion = self._calculate_ramp_proportion()

      # mix the breathe and ramp_on colours according to the current ramp_on amount
      for i in range(3):
        self.current_colour[i] = ramp_proportion * self.ramp_target_colour[i] + (1.0 - ramp_proportion) * self.breathe_colour[i]

      # output the colours
      self.set_leds(self.current_colour)
      sleep(1.0/60)

  def success_on(self):
    self.ramp_on(LEDS_SUCCESS_COLOUR, LEDS_SIGNAL_TIMES[0])

  def success_off(self):
    self.ramp_off(LEDS_SIGNAL_TIMES[-1])

  def failed(self):
    self.ramp_on(LEDS_FAILED_COLOUR, LEDS_SIGNAL_TIMES[0])
    sleep(LEDS_SIGNAL_TIMES[1])
    self.ramp_off(LEDS_SIGNAL_TIMES[-1])


class TapManager:
  """
  TapManager processes continuous messages from the idtech C code, in order to detect tap on and tap off events.
  Tap On events send a message to a nominated XOS address, and light up the reader's LEDs.
  Tap Off events are currently only logged.
  """
  def __init__(self):
    # Tap init
    self.last_id = None
    self.last_id_time = time()
    self.tap_off_timer = None
    self.leds = LEDControllerThread()
    self.leds.start()

  def send_tap(self, id):
    """refer to docs.python-requests.org for implementation examples"""
    params={
      'lens': {
        'uid':id
      },
      'tap_datetime': datetime.now(TZ).isoformat(),
      'label': XOS_LABEL_ID,
      'data': {
        'lens_reader': {
          'mac_address': MAC_ADDRESS,
          'reader_ip': IP_ADDRESS,
          'reader_model': READER_MODEL,
          'reader_name': READER_NAME,
        }
      }
    }
    headers={'Authorization': 'Token ' + AUTH_TOKEN}
    try:
      r=requests.post(url=TARGET_TAPS_ENDPOINT, json=params, headers=headers)
      if r.status_code==201:
        log(r.text)
        return(0)
      elif r.status_code==404:
        log(r.text)
        self.leds.failed()
        return(1)
      else:
        log(r.text)
        sentry_sdk.capture_message(r.text)
        return(1)
    except requests.exceptions.ConnectionError as e:
      log("Failed to post tap message to %s: %s\n%s" % (TARGET_TAPS_ENDPOINT, params, str(e)))
      sentry_sdk.capture_exception(e)
      self.leds.failed()
      return(1)

  def tap_on(self):
    log(" Tap On: ", self.last_id)
    self.leds.success_on()
    self.send_tap(self.last_id)

  def tap_off(self):
    log("Tap Off: ", self.last_id)
    self.last_id = None
    self.leds.success_off()

  def _reset_tap_off_timer(self):
    # reset the tap-off timer for this ID
    if self.tap_off_timer:
      self.tap_off_timer.cancel()
      del self.tap_off_timer
    self.tap_off_timer = Timer(TAP_OFF_TIMEOUT, self.tap_off)
    self.tap_off_timer.start()

  def _byte_string_to_lens_id(self, byte_string):
    """
    Convert an id of the form 04:04:A5:2C:F2:2A:5E:80 (as output from the C app) to 04a52cf22a5e80 by:
    - stripping ":" and whitespace
    - removing first byte
    - lowercasing
    """
    return "".join(byte_string.strip().split(":")[1:]).lower()

  def read_line(self, line):
    """
    This is called continuously while an NFC tag is present.
    """
    id = self._byte_string_to_lens_id(line)
    if id != self.last_id:
      # send a tap-off message if needed
      if self.tap_off_timer and self.tap_off_timer.is_alive():
        self.tap_off_timer.cancel()
        self.tap_off()
      self.last_id = id
      self.tap_on()
    self._reset_tap_off_timer()


def byte_string_to_lens_id(byte_string):
  """
  Convert 04:04:A5:2C:F2:2A:5E:80 to 04a52cf22a5e80 by:
  - stripping ":" and whitespace
  - removing first byte
  - lowercasing
  """
  return "".join(byte_string.strip().split(":")[1:]).lower()


def main():
  """Launcher."""
  print("XOS Lens Reader (KioskIV)")

  shell = True
  popen = subprocess.Popen(CMD, cwd=FOLDER, shell=shell, stdout=subprocess.PIPE)

  tap_manager = TapManager()
  BYTE_STRING_RE = r'([0-9a-fA-F]{2}:?)+'

  while True:
    # wait for the next line from the C interface (if there are no NFCs there will be no lines)
    line = popen.stdout.readline().decode("utf-8")

    # see if it is a tag read
    if re.match(BYTE_STRING_RE, line):
      # We have an ID.
      tap_manager.read_line(line)

if __name__ == "__main__":
  main()
