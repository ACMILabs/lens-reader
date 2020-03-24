import os
import platform
import socket
import uuid
from datetime import datetime

import pytz

DNS_SERVER = os.getenv('DNS_SERVER', '8.8.8.8')
DNS_PORT = int(os.getenv('DNS_PORT', '80'))
TIMEZONE = os.getenv('TIMEZONE', 'Australia/Victoria')
IS_OSX = platform.system() == 'Darwin'

# Set pytz timezone
TZ = pytz.timezone(TIMEZONE)


def log(*args):
    print('%s: ' % datetime.now().strftime('%Y-%m-%d %H:%M:%S'), *args)


# Get mac address
def get_mac_address():
    mac_num = hex(uuid.getnode()).replace('0x', '').upper()
    mac = ':'.join(mac_num[i:i + 2] for i in range(0, 11, 2))
    return mac


MAC_ADDRESS = get_mac_address()


# Get IP address
def get_ip_address():
    socket_ = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    socket_.connect((DNS_SERVER, DNS_PORT))
    ip_address = socket_.getsockname()[0]
    socket_.close()
    return ip_address


IP_ADDRESS = get_ip_address()


def env_to_tuple(env_name, default=()):
    """
    allow environment values of "1,2,3" to be parsed into (1,2,3)
    """
    env_value = os.getenv(env_name)
    if not env_value:
        env_value = default
    return tuple([float(elem.strip()) for elem in env_value.split(',')])
