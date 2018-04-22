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
    uint8_t timer = 0;
    //sd_notify (0, "READY=1"); // tell systemd initialization has finished
    while(shouldContinue){
        //sd_notify (0, "WATCHDOG=1"); // ping systemd
        gather(); // get the info
        model->update_frontend(); // send the info
        /*
        if(timer == 10){
            model->update_ground_station();
            timer = 0;
        }
        ++timer;
        */
    }
}

/**
* Stops the loop for clean program shutdown.
**/
void informer::finish(){
    shouldContinue = false;
}

void * gps_routine(void * p){
    // generate file name
    auto t = time(nullptr);
    auto tm = localtime(&t);
    string filename;
    stringstream ss;
    ss << put_time(tm, "%d-%m-%Y_%H-%M");
    filename = ss.str();
    FILE * fileout = fopen(filename.c_str(), "w");
    //CAN can = *((CAN*)p);
    gps_init();
    //MPU6050 mpu;
    //mpu.initialize();
    //int16_t ax, ay; // acceleration in x and y
    //int16_t az; // z axis, unused
    loc_t current;
    while(1){
        gps_location(&current);
        fprintf(fileout, "%f, %f, %f, %f", current.latitude, current.longitude,
            current.speed, current.course);
        SLEEP(.1);
    }

}
