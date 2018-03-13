#ifndef CAN_H
#define CAN_H
#include <stdint.h>
class CAN{
	public:
		CAN();
		uint32_t read_msg(char * msg);
		void write_msg(uint32_t id, char * msg);
	private:
		int sockfd;
		

};


#endif

