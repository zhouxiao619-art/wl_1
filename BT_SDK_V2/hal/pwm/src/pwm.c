#include "pwm.h"
#include "gpio.h"
#include "timer.h"
#include "app.h"
#include "uart.h"


uint8_t pwm_mode_flay[MOTOR_NUMBER];		//PWM?????????
uint8_t pwm_mode_switch_en[MOTOR_NUMBER];	//PWM???§Ý????????
uint8_t pwm_loop_switch_mode[MOTOR_NUMBER];	//PWM?????????§Ý?????
uint8_t pwm_work_en_flay[MOTOR_NUMBER];		//PWM??????????????

#if	M1_ENABLE
#define M1_PWM_GPIO		GPIO_PB3
#define M1_TIME_NUM		TIM0		
uint8_t old_m1_pwm_mode_flay = 0;		//PWM???????????
uint16_t m1_pwm_switch_timeout = 0;		//PWM?§Ý???????
uint32_t m1_old_time_flay = 0;			//PWM?§Ý??????????????????????
#endif

#if	M2_ENABLE
#define M2_PWM_GPIO		GPIO_PB4
#define M2_TIME_NUM		TIM1
uint8_t old_m2_pwm_mode_flay = 0;		//PWM???????????
uint16_t m2_pwm_switch_timeout = 0;		//PWM?§Ý???????
uint32_t m2_old_time_flay = 0;			//PWM?§Ý??????????????????????
#endif

#if	M3_ENABLE
#define M3_PWM_GPIO		GPIO_PA4
#define M3_TIME_NUM		TIM2
uint8_t old_m3_pwm_mode_flay = 0;		//PWM???????????
uint16_t m3_pwm_switch_timeout = 0;		//PWM?§Ý???????
uint32_t m3_old_time_flay = 0;			//PWM?§Ý??????????????????????
#endif

#if	CHARGE_ENABLE
#define CHARGE_PWM_GPIO		GPIO_PA3
#define CHARGE_TIME_NUM		TIM4
uint16_t charge_pwm_loop_swtich_mode = 0;		//PWM????§Ý??????
uint8_t charge_pwm_mode_switch_en	= 0;		//PWM???§Ý????
uint16_t charge_pwm_switch_timeout 	= 0;		//PWM?§Ý???????
uint32_t charge_old_time_flay 		= 0;		//PWM?§Ý??????????????????????

uint16_t get_charge_pwm_loop_swtich_mode(void)
{
	return charge_pwm_loop_swtich_mode;
}

void set_charge_pwm_loop_swtich_mode(uint16_t num)
{
	charge_pwm_loop_swtich_mode = num;
}

uint8_t get_charge_pwm_mode_switch_en(void)
{
	return charge_pwm_mode_switch_en;
}

void set_charge_pwm_mode_switch_en(uint8_t en)
{
	charge_pwm_mode_switch_en = en;
}

void reset_charge_pwm_flay(void)
{
	charge_pwm_loop_swtich_mode = 0;
	charge_pwm_mode_switch_en	= 0;
	charge_pwm_switch_timeout 	= 0;
	charge_old_time_flay 		= 0;
}
#endif



void pwm_mode_flay_init(void)
{
	for(uint8_t i=0; i<MOTOR_NUMBER; i++){
		pwm_mode_flay[i] = 0x00;
	}
}

uint8_t get_pwm_mode_flay(uint8_t i)
{
	if(i<=MOTOR_NUMBER){
		i = MOTOR_NUMBER-1;
	}
	return pwm_mode_flay[i];
}

void set_pwm_mode_flay(uint8_t i, uint8_t num)
{
	if(i<=MOTOR_NUMBER){
		i = MOTOR_NUMBER-1;
	}
	pwm_mode_flay[i] = num;
}

void pwm_mode_switch_en_init(void)
{
	for(uint8_t i=0; i<MOTOR_NUMBER; i++){
		pwm_mode_switch_en[i] = 0x00;
	}
}

uint8_t get_pwm_mode_switch_en(uint8_t i)
{
	if(i<=MOTOR_NUMBER){
		i = MOTOR_NUMBER-1;
	}
	return pwm_mode_switch_en[i];
}

void set_pwm_mode_switch_en(uint8_t i, uint8_t en)
{
	if(i<=MOTOR_NUMBER){
		i = MOTOR_NUMBER-1;
	}
	pwm_mode_switch_en[i] = en;
}

void pwm_loop_switch_mode_init(void)
{
	for(uint8_t i=0; i<MOTOR_NUMBER; i++){
		pwm_loop_switch_mode[i] = 0x00;
	}
}

uint8_t get_pwm_loop_switch_mode(uint8_t i)
{
	if(i<=MOTOR_NUMBER){
		i = MOTOR_NUMBER-1;
	}
	return pwm_loop_switch_mode[i];
}

void set_pwm_loop_switch_mode(uint8_t i, uint8_t num)
{
	if(i<=MOTOR_NUMBER){
		i = MOTOR_NUMBER-1;
	}
	pwm_loop_switch_mode[i] = num;
}

void pwm_loop_switch_mode_updata(uint8_t i)
{
	uint8_t ret = get_pwm_loop_switch_mode(i);
	ret++;
	set_pwm_loop_switch_mode(i, ret);
}

void pwm_work_en_flay_init(void)
{
	for(uint8_t i=0; i<MOTOR_NUMBER; i++){
		pwm_work_en_flay[i] = 0x00;
	}
}

uint8_t get_pwm_work_en_flay(uint8_t i)
{
	if(i<=MOTOR_NUMBER){
		i = MOTOR_NUMBER-1;
	}
	return pwm_work_en_flay[i];
}

void set_pwm_work_en_flay(uint8_t i, uint8_t num)
{
	if(i<=MOTOR_NUMBER){
		i = MOTOR_NUMBER-1;
	}
	pwm_work_en_flay[i] = num;
}

void pwm_stop_mode_pro(uint8_t num)
{
	uint8_t now_mode = get_pwm_mode_flay(num);
	if(now_mode){
		now_mode -= 1;
	}
	
	switch(num)
	{
		#if	M1_ENABLE
			case MOTOR1:{old_m1_pwm_mode_flay = now_mode; }break;
		#endif
		
		#if	M2_ENABLE
			case MOTOR2:{old_m2_pwm_mode_flay = now_mode; }break;
		#endif
		
		#if	M3_ENABLE
			case MOTOR3:{old_m3_pwm_mode_flay = now_mode; }break;
		#endif
	}
	set_pwm_mode_flay(num, now_mode);
}

void deivce_off_pwm_reset(void)
{
	pwm_mode_flay_init();			//?????
	pwm_mode_switch_en_init();		//??????
	pwm_loop_switch_mode_init();	//?§Ý????¦Ë???
	pwm_work_en_flay_init();		//???????????
	
	#if	M1_ENABLE
		old_m1_pwm_mode_flay = 0;		//PWM???????????
		m1_pwm_switch_timeout = 0;		//PWM?§Ý???????
		m1_old_time_flay = 0;			//PWM?§Ý??????????????????????
	#endif
	
	#if	M2_ENABLE
		old_m2_pwm_mode_flay = 0;		//PWM???????????
		m2_pwm_switch_timeout = 0;		//PWM?§Ý???????
		m2_old_time_flay = 0;			//PWM?§Ý??????????????????????
	#endif
	
	#if	M3_ENABLE
		old_m3_pwm_mode_flay = 0;		//PWM???????????
		m3_pwm_switch_timeout = 0;		//PWM?§Ý???????
		m3_old_time_flay = 0;			//PWM?§Ý??????????????????????
	#endif
}

void set_old_pwm_mode_flay(uint8_t i, uint8_t mode)
{
	switch(i)
	{
		#if	M1_ENABLE
			case MOTOR1: {old_m1_pwm_mode_flay = mode; }break;
		#endif
		
		#if	M2_ENABLE
			case MOTOR2: {old_m2_pwm_mode_flay = mode; }break;
		#endif
		
		#if	M3_ENABLE
			case MOTOR3: {old_m3_pwm_mode_flay = mode; }break;
		#endif
	}
}

/**
 * @brief timer?????????
 * @param void 
 */
void user_timer_nvic_init()
{
	//??????????????0?????????0
	NVIC_SetPriority(Timer_IRQn, 3);
	NVIC_EnableIRQ(Timer_IRQn);
}

/**
 * @brief PWM?????????
 * @param void 
 */
void user_pwm_timer_init(void)
{
	cpu_crm_set(CRM_F_TIMER0_EN, ENABLE);
	cpu_crm_reset(CRM_F_TIMER0_SRST, ENABLE);
	
	#if	M1_ENABLE
		gpio_remapmode_set(M1_PWM_GPIO, GPIO_PINMUX_PWM_CH0);	
		timer_togg_pol(M1_TIME_NUM, low);
		timer_en(M1_TIME_NUM, DISABLE);
	#endif
	
	#if	M2_ENABLE
		gpio_remapmode_set(M2_PWM_GPIO, GPIO_PINMUX_PWM_CH1);	
		timer_togg_pol(M2_TIME_NUM, low);
		timer_en(M2_TIME_NUM, DISABLE);
	#endif
	
	#if	M3_ENABLE
		gpio_remapmode_set(M3_PWM_GPIO, GPIO_PINMUX_PWM_CH2);	
		timer_togg_pol(M3_TIME_NUM, low);
		timer_en(M3_TIME_NUM, DISABLE);
	#endif
	
	timer_pcl_reload(PWM_MODE_TIMEOUT,1599,4);
	timer_update_en(PWM_MODE_TIMEOUT, ENABLE);
	timer_it_cfg(PWM_MODE_TIMEOUT, 0);
	timer_it_en(PWM_MODE_TIMEOUT, ENABLE);
	timer_en(PWM_MODE_TIMEOUT, DISABLE);
	
	user_timer_nvic_init();
}

//PWM????§Ý??????
void pwm_timeout_pro(void)
{
	#if M1_ENABLE
		if(m1_pwm_switch_timeout){
			if((get_timer_flay() - m1_old_time_flay) >= m1_pwm_switch_timeout){
				pwm_mode_switch_en[MOTOR1] = 0x01;
			}
		}
	#endif
		
	#if M2_ENABLE
		if(m2_pwm_switch_timeout){
			if((get_timer_flay() - m2_old_time_flay) >= m2_pwm_switch_timeout){
				pwm_mode_switch_en[MOTOR2] = 0x01;
			}
		}
	#endif
		
	#if M3_ENABLE
		if(m3_pwm_switch_timeout){
			if((get_timer_flay() - m3_old_time_flay) >= m3_pwm_switch_timeout){
				pwm_mode_switch_en[MOTOR3] = 0x01;
			}
		}
	#endif
}

#if CHARGE_ENABLE
void charge_pwm_timeout_pro(void)
{
	if(charge_pwm_switch_timeout){
		if((get_timer_flay() - charge_old_time_flay) >= charge_pwm_switch_timeout){
			charge_pwm_mode_switch_en = 0x01;
		}
	}
}
#endif
//PWM??????§Ý??????
void pwm_mode_updata_pro(void)
{
	#if M1_ENABLE
		if(old_m1_pwm_mode_flay != pwm_mode_flay[MOTOR1]){
			old_m1_pwm_mode_flay = pwm_mode_flay[MOTOR1];
			pwm_mode_switch_en[MOTOR1] = 0x01;
		}
	#endif
		
	#if M2_ENABLE
		if(old_m2_pwm_mode_flay != pwm_mode_flay[MOTOR2]){
			old_m2_pwm_mode_flay = pwm_mode_flay[MOTOR2];
			pwm_mode_switch_en[MOTOR2] = 0x01;
		}
	#endif
		
	#if M3_ENABLE
		if(old_m3_pwm_mode_flay != pwm_mode_flay[MOTOR3]){
			old_m3_pwm_mode_flay = pwm_mode_flay[MOTOR3];
			pwm_mode_switch_en[MOTOR3] = 0x01;
		}
	#endif
}

//PWM????????????§Ø????????????????
void pwm_config_data_pro(pwm_config *config)
{
	if(config->duty <= 1){
		config->duty = 2;
	}
	if(config->period <= 1){
		config->period = 2;
	}
	if(config->reload <= 1){
		config->reload = 2;
	}
}

void pwm_config_timeout_pro(uint32_t time, uint8_t pwm_num)
{
	user_timer3_stop();
	if(time == 0x00){				//?????§Ý?
		switch(pwm_num)
		{
			#if	M1_ENABLE
				case MOTOR1:{m1_pwm_switch_timeout = 0x00; m1_old_time_flay = 0x00;}break;
			#endif
			
			#if	M2_ENABLE
				case MOTOR2:{m2_pwm_switch_timeout = 0x00; m2_old_time_flay = 0x00;}break;
			#endif
			
			#if	M3_ENABLE
				case MOTOR3:{m3_pwm_switch_timeout = 0x00; m3_old_time_flay = 0x00;}break;
			#endif
				
			#if CHARGE_ENABLE
				case CHARGE_PWM:{charge_pwm_switch_timeout = 0x00; charge_old_time_flay = 0x00;}break;
			#endif
		}
	}else{
		switch(pwm_num)
		{
			#if	M1_ENABLE
				case MOTOR1:{m1_pwm_switch_timeout = time; m1_old_time_flay = get_timer_flay();}break;
			#endif
			
			#if	M2_ENABLE
				case MOTOR2:{m2_pwm_switch_timeout = time; m2_old_time_flay = get_timer_flay();}break;
			#endif
			
			#if	M3_ENABLE
				case MOTOR3:{m3_pwm_switch_timeout = time; m3_old_time_flay = get_timer_flay();}break;
			#endif
				
			#if CHARGE_ENABLE
				case CHARGE_PWM:{charge_pwm_switch_timeout = time; charge_old_time_flay = get_timer_flay();}break;
			#endif
		}
		user_timer3_start();
	}
}

//PWM????????
void pwm_param_config(uint8_t pwm_num, pwm_config *config)
{	
	uint8_t pwm;
	uint8_t gpio_num;
	uint16_t sleep_num;
	switch(pwm_num)
	{
		#if	M1_ENABLE
			case MOTOR1:
			{
				pwm = M1_TIME_NUM;
				gpio_num = M1_PWM_GPIO;
				sleep_num = (0x01 << MOTOR1);
				gpio_remapmode_set(gpio_num, GPIO_PINMUX_PWM_CH0);
			}break;
		#endif
		
		#if	M2_ENABLE
			case MOTOR2:
			{
				pwm = M2_TIME_NUM;
				gpio_num = M2_PWM_GPIO;
				sleep_num = (0x01 << MOTOR2);
				gpio_remapmode_set(gpio_num, GPIO_PINMUX_PWM_CH1);
			}break;
		#endif
		
		#if	M3_ENABLE
			case MOTOR3:
			{
				pwm = M3_TIME_NUM;
				gpio_num = M3_PWM_GPIO;
				sleep_num = (0x01 << MOTOR3);
				gpio_remapmode_set(gpio_num, GPIO_PINMUX_PWM_CH2);
			}break;
		#endif
			
		#if CHARGE_ENABLE
			case CHARGE_PWM:
			{
				pwm = CHARGE_TIME_NUM;
				gpio_num = CHARGE_PWM_GPIO;
				sleep_num = DEVICE_CHARGE_EVENT;
				gpio_remapmode_set(gpio_num, GPIO_PINMUX_PWM_CH4);	
			}
		#endif
	}
	
	if(config->duty == 0x00){
		SET_SYS_NO_SLEEP_EVT(sleep_num);
		pwm_reset(pwm_num);
	}else{
		if(config->duty == config->reload){
			SET_SYS_NO_SLEEP_EVT(sleep_num);
			timer_en(pwm, DISABLE);
			gpio_remapmode_set(gpio_num, GPIO_PINMUX_GPIO);
			gpio_pinmode_set(gpio_num, GPIO_INOUT_MODE_OUTPUT);
			gpio_set_pin(gpio_num);
		}else{
			pwm_config_data_pro(config);
			SET_SYS_NO_SLEEP_EVT(sleep_num);
			
			timer_en(pwm, DISABLE);
			timer_pcl_reload(pwm,(config->period-1),(config->reload-1));
			timer_toggle_val(pwm,(config->duty-1));
			timer_update_en(pwm, ENABLE);
			timer_en(pwm, ENABLE);
		}
	}
	
	pwm_config_timeout_pro(config->timeout, pwm_num);
}

//PWM??¦Ë????????
void pwm_reset(uint8_t pwm_num)
{
	uint8_t pwm;
	uint8_t gpio_num;
	uint16_t sleep_num;
	switch(pwm_num)
	{
		#if	M1_ENABLE
			case MOTOR1:
			{
				pwm = M1_TIME_NUM;
				gpio_num = M1_PWM_GPIO;
				sleep_num = (0x01 << MOTOR1);
			}break;
		#endif
		
		#if	M2_ENABLE
			case MOTOR2:
			{
				pwm = M2_TIME_NUM;
				gpio_num = M2_PWM_GPIO;
				sleep_num = (0x01 << MOTOR2);
			}break;
		#endif
		
		#if	M3_ENABLE
			case MOTOR3:
			{
				pwm = M3_TIME_NUM;
				gpio_num = M3_PWM_GPIO;
				sleep_num = (0x01 << MOTOR3);
			}break;
		#endif
			
		#if	CHARGE_ENABLE
			case CHARGE_PWM:
			{
				pwm = CHARGE_TIME_NUM;
				gpio_num = CHARGE_PWM_GPIO;
				sleep_num = DEVICE_CHARGE_EVENT;
			}break;
		#endif
	}
	
	if(pwm_num == CHARGE_PWM){
		timer_en(pwm, DISABLE);
		gpio_remapmode_set(gpio_num, GPIO_PINMUX_GPIO);
		gpio_pinmode_set(gpio_num, GPIO_INOUT_MODE_OUTPUT);
		gpio_reset_pin(gpio_num);
	}else{
		timer_en(pwm, DISABLE);
		gpio_remapmode_set(gpio_num, GPIO_PINMUX_GPIO);
		gpio_pinmode_set(gpio_num, GPIO_INOUT_MODE_DROPDOWN_IN);
	}
	CLEAR_SYS_NO_SLEEP_EVT(sleep_num);
}

//PWM?????????
void pwm_mode_flay_updata(uint8_t pwm_num)
{
	if(pwm_num >= MOTOR_NUMBER){
		pwm_num = (MOTOR_NUMBER-1);
	}
	
	if(pwm_mode_flay[pwm_num] <= 8){
		pwm_mode_flay[pwm_num]++;
	}else {
		pwm_mode_flay[pwm_num] = 0x01;
	}
	
	DEMO_PRINTF("pwm_mode_flay[%d]:%d\n",pwm_num, pwm_mode_flay[pwm_num]);
}

void user_pwm_param_config(uint8_t m_num, pwm_config *config)
{
	if(m_num >= MOTOR_NUMBER){
		m_num = (MOTOR_NUMBER-1);
	}
	
	pwm_mode_switch_en[m_num] = 0x00;
	pwm_param_config(m_num, config);
}

void user_pwm_reset(uint8_t m_num)
{
	if(m_num >= MOTOR_NUMBER){
		m_num = (MOTOR_NUMBER-1);
	}
	
	pwm_reset(m_num);
}


