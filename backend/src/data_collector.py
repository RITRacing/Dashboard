import time, sys, socket, settings, can_controller

def main():
    filename = sys.argv[1]
    can_socket = socket.socket(socket.AF_CAN, socket.SOCK_RAW, socket.CAN_RAW)
    can_socket.bind(("can0",))
    with open(filename, "w+") as file:
        while True:
            # get time before next message
            start_time = time.time()

            """ read the incoming ECU data """
            sender_data = bytes([])
            cf, addr = can_socket.recvfrom(settings.can_channel)
            sender_id, sender_length, sender_data = can_controller.dissect_can_frame(cf)

            # only read messages in settings.controller_id
            while sender_id not in settings.controller_id.values():
                sender_data = bytes([])
                cf, addr = can_socket.recvfrom(settings.can_channel)
                sender_id, sender_length, sender_data = can_controller.dissect_can_frame(cf)

            time_stamp = time.time() - start_time
            file.write("wait," + str(time_stamp) + "\n")

            # create values to write to the file
            rpm = 0
            oilt = 0
            watert = 0
            oilp = 0
            gear = 0
            speed = 0
            batt = 0
            airt = 0

            if sender_id == settings.controller_id["ECU_IDprim"]:
                rpm = int((sender_data[0] << 8) + sender_data[1])
                # print ("RPM is currently: " + str(settings.car_status["RPM"]))
                oilt = int(sender_data[2])
                # print ("OILT is currently: " + str(settings.car_status["OILT"]))
                watert = int(sender_data[3])
                # print ("WATERT is currently: " + str(settings.car_status["WATERT"]))
                oilp = int(sender_data[4]) / 10
                # print ("OILP is currently: "  + str(settings.car_status["OILP"]))
                gear = int(sender_data[5])
                speed = int(((sender_data[6] << 8) + sender_data[7]) / 100)

                # write to file
                file.write("ECU_IDprim," + str(rpm) + "," + str(oilt) + "," + str(watert) + "," + str(oilp) + "," + str(gear) + "," + str(speed) + "\n")

            elif sender_id == settings.controller_id["ECU_IDsec"]:
                batt = int((sender_data[0] << 8) + sender_data[1]) / 1000
                airt = int(sender_data[2] + sender_data[3])

                # write to file
                file.write("ECU_IDsec," + str(batt) + "," + str(airt) + "\n")

if __name__ == "__main__":
    main()
