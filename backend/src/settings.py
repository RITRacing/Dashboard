import threading, logging, os
import dash_log
import RPi.GPIO as GPIO
"""
Definition of global values used throughout the program
"""

""" whether or not the vehicle will shift automatically """
auto_up_status = False

""" sets the board mode """
GPIO.setmode(GPIO.BCM)

""" tells the GPIO to shut up about channels already in use """
GPIO.setwarnings(False)

""" the CAN ids read from ECU """
controller_id = {
    "ECU_IDprim":0x00FF,
    "ECU_IDsec" :0x00FC,
    "TireTempF" :0x03F2,
    "TireTempR" :0x03F4,
    "ECar"      : 0x0626
}

""" CAN id of message sent to ECU indicating shift status """
shift_id = 0x0610

""" how long the driver must hold the paddle in order to initiate a shift"""
shift_hold_time = 0.2

""" lock used to sync the message sent to the ECU"""
shift_mutex = threading.Lock()

""" CAN message indicating an upshift """
upshift_msg = 0x02

""" CAN message indicating a downshift """
downshift_msg = 0x01

""" CAN message indicating no shift is taking place """
noshift_msg = 0x00

""" dictionary that holds current information about the vehicle, sent from the ECU """
car_status = {}

""" how slow the vehicle must be going in order to shift into neutral """
speed_lockout = 20

""" format used to construct/dissect CAN frames """
can_frame_format = "=IB3x8s"

""" how far behind the watchdog time must get to initiate a shutdown """
watchdog_limit = 100

""" how often the watchdog thread checks for a trip """
wd_refresh_rate = 0.1

""" how often a message is sent to the ECU """
can_refresh_rate = 0.001

""" pin connected to upshift paddle """
upshift_listen_pin = 4

""" pin connected upshifting part of RacePack """
upshift_output_pin = 22

""" pin connected to downshift paddle """
downshift_listen_pin = 5

""" pin connected to downshifting part of RacePack """
downshift_output_pin = 6

""" bounce time for shift listeners """
shift_bounce_time = 200

""" how much time should be in between shifts of same direction """
shift_wait = 50

""" how long the driver must hold a paddle to initiate a shift """
paddle_hold = 0.005

""" channel of CAN shield used """
can_channel = 16

""" port the front end node.js web server listens on """
front_end_port = 8765

""" indicates if the pi is connected to the car, should use test data, or should use user input
    three values: car, test, user
"""
debug = "user"

""" determines whether or not there should be console output """
console_output = True


def clean_shutdown(signal, frame):
    """ closes resources and shuts down the program, caused by an error """
    logging.info('Signal handler entered. Closing server...')
    GPIO.cleanup()
    logging.info('GPIO Cleaned, server closed.')
    os._exit(1)


dash_log.print_msg("STARTUP", "settings initialized")
