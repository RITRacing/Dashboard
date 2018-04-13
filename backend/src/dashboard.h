#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <string>
#include <mutex>
#include <unistd.h>
using namespace std;

#define USAGE_STRING "Usage: ./dashboard -m <op_mode>\n -f <testfile> (if op_mode testdata)\n";

// very useful macro, unless you think in microseconds.
#define SLEEP(m) usleep(m * 1000000) // sleep seconds

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
#define SPEED "SPEED" // convert to mph for telemetry
#define BATT "BATT"
#define AIRT "AIRT"
#define AUTOUP "AUTOUP"
#define LAMBDACTL "LAMBDACTL"

// e car
#define CURRENT "CURRENT"
#define LFAULT "LFAULT"
#define SOC "SOC"
#define MCS "MCS" // motor controller state

// telemetry
#define BRAKEP "BRAKEP" // integer psi or bar 0 - 3000psi
#define SANGLE "SANGLE" // 1dec deg -90 - 90
#define LTIME "LTIME" // 3 decimal 4s - 5min+
// also uses GEAR

/**
* The following are CAN ids that devices on the bus send
**/

// Engine Control Unit (c car)
#define ECU_PRIM_ID 0x00FF
#define ECU_SEC_ID 0x00FC
// byte 3-4 little end brakeP front/200 bar
// byte 5-6 little end brakeP rear/200 bar
#define ECU_TER_ID 0x00FD // SANGLE LTIME
// 0-1 signed 16bit int
// 2-5 unsigned 32 bit int * 0.1

#define ECU_QUAT_ID 0x00FB

// Tire Temp sensors
#define TT_FRONT_ID 0x03F2
#define TT_REAR_ID 0x03F4

// Battery Management System (e car)
#define BMS_PRIM_ID 0x0626
#define BMS_SEC_ID 0x0624

// Level Fault Indication Bits (e car)
#define BMS_FLAGS_ID 0x0622

// Indication of current motor controller state
#define MCS_INTERNAL_STATE_ID 0x0093

/**
* Array of level faults used to quickly determine fault from 8 bit value
**/
static string lfaults[8] = {"plug",
					"intl",
					"comm",
					"cocurr",
					"docurr",
					"temp",
					"uvolt",
					"ovolt"};

/**
* Array of motor controller states used to quickly determine fault from given
* state code.
**/
static string mc_states[16] = {
	"0 VSM Start",
	"1 Pre-charge Init",
	"2 Pre-charge Active",
	"3 Pre-charge Complete",
	"4 VSM Wait",
	"5 VSM Ready",
	"6 Motor Running",
	"7 Blink Fault Code",
	"8 Invalid State",
	"9 Invalid State",
	"10 Invalid State",
	"11 Invalid State",
	"12 Invalid State",
	"13 Invalid State",
	"14 Shutdown in Process",
	"15 Recycle Power"
};

#define SHIFT_MSG_ID 0x610

#define TELEMETRY_PORT "/dev/ttyUSB0"
#define GPS_PORT "/dev/ttyS0"
#define GPS_ID 0x00AA
#define GPS_ID_SEC 0x00AB

#endif
