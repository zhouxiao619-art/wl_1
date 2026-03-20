#include <stdio.h>
#include <stdlib.h>
#include "ble_gatt.h"
#include "ble_gap.h"
#include "ble_scan.h"
#include "app_config.h"
#include "lib_api.h"
#include "gpio.h"
#include "uart.h"
#include "wdt.h"
#include "sleep.h"
#include "timer.h"
#include "gpadc.h"
#include "app.h"
#include "key.h"
#include "led.h"
#include "pwm.h"
#include "charge.h"

// version V1.4  2026.2.20 09时17分
// version V1.5  2026.2.20 15时56分

void SystemInit(void)
{
	//此接口不需要做任何操作
}


int main()
{
	cpu_crm_set(CRM_F_GPIO_EN, ENABLE, CRM_F_GPADC_EN, ENABLE);
	cpu_crm_reset(CRM_F_GPIO_SRST, ENABLE, CRM_F_GPADC_SRST, ENABLE);
	
//	//设置晶振频率：16M晶振传0，32M晶振传1
	soc_xtal_freq_check(0);
	
	soc_init(1);
	
	for(uint8_t i=0; i<GPIO_PB7; i++)
	{
		gpio_remapmode_set(i, GPIO_PINMUX_GPIO);
		gpio_pinmode_set(i, GPIO_INOUT_MODE_DROPDOWN_IN);
	}
	
	//串口初始化
	#if APP_UART_ENABLE
		uart_user_init();
	#endif
	
	DEMO_PRINTF("0x500050F4 = %x\n", *(volatile unsigned int *)0x500050F4);
	
	//GATT初始化
	user_ble_gatt_init();
	
	//GAP初始化
	user_ble_gap_init();
	
	//扫描初始化
	user_ble_scan_init();
	
	//看门狗初始化
	#if APP_ENABLE_WDT
		wdt_user_init();
	#else
		disable_wdt();
	#endif
	
	//睡眠初始化
	#if APP_ENABLE_SLEEP
		user_sleep_init();
	#endif
	
	//晶振校准
//	set_xtal_cap(0xBA);		//针对32M晶振校准
	set_xtal_cap(0xCD);		//针对16M晶振校准
	
	//设置发射功率
	lib_set_power(TX_POWER_0);
	
	//sm安全启用 
	lib_sm_security(ENABLE);
	
	//接收数据处理事件初始化
	user_recv_event_init();
	
	//系统循环事件定时器初始化
	system_time_init();
	
	//按键初始化
	user_key_init();
	
	//LED初始化
	led_init();
	
	//PWM初始化
	user_pwm_timer_init();
	
	//充电管理初始化
	user_charge_init();
	
//	//ADC初始化
//	#if DEVICE_ADC_EN
//		user_adc_init();
//	#endif
	
	pwm_mode_flay_init();
	
	pwm_mode_switch_en_init();
	
	pwm_loop_switch_mode_init();
	
	DEMO_PRINTF("get_lib_version=%x\n", get_lib_version());
	
	while(1)
	{
		//喂狗
		#if APP_ENABLE_WDT
			wdt_feed_dog();
		#endif
		
		//处理系统事件
		system_event_process();
		
		//处理蓝牙栈事件
		lib_stack_process();
	}
}

