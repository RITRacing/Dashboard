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
def set_flags(flagbyte):
    bits = [0,0,0,0,0,0,0,0]
    bit = 1
    for i in range(0,8):
        bits[i] = flagbyte & bit
        bit = bit << 1
    if flagbyte:
        if bits[0]:
            settings.car_status[settings.LFAULT] = "plug"
        if bits[1]:
            settings.car_status[settings.LFAULT] = "intl"
        if bits[2]:
            settings.car_status[settings.LFAULT] = "comm"
        if bits[3]:
            settings.car_status[settings.LFAULT] = "cocurr"
        if bits[4]:
            settings.car_status[settings.LFAULT] = "docurr"
        if bits[5]:
            settings.car_status[settings.LFAULT] = "temp"
        if bits[6]:
            settings.car_status[settings.LFAULT] = "uvolt"
        if bits[7]:
            settings.car_status[settings.LFAULT] = "ovolt"
    else:
        settings.car_status[settings.LFAULT] = ""

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
        settings.car_status[settings.RPM] = int((sender_data[0] << 8) + sender_data[1])
        settings.car_status[settings.OILT] = int(sender_data[2])
        settings.car_status[settings.WATERT] = int(sender_data[3])
        settings.car_status[settings.OILP] = int(sender_data[4]) / 10
        settings.car_status[settings.GEAR] = int(sender_data[5])
        settings.car_status[settings.SPEED] = int(((sender_data[6] << 8) + sender_data[7]) / 100)
    elif sender_id == settings.controller_id["ECU_IDsec"]:
        settings.car_status[settings.BATT] = int((sender_data[0] << 8) + sender_data[1]) / 1000
        settings.car_status[settings.AIRT] = int(sender_data[2] + sender_data[3])
    elif sender_id == settings.controller_id["ECar"]:
        settings.car_status[settings.SOC] = int(sender_data[0])
    elif sender_id == settings.controller_id["ECarSec"]:
        settings.car_status[settings.CURRENT] = int((sender_data[0] << 8) + sender_data[1]);
    elif sender_id == settings.controller_id["EFlags"]:
        set_flags(int(sender_data[5]))


def read_fake_input(line):
    """ read data from a csv file (for when the ECU isnt available) """

    fields = line.split(',')
    identifier = fields[0]
    if identifier == "ECU_IDprim":
        settings.car_status[settings.RPM] = int(fields[1])
        settings.car_status[settings.OILT] = int(fields[2])
        settings.car_status[settings.WATERT] = int(fields[3])
        settings.car_status[settings.OILP] = float(fields[4])
        settings.car_status[settings.GEAR] = int(fields[5])
        settings.car_status[settings.SPEED] = float(fields[6])
    elif identifier == "ECU_IDsec":
        settings.car_status[settings.BATT] = float(fields[1])
        settings.car_status[settings.AIRT] = int(fields[2])
    elif identifier == "ECar":
        settings.car_status[settings.SOC] = int(fields[1])
    elif identifier == "wait":
        time.sleep(float(fields[1]))

def read_user_input():
    """ constantly reads user input to send the frontend, for debugging
        ** must be same keys read by dash_model
    """
    line = input("Enter <name> <value>: ")
    fields = line.split(" ")
    if len(fields) == 2:
        name = fields[0]
        value = fields[1]
    else:
        name = fields[0]
        value = ""
    settings.car_status[name] = value
    print(settings.car_status)
