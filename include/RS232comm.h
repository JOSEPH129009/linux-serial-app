#ifndef RS232COMM_H_
#define RS232COMM_H_

#define BIT_RATE B115200		

extern const char PORT[]; 

void serial_port_init(int* fd);


#endif /* RS232COMM_H_ */