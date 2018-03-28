#include "dashboard.h"
#include "dash_model.h"
#include "shift_controller.h"
#include "informer.h"
#include <iostream>
#include <csignal>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <thread>
#include "wiringPi.h"
#include "GPS.h"
using namespace std;

informer * inf; // the object that collects data and sends to dash
shift_controller * shiftc;

/**
* Called on SIGINT, ends program
**/
void sigint_handle(int signal){
    inf->finish();
}

void * gps_routine(void * p){
    CAN* can = (CAN*) p;
    GPS gps;
    while(1){
        gps.read_sentence();
        map<string, float> current = gps.get_current();
        uint32_t lat_thou = current[LATITUDE] / .001;
        uint32_t long_thou = current[LONGITUDE] / .001;
        uint16_t vel = current[VELOCITY] / .1;
        uint16_t angle = current[ANGLE] / .1;

        char msg1[8] = {
            lat_thou >> 16,
            (lat_thou & 0x0000FF00) >> 8,
            lat_thou & 0xFF,
            long_thou >> 16,
            (long_thou & 0x0000FF00) >> 8,
            long_thou & 0xFF,
            vel >> 8,
            vel & 0xFF
        };

        char msg2[8] = {angle >> 8, angle & 0xFF,
            0x00,0x00,0x00,0x00,0x00,0x00};

        can->write_msg(GPS_ID_1, msg1);
        can->write_msg(GPS_ID_2, msg2);
        SLEEP(.001);
    }
}

/**
 * Define shift GPIO interrupts
 * Start autoup listen thread
 * Create model, create informer and start it
 **/
void initialize(op_mode mode, string filename){
    wiringPiSetupGpio();

	dash_model model(PORT); // create model, waits for server to connect

    CAN * can = new CAN();

    pthread_t gpsthread;
    pthread_create(&gpsthread, NULL, gps_routine, can);
    shiftc = new shift_controller(&model, can, UP_LISTEN, DOWN_LISTEN);

	inf = informer::get_informer(mode, filename, can);
	inf->connect(&model);
	inf->begin(); // loops reading and sending info
}

/**
* Get the command line arguments and initialize
**/
int main(int argc, char** argv){
	// set up the signal handler
   	signal(SIGINT, sigint_handle);

   	// get command line arguments
	op_mode mode; // mode of operation
	string filename; // name of file (if testdata)
	int c;
	while((c = getopt(argc, argv, "m:f:")) != -1){
		switch(c){
		case 'm':
			{
			string soptarg = string(optarg);
			if(!soptarg.compare("vehicle")){
				mode = vehicle;
			}else if(!soptarg.compare("testdata")){
				mode = testdata;
			}else if(!soptarg.compare("user")){
				mode = user;
			}else{
				cout << "Invalid mode specified\n"
				"modes: vehicle, testdata, user" << endl;
				return EXIT_FAILURE;
			}

			}
			break;

        case 'f':
            filename = string(optarg);
            break;

		default:
			cout << USAGE_STRING;
			return EXIT_FAILURE;
		}
	}

	initialize(mode, filename);
    return EXIT_SUCCESS;
}
