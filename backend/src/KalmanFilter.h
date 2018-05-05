
class KalmanFilter {
public:
    KalmanFilter(int pi, int vi); // initial position and velocity
private:
    int prev_pos; // previous postion
    int prev_vel; // previous velocity

    int acc_var; // accelerometer variance (stdev^2)
    int gps_varl; // gps variance, look at manual

};
