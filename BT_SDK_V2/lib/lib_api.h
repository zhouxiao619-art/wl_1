#ifndef __LIB_API_H__
#define __LIB_API_H__

#include <stdint.h>

#define SMP_MAX_NUM		1			//最多支持5个

#define BIT(n)   (1UL << n)

extern uint8_t tx_num_packet_flag; // 默认为 1：一次连接间隔可发多包； 为0时一次连接间隔只发1包；

typedef struct {
    /* 广播类型 */
    uint8_t adv_type;

    /* 是否为随机地址 */
    uint8_t addr_is_random;

    /* 数据范围 -512 ~ 511, 单位0.5dbm */
    int16_t raw_rssi;

    /* 消息CRC24 */
    uint32_t crc;

    /* 设备地址 */
    uint8_t addr[6];

    /* 广播数据长度 */
    uint8_t adv_data_len;

    /* 广播数据 */
    uint8_t adv_data[31];
}lib_scan_data_t;


typedef struct event_s {
    void *node;

    void (*callback)(void *cb_data, struct event_s *e);

    void *cb_data;

    uint8_t priority;

    uint8_t is_ready;
    
    uint8_t bp;
}event_t;

typedef struct ktimer_s {
    event_t event;

    /* 定时器到期的时间 */
    uint32_t expiry;
} ktimer_t;

//主机的连接请求参数
typedef struct{
	uint16_t connection_req_interval;
	uint16_t connection_req_latency;
	uint16_t connection_req_timeout;
	uint16_t connection_req_win_size;
	uint16_t connection_req_win_offset;
}conn_params_t;

typedef enum 
{    
	LIB_STACKEVT_CONNECT  = 0x01,    
	LIB_STACKEVT_DISCONN  = 0x02,	
	LIB_STACKEVT_L2CAP_CONN_PARA_REQ_RESULT  = 0x04,
	LIB_STACKEVT_ADV_REPORT  = 0x08,	
	LIB_STACKEVT_PAIR_REPORT = 0x10,
	LIB_STACKEVT_PAIR_REPORT_ALREADY_EXISTED = 0x20,
	LIB_STACKEVT_RSSI = 0x40,
	LIB_STACKEVT_PAIR_FAILED = 0x80,
	LIB_STACKEVT_PAIR_TIMEOUT = 0x100,
} tLib_StackEvent;

typedef enum 
{    
	LIB_STACKSTATE_IDLE,
	LIB_STACKSTATE_ADV,
	LIB_STACKSTATE_SCAN,
	LIB_STACKSTATE_CONN,
	
} tLib_StackState;

enum
{
    /* 高优先级组 */
    EVENT_PRIORITY_HIGH_GROUP = 0x80,

    /* 中优先级 */
    EVENT_PRIORITY_MIDDLE_GROUP = 0x40,

    /* 低优先级组 */
    EVENT_PRIORITY_LOWER_GROUP = 0x00,
};

typedef enum {
	IO_DRIVE_8MA = 0,
	IO_DRIVE_16MA = 1,
	IO_DRIVE_24MA = 2,
	IO_DRIVE_32MA = 3,
}IO_DRIVE;

typedef enum 
{
	TX_POWER_MINUX_10 = 2,
	TX_POWER_MINUX_6,
	TX_POWER_MINUX_4,
	TX_POWER_MINUX_2,
	TX_POWER_MINUX_1,
	TX_POWER_0,
	TX_POWER_2,
	TX_POWER_4,
	TX_POWER_6,
	TX_POWER_7,
}TX_POWER;

typedef struct 
{
	unsigned char addr_type;
	unsigned char bd_addr[6];
	unsigned char irk[16];
	unsigned char rand[8];
	unsigned char ediv[2];
	unsigned char ltk[16];
}SMP_INFO_S;

typedef struct 
{
	unsigned char total_num;
	unsigned char cur_index;
	SMP_INFO_S smp_info_group[SMP_MAX_NUM];
}SMP_INFO_G;

//广播信道及使能随机地址
#define BLE_ADV_CH37    					BIT(0)
#define BLE_ADV_CH38    					BIT(1)
#define BLE_ADV_CH39    					BIT(2)
#define BLE_ADDR_RANDOM_TXADDR		BIT(6)
#define BLE_ADDR_RANDOM_RXADDR		BIT(7)


#define BLE_RX_FILT_ADV_IND             BIT(0)
#define BLE_RX_FILT_ADV_DIRECT_IND      BIT(1)
#define BLE_RX_FILT_ADV_NONCONN_IND     BIT(2)
#define BLE_RX_FILT_SCAN_REQ            BIT(3)
#define BLE_RX_FILT_SCAN_RSP            BIT(4)
#define BLE_RX_FILT_CONNECT_REQ         BIT(5)
#define BLE_RX_FILT_ADV_SCAN_IND        BIT(6)

//广播类型参数,adv_type
#define ADV_TYPE_ADV_IND		0
#define ADV_TYPE_ADV_DIRECT_IND	1
#define ADV_TYPE_ADV_NONCONN	2
#define ADV_TYPE_ADV_SCAN_IND	6


#define GATT_Character_Property_Read	0x02
#define GATT_Character_Property_Write_Without_Rsp	0x04
#define GATT_Character_Property_Write	0x08
#define GATT_Character_Property_Notify	0x10
#define GATT_Character_Property_Indicate	0x20
#define GATT_Character_Property_Read_Dynamic	0x80

#define	GATT_Character_Read							GATT_Character_Property_Read
#define	GATT_Character_RW								GATT_Character_Property_Read 	| GATT_Character_Property_Write
#define	GATT_Character_R_RDynamic					GATT_Character_Property_Read 	| GATT_Character_Property_Read_Dynamic
#define	GATT_Character_R_WNoRsp						GATT_Character_Property_Read 	| GATT_Character_Property_Write_Without_Rsp

#define GATT_Character_RW_WNoRsp						GATT_Character_Property_Write_Without_Rsp|GATT_Character_Property_Read|GATT_Character_Property_Write
#define GATT_Character_W_WNoRsp						GATT_Character_Property_Write_Without_Rsp|GATT_Character_Property_Write
#define	GATT_Character_Rauth_RW_WNoRsp				GATT_Character_Property_Read_Dynamic|GATT_Character_Property_Write_Without_Rsp| \
														GATT_Character_Property_Read|GATT_Character_Property_Write



//#define	GATT_Character_Notify_W						GATT_Character_Property_Notify 	| GATT_Character_Property_Write
#define	GATT_Character_Ready_Indicate				GATT_Character_Property_Read 	| GATT_Character_Property_Indicate
#define GATT_Character_Notify						GATT_Character_Property_Notify
#define	GATT_Character_Notify_R						GATT_Character_Property_Notify 	| GATT_Character_Property_Read
#define	GATT_Character_Notify_W						GATT_Character_Property_Notify 	| GATT_Character_Property_Write


#define GATT_UUID_Service_Primary	0x2800
#define GATT_UUID_Service_Secondary	0x2801
#define GATT_UUID_Include			0x2802
#define GATT_UUID_Characteristic	0x2803
#define GATT_UUID_Descriptor_ext_Properties		0x2900
#define GATT_UUID_Descriptor_User				0x2901
#define GATT_UUID_Descriptor_Client_Config		0x2902
#define GATT_UUID_Descriptor_Server_Config		0x2903
#define GATT_UUID_Descriptor_Format				0x2904
#define GATT_UUID_Descriptor_Format_Aggregate	0x2905

#define UUID_Service_GAP		0x1800
#define UUID_Service_GATT		0x1801
#define UUID_Service_Device_Information		0x180A
#define UUID_Service_Battery	0x180F
#define UUID_Service_HID		0x1812



#define UUID_Characteristic_Device_Name			0x2A00
#define UUID_Characteristic_Appearance			0x2A01
#define UUID_Characteristic_Periph_Preferred_Conn_Para			0x2A04
#define UUID_Characteristic_Service_Changed     0x2A05
#define UUID_Characteristic_Battery_Level		0x2A19
#define UUID_Characteristic_Service_Changed		0x2A05
#define UUID_Characteristic_HID_Information		0x2A4A
#define UUID_Characteristic_HID_Report_Map		0x2A4B
#define UUID_Characteristic_HID_Control_Point	0x2A4C
#define UUID_Characteristic_HID_Report			0x2A4D
#define UUID_Characteristic_Protocol_Mode		0x2A4E
#define UUID_Characteristic_PNP_ID				0x2A50

#define GATT_UUID_Descriptor_Manufacturter_Name_String  	0x2A29
#define	GATT_UUID_Descriptor_Mode_Number_String				0x2A24
#define GATT_UUID_Descriptor_Serial_Number_String 			0x2A25
#define GATT_UUID_Descriptor_Hardware_Revision_String 		0x2A27
#define GATT_UUID_Descriptor_Firmware_Revision_String 		0x2A26
#define GATT_UUID_Descriptor_Software_Revision_String 		0x2A28


#define UUID_Descriptor_GATT_Client_Charac_Config	0x2902

#define UUID_Descriptor_HID_Report_Reference		0x2908

#define GATT_PDU_ERR_RSP				0x01
#define GATT_PDU_EXCHANGE_MTU_REQ		0x02
#define GATT_PDU_EXCHANGE_MTU_RSP		0x03
#define GATT_PDU_FIND_INFORMATION_REQ	0x04
#define GATT_PDU_FIND_INFORMATION_RSP	0x05
#define GATT_PDU_FIND_BY_TYPE_VALUE_REQ	0x06
#define GATT_PDU_FIND_BY_TYPE_VALUE_RSP	0x07
#define GATT_PDU_READ_BY_TYPE_REQ		0x08
#define GATT_PDU_READ_BY_TYPE_RSP		0x09
#define GATT_PDU_READ_REQ				0x0A
#define GATT_PDU_READ_RSP				0x0B
#define GATT_PDU_READ_BLOB_REQ			0x0C
#define GATT_PDU_READ_BLOB_RSP			0x0D
#define GATT_PDU_READ_BY_GROUP_TYPE_REQ	0x10
#define GATT_PDU_READ_BY_GROUP_TYPE_RSP	0x11
#define GATT_PDU_WRITE_REQ				0x12
#define GATT_PDU_WRITE_RSP				0x13
#define GATT_PDU_HANDLE_VALUE_NOTIFICATION		0x1B
#define GATT_PDU_HANDLE_VALUE_INDICATION		0x1D
#define GATT_PDU_HANDLE_VALUE_CONFIRMATION		0x1E
#define GATT_PDU_WRITE_CMD				0x52

#define GATT_ERROR_CODE_Insufficient_Authentication		0x05
#define GATT_ERROR_CODE_Invalid_Offset			0x07
#define GATT_ERROR_CODE_Attribute_Not_Found		0x0A

#define GATT_Permit_None   					0
#define GATT_Permit_Read   					1
#define GATT_Permit_Read_Need_Auth	2
#define GATT_Permit_Write   				4
#define GATT_Permit_Write_Need_Auth	8
#define GATT_Permit_RW   						(GATT_Permit_Read|GATT_Permit_Write)
#define GATT_Character_Read_Auth_Write_Auth 	(GATT_Permit_Read_Need_Auth|GATT_Permit_Write_Need_Auth)


//蓝牙库初始化接口
void lib_init(void *gatt_database_addr);

//注册事件处理函数
void lib_register_stack_event_cb(void (*callback)(uint32_t evt,uint32_t info),tLib_StackEvent evt_mask);

//注册GATT数据接收数据函数,外部定义实现
void gatt_rx_callback(uint16_t rx_handle,uint8_t *in,uint32_t in_len);

//设置蓝牙地址
void lib_stack_set_bdaddr(uint8_t addr[6]);

//获取蓝牙地址
void lib_stack_get_bdaddr(uint8_t addr[6]);

//蓝牙广播使能失能
void lib_stack_adv_enable(uint32_t state);

//设置蓝牙广播数据，名字和外观等
void lib_stack_set_adv_data(uint8_t *in,uint32_t in_len);

//设置扫描响应数据
void lib_stack_set_scan_rsp_data(uint8_t *in,uint32_t in_len);

//设置广播参数，蓝牙广播间隔和广播类型，adv_interval 单位ms
void lib_stack_set_adv_param(uint32_t adv_type,uint16_t adv_interval);

//设置扫描参数，扫描窗口和扫描间隔，单位ms,scan_window = scan_interval则100%扫描占空比
void lib_stack_set_scan_param(uint16_t scan_window,uint16_t scan_interval);

//设置扫描过滤
void lib_stack_scan_filter(uint32_t scan_filt);

//设置扫描使能和失能
void lib_stack_scan_enable(uint32_t state);

//扫描返回函数，外部定义实现
void lib_scan_cb(lib_scan_data_t *scan_data);

//蓝牙设备主动断链接口
void lib_stack_disconn(void);

//蓝牙库处理接口，放在while(1)中
void lib_stack_process(void);

//设置设备发射功率
void lib_set_power(TX_POWER power);

//获取设备发射功率
uint8_t lib_get_power(void);

//设置晶振频偏
void set_xtal_cap(uint8_t val);

//获取晶振频偏
uint8_t get_xtal_cap(void);

//事件Event初始化，可设置事件优先级，都高于while(1)循环里的处理
void lib_event_init(event_t *event, void (*event_handler)(void *cb_dat, event_t *e),uint8_t priority);

//触发一个事件
void lib_event_post(event_t *event);

//取消一个事件
void lib_event_cancel(event_t *event);

//定时器事件初始化，可设置事件优先级，都高于while(1)循环里的处理，补充说明：定时器注册和开启应该放在 lib_init 接口之后
void lib_timer_init(ktimer_t *timer, void (*timer_handler)(void *cb_dat, event_t *e),uint8_t priority);

//触发一个定时器，单位ms
void lib_timer_start(ktimer_t *timer,uint32_t timeout_ms);

//取消定时器
void lib_timer_stop(ktimer_t *timer);

//使能休眠
void lib_stack_enter_sleep(void);

//休眠后设备恢复函数，外部定义实现
void hal_system_resume_devices(void);

//休眠前设备挂起函数，外部定义实现
void hal_system_suspend_devices(void);

//阻止系统休眠
void lib_sleep_ref(void);

//系统休眠解引用，为0时才可进入休眠
void lib_sleep_unref(void);

//注册128位主服务
uint16_t gatt_register_service_primary_uuid128(uint16_t att_val_len,uint8_t *att_val);

//注册128位特性
uint16_t gatt_register_character_uuid128(uint8_t property,uint16_t type_uuid_len,uint8_t * type_uuid);

//注册128位特性值
uint16_t gatt_register_character_value_uuid128(uint16_t att_type_len,uint8_t *att_type,uint16_t att_val_len,uint8_t *att_val);

//注册128位特性描述符
uint16_t gatt_register_descriptor_uuid128(uint16_t att_type_len,uint8_t * att_type,uint16_t att_val_len,uint8_t *att_val);

//注册16位主服务
uint16_t gatt_register_service_primary(uint16_t att_val);

//注册16位特性
uint16_t gatt_register_character(uint8_t property,uint16_t type_uuid);

//注册16位特性值
uint16_t gatt_register_character_value(uint16_t att_type,uint16_t att_val_len,uint8_t *att_val);

//注册特16位性描述符
uint16_t gatt_register_descriptor(uint16_t att_type,uint16_t att_val_len,uint8_t *att_val);

//服务注册结束标识
int gatt_register_service_end(uint16_t start_handle,uint16_t end_handle);

//修改特征值
void gatt_set_handle_value(uint16_t handle,uint8_t * val, uint16_t val_len);

//返回服务表大小
int gatt_get_db_size(void);

//read回调函数,外部定义实现
void gatt_read_cb(uint16_t att_handle,uint8_t *buffer, uint32_t *length);

//read blob回调函数,外部定义实现
void gatt_read_blob_cb(uint16_t att_handle,uint8_t *buffer, uint32_t *length, uint16_t offset);

//read_by_type回调函数，外部定义实现
void gatt_read_by_type_cb(uint8_t * requst_buffer, uint32_t * requst_buffer_len);

//MTU交换回调函数，外部定义实现
void gatt_exchange_mtu_cb(uint16_t rx_mtu_len);

//长数据写回调函数，外部定义实现
void gatt_prepare_write_cb(uint16_t handle,uint16_t offset,uint8_t *pbuf,uint16_t len);

//注册服务特性权限
void gatt_register_permit(uint8_t permit);

//发送数据接口
int gatt_notify(uint16_t handle, uint8_t *in, uint32_t in_len);

////发送数据接口
int gatt_indicate(uint16_t handle, uint8_t *in, uint32_t in_len);

////发起连接参数更新接口
void l2cap_send_conn_para_update_req(uint16_t interval_min,uint16_t interval_max,uint16_t slave_latency,uint16_t timeout);

//设备发起安全请求
void sm_pair_request(void);

//配对完成回调，外部定义实现
void sm_pairing_complete_cb(void);

//系统锁中断，请务必调用此接口锁中断，不会影响蓝牙
int lib_irq_lock(void);

//系统解中断，和lib_irq_lock成对出现
void lib_irq_unlock(int key);

//获取系统时钟，单位us
uint32_t lib_get_tick_us(void);

//芯片初始化
void soc_init(int version);

//读射频寄存器
#define read_rf_reg(addr)          radio_modem_read_reg(addr)

//写射频寄存器
#define write_rf_reg(addr, val)    radio_modem_write_reg(addr, val)

//设置GPIO驱动力
void set_io_drive(IO_DRIVE DRIVE);

//获取GPIO驱动力
IO_DRIVE gei_io_drive(void);

//写OTP，adr写的地址必须从0x4080~0x8000（用户自己定义写的地址，不要和代码code地址冲突即可），地址且要4字节对齐
int lib_otp_write(uint32_t adr, uint32_t sz, uint8_t *buf);

//读OTP，adr读的地址必须从0x4080~0x8000
void lib_otp_read(uint32_t adr, uint32_t sz, uint8_t *buf);

//读取芯片序列号,每个芯片唯一，可以用来产生蓝牙地址
uint32_t lib_get_number(void);

//获取蓝牙状态
tLib_StackState lib_stack_state_get(void);

//注册sm配对信息改变回调函数
void lib_register_sm_key_info_changed_cb(void(*callback)(uint8_t *info,uint32_t info_len));

//sm配对信息设置
void sm_set_remote_key_info(uint8_t * info, uint32_t info_len);

//sm安全使能,设置为1后需要记录LTK
void lib_sm_security(uint8_t enable);

//设置ATT服务表
void lib_gatt_db_set(void *db);

//获取lib版本号
uint16_t get_lib_version(void);

//设置高温晶振频偏补偿值，高温下晶振频偏会偏大，需要增加负载电容值，默认值25。可以根据实际情况调节。可以不调用，发现高温频偏异常再适当增加
void set_xtal_compensate_offset(uint8_t offset);

//sm存储个数内存初始化，由外面分配大小实现。外面定义变量 SMP_INFO_G g_sm_mem，sm_mem_init(&g_sm_mem,SMP_MAX_NUM),可以修改SMP_MAX_NUM
void sm_mem_init(void *sm_mem, uint8_t max_num);

//设置sm配对超时时间，单位s，不设置默认是20s
void sm_timeout_set(uint8_t timeout_s);

//使用芯片Vbat供电必须要打开5v供电，否则功能异常。必须放在soc_init函数之后调用
void lib_soc_5v_power_open(uint8_t enable);

//设置广播信道和是否使能随机地址，如：lib_stack_set_adv_ch_and_addr(BLE_ADV_CH37 | BLE_ADV_CH38 | BLE_ADV_CH38 | BLE_ADDR_RANDOM_TXADDR);
void lib_stack_set_adv_ch_and_addr(uint8_t ch_and_addr);

/***************************************************************************************************************
设置广播事件结束回调函数，必须在开启广播使能之前设置。用在快速发广播的应用，设置后会影响连接。有连接功能的不要设置该接口
static void on_adv_end(void *ctx, void *ev)
{
	ktimer_t *adv_timer = (ktimer_t *)0x20000d9c;
  lib_timer_start_us(adv_timer, 40 * 1000);//40ms
}
lib_adv_event_end_register(on_adv_end)
****************************************************************************************************************/
void lib_adv_event_end_register(void (*on_adv_end)(void *ctx, void *ev));

//启动定时器接口，时间以us计数
void lib_timer_start_us(ktimer_t *timer,uint32_t timeout_us);

//休眠接口，需要放在while(1)调度里
uint32_t lib_stack_sleep(void);

//关闭boot阶段PA3上拉功能（防止上电时候该IO会产生一个短暂的上拉脉冲），调用后芯片就无法进行boot了，在lib_init后调用
void lib_gpio3_rx_disable(void);

//校验晶振频率，外部告诉是32M还是16M
void soc_xtal_freq_check(int freq_is_32m);

#endif



