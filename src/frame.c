#include "frame.h"
#include "RS232comm.h"


frame_format_t transmit_frame = {0};

uint8_t checksum(uint16_t len, uint8_t* data) 
{
	uint8_t cs = 0;
	for(int i=0; i<len; i++)
	{
		cs += data[i];
	}
	cs = ~cs + 1; //2's complment
	return cs;
}

int serial_transmit(int* fd, frame_format_t* t_frame)
{
	int ret = 0;
	if(write(*fd, &t_frame->start_of_frame ,1) < 0)
		ret = 1;
	if(write(*fd, &t_frame->frame_id ,1) < 0)
		ret = 1;
	if(write(*fd, &t_frame->payload_len ,2) < 0)
		ret = 1;
	for(int i = 0; i < t_frame->payload_len; i++)
	{
		if(write(*fd, &t_frame->payload[i] ,1) < 0)
			ret = 1;
	}
	if(write(*fd, &t_frame->checksum ,1) < 0)
		ret = 1;
	if(write(*fd, &t_frame->end_of_frame ,1) < 0)
		ret = 1;
	return ret;
}


void header_update_info_prepare(uint32_t size, uint32_t address, 
								uint8_t* version, uint32_t firmware_checksun)
{
	transmit_frame.start_of_frame = BL_START_OF_FRAME;
	transmit_frame.frame_id = BL_HEADER;
	transmit_frame.payload_len = 15;
	transmit_frame.payload[0] = (uint8_t)address;
	transmit_frame.payload[1] = (uint8_t)(address>>8);
	transmit_frame.payload[2] = (uint8_t)(address>>16);
	transmit_frame.payload[3] = (uint8_t)(address>>24);

	transmit_frame.payload[4] = (uint8_t)size;
	transmit_frame.payload[5] = (uint8_t)(size>>8);
	transmit_frame.payload[6] = (uint8_t)(size>>16);
	transmit_frame.payload[7] = (uint8_t)(size>>24);

	transmit_frame.payload[8] = version[2];  //0
	transmit_frame.payload[9] = version[1];  //0
	transmit_frame.payload[10] = version[0]; //1
	// version 1.0.0
	transmit_frame.payload[11] = (uint8_t)firmware_checksun;
	transmit_frame.payload[12] = (uint8_t)(firmware_checksun>>8);
	transmit_frame.payload[13] = (uint8_t)(firmware_checksun>>16);
	transmit_frame.payload[14] = (uint8_t)(firmware_checksun>>24);
	transmit_frame.checksum = checksum(transmit_frame.payload_len, &transmit_frame.payload[0]);
	transmit_frame.end_of_frame = BL_END_OF_FRAME;
}

void update_payload_prepare(uint32_t len)
{
	transmit_frame.start_of_frame = BL_START_OF_FRAME;
	transmit_frame.frame_id = BL_PAYLOAD;
	transmit_frame.payload_len = len;
	transmit_frame.checksum = checksum(transmit_frame.payload_len, &transmit_frame.payload[0]);
	transmit_frame.end_of_frame = BL_END_OF_FRAME;
}


void update_done_prepare(void)
{
	transmit_frame.start_of_frame = BL_START_OF_FRAME;
	transmit_frame.frame_id = BL_UPDATE_DONE;
	transmit_frame.payload_len = 0;
	transmit_frame.checksum = checksum(transmit_frame.payload_len, &transmit_frame.payload[0]);
	transmit_frame.end_of_frame = BL_END_OF_FRAME;
}

int parse_frame(const uint8_t* data)
{
	uint32_t status = 0;
	uint16_t data_len = 0;
	uint32_t error_flag = 0;
	data_len = data[DATA_LEN+1] << 8 | data[DATA_LEN]; //LSB first
	if((data[1] == BL_NACK_FRAME))
	{
		if(data[0] == BL_START_OF_FRAME
		&&	data_len == 4 
		&&	data[DATA_BEGIN+data_len] == checksum(data_len,(uint8_t*)&data[DATA_BEGIN])
		&&	data[DATA_BEGIN+data_len +1] == BL_END_OF_FRAME)
		{
			serial_sort_function((uint8_t*)&error_flag,&data[DATA_BEGIN],4,LSB_FIRST);
			printf("NAK, Error Flag:0x%X\r\n",error_flag);
			return -1;
		}
		else
		{
			printf("NACK frame error\r\n");
			for(int i = 0; i < 20; i++)
				printf("0x%02X ", data[i]);
			return -1;
		}
	}
	else if(((data[1] == BL_ACK_FRAME)))
	{
		if(data[0] == BL_START_OF_FRAME
		&&	data_len == 0 
		&&	data[DATA_BEGIN+data_len] == checksum(data_len,(uint8_t*)&data[DATA_BEGIN])
		&&	data[DATA_BEGIN+data_len +1] == BL_END_OF_FRAME)
		{
			// printf("ACK!\r\n");
		}
		else
		{
			printf("ACK frame error\r\n");
			for(int i = 0; i < 20; i++)
				printf("0x%02X ", data[i]);
			return -1;
		}

	}
	else
	{
		printf("Not ACK Nor ACK!\r\n");
		for(int i = 0; i < 20; i++)
			printf("0x%02X ", data[i]);
		return -1;
	}

	return 0;

}
