#include "informer.h"
#include "dashboard.h"
#include "dash_model.h"
using namespace std;

informer * informer::get_informer(op_mode mode, string filename){
    switch(mode){
        case vehicle:
            return new can_reader();
        case testdata: //TODO
        case user:
            return new input_reader();
    }
}

void informer::connect(dash_model *ml){
    model = ml;
}

void informer::begin(){
    shouldContinue = true;
    while(shouldContinue){
        gather(); // get the info
        model->update_frontend(); // send the info
    }
}

void informer::finish(){
    shouldContinue = false;
}
