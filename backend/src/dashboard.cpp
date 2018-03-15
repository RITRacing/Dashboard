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
void sigint_handle(int signal){
    inf->finish();
}

void ecu_up(){
	msgmx.lock();
	shiftmsg[0] |= 0x02;
	msgmx.unlock();
}

void ecu_down(){
	msgmx.lock();
	shiftmsg[0] |= 0x01;
	msgmx.unlock();
}

void * alert_ecu(void * p){
    CAN can = *((CAN*)p);
    uint8_t count = 0;
    while(true){
        msgmx.lock();
        can.write_msg(SHIFT_MSG_ID, shiftmsg);
        if(shiftmsg[0] != 0x00){
            ++count;
        }
        if(count == 50){
            count = 0;
            shiftmsg[0] = 0x00;
        }
        msgmx.unlock();
        usleep(1000);
    }
}

void * user_mode_routine(void * p){
    dash_model model = *((dash_model*)p);
    while(true){
        SLEEP(.2);
        model.update_frontend();
    }
}

pthread_t shift_thread;
/**
 * Define shift GPIO interrupts
 * Start autoup listen thread
 * Create model, create informer and start it
 **/
void initialize(op_mode mode, string filename){
    wiringPiSetupGpio();

	dash_model model(PORT); // create model, waits for server to connect

    CAN * can = new CAN();

    //pthread_t shift_thread;
    pthread_create(&shift_thread, NULL, alert_ecu, (void*)can);
    pthread_detach(shift_thread);
    shiftc = new shift_controller(&model, UP_LISTEN, UP_OUT,
        DOWN_LISTEN, DOWN_OUT);

	inf = informer::get_informer(mode, filename, can);
	inf->connect(&model);
    /*
    if(mode == user){
        pthread_t userthread;
        pthread_create(&userthread, NULL, user_mode_routine, &model);
        pthread_detach(userthread);
    }
    */
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
