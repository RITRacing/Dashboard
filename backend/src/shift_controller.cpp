#include "shift_controller.h"
#include "wiringPi.h"
#include "dashboard.h"
#include <iostream>
#include <pthread.h>
using namespace std;

void * autup_routine(void*p){
    while(true){
        SLEEP(.18);
        if(shiftc->auto_should_shift()){
            shiftc->shift(UP);
        }
    }
}

shift_controller::shift_controller(dash_model * m, CAN * c, int upl, int downl){
    // initialize functions
    model = m;
    can = c;
    up_listen = upl;
    down_listen = downl;
    autoup_status = false;

    // start message and auto-up threads
    pthread_t msg_thread, auto_thread;
    pthread_create(&msg_thread, NULL, message_routine, NULL);
    pthread_detach(msg_thread);

    pthread_create(&auto_thread, NULL, autup_routine, NULL);
    pthread_detach(auto_thread);

    // set up paddle interrupts
    pinMode(up_listen, INPUT); pinMode(down_listen, INPUT);
    wiringPiISR(up_listen, INT_EDGE_FALLING, &paddle_callback);
    wiringPiISR(down_listen, INT_EDGE_FALLING, &paddle_callback);
}

void shift_controller::shift(bool up){
    mx.lock();
    if(up){
        if(model->gear() < MAX_GEAR){
            shift_msg[0] |= UPSHIFT_MSG;
        }
    }else{
        if((model->gear() == 1 && model->speed() < SPEED_LOCKOUT) ||
            model->gear() > 1){
                msgmx.lock(); shift_msg[0] |= DOWNSHIFT_MSG; msgmx.unlock();
        }
    }
    mx.unlock();
}

bool shift_controller::is_autoup(){
    return autoup_status;
}

void shift_controller::set_autoup(bool u){
    autoup_status = u;
    model->set(AUTOUP, u ? "true" : "");
}

bool shift_controller::pressed(bool up){
    if(up)
        return !digitalRead(up_listen);
    return !digitalRead(down_listen);
}

bool shift_controller::auto_should_shift(){
    int rpm = model->rpm();
    return autoup_status && rpm != -1 && rpm >= AUTOUP_TRIGGER;
}

void shift_controller::send_ecu_msg(){
    can->write_msg(SHIFT_MSG_ID, shift_msg);
    if(shift_msg[0] != NOSHIFT_MSG){
        ++count;
    }
    if(count == SHIFT_MSG_COUNT){
        count = 0;
        shift_msg[0] = NOSHIFT_MSG;
    }
}

static bool just_changed;

void * trigger_shift(void* p){
    bool upon = shiftc->pressed(UP);
    bool downon = shiftc->pressed(DOWN);
    if(upon && downon){
            SLEEP(AUTOUP_HOLD);
            if(automx.try_lock() && shiftc->pressed(UP)
                && shiftc->pressed(DOWN)){
                cout << "AUTO: " << shiftc->is_autoup() << endl;
                shiftc->set_autoup(!shiftc->is_autoup());
            }
            automx.unlock();

    }else if(upon){
        cout << "UP" << endl;
        shiftc->shift(UP);
    }else if(downon){
        cout << "DOWN" << endl;
        shiftc->shift(DOWN);
    }
    return NULL;
}

void paddle_callback(){
    if(millis() - bounce >= BOUNCE_TIME){
        pthread_t thread;
        pthread_create(&thread, NULL, trigger_shift, (void*)0);
        pthread_detach(thread);
        bounce = millis();
    }
}

void * message_routine(void* p){
    uint8_t count = 0;
    while(!shiftc);
    while(true){
        msgmx.lock();
        shiftc->send_ecu_msg();
        msgmx.unlock();
        usleep(1000);
    }
}

/*
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
    bounce = millis();
    upshifter = new shift_output(uplisten, upout);
    downshifter = new shift_output(downlisten, downout);
    // listen for interrupt
    wiringPiISR(uplisten, INT_EDGE_FALLING, &paddle_callback);
    wiringPiISR(downlisten, INT_EDGE_FALLING, &paddle_callback);

    pthread_t autothread;
    pthread_create(&autothread, NULL, autoup_routine, NULL);
    pthread_detach(autothread);
}

void shift_controller::attempt_shift(bool up){
    shift_controller::mx.lock();

        int gear = model->gear();
        if(up && can_upshift()){
            upshifter->shift();
            model->set(GEAR, to_string(++gear));
            ecu_up();
        }else if(!up && gear > 0){
            if(can_downshift()){
                downshifter->shift();
                model->set(GEAR, to_string(--gear));
                ecu_down();
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

void paddle_callback(){
    if(millis() - bounce >= BOUNCE_TIME){
        pthread_t thread;
        pthread_create(&thread, NULL, trigger_shift, (void*)0);
        pthread_detach(thread);
        bounce = millis();
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

bool shift_controller::can_upshift(){
    int gear = model->gear();
    return gear < MAX_GEAR;

}

bool shift_controller::can_downshift(){
    int gear = model->gear();
    return (gear == 1 && model->speed() < SPEED_LOCKOUT) || gear > 1;
}

bool shift_controller::auto_should_shift(){
    int rpm = model->rpm();
    return autoup_status && rpm != -1 && rpm >= AUTOUP_TRIGGER;
}

void * autoup_routine(void * p){
    while(true){
        SLEEP(.1);
        if(shiftc->auto_should_shift()){
            shiftc->attempt_shift(UP);
        }
    }
}
*/
