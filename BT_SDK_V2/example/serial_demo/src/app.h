#ifndef _APP_H_
#define _APP_H_

#include "cpu.h"
#include "lib_api.h"

extern volatile uint8_t system_timer_event;
extern ktimer_t power_timer;
extern volatile uint32_t system_prevent_event;

typedef enum {
	KEY_PRESS_EVENT			= 0x01,		//按键事件
	KEY_LED_BLINK_EVENT		= 0x02,		//灯闪事件
	CHARGE_EVENT			= 0x04,		//充电事件
	LONG_TIME_EVENT			= 0x10,		//连接空闲唤醒
	LATENCY_EVENT 			= 0x20,		//更新连接参数事件
	FAST_ADV_EVENT			= 0x40,		//快广播事件
}SYSTEM_EVENT;

typedef enum {
	PWM_1_WORK_EVENT 	= 0x01,
	PWM_2_WORK_EVENT 	= 0x02,
	PWM_3_WORK_EVENT 	= 0x04,
	KEY_PWM_TIMER_EVENT = 0x08,
	BLE_DISCONNED_EVENT	= 0x10,
	DEVICE_CHARGE_EVENT	= 0x20,
	ALL_NO_SLEEP_EVENT	= 0xFF,
}SYSTEM_NO_SLEEP_EVENT;

#define SET_RTC_EVENT(v)		(system_timer_event |= v)
#define CLEAR_RTC_EVENT(v)		(system_timer_event &= ~(v))

#define SET_SYS_NO_SLEEP_EVT(v)			(system_prevent_event |= v)
#define CLEAR_SYS_NO_SLEEP_EVT(v)		(system_prevent_event &= ~(v))

#define RTC_TASK_ID				&power_timer 
#define RTC_TASK_UNIT   		20		//单位ms 
#define RTC_LONGTIME_UNIT    	1000	//单位ms 
#define RTC_SLEEPTIME_UNIT    	20000	//单位ms

typedef enum {
//	WARM_CMD				=0x02,	//加热
	BATTERY_LEVEL_CMD		=0x04,	//电池电量
	M1_FIXATION_MODE_CMD	=0x05,	//1号电机固定模式
	M1_FREEDOM_MODE_CMD		=0x06,	//1号电机自由模式
	M2_FIXATION_MODE_CMD	=0x07,	//2号电机固定模式
	M2_FREEDOM_MODE_CMD		=0x08,	//2号电机自由模式
	M3_FIXATION_MODE_CMD	=0x09,	//3号电机固定模式
	M3_FREEDOM_MODE_CMD		=0x0A,	//3号电机自由模式
//	M4_FIXATION_MODE_CMD	=0x0B,	//4号电机固定模式
//	M4_FREEDOM_MODE_CMD		=0x0C,	//4号电机自由模式
//	M5_FIXATION_MODE_CMD	=0x0D,	//5号电机固定模式
//	M5_FREEDOM_MODE_CMD		=0x0E,	//5号电机自由模式
}COMMAND_ID;

typedef enum {
	DEVICE_OFF = 0,
	DEVICE_ON,
}DEVICE_MODE_T;

typedef enum {
	WORK_MODE_1 = 0,	//快闪
	WORK_MODE_2,		//慢闪
	WORK_MODE_3,
	WORK_MODE_4,
}WORK_MODE_T;

typedef enum {
	FAST_ADV = 0,
	SLOW_ADV,
}ADV_MODE_T;

typedef enum {
	KEY_PWM_MODE_STOP = 0,
	KEY_PWM_MODE_ONE = 1,
	KEY_PWM_MODE_TWO = 2,
	KEY_PWM_MODE_TREE = 3,
	KEY_PWM_MODE_FOUR = 4,
	KEY_PWM_MODE_FIVE = 5,
	KEY_PWM_MODE_SIX = 6,
	KEY_PWM_MODE_SEVEN = 7,
	KEY_PWM_MODE_EIGHT = 8,
	KEY_PWM_MODE_NINE = 9,
	KEY_PWM_MODE_ON_OFF = 0xFF,
}KEY_PWM_MODE_T;

typedef enum {
	M_FREE = 0,
	M1 = 1,
	M2 = 2,
	M3 = 3,
	M_ALL = 4,
}MOTTOR_NUM;

//开关机闪灯频率和次数
#define DEVICE_OFF_LED_TIME     		0x00090009
#define DEVICE_OFF_LED_BLINK   			0x0600


//清空闪灯频率和次数
#define DEVICE_LED_OFF_TIME     		0x00000000
#define DEVICE_LED_OFF_BLINK   			0x0200

//按键闪灯
#define DEVICE_LED_KEY_WORK_TIME     	0x0000000A
#define DEVICE_LED_KEY_WORK_BLINK   	0x0200

#define DEVICE_OFF_TIMEOUT				600000

void reset_time_tick(void);

DEVICE_MODE_T get_device_mode(void);
void set_device_mode(DEVICE_MODE_T mode);


uint16_t calculate_crc_with_mac(uint8_t random_byte, const uint8_t *mac);
void process_all_crc_calculations(uint8_t *random, uint8_t *mac, uint8_t *app_crc, uint8_t *device_crc);

uint8_t get_recv_event_busy(void);

void reset_all_app_mode_en_flay(void);
uint8_t get_app_mode_en_flay(uint8_t num);
void set_app_mode_en_flay(uint8_t num, uint8_t data);

void user_recv_event_init(void);
void user_start_recv_event(void);
//void user_stop_recv_event(void);
void system_time_init(void);
void system_event_process(void);

void user_pwm_stop(uint8_t num);
void all_pwm_mode_pro(uint8_t num);
void set_ack_data(uint8_t i, uint8_t data);
void ble_recv_ack_data_pro(uint8_t *data, uint8_t len);
void pwm_fixed_mode_ack_data_pro(uint8_t pwm_mode, uint8_t pwm_en, uint8_t pwm_num);

uint8_t get_adc_low_en(void);
void set_adc_low_en(uint8_t en);

#endif
