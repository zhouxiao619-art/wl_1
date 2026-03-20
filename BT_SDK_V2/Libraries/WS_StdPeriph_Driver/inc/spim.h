/**
******************************************************************************************************************************************************************************************
*@file                       spim.h
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

#ifndef SPIM_H_
#define SPIM_H_

#include "cpu.h"

#define SPIM_DFS_8            0x07
#define SPIM_DFS_16            0x0F

#define SPIM_SCPH_1            0X00
#define SPIM_SCPH_2            0X01

#define SPIM_SCPOL_L        0X00
#define SPIM_SCPOL_H        0X01

#define SPIM_TMOD_TRX        0X00
#define SPIM_TMOD_TXO        0X01
#define SPIM_TMOD_RXO        0X02
#define SPIM_TMOD_EEROMR    0X03

#define SPIM_BAUDR_DIV_2        2
#define SPIM_BAUDR_DIV_4        4
#define SPIM_BAUDR_DIV_8        8
#define SPIM_BAUDR_DIV_16        16
#define SPIM_BAUDR_DIV_32        32
#define SPIM_BAUDR_DIV_64        64
#define SPIM_BAUDR_DIV_128        128
#define SPIM_BAUDR_DIV_256        256

//ENABLE/DISABLE
#define spim_en(var)            WS_WRITE_FIELD(SPIM_BASE, SPIM_F_SSI_EN, var)

//使能和失能所有中断
#define spim_it_all_en(var)        \
    do {                        \
        if(var)                    \
            WS_WRITE_FIELDS_NO_READBACK(SPIM_BASE, SPIM_R_IMR, 0x3F);    \
        else                                                            \
            WS_WRITE_FIELDS_NO_READBACK(SPIM_BASE, SPIM_R_IMR, 0);        \
    }while(0)

#define spim_bus_init(dfs, scph, scpol, tmod)                            \
    do{                                                                    \
        spim_en(DISABLE);                                                \
        WS_WRITE_FIELDS(SPIM_BASE, SPIM_F_DFS, dfs,                        \
        SPIM_F_SCPH, scph, SPIM_F_SCPOL, scpol, SPIM_F_TMOD, tmod);        \
        WS_WRITE_FIELD(SPIM_BASE, SPIM_F_SER, 1);                        \
    }while(0)

#define spim_baud_set(val)        WS_WRITE_FIELD(SPIM_BASE, SPIM_R_BAUDR, val)

//设置接收和发送FIFO的中断阈值
#define spim_fifo_threshold(rxftlr, txftlr)                                \
    do{                                                                    \
        WS_WRITE_FIELD(SPIM_BASE, SPIM_R_RXFTLR, rxftlr);                \
        WS_WRITE_FIELD(SPIM_BASE, SPIM_R_TXFTLR, txftlr);                \
    }while(0)

//获取当前发送FIFO中数据个数
#define spim_tx_fifo_level()        WS_READ_FIELD(SPIM_BASE, SPIM_R_TXFLR)

//获取当前接收FIFO中数据个数
#define spim_rx_fifo_level()        WS_READ_FIELD(SPIM_BASE, SPIM_R_RXFLR)

/*
 * FIELD ->>
 * SPIM_F_MSTIM: Multi-Master Contention Interrupt Mask.                       
 * SPIM_F_RXFIM: Receive FIFO Full Interrupt Mask.                       
 * SPIM_F_RXOIM: Receive FIFO Overflow Interupt Mask.                       
 * SPIM_F_RXUIM: Receive FIFO Underflow Interrupt Mask.                       
 * SPIM_F_TXOIM: Transmit FIFO Overflow Interrupt Mask.                       
 * SPIM_F_TXEIM: Transmit FIFO Empty Interrupt Mask.
 * var ->>
 * 0:masked; 1:Not masked; 
 */                       
#define spim_it_en(FIELD, var)        WS_WRITE_FIELD2(SPIM_BASE, FIELD, var)

/*
 * FIELD ->>
 * SPIM_F_MSTIS: Multi-Master Contention Interrupt Status.
 * SPIM_F_RXFIS: Receive FIFO Full Interrupt Status.
 * SPIM_F_RXOIS: Receive FIFO Overflow Interrupt Status.
 * SPIM_F_RXUIS: Receive FIFO Underflow Interrupt Status.
 * SPIM_F_TXOIS: Transmit FIFO Overflow Interrupt Status.
 * SPIM_F_TXEIS: Transmit FIFO Empty Interrupt Status.
 */
#define spim_it_status(FIELD)        WS_READ_FIELD2(SPIM_BASE, FIELD)

#define spim_it_clear()                WS_READ_FIELD(SPIM_BASE, SPIM_F_ICR);

/*
 * FIELD ->>
 * SPIM_F_DCOL: 数据碰撞错误
 * SPIM_F_TXE:  发送错误
 * SPIM_F_RFF:  接收FIFO满
 * SPIM_F_RFNE: 接收FIFO不为空
 * SPIM_F_TFE:  发送FIFO为空
 * SPIM_F_TFNF: 发送FIFO未满；
 * SPIM_F_BUSY: SPI忙标志
 */
#define spim_status(FIELD)            WS_READ_FIELD2(SPIM_BASE, FIELD)

//发送数据
#define spim_send_data(val)            WS_WRITE_FIELDS_NO_READBACK(SPIM_BASE, SPIM_F_DR, val)

//接收数据
#define spim_receive_data()            WS_READ_FIELD(SPIM_BASE, SPIM_R_DR)

#endif
