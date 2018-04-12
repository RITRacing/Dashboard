#include "kalman_filter.h"
#include <armadillo>
using namespace arma;

/**
* Initialize the filter with initial state (meters, m/s)
**/
kalman_filter::kalman_filter(float pi, float vi){
    state.pos = pi;
    state.vel = vi;
    state.pos_covar = .001;
    state.vel_covar = .5;
    time(&last_measure);
}

float kalman_filter::get_position(){
    return state.pos;
}

float kalman_filter::get_velocity(){
    return state.vel;
}

/**
* Returns the purely predicted state
* @param control: acceleration of car in this filter's dimension (m/s^2)
**/
pos_t kalman_filter::predict(float control, double elapsed){
    pos_t pred;
    time_t prev = last_measure;
    time(&last_measure);
    //double elapsed = last_measure - prev;
    pred.pos = state.pos + state.vel * elapsed + .5*control*(elapsed * elapsed);
    pred.vel = state.vel + control * elapsed;
    pred.elapsed = elapsed;
    // should factor in noise here for more accuracy
    return pred;
}

void kalman_filter::correct(pos_t pred, float pos_sense, float vel_sense){
    // calculate the covariances
    //float cur_vel_covar = GPS_STDEV * GPS_STDEV * 2 /
    //    (state.elapsed * state.elapsed);
    pred.vel_covar = pred.vel * pred.vel_covar * pred.vel + GPS_STDEV * GPS_STDEV;
    pred.pos_covar = pred.pos * pred.pos_covar * pred.pos + GPS_STDEV * GPS_STDEV;
    //pred.vel_covar = state.vel_covar;
    //pred.pos_covar = state.pos_covar;

    // calculate kalman gain
    pred.pos_gain = pred.pos_covar*pos_sense /
        (pos_sense*state.pos_covar*pos_sense);//+GPS_ACCURACY);
    // probably should use accuracy here instead of stdev...
    // using 2 stdev for more coverage of range
    pred.vel_gain = pred.vel_covar*vel_sense /
        (vel_sense*pred.vel_covar*vel_sense);//+ACC_STDEV * 2);

    // get expected mean (z subscript k), could input more noise stuff here
    float pos_z = pos_sense * state.pos;
    float vel_z = vel_sense * state.vel;

    // finally, update the state
    pred.pos = pred.pos + pred.pos_gain*(pos_z-pos_sense*pred.pos);
    pred.vel = pred.vel + pred.vel_gain*(vel_z-vel_sense*pred.vel);

    // update the covariances
    pred.pos_covar = pred.pos_covar-pred.pos_gain*pos_sense*pred.pos_covar;
    pred.vel_covar = pred.vel_covar-pred.vel_gain*vel_sense*pred.vel_covar;
    state = pred;
}
