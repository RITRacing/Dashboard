#include "informer.h"
#include "dashboard.h"
#include "dash_model.h"
#include "gps.h"
#include "MPU6050.h"
#include <pthread.h>
#include <stdio.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <systemd/sd-daemon.h>

using namespace std;

/**
* Get the oppropriate informer sub-type based on the op_mode
* @param mode: the operation mode
* @param filename: filename if using test data
* @param can: pointer to CAN object used by whole program (uses can0)
* @return: the new informer object
**/
informer * informer::get_informer(op_mode mode, string filename, CAN * can){
    switch(mode){
        case vehicle:
            return new can_reader(can);
        case testdata:
            return new test_reader(filename);
        case user:
            return new input_reader();
    }
}

/**
* "Connect" the model to the frontend
* @param model: pointer to model
**/
void informer::connect(dash_model *ml){
    model = ml;
}

/**
* Main loop of informer. Get the information, send it where it needs to go.
**/
void informer::loop(){
    shouldContinue = true;
    
    #ifdef ENABLE_TELEMETRY
    uint8_t timer = 0;
    #endif

    while(shouldContinue){
        gather(); // get the info
        model->update_frontend(); // send the info
        #ifdef ENABLE_TELEMETRY
        if(timer == 100){
            model->update_ground_station();
            timer = 0;
        }
        ++timer;
        #endif
    }
}

/**
* Stops the loop for clean program shutdown.
**/
void informer::finish(){
    shouldContinue = false;
}

/**
* Loops, reading latitude and longitude from GPS and sending over CAN
**/
void * gps_routine(void * p){
    // generate file name
    auto t = time(nullptr);
    auto tm = localtime(&t);
    string filename;
    stringstream ss;
    ss << put_time(tm, "%d-%m-%Y_%H-%M");
    filename = ss.str();
    FILE * fileout = fopen(filename.c_str(), "w");
    gps_init();

    loc_t current;
    while(1){
        gps_location(&current);
        fprintf(fileout, "%f, %f, %f, %f", current.latitude, current.longitude,
            current.speed, current.course);
        SLEEP(.1);
    }

}
