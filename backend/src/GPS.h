#ifndef GPS_H
#define GPS_H
#include <mutex>
#include <map>
using namespace std;

// different keys for the returned map
#define LATITUDE "LATITUDE"
#define LONGITUDE "LONGITUDE"
#define VELOCITY "VELOCITY"
#define ANGLE "ANGLE"

/**
* Encapsulates GPS serial reading
**/
class GPS{
private:
    mutex mx; // prevents serial reading errors
    int fd; // file descriptor for GPS socket
    map<string,float> current; // map containing current values from GPS
public:
    GPS(); // constructor
    void read_sentence(); // reads a GPRMC sentence
    map<string, float> get_current(); // returns the latest GPRMC data
};

extern void * gps_routine(void * p); // loops sending GPS data to CAN

#endif
