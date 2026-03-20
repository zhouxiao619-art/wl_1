#include "ble_gatt.h"
#include "uart.h"
#include "string.h"
#include "ble_gap.h"
#include "app.h"

uint8_t  device_name[18] = "wl_1";
uint8_t  device_name_len = 4;

uint8_t ble_recv_data[9];			//?????????????????
uint16_t ble_recv_length = 0x00;	//??????????????? 

#if ITERM_SERVIE_REGISTER
gatt_profile generic_acc_profile[] = 
{	
	{Character_16, UUID_Characteristic_Device_Name, GATT_Character_R_RDynamic, GATT_Permit_Read|GATT_Character_Property_Read_Dynamic, (char *)device_name, 9},
};
#endif

#if SERVICE_16BIT_CHOOSE
uint16_t service_16bit_rx_handle = 0;
uint16_t service_16bit_tx_handle = 0;
#if ITERM_SERVIE_REGISTER
gatt_profile iot_profile_16bit[] =
{
	{Character_16, (uint32_t)UUID_Characteristic_IOT_TX_16BIT,	GATT_Character_Notify_R, GATT_Permit_RW, "x00/x00", 2},
	{Character_16, (uint32_t)UUID_Characteristic_IOT_RX_16BIT,	GATT_Character_Property_Write_Without_Rsp, GATT_Permit_RW, "x00/x00", 2},
};
#endif
#endif

#if SERVICE_128BIT_CHOOSE
uint16_t service_128bit_rx_handle = 0;
uint16_t service_128bit_tx_handle = 0;

#define EMPTY_DATA		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#if ITERM_SERVIE_REGISTER
const gatt_profile iot_profile_128bit[] =
{
	{Character128, (uint32_t)UUID_Characteristic_IOT_TX_128BIT,	GATT_Character_Notify_W, GATT_Permit_RW, EMPTY_DATA, 20},
	{Character128, (uint32_t)UUID_Characteristic_IOT_RX_128BIT,	GATT_Character_RW_WNoRsp, GATT_Permit_RW, EMPTY_DATA, 20},
};
#endif
#endif

#if ITERM_SERVIE_REGISTER
//???GATT?????????
void register_gatt_profiles(gatts_profile type,uint32_t pri_uuid, const gatt_profile *gatts,uint8_t gap_num)
{
	uint16_t gatt_hdl_start = 0;
	uint16_t gatt_hdl_end 	= 0;
	uint16_t handle = 0;
	uint16_t i;
	
	if(type == Services_16){
		gatt_hdl_start = gatt_register_service_primary(pri_uuid);	
	}else if(type == Services128){
		gatt_hdl_start = gatt_register_service_primary_uuid128(16,(uint8_t *)pri_uuid);	
	}
	
	for(i=0;i<gap_num;i++)
	{
		if(gatts[i].type == Descriptor_t){
			gatt_hdl_end = gatt_register_descriptor(gatts[i].uuid, gatts[i].len, (uint8_t *)gatts[i].value);
		}else{
			if(gatts[i].type == Character_16){
				handle = gatt_register_character(gatts[i].property,gatts[i].uuid);
				gatt_hdl_end = gatt_register_character_value(gatts[i].uuid, gatts[i].len, (uint8_t *)gatts[i].value);
				#if SERVICE_16BIT_CHOOSE
					if(gatts[i].uuid == UUID_Characteristic_IOT_TX_16BIT){
						service_16bit_tx_handle = gatt_hdl_end;
					}else if(gatts[i].uuid == UUID_Characteristic_IOT_RX_16BIT){
						service_16bit_rx_handle = gatt_hdl_end;
					}
				#endif				
			}else{
				handle = gatt_register_character_uuid128(gatts[i].property,16,(uint8_t *)gatts[i].uuid);
				gatt_hdl_end = gatt_register_character_value_uuid128(16,(uint8_t *)gatts[i].uuid, gatts[i].len, (uint8_t *)gatts[i].value);
				#if SERVICE_128BIT_CHOOSE
					if(strncmp((char *)gatts[i].uuid, UUID_Characteristic_IOT_TX_128BIT, 16) == 0x00){
						service_128bit_tx_handle = gatt_hdl_end;
					}else if(strncmp((char *)gatts[i].uuid, UUID_Characteristic_IOT_RX_128BIT, 16) == 0x00){
						service_128bit_rx_handle = gatt_hdl_end;
					}
				#endif	
			}

			if(gatts[i].property & (GATT_Character_Property_Notify|GATT_Character_Property_Indicate)){
				gatt_hdl_end = gatt_register_descriptor(UUID_Descriptor_GATT_Client_Charac_Config, 2, (uint8_t *)"\x01\x00");
			}
		}
		
		if(gatts[i].permit)
		{
			gatt_register_permit(gatts[i].permit);
		}
	}
	gatt_register_service_end(gatt_hdl_start,gatt_hdl_end);
}
#endif

#if ITERM_SERVIE_REGISTER
uint32_t lib_gatt_database[GATT_HANDLE_NUM_MAX + 3];
#else
//GATT?????????
const uint32_t lib_gatt_database[GATT_HANDLE_NUM_MAX+3]={
	0x20000390,0x00000009,0x20000458,0x2000045C,0x20000464,0x20000470,0x20000480,0x20000488,0x20000494,0x2000049C,0x200004A4,0x200004B0,0x00000000,0x00000000,0x00000000,0x00000000,
	0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
	0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
	0x00000000,0x00000000,0x00000000,0x00020102,0x18002800,0x00050102,0x03822803,0x002A0000,0x00098102,0x4A532A00,0x31302D48,0x00000000,0x00020102,0xFDE02800,0x00050102,0x06122803,
	0x00FDE200,0x00020102,0x3078FDE2,0x00020502,0x00012902,0x00050102,0x09042803,0x00FDE100,0x00020502,0x3078FDE1,
};
#endif
void user_ble_gatt_init(void)
{
#if ITERM_SERVIE_REGISTER
	for(int i=0;i<GATT_HANDLE_NUM_MAX;i++){
		lib_gatt_database[i] = 0;
	}
	lib_init(lib_gatt_database);		//?????GATT?????????›¥????????????????
	
	DEMO_PRINTF("const uint8_t lib_gatt_database[]={\n");
	
	register_gatt_profiles(Services_16,UUID_Service_GAP,generic_acc_profile,1);
	
	#if SERVICE_16BIT_CHOOSE
		register_gatt_profiles(Services_16,(int)IOT_SERVICE_16BIT,iot_profile_16bit,2);
	#endif
	
	#if SERVICE_128BIT_CHOOSE
		register_gatt_profiles(Services128,(int)IOT_SERVICE_128BIT,iot_profile_128bit,2);
	#endif
	
	uint32_t *ptr = (uint32_t *)lib_gatt_database;
	for(int i=0;i<(gatt_get_db_size()/4);i++){
		DEMO_PRINTF("0x%08X,",ptr[i]);
		if((i%16) == 15)
		{
			DEMO_PRINTF("\r\n");
		}
	}
	DEMO_PRINTF("};\r\n");
	
	#if SERVICE_16BIT_CHOOSE
		DEMO_PRINTF("service_16bit_rx_handle=%02X\n", service_16bit_rx_handle);
		DEMO_PRINTF("service_16bit_tx_handle=%02X\n", service_16bit_tx_handle);
	#endif
	
	#if SERVICE_128BIT_CHOOSE
		DEMO_PRINTF("service_128bit_rx_handle=%02X\n", service_128bit_rx_handle);
		DEMO_PRINTF("service_128bit_tx_handle=%02X\n", service_128bit_tx_handle);
	#endif
	
	DEMO_PRINTF("lib_gatt_database[%d] size %d\n",(gatt_get_db_size()+3)/4,gatt_get_db_size());
#else
	lib_init((uint32_t *)lib_gatt_database);
	lib_gatt_db_set((void *)lib_gatt_database);
	DEMO_PRINTF("lib_gatt_database\n");
	
	#if SERVICE_16BIT_CHOOSE
		service_16bit_rx_handle = SERVICE_16BIT_RX_HANDLE;
		service_16bit_tx_handle = SERVICE_16BIT_TX_HANDLE;
	#endif
	
	#if SERVICE_128BIT_CHOOSE
		service_128bit_rx_handle = SERVICE_128BIT_RX_HANDLE;
		service_128bit_tx_handle = SERVICE_128BIT_TX_HANDLE;
	#endif
#endif
}

//read???????,?????????
void gatt_read_cb(uint16_t att_handle,uint8_t *buffer, uint32_t *length)
{
	DEMO_PRINTF("read read_handle %d\n", att_handle);
	if(att_handle == 0x02){
		memcpy(buffer,device_name,device_name_len);
		*length = device_name_len;
		DEMO_PRINTF("device name handle %d name %s len %d\n", att_handle,device_name,device_name_len);
	}
}


////??????????????????????????
//void gatt_prepare_write_cb(uint16_t handle,uint16_t offset,uint8_t *pbuf,uint16_t len)
//{
//	int i;
//	DEMO_PRINTF("gatt_prepare_write_cb, handle=%02x, offset=%02x\n", handle, offset);
//	
//	if((handle == service_16bit_rx_handle) && (get_recv_event_busy() == 0x00)){
//		memcpy(ble_recv_data, pbuf, len);
//		ble_recv_length = len;
//		user_start_recv_event();
//	}
//}

//GATT??????????????
void gatt_rx_callback(uint16_t rx_handle, uint8_t *in,uint32_t in_len)
{
	DEMO_PRINTF("gatt_rx_callback, handle=%02x\n", rx_handle);
	
	if((rx_handle == service_16bit_rx_handle) && (get_recv_event_busy() == 0x00)){
		memcpy(ble_recv_data, in, in_len);
		ble_recv_length = in_len;
		user_start_recv_event();
	}
}

/**
 * @brief ???????????????????->?????
 * @param data ????????
 * @param length ???????
 */
void user_ble_send_data_notify(uint8_t *data, uint32_t length)
{
	if(length==0x00){
		DEMO_PRINTF("length==0x00 return\n");
		return;
	}
	
	if(lib_stack_state_get() != LIB_STACKSTATE_CONN){
		DEMO_PRINTF("not LIB_STACKSTATE_CONN return\n");
		return;
	}
	
	int ret = gatt_notify(service_16bit_tx_handle,data,length);
	DEMO_PRINTF("send ret=%d, service_16bit_tx_handle=%02x\n", ret, service_16bit_tx_handle);
}
