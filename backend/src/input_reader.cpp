#include "informer.h"
#include "dash_model.h"
#include <iostream>
#include <cstring>
void input_reader::gather(){
    cout << "Enter <key> <value>";
    string in;
    cin >> in;
    char * c = strdup(in.c_str());
    string key = string(strtok(c, " "));
    string value = c;
    model->set(key, value);
}
