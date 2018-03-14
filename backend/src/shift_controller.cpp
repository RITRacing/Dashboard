#include "shift_controller.h"
#include "wiringPi.h"
#include <iostream>
#include <pthread.h>
using namespace std;

mutex shift_controller::mx;
mutex shift_controller::automx;

shift_controller::shift_output::shift_output(int l, int o){
    // initialize fields
    listen_pin = l;
    output_pin = o;

    // setup GPIO registers
    pinMode(o, OUTPUT);
    pinMode(l, INPUT);
}

bool shift_controller::shift_output::pressed(){
    return !digitalRead(listen_pin);
}

void shift_controller::shift_output::shift(){
    digitalWrite(output_pin, HIGH);
    SLEEP(SHIFT_HOLD);
    digitalWrite(output_pin, LOW);
}

shift_controller::shift_controller(dash_model * m, int uplisten, int upout,
    int downlisten, int downout){
    model = m;
    just_changed = false;
    autoup_status = false;
    current_bounce = millis();
    upshifter = new shift_output(uplisten, upout);
    downshifter = new shift_output(downlisten, downout);
    // listen for interrupt
    wiringPiISR(uplisten, INT_EDGE_FALLING, &paddle_callback);
    wiringPiISR(downlisten, INT_EDGE_FALLING, &paddle_callback);
}

void shift_controller::attempt_shift(bool up){
    shift_controller::mx.lock();
    if(true){//millis() - current_bounce >= BOUNCE_TIME){
        int gear = model->gear();
        if(up && gear < MAX_GEAR){
            upshifter->shift();
            //current_bounce = millis();
            // TODO remove these, replace with conditional using static model
            model->set(GEAR, to_string(++gear));
        }else if(!up && gear > 0){
            if(gear == 1 && (model->speed() < SPEED_LOCKOUT || gear > 1)){
                downshifter->shift();
                //current_bounce = millis();
                model->set(GEAR, to_string(--gear));
            }
        }
    }
    shift_controller::mx.unlock();
}

bool shift_controller::is_autoup(){
    return autoup_status;
}

void shift_controller::set_autoup(bool a){
    autoup_status = a;
    model->set(AUTOUP, autoup_status ? "true" : "");
}

void shift_controller::shift_status(int gear){
    // TODO - update model and implement shift message thread
}

void paddle_callback(){
    if(millis() - current_bounce >= BOUNCE_TIME){
        cout << "new thread" << endl;
        pthread_t thread;
        pthread_create(&thread, NULL, trigger_shift, (void*)0);
        pthread_detach(thread);
        current_bounce = millis();
    }
}

//mutex autom;
void * trigger_shift(void* p){
    SLEEP(PADDLE_HOLD);
    bool upon = shiftc->upshifter->pressed();
    bool downon = shiftc->downshifter->pressed();
    if(upon && downon){
            SLEEP(AUTOUP_HOLD);
            shift_controller::automx.lock();
            just_changed = !just_changed;
            if(shiftc->upshifter->pressed() && shiftc->downshifter->pressed()
                && !just_changed){
                cout << "AUTO: " << shiftc->is_autoup() << endl;
                shiftc->set_autoup(!shiftc->is_autoup());
            }
            shift_controller::automx.unlock();
    }else if(upon){
        cout << "UP" << endl;
        shiftc->attempt_shift(UP);
    }else if(downon){
        cout << "DOWN" << endl;
        shiftc->attempt_shift(DOWN);
    }
    return NULL;
}
