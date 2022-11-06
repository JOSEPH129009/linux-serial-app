#ifndef RS232COMM_H_
#define RS232COMM_H_

#define BIT_RATE B115200		

#include <stdio.h>		
#include <string.h>
#include <unistd.h>		// Unix standard functions
#include <fcntl.h>		// File control funcitons
#include <errno.h>		// Error number definitions
#include <termios.h>	// POSIX terminal control definitions
#include "stdlib.h"
#include "stdint.h"

extern const char PORT[]; 

void serial_port_init(int* file_descriptor);
void serial_sort_function(uint8_t *dest,const uint8_t* src, uint8_t len, uint32_t endianness);
enum{
    LSB_FIRST,
    MSB_FIRST
};
#endif /* RS232COMM_H_ */