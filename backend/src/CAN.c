#include "CAN.h"
#include <unistd.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <cstdlib>
#include <string>

using namespace std;
CAN::CAN(){
	struct sockaddr_can addr;
	// create the socket
	if((sockfd = socket(AF_CAN, SOCK_RAW, CAN_RAW)) < 0){
		perror("creating CAN socket");
		exit(EXIT_FAILURE);
	}

	// bind the socket
	addr.can_family = AF_CAN;
	addr.can_ifindex = 0;

	if(bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0){
		perror("binding CAN socket");
		exit(EXIT_FAILURE);
	}
}

void CAN::read_msg(char *msg){
	struct can_frame frame; 
	read(sockfd, &frame, sizeof(struct can_frame));
	for(int i = 0; i < 8; ++i)
		msg[i] = (char) frame.data[i];
}

void CAN::write_msg(char *msg){
	struct can_frame frame;
	for(int i = 0; i < 8; ++i)
		frame.data[i] = msg[i];
	write(sockfd, &frame, sizeof(struct can_frame));
}
