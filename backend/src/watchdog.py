import settings, dash_log
import threading, time
"""
Keeps track of the program and whether or not it has crashed,
restarts if so
"""

mutex = threading.Lock()
wd_timer = dash_log.current_time() + 100


def update():
    """ updates watchdog time to current time"""
    global wd_timer
    mutex.acquire()
    wd_timer = dash_log.current_time()
    mutex.release()


def monitor():
    """ executed on separate thread, constantly checks for a trip """
    while True:
        mutex.acquire()
        if dash_log.current_time() - wd_timer > settings.watchdog_limit:
            settings.clean_shutdown()
        mutex.release()
        time.sleep(settings.wd_refresh_rate)
