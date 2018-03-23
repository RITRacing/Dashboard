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
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

/**
* Establish a connection to the frontend
* @param port: the port to run the server on
**/
dash_model::dash_model(int port){
    // create the socket
    int fd;
    int opt = 1;
    if(!(fd = socket(AF_INET, SOCK_STREAM, 0))){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    // set options for our socket
    if(setsockopt(fd, IPPROTO_TCP,TCP_NODELAY,
        &opt, sizeof(opt))){
            perror("Socket initialization failed");
            exit(EXIT_FAILURE);
    }

    // bind the socket to the port it will listen on
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if(bind(fd, (struct sockaddr * ) &address, sizeof(address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen on that port
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

    // set some socket options for the frontend socket
    if(setsockopt(frontfd, IPPROTO_TCP,TCP_NODELAY,
        &opt, sizeof(opt))){
            perror("Frontend Socket initialization failed");
            exit(EXIT_FAILURE);
    }

    // now open telemetry Socket (serial port to xbee)
    telefd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
    if(telefd == -1){
        perror("could not open socket to xbee");
        exit(EXIT_FAILURE);
    }

    // create the serial port options struct
    struct termios options;

    // get the current options for the port
    tcgetattr(telefd, &options);

    // set the baud we want
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);

    options.c_cflag &= ~PARENB; // set no parity bit
    options.c_cflag &= ~CSTOPB; // set no stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~CRTSCTS;
    tcsetattr(telefd, TCSANOW, &options); // set these options NOW

    times = 0; // initialize placeholder telemetry message counter
}

/**
* If the the value has changed, stage it to be sent
* @param key: the status key
* @param value: the new value
*/
void dash_model::set(string key, string value){
    modelmx.lock();
    if(status[key].compare(value))
        status[key] = outgoing[key] = value;
    modelmx.unlock();
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
        ss << "}@"; // @ delimiter so the frontend knows the end of one json
    }
    return ss.str();
}

/**
* Send the data in outgoing to frontend in json format
* And also send it to the ground station (for the time being)
**/
void dash_model::update_frontend(){
    modelmx.lock();
    if(outgoing.size() > 0){
        string jstring = json_from_map(outgoing);
        const char * json = jstring.c_str();
        send(frontfd, json, strlen(json),0);

        // (temporary) send whole status every 10th message
        if(times == 10){
            json = string(json_from_map(status)).c_str();
            write(telefd, json, strlen(json));
            times = 0;
        }

        outgoing.clear();
        ++times;
    }
    modelmx.unlock();
}

/**
* Get the gear from the model
* @return the gear as an int
**/
int dash_model::gear(){
    modelmx.lock();
    map<string, string>::iterator itr = status.find(GEAR);
    modelmx.unlock();
    if(itr != status.end())
        return stoi(itr->second);
    else
        return -1;

}

/**
* Get the speed from the model
* @return the speed as an int
**/
int dash_model::speed(){
    modelmx.lock();
    map<string, string>::iterator itr = status.find(SPEED);
    modelmx.unlock();
    if(itr != status.end())
        return stoi(itr->second);
    else
        return -1;
}

/**
* Get the rpm from the model
* @return the rpm as an int
**/
int dash_model::rpm(){
    modelmx.lock();
    map<string, string>::iterator itr = status.find(RPM);
    modelmx.unlock();
    if(itr != status.end())
        return stoi(itr->second);
    else
        return -1;
}
