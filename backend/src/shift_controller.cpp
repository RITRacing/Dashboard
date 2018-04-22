#include "shift_controller.h"
#include "wiringPi.h"
#include "dashboard.h"
#include "CAN.h"
#include <iostream>
#include <pthread.h>
using namespace std;

/**
* Constantly checks for RPM threshold and if autoup enabled, auto upshifts
**/
void * autup_routine(void*p){
    while(true){
        SLEEP(.18);
        if(shiftc->auto_should_shift()){
            shiftc->shift(UP);
        }
    }
}

/**
* Initialize fields, spin up shift message and autoup threads
**/
shift_controller::shift_controller(dash_model * m, CAN * c, int upl, int downl,
    int upo, int downo){
    // initialize functions
    model = m;
    can = c;
    up_listen = upl;
    down_listen = downl;
    autoup_status = false;

    // create racepack
    pack = new racepack(upo, downo);

    // start message and auto-up threads
    pthread_t msg_thread, auto_thread;
    pthread_create(&msg_thread, NULL, message_routine, NULL);
    pthread_detach(msg_thread);

    pthread_create(&auto_thread, NULL, autup_routine, NULL);
    pthread_detach(auto_thread);

    // set up outputs
    //pinMode(UP_OUT, OUTPUT); pinMode(DOWN_OUT, OUTPUT);

    // set up paddle interrupts
    pinMode(up_listen, INPUT); pinMode(down_listen, INPUT);
    pullUpDnControl(up_listen, PUD_UP);
    pullUpDnControl(down_listen, PUD_UP);
    wiringPiISR(up_listen, INT_EDGE_FALLING, &paddle_callback);
    wiringPiISR(down_listen, INT_EDGE_FALLING, &paddle_callback);
}

/**
* Executes a shift by sending the ECU shift messages
**/
void shift_controller::shift(bool up){
    mx.lock();
    if(up){
        if(true){//model->gear() < MAX_GEAR){
            cout << "upshifting" << endl;
            msgmx.lock();
            shift_msg[0] = UPSHIFT_MSG;
            //char rpmsg[8] = {1,0,0,0,0,0,0,0};
            //can->write_msg(0x456, rpmsg);
            //digitalWrite(UP_OUT, HIGH); // write to racepack
            pack->start_upshift();
            msgmx.unlock();
        }
    }else{
        if(true){//)(model->gear() == 1 && //model->speed() < SPEED_LOCKOUT) ||
            //model->gear() > 1){
                msgmx.lock();
                shift_msg[0] = DOWNSHIFT_MSG;
                //char rpmsg[8] = {0,0,1,0,0,0,0,0};
                //can->write_msg(0x456, rpmsg);
                //digitalWrite(DOWN_OUT, HIGH);
                pack->start_downshift();
                msgmx.unlock();
        }
    }
    mx.unlock();
}

/**
* Get if autoup is enabled or not
**/
bool shift_controller::is_autoup(){
    return autoup_status;
}

/**
* Turn autoup on or off
**/
void shift_controller::set_autoup(bool u){
    autoup_status = u;
    model->set(AUTOUP, u ? "true" : "");
}

/**
* Get if a paddle is pressed or not
**/
bool shift_controller::pressed(bool up){
    if(up)
        return !digitalRead(up_listen);
    return !digitalRead(down_listen);
}

/**
* Get if autoup is enabled and RPM is above threshold
**/
bool shift_controller::auto_should_shift(){
    int rpm = model->rpm();
    int gear = model->gear();
    return autoup_status && rpm >= AUTOUP_TRIGGER && gear < MAX_GEAR;
    //return autoup_status;
}

/**
* Send a message indicating shift intent to the ECU
**/
void shift_controller::send_ecu_msg(){
    can->write_msg(SHIFT_MSG_ID, shift_msg);
    if(shift_msg[0] != NOSHIFT_MSG){
        ++count;
    }
    if(count == SHIFT_MSG_COUNT){
        //digitalWrite(UP_OUT, LOW);
        //digitalWrite(DOWN_OUT, LOW);
        pack->stop_shifting();
        count = 0;
        shift_msg[0] = NOSHIFT_MSG;
    }
}

/**
* Determine which kindof shift is happenening and execute it
**/
void * trigger_shift(void* p){
    SLEEP(.05);
    bool upon = shiftc->pressed(UP);
    bool downon = shiftc->pressed(DOWN);
    cout << "paddle callback" << endl;
    // check if both are down, then check upshifter, then downshifter
    if(upon && downon){
            SLEEP(AUTOUP_HOLD);
            // two threads will always spawn b/c of 2 paddles.
            // automx keeps the second from turning off autoup
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

/**
* Routine called whenver a paddle-triggered interrupt is received
**/
void paddle_callback(){
    if(millis() - bounce >= BOUNCE_TIME){
        pthread_t thread;
        pthread_create(&thread, NULL, trigger_shift, (void*)0);
        pthread_detach(thread);
        bounce = millis();
    }
}

/**
* Sends a message to the ECU indicating shift intent every millisecond
**/
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

/**
* Set up pins
**/
racepack::racepack(int up, int dn){
    up_pin = up;
    down_pin = dn;

    pinMode(up_pin, OUTPUT); pinMode(down_pin, OUTPUT);
    //digitalWrite(up_pin, LOW); digitalWrite(down_pin, LOW);
    stop_shifting();
}

/**
* Set both outputs to LOW
**/
void racepack::stop_shifting(){
    digitalWrite(up_pin, HIGH);
    digitalWrite(down_pin, HIGH);
}

/**
* Turn on the upshift output
**/
void racepack::start_upshift(){
    stop_shifting();
    digitalWrite(up_pin, LOW);
}

/**
* Turn on the downshift output
**/
void racepack::start_downshift(){
    stop_shifting();
    digitalWrite(down_pin, LOW);
}
