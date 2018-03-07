#ifndef DASH_MODEL_H
#define DASH_MODEL_H

#include <map>

class dash_model{
public:
    void set(string key, string value); // set a value in status
    void update_frontend(); // send a json rep. of status to frontend
private:
    map<string, string> status; // holds all CAN values
    
};
