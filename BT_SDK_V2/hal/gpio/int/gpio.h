/**
******************************************************************************************************************************************************************************************
*@file                       gpio.h
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

#ifndef GPIO_H_
#define GPIO_H_

#include "cpu.h"

#define GPIO_PA0			0
#define GPIO_PA1			1
#define GPIO_PA2			2
#define GPIO_PA3			3
#define GPIO_PA4			4
#define GPIO_PA5			5
#define GPIO_PA6			6
#define GPIO_PA7			7
#define GPIO_PB0			8
#define GPIO_PB1			9
#define GPIO_PB2			10
#define GPIO_PB3			11
#define GPIO_PB4			12
#define GPIO_PB5			13
#define GPIO_PB6			14
#define GPIO_PB7			15
    
typedef enum
{
    GPIO_INOUT_MODE_FLOATING_IN        = 0,
    GPIO_INOUT_MODE_PULLUP_IN         = 1,
    GPIO_INOUT_MODE_DROPDOWN_IN     = 2,
    GPIO_INOUT_MODE_OUTPUT             = 3,
}gpio_inout_mode;

typedef enum
{
    GPIO_PINMUX_GPIO                 = 0,
    GPIO_PINMUX_UART0_RX             = 1,
    GPIO_PINMUX_UART0_TX            = 2,
    GPIO_PINMUX_UART0_CTSN            = 3,
    GPIO_PINMUX_UART0_RTSN            = 4,
    //5-8£ºReserved
    GPIO_PINMUX_SPI0_CSN             = 9,
    GPIO_PINMUX_SPI0_CLK             = 10,
    GPIO_PINMUX_SPI0_MOSI             = 11,
    GPIO_PINMUX_SPI0_MISO             = 12,
    //13-14: Reserved
    GPIO_PINMUX_PWM_CH0             = 19,
    GPIO_PINMUX_PWM_CH1             = 15,
    GPIO_PINMUX_PWM_CH2             = 16,
    GPIO_PINMUX_PWM_CH3             = 17,
    GPIO_PINMUX_PWM_CH4             = 18,
    GPIO_PINMUX_PWM_CH5             = 32,
    //20: Reserved
    GPIO_PINMUX_QDEC_CHA             = 21,
    GPIO_PINMUX_QDEC_CHB             = 22,
    GPIO_PINMUX_QDEC_IDX             = 23,
    //26-24: Reserved
    GPIO_PINMUX_JTAG_TCK_SWCLK         = 27,
    GPIO_PINMUX_JTAG_TDO             = 28,
    GPIO_PINMUX_JTAG_TDI             = 29,
    GPIO_PINMUX_JTAG_TMS_SWDIO         = 30,
    GPIO_PINMUX_JTAG_TRST             = 31,
    //33-47:Reserved
    GPIO_PINMUX_Analog                 = 48, //(GPADC£ºP0[7:0]; AUADC: P0[2],P0[3];IR: P0[4])
    GPIO_PINMUX_IrDA_RX             = 49,
    GPIO_PINMUX_IrDA_TX             = 50,
    GPIO_PINMUX__12M_CLOCK             = 51,
    GPIO_PINMUX_SYS_DBG3              = 52,//(for test only)
    GPIO_PINMUX_SYS_DBG2             = 53, //(for test only)
    GPIO_PINMUX__16M_OSCRC_CLOCK     = 54,
    //55-56:Reserved
    GPIO_PINMUX_SYS_DBG1             = 57, //(for test only) 
    GPIO_PINMUX_CLK32K                 = 58, //(for test only)
    GPIO_PINMUX_MDM_CLK             = 59, //(for test only)
    GPIO_PINMUX_BTDM_DBG0             = 60, //(for test only)  
    GPIO_PINMUX_BTDM_DBG1             = 61, //(for test only)
    GPIO_PINMUX_BTDM_DBG2             = 62, //(for test only)
    GPIO_PINMUX_BTDM_DBG3             = 63, //(for test only)
}gpio_remap_mode;

typedef enum
{
    gpio_rising_edge    = 1,
    gpio_falling_edge   = 2,
    gpio_bilateral_edge = 3,
}gpio_int_type;

/* ½Ó¿ÚÉùÃ÷ */

#define gpio_pinmode_set(pin, mode)            WS_WRITE_FIELD2(GPIO_BASE, (pin/8)*0x10+0x08, (pin%8)*2, (pin%8)*2+1, mode)

#define gpio_remapmode_set(pin, mode)        WS_WRITE_FIELD2(GPIO_BASE, (pin/4)*0x04+0x40, (pin%4)*8, (pin%4)*8+5, mode)

#define gpio_set_pin(pin)                    WS_WRITE_FIELD2(GPIO_BASE, (pin/8)*0x10+0x04, pin%8, pin%8, 1)

#define gpio_reset_pin(pin)                    WS_WRITE_FIELD2(GPIO_BASE, (pin/8)*0x10+0x04, (pin%8)+8, (pin%8)+8, 1)

#define gpio_get_pin(pin)                    WS_READ_FIELD2(GPIO_BASE, (pin/8)*0x10, (pin%8)+8, (pin%8)+8)

//WS_WRITE_FIELD2(GPIO_BASE, 0x80, (pin/8)?8:0, (pin/8)?9:1, type);  
#define gpio_pin_it_config(pin, type, status)                         \
    do{                                                             \
		WS_WRITE_FIELD2(GPIO_BASE, 0x80, (pin/8)*8, (pin/8)*8+1 , type); \
        WS_WRITE_FIELD2(GPIO_BASE, 0x84, pin, pin, status);         \
    }while(0)

#define gpio_get_it_status(pin)         WS_READ_FIELD2(GPIO_BASE, 0x88, pin, pin)    

#define gpio_clear_it_status(pin)        WS_WRITE_FIELD2(GPIO_BASE, 0x88, pin, pin, 1)
	
//void user_gpio_init(void);

#endif
