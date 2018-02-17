import settings
import dash_log
import websockets
import json
import asyncio

@asyncio.coroutine
def update_frontend(websocket, path):

    # set up some variables to hold last_values[settings.CURRENT] values to avoid sending duplicates

    last_values = {} # stores the last send value to avoid redundant transmission
    last_values[settings.RPM] = 0
    last_values[settings.OILT] = 0
    last_values[settings.WATERT] = 0
    last_values[settings.OILP] = 0
    last_values[settings.GEAR] = 0
    last_values[settings.SPEED] = 0
    last_values[settings.BATT] = 0
    last_values[settings.AIRT] = 0
    last_values[settings.SOC] = 0
    last_values[settings.CURRENT] = 0
    last_values[settings.LFAULT] = ""
    connection_name = yield from websocket.recv()
    dash_log.print_msg("STARTUP","connected to: " + connection_name)
    message = {}
    while True:
        """
        if settings.RPM in settings.car_status and settings.car_status[settings.RPM] != last_values[settings.RPM]:
            message[settings.RPM] = settings.car_status[settings.RPM]
            last_values[settings.RPM] = settings.car_status[settings.RPM]

        if settings.OILT in settings.car_status and settings.car_status[settings.OILT] != last_values[settings.OILT]:
            message[settings.OILT] = settings.car_status[settings.OILT]
            last_values[settings.OILT] = settings.car_status[settings.OILT]

        if settings.OILP in settings.car_status and settings.car_status[settings.OILP] != last_values[settings.OILP]:
            message[settings.OILP] = settings.car_status[settings.OILP]
            last_values[settings.OILP] = settings.car_status[settings.OILP]
        if settings.WATERT in settings.car_status and settings.car_status[settings.WATERT] != last_values[settings.WATERT]:
            message[settings.WATERT] = settings.car_status[settings.WATERT]
            last_values[settings.WATERT] = settings.car_status[settings.WATERT]
        if settings.BATT in settings.car_status and settings.car_status[settings.BATT] != batt:
            message[settings.BATT] = settings.car_status[settings.BATT]
            batt = settings.car_status[settings.BATT]
        if settings.SOC in settings.car_status and settings.car_status[settings.SOC] != last_values[settings.SOC]:
            message[settings.SOC] = settings.car_status[settings.SOC]
            last_values[settings.SOC] = settings.car_status[settings.SOC]
        if settings.LFAULT in settings.car_status and settings.car_status[settings.LFAULT] != last_values[settings.LFAULT]:
            message[settings.LFAULT] = settings.car_status[settings.LFAULT]
            last_values[settings.LFAULT] = settings.car_status[settings.LFAULT]
        if settings.CURRENT in settings.car_status and settings.car_status[settings.CURRENT] != last_values[settings.CURRENT]:
            message[settings.CURRENT] = settings.car_status[settings.CURRENT]
            last_values[settings.CURRENT] = settings.car_status[settings.CURRENT]

        if settings.GEAR in settings.car_status:
            if settings.car_status[settings.GEAR] != last_values[settings.GEAR]:
                if settings.car_status[settings.GEAR] == 0:
                    message[settings.GEAR] = "N"
                else:
                    message[settings.GEAR] = settings.car_status[settings.GEAR]

                last_values[settings.GEAR] = settings.car_status[settings.GEAR]
        """
        value = 0
        for key in last_values:
            if key in settings.car_status:
                v = settings.car_status[key]
                if last_values[key] != v:
                    message[key] = v
                    last_values[key] = v
        if settings.GEAR in message:
            if message[settings.GEAR] == 0:
                message[settings.GEAR] = "N"

        yield from websocket.send(json.dumps(message))
        message = {}
