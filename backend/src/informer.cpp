#include "informer.h"
#include "dashboard.h"
#include "dash_model.h"
#include <pthread.h>
using namespace std;

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

void informer::connect(dash_model *ml){
    model = ml;
}

void informer::begin(){
    shouldContinue = true;
    uint8_t timer = 0;
    while(shouldContinue){
        gather(); // get the info
        model->update_frontend(); // send the info
        if(timer % 10){
            model->update_ground_station();
            timer &= 0;
        }
        //gps->read_sentence();
        //gps->get_current();
        ++timer;
    }
}

void informer::finish(){
    shouldContinue = false;
}
