#ifndef _CHARGE_H_
#define _CHARGE_H_

#include "cpu.h"
#include "lib_api.h"

#define CHARGE_ENABLE	1

#define CHARGE_WORK_IO	GPIO_PA4
#define CHARGE_MODE_IO	GPIO_PB5

typedef enum {
    CHARGE_STATE_UNPLUGGED = 0,  // 1. 未充电（未插电）
    CHARGE_STATE_PLUGGED_IN,     // 2. 充电插入（已插电但未开始充电）
    CHARGE_STATE_CHARGING,       // 3. 充电中
    CHARGE_STATE_FULL            // 4. 充满
}charge_state_t;

//充电闪灯频率
#define DEVICE_CHARGE_LED_TIME     		0x00190019
#define DEVICE_CHARGE_LED_BLINK   		0x0101

charge_state_t get_charge_state(void);
void user_charge_init(void);
uint8_t user_charge_pro(void);
void charge_work_pwm_pro(void);
#endif
