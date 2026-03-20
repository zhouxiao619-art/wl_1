#ifndef _BLE_GATT_H_
#define _BLE_GATT_H_


#include "lib_api.h"

#define GATT_HANDLE_NUM_MAX    		(74)		//gatt服务条目，使用多少开多少，节省内存

#define ITERM_SERVIE_REGISTER		0			// 使用RAM 数组

#define SERVICE_16BIT_CHOOSE		1			// 1 开启16bit示例服务    0 关闭
#define SERVICE_16BIT_RX_HANDLE		0x09		// 没有使用16bit服务,所以设置为0
#define SERVICE_16BIT_TX_HANDLE		0x06		// 没有使用16bit服务,所以设置为0

#define SERVICE_128BIT_CHOOSE		0			// 1 开启128bit示例服务   0 关闭
#define SERVICE_128BIT_RX_HANDLE	0x00		// 使用128bit服务,所以设置为实际参数
#define SERVICE_128BIT_TX_HANDLE	0x00		// 使用128bit服务,所以设置为实际参数

typedef enum _gatts_profile
{
    Services_16,
	Services128,
    Character_16,
	Character128,
	Descriptor_t,
}gatts_profile;


typedef struct _gatt_profile
{
	gatts_profile 	type;
    uint32_t 		uuid;
	uint8_t 		property;
	uint8_t 		permit;
	char 			*value;
	uint16_t 		len;
}gatt_profile;

#if SERVICE_16BIT_CHOOSE
#define IOT_SERVICE_16BIT							0xFDE0
#define UUID_Characteristic_IOT_RX_16BIT			0xFDE1
#define UUID_Characteristic_IOT_TX_16BIT			0xFDE2
#endif

//#if SERVICE_128BIT_CHOOSE
//#define IOT_SERVICE_128BIT						"\x9e\xca\xdc\x24\x0e\xe5\xa9\xe0\x93\xf3\xa3\xb5\x01\x00\x40\x6e"
//#define UUID_Characteristic_IOT_RX_128BIT			"\x9e\xca\xdc\x24\x0e\xe5\xa9\xe0\x93\xf3\xa3\xb5\x02\x00\x40\x6e"
//#define UUID_Characteristic_IOT_TX_128BIT			"\x9e\xca\xdc\x24\x0e\xe5\xa9\xe0\x93\xf3\xa3\xb5\x03\x00\x40\x6e"
//#endif

void user_ble_gatt_init(void);
void user_ble_send_data_notify(uint8_t *data, uint32_t length);

#endif
