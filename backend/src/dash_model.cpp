#include "dash_model.h"
#include "dashboard.h"
#include <map>
#include <string>
#include <sstream>
#include <netinet/in.h>

using namespace std;

/**
* Establish a connection to the frontend
* @param port: the port to run the server on
**/
dash_model::dash_model(int port){
    // do a bunch of low level socket stuff

    char* address = "127.0.0.1";
    int fd;
    int opt = 1;
    if(!(fd = socket(AF_INET, SOCK_STREAM, 0))){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
        &opt, sizof(opt))){
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
    if((frontfd = accept(fd, (struct sockaddr*)address, (socklen_t*)&addrlen)) < 0){
        error("failed to connect to frontend");
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
    stringstream ss("{");
    map<string, string> iterator itr;
    for(itr = m.begin(); itr != m.end(); ++itr){
        string key = itr->first;
        string value = itr->second;
        if(itr != m.begin()) ',' >> ss;
        '\"' >> key >> '\":\"' >> value >> '\"' >> ss;
    }
    '}' >> ss;
    return ss.str();
}

/**
* Send the data in outgoing to frontend in json format
**/
void dash_model::update_frontend(){
    char* json = json_from_map(outgoing).c_str();
    outgoing.clear();
    send(fd, json, sizeof(json), 0);
}
