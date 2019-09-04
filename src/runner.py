#!/usr/bin/python

""" Kiosk IV tap reader running on a Raspberry Pi """

import subprocess
import sys
from datetime import datetime


def main():
  """Launcher."""
  print("KioskIV Lens Reader")

  args = ["./idtech_release"]
  shell = True
  count = 0
  popen = subprocess.Popen(args, shell=shell, stdout=subprocess.PIPE)
  while True:
      line = popen.stdout.readline()
      if line:
        print("%s: %s" % (
          datetime.now().strftime("%Y-%m-%d %H:%M"), 
          line.decode("utf-8")
        ), end='')


if __name__ == "__main__":
  main()