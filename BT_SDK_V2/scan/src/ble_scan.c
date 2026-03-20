#include "ble_scan.h"
#include "stdlib.h"
#include "uart.h"
#include "app.h"
#include "pwm.h"
#include "led.h"

event_t scan_event;
void user_scan_data_pro(void *cb_dat, event_t *e);

//扫描初始化和使能
void user_ble_scan_init(void)
{
	#if SCAN_ENABLE
	if(SCAN_WINDOW<=SCAN_INITERVAL){
		//扫描窗口和扫描间隔设置
		lib_stack_set_scan_param(SCAN_WINDOW, SCAN_INITERVAL);
		//扫描使能
		lib_stack_scan_enable(false);
		
		lib_event_init(&scan_event, user_scan_data_pro, EVENT_PRIORITY_MIDDLE_GROUP);
	}
	#endif
}

#if SCAN_ENABLE
uint8_t user_scan_data[32];
uint8_t scan_data_len = 0;
uint8_t scan_busy = 0;
uint8_t old_scan_data = 0x00;

void scan_data_process(uint8_t data)
{
	switch(data)
	{
		case 0x00:
		{
			reset_time_tick();
			for(uint8_t i=0; i<MOTOR_NUMBER; i++){
				//恢复模式，为下次开始做准备
				if(get_pwm_work_en_flay(i)){
					set_pwm_work_en_flay(i, DISABLE);
					pwm_stop_mode_pro(i);
				}
				//清除标志位和停止PWM
				set_pwm_loop_switch_mode(i, 0x00);
				user_pwm_stop(i);
				
				//LED1_2亮起，其他灯熄灭
				if(i==LED_1){
					user_led_set(i, ENABLE);
				}else{
					user_led_set(i, DISABLE);
				}
				
				if(get_app_mode_en_flay(i) == 0x01){
					uint8_t mode = get_pwm_mode_flay(i);
					pwm_fixed_mode_ack_data_pro(mode, DISABLE, i);
				}
			}
		}break;
			
		case 0x02:
		{
			reset_time_tick();
			set_pwm_loop_switch_mode(MOTOR1, 0x00);
			pwm_mode_flay_updata(MOTOR1);
			
			if(get_app_mode_en_flay(MOTOR1) == 0x01){
				uint8_t mode = get_pwm_mode_flay(MOTOR1);
				pwm_fixed_mode_ack_data_pro(mode, ENABLE, MOTOR1);
			}
			
			//led处理
			set_led_parma(DEVICE_LED_KEY_WORK_TIME, DEVICE_LED_KEY_WORK_BLINK, LED_1, LED_PRO_NOT_TIMEOUT);
		}break;
//			
//		case 0x03:
//		{
//			reset_time_tick();
//			//led处理
//			if(get_pwm_work_en_flay(MOTOR2)){		//灯是亮起状态，需要灭0.3秒
//				set_led_parma(DEVICE_LED_KEY_WORK_TIME, DEVICE_LED_KEY_WORK_BLINK, LED_2, LED_PRO_NOT_TIMEOUT);
//			}else{				//灯是熄灭状态，需要亮起
//				user_led_set(LED_2, ENABLE);
//			}
//			set_pwm_loop_switch_mode(MOTOR2, 0x00);
//			pwm_mode_flay_updata(MOTOR2);
//			set_pwm_work_en_flay(MOTOR2, ENABLE);
//			
//			if(get_app_mode_en_flay(MOTOR2) == 0x01){
//				uint8_t mode = get_pwm_mode_flay(MOTOR2);
//				pwm_fixed_mode_ack_data_pro(mode, ENABLE, MOTOR2);
//			}
//		}break;
//			
//		case 0x04:
//		{
//			reset_time_tick();
//			//led处理
//			if(get_pwm_work_en_flay(MOTOR3)){		//灯是亮起状态，需要灭0.3秒
//				set_led_parma(DEVICE_LED_KEY_WORK_TIME, DEVICE_LED_KEY_WORK_BLINK, LED_3, LED_PRO_NOT_TIMEOUT);
//			}else{				//灯是熄灭状态，需要亮起
//				user_led_set(LED_3, ENABLE);
//			}
//			set_pwm_loop_switch_mode(MOTOR3, 0x00);
//			pwm_mode_flay_updata(MOTOR3);
//			set_pwm_work_en_flay(MOTOR3, ENABLE);
//			
//			if(get_app_mode_en_flay(MOTOR3) == 0x01){
//				uint8_t mode = get_pwm_mode_flay(MOTOR3);
//				pwm_fixed_mode_ack_data_pro(mode, ENABLE, MOTOR3);
//			}
//		}break;
	}
}

void user_scan_data_pro(void *cb_dat, event_t *e)
{
	if(old_scan_data != user_scan_data[6]){
		old_scan_data = user_scan_data[6];
		DEMO_PRINTF("MAC: %02x\n", user_scan_data[7]);
		scan_data_process(user_scan_data[7]);
	}
	scan_busy = 0;
}

void user_scan_reset(void)
{
	lib_stack_scan_enable(false);
}

void lib_scan_cb(lib_scan_data_t *scan_data)
{
	if(scan_busy && (get_pwm_mode_flay(0) == KEY_PWM_MODE_ON_OFF)){
		return;
	}
	
	uint32_t user_adv_len = 0; 
	uint8_t *user_adv_data;
	uint8_t *user_adv_mac;
	uint32_t user_buf_len;
	uint32_t user_adv_type;
	uint32_t pos = 0;
	
	user_buf_len = scan_data->adv_data_len;
    user_adv_data = scan_data->adv_data;
	user_adv_mac = scan_data->addr;
	

    /* 处理扫描的数据 scan_data
     * 若需要执行复杂的操作，应该触发一个事件，在事件中执行
     */
    while (pos < user_buf_len) {
        user_adv_len = user_adv_data[pos++];

        /* AD遍历提前结束 */
        if (user_adv_len == 0 || pos + user_adv_len > user_buf_len) {
            return;
        }
        
        /* 获取AD Type */
        user_adv_type = user_adv_data[pos];
				
        if (user_adv_type == 0xFF){ // 类型
			if((user_adv_mac[5] == 0x58) && (user_adv_mac[4] == 0x2B)){
				scan_busy = 0x01;
				memset(user_scan_data, 0, sizeof(user_scan_data));
				memcpy(user_scan_data, &user_adv_data[pos+1], user_adv_len-1);	//获取扫描数据
				scan_data_len = user_adv_len-1;
				lib_event_post(&scan_event);
			}
				
            return;
        }
  
        /* 指向下一个ad */
        pos += user_adv_len;
    }
	
}
#endif
