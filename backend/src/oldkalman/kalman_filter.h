#include <time.h>

/**
* Obtained from a field of study in GPS error. Factors in everything but
* Positional Dilution of Precision. PDOP is how the standard deviation of the
* error of receiver position takes into the account how far apart (angularly)
* the Satellites are. The course/acquisition standard deviation is multiplied
* by the PDOP in determining this error. So, since we aren't factoring in the
* positions of the satellites, we are keeping PDOP as 1. This assumes perfect
* satellite positioning.
**/
#define GPS_STDEV 2.4166
#define GPS_ACCURACY 2.6 // meters

/**
* Obtained by an obscure member of an online forum's estimation.
* Will measure sometime...
* https://forum.arduino.cc/index.php?topic=484465.0
**/
#define ACC_STDEV .3 // deg

typedef struct position_data {
    float pos; // position
    float vel; // velocity
    float pos_covar; // position covariance
    float vel_covar; // velocity covariance
    double elapsed; // delta time
    float pos_gain; // positional kalman gain
    float vel_gain; // velocity kalman gain
} pos_t;

class kalman_filter {
public:
    kalman_filter(float pi, float vi); // initial position and velocity
    pos_t predict(float control, double elapsed); // predicts new values based on old ones
    void correct(pos_t pred, float pos, float vel);
    float get_position();
    float get_velocity();
private:
    pos_t state;
    time_t last_measure;
};
