#include "CAN.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <stdint.h>
#include <iostream>

using namespace std;



CAN::CAN(){
	struct sockaddr_can addr;
	// create the socket
	if((sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0){
		perror("creating CAN socket");
		exit(EXIT_FAILURE);
	}

	// bind the socket
	addr.can_family = AF_CAN;
	struct ifreq ifr;	
	strcpy(ifr.ifr_name, "can0");
	if(ioctl(sockfd,SIOCGIFINDEX, &ifr) < 0){
		perror("SIIIGJJGIGJGJJG");
		exit(EXIT_FAILURE);
	}
	addr.can_ifindex = ifr.ifr_ifindex;

	//addr.can_ifindex = 0;
	if(bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0){
		perror("binding CAN socket");
		exit(EXIT_FAILURE);
	}
}

uint32_t CAN::read_msg(char *msg){
	canmx.lock();
	struct can_frame frame; 
	read(sockfd, &frame, sizeof(struct can_frame));
	for(int i = 0; i < 8; ++i)
		msg[i] = (char) frame.data[i];
	canmx.unlock();
	return frame.can_id;
}

void CAN::write_msg(uint32_t id, char *msg){
	canmx.lock();
	struct can_frame frame;
	memset(&frame, 0, sizeof(frame));
	frame.can_id = id;
	for(int i = 0; i < 8; ++i)
		frame.data[i] = msg[i];
	frame.can_dlc = 8;
	int nbytes;
	if((nbytes = write(sockfd, &frame, sizeof(frame))) != sizeof(frame)){
	//if(nbytes = sendto(sockfd, &frame, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, sizeof(addr))){
		perror("Write error");
		exit(EXIT_FAILURE);		
	}
	canmx.unlock();
}
