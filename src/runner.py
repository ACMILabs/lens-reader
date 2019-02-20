#!/usr/bin/python

""" description of the module"""

#imports
import time
import re
from blinkt import set_pixel, show, set_all
import subprocess
import requests
from random import choice
import os
import pytz
import socket
import uuid
from datetime import datetime

#globals

#class definitions

# function definitions

# Constants
TIMEZONE = os.getenv('TIMEZONE')
DNS_SERVER = os.getenv('DNS_SERVER')
DNS_PORT = os.getenv('DNS_PORT')
DEVICE_NAME = os.getenv('DEVICE_NAME')
XOS_URL = os.getenv('XOS_URL')
READER_MODEL = os.getenv('READER_MODEL')

# Set pytz timezone
pytz_timezone = pytz.timezone(TIMEZONE)

# Get mac address
def get_mac():
  mac_num = hex(uuid.getnode()).replace('0x', '').upper()
  mac = ':'.join(mac_num[i : i + 2] for i in range(0, 11, 2))
  return mac

# Get IP address
def get_ip_address():
  s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  s.connect((DNS_SERVER, int(DNS_PORT)))
  tmp_ip_address = s.getsockname()[0]
  s.close()
  return tmp_ip_address
ip_address = get_ip_address()
reader_name = DEVICE_NAME or 'nfc-' + ip_address.split('.')[-1]

def datetimeNowTimeZoneIso8601():
  return datetime.now(pytz_timezone).isoformat()

def read_command_script_to_file():
  temp_file = open("temp", 'w')
  subprocess.call(['./oneshot_read.sh'], stdout=temp_file)
  with open("temp",'r') as file:
    output=file.read()
    p = re.compile("DFED44\w\w(\w{14,14})", re.MULTILINE)
    m = p.search(output)
    # print(output)
    if m:
      print("UID: "+ m.group(1))
      return m.group(1)

def read_command_process():
  """runs the command to get a read fromt the kioskIV demo command line program"""
  demoProcess = subprocess.Popen(['./IDTechSDK_Demo'], shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  
  print(demoProcess)
  output, error=demoProcess.communicate()
  if error:
    raise Exception("Error "+str(error))
  print(output)
  pass

def post_uid_to_museumos(data):
  """refer to docs.python-requests.org for implementation examples"""
  URL=XOS_URL
  PARAMS={'nfc_tag': {'uid':data.lower()},'nfc_reader': {'mac_address':get_mac(),'reader_ip':get_ip_address(), 'reader_name': reader_name, 'reader_model':READER_MODEL},'tap_datetime':datetimeNowTimeZoneIso8601()}
  try:
    r=requests.post(url=URL,json=PARAMS)
    if r.status_code==201:
      print(r.text)
      return(0)
    else:
      print(r.text)
      return(1)
  except requests.exceptions.ConnectionError as e:
    print("Failed to post to MuseumOS: ", str(e))
    return(1)


def main():
  """Launcher."""
  print("KioskIV EO Prototype Test Script")
  while (1):
    set_all(15,15,15)
    show()
    uid = read_command_script_to_file()
    if uid:
      set_all(0,0,128)
      show()
      h = post_uid_to_museumos(data=uid)
      if h==1:
        print("error")
        set_all(255,0,0)
        show()
        time.sleep(1)
        set_all(0,0,0)
      elif h==0:
        print("success")
        set_all(0,255,0)
        show()
        time.sleep(1)
        set_all(0,0,0)

if __name__ == "__main__":
  main()

