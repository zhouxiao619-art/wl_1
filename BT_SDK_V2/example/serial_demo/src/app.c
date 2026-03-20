#include "app.h"
#include <stdio.h>
#include <stdlib.h>
#include "ble_gatt.h"
#include "ble_gap.h"
#include "app_config.h"
#include "lib_api.h"
#include "gpio.h"
#include "uart.h"
#include "wdt.h"
#include "sleep.h"
#include "timer.h"
#include "gpadc.h"
#include "led.h"
#include "key.h"
#include "pwm.h"
#include "charge.h"
#include "ble_scan.h"

uint8_t recv_event_busy = 0;								//接收事件忙标记
event_t recv_event;											//接收事件结构体
void user_recv_event_pro(void *cb_dat, event_t *e);			//接收事件回调函数

ktimer_t power_timer;										//系统定时器事件
void system_looper(void *cb_dat, event_t *e);				//系统定时器事件回调函数

volatile uint8_t system_timer_event 	= 0;				//系统定时器事件标记
volatile uint8_t system_time_unit_flag 	= 1;				//系统时间单位标志
volatile uint16_t rtc_timer 			= RTC_TASK_UNIT;	//系统定时器单位设置
volatile uint16_t last_rtc_timer 		= RTC_TASK_UNIT;	//系统定时器单位记录值
uint32_t time_tick						= 0;				//系统定时实际值存储变量
uint32_t adc_time_tick					= 0;				//ADC检测定时标志值

volatile uint32_t system_prevent_event 	= 0;				//系统防睡眠事件

uint32_t disconnect_timeout				= 0;				//应用层断开连接超时计数

DEVICE_MODE_T device_mode 				= DEVICE_OFF;		//设备系统状态
DEVICE_MODE_T last_device_mode 			= DEVICE_OFF;		//设备状态记录

uint16_t stop_rtc_cnt					= 0;				//停止状态下的计数变量

uint8_t app_mode_en_flay[MOTOR_NUMBER];						//APP模式使能标记

pwm_config pwm_config_fifo[15] = {
	{.duty=410,		.period=320,	.reload=1000,	.timeout=0		},	//模式1 0档
	
	{.duty=700,		.period=320,	.reload=1000,	.timeout=0		},	//模式2 1档
	
	{.duty=1000,	.period=320,	.reload=1000,	.timeout=0		},	//模式3 2档
	
	{.duty=3450,	.period=3200,	.reload=3970,	.timeout=0		},	//模式4 3档
	
	{.duty=7070,	.period=3200,	.reload=7590,	.timeout=0		},	//模式5 4档
	
	{.duty=0,		.period=320,	.reload=1000,	.timeout=52		},	//模式6 5档
	{.duty=1000,	.period=320,	.reload=1000,	.timeout=2000	},	//模式6 6档
	{.duty=2400,	.period=320,	.reload=3400,	.timeout=34		},	//模式6 7档
	
	{.duty=260,		.period=320,	.reload=1000,	.timeout=180	},	//模式7 8档
	{.duty=3797,	.period=3200,	.reload=3798,	.timeout=379	},	//模式7 9档
	
	{.duty=5000,	.period=3200,	.reload=10000,	.timeout=0		},	//模式8 10档
	
	{.duty=10000,	.period=3200,	.reload=10000,	.timeout=1000	},	//模式9 11档
	{.duty=0,		.period=3200,	.reload=10000,	.timeout=1000	},	//模式9 12档
	
	{.duty=120,		.period=3200,	.reload=200,	.timeout=500	},	//开关模式 13档
	{.duty=0,		.period=3200,	.reload=200,	.timeout=500	},	//开关模式 14档
};

pwm_config change_pwm_config_fifo[2] = {
	{.duty=0,		.period=3200,	.reload=1111,	.timeout=110	},	//停止模式配置
	{.duty=0,		.period=3200,	.reload=200,	.timeout=0		},	//自由模式配置
};

void reset_all_app_mode_en_flay(void)
{
	for(uint8_t i=0; i<MOTOR_NUMBER; i++){
		app_mode_en_flay[i] = 0x00;
	}
}

uint8_t get_app_mode_en_flay(uint8_t num)
{
	if(num<=MOTOR_NUMBER){
		num = MOTOR_NUMBER-1;
	}
	return app_mode_en_flay[num];
}

void set_app_mode_en_flay(uint8_t num, uint8_t data)
{
	if(num<=MOTOR_NUMBER){
		num = MOTOR_NUMBER-1;
	}
	app_mode_en_flay[num] = data;
	DEMO_PRINTF("app_mode_en_flay[%d] = %d\n", num, data);
}

void reset_time_tick(void)
{
	time_tick = 0x00;
	adc_time_tick = 0x00;
}

DEVICE_MODE_T get_device_mode(void)
{
	return device_mode;
}

void set_device_mode(DEVICE_MODE_T mode)
{
	device_mode = mode;
}


uint8_t get_recv_event_busy(void)
{
	return recv_event_busy;
}

void user_recv_event_init(void)
{
	lib_event_init(&recv_event, user_recv_event_pro, EVENT_PRIORITY_MIDDLE_GROUP);
}

void user_start_recv_event(void)
{
	recv_event_busy = 0x01;
	lib_event_post(&recv_event);
}

extern uint8_t ble_recv_data[9];			//????????????????
extern uint16_t ble_recv_length;			//??????????????? 
extern uint16_t service_16bit_tx_handle;	//???????handle

uint8_t ack_data[9] = {0xAA, 0xE2, 	//??? 00 01
					   0x00,		//??? 02
					   0x01,		//?????λ??????? 03
					   0x00,0x00,	//??? 04 05
					   0x00,		//??????????1 ??   0 ??/ADC???? 06
					   0x00,		//????????????/?????1-9/ADC???? 07
					   0xAA			//??? 08
};

void set_ack_data(uint8_t i, uint8_t data)
{
	if(i>=9){
		return;
	}
	ack_data[i] = data;
}

void ble_recv_ack_data_pro(uint8_t *data, uint8_t len)
{
	if((data[1] == 0xE1) && (data[3] == 0x01)){
		if(data[2] == 0x04){	//???????
			memcpy(ack_data, data, len);		//????????
			
			//????????????????????
			#if DEVICE_ADC_EN
//				uint32_t adc_num = 0;
//				adc_num = user_get_adc();
//				if(adc_num <= 505){
//					set_ack_data(6, 0x00);
//					set_ack_data(7, 0x00);
//				}else{
//					adc_num = ((((adc_num*1200)/1024)*4)+85);
//					set_ack_data(6, ((adc_num>> 8)&0xFF));
//					set_ack_data(7, (adc_num&0xFF));
//				}
				set_ack_data(6, 0x0E);
				set_ack_data(7, 0xCB);
			#else
				set_ack_data(6, 0x0E);
				set_ack_data(7, 0xCB);
			#endif
		}else{	//?????
			memcpy(ack_data, data, len);	//????????
			//????????????????????
			switch(data[2])
			{
				case 0x05:{
					ack_data[6] = get_pwm_work_en_flay(MOTOR1);
					if(data[7] == 0x00){
						uint8_t mode = get_pwm_mode_flay(MOTOR1);
						if(mode==0x00){
							mode = 0x01;
						}
						ack_data[7] = mode;
					}
				}break;
				
				case 0x07:{
					ack_data[6] = get_pwm_work_en_flay(MOTOR2);
					if(data[7] == 0x00){
						uint8_t mode = get_pwm_mode_flay(MOTOR2);
						if(mode==0x00){
							mode = 0x01;
						}
						ack_data[7] = mode;
					}
				}break;
				
				case 0x09:{
					ack_data[6] = get_pwm_work_en_flay(MOTOR3);
					if(data[7] == 0x00){
						uint8_t mode = get_pwm_mode_flay(MOTOR3);
						if(mode==0x00){
							mode = 0x01;
						}
						ack_data[7] = mode;
					}
				}break;
			}
			
		}
		ack_data[1] = 0xE2;
		user_ble_send_data_notify(ack_data, sizeof(ack_data));
	}
}

//pwm_mode:PWM??  pwm_en:PWM????  pwm_num:PWM???0-5
void pwm_fixed_mode_ack_data_pro(uint8_t pwm_mode, uint8_t pwm_en, uint8_t pwm_num)
{
	ack_data[1] = 0xE2;	//?????????
	switch(pwm_num)
	{
		case MOTOR1:{ack_data[2] = 0x05; }break;
		case MOTOR2:{ack_data[2] = 0x07; }break;
		case MOTOR3:{ack_data[2] = 0x09; }break;
	}
	ack_data[4] = ack_data[5] = 0x00;
	ack_data[6] = pwm_en;
	
	if(pwm_mode==0x00){
		ack_data[7] = 0x01;
	}else{
		ack_data[7] = pwm_mode;
	}
	
	user_ble_send_data_notify(ack_data, sizeof(ack_data));	//???????
}

uint8_t adc_low_en = 0x00;

uint8_t get_adc_low_en(void)
{
	return adc_low_en;
}

void set_adc_low_en(uint8_t en)
{
	adc_low_en = en;
}

////????????????
//void adc_num_pro(uint32_t num)
//{
//	uint8_t user_adc_class;
//	ack_data[1] = 0xE2;	//?????????
//	ack_data[2] = 0x04;	//???????
//	ack_data[3] = 0x00;
//	#if DEVICE_ADC_EN
//		if(num < 150){
//			set_ack_data(6, 0x00);
//			set_ack_data(7, 0x00);
//		}else{
//			if(num <= 505){
//				set_ack_data(6, 0x00);
//				set_ack_data(7, 0x00);
//				if(device_mode != DEVICE_OFF){
//					device_mode = DEVICE_OFF;
//					adc_low_en = 0x01;
//				}
//			}else{
//				if(num <= 590){
//					if(device_mode != DEVICE_OFF){
//						device_mode = DEVICE_OFF;
//						adc_low_en = 0x01;
//					}
//				}else{
//					user_adc_class = user_adc_num_pro(num);
//					user_adc_ack_data_pro(user_adc_class, ack_data);
//					if(lib_stack_state_get() == LIB_STACKSTATE_CONN){
//						user_ble_send_data_notify(ack_data, sizeof(ack_data));
//					}
//				}
//				
//			}
//		}
//	#else
//		set_ack_data(6, 0x0E);
//		set_ack_data(7, 0xCB);
//		
//		if(lib_stack_state_get() == LIB_STACKSTATE_CONN){
//			user_ble_send_data_notify(ack_data, sizeof(ack_data));
//		}
//	#endif
//}

void recv_data_duty_pro(uint16_t data)
{
	change_pwm_config_fifo[1].duty = (((data + 1) * 143) / 254) + 56;
}

/**
 * @brief ?????????????????????????
 */
void user_recv_event_pro(void *cb_dat, event_t *e)
{
	DEMO_PRINTF("length = %d\n", ble_recv_length);
	for(uint16_t i=0; i<ble_recv_length; i++){
		DEMO_PRINTF("%02x ", ble_recv_data[i]);
	}
	DEMO_PRINTF("\n");
	
	switch(ble_recv_data[2])
	{
//		case WARM_CMD:
//		{
//			
//		}break;
		
		case BATTERY_LEVEL_CMD:
		{
			ble_recv_ack_data_pro(ble_recv_data, sizeof(ble_recv_data));
		}break;
		
		#if M1_ENABLE
			case M1_FIXATION_MODE_CMD:
			{
				reset_time_tick();
				if(ble_recv_data[6]){
					if(ble_recv_data[7] != 0x00){
						if(get_pwm_mode_flay(MOTOR1) == ble_recv_data[7]){
							set_pwm_mode_switch_en(MOTOR1, ENABLE);
							set_pwm_loop_switch_mode(MOTOR1, 0x00);
						}else{
							set_pwm_mode_flay(MOTOR1, ble_recv_data[7]);
							set_pwm_loop_switch_mode(MOTOR1, 0x00);
						}
					}
					set_pwm_work_en_flay(MOTOR1, ENABLE);
					//led????
					set_led_parma(DEVICE_LED_KEY_WORK_TIME, DEVICE_LED_KEY_WORK_BLINK, LED_1, LED_PRO_WITH_TIMEOUT);
					set_app_mode_en_flay(MOTOR1, ble_recv_data[6]);
				}else{
					//??????????ο???????
					if(get_pwm_work_en_flay(MOTOR1)){
						set_pwm_work_en_flay(MOTOR1, DISABLE);
						pwm_stop_mode_pro(MOTOR1);
					}
					user_pwm_stop(MOTOR1);
					set_pwm_loop_switch_mode(MOTOR1, 0x00);
					//保持app_mode_en_flay为1，以便按键切换模式时仍然向APP上报数据
				}
				ble_recv_ack_data_pro(ble_recv_data, sizeof(ble_recv_data));
			}break;
			
			case M1_FREEDOM_MODE_CMD:
			{
				reset_time_tick();
				if(ble_recv_data[7] == 0x00){
					DEMO_PRINTF("M1_FREEDOM_MODE_CMD stop\n");
					user_pwm_stop(MOTOR1);
					set_pwm_loop_switch_mode(MOTOR1, 0x00);
					//保持app_mode_en_flay为1，以便按键切换模式时仍然向APP上报数据
				}else{
					recv_data_duty_pro(ble_recv_data[7]);
					DEMO_PRINTF("M1 duty=%d\n", change_pwm_config_fifo[1].duty);
					if(get_pwm_mode_flay(MOTOR1) != KEY_PWM_MODE_STOP){
						set_pwm_mode_flay(MOTOR1, KEY_PWM_MODE_STOP);
						set_old_pwm_mode_flay(MOTOR1, KEY_PWM_MODE_STOP);
						set_pwm_loop_switch_mode(MOTOR1, 0x00);
					}
					user_pwm_param_config(MOTOR1, &change_pwm_config_fifo[1]);
					set_app_mode_en_flay(MOTOR1, 1);
				}
				
				ble_recv_ack_data_pro(ble_recv_data, sizeof(ble_recv_data));
			}break;
		#endif
		
		#if M2_ENABLE
			case M2_FIXATION_MODE_CMD:
			{
				reset_time_tick();
				if(ble_recv_data[6]){
					//led????
					if(get_pwm_work_en_flay(MOTOR2)){		//?????????????????0.3??
						set_led_parma(DEVICE_LED_KEY_WORK_TIME, DEVICE_LED_KEY_WORK_BLINK, LED_2, LED_PRO_WITH_TIMEOUT);
					}else{				//??????????????????
						user_led_set(LED_2, ENABLE);
					}
					
					if(ble_recv_data[7] != 0x00){
						if(get_pwm_mode_flay(MOTOR2) == ble_recv_data[7]){
							set_pwm_mode_switch_en(MOTOR2, ENABLE);
							set_pwm_loop_switch_mode(MOTOR2, 0x00);
						}else{
							set_pwm_mode_flay(MOTOR2, ble_recv_data[7]);
							set_pwm_loop_switch_mode(MOTOR2, 0x00);
						}
					}
					set_pwm_work_en_flay(MOTOR2, ENABLE);
					set_app_mode_en_flay(MOTOR2, ble_recv_data[6]);
				}else{
					//??????????ο???????
					if(get_pwm_work_en_flay(MOTOR2)){
						set_pwm_work_en_flay(MOTOR2, DISABLE);
						pwm_stop_mode_pro(MOTOR2);
					}
					user_pwm_stop(MOTOR2);
					set_pwm_loop_switch_mode(MOTOR2, 0x00);
					
					//led????
					user_led_set(LED_2, DISABLE);
					//保持app_mode_en_flay为1，以便按键切换模式时仍然向APP上报数据
				}
				
				ble_recv_ack_data_pro(ble_recv_data, sizeof(ble_recv_data));
			}break;
			
			case M2_FREEDOM_MODE_CMD:
			{
				reset_time_tick();
				if(ble_recv_data[7] == 0x00){
					DEMO_PRINTF("M2_FREEDOM_MODE_CMD stop\n");
					user_pwm_stop(MOTOR2);
					set_pwm_loop_switch_mode(MOTOR2, 0x00);
					//保持app_mode_en_flay为1，以便按键切换模式时仍然向APP上报数据
				}else{
					recv_data_duty_pro(ble_recv_data[7]);
					DEMO_PRINTF("M2 duty=%d\n", change_pwm_config_fifo[1].duty);
					if(get_pwm_mode_flay(MOTOR2) != KEY_PWM_MODE_STOP){
						set_pwm_mode_flay(MOTOR2, KEY_PWM_MODE_STOP);
						set_old_pwm_mode_flay(MOTOR2, KEY_PWM_MODE_STOP);
						set_pwm_loop_switch_mode(MOTOR2, 0x00);
					}
					user_pwm_param_config(MOTOR2, &change_pwm_config_fifo[1]);
					set_app_mode_en_flay(MOTOR2, 1);
				}
				
				ble_recv_ack_data_pro(ble_recv_data, sizeof(ble_recv_data));
			}break;
		#endif
		
		#if M3_ENABLE
			case M3_FIXATION_MODE_CMD:
			{
				reset_time_tick();
				if(ble_recv_data[6]){
					//led????
					if(get_pwm_work_en_flay(MOTOR3)){		//?????????????????0.3??
						set_led_parma(DEVICE_LED_KEY_WORK_TIME, DEVICE_LED_KEY_WORK_BLINK, LED_3, LED_PRO_WITH_TIMEOUT);
					}else{				//??????????????????
						user_led_set(LED_3, ENABLE);
					}
					
					if(ble_recv_data[7] != 0x00){
						if(get_pwm_mode_flay(MOTOR3) == ble_recv_data[7]){
							set_pwm_mode_switch_en(MOTOR3, ENABLE);
							set_pwm_loop_switch_mode(MOTOR3, 0x00);
						}else{
							set_pwm_mode_flay(MOTOR3, ble_recv_data[7]);
							set_pwm_loop_switch_mode(MOTOR3, 0x00);
						}
					}
					set_pwm_work_en_flay(MOTOR3, ENABLE);
					set_app_mode_en_flay(MOTOR3, ble_recv_data[6]);
				}else{
					//??????????ο???????
					if(get_pwm_work_en_flay(MOTOR3)){
						set_pwm_work_en_flay(MOTOR3, DISABLE);
						pwm_stop_mode_pro(MOTOR3);
					}
					user_pwm_stop(MOTOR3);
					set_pwm_loop_switch_mode(MOTOR3, 0x00);
					
					//led????
					user_led_set(LED_3, DISABLE);
					//保持app_mode_en_flay为1，以便按键切换模式时仍然向APP上报数据
				}
				
				ble_recv_ack_data_pro(ble_recv_data, sizeof(ble_recv_data));
			}break;
			
			case M3_FREEDOM_MODE_CMD:
			{
				reset_time_tick();
				if(ble_recv_data[7] == 0x00){
					DEMO_PRINTF("M3_FREEDOM_MODE_CMD stop\n");
					user_pwm_stop(MOTOR3);
					set_pwm_loop_switch_mode(MOTOR3, 0x00);
					//保持app_mode_en_flay为1，以便按键切换模式时仍然向APP上报数据
				}else{
					recv_data_duty_pro(ble_recv_data[7]);
					DEMO_PRINTF("M3 duty=%d\n", change_pwm_config_fifo[1].duty);
					if(get_pwm_mode_flay(MOTOR3) != KEY_PWM_MODE_STOP){
						set_pwm_mode_flay(MOTOR3, KEY_PWM_MODE_STOP);
						set_old_pwm_mode_flay(MOTOR3, KEY_PWM_MODE_STOP);
						set_pwm_loop_switch_mode(MOTOR3, 0x00);
					}
					user_pwm_param_config(MOTOR3, &change_pwm_config_fifo[1]);
					set_app_mode_en_flay(MOTOR3, 1);
				}
				
				ble_recv_ack_data_pro(ble_recv_data, sizeof(ble_recv_data));
			}break;
		#endif
	}
	
	recv_event_busy = 0x00;
}


void system_time_init(void)
{
	lib_timer_init(RTC_TASK_ID, system_looper, EVENT_PRIORITY_LOWER_GROUP);
}


void system_looper(void *cb_dat, event_t *e)
{	
	if(time_tick < 0x3FFFFFFF){
		time_tick += last_rtc_timer;
	}
	last_rtc_timer = rtc_timer;
	system_time_unit_flag = 1;
	lib_timer_start(RTC_TASK_ID,rtc_timer);
}



static uint8_t l_timer_event = 0x01;	//???????????0x01???????????ι???????
void system_timer_start(uint8_t timer_event)
{
	if(l_timer_event == timer_event){
		return;
	}
	
	l_timer_event = timer_event;
	
	lib_timer_stop(RTC_TASK_ID);
	
	if(timer_event & 0x0F){
		rtc_timer = RTC_TASK_UNIT;
		last_rtc_timer = rtc_timer; 	// ?????????
	}
	else if(timer_event & 0xF0){		
		rtc_timer = RTC_LONGTIME_UNIT;
	}else{
		rtc_timer = RTC_SLEEPTIME_UNIT;
	}
	
	DEMO_PRINTF("rtc_timer %d, timer_event:%x\n",rtc_timer, timer_event);
	lib_timer_start(RTC_TASK_ID,rtc_timer);
}

/**
 * @brief ??????????????
 * @param void
 */
void adv_mode_choose(uint8_t mode)
{
	int i;
	lib_stack_adv_enable(false);
	for(i=0;i<20000;i++);
	lib_stack_set_adv_param(ADV_TYPE_ADV_IND, mode?200:30);	
	lib_stack_adv_enable(true);
}

/**
 * @brief ????????
 * @param void
 */
void system_mode_updata(void)
{
	if(device_mode != last_device_mode) {
		last_device_mode = device_mode;
		reset_time_tick();
		
		if(device_mode) {
			SET_RTC_EVENT(LONG_TIME_EVENT);
			reset_led_all_flay();		//????????????????
			if(lib_stack_state_get() == LIB_STACKSTATE_IDLE) {
				DEMO_PRINTF("device adc strat ---\n");
				lib_stack_adv_enable(true);
				lib_stack_scan_enable(true);
				for(uint8_t i=0; i<MOTOR_NUMBER; i++){
					set_pwm_loop_switch_mode(i, 0x00);
					set_pwm_mode_flay(i, KEY_PWM_MODE_ON_OFF);
				}
				//????LED1_2????
				gpio_set_pin(LED1_2_IO);
				set_led_enable_flay(LED_1);
			}
		}else {
			if(get_charge_state() == 0x00){		//?????????????
				reset_led_all_flay();
				//??????????????????????????????
				if(get_adc_low_en()==0x01){
					set_led_parma(DEVICE_OFF_LED_TIME, DEVICE_OFF_LED_BLINK, LED_1, LED_PRO_NOT_TIMEOUT);
				}else{
					reset_led_all_flay();
					led_init();	//??????
				}
				
			}
			
			for(uint8_t i=0; i<MOTOR_NUMBER; i++){		//?????
				user_pwm_stop(i);
			}
			deivce_off_pwm_reset();		//??λ??????б????????ο????????
			device_off_timer_reset();	//??λ?????
		}
	}
	
	//???????????
	if(device_mode == DEVICE_OFF)
	{
		CLEAR_RTC_EVENT(LONG_TIME_EVENT);
		
		if((get_adc_low_en()==0x01) && (led_work_busy() == 0x00)){
			set_adc_low_en(0x02);
		}
		
		if(lib_stack_state_get() == LIB_STACKSTATE_CONN) {
			if((system_prevent_event & BLE_DISCONNED_EVENT) == 0x00){
				DEMO_PRINTF("device disconnected ---\n");
				SET_SYS_NO_SLEEP_EVT(BLE_DISCONNED_EVENT);
				lib_stack_disconn();			//???????????
				lib_stack_scan_enable(false);	//??????
			}
		}else{
			if(lib_stack_state_get() != LIB_STACKSTATE_IDLE){
				DEMO_PRINTF("device adc stop ---\n");
				lib_stack_adv_enable(false);		//????
				lib_stack_scan_enable(false);		//??????
			}
			
		}
		return;
	}
	
	#if DEVICE_ADC_EN
		if(time_tick - adc_time_tick >= 1000){
			adc_time_tick = time_tick;
			uint32_t adc_num = 0;
			adc_num = gpadc_get_vbat();
			DEMO_PRINTF("adc_num=%d\n", adc_num);
//			adc_num_pro(adc_num);
			if((adc_num>150) && (adc_num<=571)){
				if(get_adc_low_en()==0x00){
					set_adc_low_en(0x01);
				}
				
				if(get_device_mode() == DEVICE_ON){
					set_device_mode(DEVICE_OFF);
				}
			}else{
				set_adc_low_en(0x00);
			}
		}
	#endif
		
	//PWM?л???????
	pwm_timeout_pro();
	
	//PWM??????
	pwm_mode_updata_pro();
	
	//PWM?????????
	for(uint8_t i=0; i<MOTOR_NUMBER; i++){
		all_pwm_mode_pro(i);
	}
}

void user_pwm_stop(uint8_t num)
{
	user_pwm_param_config(num, &change_pwm_config_fifo[1]);
	user_pwm_reset(num);
}

/**
 * @brief PWM??????л?????
 * @param num ??????
 */
void all_pwm_mode_pro(uint8_t num)
{
	if(get_pwm_mode_switch_en(num) == 0x00){
		return;
	}

	switch(get_pwm_mode_flay(num))
	{
		case KEY_PWM_MODE_STOP:
		{
			user_pwm_stop(num);
		}break;
		
		case KEY_PWM_MODE_ONE:
		{
			user_pwm_param_config(num, (pwm_config *)&pwm_config_fifo[0]);
		}break;
		
		case KEY_PWM_MODE_TWO:
		{
			user_pwm_param_config(num, (pwm_config *)&pwm_config_fifo[1]);
		}break;
		
		case KEY_PWM_MODE_TREE:
		{
			user_pwm_param_config(num, (pwm_config *)&pwm_config_fifo[2]);
		}break;
		
		case KEY_PWM_MODE_FOUR:
		{
			user_pwm_param_config(num, (pwm_config *)&pwm_config_fifo[3]);
		}break;
		
		case KEY_PWM_MODE_FIVE:
		{
			user_pwm_param_config(num, (pwm_config *)&pwm_config_fifo[4]);
		}break;
		
		case KEY_PWM_MODE_SIX:
		{
			uint8_t ret = get_pwm_loop_switch_mode(num);
			user_pwm_param_config(num, (pwm_config *)&pwm_config_fifo[5 + ret]);
			if(ret < 2){
				pwm_loop_switch_mode_updata(num);
			}else{
				set_pwm_loop_switch_mode(num, 0x00);
			}
		}break;
		
		case KEY_PWM_MODE_SEVEN:
		{
			uint8_t ret = get_pwm_loop_switch_mode(num);
			if(((ret * 10) + 260) == 990){
				user_pwm_param_config(num, (pwm_config *)&pwm_config_fifo[9]);
				set_pwm_loop_switch_mode(num, 0x00);
			}else{
				pwm_config_fifo[8].duty = ((ret * 10) + 260);
				user_pwm_param_config(num, (pwm_config *)&pwm_config_fifo[8]);
				pwm_loop_switch_mode_updata(num);
			}
		}break;
		
		case KEY_PWM_MODE_EIGHT:
		{
			user_pwm_param_config(num, (pwm_config *)&pwm_config_fifo[10]);
		}break;
		
		case KEY_PWM_MODE_NINE:
		{
			if(get_pwm_loop_switch_mode(num) == 0x00){
				user_pwm_param_config(num, (pwm_config *)&pwm_config_fifo[11]);
				set_pwm_loop_switch_mode(num, 0x01);
			}else if(get_pwm_loop_switch_mode(num) == 0x01){
				user_pwm_param_config(num, (pwm_config *)&pwm_config_fifo[12]);
				set_pwm_loop_switch_mode(num, 0x00);
			}
		}break;
		
		case KEY_PWM_MODE_ON_OFF:
		{
			uint8_t ret = get_pwm_loop_switch_mode(num);
			if(ret >= 2){
				set_pwm_mode_flay(num, KEY_PWM_MODE_STOP);
				set_pwm_loop_switch_mode(num, 0x00);
				return;
			}
			if((ret % 2) == 0x00){
				user_pwm_param_config(num, (pwm_config *)&pwm_config_fifo[13]);
				pwm_loop_switch_mode_updata(num);
			}else{
				user_pwm_param_config(num, (pwm_config *)&pwm_config_fifo[14]);
				pwm_loop_switch_mode_updata(num);
			}
		}break;
	}
}

/**
 * @brief ????????????
 * @param void
 */
void system_time_tick_updata(void)
{
	if(time_tick > DEVICE_OFF_TIMEOUT){
		if(get_device_mode() != DEVICE_OFF) {
			DEMO_PRINTF("device off timeout\n");
			set_device_mode(DEVICE_OFF);
		}
	}
}

/**
 * @brief ?????????
 * @param void
 */
void system_event_process(void)
{
	if(system_time_unit_flag){
		if(stop_rtc_cnt < 300){
			stop_rtc_cnt++;
		}
		
		system_time_tick_updata();		//?????????
		//????ж?????
		if(user_charge_pro() != 0x01){
			if(get_adc_low_en()<=0x01){		//??????????г?????????Ч
				if(key_task(true) == btn_state_work){	//????????
					stop_rtc_cnt = 0x00;
				}
				
				led_fun();		//LED?????????, ??????????????????
			}
		}
		system_time_unit_flag = 0;
	}
	
	//???????????
	system_mode_updata();
	
	//?????????????
	if(gpio_get_pin(CHARGE_WORK_IO)){
		SET_RTC_EVENT(CHARGE_EVENT);
		
		#if CHARGE_ENABLE	
			if(get_charge_state() == CHARGE_STATE_CHARGING){
				charge_pwm_timeout_pro();	//?????л??????
				charge_work_pwm_pro();		//???????л?
			}
		#endif
	}else{
		//??????????????
		if(get_adc_low_en()==0x00){
			if(key_work_get()){
				SET_RTC_EVENT(KEY_PRESS_EVENT);
				reset_time_tick();
			}else{
				if(stop_rtc_cnt > 30){
					CLEAR_RTC_EVENT(KEY_PRESS_EVENT);
					stop_rtc_cnt = 0x00;
				}
			}
		}
	}
	
	//?????????
	system_timer_start(system_timer_event);
	
	//???????
	#if APP_ENABLE_SLEEP
		if(system_prevent_event==0x00){
			lib_stack_sleep();
		}else {
			if((system_prevent_event & BLE_DISCONNED_EVENT) == BLE_DISCONNED_EVENT){
				if(disconnect_timeout++ > 2000){		//?????????????????????????????????????????????
					DEMO_PRINTF("not sleep timeout\n");
					disconnect_timeout = 0;
					CLEAR_SYS_NO_SLEEP_EVT(BLE_DISCONNED_EVENT);
				}
			}
		}
	#endif
}
