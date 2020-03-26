from time import sleep, time

from src.runner import LEDControllerThread


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
