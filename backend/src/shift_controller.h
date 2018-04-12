#ifndef SHIFT_CONTROLLER_H
#define SHIFT_CONTROLLER_H
#include <mutex>
#include "dashboard.h"
#include "dash_model.h"
#include "CAN.h"

#define PADDLE_HOLD 0.15 // sec
#define AUTOUP_HOLD 2 // sec
#define AUTOUP_TRIGGER 10500 // RPM
#define UP true
#define DOWN false
#define BOUNCE_TIME 200 // milliseconds
#define MAX_GEAR 4
#define SPEED_LOCKOUT 20 // kph
#define SHIFT_HOLD 0.2 // sec

// paddle pins
#define UP_LISTEN 4
#define DOWN_LISTEN 5

// output pins
#define UP_OUT 2
#define DOWN_OUT 3

// shift intent messages for greedy ECU
#define UPSHIFT_MSG 0x02
#define DOWNSHIFT_MSG 0x01
#define NOSHIFT_MSG 0x00

// number of up or down intent messages to feed greedy ECU per shift
#define SHIFT_MSG_COUNT 10

using namespace std;

/**
* Encapsulates control to the racepack
**/
class racepack{
private:
    // output pins
    int up_pin;
    int down_pin;
public:
    racepack(int up, int dn);
    void start_upshift();
    void start_downshift();
    void stop_shifting();
};

/**
* Encapsulates access to and facilitates shift operations
**/
class shift_controller{
private:
    // message thread constantly sends this to ECU
    char shift_msg[8] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};
    uint8_t up_listen, down_listen; // paddle pins
    dash_model * model;
    bool autoup_status;
    CAN * can;
    int count; // count for message sending routine
    racepack * pack;
public:
    shift_controller(dash_model *m, CAN * c, int upl, int downl, int upout,
        int downout);
    inline void send_ecu_msg();
    void shift(bool up);
    bool is_autoup();
    bool auto_should_shift();
    void set_autoup(bool u);
    bool pressed(bool up);
};

// the global shift_controller
extern shift_controller * shiftc;

static long bounce = BOUNCE_TIME; // current time since last shift attempt
static void paddle_callback();
static mutex mx;
static mutex msgmx;
static mutex automx;
static void * message_routine(void* p);

#endif
