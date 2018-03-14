#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "dashboard.h"
#include "informer.h"
using namespace std;

test_reader::test_reader(string filename){
    filein.open(filename);
    if(!filein.is_open()){
        cout << "Could not open file" << endl;
        exit(EXIT_FAILURE);
    }
}

void test_reader::gather(){
    string line;
    if(getline(filein, line)){
        string id = line.substr(0, line.find(","));
        if(!id.compare("ECU_IDprim")){
            line = line.substr(line.find(",") + 1);
            model->set(RPM, line.substr(0, line.find(",")));
            line = line.substr(line.find(",") + 1);
            model->set(OILT, line.substr(0, line.find(",")));
            line = line.substr(line.find(",") + 1);
            model->set(WATERT, line.substr(0, line.find(",")));
            line = line.substr(line.find(",") + 1);
            model->set(OILP, line.substr(0, line.find(",")));
            line = line.substr(line.find(",") + 1);
            model->set(GEAR, line.substr(0, line.find(",")));
            line = line.substr(line.find(",") + 1);
            model->set(SPEED, line);
        }else if(!id.compare("ECU_IDsec")){
            line = line.substr(line.find(",")+1);
            model->set(BATT, line.substr(0, line.find(",")));
        }else if(!id.compare("wait")){
            usleep(stof(line.substr(line.find(',')+1)) * 1000000);
        }
    }
}
