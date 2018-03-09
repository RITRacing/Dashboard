#ifndef CAN_H
#define CAN_H
class CAN{
	public:
		CAN();
		void read_msg(char * msg);
		void write_msg(char * msg);
	private:
		int sockfd;


};


#endif
