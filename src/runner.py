#!/usr/bin/python

""" description of the module"""

#imports
import time
import re
from blinkt import set_pixel, show, set_all
import subprocess
import requests
from random import choice
import pytz
import socket
import uuid
from datetime import datetime

#globals

#class definitions

# function definitions

# Set pytz timezone
pytz_timezone = pytz.timezone('Australia/Melbourne')

# Get mac address
def get_mac():
  mac_num = hex(uuid.getnode()).replace('0x', '').upper()
  mac = ':'.join(mac_num[i : i + 2] for i in range(0, 11, 2))
  return mac

# Get IP address
def get_ip_address():
  # TOFIX:
#   return (([ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] if not ip.startswith("127.")] or [[(s.connect(("8.8.8.8", 53)), s.getsockname()[0], s.close()) for s in [socket.socket(socket.AF_INET, socket.SOCK_DGRAM)]][0][1]]) + ["no IP found"])[0]
  return '172.16.80.133'
ip_address = get_ip_address()
reader_name = 'nfc-' + ip_address.split('.')[-1]

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
  #print("Simulating MuseumOS Post")
  #return(choice([0,1]))
  URL="https://museumos-prod.acmi.net.au/api/taps/"
  #URL="https://museumos-stg.acmi.net.au/api/taps/"
  #PARAMS={'q':data}
  #PARAMS_NFC={'atr':'SECOND_STORY_TEST','uid':data}
  #PARAMS_NFC_READER={'mac_address':'PROTOTYPE_MAC_ADDRESS','reader_ip':'PROTOTYPE_READER_IP'}
  #PARAMS={'nfc_tag': PARAMS_NFC,'nfc_reader': PARAMS_NFC_READER,'tap_datetime':'CURRENT_TIMESTAMP'}
  PARAMS={'nfc_tag': {'atr':'SECOND_STORY_TEST','uid':data.lower()},'nfc_reader': {'mac_address':get_mac(),'reader_ip':get_ip_address(), 'reader_name': reader_name, 'reader_model':'IDTech Kiosk IV'},'tap_datetime':datetimeNowTimeZoneIso8601()}
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

