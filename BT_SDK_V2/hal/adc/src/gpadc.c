#include "gpadc.h"
#include "lib_api.h"
#include "uart.h"

#define ADC_IO				GPIO_PA0

uint8_t adc_class = CLASS_100;

uint8_t get_adc_class(void)
{
	return adc_class;
}

void set_adc_class(uint8_t num)
{
	adc_class = num;
}

//ADC初始化
void user_adc_init(void)
{
//	gpio_remapmode_set(ADC_IO, GPIO_PINMUX_GPIO);
//	gpio_pinmode_set(ADC_IO, GPIO_INOUT_MODE_OUTPUT);
//	gpio_reset_pin(ADC_IO);
//	gpadc_ch_select(ADC_IO);
//	//设置ADC模式
//	gpadc_mode_select(GPADC_MODE_ADC, GPADC_SINGLE_CONV);
//	gpadc_en(ENABLE);
}

void bubble_sort(uint16_t arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // 交换相邻元素
                uint16_t temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int gpadc_get_vbat(void)
{
	int val;
	cpu_crm_set(CRM_F_GPADC_EN, ENABLE);
	cpu_crm_set(CRM_F_BB_EN, ENABLE);
//	write_rf_reg(0x204,read_rf_reg(0x204) | 0x4000);
	gpadc_mode_select(GPADC_MODE_POWER, GPADC_MULTI_CONV);	
	for(int i=0; i<0xff; i++);
	gpadc_en(ENABLE);
	for(int i=0; i<0xff; i++);
	val = gpadc_read_data();
//	write_rf_reg(0x204,read_rf_reg(0x204) & ~0x4000);
	gpadc_en(DISABLE);
	return val;
}

//ADC数据获取
uint16_t user_get_adc(void)
{
//	int i=0;
//	uint16_t adc[17];

//	gpio_pinmode_set(ADC_IO,	GPIO_INOUT_MODE_FLOATING_IN);
//	for(i=0;i<200;i++);
//	gpio_remapmode_set(ADC_IO, GPIO_PINMUX_Analog);

//	gpadc_ch_select(GPADC_CH_A0);
//	gpadc_mode_select(GPADC_MODE_ADC, GPADC_SINGLE_CONV);
//	for(i=0;i<200;i++);

//	for(i=0;i<17;i++){
//		gpadc_en(ENABLE);
//		lib_stack_process();	
//		while(WS_READ_FIELD(GPADC_BASE, GPADC_F_GPADC_EN));
//		adc[i] = gpadc_read_data();
//	}
////	gpio_pinmode_set(ADC_IO, GPIO_INOUT_MODE_FLOATING_IN);
//	
//	gpio_remapmode_set(ADC_IO, GPIO_PINMUX_GPIO);
//	gpio_pinmode_set(ADC_IO, GPIO_INOUT_MODE_OUTPUT);
//	gpio_reset_pin(ADC_IO);
//	
//	bubble_sort(adc,17);
//	return adc[12];
}

//uint8_t adc_change_en = 0x00;
//#define ADC_CHANGE_EN_TIME	5
//uint8_t user_adc_num_pro(uint32_t num)
//{
//	if(num>=834){
//		if(get_adc_class() != CLASS_100){
//			if(adc_change_en++ >= ADC_CHANGE_EN_TIME){
//				adc_change_en = 0x00;
//				set_adc_class(CLASS_100);
//			}
//		}
//	}else if((num<834) && (num>=812)){
//		if(get_adc_class() != CLASS_90){
//			if(adc_change_en++ >= ADC_CHANGE_EN_TIME){
//				adc_change_en = 0x00;
//				set_adc_class(CLASS_90);
//			}
//		}
//	}else if((num<812) && (num>=800)){
//		if(get_adc_class() != CLASS_80){
//			if(adc_change_en++ >= ADC_CHANGE_EN_TIME){
//				adc_change_en = 0x00;
//				set_adc_class(CLASS_80);
//			}
//		}
//	}else if((num<800) && (num>=785)){
//		if(get_adc_class() != CLASS_70){
//			if(adc_change_en++ >= ADC_CHANGE_EN_TIME){
//				adc_change_en = 0x00;
//				set_adc_class(CLASS_70);
//			}
//		}
//	}else if((num<785) && (num>=774)){
//		if(get_adc_class() != CLASS_60){
//			if(adc_change_en++ >= ADC_CHANGE_EN_TIME){
//				adc_change_en = 0x00;
//				set_adc_class(CLASS_60);
//			}
//		}
//	}else if((num<774) && (num>=755)){
//		if(get_adc_class() != CLASS_50){
//			if(adc_change_en++ >= ADC_CHANGE_EN_TIME){
//				adc_change_en = 0x00;
//				set_adc_class(CLASS_50);
//			}
//		}
//	}else if((num<755) && (num>=740)){
//		if(get_adc_class() != CLASS_25){
//			if(adc_change_en++ >= ADC_CHANGE_EN_TIME){
//				adc_change_en = 0x00;
//				set_adc_class(CLASS_25);
//			}
//		}
//	}else if(num<740){
//		if(get_adc_class() != CLASS_ZERO){
//			if(adc_change_en++ >= ADC_CHANGE_EN_TIME){
//				adc_change_en = 0x00;
//				set_adc_class(CLASS_ZERO);
//			}
//		}
//	}
//	
//	return get_adc_class();
//}

//void user_adc_ack_data_pro(uint8_t adc_class, uint8_t *ack_data)
//{
//	uint32_t num; 
//	switch(adc_class)
//	{
//		case CLASS_100: 	{num = 3960; }break;
//		case CLASS_90: 		{num = 3900; }break;
//		case CLASS_80: 		{num = 3820; }break;
//		case CLASS_70: 		{num = 3790; }break;
//		case CLASS_60: 		{num = 3710; }break;
//		case CLASS_50: 		{num = 3650; }break;
//		case CLASS_25: 		{num = 3580; }break;
//		case CLASS_ZERO: 	{num = 3000; }break;
//	}
//	ack_data[6] = ((num>>8) & 0xFF);
//	ack_data[7] = (num & 0xFF);
//	DEMO_PRINTF("ADC: %d %02x %02x\n",num, ack_data[6], ack_data[7]);
//}

