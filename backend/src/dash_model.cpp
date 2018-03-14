#include "dash_model.h"
#include "dashboard.h"
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/tcp.h>

using namespace std;
// TODO protec with mutex
/**
* Establish a connection to the frontend
* @param port: the port to run the server on
**/
dash_model::dash_model(int port){
    // do a bunch of low level socket stuff

    int fd;
    int opt = 1;
    if(!(fd = socket(AF_INET, SOCK_STREAM, 0))){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    // SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT
    if(setsockopt(fd, IPPROTO_TCP,TCP_NODELAY,
        &opt, sizeof(opt))){
            perror("Socket initialization failed");
            exit(EXIT_FAILURE);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if(bind(fd, (struct sockaddr * ) &address, sizeof(address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(fd, 3) < 0){
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // now wait for frontend to connect
    int addrlen = sizeof(address);
    if((frontfd = accept(fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0){
        perror("failed to connect to frontend");
        exit(EXIT_FAILURE);
    }
    if(setsockopt(frontfd, IPPROTO_TCP,TCP_NODELAY,
        &opt, sizeof(opt))){
            perror("Frontend Socket initialization failed");
            exit(EXIT_FAILURE);
    }
}

/**
* If the the value has changed, stage it to be sent
* @param key: the status key
* @param value: the new value
*/
void dash_model::set(string key, string value){
    if(status[key].compare(value))
        status[key] = outgoing[key] = value;
}

/**
* Build a simple json from a map for the frontend
* @param m: the map
**/
string dash_model::json_from_map(map<string,string> m){
    ostringstream ss;
    map<string, string>::iterator itr;
    ss << "{";
    if(m.size() != 0){
        for(itr = m.begin(); itr != m.end(); ++itr){
            string key = itr->first;
            string value = itr->second;
            if(itr != m.begin()) ss<<',';
            ss << "\"" << key << "\":\"" << value << '\"';
        }
        ss << "}@";
    }
    //cout << ss.str() << endl;
    return ss.str();
}

/**
* Send the data in outgoing to frontend in json format
**/
void dash_model::update_frontend(){
    if(outgoing.size() > 0){
        string jstring = json_from_map(outgoing);
        const char * json = jstring.c_str();
        send(frontfd, json, strlen(json),0);
        outgoing.clear();
    }
}

int dash_model::gear(){
    map<string, string>::iterator itr = status.find(GEAR);
    if(itr != status.end())
        return stoi(itr->second);
    else
        return -1;
}

int dash_model::speed(){
    map<string, string>::iterator itr = status.find(SPEED);
    if(itr != status.end())
        return stoi(itr->second);
    else
        return -1;
}
