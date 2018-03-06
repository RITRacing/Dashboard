#include "dashboard.h"
#include <iostream>
#include <csignal>
#include <string>
#include <unistd.h>
#include <stdio.h>
using namespace std;

static bool go = true;

void sigint_handle(int signal){
    go = false;
}

int main(int argc, char** argv){
	// set up the signal handler
   	signal(SIGINT, sigint_handle);
	
   	//get command line arguments
	op_mode mode;	
	int c;
	while((c = getopt(argc, argv, "m:")) != -1){
		printf("%d",c);
		switch(c){
		case 'm':
			{
			string soptarg = string(optarg);
			if(!soptarg.compare("vehicle")){
				mode = vehicle;
			}else if(!soptarg.compare("testdata")){
				mode = testdata;
			}else if(!soptarg.compare("user")){
				mode = user;
			}else{
				cout << "Invalid mode specified\n" 
				"modes: vehicle, testdata, user" << endl;
				return EXIT_FAILURE;
			}
			
			}
			break;
		default:
			cout << usage_string;
			return EXIT_FAILURE;	
		}
	}
    	return EXIT_SUCCESS;
}
