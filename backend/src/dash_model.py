import settings
import dash_log
import websockets
import json
import asyncio



@asyncio.coroutine
def update_frontend(websocket, path):
    # set up some variables to hold current values to avoid sending duplicates
    oilt = 0
    oilp = 0
    watert = 0
    batt = 0
    rpm = 0
    gear = 0;

    connection_name = yield from websocket.recv()
    dash_log.print_msg("STARTUP","connected to: " + connection_name)
    message = {}

    while True:
        if "RPM" in settings.car_status and settings.car_status["RPM"] != rpm:
            message["RPM"] = settings.car_status["RPM"]
            rpm = settings.car_status["RPM"]

        if "OILT" in settings.car_status and settings.car_status["OILT"] != oilt:
            message["OILT"] = settings.car_status["OILT"]
            oilt = settings.car_status["OILT"]

        if "OILP" in settings.car_status and settings.car_status["OILP"] != oilp:
            message["OILP"] = settings.car_status["OILP"]
            oilp = settings.car_status["OILP"]
        if "WATERT" in settings.car_status and settings.car_status["WATERT"] != watert:
            message["WATERT"] = settings.car_status["WATERT"]
            watert = settings.car_status["WATERT"]
        if "BATT" in settings.car_status and settings.car_status["BATT"] != batt:
            message["BATT"] = settings.car_status["BATT"]
            batt = settings.car_status["BATT"]
        """
        if packSOC.barName in settings.car_status and settings.car_status[packSOC.barName] != packSOC.value:
            packSOC.updateBar(settings.car_status[packSOC.barName])

        if packTemp.barName in settings.car_status and settings.car_status[packTemp.barName] != packTemp.value:
            packTemp.updateBar(settings.car_status[packTemp.barName])

        if "ANG_V" in settings.car_status:
            if settings.car_status["ANG_V"] != prev_V:
                messageList.append({"ANG_V": settings.car_status["ANG_V"]})
                prev_V = settings.car_status["ANG_V"]
        """

        if "GEAR" in settings.car_status:
            if settings.car_status["GEAR"] != gear:
                if settings.car_status["GEAR"] == 0:
                    message["GEAR"] = "N"
                else:
                    message["GEAR"] = settings.car_status["GEAR"]

                gear = settings.car_status["GEAR"]

        """
        if "DPAD" in settings.car_status:
            if settings.car_status["DPAD"] != "":
                messageList.append({"DPAD": settings.car_status["DPAD"]})
        """
        yield from websocket.send(json.dumps(message))
        message = {}