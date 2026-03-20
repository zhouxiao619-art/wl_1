/**
******************************************************************************************************************************************************************************************
*@file                       timer.h
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

#ifndef TIMER_H_
#define TIMER_H_

#include "cpu.h"


#define TIM0                0
#define TIM1                1
#define TIM2                2
#define TIM3                3
#define TIM4                4
#define TIM5                5

/* TIM对外接口 */
/*
 * @brief timer的使能和失能
 * @param
 *         timn: TIM0 -- TIM5
 *        status: ENABLE/DISABLE
 */
#define timer_en(timn, status)            WS_WRITE_FIELD2(TIMER_BASE, 0x00, timn, timn, status)
/* 获取当前定时器的使能状态 */
#define timer_en_now(timn)                WS_READ_FIELD2(TIMER_BASE, 0x00, timn, timn)

/*
 * @brief 设置PWM输出的初始电平
 * @param
 *        timn: TIM0 -- TIM5
 *         le:   low/high
 */
#define timer_togg_pol(timn, le)        WS_WRITE_FIELD2(TIMER_BASE, 0x00, timn+8, timn+8, le)

/*
 * @brief timer重装载使能，每次修改重装载值后需要重新使能
 * @param
 *        timn: TIM0 -- TIM5
 *        status: ENABLE/DISABLE
 */
#define timer_update_en(timn, status)    WS_WRITE_FIELD2(TIMER_BASE, 0x04, timn, timn, status)

/*
 * @brief timer预分频值设置
 * @param
 *        timn: TIM0 -- TIM5
 *        val:  预分频值，0 -- 65535
 */
#define timer_prescaler(timn, val)        WS_WRITE_FIELD2(TIMER_BASE, (timn + 1)*0x10, 0, 15, val)

/*
 * @brief timer重装载值设置
 * @param
 *        timn: TIM0 -- TIM5
 *        val:  重装载值，0 -- 65535
 */
#define timer_reload_cnt(timn, val)     WS_WRITE_FIELD2(TIMER_BASE, (timn + 1)*0x10, 16, 31, val)

/*
 * @brief 设置timer的预分频值和重装载值
 * @param
 *        timn: TIM0 -- TIM5
 *        pcl:  预分频值，0 -- 65535
 *      rel:  重装载值，0 -- 65535
 */
#define timer_pcl_reload(timn, pcl, rel) WS_WRITE_FIELD3_NO_READBACK(TIMER_BASE, (timn + 1)*0x10, (rel << 16) | pcl)

/*
 * @brief 设置timer中断出发值
 * @param
 *        timn:
 *        togv: 触发值
 */
#define timer_toggle_val(timn, togv)    WS_WRITE_FIELD3_NO_READBACK(TIMER_BASE, timn * 0x10 + 0x14, togv)

//获取timer当前计数值
#define timer_get_counter(timn)            WS_REG_VAL(TIMER_BASE, timn * 0x10 + 0x18)

//设置中断触发时机- 0：计数到0产生中断；1：计数到togger值产生中断；
#define timer_it_cfg(timn, val)            WS_WRITE_FIELD2(TIMER_BASE, 0x08, timn+8, timn+8, val)

//使能或失能中断 -- ENABLE/DISABLE
#define timer_it_en(timn, val)            WS_WRITE_FIELD2(TIMER_BASE, 0x08, timn, timn, val)

//获取当前中断状态
#define timer_it_status(timn)            WS_READ_FIELD2(TIMER_BASE, 0x0C, timn, timn)
#define timer_it_clear(timn)            WS_WRITE_FIELD2(TIMER_BASE, 0x0C, timn, timn, 0)

/* basetimer(TIM1)对外接口 */
/* 设置定时器的当前计数值 */
#define basetimer_counter_set(var)        WS_WRITE_FIELD(BSTIM_BASE, BSTIM_F_LONG_TIMER_VAL, var)

/* 读取定时器当前计数值 */
#define basetimer_get_counter()            WS_READ_FIELD(BSTIM_BASE, BSTIM_F_LONG_TIMER_VAL)

/* 0-不分频；1-二分频； */
#define basetimer_div_set(div)            WS_WRITE_FIELD(BSTIM_BASE, BSTIM_F_TIMER_DIV, div)

/* 中断比较值设置，当定时器计数值与该值相等时产生中断 */
#define basetimer_comp_set(var)            WS_WRITE_FIELD(BSTIM_BASE, BSTIM_F_TIMER_COMP_SET, var)

/* 清除中断标志 */
#define basetimer_it_clear()            WS_WRITE_FIELD(BSTIM_BASE, BSTIM_F_INTP_CLR, 1)

//ENABLE/DISABLE 中断使能和失能
#define basetimer_it_set(var)            WS_WRITE_FIELD(BSTIM_BASE, BSTIM_F_INTP_MASK, !var)

//ENABLE/DISABLE 定时器使能和失能
#define basetimer_en(var)                WS_WRITE_FIELD(BSTIM_BASE, BSTIM_F_LONG_TIMER_EN, var)

//获取当前中断状态
#define basetimer_it_status()            WS_READ_FIELD(BSTIM_BASE, BSTIM_F_TIMER_INT_STATUS)

//获取进入休眠时basetimer采样值
#define basetimer_sleep_sample_val()    WS_READ_FIELD(SYSCFG_BASE, SYSCFG_F_BSTIM_CUR_VAL)

void reset_timer_flay(void);
uint32_t get_timer_flay(void);

void device_off_timer_reset(void);

void user_timer3_start(void);
void user_timer3_stop(void);

void user_timer_init(void);

#endif
