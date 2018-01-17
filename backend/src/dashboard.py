
import settings

import dash_log

import racepack

import can_controller

import watchdog

import dash_model

import signal
import threading
import sys
import websockets
import asyncio
"""
-Takes information from the ECU and passes it to the front end
-Listens for driver paddle presses and initiates shifts
-Tells ECU about shifts
"""
# TODO: remove white flashing


def main():
    # setup the shutdown hook
    signal.signal(signal.SIGINT, settings.clean_shutdown)

    #determine if connected to can shield or should use test data
    settings.debug = (len(sys.argv) != 1)

    if not settings.debug:
        #connect to CAN shield
        can_controller.setup_can()
        dash_log.print_msg("STARTUP", "connected to can interface")
    else:
        dash_log.print_msg("STARTUP", "debug mode selected")


    # default car_status
    settings.car_status["GEAR"] = 1
    settings.car_status["SPEED"] = 10
    settings.car_status["AUTOUP"] = 0

    dash_log.print_msg("STARTUP", "car status defaulted")

    if not settings.debug:
        # start ECU communication thread
        ecu_thread = threading.Thread(target=can_controller.output_shift)
        ecu_thread.start()
        dash_log.print_msg("STARTUP", "began sending to ECU")
        dash_log.log_event(dash_log.LogType.SHIFT_THREAD)

        # start watchdog thread
        watchdog_thread = threading.Thread(target=watchdog.monitor)
        watchdog_thread.start()
        dash_log.print_msg("STARTUP", "watchdog started")
        dash_log.log_event(dash_log.LogType.WD_THREAD)

    input_thread = threading.Thread(target=read_can_input)
    input_thread.start()

    event_loop = asyncio.get_event_loop()
    event_loop.run_until_complete(websockets.serve(dash_model.update_frontend, "localhost", settings.front_end_port))
    event_loop.run_forever()



def read_can_input():
    if settings.debug:
        dash_log.print_msg("STARTUP", "reading test data")
    else:
        dash_log.print_msg("STARTUP", "reading CAN")
    while True:
        if settings.debug:
            with open(sys.argv[1]) as test_file:
                for line in test_file:
                    can_controller.read_fake_input(line)

        else:
            can_controller.read_input()




if __name__ == "__main__":
    main()
