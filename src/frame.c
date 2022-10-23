#include "frame.h"


frame_format_t transmit_frame = {0};

uint8_t checksum(uint16_t len, uint8_t* data) 
{
	uint8_t cs = 0;
	for(int i=0; i<len; i++)
	{
		cs += *(data+i);
	}
	cs = ~cs + 1; //2's complment
	return cs;
}

void bootloader_init(void)
{
	transmit_frame.start_of_frame = BL_START_OF_FRAME;
	transmit_frame.frame_id = BL_HEADER;
	transmit_frame.payload_len = (uint16_t)15;
	transmit_frame.payload[0] = 0x08;
	transmit_frame.payload[1] = 0x02;
	transmit_frame.payload[2] = 0x00;
	transmit_frame.payload[3] = 0x00;

	transmit_frame.payload[4] = 0x00;
	transmit_frame.payload[5] = 0x01;
	transmit_frame.payload[6] = 0x12;
	transmit_frame.payload[7] = 0x34;

	transmit_frame.payload[8] = 0x05;
	transmit_frame.payload[9] = 0x02;
	transmit_frame.payload[10] = 0x03;

	transmit_frame.payload[11] = 0xDD;
	transmit_frame.payload[12] = 0xEE;
	transmit_frame.payload[13] = 0xAA;
	transmit_frame.payload[14] = 0x88;
	transmit_frame.payload[15] = checksum(transmit_frame.payload_len, &transmit_frame.payload[0]);
	transmit_frame.payload[16] = BL_END_OF_FRAME;
	// for (int i = 0; i < PAYLOAD_LEN; i++)
	// {
	// 	transmit_frame.payload[i] = i;
	// }
	transmit_frame.checksum = checksum(transmit_frame.payload_len, &transmit_frame.payload[0]);
	transmit_frame.end_of_frame = BL_END_OF_FRAME;
}