#include "informer.h"
#include "dashboard.h"
#include "dash_model.h"
#include <pthread.h>
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
    while(shouldContinue){
        gather(); // get the info
        model->update_frontend(); // send the info
        if(timer % 10){
            model->update_ground_station();
            timer &= 0;
        }
        ++timer;
    }
}

/**
* Stops the loop for clean program shutdown.
**/
void informer::finish(){
    shouldContinue = false;
}
