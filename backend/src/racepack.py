import settings, dash_log, can_controller
import RPi.GPIO as GPIO
import time, threading

"""
Okay so here is how all this nonsense works:

Driver Initiated Shift:
1. driver presses and holds button, "interrupt" triggers paddle_callback
2. paddle_callback starts a thread running code under try_shift
3. try_shift tests to see if driver has held down one or two buttons
    a) if one button held, asks the shifter to shift in that direction
        i) shifter waits until safe, then uses the correct RacePackOutput to
        pull racepack pin HIGH
    b) if both buttons held, autoup is toggled
"""



shift_mutex = threading.Lock() # prevents synchronous GPIO output

class RacePackOutput:
    """
    Encapsulates access to the GPIO outputs to the racepack
    """

    def __init__(self, listen_pin, output_pin, msg):
        self.listen_pin = listen_pin
        self.output_pin = output_pin
        self.message = msg

    def shift(self):
        """ sends the racepack and ecu signals indicating shifts """
        settings.shiftmsg_mutex.acquire()
        can_controller.shift_message[0] |= self.message
        settings.shiftmsg_mutex.release()

        GPIO.output(self.output_pin, GPIO.HIGH)
        time.sleep(settings.shift_hold_time)
        GPIO.output(self.output_pin, GPIO.LOW)

        # show me that the gear is changing if im testing
        # this will happen via a CAN message from the ECU normally
        if settings.debug == "user":
            if self.listen_pin == settings.downshift_listen_pin:
                if settings.car_status["GEAR"] == 1:
                    settings.car_status["GEAR"] = "N"
                else:
                    settings.car_status["GEAR"] = int(settings.car_status["GEAR"]) - 1
            else:
                if settings.car_status["GEAR"] == "N":
                    settings.car_status["GEAR"] = 1
                else:
                    settings.car_status["GEAR"] = int(settings.car_status["GEAR"]) + 1

class Shifter:
    """
    Faciliates shifts initiated from autoup and/or that human in the car
    """
    def __init__(self):
        self.current_bounce = dash_log.current_time()
        self.upshifter = RacePackOutput(settings.upshift_listen_pin, settings.upshift_output_pin, settings.upshift_msg)
        self.downshifter = RacePackOutput(settings.downshift_listen_pin, settings.downshift_output_pin, settings.downshift_msg)

    # executes the shift if the correct amount of time has passed
    # only lets one thread do it at a time
    def ask_for_shift(self, up: bool):
        shift_mutex.acquire()
        if(dash_log.current_time() - self.current_bounce >= settings.shift_bounce_time):
            if up and (settings.car_status["GEAR"] == "N" or int(settings.car_status["GEAR"]) < 4):
                self.upshifter.shift()

            elif not up and settings.car_status["GEAR"] != "N":
                gear = int(settings.car_status["GEAR"])
                if (gear == 1 and
                settings.car_status["SPEED"] < settings.speed_lockout) or(
                gear > 1):
                    self.downshifter.shift()

            self.current_bounce = dash_log.current_time()
        shift_mutex.release()
shifter = Shifter()
just_changed = False
def try_shift():
    global just_changed
    """
    initiated in new thread, looks at paddle positions and determines whether to
    1. upshift
    2. downshift
    or 3. toggle autoup
    """
    time.sleep(settings.paddle_hold)
    up_on = GPIO.input(settings.upshift_listen_pin) == GPIO.LOW
    down_on = GPIO.input(settings.downshift_listen_pin) == GPIO.LOW
    if(up_on and down_on):
        time.sleep(2)
        up_on = GPIO.input(settings.upshift_listen_pin) == GPIO.LOW
        down_on = GPIO.input(settings.downshift_listen_pin) == GPIO.LOW
        if up_on and down_on and not just_changed:
            settings.auto_up_status = not settings.auto_up_status
            settings.car_status[settings.AUTOUP] = settings.auto_up_status
            print("auto up is " + str(settings.auto_up_status))
        just_changed = not just_changed
    elif up_on:
        print("UP!!")
        shifter.ask_for_shift(True) # ask for upshift
    elif down_on:
        down_press = False # ask for downshift
        print("DOWN!!")
        shifter.ask_for_shift(False)

# called on press to shift paddle
def paddle_callback(channel):
        threading.Thread(target=try_shift).start()

# set up all the GPIO stuff
GPIO.setup(settings.upshift_listen_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.add_event_detect(settings.upshift_listen_pin, GPIO.FALLING, callback=paddle_callback)

GPIO.setup(settings.downshift_listen_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.add_event_detect(settings.downshift_listen_pin, GPIO.FALLING, callback=paddle_callback)

GPIO.setup(settings.upshift_output_pin, GPIO.OUT)
GPIO.setup(settings.downshift_output_pin, GPIO.OUT)

def auto_up_routine():
    while True:
        time.sleep(.1)
        if "RPM" in settings.car_status:
            rpm = int(settings.car_status["RPM"])
            if settings.auto_up_status and rpm >= 10500:
                shifter.ask_for_shift(True)
                time.sleep(.25)

threading.Thread(target=auto_up_routine).start()
