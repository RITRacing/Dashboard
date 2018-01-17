import settings, dash_log, can_controller
import RPi.GPIO as GPIO
import time, threading
"""
-Defines classes that encapsulate the shifting process
-handles driver shifts
"""

"""
Represents a pin connected to the RacePack
"""
class RacePackOutput:
    def __init__(self, listen_pin, output_pin):
        """ the pin connected to the shift paddle button """
        self.listen_pin = listen_pin

        """ the pin connected to the RacePack """
        self.output_pin = output_pin

        """ keeps track of when the last shift happened to prevent shifts from occurring too often """
        self.prev_end = dash_log.current_time()

    def execute_shift(self, msg):
        """ tells the racepack to shift """

        # make sure the pack is not shifting
        if GPIO.input(self.listen_pin) == GPIO.LOW:
            # ensure the driver has held it long enough
            time.sleep(settings.paddle_hold)
            if GPIO.input(self.listen_pin) == GPIO.LOW:

                #update what is being sent to the ECU
                settings.shift_mutex.aquire()
                can_controller.shift_message |= msg
                settings.shift_mutex.release()

                #tell the racepack to shift
                GPIO.output(self.output_pin, GPIO.HIGH)
                time.sleep(settings.shift_hold_time)
                GPIO.output(self.output_pin, GPIO.LOW)

        # update the last shift time
        self.prev_end = dash_log.current_time()


def upshift():
    """ shifts up one gear if possible """

    # ensure this shift isn't happening too soon since the last upshift
    if dash_log.current_time() - upshifter.prev_end > settings.shift_wait:

        # cant shift past gear 4
        if settings.car_status["GEAR"] != 4:
            dash_log.log_event(dash_log.LogType.SHIFT_INIT)

            # execute shift in new thread
            shift_control_thread = threading.Thread(target=upshifter.execute_shift(settings.upshift_msg))
            shift_control_thread.start()


def downshift():
    """ shifts down one gear if possible """

    #ensure this shift isn't happening too soon since the last downshift
    if dash_log.current_time() - downshifter.prev_end > settings.shift_wait:
        dash_log.log_event(dash_log.LogType.SHIFT_INIT)

        # shift into neutral if gear is 1 and speed is low enough
        if settings.car_status["GEAR"] == 1:
            if settings.car_status["SPEED"] < settings.speed_lockout:
                shift_control_thread = threading.Thread(target=downshifter.execute_shift())
                shift_control_thread.start()

        # shift down if gear is > 1
        elif settings.car_status["GEAR"] > 1:
            shift_control_thread = threading.Thread(target=downshifter.execute_shift())
            shift_control_thread.start()


""" set up the upshift listener """
GPIO.setup(settings.upshift_listen_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(settings.upshift_output_pin, GPIO.OUT)
GPIO.add_event_detect(settings.upshift_listen_pin, GPIO.FALLING, callback=upshift,
                      bouncetime=settings.shift_bounce_time)

upshifter = RacePackOutput(settings.upshift_listen_pin, settings.upshift_output_pin)

dash_log.print_msg("STARTUP", "upshift routine defined")

""" setup the downshift listener """
GPIO.setup(settings.downshift_listen_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(settings.downshift_output_pin, GPIO.OUT)
GPIO.add_event_detect(settings.downshift_listen_pin, GPIO.FALLING, callback=downshift,
                      bouncetime=settings.shift_bounce_time)

downshifter = RacePackOutput(settings.downshift_listen_pin, settings.downshift_output_pin)

dash_log.print_msg("STARTUP", "downshift routine defined")