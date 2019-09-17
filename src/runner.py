#!/usr/bin/python

""" Kiosk IV tap reader running on a Raspberry Pi """

import os
import re
import subprocess
import sys
from datetime import datetime
from threading import Timer
from time import sleep, time

import adafruit_dotstar as dotstar
import requests
import sentry_sdk

try:
  import board
except NotImplementedError:
  board = None

from utils import log, MAC_ADDRESS, IP_ADDRESS, TZ, IS_OSX

# Constants defined in environment
DEBUG = os.getenv('DEBUG', '').lower() == "true"
XOS_URL = os.getenv('XOS_URL', 'http://localhost:8888')
AUTH_TOKEN = os.getenv('AUTH_TOKEN', '')
LABEL = os.getenv('LABEL')
SENTRY_ID = os.getenv('SENTRY_ID')
TAP_OFF_TIMEOUT = float(os.getenv('TAP_OFF_TIMEOUT', '0.5')) # seconds
DEVICE_NAME = os.getenv('DEVICE_NAME')
READER_MODEL = os.getenv('READER_MODEL')
READER_NAME = DEVICE_NAME or 'nfc-' + IP_ADDRESS.split('.')[-1]


LEDS_COLOUR_DEFAULT = (254, 254, 254)  # White
LEDS_COLOUR_SUCCESS = (0, 254, 0)  # Green
LEDS_COLOUR_FAILED = (254, 0, 0)  # Red
LEDS_DEFAULT_BRIGHTNESS = 0.1
LEDS_MAX_BRIGHTNESS = 0.5
LEDS_FADE_BRIGHTNESS_STEP = 0.01
LEDS_FADE_TIME_BETWEEN_STEP = 0.005
LEDS_TIME_BETWEEN_FAILED_BLINKS = 0.3

BYTE_STRING_RE = r'([0-9a-fA-F]{2}:?)+'


if IS_OSX:
  FOLDER = "../bin/mac/"
else:
  FOLDER = "../bin/arm_32/"

if DEBUG:
  CMD = ["./idtech_debug"]
else:
  CMD = ["./idtech"]

# Setup Sentry
sentry_sdk.init(SENTRY_ID)


class LedsManager:
  """
  LedsManager manages the state of the reader's LEDs.
  """
  def __init__(self):
    # LED init
    # Using a DotStar Digital LED Strip with 12 LEDs connected to hardware SPI
    if board:
      self.LEDS = dotstar.DotStar(board.SCK, board.MOSI, 12, brightness=0.1)
      self.fill_default()
    else:
      self.LEDS = None

  def fill_default(self):
    if self.LEDS:
      self.LEDS.fill((*LEDS_COLOUR_DEFAULT, LEDS_DEFAULT_BRIGHTNESS))

  def success(self):
    if self.LEDS:
      current_brightness = LEDS_MAX_BRIGHTNESS
      while current_brightness > 0.0:
        current_brightness -= LEDS_FADE_BRIGHTNESS_STEP
        self.LEDS.fill((*LEDS_COLOUR_SUCCESS, current_brightness))
        sleep(LEDS_FADE_TIME_BETWEEN_STEP)
      self.fill_default()

  def failed(self):
    if self.LEDS:
      current_brightness = LEDS_MAX_BRIGHTNESS

      self.LEDS.fill((*LEDS_COLOUR_FAILED, current_brightness))
      sleep(LEDS_TIME_BETWEEN_FAILED_BLINKS)

      self.LEDS.fill((*LEDS_COLOUR_FAILED, 0.0))
      sleep(LEDS_TIME_BETWEEN_FAILED_BLINKS)

      self.LEDS.fill((*LEDS_COLOUR_FAILED, current_brightness))
      sleep(LEDS_TIME_BETWEEN_FAILED_BLINKS)

      while current_brightness > 0.0:
        current_brightness -= LEDS_FADE_BRIGHTNESS_STEP
        self.LEDS.fill((*LEDS_COLOUR_FAILED, current_brightness))
        sleep(LEDS_FADE_TIME_BETWEEN_STEP)
      self.fill_default()


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
    self.leds = LedsManager()

  def send_tap(self, id):
    """refer to docs.python-requests.org for implementation examples"""
    params={
      'nfc_tag': {
        'uid':id
      },
      'tap_datetime': datetime.now(TZ).isoformat(),
      'label': LABEL,
      'data': {
        'nfc_reader': {
          'mac_address': MAC_ADDRESS,
          'reader_ip': IP_ADDRESS,
          'reader_model': READER_MODEL,
          'reader_name': READER_NAME,
        }
      }
    }
    headers={'Authorization': 'Token ' + AUTH_TOKEN}
    try:
      r=requests.post(url=XOS_URL, json=params, headers=headers)
      if r.status_code==201:
        log(r.text)
        return(0)
      else:
        log(r.text)
        sentry_sdk.capture_message(r.text)
        return(1)
    except requests.exceptions.ConnectionError as e:
      log("Failed to post tap message to %s: %s\n%s" % (XOS_URL, params, str(e)))
      sentry_sdk.capture_exception(e)
      return(1)

  def tap_on(self):      
    log(" Tap On: ", self.last_id)
    self.leds.success()
    self.send_tap(self.last_id)

  def tap_off(self):
    log("Tap Off: ", self.last_id)
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
    Convert 04:04:A5:2C:F2:2A:5E:80 to 04a52cf22a5e80 by:
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
  print("KioskIV Lens Reader")

  shell = True
  popen = subprocess.Popen(CMD, cwd=FOLDER, shell=shell, stdout=subprocess.PIPE)

  tap_manager = TapManager()
    
  while True:
    # wait for the next line from the C interface (if there are no NFCs there will be no lines)
    line = popen.stdout.readline().decode("utf-8")

    # see if it is a tag read
    if re.match(BYTE_STRING_RE, line):
      # We have an ID.
      tap_manager.read_line(line)


if __name__ == "__main__":
  main()
