/**
******************************************************************************************************************************************************************************************
*@file                       wdt.h
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

#ifndef WDT_H_
#define WDT_H_

#include "cpu.h"
#include "lib_api.h"

#define APP_ENABLE_WDT			1			//打开看门狗
#define APP_WDT_TIMEOUT			30			//看门狗复位超时时间，单位：s

//设置WDT重装载值
#define wdt_reload_val(val)            WS_WRITE_FIELD(WDT_BASE, WDT_F_RLD, val)

//WDT中断使能和失能
#define wdt_it_en(val)                WS_WRITE_FIELD(WDT_BASE, WDT_F_RMOD, val)

//WDT使能和失能
#define wdt_en(val)                    do{	\
	uint32_t reg_val = WS_READ_FIELD(WDT_BASE,WDT_R_WDT_CR);\
	reg_val  = (reg_val & 0x1E) | 0x8e690000 | val;	\
	WS_WRITE_FIELD(WDT_BASE, WDT_R_WDT_CR, reg_val);	\
}while(0)

//喂狗
#define wdt_feed_dog()				WS_WRITE_FIELD(WDT_BASE, WDT_F_WDT_CRR, 0x76)

//读取WDT当前计数值
#define wdt_conuter_cnt()			WS_READ_FIELD(WDT_BASE, WDT_F_WDT_CNT)

//获取中断状态
#define wdt_it_status()				WS_READ_FIELD(WDT_BASE, WDT_F_STAT)

//清除中断
#define wdt_it_clear()				WS_WRITE_FIELD(WDT_BASE, WDT_F_ICR, 1)

#define wdt_rmod_set(val)			WS_WRITE_FIELD(WDT_BASE, WDT_F_RMOD, val)

//看门狗初始化
void wdt_user_init(void);
//看门狗失能
void disable_wdt(void);


#endif
