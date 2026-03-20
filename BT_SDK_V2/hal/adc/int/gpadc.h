/**
******************************************************************************************************************************************************************************************
*@file                       gpadc.h
*@author         
*@data                       
*@version                    v1.0
*@par Copyright(c):
*
*@par History:
*     version:author,date,desc
*     v1.0   :create this file
*
*******************************************************************************************************************************************************************************************
*/

#ifndef GPADC_H_
#define GPADC_H_

#include "cpu.h"
#include "gpio.h"

#define DEVICE_ADC_EN			1

#define GPADC_CH_A0            0
#define GPADC_CH_A1            1
#define GPADC_CH_A2            2
#define GPADC_CH_A3            3

#define GPADC_MODE_ADC        0
#define GPADC_MODE_OFFSET    1
#define GPADC_MODE_TEMP        2
#define GPADC_MODE_POWER    3

#define GPADC_SINGLE_CONV        0
#define GPADC_MULTI_CONV        1
#define GPADC_CON_MULT_CONV        2

typedef struct
{
    uint16_t min;
    uint16_t max;
    float    val;
}v_rang_t;

enum ADC_NUM_CLASS
{
	CLASS_ZERO,
	CLASS_25,
	CLASS_50,
	CLASS_60,
	CLASS_70,
	CLASS_80,
	CLASS_90,
	CLASS_100,
};

/*
*   io检测外部电压表
*   精度为0.2V
*   min=0.05V,max=1.19V
*/
//#define GPADC_OUTVOL_TABLE                 \
//{                                       \
//    {33, 51, 0.05}, {52, 67, 0.07},     \
//	{68, 83, 0.09}, {84, 99, 0.11},     \
//    {100, 116, 0.13}, {117, 131, 0.15}, \
//    {132, 149, 0.17}, {150, 164, 0.19}, \
//    {165, 181, 0.21}, {182, 197, 0.23}, \
//    {198, 214, 0.25}, {215, 229, 0.27}, \
//    {230, 246, 0.29}, {247, 263, 0.31}, \
//    {264, 279, 0.33}, {280, 297, 0.35}, \
//    {298, 312, 0.37}, {313, 329, 0.39}, \
//    {330, 345, 0.41}, {346, 362, 0.43}, \
//    {363, 377, 0.45}, {378, 395, 0.47}, \
//    {396, 410, 0.49}, {411, 427, 0.51}, \
//    {428, 443, 0.53}, {444, 459, 0.55}, \
//    {460, 476, 0.57}, {477, 492, 0.59}, \
//    {493, 508, 0.61}, {509, 525, 0.63}, \
//    {526, 540, 0.65}, {541, 557, 0.67}, \
//    {558, 573, 0.69}, {574, 590, 0.71}, \
//    {591, 606, 0.73}, {607, 622, 0.75}, \
//    {623, 638, 0.77}, {639, 655, 0.79}, \
//    {656, 672, 0.81}, {673, 687, 0.83}, \
//    {688, 704, 0.85}, {705, 720, 0.87}, \
//    {721, 737, 0.89}, {738, 752, 0.91}, \
//    {753, 770, 0.93}, {771, 786, 0.95}, \
//    {787, 802, 0.97}, {803, 817, 0.99}, \
//    {818, 835, 1.01}, {836, 850, 1.03}, \
//    {851, 867, 1.05}, {868, 882, 1.07}, \
//    {883, 900, 1.09}, {901, 915, 1.11}, \
//    {916, 932, 1.13}, {933, 947, 1.15}, \
//    {948, 963, 1.17}, {964, 978, 1.19}, \
//}

/*
*   检测内部电池电压
*   精度为0.3V
*   min=2.62V
*/
//#define GPADC_INVOL_TABLE                 \
//{                                       \
//    {462, 471, 2.63}, {472, 481, 2.66}, \
//    {482, 491, 2.69}, {492, 501, 2.72}, \
//    {502, 508, 2.75}, {509, 515, 2.78}, \
//    {516, 522, 2.81}, {523, 529, 2.84}, \
//    {530, 534, 2.87}, {535, 539, 2.90}, \
//    {540, 544, 2.93}, {545, 949, 2.96}, \
//    {550, 555, 2.99}, {556, 561, 3.02}, \
//    {562, 567, 3.05}, {568, 573, 3.08}, \
//    {574, 579, 3.11}, {580, 585, 3.14}, \
//    {586, 591, 3.17}, {592, 597, 3.20}, \
//    {598, 602, 3.23}, {603, 607, 3.26}, \
//    {608, 612, 3.29}, {613, 616, 3.32}, \
//    {617, 620, 3.35}, {621, 624, 3.38}, \
//    {625, 628, 3.40},                   \
//}


#define gpadc_ch_select(ch_ax)            WS_WRITE_FIELD(GPADC_BASE, GPADC_F_GPADC_SEL1, ch_ax)

/*
 *    GPADC工作模式和转换方式的配置
 * mode ->>                
 * GPADC_MODE_ADC        ADC模式采集外部电压
 * GPADC_MODE_OFFSET    VCM的offset功能
 * GPADC_MODE_TEMP        温度检测
 * GPADC_MODE_POWER        内部电源电压检测
 *
 * conv ->>
 * GPADC_SINGLE_CONV    单纯转换
 * GPADC_MULTI_CONV        多次转换
 * GPADC_CON_MULT_CONV  连线多次转换
 */
#define gpadc_mode_select(mode, conv)                            \
    do{                                                            \
        WS_WRITE_FIELD(GPADC_BASE, GPADC_F_GPADC_CH_SEL, mode);    \
        WS_WRITE_FIELD(GPADC_BASE, GPADC_F_GPADC_MODE, conv);    \
    }while(0)

/*
 * 当转换方式位GPADC_SINGLE_CONV（单次转换）时每次读取数据前需要
 * 重新使能GPADC.
 * 
 * val: ENABLE/DISABLE
 */
#define gpadc_en(val)                    WS_WRITE_FIELD(GPADC_BASE, GPADC_F_GPADC_EN, val)

/* 获取当前FIFO的数据个数 */
#define gpadc_fifo_level()                WS_READ_FIELD(GPADC_BASE, GPADC_F_FIFO_LEVEL)

/* 设置FIFO中断阈值 */
#define gpadc_fifo_threshold(val)        WS_WRITE_FIELD(GPADC_BASE, GPADC_F_FIFO_THRESHOLD, val)

/* 刷新FIFO */
#define gpadc_fifo_flush()                WS_WRITE_FIELD(GPADC_BASE, GPADC_F_FIFO_FLUSH, 1)

/*
 * 获取中断状态
 * FIELD ->>
 * GPADC_F_INT_OVFL        FIFO写满溢出
 * GPADC_F_INT_ALERT    FIFO level > FIFO threshold
 * GPADC_F_INT_DONE        转换完成
 */
#define gpadc_it_status(FIELD)            WS_READ_FIELD2(GPADC_BASE, FIELD)
#define gpadc_it_clear(FIELD)            WS_WRITE_FIELD2(GPADC_BASE, FIELD, 1)

/*
 * 中断使能
 * FIELD ->>
 * GPADC_F_INT_OVFL_EN ：    FIFO写满溢出中断使能
 * GPADC_F_INT_ALERT_EN：    FIFO level > FIFO threshold中断使能
 * GPADC_F_INT_DONE_EN：        转换完成中断使能
 *
 * val ->>    ENABLE/DISABLE
 */
#define gpadc_it_en(FIELD, val)            WS_WRITE_FIELD2(GPADC_BASE, FIELD, val)

/*
 * 中断mask
 * FIELD ->>
 * GPADC_F_INT_OVFL_MASK
 * GPADC_F_INT_ALERT_MASK
 * GPADC_F_INT_DONE_MASK
 * 
 * val ->> ENABLE/DISABLE
 */
#define gpadc_it_mask(FIELD, val)        WS_WRITE_FIELD2(GPADC_BASE, FIELD, val)

///GPADC FIFO Mode sample rate = 1.33Mhz/(sample_rate+1)
#define ADC_SAMPLE_RATE_8Khz    167
#define ADC_SAMPLE_RATE_16Khz    83

///设置采样率
static inline void gpadc_set_sample_rate(uint8_t sample_rate)
{
    WS_WRITE_FIELD(GPADC_BASE, GPADC_F_SAMPLE_RATE, sample_rate);
}

///设置FIFO阈值
static inline void gpadc_set_fifo_threshold(uint8_t fifo_threshold)
{
    WS_WRITE_FIELD(GPADC_BASE, GPADC_F_FIFO_THRESHOLD, fifo_threshold);
}

///读取数据
static inline uint16_t gpadc_read_data()
{    
    switch(WS_READ_FIELD(GPADC_BASE, GPADC_F_GPADC_MODE))
    {
    case GPADC_SINGLE_CONV:
        gpadc_en(ENABLE);
        while(WS_READ_FIELD(GPADC_BASE, GPADC_F_GPADC_EN));
    case GPADC_MULTI_CONV:
        return ((uint16_t)WS_READ_FIELD(GPADC_BASE, GPADC_F_GPADC_DATA));
	 return ((uint16_t)WS_READ_FIELD(GPADC_BASE, GPADC_F_GPADC_DATA));
    case GPADC_CON_MULT_CONV:
        return ((uint16_t)WS_READ_FIELD(GPADC_BASE, GPADC_F_FIFO_DATA));
    default:
        return 0;
    }
}

static inline bool gpadc_get_status(void)
{
    return WS_READ_FIELD(GPADC_BASE, GPADC_F_GPADC_EN);
}

uint8_t get_adc_class(void);
void set_adc_class(uint8_t num);

void user_adc_init(void);
uint16_t user_get_adc(void);

uint8_t user_adc_num_pro(uint32_t num);
void user_adc_ack_data_pro(uint8_t adc_class, uint8_t *ack_data);

int gpadc_get_vbat(void);

#endif
