#!/usr/bin/python

""" Kiosk IV tap reader running on a Raspberry Pi """

import subprocess
import sys


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
        print("Got: ", line)


if __name__ == "__main__":
  main()

