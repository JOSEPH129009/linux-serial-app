#include "frame.h"


frame_format_t transmit_frame = {0};


void bootloader_init(void)
{
	transmit_frame.start_of_frame = BL_START_OF_FRAME;
	transmit_frame.frame_id = BL_ACK_FRAME;
	transmit_frame.payload_len = (uint16_t)65535;
	transmit_frame.crc32 = 0xffffffff; // TODO:
	transmit_frame.end_of_frame = BL_END_OF_FRAME;
	for (int i = 0; i < PAYLOAD_LEN; i++)
	{
		transmit_frame.payload[i] = i;
	}

}