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
#include "wiringPi.h"

using namespace std;

shift_controller * shiftc;

informer * inf; // the object that collects data and sends to dash
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

	dash_model model(PORT); // create model, waits for server to connect
    cout << "about to make sc" << endl;
    shiftc = new shift_controller(&model, UP_LISTEN, UP_OUT, DOWN_LISTEN, DOWN_OUT);
    cout << "made sc" << endl;
    //shiftc.begin(); // spawns shift thread


	inf = informer::get_informer(mode, filename);
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
    // TODO implement filename reading
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
