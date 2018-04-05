#include "dashboard.h"
#include "informer.h"
#include "CAN.h"
#include <string>
#include <iostream>
using namespace std;

/**
* Set the can_reader's pointer to CAN
* @param c: pointer to shared CAN
**/
can_reader::can_reader(CAN * c){
    can = c;
}

/**
* Implements true virtual Informer.gather.
* Reads a CAN message from the bus and sets the model according to what it read.
**/
void can_reader::gather(){
    char msg[8];
    uint32_t id = can->read_msg(msg);
    switch(id){
        case ECU_PRIM_ID:
            model->set(RPM, to_string((msg[0] << 8) | msg[1]));
            model->set(OILT, to_string((uint8_t)msg[2]));
            model->set(WATERT, to_string((uint8_t)msg[3]));
            model->set(OILP, to_string((uint8_t)msg[4]/10));
            model->set(GEAR, "1");//to_string((uint8_t)msg[5]));
            model->set(SPEED, to_string(((((uint16_t)msg[6]) << 8)
                | msg[7])/10));
            break;
        case ECU_SEC_ID:
            model->set(BATT, to_string(((float)((msg[0] << 8) | msg[1]))/1000));
            break;
        case BMS_PRIM_ID:
            model->set(SOC, to_string((uint8_t)msg[0]));
            break;
        case BMS_SEC_ID:
            model->set(CURRENT, to_string((msg[0] << 8) | msg[1]));
            break;
        case BMS_FLAGS_ID:
            set_flags(msg[5]);
            break;
        case MCS_INTERNAL_STATE_ID:
            model->set(MCS, mc_states[msg[0]]);
            break;
        // TODO write telemetry data case(s)
    }
}

/**
* Helper function that reads individual bits from e car level fault byte
**/
void can_reader::set_flags(uint8_t flagbyte){
    uint8_t b = 1;
    for(int i = 0; i < 8; ++i){
    if((b << i) & flagbyte)
        model->set(LFAULT, lfaults[i]);
    }
}
