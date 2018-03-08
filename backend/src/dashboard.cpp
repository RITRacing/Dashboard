#include "dashboard.h"
#include "dash_model.h"
//#include "shift_controller.h" // TODO
#include "informer.h"
#include <iostream>
#include <csignal>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>

using namespace std;

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
	//shift_controller shiftc;
	//shiftc.begin(); // spawns shift thread
	dash_model model(8787); // create model, waits for server to connect

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
	while((c = getopt(argc, argv, "m:")) != -1){
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
		default:
			cout << USAGE_STRING;
			return EXIT_FAILURE;
		}
	}

	initialize(mode, filename);
    return EXIT_SUCCESS;
}
