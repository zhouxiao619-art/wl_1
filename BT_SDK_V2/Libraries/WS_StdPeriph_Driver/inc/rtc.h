/**
******************************************************************************************************************************************************************************************
*@file                       rtc.h
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

#ifndef RTC_H_
#define RTC_H_

#include "cpu.h"

//设置rtc的比较值和重装值
#define rtc_cmv_relv(cmv, relv)                            \
    do{                                                    \
        WS_WRITE_FIELD(RTC_BASE, RTC_R_CMR, cmv);         \
        WS_WRITE_FIELD(RTC_BASE, RTC_R_CLR, relv);        \
    }while(0)

//使能和失能RTC -- ENABLE/DISABLE
#define rtc_en(val)                                        \
    do{                                                    \
        WS_WRITE_FIELD(RTC_BASE, RTC_F_RTC_EN, val);    \
        WS_WRITE_FIELD(RTC_BASE, RTC_F_RTC_WEN, val);    \
    }while(0)

//RTC中断使能和失能 -- ENABLE/DISABLE
#define rtc_it_en(val)                                    \
    do{                                                    \
        WS_WRITE_FIELD(RTC_BASE, RTC_F_RTC_IEN, val);    \
        WS_WRITE_FIELD(RTC_BASE, RTC_F_RTC_MASK, !val);    \
    }while(0)

//获取中断状态
#define rtc_it_status()            WS_READ_FIELD(RTC_BASE, RTC_F_RTC_STAT)

//清除中断状态
#define rtc_it_clear()            WS_READ_FIELD(RTC_BASE, RTC_F_RTC_EOI)

//获取休眠时刻RTC的采样值
#define rtc_sleep_sample_val()    WS_READ_FIELD(SYSCFG_BASE, SYSCFG_F_RTC_CUR_VAL)

//获取RTC当前计数值
#define rtc_current_cnt_val()       WS_READ_FIELD(RTC_BASE, RTC_F_RTC_CCRV)

#endif
