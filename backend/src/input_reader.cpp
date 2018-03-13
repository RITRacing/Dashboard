#include "informer.h"
#include "dash_model.h"
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <string>

using namespace std;

void input_reader::gather(){
    cout << "Enter <key> <value>";
    string in;
    getline(cin, in);
    string key = in.substr(0, in.find(" "));
    string value = in.substr(in.find(" ") + 1, in.size());
    cout << "KEY: " << key << endl << "VALUE: " << value << endl;
    model->set(key, value);
}
