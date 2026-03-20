#ifndef  _FIFO_H__
#define  _FIFO_H__

//#include "stdint.h"
//#include "string.h"
//#include "stdbool.h"

#include "uart.h"


#define FIFO_NUM_MAX  128

typedef struct FIFO
{
	uint8_t fifo_buff[FIFO_NUM_MAX];
	uint16_t  front;
	uint16_t  tail;
} FOFI_T,*FIFO_P;


uint8_t user_fifo_del(void);
uint16_t user_fifo_front_read(void);
uint16_t user_fifo_num_read(void);

#endif
