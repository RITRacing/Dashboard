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
using namespace std;

shift_controller * shiftc;
informer * inf; // the object that collects data and sends to dash

/**
* Called on SIGINT, ends program
**/
void sigint_handle(int signal){
    inf->finish();
}

/**
 * Define shift GPIO interrupts
 * Start autoup listen thread
 * Create model, create informer and start it
 **/
void initialize(op_mode mode, string filename){
    wiringPiSetupGpio();
    cout << "Waiting for frontend..." << endl;
	dash_model model(PORT); // create model, waits for server to connect
    cout << "frontend connected" << endl;
    CAN * can = new CAN(); // connect to can0

    // start the GPS thread
    pthread_t gpsthread;
    pthread_create(&gpsthread, NULL, gps_routine, can);

    // create and start the shift_controller
    shiftc = new shift_controller(&model, can, UP_LISTEN, DOWN_LISTEN,
        UP_OUT, DOWN_OUT);

    // set up information gathering, sending
	inf = informer::get_informer(mode, filename, can);
	inf->connect(&model);
	inf->loop(); // loops reading and sending info
}

/**
* Get the command line arguments and initialize
* @param argc: number of CLIs
* @param argv: CLIs
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
