import os
import platform
import uuid
from datetime import datetime

import pytz
import requests
import sentry_sdk

DEBUG = os.getenv('DEBUG', 'false').lower() == 'true'
DNS_SERVER = os.getenv('DNS_SERVER', '8.8.8.8')
DNS_PORT = int(os.getenv('DNS_PORT', '80'))
TIMEZONE = os.getenv('TIMEZONE', 'Australia/Victoria')
IS_OSX = platform.system() == 'Darwin'
BALENA_SUPERVISOR_ADDRESS = os.getenv('BALENA_SUPERVISOR_ADDRESS')
BALENA_SUPERVISOR_API_KEY = os.getenv('BALENA_SUPERVISOR_API_KEY')
DEVICE_NAME = os.getenv('BALENA_DEVICE_NAME_AT_INIT', 'DD-00')
SENTRY_ID = os.getenv('SENTRY_ID')

# Set pytz timezone
TZ = pytz.timezone(TIMEZONE)

# Setup Sentry
sentry_sdk.init(SENTRY_ID)


def log(*args):
    print('%s: ' % datetime.now().strftime('%Y-%m-%d %H:%M:%S'), *args)


# Get mac address
def get_mac_address():
    mac_num = hex(uuid.getnode()).replace('0x', '').upper()
    mac = ':'.join(mac_num[i:i + 2] for i in range(0, 11, 2))
    return mac


MAC_ADDRESS = get_mac_address()


def get_ip_address():
    """
    Get the Lens Reader's IP address from the Balena supervisor API.
    Note: the Balena supervisor is a local API.
    """
    ip_address = None
    try:
        balena_api_url = f'{BALENA_SUPERVISOR_ADDRESS}/v1/device'\
                            f'?apikey={BALENA_SUPERVISOR_API_KEY}'
        headers = {
            'Content-Type': 'application/json',
        }
        response = requests.get(balena_api_url, headers=headers)
        response.raise_for_status()
        ip_address = response.json().get('ip_address')
    except (
            requests.exceptions.ConnectionError,
            requests.exceptions.HTTPError,
            requests.exceptions.MissingSchema,
    ) as exception:
        print(f'Failed to get IP address from Balena API: {exception}')
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


def set_hostname():
    """
    Set the hostname of this device to the DEVICE_NAME using the Balena supervisor API.
    Note: the Balena supervisor is a local API.
    """
    status = None
    try:
        if DEBUG:
            print(f'Attempting to set hostname to {DEVICE_NAME}')
        balena_api_url = f'{BALENA_SUPERVISOR_ADDRESS}/v1/device/host-config'\
                            f'?apikey={BALENA_SUPERVISOR_API_KEY}'
        json_data = {
            'hostname': DEVICE_NAME
        }
        response = requests.patch(balena_api_url, json=json_data)
        status = response.status_code
        response.raise_for_status()
    except (
            requests.exceptions.ConnectionError,
            requests.exceptions.HTTPError,
            requests.exceptions.MissingSchema,
    ) as exception:
        message = f'Failed to set the hostname with exception: {exception}'
        print(message)
        sentry_sdk.capture_exception(exception)
    if DEBUG:
        print(f'Set hostname to {DEVICE_NAME} returned {status}')
    return status
