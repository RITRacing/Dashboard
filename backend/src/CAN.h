#ifndef CAN_H
#define CAN_H
#include <stdint.h>

/**
* Abstraction from the socket communication with the CAN shield.
**/
class CAN{
	public:
		CAN(); // Constructor
		uint32_t read_msg(char * msg); // reads a CAN message
		void write_msg(uint32_t id, char * msg); // writes a CAN message
	private:
		int sockfd; // file descriptor representing the socket
};

#endif
