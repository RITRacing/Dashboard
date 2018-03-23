#ifndef DASH_MODEL_H
#define DASH_MODEL_H

#include <map>
#include <string>
#include <mutex>
using namespace std;

class dash_model{
public:
    dash_model(int port); // connect to frontend
    void set(string key, string value); // set a value in status
    void update_frontend(); // send a json rep. of status to frontend
    int gear(); // get the gear
    int speed(); // get the speed
    int rpm(); // get the rpm
private:
    map<string, string> status; // holds all current CAN values
    map<string, string> outgoing; // holds the last changed CAN values
    string json_from_map(map<string,string> m); // simple json building
    int frontfd; // frontend socket descriptor
    int telefd; // telemetry socket descriptor
    int times; // (temporary) telemetry counter
};

static mutex modelmx; // mutex that makes for atomic access to model operations

#endif
