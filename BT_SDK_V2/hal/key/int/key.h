#ifndef _KEY_H_
#define _KEY_H_

#include "cpu.h"

typedef enum _key_status
{
    btn_state_idle,
	btn_state_work,       
}key_status;

typedef enum _task_key
{
	S1_KEY,
	S2_KEY,
	S3_KEY,
}task_key;

typedef enum _PRESS_STA
{
    PRESSED,					// 按下
	RELEASED,					// 已抬起
	SINGLE_CLICK,				// 单击
	DOUBLE_CLICK,				// 双击
	LONG_CONTINUE_3S_PRESSED, 	// 持续长按响应
	LONG_PRESSED,				// 单次长按响应
}PRESS_STA;

void user_key_init(void);
key_status key_task(uint8_t exe);
uint16_t key_work_get(void);

#endif
