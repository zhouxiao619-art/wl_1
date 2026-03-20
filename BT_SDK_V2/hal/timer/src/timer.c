#include "timer.h"
#include "uart.h"
#include "gpio.h"
#include "app.h"
#include "pwm.h"

uint32_t timer_flay = 0x00;

//复位定时器计数变量（关机或者停止定时器时候使用）
void reset_timer_flay(void)
{
	timer_flay = 0x00;	
}

//获取当前计数值
uint32_t get_timer_flay(void)
{
	return timer_flay;
}

void device_off_timer_reset(void)
{
	reset_timer_flay();
	user_timer3_stop();
}

/**
 * @brief 切换电机频率计时器设置开启
 * @param void
 */
void user_timer3_start(void)
{
	SET_SYS_NO_SLEEP_EVT(KEY_PWM_TIMER_EVENT);
	timer_en(PWM_MODE_TIMEOUT, ENABLE);
}

/**
 * @brief 切换电机频率计时器停止
 * @param void
 */
void user_timer3_stop(void)
{
	timer_en(PWM_MODE_TIMEOUT, DISABLE);
	CLEAR_SYS_NO_SLEEP_EVT(KEY_PWM_TIMER_EVENT);
}

//定时器中断处理
void TIMER_IRQHandler()
{
    if(timer_it_status(PWM_MODE_TIMEOUT))
    {
		if(timer_flay < 0x3FFFFFFF){
			timer_flay++;
		}
        timer_it_clear(PWM_MODE_TIMEOUT);
    }
    NVIC_ClearPendingIRQ(Timer_IRQn);
}


