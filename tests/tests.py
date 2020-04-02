from time import sleep, time
from unittest.mock import MagicMock, patch

from requests.exceptions import ConnectionError as RequestsConnectionError

import src.runner
from src.runner import LEDControllerThread, TapManager

src.runner.TAP_SEND_RETRY_SECS = 0.1


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


@patch('requests.post', MagicMock(side_effect=RequestsConnectionError()))
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


@patch('requests.post', MagicMock(side_effect=RequestsConnectionError()))
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
