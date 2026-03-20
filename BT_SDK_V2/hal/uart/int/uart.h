/**
******************************************************************************************************************************************************************************************
*@file                       uart.h
*@author         
*@data                       
*@version                    v1.0
*@par Copyright(c):
*
*@par History:
*     version:author,date,desc
*     v1.0   :create this file
*
*******************************************************************************************************************************************************************************************
*/

#ifndef UART_H_
#define UART_H_

#include "cpu.h"

#define APP_UART_ENABLE		DEMO_DEBUG	//使能uart

#define DEMO_DEBUG			0			// 1 打开log 0 关闭

#if DEMO_DEBUG
#define  DEMO_PRINTF  printf
#else
#define DEMO_PRINTF(...)
#endif



#define UART_WORDLENGTH_5b                 0
#define UART_WORDLENGTH_6b                1
#define UART_WORDLENGTH_7b                2
#define UART_WORDLENGTH_8b                3

#define UART_STOPBIT_1                    0
#define UART_STOPBIT_2                    1

#define UART_PARITY_NO                    0
#define UART_PARITY_ODD                   1
#define UART_PARITY_EVENT                	3

/* 接口声明 */

//设置波特率
static inline void  uart_baud_val(uint32_t val) 
{
	uint32_t 	BaudRateI,BaudRateF;
	uint32_t 	freq_is_32m;
	
//	*(volatile uint32_t *)0x50005000 = 0x203;	
	freq_is_32m =  WS_READ_FIELD(SYSCFG_BASE, SYSCFG_F_XTAL_FREQ);
		
	if(freq_is_32m == 1)
	{
		BaudRateI = 32000000/8/val;
		BaudRateF= ((3200000000/8/val-BaudRateI*100)*64+50)/100;
	}
	else
	{
		BaudRateI = 16000000/8/val;
		BaudRateF= ((1600000000/8/val-BaudRateI*100)*64+50)/100;
	}	
	WS_WRITE_FIELD(UART_BASE, UART_R_IBRD, BaudRateI);      
	WS_WRITE_FIELD(UART_BASE, UART_R_FBRD, BaudRateF);  
	
}
	 
/*
 * wordlength ->>
 *   UART_WORDLENGTH_5b
 *   UART_WORDLENGTH_6b
 *   UART_WORDLENGTH_7b
 *   UART_WORDLENGTH_8b
 *
 * stopbit ->>
 *     UART_STOPBIT_1
 *     UART_STOPBIT_2
 *
 * parity ->>
 *     UART_PARITY_NO    
 *     UART_PARITY_ODD    
 *     UART_PARITY_EVENT
 */
#define uart_lcrh_val(wordlength, stopbit, parity)                    \
    do{                                                                \
        WS_WRITE_FIELD(UART_BASE, UART_F_WLEN_RX, wordlength);        \
        WS_WRITE_FIELD(UART_BASE, UART_F_WLEN_TX, wordlength);        \
        WS_WRITE_FIELD(UART_BASE, UART_F_STP2_RX, stopbit);            \
        WS_WRITE_FIELD(UART_BASE, UART_F_STP2_TX, stopbit);            \
        WS_WRITE_FIELDS(UART_BASE, UART_F_PEN_RX, parity&0x01,        \
                                   UART_F_EPS_RX, parity&0x02);        \
        WS_WRITE_FIELDS(UART_BASE, UART_F_PEN_TX, parity&0x01,        \
                                   UART_F_EPS_TX, parity&0x02);        \
    }while(0)

/* FIFO使能控制       ENABLE/DISABLE */
#define uart_fifo_en(tfe, rfe)                                        \
    do{                                                                \
        WS_WRITE_FIELD(UART_BASE, UART_F_FEN_TX, tfe);                \
        WS_WRITE_FIELD(UART_BASE, UART_F_FEN_RX, rfe);                \
    }while(0)

#define UART_RX_FIFO_FULL_1            0x00
#define UART_RX_FIFO_FULL_2            0x01
#define UART_RX_FIFO_FULL_4            0x02
#define UART_RX_FIFO_FULL_8            0x03

#define UART_TX_FIFO_FULL_1            0x00
#define UART_TX_FIFO_FULL_2            0x01
/* FIFO中断触发阈值设置 */
#define uart_fifo_level_select(txfl, rxfl)                            \
        WS_WRITE_FIELDS(UART_BASE, UART_F_TXIFLSEL, txfl, UART_F_RXIFLSEL, rxfl)

/* uart使能控制       ENABLE/DISABLE */
#define uart_en(txe, rxe, uarte)                                    \
        WS_WRITE_FIELDS(UART_BASE, UART_F_UARTEN, uarte,            \
        UART_F_OVSFACT, 1, UART_F_TXE, txe, UART_F_RXE, rxe)

/* uart硬件流控制 */
#define uart_hardwareflow_ctr(rts, cts)                                \
        WS_WRITE_FIELDS(UART_BASE, UART_F_RTSEN, rts, UART_F_CTSEN, cts)



/* 接收数据 */
#define uart_receive_byte()                        WS_READ_FIELD(UART_BASE, UART_F_DATA)

/* 发送数据 */
#define uart_send_byte(byte)                    WS_WRITE_FIELDS_NO_READBACK(UART_BASE, UART_F_DATA, byte)

/*
 * 获取串口状态标志位
 * FIALD：
 * UART_F_TXFE    发送FIFO空
 * UART_F_TXFF    发送FIFO满
 * UART_F_RXFE    接收FIFO空
 * UART_F_RXFF    接收FIFO满
 * UART_F_BUSY    uart忙
 */
#define uart_get_flag(FIALD)                    WS_READ_FIELD2(UART_BASE, FIALD)

/*
 * 中断状态配置
 * FIALD:
 * UART_F_TXFEIM     发送FIFO空中断mask
 * UART_F_XOFFIM     XOFF中断mask
 * UART_F_OEIM       Overrun error中断mask
 * UART_F_BEIM       break error中断mask
 * UART_F_PEIM       校验错误中断mask
 * UART_F_FEIM       帧错误中端mask
 * UART_F_RTIM       接收超时中断mask
 * UART_F_TXIM       发送中断mask
 * UART_F_RXIM       接收中断mask 
 *
 * newstatus: ENABLE/DISABLE
 */
#define uart_it_config(FIALD, newstatus)        WS_WRITE_FIELD2(UART_BASE, FIALD, newstatus)    

/*
 * 读取中断状态
 * FIALD:
 *    UART_F_TXFEMIS         发送FIFO空中断
 *    UART_F_XOFFMIS         XOFF中断
 *    UART_F_OEMIS           Overrun error中断
 *    UART_F_BEMIS           break error中断
 *    UART_F_PEMIS           校验错误中断
 *    UART_F_FEMIS           帧错误中断
 *    UART_F_TRMIS           接收超时中断
 *    UART_F_TXMIS           发送中断
 *    UART_F_RXMIS           接收中断   
 */
#define uart_get_it_status(FIALD)                WS_READ_FIELD2(UART_BASE, FIALD)

//串口初始化
void uart_user_init(void);

//串口休眠准备
void hal_uart_suspend(void);

void hal_uart_send(uint8_t *pdata,uint16_t data_len);

//串口数据处理
//void user_uart_receive_handle(void);

#endif
