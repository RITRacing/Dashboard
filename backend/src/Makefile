dashboard : dashboard.cpp dashboard.h dash_model.cpp dash_model.h informer.h informer.cpp input_reader.cpp CAN.o CAN.h can_reader.cpp test_reader.cpp libwiringPi.so libwiringPiDev.so wiringPi.h shift_controller.h shift_controller.cpp libgps.a MPU6050.o I2Cdev.o
	g++  $^ -o dashboard -std=c++11 -lpthread -ggdb -Iinclude -lsystemd
clean :
	rm -f *.gch
