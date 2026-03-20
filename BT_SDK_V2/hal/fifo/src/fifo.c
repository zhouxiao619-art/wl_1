#include "fifo.h"
#include "gpio.h"

static FOFI_T uart_fifo;

/* 在循环队列中加入新的元素 */
void user_fifo_add(uint8_t elem)
{
	if(((uart_fifo.tail + 1) % FIFO_NUM_MAX) != uart_fifo.front){
		uart_fifo.tail = ((uart_fifo.tail +1) % FIFO_NUM_MAX);
		uart_fifo.fifo_buff[uart_fifo.tail] = elem;
	}
}
/* 删除队列中的队首元素并返回该数据 */
uint8_t user_fifo_del(void)
{
	uint8_t ret = 0;
	if(uart_fifo.front != uart_fifo.tail){
		uart_fifo.front = ((uart_fifo.front + 1) % FIFO_NUM_MAX);
		ret = uart_fifo.fifo_buff[uart_fifo.front];
		return ret;
	}
	return 0;
}

/* 获取队列中的队列元素个数*/
uint16_t user_fifo_num_read(void)
{
	if(uart_fifo.front != uart_fifo.tail){
		if(uart_fifo.tail > uart_fifo.front){
			return (uart_fifo.tail - uart_fifo.front);
		}else{
			return ((FIFO_NUM_MAX - uart_fifo.front) + uart_fifo.tail);
		}
	}else{
		return 0;
	}
}
