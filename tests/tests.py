import json
import os
from time import sleep, time
from unittest.mock import MagicMock, patch

import requests

import src.runner
from src.runner import LEDControllerThread, TapManager
from src.utils import get_ip_address

src.runner.TAP_SEND_RETRY_SECS = 0.1


def file_to_string_strip_new_lines(filename):
    """
    Read file and return as string with new line characters stripped
    :param filename: a filename relative to the current working directory.
    e.g. 'xml_files/example.xml' or 'example.xml'
    :return: a string representation of the contents of filename, with new line characters removed
    """
    # get current working directory
    cwd = os.path.dirname(__file__)
    file_as_string = ""

    # open filename assuming filename is relative to current working directory
    with open(os.path.join(cwd, filename), 'r') as file_obj:
        # strip new line characters
        file_as_string = file_obj.read().replace('\n', '')
    # return string
    return file_as_string


def mocked_requests_get(*args, **kwargs):
    """
    Thanks to https://stackoverflow.com/questions/15753390/how-can-i-mock-requests-and-the-response
    """
    class MockResponse:
        def __init__(self, json_data, status_code):
            self.content = json.loads(json_data)
            self.status_code = status_code

        def json(self):
            return self.content

        def raise_for_status(self):
            return None

    if '/v1/device?apikey' in args[0]:
        return MockResponse(file_to_string_strip_new_lines('data/device.json'), 200)

    return MockResponse(None, 404)


def test_leds_ramp_on():
    """
    Test that ramp_on on the LEDs work. That is, the LEDs become a colour
    in a defined time frame.
    """
    leds_controller = LEDControllerThread()

    # initial leds colour
    assert leds_controller.current_colour == [0, 0, 0]

    # send a ramp on action to the leds
    target_colour = [100, 150, 200]
    ramp_duration = 1.0
    leds_controller.ramp_on(target_colour, ramp_duration)

    # leds are being animated
    start_time = time()
    sleep(0.1)
    leds_controller.update_leds(start_time)
    assert leds_controller.current_colour != [0, 0, 0]
    assert leds_controller.current_colour != target_colour

    # leds should have finished animating
    sleep(ramp_duration)
    leds_controller.update_leds(start_time)
    assert leds_controller.current_colour == target_colour


def test_leds_ramp_off():
    """
    Test that ramp_off on the LEDs work. That is, the LEDs go back to the
    breathe colour in a defined time frame.
    """
    leds_controller = LEDControllerThread()
    leds_controller.current_colour = [100, 150, 200]

    # send a ramp off action to the leds
    ramp_duration = 1.0
    leds_controller.ramp_off(ramp_duration)

    # leds are being animated
    start_time = time()
    sleep(0.1)
    leds_controller.update_leds(start_time)
    assert leds_controller.current_colour != [100, 150, 200]
    assert leds_controller.current_colour != leds_controller.breathe_colour

    # leds should have finished animating
    sleep(ramp_duration)
    leds_controller.update_leds(start_time)
    assert leds_controller.current_colour == leds_controller.breathe_colour


def test_tap_on_queues_taps():
    """
    Test that a tap_on enqueues a tap
    """
    tap_manager = TapManager()
    assert tap_manager.queue.empty()

    # add a tap to the queue
    tap_manager.last_id = '123456789'
    tap_manager.tap_on()
    assert tap_manager.queue.qsize() == 1

    _, tap = tap_manager.queue.get()
    assert tap['lens']['uid'] == '123456789'


@patch('requests.post', side_effect=MagicMock())
def test_send_tap_or_requeue(xos_request):
    """
    Test send_tap_or_requeue takes taps from the queue and makes requests to XOS
    """
    tap_manager = TapManager()
    assert tap_manager.queue.empty()

    # add two taps to the queue
    tap_manager.last_id = '123456789'
    tap_manager.tap_on()
    tap_manager.last_id = '000000000'
    tap_manager.tap_on()
    assert tap_manager.queue.qsize() == 2

    # send the taps
    tap_manager.send_tap_or_requeue()
    tap_manager.send_tap_or_requeue()

    assert xos_request.call_count == 2
    assert tap_manager.queue.empty()


@patch('requests.post', MagicMock(side_effect=requests.exceptions.ConnectionError()))
def test_send_tap_or_requeue_no_network():
    """
    Test send_tap_or_requeue enqueues tap again on network failure
    """
    tap_manager = TapManager()
    assert tap_manager.queue.empty()

    # add a tap to the queue
    tap_manager.last_id = '123456789'
    tap_manager.tap_on()
    assert tap_manager.queue.qsize() == 1

    # try to send the tap
    tap_manager.send_tap_or_requeue()

    # check that the tap was put back in the queue
    assert tap_manager.queue.qsize() == 1
    _, tap = tap_manager.queue.get()
    assert tap['lens']['uid'] == '123456789'


@patch('requests.post', MagicMock(side_effect=requests.exceptions.ConnectionError()))
@patch('sentry_sdk.capture_exception', side_effect=MagicMock())
def test_send_tap_or_requeue_no_network_does_not_spam_sentry(capture_exception):
    """
    Test that send_tap_or_requeue posts errors to sentry only once on network failure
    """
    tap_manager = TapManager()
    assert tap_manager.queue.empty()

    # add a tap to the queue
    tap_manager.last_id = '123456789'
    tap_manager.tap_on()
    assert tap_manager.queue.qsize() == 1

    # try to send the tap, check capture_exception was called once
    tap_manager.send_tap_or_requeue()
    assert capture_exception.call_count == 1

    # try to send the tap again, check capture_exception was called only once
    tap_manager.send_tap_or_requeue()
    tap_manager.send_tap_or_requeue()
    tap_manager.send_tap_or_requeue()
    assert capture_exception.call_count == 1


@patch('requests.get', MagicMock(side_effect=mocked_requests_get))
def test_get_ip_address_from_balena():
    """
    Test get_ip_address from the Balena supervisor API works as expected.
    """
    ip_address = get_ip_address()
    assert ip_address == '10.1.2.3'


@patch('requests.get', MagicMock(side_effect=requests.exceptions.ConnectionError()))
def test_get_ip_address_from_balena_fails_gracefully():
    """
    Test get_ip_address from the Balena supervisor API fails gracefully.
    """
    ip_address = get_ip_address()
    assert not ip_address


@patch('requests.get', MagicMock(side_effect=mocked_requests_get))
def test_tap_data_shape():
    """
    Test the Tap data shape is as expected.
    """
    # src.utils gets run before we can mock it
    # So run get_ip_address() again to set it here
    src.runner.IP_ADDRESS = get_ip_address()
    lens_uid = 'abcdef123456'
    tap_manager = TapManager()
    tap = tap_manager.create_tap(lens_uid)
    assert tap['lens']['uid'] == lens_uid
    assert tap['tap_datetime']
    assert tap['label'] == '1'
    assert tap['data']['lens_reader']['device_name'] == 'DD-00'
    assert tap['data']['lens_reader']['mac_address']
    assert tap['data']['lens_reader']['reader_ip'] == '10.1.2.3'
    assert tap['data']['lens_reader']['reader_model'] == 'IDTech Kiosk IV'
