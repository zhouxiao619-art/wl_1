/*************************************************************
休眠说明：
开启休眠之后，蓝牙广播可以自动唤醒广播
需要注意的是外设，进入休眠之后外设会被失能
需要的操作：
1.进入休眠之前在hal_system_suspend_devices处理好相关外设
比如：等待串口数据发送完成，停止定时器等
2.退出休眠之后会运行hal_system_resume_devices，在该接口中
重新对外设进行初始化并设置好外设对应的功能即可。
补充说明：
1.进入休眠之后看门狗不会停止，需要定时唤醒喂狗
2.进入休眠时候，GPIO的电平不会发生变化
*************************************************************/

#include "sleep.h"
#include "lib_api.h"
#include "uart.h"
#include "pwm.h"

#if (APP_ENABLE_SLEEP)
/* 系统休眠实现：挂起设备 */
void hal_system_suspend_devices(void)
{    
	/* 休眠时等待串口发送完成 */
	#if APP_UART_ENABLE
		hal_uart_suspend();
	#endif
}

/* 系统唤醒实现：恢复设备 */
void hal_system_resume_devices(void)
{
	#if APP_UART_ENABLE
		/* 重新初始化串口 */
		uart_user_init();
	#endif
	user_pwm_timer_init();
	NVIC_EnableIRQ(GPIO_INT_IRQn);
}
#endif

void user_sleep_init(void)
{
	sleep_type_ctr(SLEEP_TYPE_01);
	//使能休眠
	lib_stack_enter_sleep();
	//使能GPIO唤醒
	sleep_wk_src_en(SYSCFG_F_GPIO_WK_EN, ENABLE);
	
	NVIC_EnableIRQ(GPIO_INT_IRQn);
}

void GPIO_IRQHandler(void)
{
//    if(gpio_get_it_status(SLEEP_WAKE_GPIO))
//    {
//        gpio_clear_it_status(SLEEP_WAKE_GPIO);
//    }
    NVIC_ClearPendingIRQ(GPIO_INT_IRQn);
}

