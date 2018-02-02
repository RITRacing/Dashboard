import settings, watchdog
import socket, struct, time, asyncio

""" the socket that talks with the can shield """
can_socket = None

""" the message sent to the ECU """
shift_message = bytearray([
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00
])


def setup_can():
    """ set up CAN shield socket """
    global can_socket
    can_socket = socket.socket(socket.AF_CAN, socket.SOCK_RAW, socket.CAN_RAW)
    can_socket.bind(("can0",))


def construct_can_frame(can_id, data):
    """ creates a CAN message from components """
    length = len(data)
    data = data.ljust(8, b'\x00')
    return struct.pack(settings.can_frame_format, can_id, length, data)


def dissect_can_frame(frame):
    """gets components of CAN message """
    can_id, length, data = struct.unpack(settings.can_frame_format, frame)
    return can_id, length, data[:length]


def output_shift():
    """ executed in a separate thread, constantly tells ECU status of shifter """
    counter = 0
    while True:

        # safely send the CAN message
        settings.shift_mutex.acquire()

        can_socket.send(construct_can_frame(settings.shift_id, shift_message))

        # only send 50 iterations of a shift-in-progress message
        if shift_message[0] != settings.noshift_msg:
            counter += 1

        if counter >= 50:
            counter = 0
            shift_message[0] = settings.noshift_msg

        # update the watchdog timer
        settings.shift_mutex.release()

        watchdog.update()

        time.sleep(settings.can_refresh_rate)


def read_input():
    """ read the incoming ECU data """
    sender_data = bytes([])
    cf, addr = can_socket.recvfrom(settings.can_channel)
    sender_id, sender_length, sender_data = dissect_can_frame(cf)

    # only read messages in settings.controller_id
    while sender_id not in settings.controller_id.values():
        sender_data = bytes([])
        cf, addr = can_socket.recvfrom(settings.can_channel)
        sender_id, sender_length, sender_data = dissect_can_frame(cf)

    # update settings.car_status
    if sender_id == settings.controller_id["ECU_IDprim"]:
        settings.car_status["RPM"] = int((sender_data[0] << 8) + sender_data[1])
        settings.car_status["OILT"] = int(sender_data[2])
        settings.car_status["WATERT"] = int(sender_data[3])
        settings.car_status["OILP"] = int(sender_data[4]) / 10
        settings.car_status["GEAR"] = int(sender_data[5])
        settings.car_status["SPEED"] = int(((sender_data[6] << 8) + sender_data[7]) / 100)
    elif sender_id == settings.controller_id["ECU_IDsec"]:
        settings.car_status["BATT"] = int((sender_data[0] << 8) + sender_data[1]) / 1000
        settings.car_status["AIRT"] = int(sender_data[2] + sender_data[3])


def read_fake_input(line):
    """ read data from a .csv file (for when the ECU isnt available) """

    fields = line.split(',')
    identifier = fields[0]
    if identifier == "ECU_IDprim":
        settings.car_status["RPM"] = int(fields[1])
        settings.car_status["OILT"] = int(fields[2])
        settings.car_status["WATERT"] = int(fields[3])
        settings.car_status["OILP"] = float(fields[4])
        settings.car_status["GEAR"] = int(fields[5])
        settings.car_status["SPEED"] = float(fields[6])
    elif identifier == "ECU_IDsec":
        settings.car_status["BATT"] = float(fields[1])
        settings.car_status["AIRT"] = int(fields[2])
    elif identifier == "wait":
        time.sleep(float(fields[1]))

def read_user_input():
    """ constantly reads user input to send the frontend, for debugging
        ** must be correct values read by dash_model
    """
    line = input("Enter <name> <value>: ")
    fields = line.split(" ")
    name = fields[0]
    value = fields[1]
    settings.car_status[name] = value
