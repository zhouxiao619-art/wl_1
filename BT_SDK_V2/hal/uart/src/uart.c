#include "uart.h"
#include "lib_api.h"
#include <stdio.h>
#include "gpio.h"
#include "cpu_nvic.h"
#include "ble_gatt.h"

void uart_user_init(void)
{
	#if APP_UART_ENABLE
		cpu_crm_set(CRM_F_UART0_EN, ENABLE);
		cpu_crm_reset(CRM_F_UART0_SRST, ENABLE);
		
//		gpio_pinmode_set(GPIO_PA3,GPIO_INOUT_MODE_PULLUP_IN);
		gpio_pinmode_set(GPIO_PA0,GPIO_INOUT_MODE_PULLUP_IN);
		
//		gpio_remapmode_set(GPIO_PA3, GPIO_PINMUX_UART0_RX);
		gpio_remapmode_set(GPIO_PA0, GPIO_PINMUX_UART0_TX);
		
		//波特率配置
		uart_baud_val(1000000);
		
		//串口帧配置、发送接收fifo使能
		uart_lcrh_val(UART_WORDLENGTH_8b, UART_STOPBIT_1, UART_PARITY_NO);
		
		//设置fifo 阈值中断
//		uart_fifo_level_select(UART_TX_FIFO_FULL_1,UART_RX_FIFO_FULL_8);
//		uart_fifo_en(DISABLE,ENABLE);
	
		//uart硬件流控制
		uart_hardwareflow_ctr(DISABLE, DISABLE);
		
		//串口中断优先级配置，优先级不可配置为0，需要大于0
		NVIC_SetPriority(UART_IRQn, 2);
		NVIC_EnableIRQ(UART_IRQn);
		
		//接收中断使能、发送中断使能、接收超时中断使能
//		uart_it_config(UART_F_RXIM, ENABLE);  
//		uart_it_config(UART_F_TXIM, DISABLE);  
//		uart_it_config(UART_F_RTIM, ENABLE);
		
		//uart使能
		uart_en(ENABLE, ENABLE, ENABLE);
	#endif
}

/* 串口休眠准备 */
void hal_uart_suspend(void)
{
	#if APP_UART_ENABLE
		while(uart_get_flag(UART_F_BUSY));

		/* 关闭串口时钟 */
		cpu_crm_set(CRM_F_UART0_EN, 0);
//		gpio_remapmode_set(GPIO_PA3, GPIO_PINMUX_GPIO);
		gpio_remapmode_set(GPIO_PA0, GPIO_PINMUX_GPIO);
	#endif
}

//void UART0_IRQHandler(void)
//{
//	if(uart_get_it_status(UART_F_RXMIS)){
//		while(!uart_get_flag(UART_F_RXFE)){
//			user_fifo_add(uart_receive_byte());
//		}
//	}
//	
//	if(uart_get_it_status(UART_F_TRMIS)){
//		while(!uart_get_flag(UART_F_RXFE)){
//			user_fifo_add(uart_receive_byte());
//		}
//	}

//	NVIC_ClearPendingIRQ(UART_IRQn);
//}


//void hal_uart_send(uint8_t *pdata,uint16_t data_len)
//{
//	for(uint8_t i = 0; i < data_len; i++)
//	{
//		while(uart_get_flag(UART_F_TXFF));
//		uart_send_byte(pdata[i]);
//	}
//}

#if APP_UART_ENABLE
int fputc(int ch, FILE *f)
{
  while(uart_get_flag(UART_F_TXFF));
  uart_send_byte((uint8_t) ch);
  return ch;
}
#endif


