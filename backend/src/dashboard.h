#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <string>
using namespace std;

#define USAGE_STRING "Usage: ./dashboard -m <op_mode>\n -f <testfile> (if op_mode testdata)\n";

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





#endif
