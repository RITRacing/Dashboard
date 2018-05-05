#include <armadillo>
using namespace arma;
class kalman_filter{
private:
    mat state;
    mat covar;
public:
    kalman_filter(float init_p, float init_v);
    float get_position();
    float get_velocity();
    void predict(float seconds);
    void update(float pos, float vel);
};
