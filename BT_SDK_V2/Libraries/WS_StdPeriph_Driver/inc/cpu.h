/**
******************************************************************************************************************************************************************************************
*@file                       cpu.h
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

#ifndef CPU_H_
#define CPU_H_

#include "cpu_regs.h"
#include "cpu_reg_util.h"


/* HCLK分频系数：0不分频；1二分频；2四分频; */
#define HCLK_DIV                        0
/* PCLK分频系数：0不分频；1二分频；2四分频; */
#define PCLK_DIV                        0


#define Sys_Parameters  ((struct calib_val *)0x80U)

typedef unsigned char         uint8_t;
typedef unsigned short        uint16_t;
typedef unsigned int        uint32_t;

extern struct {
    uint8_t    rsvd[14];
    uint8_t    is_xtal_32m;
} chip_state;

typedef enum IRQn
{
/* -------------------  Cortex-M0 Processor Exceptions Numbers  ------------------- */
    NMI_IRQn                 = -14,      /*  2 Non Maskable Interrupt */
    HardFault_IRQn         = -13,      /*  3 HardFault Interrupt */



    SVCall_IRQn            =  -5,      /* 11 SV Call Interrupt */

    PendSV_IRQn            =  -2,      /* 14 Pend SV Interrupt */
    SysTick_IRQn           =  -1,      /* 15 System Tick Interrupt */

/* ----------------------  ARMCM0 Specific Interrupt Numbers  --------------------- */
    UART_IRQn           = 0,
    R24G_IRQn           = 1,  
    QDEC_IRQn           = 2,
    Timer_IRQn             = 3,
    BSTIM_IRQn            = 4,  
    RTC_IRQn            = 5,
    GPADC_IRQn          = 6,
    QSPI_IRQn           = 7,
    GPIO_INT_IRQn          = 8,
    USB_Low_Priority_IRQn       = 9,
    USB_Hight_Priority_IRQn     = 10,
    USB_Wake_UP_IRQn            = 11,
    RESERVE4            = 12,
    SPI_IRQn            = 13,
    
} IRQn_Type;

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* -------  Start of section using anonymous unions and disabling warnings  ------- */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif

/* --------  Configuration of the Cortex-M0 Processor and Core Peripherals  ------- */
#define __CM0_REV                 0x0000U   /* Core revision r0p0 */
#define __MPU_PRESENT             0         /* MPU present or not */
#define __VTOR_PRESENT            0         /* no VTOR present*/
#define __NVIC_PRIO_BITS          2         /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0         /* Set to 1 if different SysTick Config is used */


#include "core.h"

typedef enum
{
    DISABLE = 0,
    ENABLE = !DISABLE,
} Status;

typedef enum
{
    false = 0, true = !false,
}bool;

typedef enum
{
    low  = 0,
    high = 1,
}level;


#define BIT0        (0x00000001U)
#define BIT1        (0x00000002U)
#define BIT2        (0x00000004U)
#define BIT3        (0x00000008U)
#define BIT4        (0x00000010U)
#define BIT5        (0x00000020U)
#define BIT6        (0x00000040U)
#define BIT7        (0x00000080U)
#define BIT8        (0x00000100U)
#define BIT9        (0x00000200U)
#define BIT10       (0x00000400U)
#define BIT11       (0x00000800U)
#define BIT12       (0x00001000U)
#define BIT13       (0x00002000U)
#define BIT14       (0x00004000U)
#define BIT15       (0x00008000U)
#define BIT16       (0x00010000U)
#define BIT17       (0x00020000U)
#define BIT18       (0x00040000U)
#define BIT19       (0x00080000U)
#define BIT20       (0x00100000U)
#define BIT21       (0x00200000U)
#define BIT22       (0x00400000U)
#define BIT23       (0x00800000U)
#define BIT24       (0x01000000U)
#define BIT25       (0x02000000U)
#define BIT26       (0x04000000U)
#define BIT27       (0x08000000U)
#define BIT28       (0x10000000U)
#define BIT29       (0x20000000U)
#define BIT30       (0x40000000U)
#define BIT31       (0x80000000U)

struct calib_val
{
    uint32_t pll_vc_tx[10];
    uint32_t pll_vc_rx[10];
    uint8_t lpf;
    uint8_t dev_ib;
    uint16_t TEMP;
    uint8_t rsvd1[4];
    int8_t  gpadc_offset;
    uint8_t bgr_ctl_tmp_rang;
    uint8_t rsvd[22];
    uint8_t die;
    uint8_t flash_e2_type;
    uint8_t boot_otp_version;
    uint8_t user_manufacturer_id;
    uint32_t sernumber_macaddr;
    uint8_t param_vaild;
    uint8_t otp_size;
    uint8_t ram_size;
    uint8_t radio_fun;
    uint32_t flash_size;
};


/* 使能内部32K */
#define rc32_en(val)                        WS_WRITE_FIELD(SYSCFG_BASE, SYSCFG_F_RC32K_EN, val)

/* CRM门控使能和失能 */
#define cpu_crm_set(...)                    WS_WRITE_FIELD3(CRM_BASE, WS_FIELDS_OFFSET(__VA_ARGS__), WS_FIELDS_MASK(__VA_ARGS__), WS_FIELDS_VAL(__VA_ARGS__))

/* 对外设进行Reset */
#define cpu_crm_reset(...)                  WS_WRITE_FIELD3(CRM_BASE, WS_FIELDS_OFFSET(__VA_ARGS__), WS_FIELDS_MASK(__VA_ARGS__), WS_FIELDS_VAL(__VA_ARGS__))

/* 外部晶体振荡器高速时钟的使能和失能 -- ENABLE/DISABLE*/
#define cpu_external_hclock_en(val)            WS_WRITE_FIELD(SYSCFG_BASE, SYSCFG_F_XTAL16M_EN, val)    

/* 接口声明 */
#define cpu_sys_init()                                                    \
    do{                                                                    \
        cpu_external_hclock_en(ENABLE);                                    \
        WS_WRITE_FIELDS(SYSCFG_BASE, SYSCFG_F_HCLK_DIV_VAL, HCLK_DIV, SYSCFG_F_PCLK_DIV_VAL, PCLK_DIV);    \
    }while(0)


#define sleep_core_external_ctl()                WS_WRITE_REG(0xE000ED10, 0x14)

#define SLEEP_TYPE_01            1
#define SLEEP_TYPE_00            0
	
/*
 * sleeptype ->>
 * SLEEP_TYPE_00 / SLEEP_TYPE_01 / SLEEP_TYPE_10
 */
#define sleep_type_ctr(sleeptype)        WS_WRITE_FIELD(SYSCFG_BASE, SYSCFG_F_DEEP_SLP_SEL, sleeptype)

/*
 * wk_src ->>
 * SYSCFG_F_JTAG_WK_EN : 唤醒源JTAG使能
 * SYSCFG_F_GPIO_WK_EN : 唤醒源GPIO使能
 * SYSCFG_F_RTC_WK_EN  : 唤醒源RTC使能
 */
#define sleep_wk_src_en(wk_src, val)    WS_WRITE_FIELD2(SYSCFG_BASE, wk_src, val)


// jtag TMS信号唤醒电平 low/high
#define jtag_wk_level(le)                    WS_WRITE_FIELD(SYSCFG_BASE, SYSCFG_F_JTAG_WKUP_TYPE, le)

#define GPIO_WK_NOWK        0
#define GPIO_WK_HIGH        1
#define GPIO_WK_IG_HIGH        2
#define GPIO_WK_LOW            3
#define GPIO_WK_IG_LOW        4
#define GPIO_WK_RE            5
#define GPIO_WK_FE            6
#define GPIO_WK_DE            7
/*
 * pin: gpio_pin_0 -- gpio_pin_15
 * type: GPIO_WK_XX
 */
#define gpio_wk_src_type(pin, type)            WS_WRITE_FIELD2(SYSCFG_BASE, 0x04*(pin/8)+0x24, (pin%8)*4, (pin%8)*4+2, type)

/* FIALD ->> 
 * SYSCFG_F_ALL_WK_EN          所有Exception，External Interrupt都能唤醒MCU                                
 * SYSCFG_F_QDEC_WK_MCU        QDEC中断唤醒MCU使能                    
 * SYSCFG_F_GPIO_INT0_WK_MCU   GPIO中断0唤醒MCU使能              
 * SYSCFG_F_RTC_WK_MCU         RTC中断唤醒MCU使能                
 * SYSCFG_F_TIMER1_WK_MCU      Timer1中断唤醒MCU使能              
 * SYSCFG_F_TIMER0_WK_MCU      Timer0中断唤醒MCU使能                
 * SYSCFG_F_SPI0_WK_MCU        SPI0中断唤醒MCU使能                    
 * SYSCFG_F_UART0_WK_MCU       UART0中断唤醒MCU使能
 * 
 * val -- ENABLE/DISABLE
 */
#define mcu_wk_src_ctr(...)                 WS_WRITE_FIELD3(SYSCFG_BASE, WS_FIELDS_OFFSET(__VA_ARGS__), WS_FIELDS_MASK(__VA_ARGS__), WS_FIELDS_VAL(__VA_ARGS__))

//val -- ENABLE/DISABLE
#define sleep_sample_en(val)                WS_WRITE_FIELD(SYSCFG_BASE, SYSCFG_F_BSTIM_SAMPLE_EN, val)

static inline void hw_write_reg_redio(uint16_t addr, uint16_t data) 
{
    if((addr >> 8) <= 3)                                                    
        WS_WRITE_FIELD2(0x30003000, 0x2C, 0, 1, (addr >> 8));                
    while(WS_READ_FIELD(RFSPI_BASE, RFSPI_F_SW_CMD0_CMD_VLD));                
    WS_WRITE_FIELD(RFSPI_BASE, RFSPI_R_SW_CMD0,                             
        (1 << 24) + ((addr & 0xff)<< 16) + data);                            
    while(WS_READ_FIELD(RFSPI_BASE, RFSPI_F_SW_CMD0_CMD_VLD));                
    WS_WRITE_FIELD2(0x30003000, 0x2C, 0, 1, 1);                                
}

static inline uint16_t hw_read_reg_redio(uint16_t addr)
{
    uint16_t dat;
    uint32_t val;
    uint8_t page = addr >> 8;

    if(page <= 3)
        WS_WRITE_FIELD2(0x30003000, 0x2C, 0, 1, page);
    while(WS_READ_FIELD(RFSPI_BASE, RFSPI_F_SW_CMD0_CMD_VLD));
    val = (1 << 24) + (1 << 23) + ((addr &0xFF) << 16);
    WS_WRITE_FIELD(RFSPI_BASE, RFSPI_R_SW_CMD0, val);
    while(WS_READ_FIELD(RFSPI_BASE, RFSPI_F_SW_CMD0_CMD_VLD));

    dat = WS_READ_FIELD(RFSPI_BASE, RFSPI_F_SW_CMD0_SPI_RWDATA);
    WS_WRITE_FIELD2(0x30003000, 0x2C, 0, 1, 1);

    return dat;
}

void rf_init_0(void);
void rf_init_1(void);
void chip_init(void);
void otp_enter_sleep(void);
bool deepsleep_enter(void);
void SystemInit(void);
void restore_reg(void);
void set_xtal_cap(uint8_t val);
uint8_t get_xtal_cap(void);
void temp_cal_fd(uint16_t adc_val);

/* --------  End of section using anonymous unions and disabling warnings  -------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif

#ifdef  USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *   which reports the name of the source file and the source
  *   line number of the call that failed. 
  *   If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) //((void)0)
#endif /* USE_FULL_ASSERT */

#define SF_SUPPORT_ASSERT
#if defined(SF_SUPPORT_ASSERT)
#define ASSERT_INFO(cond, param0, param1)             \
    do {                                              \
        if (!(cond)) {                                \
            CPU_Assert((int)param0, (int)param1, __MODULE__, __LINE__);  \
        }                                             \
    } while(0)


#else        
#define ASSERT_INFO(cond, param0, param1)         
#endif //PLF_DEBUG

#ifndef force_inline

#if defined(__ARMCC_VERSION)
#define force_inline __forceinline
#elif defined(__GNUC__)
#define force_inline __attribute__((always_inline))
#else
#if defined(_MSC_VER)
#define force_inline __inline
#else
#define force_inline inline
#endif
#endif

#endif

/* 是否支持中断安全 */
#define INTERRUPT_SECURITY 1

#if INTERRUPT_SECURITY
static force_inline int irq_lock(void)
{
#if defined(__GNUC__)
    int key;

    __asm__ volatile("mrs %0, PRIMASK;"
        "cpsid i"
		    : "=r" (key)
		    :
		    : "memory");
	
		return key;
#elif defined(__ARMCC_VERSION)
    int key;

    __asm volatile("mrs key, PRIMASK");
    __asm volatile("cpsid i");
	
    return key;
#else
    #error "Please implement the current compiler mask interrupt function"
#endif
}

static force_inline void irq_unlock(int key)
{
    if (key) {
        return;
    }

#if defined(__GNUC__)
    __asm__ volatile(
        "cpsie i;"
        "isb;"
        : : : "memory");	
#elif defined(__ARMCC_VERSION)
    __asm volatile("cpsie i");
    __asm volatile("isb");
#else
    #error "Please implement the current compiler mask interrupt function"
#endif
}

#else

static force_inline int irq_lock(void)
{
    return 0;
}

static force_inline void irq_unlock(int key)
{
}

#endif /* INTERRUPT_SECURITY */


#endif
