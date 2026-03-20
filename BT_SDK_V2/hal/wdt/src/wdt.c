#include "wdt.h"
#include "uart.h"	

void wdt_user_init(void)
{
	//使能看门狗门控
	cpu_crm_set(CRM_F_IWDT_EN, ENABLE);
	cpu_crm_reset(CRM_F_IWDT_SRST, ENABLE);
	//看门狗超时配置，单位32768-->1S
	wdt_reload_val(32768 * APP_WDT_TIMEOUT);
	//使能看门狗
	wdt_en(ENABLE);
}

//看门狗失能接口
void disable_wdt(void)
{
	cpu_crm_set(CRM_F_IWDT_EN, ENABLE);
	cpu_crm_reset(CRM_F_IWDT_SRST, ENABLE);
	
	*(volatile uint32_t *)0x50003000 = 0x8e690000;//看门狗失能 
}



