#ifndef INFORMER_H
#define INFORMER_H
#include <fstream>
#include "dashboard.h"
#include "dash_model.h"
#include "CAN.h"

class informer{
public:
    static informer * get_informer(op_mode mode, string filename, CAN * can);
    void connect(dash_model * ml);
    void begin(); // loop gathering data, then sending to frontend
    void finish(); // stop looping
protected:
    dash_model * model;
    bool shouldContinue;
private:
    virtual void gather()=0;
};

/**
* Informer that reads from stdin to get values
**/
class input_reader: public informer{
private:
    void gather();
};

class can_reader: public informer{
public:
    can_reader(CAN * can);
private:
    void gather();
    void set_flags(uint8_t flagbyte);
    CAN * can;
};

class test_reader: public informer{
    public:
         test_reader(string filename);
    private:
        ifstream filein;
        void gather();
};

#endif
