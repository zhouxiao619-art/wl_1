#ifndef _LED_H_
#define _LED_H_

#include "cpu.h"
#include "gpio.h"

#define LED_NUM		1

#define LED1_2_IO		GPIO_PA3
//#define LED3_6_IO		GPIO_PA1

typedef enum
{
	LIGHT_STATE_OFF = 0,
	LIGHT_STATE_ON,
}led_flag_e;

typedef enum
{
	LED_OFF = 0,
	LED_ON,
}led_level_e;

typedef enum
{
	LED_PRO_NOT_TIMEOUT = 0,
	LED_PRO_WITH_TIMEOUT = 1,
}led_pro_timeout_en;

typedef enum
{
	LED_1 = 0x00,
	LED_2 = 0x01,
	LED_3 = 0x02,
	LED_ALL = 0xFF,
}led_num_t;

void led_init(void);

void led_1_2_charge_full(void);
void led_1_2_charge_unplugged(void);
void led_1_2_charge_charging(void);

void set_led_level(uint8_t level);
void user_led_set(uint8_t led_num, uint8_t en);
void set_led_parma(uint32_t led_time,uint16_t led_num, uint8_t led, uint8_t timeout_en);

void reset_led_all_flay(void);

void set_led_work_enable(uint8_t num, uint8_t en);
void set_led_enable_flay(uint8_t num);
void reset_led_enable_flay(uint8_t num);
uint8_t get_led_enable_flay(uint8_t num);
void led_fun(void);

uint16_t led_work_busy(void);

#endif
