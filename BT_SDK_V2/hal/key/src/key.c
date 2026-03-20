#include "key.h"
#include "gpio.h"
#include "uart.h"
#include "led.h"
#include "app.h"
#include "pwm.h"

#define SINGLE_SIZE 1
uint8_t test_table[SINGLE_SIZE]={GPIO_PB5};

/**
 * @brief 按键初始化
 * @param void
 */
void user_key_init(void)
{
	for(uint8_t i=0;i<SINGLE_SIZE;i++){
		gpio_remapmode_set(test_table[i], 	GPIO_PINMUX_GPIO);
		gpio_pinmode_set(test_table[i],		GPIO_INOUT_MODE_PULLUP_IN);
		gpio_wk_src_type(test_table[i], 	GPIO_WK_IG_LOW);
	}
}

typedef struct _key_code_message
{
    uint8_t gpio_sta;
	task_key key;
	uint16_t filter_cnt;
	uint8_t  click_cnt;
	PRESS_STA state;
}key_code_message;

#define KEY_CODE_SIZE 1
key_code_message key_code[KEY_CODE_SIZE]={
	{.key=S1_KEY,	.gpio_sta = 1,.filter_cnt = 100},	//长按开关机，单击切换模式，双击关闭
//	{.key=S2_KEY,	.gpio_sta = 1,.filter_cnt = 100},	//单击切换模式，双击关闭
//	{.key=S3_KEY,	.gpio_sta = 1,.filter_cnt = 100},	//单击切换模式，双击关闭
};

/**
 * @brief 按键扫描和处理
 * @param _key_code 按键信息
 */
void task_event_process(key_code_message *_key_code)
{
	if(get_device_mode() == DEVICE_OFF){
		if((_key_code->key != S1_KEY) | (_key_code->state != LONG_PRESSED)){
			return;
		}
	}
	
	switch(_key_code->key){
		case S1_KEY:
		{
			switch(_key_code->state){
				case SINGLE_CLICK:
				{
					DEMO_PRINTF("S1_KEY SINGLE_CLICK\n");
					//pwm模式处理
					set_pwm_loop_switch_mode(MOTOR1, 0x00);
					pwm_mode_flay_updata(MOTOR1);
					set_pwm_work_en_flay(MOTOR1, ENABLE);
					
					//led处理 LED1_2开机保持常亮的，所以直接闪烁
					set_led_parma(DEVICE_LED_KEY_WORK_TIME, DEVICE_LED_KEY_WORK_BLINK, LED_1, LED_PRO_NOT_TIMEOUT);
					
					//模式上报
					if(get_app_mode_en_flay(MOTOR1) == 0x01){
						uint8_t mode = get_pwm_mode_flay(MOTOR1);
						pwm_fixed_mode_ack_data_pro(mode, ENABLE, MOTOR1);
					}
				}break;
				
				case DOUBLE_CLICK:
				{
					DEMO_PRINTF("S1_KEY DOUBLE_CLICK\n");
					//模式上报
					if((get_app_mode_en_flay(MOTOR1) == 0x01) && (get_pwm_work_en_flay(MOTOR1))){
						uint8_t mode = get_pwm_mode_flay(MOTOR1);
						pwm_fixed_mode_ack_data_pro(mode, DISABLE, MOTOR1);
					}
					
					//恢复模式，为下次开始做准备
					if(get_pwm_work_en_flay(MOTOR1)){
						set_pwm_work_en_flay(MOTOR1, DISABLE);
						pwm_stop_mode_pro(MOTOR1);
					}
					
					//清除标志位和停止PWM
					set_pwm_loop_switch_mode(MOTOR1, 0x00);
					user_pwm_stop(MOTOR1);
					
					//led处理 LED1_2开机保持亮
					user_led_set(LED_1, ENABLE);
				}break;
					
				
				case LONG_PRESSED:
				{
					DEMO_PRINTF("S1_KEY LONG_PRESSED\n");
					//开关机
					if(get_device_mode() == DEVICE_ON){
						set_device_mode(DEVICE_OFF);
					}else{
						set_device_mode(DEVICE_ON);
					}
				}break;
			}
		}break;
//		
//		case S2_KEY:
//		{
//			switch(_key_code->state){
//				case SINGLE_CLICK:
//				{
//					DEMO_PRINTF("S2_KEY SINGLE_CLICK\n");
//					//led处理
//					if(get_pwm_work_en_flay(MOTOR2)){		//灯是亮起状态，需要灭0.3秒
//						set_led_parma(DEVICE_LED_KEY_WORK_TIME, DEVICE_LED_KEY_WORK_BLINK, LED_2, LED_PRO_NOT_TIMEOUT);
//					}else{				//灯是熄灭状态，需要亮起
//						user_led_set(LED_2, ENABLE);
//					}
//					
//					//pwm模式处理
//					set_pwm_loop_switch_mode(MOTOR2, 0x00);
//					pwm_mode_flay_updata(MOTOR2);
//					set_pwm_work_en_flay(MOTOR2, ENABLE);
//					
//					//模式上报
//					if(get_app_mode_en_flay(MOTOR2) == 0x01){
//						uint8_t mode = get_pwm_mode_flay(MOTOR2);
//						pwm_fixed_mode_ack_data_pro(mode, ENABLE, MOTOR2);
//					}
//				}break;
//				
//				case DOUBLE_CLICK:
//				{
//					DEMO_PRINTF("S2_KEY DOUBLE_CLICK\n");
//					//模式上报
//					if((get_app_mode_en_flay(MOTOR2) == 0x01) && (get_pwm_work_en_flay(MOTOR2))){
//						uint8_t mode = get_pwm_mode_flay(MOTOR2);
//						pwm_fixed_mode_ack_data_pro(mode, DISABLE, MOTOR2);
//					}
//					
//					//恢复模式，为下次开始做准备
//					if(get_pwm_work_en_flay(MOTOR2)){
//						set_pwm_work_en_flay(MOTOR2, DISABLE);
//						pwm_stop_mode_pro(MOTOR2);
//					}
//					
//					//清除标志位和停止PWM
//					set_pwm_loop_switch_mode(MOTOR2, 0x00);
//					user_pwm_stop(MOTOR2);
//					
//					//led处理
//					user_led_set(LED_2, DISABLE);
//				}break;
//			}
//		}break;
//		
//		case S3_KEY:
//		{
//			switch(_key_code->state){	
//				case SINGLE_CLICK:
//				{
//					DEMO_PRINTF("S3_KEY SINGLE_CLICK\n");
//					//led处理
//					if(get_pwm_work_en_flay(MOTOR3)){		//灯是亮起状态，需要灭0.3秒
//						set_led_parma(DEVICE_LED_KEY_WORK_TIME, DEVICE_LED_KEY_WORK_BLINK, LED_3, LED_PRO_NOT_TIMEOUT);
//					}else{				//灯是熄灭状态，需要亮起
//						user_led_set(LED_3, ENABLE);
//					}
//					
//					//pwm模式处理
//					set_pwm_loop_switch_mode(MOTOR3, 0x00);
//					pwm_mode_flay_updata(MOTOR3);
//					set_pwm_work_en_flay(MOTOR3, ENABLE);
//					
//					//状态上报
//					if(get_app_mode_en_flay(MOTOR3) == 0x01){
//						uint8_t mode = get_pwm_mode_flay(MOTOR3);
//						pwm_fixed_mode_ack_data_pro(mode, ENABLE, MOTOR3);
//					}
//				}break;
//				
//				case DOUBLE_CLICK:
//				{
//					DEMO_PRINTF("S3_KEY DOUBLE_CLICK\n");
//					//模式上报
//					if((get_app_mode_en_flay(MOTOR3) == 0x01) && (get_pwm_work_en_flay(MOTOR3))){
//						uint8_t mode = get_pwm_mode_flay(MOTOR3);
//						pwm_fixed_mode_ack_data_pro(mode, DISABLE, MOTOR3);
//					}
//					
//					//恢复模式，为下次开始做准备
//					if(get_pwm_work_en_flay(MOTOR3)){
//						set_pwm_work_en_flay(MOTOR3, DISABLE);
//						pwm_stop_mode_pro(MOTOR3);
//					}
//					
//					//清除标志位和停止PWM
//					set_pwm_loop_switch_mode(MOTOR3, 0x00);
//					user_pwm_stop(MOTOR3);
//					
//					//led处理
//					user_led_set(LED_3, DISABLE);
//				}break;
//			}
//		}break;
	}
}

#define KEY_PRESSED_TIME		1
#define KEY_LONG_PRESSED_TIME	100
#define KEY_MULTIHIT_MIX_TIME	2
#define KEY_MULTIHIT_MAX_TIME	30
#define KEY_PUT_UP_TIMEROUT		10
#define KEY_FILTER_CNT_TIMEOUT	8000

/**
 * @brief 按键扫描和处理
 * @param i 按键位置  key_status 按键状态
 */
void key_process(uint8_t i,uint8_t key_status)
{
	if(key_status != key_code[i].gpio_sta){
		if((key_code[i].filter_cnt >= KEY_MULTIHIT_MIX_TIME) && (key_code[i].filter_cnt < KEY_MULTIHIT_MAX_TIME)){
			key_code[i].click_cnt ++;
		}
		else if(key_code[i].filter_cnt >= KEY_MULTIHIT_MAX_TIME){
			key_code[i].click_cnt = 0;
		}
		
		key_code[i].filter_cnt = 0;
		key_code[i].gpio_sta = key_status;
	}
	
	if(key_code[i].filter_cnt < 8000){
		key_code[i].filter_cnt ++;							//误压超过2分钟
	}
	
	if(key_status == 0){
		if(key_code[i].filter_cnt == KEY_PRESSED_TIME){	
//			DEMO_PRINTF("----key[%d] %d press \n",i,key_code[i].key,key_code[i].click_cnt);
			key_code[i].state = PRESSED;
			task_event_process(&key_code[i]);
		}
		
//		if((key_code[i].filter_cnt >= 40) && ((key_code[i].filter_cnt % 25) == 0))
//		{
//			key_code[i].state 		= LONG_CONTINUE_3S_PRESSED;
//			task_event_process(&key_code[i]);
//		}
		
		if(key_code[i].filter_cnt == KEY_LONG_PRESSED_TIME){
			key_code[i].state = LONG_PRESSED;
			task_event_process(&key_code[i]);
		}
	}else {
		if(key_code[i].filter_cnt == KEY_PUT_UP_TIMEROUT){
				if(key_code[i].click_cnt < 1){

				}else if(key_code[i].click_cnt < 3){		// 单击弹起
					key_code[i].state = SINGLE_CLICK;
					task_event_process(&key_code[i]);
				}else if(key_code[i].click_cnt < 5){		// 双击弹起
					key_code[i].state = DOUBLE_CLICK;
					task_event_process(&key_code[i]);
				}		
				key_code[i].click_cnt = 0;
				CLEAR_RTC_EVENT(KEY_PRESS_EVENT);
		}
	}
}

/**
 * @brief 按键扫描和处理
 * @param exe 按键扫描控制
 * @return key_status 按键状态
 */
key_status key_task(uint8_t exe)
{
	key_status key_stat = btn_state_idle;
	uint8_t key_lever;
	for(uint8_t i=0;i<SINGLE_SIZE;i++){
		lib_stack_process();
		key_lever = gpio_get_pin(test_table[i]);
		if(exe){
			key_process(i,key_lever);
		}
		
		if(key_lever == 0){
			key_stat = btn_state_work;
		}
	}
	
	return key_stat;
}

uint16_t key_work_get(void)
{
	uint16_t ret = 0x00;
	for(uint8_t i=0; i<SINGLE_SIZE; i++){
		if(!gpio_get_pin(test_table[i])){
			ret |= (1<<i);
		}
	}
	return ret;
}
