#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <string>
using namespace std;

#define USAGE_STRING "Usage: ./dashboard -m <op_mode>\n -f <testfile> (if op_mode testdata)\n";

// the port to host the socket server on
#define PORT 8787

// enum for different modes of operation
enum op_mode{
	vehicle, // reading from can_bus
	testdata, // reading from fomatted .csv file
	user // reading from stdin
};

/**
* The following are keys for values stored in the JSON
* the frontend expects
**/

// c car
#define RPM "RPM"
#define OILT "OILT"
#define WATERT "WATERT"
#define OILP "OILP"
#define GEAR "GEAR"
#define SPEED "SPEED"
#define BATT "BATT"
#define AIRT "AIRT"
#define AUTOUP "AUTOUP"
#define LAMBDACTL "LAMBDACTL"

// e car
#define CURRENT "CURRENT"
#define LFAULT "LFAULT"
#define SOC "SOC"

/**
* The following are CAN ids that devices on the bus send
**/

// Engine Control Unit (c car)
#define ECU_PRIM_ID 0x00FF
#define ECU_SEC_ID 0x00FC

// Tire Temp sensors
#define TT_FRONT_ID 0x03F2
#define TT_REAR_ID 0x03F4

// Battery Management System (e car)
#define BMS_PRIM_ID 0x0626
#define BMS_SEC_ID 0x0624

// Level Fault Indication Bits (e car)
#define BMS_FLAGS_ID 0x0622

/**
* Enum of level faults used to quickly determine fault from 8 bit value
**/
static string lfaults[8] = {"plug",
					"intl",
					"comm",
					"cocurr",
					"docurr",
					"temp",
					"uvolt",
					"ovolt"};



#endif
