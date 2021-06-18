import json
import os
from time import sleep, time
from unittest.mock import MagicMock, patch

import requests

import src.runner
from src.runner import LEDControllerThread, TapManager
from src.utils import env_to_tuple, get_ip_address

src.runner.TAP_SEND_RETRY_SECS = 0.1


class MethodWrapper():
    """
    Used to add call_count to a patched method so we can test how many times a method is called.
    """
    def __init__(self, func):
        self.call_count = 0
        self.func = func

    def __call__(self, *args, **kwargs):
        self.call_count += 1
        return self.func(*args, **kwargs)


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


def mocked_requests_post(*args, **kwargs):
    """
    Thanks to https://stackoverflow.com/questions/15753390/how-can-i-mock-requests-and-the-response
    """
    class MockResponse:
        def __init__(self, json_data, status_code):
            self.text = json_data
            self.status_code = status_code

        def json(self):
            return json.loads(self.text)

        def raise_for_status(self):
            return None

    response = MockResponse('["No lens matching that uid could be found."]', 400)

    if 'maker-moment' in kwargs['url']:
        response = MockResponse(file_to_string_strip_new_lines('data/tap.json'), 200)
    elif 'xos' in kwargs['url']:
        response = MockResponse(file_to_string_strip_new_lines('data/tap.json'), 201)
    elif 'constellation-table' in kwargs['url']:
        response = MockResponse('{"invalid JSON" = "response"}', 200)
    elif 'valid-non-tap-response' in kwargs['url']:
        response = MockResponse('{"response": "ok"}', 200)
    elif 'lens-reader-taps-api' in kwargs['url']:
        response = MockResponse(file_to_string_strip_new_lines('data/tap.json'), 201)
    elif '502' in kwargs['url']:
        response = MockResponse('{"response": "502"}', 502)
    elif '503' in kwargs['url']:
        response = MockResponse('{"response": "503"}', 503)

    return response


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
    src.runner.AUTH_TOKEN = 'api-key'
    assert tap_manager.queue.empty()

    # add a tap to the queue
    tap_manager.last_id = '123456789'
    tap_manager.tap_on()
    assert tap_manager.queue.qsize() == 1

    _, tap, endpoint, api_key = tap_manager.queue.get()
    assert tap['lens']['uid'] == '123456789'
    assert endpoint == src.runner.TARGET_TAPS_ENDPOINT
    assert api_key == 'api-key'


def test_tap_on_doesnt_queue_phone_taps():
    """
    Test that a tap_on doesn't enqueue a phone tap (8 character UID)
    """
    tap_manager = TapManager()
    src.runner.AUTH_TOKEN = 'api-key'
    assert tap_manager.queue.empty()

    # a phone tap UID which is 8 characters
    tap_manager.last_id = '12345678'
    tap_manager.tap_on()
    assert tap_manager.queue.qsize() == 0
    tap_manager.tap_off()

    # add a tap to the queue
    tap_manager.last_id = '123456789'
    tap_manager.tap_on()
    assert tap_manager.queue.qsize() == 1


@patch('requests.post', MagicMock(side_effect=mocked_requests_post))
def test_send_tap_or_requeue():
    """
    Test send_tap_or_requeue takes taps from the queue and makes requests to XOS
    """
    tap_manager = TapManager()
    assert tap_manager.queue.empty()

    # add two taps to the queue
    tap_manager.last_id = '123456789'
    tap_manager.tap_on()
    tap_manager.tap_off()
    tap_manager.last_id = '000000000'
    tap_manager.tap_on()
    assert tap_manager.queue.qsize() == 2

    # send the taps
    src.runner.TARGET_TAPS_ENDPOINT = 'https://xos.acmi.net.au/api/taps/'
    tap_manager.send_tap_or_requeue()
    tap_manager.send_tap_or_requeue()
    assert tap_manager.queue.empty()
    src.runner.TARGET_TAPS_ENDPOINT = 'http://localhost:8000/api/taps/'


@patch('requests.post', MagicMock(side_effect=mocked_requests_post))
def test_send_tap_or_requeue_success_codes():
    """
    Test send_tap_or_requeue handles all response codes in TAP_SUCCESS_RESPONSE_CODES.
    """
    tap_manager = TapManager()
    assert tap_manager.queue.empty()

    # add two taps to the queue
    tap_manager.last_id = '123456789'
    # Mock XOS response
    src.runner.TARGET_TAPS_ENDPOINT = 'https://xos.acmi.net.au/api/taps/'
    tap_manager.tap_on()
    tap_manager.tap_off()
    tap_manager.last_id = '000000000'
    # Mock Maker Moment response
    src.runner.TARGET_TAPS_ENDPOINT = 'https://maker-moment/api/taps/'
    tap_manager.tap_on()
    tap_manager.tap_off()
    assert tap_manager.queue.qsize() == 2

    # send the taps
    assert tap_manager.send_tap_or_requeue() == 0
    assert tap_manager.send_tap_or_requeue() == 0

    # Mock Constellation Table bad JSON response
    tap_manager.last_id = '123456789'
    src.runner.TARGET_TAPS_ENDPOINT = 'https://constellation-table/api/taps/'
    tap_manager.tap_on()
    tap_manager.tap_off()
    assert tap_manager.send_tap_or_requeue() == 0

    # Mock a non-tap response
    tap_manager.last_id = '123456789'
    src.runner.TARGET_TAPS_ENDPOINT = 'https://valid-non-tap-response/api/taps/'
    tap_manager.tap_on()
    tap_manager.tap_off()
    assert tap_manager.send_tap_or_requeue() == 0

    assert tap_manager.queue.empty()
    # Reset Taps endpoint
    src.runner.TARGET_TAPS_ENDPOINT = 'http://localhost:8000/api/taps/'


@patch('requests.post', MagicMock(side_effect=mocked_requests_post))
def test_send_tap_or_requeue_failure():
    """
    Test send_tap_or_requeue handles a 400 error response from XOS.
    Test that the failed LEDs method is called as expected.
    """
    # XOS failed tap arrives before tap off
    tap_manager = TapManager()
    assert tap_manager.queue.empty()
    new_call = MethodWrapper(tap_manager.leds.failed)
    assert tap_manager.last_id_failed is None
    with patch.object(tap_manager.leds, 'failed', new=new_call) as fake_leds_failed:
        # add one tap to the queue
        tap_manager.last_id = '123456789'
        tap_manager.tap_on()
        assert tap_manager.queue.qsize() == 1
        assert tap_manager.leds.blocked_by == 'tap'

        # send the tap
        return_code = tap_manager.send_tap_or_requeue()
        assert tap_manager.queue.qsize() == 0
        assert return_code == 1
        assert tap_manager.last_id_failed
        assert fake_leds_failed.call_count == 0

        # simulate removing the lens
        tap_manager.tap_off()
        assert not tap_manager.last_id_failed
        assert fake_leds_failed.call_count == 1

    # XOS failed tap arrives after tap off
    tap_manager = TapManager()
    assert tap_manager.queue.empty()
    new_call = MethodWrapper(tap_manager.leds.failed)
    with patch.object(tap_manager.leds, 'failed', new=new_call) as fake_leds_failed:
        # add one tap to the queue
        tap_manager.last_id = '123456789'
        tap_manager.tap_on()
        assert tap_manager.queue.qsize() == 1
        assert tap_manager.leds.blocked_by == 'tap'

        # simulate removing the lens
        tap_manager.tap_off()
        assert tap_manager.last_id_failed is None
        assert fake_leds_failed.call_count == 0

        # send the tap
        return_code = tap_manager.send_tap_or_requeue()
        assert tap_manager.queue.qsize() == 0
        assert return_code == 1
        assert tap_manager.last_id_failed is None
        assert fake_leds_failed.call_count == 1


@patch('requests.post', MagicMock(side_effect=mocked_requests_post))
def test_send_tap_or_requeue_failure_unexpected_errors_before_tap_off():
    """
    Test send_tap_or_requeue handles unexpected (non-400) error responses from XOS before tap off.
    Test that the failed LEDs method is called as expected.
    """
    # XOS failed tap arrives before tap off
    tap_manager = TapManager()
    assert tap_manager.queue.empty()
    new_call = MethodWrapper(tap_manager.leds.failed)
    assert tap_manager.last_id_failed is None
    with patch.object(tap_manager.leds, 'failed', new=new_call) as fake_leds_failed:
        # Test a 502 error response
        src.runner.TARGET_TAPS_ENDPOINT = 'https://502.acmi.net.au/api/taps/'
        # add one tap to the queue
        tap_manager.last_id = '123456789'
        tap_manager.tap_on()
        assert tap_manager.queue.qsize() == 1
        assert tap_manager.leds.blocked_by == 'tap'

        # send the tap
        return_code = tap_manager.send_tap_or_requeue()
        assert tap_manager.queue.qsize() == 1
        assert return_code == 1
        assert tap_manager.last_id_failed
        assert fake_leds_failed.call_count == 0

        # simulate removing the lens
        tap_manager.tap_off()
        assert not tap_manager.last_id_failed
        assert fake_leds_failed.call_count == 1

        # send the tap successfully
        _, tap, _, _ = tap_manager.queue.get()
        tap_manager.queue.put((
            tap['tap_datetime'],
            tap,
            'https://xos.acmi.net.au/api/taps/',
            'api-key',
        ))
        return_code = tap_manager.send_tap_or_requeue()
        assert tap_manager.queue.qsize() == 0
        assert return_code == 0
        assert tap_manager.last_id_failed is False
        assert fake_leds_failed.call_count == 1

        src.runner.TARGET_TAPS_ENDPOINT = 'http://localhost:8000/api/taps/'


@patch('requests.post', MagicMock(side_effect=mocked_requests_post))
def test_send_tap_or_requeue_failure_unexpected_errors_after_tap_off():
    """
    Test send_tap_or_requeue handles unexpected (non-400) error responses from XOS after tap off.
    Test that the failed LEDs method is called as expected.
    """
    # XOS failed tap arrives after tap off
    tap_manager = TapManager()
    assert tap_manager.queue.empty()
    new_call = MethodWrapper(tap_manager.leds.failed)
    with patch.object(tap_manager.leds, 'failed', new=new_call) as fake_leds_failed:
        # Test a 503 error response
        src.runner.TARGET_TAPS_ENDPOINT = 'https://503.acmi.net.au/api/taps/'
        # add one tap to the queue
        tap_manager.last_id = '123456789'
        tap_manager.tap_on()
        assert tap_manager.queue.qsize() == 1
        assert tap_manager.leds.blocked_by == 'tap'

        # simulate removing the lens
        tap_manager.tap_off()
        assert tap_manager.last_id_failed is None
        assert fake_leds_failed.call_count == 0

        # send the tap
        return_code = tap_manager.send_tap_or_requeue()
        assert tap_manager.queue.qsize() == 1
        assert return_code == 1
        assert tap_manager.last_id_failed is None
        assert fake_leds_failed.call_count == 1

        # send the tap successfully
        _, tap, _, _ = tap_manager.queue.get()
        tap_manager.queue.put((
            tap['tap_datetime'],
            tap,
            'https://xos.acmi.net.au/api/taps/',
            'api-key',
        ))
        return_code = tap_manager.send_tap_or_requeue()
        assert tap_manager.queue.qsize() == 0
        assert return_code == 0
        assert tap_manager.last_id_failed is False
        assert fake_leds_failed.call_count == 1

        src.runner.TARGET_TAPS_ENDPOINT = 'http://localhost:8000/api/taps/'


@patch('requests.post', MagicMock(side_effect=mocked_requests_post))
def test_send_tap_or_requeue_success_leds():
    """
    Test that the success LEDs method is called as expected.
    """
    src.runner.TARGET_TAPS_ENDPOINT = 'https://xos.acmi.net.au/api/taps/'
    src.runner.ONBOARDING_LEDS_API = 'https://xos.acmi.net.au/api/fake'
    # XOS success tap arrives before tap off
    tap_manager = TapManager()
    assert tap_manager.queue.empty()
    new_call = MethodWrapper(tap_manager.leds.success)
    assert tap_manager.last_id_failed is None
    with patch.object(tap_manager.leds, 'success', new=new_call) as fake_leds_success:
        # add one tap to the queue
        tap_manager.last_id = '123456789'
        tap_manager.tap_on()
        assert tap_manager.queue.qsize() == 1
        assert not tap_manager.leds.blocked_by

        # send the tap
        return_code = tap_manager.send_tap_or_requeue()
        assert tap_manager.queue.qsize() == 0
        assert return_code == 0
        assert tap_manager.last_id_failed is None
        assert not tap_manager.last_id_failed
        assert fake_leds_success.call_count == 1

        # simulate removing the lens
        tap_manager.tap_off()
        assert tap_manager.last_id_failed is None
        assert fake_leds_success.call_count == 1

    # XOS success tap arrives after tap off
    tap_manager = TapManager()
    assert tap_manager.queue.empty()
    new_call = MethodWrapper(tap_manager.leds.success)
    with patch.object(tap_manager.leds, 'success', new=new_call) as fake_leds_success:
        # add one tap to the queue
        tap_manager.last_id = '123456789'
        tap_manager.tap_on()
        assert tap_manager.queue.qsize() == 1
        assert not tap_manager.leds.blocked_by

        # simulate removing the lens
        tap_manager.tap_off()
        assert tap_manager.last_id_failed is None
        assert fake_leds_success.call_count == 0

        # send the tap
        return_code = tap_manager.send_tap_or_requeue()
        assert tap_manager.queue.qsize() == 0
        assert return_code == 0
        assert tap_manager.last_id_failed is None
        assert fake_leds_success.call_count == 1

    src.runner.TARGET_TAPS_ENDPOINT = 'http://localhost:8000/api/taps/'
    src.runner.ONBOARDING_LEDS_API = None


@patch('requests.post', MagicMock(side_effect=requests.exceptions.ConnectionError()))
def test_send_tap_or_requeue_no_network():
    """
    Test send_tap_or_requeue enqueues tap again on network failure
    """
    tap_manager = TapManager()
    src.runner.AUTH_TOKEN = 'api-key'
    assert tap_manager.queue.empty()

    # add a tap to the queue
    tap_manager.last_id = '123456789'
    tap_manager.tap_on()
    assert tap_manager.queue.qsize() == 1

    # try to send the tap
    tap_manager.send_tap_or_requeue()

    # check that the tap was put back in the queue
    assert tap_manager.queue.qsize() == 1
    _, tap, endpoint, api_key = tap_manager.queue.get()
    assert tap['lens']['uid'] == '123456789'
    assert endpoint == src.runner.TARGET_TAPS_ENDPOINT
    assert api_key == 'api-key'
    src.runner.AUTH_TOKEN = ''


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


@patch('requests.post', MagicMock(side_effect=mocked_requests_post))
def test_taps_when_leds_blocked_by_remote():
    """
    Test tap_on and tap_off function as expected when blocked by a remote LED request.
    """
    tap_manager = TapManager()

    # simulate a remote LED request
    tap_manager.leds.blocked_by = 'remote'
    assert tap_manager.queue.qsize() == 0

    # try and tap
    tap_manager.last_id = '123456789'
    tap_manager.tap_on()
    tap_manager.tap_off()
    assert tap_manager.queue.qsize() == 0

    # return to initial state
    tap_manager.leds.blocked_by = None

    # try and tap
    tap_manager.last_id = '123456789'
    tap_manager.tap_on()
    tap_manager.tap_off()
    assert tap_manager.queue.qsize() == 1

    tap_manager.send_tap_or_requeue()
    assert tap_manager.queue.empty()


def test_toggle_lights():
    """
    Test the toggle_lights route.
    """
    tap_manager = src.runner.tap_manager
    tap_manager.__init__()
    with src.runner.app.test_client() as client:
        assert not tap_manager.leds.blocked_by

        # turn on LEDs
        data = dict(
            rgb_value=[5, 25, 25],
            ramp_time=0.5,
            cross_fade=1.0,
        )
        response = client.post(
            '/api/lights/',
            data=json.dumps(data),
            content_type='application/json',
        )
        assert response.status_code == 200
        assert tap_manager.leds.blocked_by == 'remote'
        assert tap_manager.leds.ramp_target_colour == data['rgb_value']
        assert tap_manager.leds.ramp_target == data['cross_fade']
        assert tap_manager.leds.ramp_duration == data['ramp_time']

        # try and tap
        tap_manager.last_id = '123456789'
        tap_manager.tap_on()
        tap_manager.tap_off()
        assert tap_manager.queue.qsize() == 0

        # turn off LEDs
        data = dict(
            rgb_value=[5, 25, 25],
            ramp_time=0.5,
            cross_fade=0.0,
        )
        response = client.post(
            '/api/lights/',
            data=json.dumps(data),
            content_type='application/json',
        )
        assert response.status_code == 200
        assert not tap_manager.leds.blocked_by

        # try and tap
        tap_manager.last_id = '123456789'
        tap_manager.tap_on()
        tap_manager.tap_off()
        assert tap_manager.queue.qsize() == 1


def test_toggle_lights_without_leds_control_override():
    """
    Test that the toggle_lights route responds as expected when
    the LEDS_CONTROL_OVERRIDE flag isn't set.
    """
    tap_manager = src.runner.tap_manager
    tap_manager.__init__()
    with src.runner.app.test_client() as client:
        assert not tap_manager.leds.blocked_by

        # tap on, but not off
        tap_manager.last_id = '123456789'
        tap_manager.tap_on()
        assert tap_manager.queue.qsize() == 1

        # turn on LEDs
        data = dict(
            rgb_value=[5, 25, 25],
            ramp_time=0.5,
            cross_fade=1.0,
        )
        response = client.post(
            '/api/lights/',
            data=json.dumps(data),
            content_type='application/json',
        )
        assert response.status_code == 409
        assert tap_manager.leds.blocked_by == 'tap'
        assert tap_manager.leds.ramp_target_colour != data['rgb_value']


def test_toggle_lights_with_leds_control_override():
    """
    Test that the toggle_lights route responds as expected when
    the LEDS_CONTROL_OVERRIDE flag is set True.
    """
    src.runner.LEDS_CONTROL_OVERRIDE = True
    tap_manager = src.runner.tap_manager
    tap_manager.__init__()
    with src.runner.app.test_client() as client:
        assert not tap_manager.leds.blocked_by

        # tap on, but not off
        tap_manager.last_id = '123456789'
        tap_manager.tap_on()
        assert tap_manager.queue.qsize() == 1

        # turn on LEDs
        data = dict(
            rgb_value=[5, 25, 25],
            ramp_time=0.5,
            cross_fade=1.0,
        )
        response = client.post(
            '/api/lights/',
            data=json.dumps(data),
            content_type='application/json',
        )
        assert response.status_code == 200
        assert tap_manager.leds.blocked_by == 'remote'
        assert tap_manager.leds.ramp_target_colour == data['rgb_value']
        assert tap_manager.leds.ramp_target == data['cross_fade']
        assert tap_manager.leds.ramp_duration == data['ramp_time']

        # turn off LEDs
        data = dict(
            rgb_value=[5, 25, 25],
            ramp_time=0.5,
            cross_fade=0.0,
        )
        response = client.post(
            '/api/lights/',
            data=json.dumps(data),
            content_type='application/json',
        )
        assert response.status_code == 200
        assert not tap_manager.leds.blocked_by

        # try and tap
        tap_manager.last_id = '123456789'
        tap_manager.tap_on()
        tap_manager.tap_off()
        assert tap_manager.queue.qsize() == 2


@patch('requests.post', side_effect=MagicMock())
def test_success_onboarding_lights(led_post):
    """
    Test that the onboarding lights POST request is sent as expected,
    and errors are handled successfully.
    """
    leds_controller = LEDControllerThread()
    leds_controller.success_onboarding_lights()
    assert led_post.call_count == 0

    src.runner.ONBOARDING_LEDS_API = 'https://xos.acmi.net.au/api/fake'
    src.runner.ONBOARDING_LEDS_DATA_SUCCESS = '{}'
    leds_controller.success_onboarding_lights()
    assert led_post.call_count == 1
    src.runner.ONBOARDING_LEDS_DATA_FAILED = '{}'
    leds_controller.failed_onboarding_lights()
    assert led_post.call_count == 2

    src.runner.ONBOARDING_LEDS_API = None


@patch('requests.post', MagicMock(side_effect=[
    requests.exceptions.ConnectionError(),
    requests.exceptions.Timeout(),
    requests.exceptions.HTTPError(),
]))
@patch('sentry_sdk.capture_exception', side_effect=MagicMock())
def test_success_onboarding_lights_fails_gracefully(capture_exception):
    """
    Test that the onboarding lights POST request is sent as expected,
    and errors are handled successfully.
    """
    src.runner.ONBOARDING_LEDS_API = None
    leds_controller = LEDControllerThread()
    leds_controller.success_onboarding_lights()
    assert capture_exception.call_count == 0

    src.runner.ONBOARDING_LEDS_API = 'https://xos.acmi.net.au/api/fake'
    src.runner.ONBOARDING_LEDS_DATA_SUCCESS = 'not-json'
    leds_controller.success_onboarding_lights()
    assert capture_exception.call_count == 1
    src.runner.ONBOARDING_LEDS_DATA_SUCCESS = '{"this": "that"}'
    leds_controller.success_onboarding_lights()
    assert capture_exception.call_count == 2
    leds_controller.success_onboarding_lights()
    assert capture_exception.call_count == 3
    leds_controller.success_onboarding_lights()
    assert capture_exception.call_count == 4

    src.runner.ONBOARDING_LEDS_API = None


def test_env_to_tuple():
    """
    Test the env_to_tuple method returns the expected results.
    """
    os.environ['ENV_VAR'] = '1,2,3'
    os.environ['ENV_VAR_FLOATS'] = '1.2,2.3,3.45'
    assert env_to_tuple('ENV_VAR') == (1, 2, 3)
    assert env_to_tuple('ENV_VAR_FLOATS') == (1.2, 2.3, 3.45)


@patch('requests.post', MagicMock(side_effect=requests.exceptions.ConnectionError()))
def test_taps_api():
    """
    Test the taps_endpoint route functions as expected.
    """
    src.runner.AUTH_TOKEN = 'another-key'
    tap_manager = src.runner.tap_manager
    tap_manager.__init__()
    with src.runner.app.test_client() as client:
        assert not tap_manager.leds.blocked_by
        assert tap_manager.queue.qsize() == 0

        # POST a maker moment tap to the Lens Reader API
        data = {
            'lens': {
                'uid': 'abcdef123456'
            },
            'tap_datetime': '2021-05-31T14:46:44.202649+09:30',
            'experience_id': 'some-experience-id',
            'maker_moment': 3,
            'data': {
                'lens_reader': {
                    'mac_address': '24:2A:C1:10:00:4',
                    'reader_ip': '192.168.1.108',
                    'reader_model': 'Tests',
                    'reader_name': 'Pytest'
                }
            }
        }
        response = client.post(
            '/api/taps/',
            data=json.dumps(data),
            content_type='application/json',
            headers={'Authorization': 'Token api-key'},
        )
        assert response.status_code == 201
        assert json.loads(response.data)['success']
        assert not tap_manager.leds.blocked_by
        assert tap_manager.queue.qsize() == 1

        # POST with no data
        response = client.post(
            '/api/taps/',
            data=json.dumps({}),
            content_type='application/json',
            headers={'Authorization': 'Token api-key'},
        )
        assert response.status_code == 400
        assert not json.loads(response.data)['success']
        assert not tap_manager.leds.blocked_by
        assert tap_manager.queue.qsize() == 1

        # POST with no api_key
        response = client.post(
            '/api/taps/',
            data=json.dumps(data),
            content_type='application/json',
        )
        assert response.status_code == 401
        assert not json.loads(response.data)['success']
        assert not tap_manager.leds.blocked_by
        assert tap_manager.queue.qsize() == 1

        # POST with no Lens UID
        tap_data = dict(data)
        response = client.post(
            '/api/taps/',
            data=json.dumps(tap_data.pop('lens')),
            content_type='application/json',
        )
        assert response.status_code == 400
        assert not json.loads(response.data)['success']
        assert not tap_manager.leds.blocked_by
        assert tap_manager.queue.qsize() == 1

        # Try to send the tap
        tap_manager.send_tap_or_requeue()

        # Check tap is re-queued as expected
        tap_datetime, tap, endpoint, api_key = tap_manager.queue.get()
        assert tap_datetime == data['tap_datetime']
        assert tap == data
        assert endpoint == src.runner.XOS_TAPS_ENDPOINT
        assert api_key == 'api-key'
        assert src.runner.AUTH_TOKEN == 'another-key'
