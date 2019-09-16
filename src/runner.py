#!/usr/bin/python

""" Kiosk IV tap reader running on a Raspberry Pi """

import subprocess
import sys
from datetime import datetime
from time import time
import re
from threading import Timer
import board
import adafruit_dotstar as dotstar

BYTE_STRING_RE = r'([0-9a-fA-F]{2}:?)+'
TAP_OFF_TIMEOUT = 0.5 # seconds


def byte_string_to_lens_id(byte_string):
  """
  Convert 04:04:A5:2C:F2:2A:5E:80 to 04a52cf22a5e80 by:
  - stripping ":" and whitespace
  - removing first byte
  - lowercasing
  """
  return "".join(byte_string.strip().split(":")[1:]).lower()


def log(*args):
    print("%s: " % datetime.now().strftime("%Y-%m-%d %H:%M:%S"), 
      *args
    )


class TapManager:
  last_id = None
  last_id_time = time()
  tap_off_timer = None

  # Using a DotStar Digital LED Strip with 12 LEDs connected to hardware SPI
  LEDS = dotstar.DotStar(board.SCK, board.MOSI, 12, brightness=0.1)
  LEDS_COLOUR_DEFAULT = (254, 254, 254)  # White
  LEDS_COLOUR_SUCCESS = (0, 254, 0)  # Green
  LEDS_COLOUR_FAILED = (254, 0, 0)  # Red
  LEDS_DEFAULT_BRIGHTNESS = 0.1
  LEDS_MAX_BRIGHTNESS = 0.5
  LEDS_FADE_BRIGHTNESS_STEP = 0.01
  LEDS_FADE_TIME_BETWEEN_STEP = 0.005
  LEDS_TIME_BETWEEN_FAILED_BLINKS = 0.3

  def leds_fill_default(self):
    self.LEDS.fill((*self.LEDS_COLOUR_DEFAULT, self.LEDS_DEFAULT_BRIGHTNESS))

  def leds_success(self):
    current_brightness = self.LEDS_MAX_BRIGHTNESS
    while current_brightness > 0.0:
      current_brightness -= self.LEDS_FADE_BRIGHTNESS_STEP
      self.LEDS.fill((*self.LEDS_SUCCESS_COLOUR, current_brightness))
      time.sleep(self.LEDS_FADE_TIME_BETWEEN_STEP)
    self.leds_fill_default()

  def leds_failed(self):
    current_brightness = self.LEDS_MAX_BRIGHTNESS

    self.LEDS.fill((*self.LEDS_COLOUR_FAILED, current_brightness))
    time.sleep(self.LEDS_TIME_BETWEEN_FAILED_BLINKS)

    self.LEDS.fill((*self.LEDS_COLOUR_FAILED, 0.0))
    time.sleep(self.LEDS_TIME_BETWEEN_FAILED_BLINKS)

    self.LEDS.fill((*self.LEDS_COLOUR_FAILED, current_brightness))
    time.sleep(self.LEDS_TIME_BETWEEN_FAILED_BLINKS)

    while current_brightness > 0.0:
      current_brightness -= self.LEDS_FADE_BRIGHTNESS_STEP
      self.LEDS.fill((*self.LEDS_COLOUR_FAILED, current_brightness))
      time.sleep(self.LEDS_FADE_TIME_BETWEEN_STEP)
    self.leds_fill_default()

  def tap_on(self):      
    log(" Tap On: ", self.last_id)

  def tap_off(self):
    log("Tap Off: ", self.last_id)
    self.last_id = None

  def reset_tap_off_timer(self):
    # reset the tap-off timer for this ID
    if self.tap_off_timer:
      self.tap_off_timer.cancel()
      del self.tap_off_timer
    self.tap_off_timer = Timer(TAP_OFF_TIMEOUT, self.tap_off)
    self.tap_off_timer.start()

  def read_id(self, id):
    if id != self.last_id:
      # send a tap-off message if needed
      if self.tap_off_timer and self.tap_off_timer.is_alive():
        self.tap_off_timer.cancel()
        self.tap_off()
      self.last_id = id
      self.tap_on()
    self.reset_tap_off_timer()


def main():
  """Launcher."""
  print("KioskIV Lens Reader")

  cmd = ["./idtech_debug"]
  shell = True
  count = 0
  popen = subprocess.Popen(cmd, shell=shell, stdout=subprocess.PIPE)

  tap_manager = TapManager()
    
  while True:
    # wait for the next line from the C interface (if there are no NFCs there will be no lines)
    line = popen.stdout.readline().decode("utf-8")

    # see if it is a tag read
    if re.match(BYTE_STRING_RE, line):
      # We have an ID. Note the time and ID
      id = byte_string_to_lens_id(line)
      tap_manager.read_id(id)


if __name__ == "__main__":
  main()