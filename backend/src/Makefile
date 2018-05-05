dashboard : dashboard.cpp dash_model.cpp informer.cpp input_reader.cpp CAN.o can_reader.cpp test_reader.cpp shift_controller.cpp libgps.a MPU6050.o I2Cdev.o
	g++  $^ -o dashboard -std=c++11 -lpthread -lsystemd -lwiringPi

clean :
	rm -f *.gch
