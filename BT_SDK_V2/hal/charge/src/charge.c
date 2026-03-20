#include "charge.h"
#include "gpio.h"
#include "led.h"
#include "app.h"
#include "uart.h"
#include "pwm.h"
#include "timer.h"

uint16_t charge_time = 0;
uint16_t old_charge_time = 0;
charge_state_t charge_state = CHARGE_STATE_UNPLUGGED;

pwm_config charge_pwm_config_fifo[1] = {
	{.duty=2,	.period=2,	.reload=800,	.timeout=0	},
};

charge_state_t get_charge_state(void)
{
	return charge_state;
}

uint8_t charge_led_mode = 0;// 0 表示渐亮，1 表示渐灭
void charge_work_pwm_pro(void)
{
	#if CHARGE_ENABLE
	if(get_charge_pwm_mode_switch_en()){
		set_charge_pwm_mode_switch_en(DISABLE);
		
		uint16_t num = get_charge_pwm_loop_swtich_mode();
		
		if(charge_led_mode == 0x00){ 		//渐亮模式
			if(charge_pwm_config_fifo[0].duty < 800){
				charge_pwm_config_fifo[0].duty = (((num * 133) / 250) + 2);
//				DEMO_PRINTF("%d, %d\n", num, charge_pwm_config_fifo[0].duty);
				pwm_param_config(CHARGE_PWM, &charge_pwm_config_fifo[0]);
				num += 1;
				set_charge_pwm_loop_swtich_mode(num);
			}else{
				charge_led_mode = 0x01;
			}
		}else{ 		//渐灭模式
			if(charge_pwm_config_fifo[0].duty > 2){
				charge_pwm_config_fifo[0].duty = (((num * 133) / 250) + 2);
//				DEMO_PRINTF("%d, %d\n", num, charge_pwm_config_fifo[0].duty);
				pwm_param_config(CHARGE_PWM, &charge_pwm_config_fifo[0]);
				num -= 1;
				set_charge_pwm_loop_swtich_mode(num);
			}else{
				charge_led_mode = 0x00;
			}
		}

	}
	#endif
}

void user_charge_init(void)
{
	#if CHARGE_ENABLE
		//充电管理初始化，设置为浮空输入
		gpio_remapmode_set(CHARGE_WORK_IO, 	GPIO_PINMUX_GPIO);
		gpio_pinmode_set(CHARGE_WORK_IO, 	GPIO_INOUT_MODE_FLOATING_IN);
		gpio_wk_src_type(CHARGE_WORK_IO, 	GPIO_WK_IG_HIGH);
	#endif
}

void user_charge_mode_set_gpio(void)
{
	//充电状态下设置模式，设置为浮空输入，关闭唤醒
	gpio_remapmode_set(CHARGE_MODE_IO, 	GPIO_PINMUX_GPIO);
	gpio_pinmode_set(CHARGE_MODE_IO, 	GPIO_INOUT_MODE_FLOATING_IN);
	gpio_wk_src_type(CHARGE_MODE_IO, 	GPIO_WK_NOWK);
}

void user_charge_mode_reset_gpio(void)
{
	//退出充电模式时需要恢复默认状态
	gpio_remapmode_set(CHARGE_MODE_IO, 	GPIO_PINMUX_GPIO);
	gpio_pinmode_set(CHARGE_MODE_IO,	GPIO_INOUT_MODE_PULLUP_IN);
	gpio_wk_src_type(CHARGE_MODE_IO, 	GPIO_WK_IG_LOW);
}

uint8_t user_charge_pro(void)
{
	#if CHARGE_ENABLE
		if(gpio_get_pin(CHARGE_WORK_IO)){
			if(charge_time<8000){
				charge_time++;
			}
			
			if((charge_time>1) && (charge_state == CHARGE_STATE_UNPLUGGED)){ 	//检测到开始充电，准备切换到硬件
			charge_state = CHARGE_STATE_PLUGGED_IN;
			user_charge_mode_set_gpio();
			SET_SYS_NO_SLEEP_EVT(DEVICE_CHARGE_EVENT);
		}
		
		if((charge_time>=20) && (charge_state >= CHARGE_STATE_PLUGGED_IN)){ 	//充电状态稳定
			if(get_adc_low_en() != 0x00){
				set_adc_low_en(0x00);
			}
			
			if(get_device_mode() == DEVICE_ON){
				set_device_mode(DEVICE_OFF);
			}
			if(gpio_get_pin(CHARGE_MODE_IO)){ 	//充满
				if(charge_state != CHARGE_STATE_FULL){
					charge_state = CHARGE_STATE_FULL;
					reset_charge_pwm_flay();
					reset_led_all_flay();
					led_1_2_charge_full();
					DEMO_PRINTF("CHARGE_STATE_FULL\n");
				}
			}else{ 		//充电中
				if((charge_state != CHARGE_STATE_CHARGING) && (charge_state != CHARGE_STATE_FULL)){ 	//charge_state != CHARGE_STATE_FULL 是为了防止重复
					charge_state = CHARGE_STATE_CHARGING;
					led_1_2_charge_charging(); 		//准备切换gpio模式
					charge_pwm_config_fifo[0].timeout = 2;
					pwm_param_config(CHARGE_PWM, &charge_pwm_config_fifo[0]); 	//设置充电指示灯
					DEMO_PRINTF("CHARGE_STATE_CHARGING\n");
				}
			}
		}
			
			return 0x01;
		}else{
			if(charge_state != CHARGE_STATE_UNPLUGGED){ 		//充电线已拔出
				//复位充电状态标志
				charge_state = CHARGE_STATE_UNPLUGGED;
				charge_time = 0;
				
				//恢复IO为默认状态
				user_charge_mode_reset_gpio();
				
				//复位充电pwm标志并停止充电指示PWM
				reset_charge_pwm_flay();
				charge_pwm_config_fifo[0].duty = 0;
				charge_pwm_config_fifo[0].timeout = 0;
				pwm_param_config(CHARGE_PWM, &charge_pwm_config_fifo[0]);
				
				//恢复全部的灯
				reset_led_all_flay();
				led_1_2_charge_unplugged();
				
				//清除充电事件的定时器事件
				CLEAR_RTC_EVENT(CHARGE_EVENT);
				
				//清除系统不睡眠标志
				CLEAR_SYS_NO_SLEEP_EVT(DEVICE_CHARGE_EVENT);
				DEMO_PRINTF("CHARGE_STATE_UNPLUGGED\n");
			}
			
			return 0x00;
		}
	#else
		return 0x00;
	#endif
}

