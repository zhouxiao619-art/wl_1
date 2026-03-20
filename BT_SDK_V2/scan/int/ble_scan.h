#ifndef _BLE_SCAN_H_
#define _BLE_SCAN_H_

#include "cpu.h"
#include "lib_api.h"

#define SCAN_ENABLE			1

//SCAN_WINDOW参数值不可大于SCAN_INITERVAL参数值
#define SCAN_WINDOW			40
#define	SCAN_INITERVAL		50

void user_ble_scan_init(void);
//void user_scan_reset(void);
//void user_scan_timeout_strat(uint32_t num);
//void user_scan_timeout_stop(void);

#endif
