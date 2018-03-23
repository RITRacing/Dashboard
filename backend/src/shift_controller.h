#ifndef SHIFT_CONTROLLER_H
#define SHIFT_CONTROLLER_H
#include <mutex>
#include <unistd.h>
#include "dashboard.h"
#include "dash_model.h"
#include "CAN.h"
// seconds
#define PADDLE_HOLD 0.15
#define AUTOUP_HOLD 2
#define AUTOUP_TRIGGER 10500
#define SLEEP(m) usleep(m * 1000000) // sleep seconds
#define UP true
#define DOWN false
#define BOUNCE_TIME 200 // milliseconds
#define MAX_GEAR 4
#define SPEED_LOCKOUT 20
#define SHIFT_HOLD 0.2

#define UP_LISTEN 4
#define UP_OUT 22
#define DOWN_LISTEN 5
#define DOWN_OUT 6

#define UPSHIFT_MSG 0x02
#define DOWNSHIFT_MSG 0x01
#define NOSHIFT_MSG 0x00

#define SHIFT_MSG_COUNT 50

using namespace std;
class shift_controller{
private:
    char shift_msg[8] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};
    uint8_t up_listen, down_listen;
    dash_model * model;
    bool autoup_status;
    CAN * can;
    int count;
public:
    shift_controller(dash_model *m, CAN * c, int upl, int downl);
    inline void send_ecu_msg();
    inline void reset_msg();
    void shift(bool up);
    bool is_autoup();
    bool auto_should_shift();
    void set_autoup(bool u);
    bool pressed(bool up);
};

extern shift_controller * shiftc;

static long bounce = BOUNCE_TIME;
static void paddle_callback();
static mutex mx;
static mutex msgmx;
static mutex automx;
static void * message_routine(void* p);
/*
class shift_controller{
private:
    class shift_output{
    private:
        int listen_pin, output_pin;
    public:
        shift_output(int l, int o);
        bool pressed();
        void shift();
    };
    bool can_upshift();
    bool can_downshift();
    dash_model * model;
    bool autoup_status;
public:
    shift_controller(dash_model * m, int uplisten, int upout, int downlisten,
        int downout);

    static mutex mx;
    static mutex automx;
    shift_output * upshifter;
    shift_output * downshifter;
    void attempt_shift(bool up);
    bool is_autoup();
    void set_autoup(bool a);

    bool auto_should_shift();
};

static long current_bounce;
static bool just_changed;
extern shift_controller * shiftc;

void * trigger_shift(void* p);
void paddle_callback();

static void * autoup_routine(void * p);
*/
#endif
