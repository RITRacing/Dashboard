#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <string>
using namespace std;
string usage_string = "Usage: ./dashboard -m <op_mode>\n"
			"-f <testfile> (if op_mode testdata)\n";

// enum for different modes of operation
enum op_mode{
	vehicle, // reading from can_bus
	testdata, // reading from fomatted .csv file
	user // reading from stdin
};

#endif

