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
    last_values[settings.CURRENT] = -32000
    last_values[settings.LFAULT] = ""
    last_values[settings.AUTOUP] = 0
    last_values[settings.LAMBDACTL] = 0;
    connection_name = yield from websocket.recv()
    dash_log.print_msg("STARTUP","connected to: " + connection_name)
    message = {}
    while True:
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
