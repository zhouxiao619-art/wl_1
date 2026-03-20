#ifndef _BLE_GAP_H_
#define _BLE_GAP_H_

#include "lib_api.h"

typedef enum DEVICE_PAIR_MODE
{
    NO_PAIR = 0,
	PAIR_NEW,
	PAIR_OLD
}DEVICE_PAIR_MODE_T;

//uint8_t get_pair_mode(void);
//void set_pair_mode(uint8_t mode);

void ble_event_ind_process(void);
void serial_stack_evt_callback(uint32_t evt,uint32_t info);
void user_ble_gap_init(void);

#endif
