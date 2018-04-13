#include "kalman_filter.h"

kalman_filter::kalman_filter(float init_p, float init_v){
    state = mat(2,1);
    covar = mat(2,2);

    state(0,0) = init_p;
    state(1,0) = init_v;
    covar.fill(0);
    covar(0,0) = .0001;
    covar(1,1) = .5;
}

void kalman_filter::predict(float seconds){
    mat A;
    A << 1 << seconds << endr
      << 0 << 1;
    state = state * A;
    covar = state * covar * state.t();
}

void kalman_filter::update(float pos, float vel){
    mat H;
    H << pos << endr
      << vel;
    mat K = covar * H.t() * 1/(H * covar * H.t());
    mat z = H * state;
    state = state + K * (z-H*state);
    covar = covar -K*H*covar;
}

float kalman_filter::get_position(){
    return state(0,0);
}

float kalman_filter::get_velocity(){
    return state(1,0);
}
