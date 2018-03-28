#include "GPS.h"
#include "dashboard.h"
#include "CAN.h"
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <iostream>

/**
* Open the GPS port
**/
GPS::GPS(){
    fd = open(GPS_PORT, O_RDWR | O_NOCTTY | O_SYNC);
    if(fd < 0){
        cerr << "Couldn't open GPS port" << endl;
        exit(EXIT_FAILURE);
    }
    struct termios options;

    // get the current options for the port
    tcgetattr(fd, &options);

    // set the baud we want
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);

    options.c_cflag &= ~PARENB; // set no parity bit
    options.c_cflag &= ~CSTOPB; // set no stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~CRTSCTS;
    tcsetattr(fd, TCSANOW, &options);
}

void GPS::read_sentence(){
    string sentence;
    string prefix;
    char c;
    do{
        prefix = "";
        c = '0';
        while(c != '$'){
            read(fd, &c, 1);
        }
        prefix = string(&c);
        int index = 0;
        while(index < 5){
            read(fd, &c, 1);
            prefix += c;
            ++index;
        }
    }while(!prefix.compare("$GPRMC"));
    sentence += prefix;
    read(fd, &c, 1);
    do{
        sentence += string(&c);
        read(fd, &c, 1);
    }while(c != '$');
    cout << sentence << endl;
    // skip to latitude
    sentence = sentence.substr(sentence.find('A')+2);

    float latdeg = atof(sentence.substr(0,2).c_str());
    float latmin = atof(sentence.substr(2,4).c_str());
    sentence = sentence.substr(sentence.find('.') + 1);
    latmin += atof((string(".") + sentence.substr(0,4)).c_str());
    latdeg += latmin/60;

    sentence = sentence.substr(sentence.find('N') + 2);

    float longdeg = atof(sentence.substr(0, 3).c_str());
    //get the ones and tens place of min
    float longmin = atof(sentence.substr(3,5).c_str());
    //move past decimal point
    sentence = atof(sentence.substr(sentence.find('.') + 1).c_str());
    //get the 4 decimal places
    longmin += atof((string(".") + sentence.substr(0,4)).c_str());

    sentence = sentence.substr(sentence.find('W') + 2);

    float vel = atof(sentence.substr(0, sentence.find(',')).c_str());

    sentence = sentence.substr(sentence.find(',')+1);

    float ang = atof(sentence.substr(0, sentence.find(',')).c_str());

    mx.lock();
    current[LATITUDE] = latdeg;
    current[LONGITUDE] = longdeg;
    current[VELOCITY] = vel;
    current[ANGLE] = ang;
    mx.unlock();
}

map<string, float> GPS::get_current(){
    cout << "LAT: " << current[LATITUDE] << endl;
    cout << "LONG: " << current[LONGITUDE] << endl;
    cout << "VEL: " << current[VELOCITY] << endl;
    cout << "ANGLE: " << current[ANGLE] << endl;
    return map<string, float>(current);
}

void * gps_routine(void * p){
    CAN* can = (CAN*) p;
    GPS gps;
    while(1){
        gps.read_sentence();
        map<string, float> current = gps.get_current();
        uint32_t lat_thou = current[LATITUDE] / .001;
        uint32_t long_thou = current[LONGITUDE] / .001;
        uint16_t vel = current[VELOCITY] / .1;
        uint16_t angle = current[ANGLE] / .1;

        char msg1[8] = {
            lat_thou >> 16,
            (lat_thou & 0x0000FF00) >> 8,
            lat_thou & 0xFF,
            long_thou >> 16,
            (long_thou & 0x0000FF00) >> 8,
            long_thou & 0xFF,
            vel >> 8,
            vel & 0xFF
        };

        char msg2[8] = {angle >> 8, angle & 0xFF,
            0x00,0x00,0x00,0x00,0x00,0x00};

        can->write_msg(GPS_ID, msg1);
        can->write_msg(GPS_ID_SEC, msg2);
        SLEEP(.001);
    }
}
