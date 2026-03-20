;/**************************************************************************//**
; * @file     startup_<Device>.s
; * @brief    CMSIS Core Device Startup File for
; *           Device <Device>
; * @version  V3.10
; * @date     23. November 2012
; *
; * @note
; *
; ******************************************************************************/
;/* Copyright (c) 2012 ARM LIMITED
;
;   All rights reserved.
;   Redistribution and use in source and binary forms, with or without
;   modification, are permitted provided that the following conditions are met:
;   - Redistributions of source code must retain the above copyright
;     notice, this list of conditions and the following disclaimer.
;   - Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the
;     documentation and/or other materials provided with the distribution.
;   - Neither the name of ARM nor the names of its contributors may be used
;     to endorse or promote products derived from this software without
;     specific prior written permission.
;   *
;   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
;   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
;   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
;   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
;   POSSIBILITY OF SUCH DAMAGE.
;   ---------------------------------------------------------------------------*/
;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

                ;芯片状态信息
                ;AREA    CHIPSTATE, NOINIT, DATA, READWRITE, ALIGN=3
chip_state      EQU     0x20000000
                EXPORT  chip_state
                ;int     control
                ;int     msp
                ;int     psp
                ;short   valid
                ;char    power_flag
                ;char    is_xtal_32m

                ;RAM中断向量表
                ;AREA    RAM_VECTORS, NOINIT, DATA, READWRITE, ALIGN=7
;ram_vectors     EQU     0x20000000

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
				IMPORT  BB_FAST_IRQHandler

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     HardFault_Handler         ; MPU Fault Handler
                DCD     HardFault_Handler         ; Bus Fault Handler
                DCD     HardFault_Handler         ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0               ; SVCall Handler
                DCD     0          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
				DCD     UART0_IRQHandler
				DCD     BB_FAST_IRQHandler
				DCD     QDEC_IRQHandler
				DCD     TIMER_IRQHandler
				DCD     BSTIM_IRQHandler
				DCD     RTC_IRQHandler
				DCD     ADC_IRQHandler
				DCD     0
				DCD     GPIO_IRQHandler
				DCD     USB_Low_Priority_IRQHandler
				DCD     USB_Hight_Priority_IRQHandler
				DCD     USB_Wake_UP_IRQHandler
				DCD     0
				DCD     SPI0_IRQHandler
				DCD     0
				DCD     RC32K_IRQHandler

__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]				
                IMPORT  __main
                EXPORT const_chip_state
                IMPORT SystemInit

                ;加载RAM向量表地址
                LDR     R6, const_vectors
				
                ;设置向量表为RAM地址
				LDR     R1, const_syscfg_reg
				STR     R6, [R1, #0x08]
                ;MOVS    R2, #0
                ;STR     R2, [R1, #0x04]

                ;读取上电标识
                LDR     R2, [R1, #0x14]
                ;清除上电标识
                ;STR     R2, [R1, #0x14]

                ;将上电标识保存至系统变量中
                LDR     R5, const_chip_state
                ;STRB    R2, [R5, #14]

                ;读取软件休眠标识
                LDRH    R3, [R5, #12]

                ;仅软件休眠标识与硬件休眠唤醒标识同时置位时才进行休眠恢复流程
                ADD     R3, R2
                CMP     R3, #0xC8

                ;执行休眠唤醒流程
                BEQ     __wakeup

                ;读取向量表地址
				LDR     R0, const_vectors

                ;读取时钟寄存器
                SUBS    R1, #0xe0
                LDR     R3, [R1]

                ;判断是否为上电复位
                LSLS    R2, #31
                BPL     XTAL_RESTORE

                ;读取XTAL主频保存至系统变量中
                LSLS    R3, #30
                LSRS    R3, #31
                STRB    R3, [R5, #14]
                B       XTAL_RESTORE_END

                ;优化CACHE加载地址
;const_ram_vectors
                ;DCD     ram_vectors
const_syscfg_reg
                DCD     0x500050e0
const_chip_state
                DCD     chip_state
const_vectors
                DCD     __Vectors

XTAL_RESTORE
                ;恢复XTAL设置
                LDRB    R4, [R5, #14]
                CMP     R4, #0
                BEQ     XTAL_RESTORE_END
                MOVS    R2, #2
                ORRS    R3, R2
                STR     R3, [R1]

XTAL_RESTORE_END
                ;拷贝向量表至RAM
                ;MOVS    R3, R6
                ;ADDS    R3, #0x80
;COPY_LOOP
                ;LDM     R0!, {R1, R2, R4, R5}
                ;STM     R6!, {R1, R2, R4, R5}
                ;CMP     R6, R3
                ;BNE     COPY_LOOP

                ;延时2S启动
                ;LDR     R1, =4000000
                ;SUBS    R1, #1
                ;BNE     STARUP_DELAY

                ;正常跳转至main函数
                ;SUBS    R0, #0x80
                ;LDR     R0, [R0]
                MOV     SP, R0
				LDR		R0, =SystemInit
				BLX		R0
                LDR     R0, const__main
                BX      R0
                NOP
const__main
                DCD     __main
                ENDP

;进入休眠函数
sleep_enter 	PROC
                EXPORT  sleep_enter

                PUSH    {R4-R7, LR}

                MOV     R0, R8
                MOV     R1, R9
                MOV     R2, R10
                MOV     R3, R11
                MOV     R4, R12

                PUSH    {R0-R4}
                LDR     R5, const1_chip_state
                MRS     R0, CONTROL
                MRS     R1, MSP
                MRS     R2, PSP
                MOVS    R3, #0xC0

                STM     R5!, {R0-R2}
                STRH    R3, [R5]
                ;进入休眠
                WFI
                SUBS    R5, #12
                B       __wakeup
const1_chip_state
                DCD     chip_state

__wakeup        ;唤醒位置
                LDM     R5!, {R0-R2}

                MSR     CONTROL, R0
                MSR     MSP, R1
                MSR     PSP, R2
                ISB

                POP     {R0-R4}

                MOV     R12,  R4
                MOV     R11,  R3
                MOV     R10,  R2
                MOV     R9,   R1
                MOV     R8,   R0

                POP     {R4-R7, PC}
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
SysTick_Handler\
                PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP
PendSV_Handler\
                PROC
                EXPORT  PendSV_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC
; ToDo:  Add here the export definition for the device specific external interrupts handler
				EXPORT		NMI_Handler				[WEAK]
				EXPORT		UART0_IRQHandler		[WEAK]
				EXPORT		GPIO_IRQHandler			[WEAK]
				EXPORT		BSTIM_IRQHandler		[WEAK]
				EXPORT		TIMER_IRQHandler		[WEAK]
				EXPORT		BB_IRQHandler			[WEAK]
				EXPORT		RTC_IRQHandler			[WEAK]
				EXPORT		RC32K_IRQHandler		[WEAK]
				EXPORT		ADC_IRQHandler			[WEAK]
				EXPORT      SPI0_IRQHandler         [WEAK]
				EXPORT      QDEC_IRQHandler         [WEAK]
				EXPORT		USB_Low_Priority_IRQHandler	  [WEAK]
				EXPORT      USB_Hight_Priority_IRQHandler [WEAK]
				EXPORT      USB_Wake_UP_IRQHandler        [WEAK]				
; ToDo:  Add here the names for the device specific external interrupts handler
UART0_IRQHandler
GPIO_IRQHandler
QDEC_IRQHandler
TIMER_IRQHandler
BSTIM_IRQHandler
BB_IRQHandler
RTC_IRQHandler
ADC_IRQHandler
SPI0_IRQHandler
RC32K_IRQHandler
USB_Low_Priority_IRQHandler                           
USB_Hight_Priority_IRQHandler                         
USB_Wake_UP_IRQHandler


                B       .
                ENDP


                ALIGN

                EXPORT  __initial_sp

                END
