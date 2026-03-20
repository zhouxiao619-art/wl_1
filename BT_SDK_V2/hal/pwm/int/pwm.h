#ifndef _PWM_H_
#define _PWM_H_

#include "cpu.h"
#include "charge.h"

//电机总数
#define MOTOR_NUMBER		1

//电机使能
#define M1_ENABLE			1
#define M2_ENABLE			0
#define M3_ENABLE			0

typedef enum {
	MOTOR1 = 0,
	MOTOR2,
	MOTOR3,
	CHARGE_PWM,
}MOTTOR_NUMBER;

#define PWM_MODE_TIMEOUT	TIM3

typedef struct _pwm_config
{
    uint16_t duty; 
	uint16_t period; 
	uint16_t reload;
	uint32_t timeout;
}pwm_config;

#if	CHARGE_ENABLE
uint16_t get_charge_pwm_loop_swtich_mode(void);
void set_charge_pwm_loop_swtich_mode(uint16_t num);
uint8_t get_charge_pwm_mode_switch_en(void);
void set_charge_pwm_mode_switch_en(uint8_t en);
void reset_charge_pwm_flay(void);
void charge_pwm_timeout_pro(void);
#endif

//电机初始化（PWM）
void user_pwm_timer_init(void);

void pwm_mode_flay_init(void);
uint8_t get_pwm_mode_flay(uint8_t i);
void set_pwm_mode_flay(uint8_t i, uint8_t num);

void pwm_mode_switch_en_init(void);
uint8_t get_pwm_mode_switch_en(uint8_t i);
void set_pwm_mode_switch_en(uint8_t i, uint8_t en);

void pwm_loop_switch_mode_init(void);
uint8_t get_pwm_loop_switch_mode(uint8_t i);
void set_pwm_loop_switch_mode(uint8_t i, uint8_t num);
void pwm_loop_switch_mode_updata(uint8_t i);

void pwm_work_en_flay_init(void);
uint8_t get_pwm_work_en_flay(uint8_t i);
void set_pwm_work_en_flay(uint8_t i, uint8_t num);

void pwm_stop_mode_pro(uint8_t i);

void pwm_timeout_pro(void);
void pwm_mode_updata_pro(void);
void pwm_config_data_pro(pwm_config *config);

void deivce_off_pwm_reset(void);

void set_old_pwm_mode_flay(uint8_t i, uint8_t mode);


void pwm_param_config(uint8_t pwm_num, pwm_config *config);
void pwm_reset(uint8_t pwm_num);

void user_pwm_param_config(uint8_t m_num, pwm_config *config);
void user_pwm_reset(uint8_t m_num);
void pwm_mode_flay_updata(uint8_t pwm_num);

#endif
