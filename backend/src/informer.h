#ifndef INFORMER_H
#define INFORMER_H
#include <fstream>
#include "dashboard.h"
#include "dash_model.h"
#include "CAN.h"

/**
* Class with pure virtual member that controls the process of getting vehicle
* info and sending it where it needs to go
**/
class informer{
public:
    // returns the appropriate type of informer
    static informer * get_informer(op_mode mode, string filename, CAN * can);
    void connect(dash_model * ml); // give informer model pointer
    void loop(); // loop gathering data, then sending to frontend
    void finish(); // stop looping
protected:
    dash_model * model; // model pointer
    bool shouldContinue; // used to stop loop
private:
    virtual void gather()=0; // how the informer gets its information
};

/**
* Informer that reads from stdin to get values
**/
class input_reader: public informer{
private:
    void gather();
};

/**
* Informer that reads from CAN bus to get values
**/
class can_reader: public informer{
public:
    can_reader(CAN *  can); // initialize with pointer to CAN interface
private:
    void gather();
    void set_flags(uint8_t flagbyte);
    CAN * can;
};

/**
* Informer that reads from a .csv file containing fake CAN readings
**/
class test_reader: public informer{
    public:
         test_reader(string filename);
    private:
        ifstream filein;
        void gather();
};

extern void * gps_routine(void * p);

#endif
