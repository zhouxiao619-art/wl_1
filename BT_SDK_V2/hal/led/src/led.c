#include "led.h"
#include "uart.h"
#include "app.h"
#include "timer.h"

uint16_t led_on_time[LED_NUM]; 				//LED亮时间
uint16_t led_off_time[LED_NUM];				//LED灭时间
uint8_t led_twinkle_num[LED_NUM]; 			//闪烁次数
uint8_t led_constant_flash_flag[LED_NUM];	//持续闪烁标志位
uint16_t led_tick[LED_NUM];    				//led计时
uint8_t led_flag[LED_NUM];    				//led状态切换标志位
uint16_t led_work_enable[LED_NUM];			//不同led使能位
uint16_t led_enable_flay[LED_NUM];			//不同led状态使能位

ktimer_t led_timer;							//led定时器，用于超时处理
uint8_t led_pro_timeout = 0x00;				//led超时处理标志位

void set_led_work_enable(uint8_t num, uint8_t en)
{
	if(num >= LED_NUM){
		num = LED_NUM-1;
	}
	led_work_enable[num] = en;
}

void set_led_enable_flay(uint8_t num)
{
	if(num >= LED_NUM){
		num = LED_NUM-1;
	}
	led_enable_flay[num] = 0x01;
}

void reset_led_enable_flay(uint8_t num)
{
	if(num >= LED_NUM){
		num = LED_NUM-1;
	}
	led_enable_flay[num] = 0x00;
}

uint8_t get_led_enable_flay(uint8_t num)
{
	if(num >= LED_NUM){
		num = LED_NUM-1;
	}
	return led_enable_flay[num];
}

uint16_t led_work_busy(void)
{
	uint32_t ret = 0;
	for(uint16_t i=0; i<LED_NUM; i++){
		if(led_twinkle_num[i]){
			ret |= (0x01 << i);
		}
		
		if(led_constant_flash_flag[i]){
			ret |= (0x01 << (i+16));
		}
	}
}

void reset_led_all_flay(void)
{
	for(uint8_t i=0; i<LED_NUM; i++){
		led_on_time[i] 				= 0x00; 	//LED亮时间
		led_off_time[i] 			= 0x00; 	//LED灭时间
		led_twinkle_num[i] 			= 0x00; 	//闪烁次数
		led_constant_flash_flag[i] 	= 0x00; 	//持续闪烁标志位
		led_tick[i] 				= 0x00; 	//led计时
		led_flag[i] 				= 0x00; 	//led状态切换标志位
		led_work_enable[i] 			= 0x00; 	//不同led使能位
		led_enable_flay[i] 			= 0x00; 	//不同led状态使能位
	}
}

void led_timer_pro(void *cb_dat, event_t *e)
{	
	if(led_pro_timeout){
		led_pro_timeout = 0x00;
		DEMO_PRINTF("led_pro_timeout=%d\n", led_pro_timeout);
	}
}

void led_timer_init(void)
{
	lib_timer_init(&led_timer, led_timer_pro, EVENT_PRIORITY_LOWER_GROUP);
}

void led_timer_strat(void)
{
	lib_timer_stop(&led_timer);
	lib_timer_start(&led_timer, 200);
}

/**
 * @brief LED?????
 * @param void
 */
void led_init(void)
{
	gpio_remapmode_set(LED1_2_IO, GPIO_PINMUX_GPIO);
	gpio_pinmode_set(LED1_2_IO, GPIO_INOUT_MODE_OUTPUT);
	gpio_reset_pin(LED1_2_IO);
	
	led_timer_init();
}

void led_1_2_charge_full(void)
{
	timer_en(TIM4, DISABLE);
	gpio_remapmode_set(LED1_2_IO, GPIO_PINMUX_GPIO);
	gpio_pinmode_set(LED1_2_IO, GPIO_INOUT_MODE_OUTPUT);
	gpio_set_pin(LED1_2_IO);
}

void led_1_2_charge_unplugged(void)
{
	timer_en(TIM4, DISABLE);
	gpio_remapmode_set(LED1_2_IO, GPIO_PINMUX_GPIO);
	gpio_pinmode_set(LED1_2_IO, GPIO_INOUT_MODE_OUTPUT);
	gpio_reset_pin(LED1_2_IO);
}

void led_1_2_charge_charging(void)
{
	gpio_remapmode_set(LED1_2_IO, GPIO_PINMUX_PWM_CH4);	
	timer_togg_pol(TIM4, low);
	timer_en(TIM4, DISABLE);
}

//此接口用于设置LED状态
void set_led_level(uint8_t level)
{
	//控制LED1_2
	if(led_work_enable[LED_1]==ENABLE){
		set_led_work_enable(LED_1, DISABLE);
		if(level){
			DEMO_PRINTF("set_led_level led1 high\n");
			gpio_set_pin(LED1_2_IO);
			set_led_enable_flay(LED_1);
		}else{
			DEMO_PRINTF("set_led_level led1 low\n");
			gpio_reset_pin(LED1_2_IO);
			reset_led_enable_flay(LED_1);
		}
	}
	
//	if(led_work_enable[LED_2]==ENABLE){ 		//控制LED3_4
//		set_led_work_enable(LED_2, DISABLE);
//		led_3_6_reset();
//		if(level){
//			DEMO_PRINTF("set_led_level led2 high\n");
//			set_led_enable_flay(LED_2);
//			if(led_enable_flay[LED_3]==0x01){
//				led_3_6_set();
//			}else{
//				gpio_remapmode_set(LED3_6_IO, GPIO_PINMUX_GPIO);
//				gpio_pinmode_set(LED3_6_IO, GPIO_INOUT_MODE_OUTPUT);
//				gpio_set_pin(LED3_6_IO);
//			}
//		}else{
//			DEMO_PRINTF("set_led_level led2 low\n");
//			reset_led_enable_flay(LED_2);
//			if(led_enable_flay[LED_3]==0x01){ 		//LED5_6原来处于开启状态需要关闭
//				gpio_remapmode_set(LED3_6_IO, GPIO_PINMUX_GPIO);
//				gpio_pinmode_set(LED3_6_IO, GPIO_INOUT_MODE_OUTPUT);
//				gpio_reset_pin(LED3_6_IO);
//			}
//		}
//	}
//	
//	if(led_work_enable[LED_3]==ENABLE){ 		//控制LED5_6
//		set_led_work_enable(LED_3, DISABLE);
//		led_3_6_reset();
//		if(level){
//			DEMO_PRINTF("set_led_level led3 high\n");
//			set_led_enable_flay(LED_3);
//			if(led_enable_flay[LED_2]==0x01){
//				led_3_6_set();
//			}else{
//				gpio_remapmode_set(LED3_6_IO, GPIO_PINMUX_GPIO);
//				gpio_pinmode_set(LED3_6_IO, GPIO_INOUT_MODE_OUTPUT);
//				gpio_reset_pin(LED3_6_IO);
//			}
//		}else{
//			DEMO_PRINTF("set_led_level led3 low\n");
//			reset_led_enable_flay(LED_3);
//			if(led_enable_flay[LED_2]==0x01){ 	//LED3_4原来处于开启状态需要开启
//				gpio_remapmode_set(LED3_6_IO, GPIO_PINMUX_GPIO);
//				gpio_pinmode_set(LED3_6_IO, GPIO_INOUT_MODE_OUTPUT);
//				gpio_set_pin(LED3_6_IO);
//			}
//		}
//	}
}

//led模式处理函数
void led_mode_pro(void)
{
//	//LED3_4和LED5_6模式需要单独处理
//	if((led_work_enable[LED_2] == 0x00) && (led_work_enable[LED_3] == 0x00)){
//		if((led_enable_flay[LED_2]==0x01) && (led_enable_flay[LED_3]==0x01)){
//				led_3_6_set();
//		}else{
//			led_3_6_reset();
//			if(led_enable_flay[LED_2]==0x01){
//				gpio_remapmode_set(LED3_6_IO, GPIO_PINMUX_GPIO);
//				gpio_pinmode_set(LED3_6_IO, GPIO_INOUT_MODE_OUTPUT);
//				gpio_set_pin(LED3_6_IO);
//			}
//			
//			if(led_enable_flay[LED_3]==0x01){
//				gpio_remapmode_set(LED3_6_IO, GPIO_PINMUX_GPIO);
//				gpio_pinmode_set(LED3_6_IO, GPIO_INOUT_MODE_OUTPUT);
//				gpio_reset_pin(LED3_6_IO);
//			}
//		}
//	}
	
	//LED1_2模式需要单独处理
	if(led_work_enable[LED_1] == 0x00){
		if(led_enable_flay[LED_1]==0x01){
			gpio_set_pin(LED1_2_IO);
		}else{
			gpio_reset_pin(LED1_2_IO);
		}
	}
}

//需要闪烁的LED、占空比和闪烁次数使用此接口
void set_led_parma(uint32_t led_time,uint16_t led_num, uint8_t led, uint8_t timeout_en)
{
	lib_timer_stop(&led_timer);
	
	if(led_num > 0){
		SET_RTC_EVENT(KEY_LED_BLINK_EVENT);
	}else{
		CLEAR_RTC_EVENT(KEY_LED_BLINK_EVENT);
	}
	
	led_on_time[led]  				= (led_time >> 16) & 0xFFFF;
	led_off_time[led] 				= led_time & 0xFFFF;
	
	led_twinkle_num[led]			= (led_num >> 8) & 0xFF;
	led_constant_flash_flag[led] 	= led_num & 0xFF;
	
	led_flag[led] 				= led_enable_flay[led];		//获取led当前状态
	led_tick[led]					= 0;
	
	if(timeout_en){
		led_pro_timeout = 0x01;
		lib_timer_start(&led_timer, 120);
	}
	
	DEMO_PRINTF("Led On %d Led Off %d Num %d LED %d, Timeout %d\n",led_on_time[led],led_off_time[led],led_twinkle_num[led], led, timeout_en);
}

void user_led_set(uint8_t led_num, uint8_t en)
{
	//使用LED工作位
	set_led_work_enable(led_num, ENABLE);
	//设置LED电平
	set_led_level(en);
}

/**
 * @brief LED闪烁功能处理函数
 * @param void
 */
void led_fun(void)
{
	if(led_pro_timeout){
		return;
	}
	uint8_t ret = 0;
	
	//先判断LED的状态是否需要改变
	for(uint8_t i=0; i<LED_NUM; i++){
		if((led_twinkle_num[i] > 0)||(led_constant_flash_flag[i]))
		{
			ret |= (0x01 << i);
			if(led_tick[i]++ > ((led_flag[i] == LIGHT_STATE_OFF)?led_off_time[i]:led_on_time[i]))
			{
				if(led_twinkle_num[i]){
					if(led_constant_flash_flag[i] == 0x00){
						led_twinkle_num[i]--;
					}

					if(led_twinkle_num[i]==0x00){
						ret &= (~(0x01 << i));
					}
					
					led_tick[i] = 0;
					led_flag[i] = (!led_flag[i]);
					//执行LED状态切换
					if(led_flag[i] == LIGHT_STATE_ON){
						DEMO_PRINTF("set_led_enable_flay[%d]\n",i);
						set_led_enable_flay(i); 		
					}else{
						DEMO_PRINTF("reset_led_enable_flay[%d]\n",i);
						reset_led_enable_flay(i);
					}
				}
			}
		}
	}
	
	led_mode_pro();
	
	if(ret==0x00){
		CLEAR_RTC_EVENT(KEY_LED_BLINK_EVENT);
	}
}
