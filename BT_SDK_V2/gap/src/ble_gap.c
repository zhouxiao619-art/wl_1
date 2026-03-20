#include "ble_gap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "uart.h"
#include "app.h"
#include "led.h"
#include "pwm.h"

uint8_t ble_mac[6];						//蓝牙mac地址
SMP_INFO_G g_sm_mem;	

//蓝牙广播数据包
uint8_t adv_data[] = {
	0x02,0x01,0x06,
	0x03,0x02,0xFD,0xFD,
	0x05,0x09,'w','l','_','1',
};

//蓝牙扫描回应包 - 使用用户提供的格式
//正确的制造商数据格式：长度(0x15) + 类型(0xFF) + 制造商ID(0x0901) + 数据
uint8_t scan_rsp_data[] = {
	0x15,0xFF,                 // 长度(21字节)和类型(制造商特定数据)
	0x09,0x01,                 // 制造商ID
	0x00,0x01,                 // 品牌码
	0x00,0x01,                 // 板厂码
	0x10,0x01,                 // 工厂码
	0x00,0x01,                 // 设备码
	0xD6,0x00,0x00,0x00,         // 销售码
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // MAC码
	0xAA                        // 结尾码 (0xAA)
};



void serial_stack_evt_callback(uint32_t evt,uint32_t info)
{
	switch(evt)
	{	
		//蓝牙连接事件
		case LIB_STACKEVT_CONNECT:
		{
			DEMO_PRINTF("LIB_STACKEVT_CONNECT: state: %02X\r\n",lib_stack_state_get());
			reset_time_tick();
			reset_led_all_flay();
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
			}
		}break;
		//蓝牙断开事件
		case LIB_STACKEVT_DISCONN:
		{
			DEMO_PRINTF("LIB_STACKEVT_DISCONN: info: %02X\r\n",info);
			reset_time_tick();
			CLEAR_SYS_NO_SLEEP_EVT(BLE_DISCONNED_EVENT);
			if(get_device_mode() != DEVICE_OFF) {
				lib_stack_adv_enable(true);
				reset_led_all_flay();
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
				}
			}
		}break;
	}
}

void device_mac_data_pro(void)
{
	//设置蓝牙mac地址
	uint32_t mac_data = 0;
	mac_data = lib_get_number();
	
	for(uint8_t i=0; i<4; i++){
		ble_mac[i] = ((mac_data >> (i*8)) & 0xFF);
		scan_rsp_data[i+14] = ((mac_data >> (i*8)) & 0xFF); // 更新MAC地址位置 (偏移量14)
	}
	ble_mac[4] = 0x2B;
	ble_mac[5] = 0x58;
	scan_rsp_data[18] = 0x2B; // 更新MAC地址位置 (偏移量18)
	scan_rsp_data[19] = 0x58; // 更新MAC地址位置 (偏移量19)
	
	lib_stack_set_bdaddr((uint8_t *)ble_mac);
}
void user_ble_gap_init(void)
{
	//设置MAC地址
	device_mac_data_pro();
	
	//设置广播包数据
	lib_stack_set_adv_data((uint8_t *)&adv_data[0], sizeof(adv_data));
	
	//设置扫描响应包数据
	lib_stack_set_scan_rsp_data((uint8_t *)&scan_rsp_data[0], sizeof(scan_rsp_data));
	
	//设置蓝牙广播类型和间隔
	lib_stack_set_adv_param(ADV_TYPE_ADV_IND, 100);
	
	//设置蓝牙回调事件注册处理
	lib_register_stack_event_cb(serial_stack_evt_callback,LIB_STACKEVT_CONNECT | LIB_STACKEVT_DISCONN);
	
	//蓝牙配对信息数量设置
	sm_mem_init(&g_sm_mem,SMP_MAX_NUM);

	//使能蓝牙广播
	lib_stack_adv_enable(true);
}

