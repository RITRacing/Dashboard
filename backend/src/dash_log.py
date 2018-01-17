import logging
import time
from datetime import datetime
from enum import Enum
import settings
"""
Provides a modifiable way to log events
"""


class LogType(Enum):
    AU_ON = 1 #autoup turned on
    AU_OFF = 2 #autoup turned off
    SHIFT_INIT = 3 #shift initiated by driver
    UPS_COMPLETE = 4 #upshift complete
    DS_COMPLETE = 5 #downshift complete
    SHIFT_THREAD = 6 #shift thread started
    WD_TRIP = 7 #watchdog thread tripped
    WD_THREAD = 8 #watchdog thread started
    AU_TRIGGER = 9 #autoup triggered upshift

# the current date
today = datetime.today()

# sets up a new log file for the new session
logging.basicConfig(filename='/home/dash/logs/{}_{}_{}.log'.format(str(today.day),
                                                                   str(today.month),
                                                                   str(today.year)), level=logging.DEBUG)


def current_time():
    """
    returns the current time in ms
    """
    return int(round(time.time() * 1000))


def formatted_current_time():
    """
    returns a string representation of the current time
    """
    return '({}:{}:{}.{}): '.format(datetime.now().time().hour,
                                           datetime.now().time().minute,
                                           datetime.now().time().second,
                                           datetime.now().time().microsecond)


def log_event(log_type):
    """
    logs an event based on the events defined in LogType
    """
    if log_type == LogType.AU_ON:
        logging.warning("{} AutoUp Activated.".format(formatted_current_time()))
    elif log_type == LogType.AU_OFF:
        logging.warning("{} AutoUp Deactivated.".format(formatted_current_time()))
    elif log_type == LogType.SHIFT_INIT:
        logging.info("{} Shift Demanded.".format(formatted_current_time()))
    elif log_type == LogType.UPS_COMPLETE:
        logging.info("{} UpShift Completed.".format(formatted_current_time()))
    elif log_type == LogType.DS_COMPLETE:
        logging.info("{} DownShift Completed.".format(formatted_current_time()))
    elif log_type == LogType.SHIFT_THREAD:
        logging.info("{} Shift Thread Started.".format(formatted_current_time()))
    elif log_type == LogType.WD_TRIP:
        logging.warning("{} Watchdog has tripped! Resetting...".format(formatted_current_time()))
    elif log_type == LogType.WD_THREAD:
        logging.info("{} WatchDog Thread Started.".format(formatted_current_time()))
    elif log_type == LogType.AU_TRIGGER:
        logging.info("{} Auto-Up Triggered an UpShift.".format(formatted_current_time()))


def log_info(info):
    logging.info("{} .".format(formatted_current_time()) + info)


def print_msg(type, msg):
    if settings.console_output:
        print("[" + type + "] " + msg)
