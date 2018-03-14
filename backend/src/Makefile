dashboard : dashboard.cpp dashboard.h dash_model.cpp dash_model.h informer.h informer.cpp input_reader.cpp CAN.o CAN.h can_reader.cpp test_reader.cpp libwiringPi.so libwiringPi.so.2.44 libwiringPiDev.so libwiringPiDev.so.2.44 wiringPi.h shift_controller.h shift_controller.cpp
	/bin/armv8-rpi3-linux-gnueabihf/bin/armv8-rpi3-linux-gnueabihf-g++  $^ -o dashboard -std=c++11 -lpthread -ggdb

CAN.o:
	ls
clean :
	rm -f *.gch
