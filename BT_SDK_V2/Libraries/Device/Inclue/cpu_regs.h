#include "cpu_reg_util.h"

/* 外设基地址定义 */
#define BB_BASE                                                 0x30003000
#define BSTIM_BASE                                              0x50007000
#define CACHE_BASE                                              0x30001000
#define CRM_BASE                                                0x30000000
#define GPADC_BASE                                              0x5000A000
#define GPIO_BASE                                               0x50009000
#define OTP_BASE                                                0x30004000
#define QDEC_BASE                                               0x50004000
#define RFSPI_BASE                                              0x5000B000
#define RTC_BASE                                                0x50008000
#define SPIM_BASE                                               0x50002000
#define SYSCFG_BASE                                             0x50005000
#define TIMER_BASE                                              0x50006000
#define UART_BASE                                               0x50001000
#define WDT_BASE                                                0x50003000


/*
 * BB registers and fields
 */
/* Modem配置寄存器  */
#define BB_R_CFG00                                              WS_REG(0x0000)           /* RW 初始值：00E00003 */
/* 0-crc 错误或者接收包长错误不退出接收状态
1-crc 错误或者接收包长错误不接收状态，状态进入IDLE，需要软件重新配置进入RX或者TX->RX
rx_always模式下配置无效 */
#define BB_F_ERR_EXIT                                           WS_FIELD(0x0000, 23, 23) /* RW 初始值：0 */
/* 中断清除选择
0-软件读取BB2P4GCFG22寄存器清除中断（中断读清）
1-软件对BB2P4GCFG222写1寄存器清除中断（相应bit写1清除中断） */
#define BB_F_INTP_CLR_SEL                                       WS_FIELD(0x0000, 22, 22) /* RW 初始值：0 */
/* 1-软件复位fsm，复位后所有状态机进入idle状态
（当接收超时功能关闭时可以配置该寄存器退出接收）
写完自动清零 */
#define BB_F_RST_FSM                                            WS_FIELD(0x0000, 18, 18) /* WC 初始值：0 */
/* 1- 2.4g处于接收一包发送状态，写完自动清零 */
#define BB_F_TX_VLD_FLAG                                        WS_FIELD(0x0000, 17, 17) /* WC 初始值：0 */
/* 1- 2.4g处于接收一包状态，写完自动清零 */
#define BB_F_RX_VLD_FLAG                                        WS_FIELD(0x0000, 16, 16) /* WC 初始值：0 */

/* TX RF DELAY 控制寄存器  */
#define BB_R_CFG01                                              WS_REG(0x0004)           /* RW 初始值：00000000 */
/* baseband开始退出发送状态后，rf delay一定时间后退出发送状态 */
#define BB_F_POST_RF_TX_CNT                                     WS_FIELD(0x0004, 16, 31) /* RW 初始值：0 */
/* nn */
#define BB_F_PRE_RF_TX_CNT                                      WS_FIELD(0x0004,  0, 15) /* RW 初始值：0 */

/* RX RF DELAY 控制寄存器  */
#define BB_R_CFG02                                              WS_REG(0x0008)           /* RW 初始值：00000000 */
/* baseband开始退出接收状态后，rf delay一定时间后退出接收状态 */
#define BB_F_POST_RF_RX_CNT                                     WS_FIELD(0x0008, 16, 31) /* RW 初始值：0 */
/* 进入接收状态后,打开rf delay一定时间后，baseband开始进入接收状态 */
#define BB_F_PRE_RF_RX_CNT                                      WS_FIELD(0x0008,  0, 15) /* RW 初始值：0 */

/* preamble控制寄存器  */
#define BB_R_CFG03                                              WS_REG(0x000C)           /* RW 初始值：000C0006 */
/* 1-4byte, 0-1byte ,目前固定为4byte */
#define BB_F_TX_ID_LENGTH                                       WS_FIELD(0x000C, 18, 19) /* RW 初始值：3 */
/* 1-发送的包不包含id code */
#define BB_F_TX_MV_ID_CODE_FLAG                                 WS_FIELD(0x000C, 17, 17) /* RW 初始值：0 */
/* 0-1byte 3-4byte */
#define BB_F_PREAMBLE_LENGTH                                    WS_FIELD(0x000C,  1,  2) /* RW 初始值：3 */
/* 1.发送的包不包含preamble */
#define BB_F_MV_PREAMBLE_FLAG                                   WS_FIELD(0x000C,  0,  0) /* RW 初始值：0 */

/* RX ID CODE控制寄存器  */
#define BB_R_CFG04                                              WS_REG(0x0010)           /* RW 初始值：00000000 */
/* rx_sync_word */
#define BB_F_RX_SYNC_WORD                                       WS_FIELD(0x0010,  0, 31) /* RW 初始值：0 */

/* 白化控制寄存器  */
#define BB_R_CFG05                                              WS_REG(0x0014)           /* RW 初始值：0000000F */
/* tx状态下产生HW SPI PA延迟开启的时间 */
#define BB_F_DLY2PA_CNT_CFG                                     WS_FIELD(0x0014, 16, 31) /* RO 初始值：0 */
/* 1-pa开启后逐渐增大功能开启
0-pa开启后逐渐增大功能关闭
说明-在延迟dly2pa_cnt_cfg时间后开启PA，每1us +1直到和power值一致 */
#define BB_F_PA_ADDER_EN                                        WS_FIELD(0x0014, 15, 15) /* RO 初始值：0 */
/* 白化的pn7初始值 */
#define BB_F_WHITENING_INIT_PN7                                 WS_FIELD(0x0014,  4, 10) /* RW 初始值：0 */
/* 1-crc不加白化 */
#define BB_F_WHITENING_MV_CRC                                   WS_FIELD(0x0014,  3,  3) /* RW 初始值：1 */
/* 1-payload不加白化 */
#define BB_F_WHITENING_MV_PAYLOAD                               WS_FIELD(0x0014,  2,  2) /* RW 初始值：1 */
/* 1-sign不加白化 */
#define BB_F_WHITENING_MV_SIGN                                  WS_FIELD(0x0014,  1,  1) /* RW 初始值：1 */
/* 1-ID不加白化 */
#define BB_F_WHITENING_MV_ID                                    WS_FIELD(0x0014,  0,  0) /* RW 初始值：1 */
#define BB_F_WHITENING_CHECK                                    WS_FIELD(0x0014,  0,  3)

/* 模式选择  */
#define BB_R_CFG06                                              WS_REG(0x0018)           /* RW 初始值：000FE0D0 */
/* 0-pid 1bit
1-pid 2bit */
#define BB_F_PID_SEL                                            WS_FIELD(0x0018,  9,  9) /* RW 初始值：1 */
/* 1-使能动态payload长度（决定接收）
0-关闭动态包长功能 */
#define BB_F_EN_DPL                                             WS_FIELD(0x0018,  8,  8) /* RW 初始值：0 */
/* 数据长度选择（只对动态包长有影响，改变数据长度标志长度6/7bit）
11-64byte模式(7bit)
00-32byte模式(6bit)
蓝牙模式下无效，固定6bit */
#define BB_F_DATA_LEN_SEL                                       WS_FIELD(0x0018,  6,  7) /* RW 初始值：3 */
/* 1-payload 发射和接收以msb方式
0-payload 发射和接收以lsb方式 */
#define BB_F_PAY_MSB_EN                                         WS_FIELD(0x0018,  5,  5) /* RW 初始值：0 */
/* 1-发送和接收机自动应答机制关闭
0-发送和接收机自动应答机制开启
不自动应答（但是可以发送增强帧） */
#define BB_F_EN_NOACK                                           WS_FIELD(0x0018,  4,  4) /* RW 初始值：1 */
/* 收发速率选择
0--1M bit/s
1--2M bit/s
2--250K bit/s
3--无效 */
#define BB_F_DATA_RATE                                          WS_FIELD(0x0018,  2,  3) /* RW 初始值：0 */

/* TX ID CODE 值  */
#define BB_R_CFG07                                              WS_REG(0x001C)           /* RW 初始值：00000000 */
/* tx_ID code */
#define BB_F_TX_SYNC_WORD                                       WS_FIELD(0x001C,  0, 31) /* RW 初始值：0 */

/* tx & ack PAYLOAD 控制寄存器  */
#define BB_R_CFG08                                              WS_REG(0x0020)           /* RW 初始值：00000000 */
/* ack包的payload长度 */
#define BB_F_ACK_PAYLOAD_LENGTH                                 WS_FIELD(0x0020, 16, 23) /* RW 初始值：0 */

/* tx_payload_len
0-255byte（普通模式）
0-64byte（增强模式）*/
#define BB_F_TX_PAYLOAD_LEN                                     WS_FIELD(0x0020,  0,  7) /* RW 初始值：0 */

/* RX PAYLOAD 控制寄存器  */
#define BB_R_CFG09                                              WS_REG(0x0024)           /* RW 初始值：00000000 */
/* 固定包长下使用-
0-255byte（普通模式）
0-64byte（增强模式）
data pipe0 中rx_payload的数据长度
在多通道模式下只支持所有通道都使用固定包长
想要不同通道使用不同包长需要使用动态包长模式
 */
#define BB_F_RX_PAYLOAD_LENGTH_PIPE0                            WS_FIELD(0x0024,  0,  7) /* RW 初始值：0 */

/* CRC 控制寄存器  */
#define BB_R_CFG10                                              WS_REG(0x0028)           /* RW 初始值：00000000 */
/* 1-crc 输出取反
0-crc 输出不取反*/
#define BB_F_CRC_OUT_INV                                        WS_FIELD(0x0028, 31, 31) /* RW 初始值：0 */
/* 1-CRC 8005 CCITT-16 CRC(X16+X15+X2+1)
0-CRC 1021 x16+x12+x5+1 */
#define BB_F_CRC_SEL                                            WS_FIELD(0x0028, 30, 30) /* RW 初始值：0 */
/* 1-计算CRC不加入payload */
#define BB_F_CRC_MV_PAYLOAD                                     WS_FIELD(0x0028, 28, 28) /* RW 初始值：0 */
/* 1-计算CRC不加入SIGN code */
#define BB_F_CRC_MV_SIGN                                        WS_FIELD(0x0028, 27, 27) /* RW 初始值：0 */
/* 1-计算CRC不加入ID code */
#define BB_F_CRC_MV_ID                                          WS_FIELD(0x0028, 26, 26) /* RW 初始值：0 */
#define BB_F_CRC_CHECK                                          WS_FIELD(0x0028, 26, 28)
/* 1-关闭CRC功能
0-开启CRC功能 */
#define BB_F_MV_CRC_FLAG                                        WS_FIELD(0x0028, 24, 24) /* RW 初始值：0 */
/* crc 初始值 低16bit */
#define BB_F_CRC_INIT_DATA                                      WS_FIELD(0x0028,  0, 23) /* RW 初始值：0 */

/* 配置寄存器页选择  */
#define BB_R_CFG11                                              WS_REG(0x002C)           /* RW 初始值：00000000 */
/* reg page sel
0-无效
1-HW SPI 配置 RF1 页
2-HW SPI 配置 RF2 页
3-HW SPI 配置 MODEM页 */
#define BB_F_PAGE_SEL                                           WS_FIELD(0x002C,  0,  1) /* RW 初始值：0 */

/* txrx配置  */
#define BB_R_CFG12                                              WS_REG(0x0030)           /* RW 初始值：00000001 */
/* 发射自动转到接收状态切换时间 */
#define BB_F_RX2TX_DLY                                          WS_FIELD(0x0030, 16, 31) /* RW 初始值：0 */
/* 接收自动转到发射状态切换时间 */
#define BB_F_TX2RX_DLY                                          WS_FIELD(0x0030,  0, 15) /* RW 初始值：0 */

/* INT控制寄存器  */
#define BB_R_CFG13                                              WS_REG(0x0034)           /* RW 初始值：00000EFF */
/* 接收超过最大长度中断MASK
1-MASK
0-NOT MASK */
#define BB_F_RX_LEN_ERR_MSK                                     WS_FIELD(0x0034, 11, 11) /* RW 初始值：1 */
/* 接收超时中断是否MASK
1-MASK
0-NOT MASK */
#define BB_F_RX_TIMEOUT_MSK                                     WS_FIELD(0x0034, 10, 10) /* RW 初始值：1 */
#define BB_F_MAX_RT_MSK                                         WS_FIELD(0x0034,  9,  9) /* RW 初始值：1 */
#define BB_F_FIFO_ERR_INT_MASK                                  WS_FIELD(0x0034,  7,  7) /* RW 初始值：1 */
#define BB_F_RX_FIFO_AF_INT_MSK                                 WS_FIELD(0x0034,  6,  6) /* RW 初始值：1 */
#define BB_F_TX_FIFO_AF_INT_MSK                                 WS_FIELD(0x0034,  5,  5) /* RW 初始值：1 */
#define BB_F_TX_FIFO_AE_INT_MSK                                 WS_FIELD(0x0034,  4,  4) /* RW 初始值：1 */
#define BB_F_RX_DONE_INT_MSK                                    WS_FIELD(0x0034,  3,  3) /* RW 初始值：1 */
#define BB_F_TX_DONE_INT_MSK                                    WS_FIELD(0x0034,  2,  2) /* RW 初始值：1 */
#define BB_F_CRC_ERROR_INT_MSK                                  WS_FIELD(0x0034,  1,  1) /* RW 初始值：1 */
/* mask id match int */
#define BB_F_ID_MATCH_INT_MSK                                   WS_FIELD(0x0034,  0,  0) /* RW 初始值：1 */

/* PIPE0 ADDR  */
#define BB_R_CFG15                                              WS_REG(0x003C)           /* RW 初始值：F1D2E6A2 */
#define BB_F_PIPE0_BYTE4                                        WS_FIELD(0x003C, 24, 31) /* RW 初始值：0xF1 */
#define BB_F_PIPE0_BYTE3                                        WS_FIELD(0x003C, 16, 23) /* RW 初始值：0xD2 */
#define BB_F_PIPE0_BYTE2                                        WS_FIELD(0x003C,  8, 15) /* RW 初始值：0xE6 */
#define BB_F_PIPE0_BYTE1                                        WS_FIELD(0x003C,  0,  7) /* RW 初始值：0xA2 */

/* PIPE3_0_BYTE0  */
#define BB_R_CFG16                                              WS_REG(0x0040)           /* RW 初始值：D5D4D333 */
#define BB_F_PIPE3_BYTE0                                        WS_FIELD(0x0040, 24, 31) /* RW 初始值：0xD5 */
#define BB_F_PIPE2_BYTE0                                        WS_FIELD(0x0040, 16, 23) /* RW 初始值：0xD4 */
#define BB_F_PIPE1_BYTE0                                        WS_FIELD(0x0040,  8, 15) /* RW 初始值：0xD3 */
#define BB_F_PIPE0_BYTE0                                        WS_FIELD(0x0040,  0,  7) /* RW 初始值：0x33 */

/* PIPE54_BYTE0  */
#define BB_R_CFG17                                              WS_REG(0x0044)           /* RW 初始值：0000D7D6 */
/* 0-2byte
1-3byte
2-4byte
3-5byte */
#define BB_F_PIPE_HEADER_LENGTH                                 WS_FIELD(0x0044, 22, 23) /* RW 初始值：0 */
/* 配置发射时只能1bit（一个通道）有效
配置接收时可以多通道有效
000000-多通道模式关闭
000001-通道0使能
000010-通道1使能
100010-通道1,5使能 */
#define BB_F_PIPE_EN                                            WS_FIELD(0x0044, 16, 21) /* RW 初始值：0 */
#define BB_F_PIPE5_BYTE0                                        WS_FIELD(0x0044,  8, 15) /* RW 初始值：0xD7 */
#define BB_F_PIPE4_BYTE0                                        WS_FIELD(0x0044,  0,  7) /* RW 初始值：0xD6 */

/* PIPE1_5_ADDR  */
#define BB_R_CFG18                                              WS_REG(0x0048)           /* RO 初始值：D3D3D3D3 */
#define BB_F_PIPE1_5_BYTE4                                      WS_FIELD(0x0048, 24, 31) /* RW 初始值：0xD3 */
#define BB_F_PIPE1_5_BYTE3                                      WS_FIELD(0x0048, 16, 23) /* RW 初始值：0xD3 */
#define BB_F_PIPE1_5_BYTE2                                      WS_FIELD(0x0048,  8, 15) /* RW 初始值：0xD3 */
#define BB_F_PIPE1_5_BYTE1                                      WS_FIELD(0x0048,  0,  7) /* RW 初始值：0xD3 */

/* 2p4g_ext_tx_header  */
#define BB_R_CFG19                                              WS_REG(0x004C)           /* RO 初始值：00000001 */
#define BB_F_TX_EXT_HEADER_32_39                                WS_FIELD(0x004C, 16, 23) /* RW 初始值：6 */

/* 2p4g_ext_tx_header  */
#define BB_R_CFG20                                              WS_REG(0x0050)           /* RW 初始值：00000000 */
/*tx_ext_header[31-16] 2.4G多通道模式下使用 */
#define BB_F_TX_EXT_HEADER_16_31                                WS_FIELD(0x0050, 16, 31) /* RW 初始值：0 */
/*tx_ext_header[15-0] 2.4G多通道模式下使用 */
#define BB_F_TX_EXT_HEADER_0_15                                 WS_FIELD(0x0050,  0, 15) /* RW 初始值：0 */

#define BB_R_CFG21                                              WS_REG(0x0054)           /* RW 初始值：00000000 */

/* OBSERVE_STATE  */
#define BB_R_CFG22                                              WS_REG(0x0058)           /* RW 初始值：00000000 */
/* 接收超过最大长度中断 */
#define BB_F_RX_LEN_ERR_INT                                     WS_FIELD(0x0058, 27, 27) /* RC 初始值：0 */
/* 接收超时中断 */
#define BB_F_RX_TIMEOUT                                         WS_FIELD(0x0058, 26, 26) /* RC 初始值：0 */
/* 超过最大重传次数中断 */
#define BB_F_MAX_RT_INT                                         WS_FIELD(0x0058, 25, 25) /* RC 初始值：0 */
/* 发射 fifo下溢出 或者 接收 fifo上溢出 */
#define BB_F_TFU_RFO_FLOW_INT                                   WS_FIELD(0x0058, 24, 24) /* RC 初始值：0 */
/* 发射 fifo上溢出 或者 接收 fifo下溢出 */
#define BB_F_TFO_RFU_FLOW_INT                                   WS_FIELD(0x0058, 23, 23) /* RC 初始值：0 */
/* 接收fifo将满中断 */
#define BB_F_RX_FIFO_FULL_INT                                   WS_FIELD(0x0058, 22, 22) /* RC 初始值：0 */
/* 发射fifo将满中断 */
#define BB_F_TX_FIFO_AF_INT                                     WS_FIELD(0x0058, 21, 21) /* RC 初始值：0 */
/* 发射fifo将空中断 */
#define BB_F_TX_FIFO_AE__INT                                    WS_FIELD(0x0058, 20, 20) /* RC 初始值：0 */
/* 接收完成中断 */
#define BB_F_RX_DONE_INT                                        WS_FIELD(0x0058, 19, 19) /* RC 初始值：0 */
/* 发送完成中断 */
#define BB_F_TX_DONE_INT                                        WS_FIELD(0x0058, 18, 18) /* RC 初始值：0 */
/* crc错误中断 */
#define BB_F_CRC_ERROR_INT                                      WS_FIELD(0x0058, 17, 17) /* RC 初始值：0 */
/* id match int  */
#define BB_F_ID_MATCH_INT                                       WS_FIELD(0x0058, 16, 16) /* RC 初始值：0 */
/* fifo下溢出 status */
#define BB_F_FIFO_UNDER_FLOW                                    WS_FIELD(0x0058, 12, 12) /* RO 初始值：0 */
/* fifo上溢出 status */
#define BB_F_FIFO_OVER_FLOW                                     WS_FIELD(0x0058, 11, 11) /* RO 初始值：0 */
/* 发射顶层状态机 */
#define BB_F_TX_STATE                                           WS_FIELD(0x0058,  7, 10) /* RO 初始值：0 */
/* 接收状态机 */
#define BB_F_RX_STATE                                           WS_FIELD(0x0058,  3,  6) /* RO 初始值：0 */
/* 顶层控制状态机 */
#define BB_F_TOP_STATE                                          WS_FIELD(0x0058,  0,  2) /* RO 初始值：0 */

/* HEADER_2P4G  */
#define BB_R_CFG23_2P4G                                         WS_REG(0x005C)           /* RW 初始值：00000000 */
/* 发射状态 */
#define BB_F_2P4G_TX_VLD_FLAG                                   WS_FIELD(0x005C, 31, 31) /* RO 初始值：0 */
/* 接收状态机 */
#define BB_F_2P4G_RX_STATE                                      WS_FIELD(0x005C, 27, 30) /* RO 初始值：0 */
/* 顶层控制状态机 */
#define BB_F_2P4G_TOP_STATE                                     WS_FIELD(0x005C, 24, 26) /* RO 初始值：0 */
/* 丢包寄存器（达到最大次数后自动清零）
当到达最大重传次数时还未接收到ACK包plos_cnt加一
状态机退出到空闲状态 */
#define BB_F_2P4G_PLOS_CNT                                      WS_FIELD(0x005C, 20, 23) /* RO 初始值：0 */
/* 自动重传的传输次数寄存器
往发射FIFO中写入寄存器后该值清零 */
#define BB_F_2P4G_ARC_CNT                                       WS_FIELD(0x005C, 16, 19) /* RO 初始值：0 */
/* 接收的包长 */
#define BB_F_2P4G_PAYLOAD_LENGTH_RCV_0_10                       WS_FIELD(0x005C,  5, 15) /* RO 初始值：0 */
/* 当前发送的pid值 */
#define BB_F_2P4G_TX_PID                                        WS_FIELD(0x005C,  3,  4) /* RO 初始值：0 */
/* 接收的pid */
#define BB_F_2P4G_RX_PID                                        WS_FIELD(0x005C,  1,  2) /* RO 初始值：0 */
/* 接收的ack */
#define BB_F_2P4G_RX_ACK                                        WS_FIELD(0x005C,  0,  0) /* RO 初始值：0 */

/* 状态寄存器  */
#define BB_R_CFG25                                              WS_REG(0x0064)           /* RW 初始值：00000000 */

/* send more bit寄存器  */
#define BB_R_CFG26                                              WS_REG(0x0068)           /* RW 初始值：00000003 */
/* 自动传输延时
0000-250us
0001-500us
0010-750us
…
1111-4000us */
#define BB_F_ARD                                                WS_FIELD(0x0068,  8, 11) /* RW 初始值：0 */
/* 自动传输次数设置
0000-普通模式
0000~1111-增强模式
0001-增强一次传输
…
1111-增强15次传输
 */
#define BB_F_ARC                                                WS_FIELD(0x0068,  4,  7) /* RW 初始值：0 */
/* 多发送几bit最后一bit数
0-不多发送
n-多发n bit */
#define BB_F_TX_SEND_MORE_BIT_NUM                               WS_FIELD(0x0068,  0,  3) /* RW 初始值：3 */

/* TX_PREA  */
#define BB_R_CFG27                                              WS_REG(0x006C)           /* RW 初始值：710F5555 */
/* 发射前导配置值 */
#define BB_F_TX_PREAMBLE                                        WS_FIELD(0x006C,  0, 31) /* RW 初始值：0x710F5555 */

/* TX_PID  */
#define BB_R_CFG28                                              WS_REG(0x0070)           /* RW 初始值：00000000 */
/* 手动配置PID使能信号，写清 */
#define BB_F_TX_PID_CFG_EN                                      WS_FIELD(0x0070,  4,  4) /* RW 初始值：0 */
/* 手动配置下包发送的PID */
#define BB_F_TX_PID_CFG                                         WS_FIELD(0x0070,  2,  3) /* RW 初始值：0 */

/* ch  */
#define BB_R_CFG29                                              WS_REG(0x0074)           /* RW 初始值：00000000 */
/* channel值 */
#define BB_F_CH                                                 WS_FIELD(0x0074,  0,  7) /* RW 初始值：0 */

/* RSSI  */
#define BB_R_CFG30                                              WS_REG(0x0078)           /* RW 初始值：00000000 */
/* RSSI模块输出的平均值 */
#define BB_F_RX_RSSI_MEAN_DB                                         WS_FIELD(0x0078,  16,  25) /* RW 初始值：0 */
/* RSSI模块输出的当前截位值 */
#define BB_F_RX_RSSI_DB                                              WS_FIELD(0x0078,  0,  9) /* RW 初始值：0 */

/* FIFO_OBSERVE  */
#define BB_R_CFG60                                              WS_REG(0x00F0)           /* RW 初始值：00000000 */
/* 接收FIFO中剩余数据数量 */
#define BB_F_RXFIFO_NUM                                         WS_FIELD(0x00F0, 16, 23) /* RO 初始值：0 */
/* 发射FIFO中剩余数据数量 */
#define BB_F_TXFIFO_NUM                                         WS_FIELD(0x00F0,  0,  7) /* RO 初始值：0 */

/* FIFO_STATE  */
#define BB_R_CFG61                                              WS_REG(0x00F4)           /* RW 初始值：00000001 */
/* tx_fifo clean, 写清0 */
#define BB_F_TX_FIFO_CLEAN                                      WS_FIELD(0x00F4, 17, 17) /* WC 初始值：0 */
/* rx fifo clean，写清0 */
#define BB_F_RX_FIFO_CLEAN                                      WS_FIELD(0x00F4, 16, 16) /* WC 初始值：0 */

/* FIFO_THRESHOLD  */
#define BB_R_CFG62                                              WS_REG(0x00F8)           /* RW 初始值：37003704 */
/* 接收fifo将满阈值
阈值+1产生将满中断 */
#define BB_F_RXFF_AFTH                                          WS_FIELD(0x00F8, 24, 31) /* RW 初始值：37 */
/* 发射fifo将满阈值 */
#define BB_F_TXFF_AFTH                                          WS_FIELD(0x00F8,  8, 15) /* RW 初始值：37 */
/* 发射fifo将空阈值 */
#define BB_F_TXFF_AETH                                          WS_FIELD(0x00F8,  0,  7) /* RW 初始值：4 */

/* FIFO_DATA  */
#define BB_R_CFG63                                              WS_REG(0x00FC)           /* RW 初始值：00000000 */
/* 写-写入TX FIFO
读-读取RX FIFO
接收64 byte
发射128 byte */
#define BB_F_FIFO_DATA                                          WS_FIELD(0x00FC,  0,  7) /* WO 初始值：0 */

/*
 * BSTIM registers and fields
 */
/* 控制寄存器 1  */
#define BSTIM_R_CR1                                             WS_REG(0x0000)           /* RW 初始值：0000 */
/* 1：中断disable
0：中断enable
只会影响MCU是否进入中断服务程序，不影响中断状态timer_int_status */
#define BSTIM_F_INTP_MASK                                       WS_FIELD(0x0000,  2,  2) /* RW 初始值：1 */
/* 0：basic timer计数和PCLK同频
1：二分频 */
#define BSTIM_F_TIMER_DIV                                       WS_FIELD(0x0000,  1,  1) /* RW 初始值：0 */
/* 使能32位计数器  位0 
0：禁止计数器； 
1：使能计数器。 
禁止计数器后计数器保持上次计数不变 */
#define BSTIM_F_LONG_TIMER_EN                                   WS_FIELD(0x0000,  0,  0) /* RW 初始值：0 */

/* 控制寄存器 2  */
#define BSTIM_R_CR2                                             WS_REG(0x0004)           /* RW 初始值：00000100 */
/* 32位计数器当前计数值
写入可以改变32位计数值
timer不使能也可以写入 */
#define BSTIM_F_LONG_TIMER_VAL                                  WS_FIELD(0x0004,  0, 31) /* RW 初始值：0 */

/* 控制寄存器 3  */
#define BSTIM_R_CR3                                             WS_REG(0x0008)           /* RW 初始值：0000 */
/* 32位计数器比较值
比较到相同值时产生中断 */
#define BSTIM_F_TIMER_COMP_SET                                  WS_FIELD(0x0008,  0, 31) /* RW 初始值：0 */

/* 控制寄存器 4  */
#define BSTIM_R_CR4                                             WS_REG(0x000C)           /* RW 初始值：0000 */
/* 产生rx_done后的时间戳 */
#define BSTIM_F_RX_DONE_TIME                                    WS_FIELD(0x000C,  0, 31) /* RO 初始值：0 */

/* 控制寄存器 5  */
#define BSTIM_R_CR5                                             WS_REG(0x0010)           /* RW 初始值：0000 */
/* 写1清除timer1中断
清除完中断后该位自清 */
#define BSTIM_F_INTP_CLR                                        WS_FIELD(0x0010,  1,  1) /* WC 初始值：0 */
/* timer中断状态
intp_clr写1后清除该位
中断mask位对该位不产生影响
中断mask后可以采用轮询方式读取 */
#define BSTIM_F_TIMER_INT_STATUS                                WS_FIELD(0x0010,  0, 255) /* RO 初始值：0 */

/* 控制寄存器 6  */
#define BSTIM_R_CR6                                             WS_REG(0x0014)           /* RW 初始值：0000 */
/* 产生tx_done后的时间戳 */
#define BSTIM_F_TX_DONE_TIME                                    WS_FIELD(0x0014,  0, 31) /* RO 初始值：0 */

/*
 * CACHE registers and fields
 */
/* AES控制寄存器  */
#define CACHE_R_CACHE_CS                                        WS_REG(0x0020)           /* RW 初始值：00000000 */
/* 1：cache正在从Flash中取指
0：cache没有从Flash中取指 */
#define CACHE_F_CACHE_BUSY                                      WS_FIELD(0x0020,  0,  0) /* RO 初始值：0 */

/* CACHE刷新控制  */
#define CACHE_R_CACHE_REF                                       WS_REG(0x0024)           /* RW 初始值：00000000 */
/* 置1后刷新refresh_index指定Cache TAG，刷新完成后自动清0 */
#define CACHE_F_REFRESH                                         WS_FIELD(0x0024, 31, 31) /* RW 初始值：0 */
/* 0:只根据refresh_index进行刷新
1:对所有TAG进行进行全刷新
软件置1硬件清0 */
#define CACHE_F_ALL_TAG                                         WS_FIELD(0x0024, 30, 30) /* RW 初始值：0 */
/* 刷新地址，对应CODE BUS地址的23:0-bit */
#define CACHE_F_REFRESH_INDEX                                   WS_FIELD(0x0024,  0, 25) /* RW 初始值：0 */

/* cache后门寄存器  */
#define CACHE_R_CACHE_BACK_DOOR                                 WS_REG(0x0040)           /* RW 初始值：00A50000 */
/* 8'hEB : 关闭cache
other : 开启cache */
#define CACHE_F_CACHE_CTRL                                      WS_FIELD(0x0040, 24, 31) /* RO 初始值：0 */
/* 当等于A5时，对QSPI Controller的AHB WRAP读取开启
不等于A5时，对QSPI Controller的AHB WRAP读取关闭 */
#define CACHE_F_WRAP_CTRL                                       WS_FIELD(0x0040, 16, 23) /* RW 初始值：A5 */

/* cache Code结束地址寄存器  */
#define CACHE_R_CACHE_END_ADDR                                  WS_REG(0x007C)           /* RW 初始值：ffffffff */
/* code address必须 <= {cache_end_addr,14'hfff}
访问非法地址会产生总线错误 */
#define CACHE_F_CACHE_END_ADDR                                  WS_FIELD(0x007C, 14, 31) /* RW 初始值：3ffff */

/*
 * CRM registers and fields
 */
/* 时钟门控控制寄存器  */
#define CRM_R_CG_CTRL                                           WS_REG(0x0000)           /* RW 初始值：00000000 */

#define CRM_F_USBD_EN											WS_FIELD(0x0000, 23, 23) /* RW 初始值：0 */

#define CRM_F_HW_RF_SPI_EN                                      WS_FIELD(0x0000, 22, 22) /* RW 初始值：0 */
/* iWatchdog模块门控时钟使能：
0: 不使能； 1：使能 */
#define CRM_F_IWDT_EN                                           WS_FIELD(0x0000, 19, 19) /* RW 初始值：0 */
#define CRM_F_QSPI_CACHE_EN                                     WS_FIELD(0x0000, 18, 18) /* RW 初始值：1 */
#define CRM_F_BB_EN                                       		WS_FIELD(0x0000, 16, 16) /* RW 初始值：0 */
/* GPADC模块门控时钟使能：
0: 不使能； 1：使能 */
#define CRM_F_GPADC_EN                                          WS_FIELD(0x0000, 14, 14) /* RW 初始值：0 */
/* QDEC模块门控时钟使能：
0: 不使能； 1：使能 */
#define CRM_F_QDEC_EN                                           WS_FIELD(0x0000, 13, 13) /* RW 初始值：0 */
/* GPIO模块门控时钟使能：
0: 不使能； 1：使能 */
#define CRM_F_GPIO_EN                                           WS_FIELD(0x0000, 12, 12) /* RW 初始值：0 */
/* RTC模块门控时钟使能：
0: 不使能； 1：使能 */
#define CRM_F_RTC_EN                                            WS_FIELD(0x0000, 11, 11) /* RW 初始值：0 */
/* Timer1模块门控时钟使能：
0: 不使能； 1：使能 */
#define CRM_F_TIMER1_EN                                         WS_FIELD(0x0000,  9,  9) /* RW 初始值：0 */
/* Timer0模块门控时钟使能：
0: 不使能； 1：使能 */
#define CRM_F_TIMER0_EN                                         WS_FIELD(0x0000,  8,  8) /* RW 初始值：0 */
/* 系统配置模块门控时钟使能：
0: 不使能； 1：使能 */
#define CRM_F_SYSCFG_EN                                         WS_FIELD(0x0000,  6,  6) /* RW 初始值：0 */
/* SPI0模块门控时钟使能：
0: 不使能； 1：使能 */
#define CRM_F_SPI0_EN                                           WS_FIELD(0x0000,  3,  3) /* RW 初始值：0 */
/* UART0模块门控时钟使能：
0: 不使能； 1：使能 */
#define CRM_F_UART0_EN                                          WS_FIELD(0x0000,  1,  1) /* RW 初始值：0 */

/* 软复位寄存器  */
#define CRM_R_SOFT_RST                                          WS_REG(0x0004)           /* WC 初始值：00000000 */

#define CRM_F_USBD_SRST                                    		WS_FIELD(0x0004, 29, 29) /* WC 初始值：0 */

#define CRM_F_HW_RF_SPI_SRST                                    WS_FIELD(0x0004, 28, 28) /* WC 初始值：0 */
#define CRM_F_MDM_SRST                                          WS_FIELD(0x0004, 27, 27) /* WC 初始值：0 */
/* gpadc模块软复位信号 */
#define CRM_F_GPADC_SRST                                        WS_FIELD(0x0004, 25, 25) /* WC 初始值：0 */
/* 芯片数字部分全局软复位信号 */
#define CRM_F_GLB_SRST                                          WS_FIELD(0x0004, 24, 24) /* WC 初始值：0 */
/* iWatchdog模块软复位信号 */
#define CRM_F_IWDT_SRST                                         WS_FIELD(0x0004, 23, 23) /* WC 初始值：0 */
/* QSPI/Cache模块软复位信号 */
#define CRM_F_QSPI_CACHE_SRST                                   WS_FIELD(0x0004, 22, 22) /* RW 初始值：0 */
/* CPU内核软复位信号 */
#define CRM_F_CPU_SRST                                          WS_FIELD(0x0004, 18, 18) /* WC 初始值：0 */
/* CRM模块软复位信号 */
#define CRM_F_CRM_SRST                                          WS_FIELD(0x0004, 17, 17) /* WC 初始值：0 */
#define CRM_F_BB_SRST                                           WS_FIELD(0x0004, 16, 16) /* WC 初始值：0 */
/* QDEC模块软复位信号 */
#define CRM_F_QDEC_SRST                                         WS_FIELD(0x0004, 13, 13) /* WC 初始值：0 */
/* GPIO模块软复位信号 */
#define CRM_F_GPIO_SRST                                         WS_FIELD(0x0004, 12, 12) /* WC 初始值：0 */
/* RTC模块软复位信号 */
#define CRM_F_RTC_SRST                                          WS_FIELD(0x0004, 11, 11) /* WC 初始值：0 */
/* Timer1模块软复位信号 */
#define CRM_F_TIMER1_SRST                                       WS_FIELD(0x0004,  9,  9) /* WC 初始值：0 */
/* Timer0模块软复位信号 */
#define CRM_F_TIMER0_SRST                                       WS_FIELD(0x0004,  8,  8) /* WC 初始值：0 */
/* 系统配置模块软复位信号 */
#define CRM_F_SYSCFG_SRST                                       WS_FIELD(0x0004,  6,  6) /* WC 初始值：0 */
/* SPI0模块软复位信号 */
#define CRM_F_SPI0_SRST                                         WS_FIELD(0x0004,  3,  3) /* WC 初始值：0 */
/* UART0软复位信号 */
#define CRM_F_UART0_SRST                                        WS_FIELD(0x0004,  1,  1) /* WC 初始值：0 */

/*
 * GPADC registers and fields
 */
/* GPADC配置寄存器  */
#define GPADC_R_GPADC_CONF                                      WS_REG(0x0000)           /* RW 初始值：0000C050 */
/* 00: 检测链路的增益为0.5
01: 检测链路的增益为1
10: 检测链路的增益为2
11: 检测链路的增益为4
电压检测时增益必须设置为2
温度检测时增益必须设置为4 */
#define GPADC_F_GPADC_GAIN                                      WS_FIELD(0x0000, 14, 15) /* RW 初始值：3 */
/* 对8个模拟GPIO进行8选1作为负输入端 */
#define GPADC_F_GPADC_SEL2                                      WS_FIELD(0x0000, 11, 13) /* RW 初始值：0 */
/* 对4个模拟GPIO进行4选1作为正输入端
0:GPIO A0 
1:GPIO A1 
2:GPIO A2 
3:GPIO A3 
others:无效 */
#define GPADC_F_GPADC_SEL1                                      WS_FIELD(0x0000,  8, 10) /* RW 初始值：0 */
/* 000: VCM=0.45
001: VCM=0.50
010: VCM=0.55
011: VCM=0.60
100: VCM=0.65
101: VCM=0.70
110: VCM=0.75
111: VCM=0.80 */
#define GPADC_F_GPADC_VCM_SEL                                   WS_FIELD(0x0000,  4,  6) /* RW 初始值：5 */
/* 0：检测GPIO单端输入信号
1: 检测GPIO差分输入信号
当检测温度、电池和offset时，gpadc_diff_en必须设置0 */
#define GPADC_F_GPADC_DIFF_EN                                   WS_FIELD(0x0000,  3,  3) /* RW 初始值：0 */
/* 00: 测量链路的offset检测
01：温度检测
10：电池检测
11：GPIO检测 */
#define GPADC_F_GPADC_CH_SEL                                    WS_FIELD(0x0000,  1,  2) /* RW 初始值：0 */

/* GPADC控制寄存器  */
#define GPADC_R_GPADC_CTRL                                      WS_REG(0x0004)           /* RW 初始值：00000000 */
/* GPADC FIFO Mode sample rate = 1Mhz/(sample_rate+1) */
#define GPADC_F_SAMPLE_RATE                                     WS_FIELD(0x0004,  8, 15) /* RW 初始值：0 */
/* 00: 单次转换，数据写入GPADC_DATA
01：多次转换，数据写入GPADC_DATA
10：连续转换，数据写入FIFO */
#define GPADC_F_GPADC_MODE                                      WS_FIELD(0x0004,  1,  2) /* RW 初始值：0 */
/* GPADC enable
当gpadc_mode=00时,单次转换，数据写入GPADC_DATA，完成后gpadc_en硬件自动清零，并且给出gpadc_done_int中断；

当gpadc_mode=01时，多次转换，当前实时数据写入GPADC_DATA，不会产生gpadc_done_int中断，需软件清零gpadc_en。

当gpadc_mode=10时,多次转换，数据写入FIFO，软件清零gpadc_en。
 */
#define GPADC_F_GPADC_EN                                        WS_FIELD(0x0004,  0,  0) /* RW 初始值：0 */

/* GPADC FIFOLEVEL寄存器  */
#define GPADC_R_GPADC_FIFOLEVEL                                 WS_REG(0x0008)           /* RW 初始值：00000000 */
/* FIFO Level，最大值为FIFO深度16
 */
#define GPADC_F_FIFO_LEVEL                                      WS_FIELD(0x0008,  0,  4) /* RO 初始值：0 */

/* GPADC Threshold寄存器  */
#define GPADC_R_GPADC_THRESHOLD                                 WS_REG(0x000C)           /* RW 初始值：00000008 */
/* FIFO Threshold
 */
#define GPADC_F_FIFO_THRESHOLD                                  WS_FIELD(0x000C,  0,  4) /* RW 初始值：8 */

/* GPADC FIFO FLUSH寄存器  */
#define GPADC_R_GPADC_FIFOFLUSH                                 WS_REG(0x0010)           /* WO 初始值：00000000 */
/* FIFO Flush操作
 */
#define GPADC_F_FIFO_FLUSH                                      WS_FIELD(0x0010,  0,  0) /* WO 初始值：0 */

/* GPADC FIFO DATA寄存器  */
#define GPADC_R_GPADC_FIFODATA                                  WS_REG(0x0014)           /* RO 初始值：00000000 */
/* FIFO DATA */
#define GPADC_F_FIFO_DATA                                       WS_FIELD(0x0014,  0,  9) /* RO 初始值：0 */

/* 中断状态寄存器  */
#define GPADC_R_GPADC_STAT                                      WS_REG(0x0018)           /* RO 初始值：00000000 */
/* FIFO写满溢出时产生中断
0: GPADC interrupt not occur
1: GPADC interrupt occur
Write 1 to clear it. Write 0 is ignored. */
#define GPADC_F_INT_OVFL                                        WS_FIELD(0x0018,  2,  2) /* WC 初始值：0 */
/* 当FIFO Level>=FIFO Threshold时，产生中断
0: GPADC interrupt not occur
1: GPADC interrupt occur
Write 1 to clear it. Write 0 is ignored. */
#define GPADC_F_INT_ALERT                                       WS_FIELD(0x0018,  1,  1) /* WC 初始值：0 */
/* GPADC done interrupt occurs.
0: GPADC interrupt not occur
1: GPADC interrupt occur
Write 1 to clear it. Write 0 is ignored.
 */
#define GPADC_F_INT_DONE                                        WS_FIELD(0x0018,  0,  0) /* WC 初始值：0 */

/* 中断控制寄存器  */
#define GPADC_R_GPADC_INT_CTRL                                  WS_REG(0x001C)           /* RW 初始值：00000000 */
/* Allows the user to mask a generated FIFO Overflow interrupt.
0 = Interrupt unmasked
1 = Interrupt masked */
#define GPADC_F_INT_OVFL_MASK                                   WS_FIELD(0x001C, 10, 10) /* RW 初始值：0 */
/* Allows the user to mask a generated FIFO Threshols Alert interrupt.
0 = Interrupt unmasked
1 = Interrupt masked */
#define GPADC_F_INT_ALERT_MASK                                  WS_FIELD(0x001C,  9,  9) /* RW 初始值：0 */
/* Allows the user to mask a generated GPADC done interrupt.
0 = Interrupt unmasked
1 = Interrupt masked */
#define GPADC_F_INT_DONE_MASK                                   WS_FIELD(0x001C,  8,  8) /* RW 初始值：0 */
/* Allows the user to disable FIFO Overflow interrupt generation.
0 = Interrupt disabled
1 = Interrupt enabled */
#define GPADC_F_INT_OVFL_EN                                     WS_FIELD(0x001C,  2,  2) /* RW 初始值：0 */
/* Allows the user to disable FIFO Threshols Alert interrupt generation.
0 = Interrupt disabled
1 = Interrupt enabled */
#define GPADC_F_INT_ALERT_EN                                    WS_FIELD(0x001C,  1,  1) /* RW 初始值：0 */
/* Allows the user to disable GPADC done interrupt generation.
0 = Interrupt disabled
1 = Interrupt enabled */
#define GPADC_F_INT_DONE_EN                                     WS_FIELD(0x001C,  0,  0) /* RW 初始值：0 */

/* GPADC数据寄存器  */
#define GPADC_R_GPADC_DATA                                      WS_REG(0x0020)           /* RO 初始值：00000000 */
/* ADC数据 */
#define GPADC_F_GPADC_DATA                                      WS_FIELD(0x0020,  0,  9) /* RO 初始值：0 */

/* DMA控制寄存器  */
#define GPADC_R_DMA_CR                                          WS_REG(0x0024)           /* RW 初始值：00000000 */
/* DMA使能 */
#define GPADC_F_DMACR                                           WS_FIELD(0x0024,  0,  0) /* RW 初始值：0 */

/* DMA接收FIFOLEVEL寄存器  */
#define GPADC_R_DMA_RDLR                                        WS_REG(0x0028)           /* RW 初始值：00000000 */
/* DMA receive fifo data level
当FIFO Level >= dmardlr + 1时，发起DMA请求  */
#define GPADC_F_DMARDLR                                         WS_FIELD(0x0028,  0,  3) /* RW 初始值：0 */

/*
 * GPIO registers and fields
 */
/* GPIO A数据寄存器  */
#define GPIO_R_PA_IOD                                           WS_REG(0x0000)           /* RW 初始值：00000000 */
/* GPIO A输入数据寄存器 */
#define GPIO_F_PA_DATA_IN                                       WS_FIELD(0x0000,  8, 15) /* RO 初始值：0 */
/* GPIO PA7输入位 */
#define GPIO_F_PA7_IN                                           WS_FIELD(0x0000, 15, 15) /* RO 初始值：0 */
/* GPIO PA6输入位 */
#define GPIO_F_PA6_IN                                           WS_FIELD(0x0000, 14, 14) /* RO 初始值：0 */
/* GPIO PA5输入位 */
#define GPIO_F_PA5_IN                                           WS_FIELD(0x0000, 13, 13) /* RO 初始值：0 */
/* GPIO PA4输入位 */
#define GPIO_F_PA4_IN                                           WS_FIELD(0x0000, 12, 12) /* RO 初始值：0 */
/* GPIO PA3输入位 */
#define GPIO_F_PA3_IN                                           WS_FIELD(0x0000, 11, 11) /* RO 初始值：0 */
/* GPIO PA2输入位 */
#define GPIO_F_PA2_IN                                           WS_FIELD(0x0000, 10, 10) /* RO 初始值：0 */
/* GPIO PA1输入位 */
#define GPIO_F_PA1_IN                                           WS_FIELD(0x0000,  9,  9) /* RO 初始值：0 */
/* GPIO PA0输入位 */
#define GPIO_F_PA0_IN                                           WS_FIELD(0x0000,  8,  8) /* RO 初始值：0 */
/* GPIO A输出数据寄存器 */
#define GPIO_F_PA_DATA_OUT                                      WS_FIELD(0x0000,  0,  7) /* RW 初始值：0 */
/* GPIO PA7输出位 */
#define GPIO_F_PA7_OUT                                          WS_FIELD(0x0000,  7,  7) /* RW 初始值：0 */
/* GPIO PA6输出位 */
#define GPIO_F_PA6_OUT                                          WS_FIELD(0x0000,  6,  6) /* RW 初始值：0 */
/* GPIO PA5输出位 */
#define GPIO_F_PA5_OUT                                          WS_FIELD(0x0000,  5,  5) /* RW 初始值：0 */
/* GPIO PA4输出位 */
#define GPIO_F_PA4_OUT                                          WS_FIELD(0x0000,  4,  4) /* RW 初始值：0 */
/* GPIO PA3输出位 */
#define GPIO_F_PA3_OUT                                          WS_FIELD(0x0000,  3,  3) /* RW 初始值：0 */
/* GPIO PA2输出位 */
#define GPIO_F_PA2_OUT                                          WS_FIELD(0x0000,  2,  2) /* RW 初始值：0 */
/* GPIO PA1输出位 */
#define GPIO_F_PA1_OUT                                          WS_FIELD(0x0000,  1,  1) /* RW 初始值：0 */
/* GPIO PA0输出位 */
#define GPIO_F_PA0_OUT                                          WS_FIELD(0x0000,  0,  0) /* RW 初始值：0 */

/* GPIO A bit置位/复位寄存器  */
#define GPIO_R_PA_BSR                                           WS_REG(0x0004)           /* WO 初始值：00000000 */
/* GPIO A对应bit复位寄存器，当向对应的bit写1时，pa_datout对应的bit位被清零 */
#define GPIO_F_PA_BIT_CLR                                       WS_FIELD(0x0004,  8, 15) /* WO 初始值：0 */
/* GPIO A7清除位 */
#define GPIO_F_PA7_CLR                                          WS_FIELD(0x0004, 15, 15) /* WO 初始值：0 */
/* GPIO A6清除位 */
#define GPIO_F_PA6_CLR                                          WS_FIELD(0x0004, 14, 14) /* WO 初始值：0 */
/* GPIO A5清除位 */
#define GPIO_F_PA5_CLR                                          WS_FIELD(0x0004, 13, 13) /* WO 初始值：0 */
/* GPIO A4清除位 */
#define GPIO_F_PA4_CLR                                          WS_FIELD(0x0004, 12, 12) /* WO 初始值：0 */
/* GPIO A3清除位 */
#define GPIO_F_PA3_CLR                                          WS_FIELD(0x0004, 11, 11) /* WO 初始值：0 */
/* GPIO A2清除位 */
#define GPIO_F_PA2_CLR                                          WS_FIELD(0x0004, 10, 10) /* WO 初始值：0 */
/* GPIO A1清除位 */
#define GPIO_F_PA1_CLR                                          WS_FIELD(0x0004,  9,  9) /* WO 初始值：0 */
/* GPIO A0清除位 */
#define GPIO_F_PA0_CLR                                          WS_FIELD(0x0004,  8,  8) /* WO 初始值：0 */
/* GPIO A对应bit置位寄存器，当向对应的bit写1时，pa_datout对应的bit位被置1 */
#define GPIO_F_PA_BIT_SET                                       WS_FIELD(0x0004,  0,  7) /* WO 初始值：0 */
/* GPIO A7设置位 */
#define GPIO_F_PA7_SET                                          WS_FIELD(0x0004,  7,  7) /* WO 初始值：0 */
/* GPIO A6设置位 */
#define GPIO_F_PA6_SET                                          WS_FIELD(0x0004,  6,  6) /* WO 初始值：0 */
/* GPIO A5设置位 */
#define GPIO_F_PA5_SET                                          WS_FIELD(0x0004,  5,  5) /* WO 初始值：0 */
/* GPIO A4设置位 */
#define GPIO_F_PA4_SET                                          WS_FIELD(0x0004,  4,  4) /* WO 初始值：0 */
/* GPIO A3设置位 */
#define GPIO_F_PA3_SET                                          WS_FIELD(0x0004,  3,  3) /* WO 初始值：0 */
/* GPIO A2设置位 */
#define GPIO_F_PA2_SET                                          WS_FIELD(0x0004,  2,  2) /* WO 初始值：0 */
/* GPIO A1设置位 */
#define GPIO_F_PA1_SET                                          WS_FIELD(0x0004,  1,  1) /* WO 初始值：0 */
/* GPIO A0设置位 */
#define GPIO_F_PA0_SET                                          WS_FIELD(0x0004,  0,  0) /* WO 初始值：0 */

/* GPIO A控制寄存器  */
#define GPIO_R_PA_CON                                           WS_REG(0x0008)           /* RW 初始值：0000AAAA */
/* GPIO A对应bit输入/输出使能,上拉/下拉电阻使能控制寄存器：
00 ： 输入, 不选择上拉/下拉电阻；
01 ： 输入，选择上拉电阻
10 ： 输入，选择下拉电阻 
11 ： 输出。
注意，作为ADC输入口时，不受该控制位控制 */
#define GPIO_F_PA_CON                                           WS_FIELD(0x0008,  0, 15) /* RW 初始值：AAAA */
/* GPIO PA7控制位 */
#define GPIO_F_PA7_CON                                          WS_FIELD(0x0008, 14, 15) /* RW 初始值：2 */
/* GPIO PA6控制位 */
#define GPIO_F_PA6_CON                                          WS_FIELD(0x0008, 12, 13) /* RW 初始值：2 */
/* GPIO PA5控制位 */
#define GPIO_F_PA5_CON                                          WS_FIELD(0x0008, 10, 11) /* RW 初始值：2 */
/* GPIO PA4控制位 */
#define GPIO_F_PA4_CON                                          WS_FIELD(0x0008,  8,  9) /* RW 初始值：2 */
/* GPIO PA3控制位 */
#define GPIO_F_PA3_CON                                          WS_FIELD(0x0008,  6,  7) /* RW 初始值：2 */
/* GPIO PA2控制位 */
#define GPIO_F_PA2_CON                                          WS_FIELD(0x0008,  4,  5) /* RW 初始值：2 */
/* GPIO PA1控制位 */
#define GPIO_F_PA1_CON                                          WS_FIELD(0x0008,  2,  3) /* RW 初始值：2 */
/* GPIO PA0控制位 */
#define GPIO_F_PA0_CON                                          WS_FIELD(0x0008,  0,  1) /* RW 初始值：2 */

/* GPIO B数据寄存器  */
#define GPIO_R_PB_IOD                                           WS_REG(0x0010)           /* RW 初始值：00000000 */
/* GPIO A输入数据寄存器 */
#define GPIO_F_PB_DATA_IN                                       WS_FIELD(0x0010,  8, 15) /* RO 初始值：0 */
/* GPIO PB7输入位 */
#define GPIO_F_PB7_IN                                           WS_FIELD(0x0010, 15, 15) /* RO 初始值：0 */
/* GPIO PB6输入位 */
#define GPIO_F_PB6_IN                                           WS_FIELD(0x0010, 14, 14) /* RO 初始值：0 */
/* GPIO PB5输入位 */
#define GPIO_F_PB5_IN                                           WS_FIELD(0x0010, 13, 13) /* RO 初始值：0 */
/* GPIO PB4输入位 */
#define GPIO_F_PB4_IN                                           WS_FIELD(0x0010, 12, 12) /* RO 初始值：0 */
/* GPIO PB3输入位 */
#define GPIO_F_PB3_IN                                           WS_FIELD(0x0010, 11, 11) /* RO 初始值：0 */
/* GPIO PB2输入位 */
#define GPIO_F_PB2_IN                                           WS_FIELD(0x0010, 10, 10) /* RO 初始值：0 */
/* GPIO PB1输入位 */
#define GPIO_F_PB1_IN                                           WS_FIELD(0x0010,  9,  9) /* RO 初始值：0 */
/* GPIO PB0输入位 */
#define GPIO_F_PB0_IN                                           WS_FIELD(0x0010,  8,  8) /* RO 初始值：0 */
/* GPIO B输出数据寄存器 */
#define GPIO_F_PB_DATA_OUT                                      WS_FIELD(0x0010,  0,  7) /* RW 初始值：0 */
/* GPIO PB7输出位 */
#define GPIO_F_PB7_OUT                                          WS_FIELD(0x0010,  7,  7) /* RW 初始值：0 */
/* GPIO PB6输出位 */
#define GPIO_F_PB6_OUT                                          WS_FIELD(0x0010,  6,  6) /* RW 初始值：0 */
/* GPIO PB5输出位 */
#define GPIO_F_PB5_OUT                                          WS_FIELD(0x0010,  5,  5) /* RW 初始值：0 */
/* GPIO PB4输出位 */
#define GPIO_F_PB4_OUT                                          WS_FIELD(0x0010,  4,  4) /* RW 初始值：0 */
/* GPIO PB3输出位 */
#define GPIO_F_PB3_OUT                                          WS_FIELD(0x0010,  3,  3) /* RW 初始值：0 */
/* GPIO PB2输出位 */
#define GPIO_F_PB2_OUT                                          WS_FIELD(0x0010,  2,  2) /* RW 初始值：0 */
/* GPIO PB1输出位 */
#define GPIO_F_PB1_OUT                                          WS_FIELD(0x0010,  1,  1) /* RW 初始值：0 */
/* GPIO PB0输出位 */
#define GPIO_F_PB0_OUT                                          WS_FIELD(0x0010,  0,  0) /* RW 初始值：0 */

/* GPIO B bit置位/复位寄存器  */
#define GPIO_R_PB_BSR                                           WS_REG(0x0014)           /* WO 初始值：00000000 */
/* GPIO B对应bit复位寄存器，当向对应的bit写1时，pb_dataout对应的bit位被清零 */
#define GPIO_F_PB_BIT_CLR                                       WS_FIELD(0x0014,  8, 15) /* WO 初始值：0 */
/* GPIO B7清除位 */
#define GPIO_F_PB7_CLR                                          WS_FIELD(0x0014, 15, 15) /* WO 初始值：0 */
/* GPIO B6清除位 */
#define GPIO_F_PB6_CLR                                          WS_FIELD(0x0014, 14, 14) /* WO 初始值：0 */
/* GPIO B5清除位 */
#define GPIO_F_PB5_CLR                                          WS_FIELD(0x0014, 13, 13) /* WO 初始值：0 */
/* GPIO B4清除位 */
#define GPIO_F_PB4_CLR                                          WS_FIELD(0x0014, 12, 12) /* WO 初始值：0 */
/* GPIO B3清除位 */
#define GPIO_F_PB3_CLR                                          WS_FIELD(0x0014, 11, 11) /* WO 初始值：0 */
/* GPIO B2清除位 */
#define GPIO_F_PB2_CLR                                          WS_FIELD(0x0014, 10, 10) /* WO 初始值：0 */
/* GPIO B1清除位 */
#define GPIO_F_PB1_CLR                                          WS_FIELD(0x0014,  9,  9) /* WO 初始值：0 */
/* GPIO B0清除位 */
#define GPIO_F_PB0_CLR                                          WS_FIELD(0x0014,  8,  8) /* WO 初始值：0 */
/* GPIO B对应bit置位寄存器，当向对应的bit写1时，pb_dataout对应的bit位被置1 */
#define GPIO_F_PB_BIT_SET                                       WS_FIELD(0x0014,  0,  7) /* WO 初始值：0 */
/* GPIO B7设置位 */
#define GPIO_F_PB7_SET                                          WS_FIELD(0x0014,  7,  7) /* WO 初始值：0 */
/* GPIO B6设置位 */
#define GPIO_F_PB6_SET                                          WS_FIELD(0x0014,  6,  6) /* WO 初始值：0 */
/* GPIO B5设置位 */
#define GPIO_F_PB5_SET                                          WS_FIELD(0x0014,  5,  5) /* WO 初始值：0 */
/* GPIO B4设置位 */
#define GPIO_F_PB4_SET                                          WS_FIELD(0x0014,  4,  4) /* WO 初始值：0 */
/* GPIO B3设置位 */
#define GPIO_F_PB3_SET                                          WS_FIELD(0x0014,  3,  3) /* WO 初始值：0 */
/* GPIO B2设置位 */
#define GPIO_F_PB2_SET                                          WS_FIELD(0x0014,  2,  2) /* WO 初始值：0 */
/* GPIO B1设置位 */
#define GPIO_F_PB1_SET                                          WS_FIELD(0x0014,  1,  1) /* WO 初始值：0 */
/* GPIO B0设置位 */
#define GPIO_F_PB0_SET                                          WS_FIELD(0x0014,  0,  0) /* WO 初始值：0 */

/* GPIO B控制寄存器  */
#define GPIO_R_PB_CON                                           WS_REG(0x0018)           /* RW 初始值：0000AAAA */
/* GPIO B对应bit输入/输出使能,上拉/下拉电阻使能控制寄存器：
00 ： 输入, 不选择上拉/下拉电阻；
01 ： 输入，选择上拉电阻
10 ： 输入，选择下拉电阻 
11 ： 输出。
注意，作为ADC输入口时，不受该控制位控制 */
#define GPIO_F_PB_CON                                           WS_FIELD(0x0018,  0, 15) /* RW 初始值：AAAA */
/* GPIO PB7控制位 */
#define GPIO_F_PB7_CON                                          WS_FIELD(0x0018, 14, 15) /* RW 初始值：2 */
/* GPIO PB6控制位 */
#define GPIO_F_PB6_CON                                          WS_FIELD(0x0018, 12, 13) /* RW 初始值：2 */
/* GPIO PB5控制位 */
#define GPIO_F_PB5_CON                                          WS_FIELD(0x0018, 10, 11) /* RW 初始值：2 */
/* GPIO PB4控制位 */
#define GPIO_F_PB4_CON                                          WS_FIELD(0x0018,  8,  9) /* RW 初始值：2 */
/* GPIO PB3控制位 */
#define GPIO_F_PB3_CON                                          WS_FIELD(0x0018,  6,  7) /* RW 初始值：2 */
/* GPIO PB2控制位 */
#define GPIO_F_PB2_CON                                          WS_FIELD(0x0018,  4,  5) /* RW 初始值：2 */
/* GPIO PB1控制位 */
#define GPIO_F_PB1_CON                                          WS_FIELD(0x0018,  2,  3) /* RW 初始值：2 */
/* GPIO PB0控制位 */
#define GPIO_F_PB0_CON                                          WS_FIELD(0x0018,  0,  1) /* RW 初始值：2 */

/* GPIO A[3:0]模式寄存器  */
#define GPIO_R_PA_L_MODE                                        WS_REG(0x0040)           /* RW 初始值：00000000 */
/* GPIO的功能复用选择：
0：GPIO
1：UART0_RX
2：UART0_TX
3：UART0_CTSN
4: UART0_RTSN
5-8：Reserved
9: SPI0_CSN
10: SPI0_CLK
11: SPI0_MOSI
12: SPI0_MISO
13-14: Reserved
15: PWM_CH2
16: PWM_CH3
17: PWM_CH4
18: PWM_CH5
19: PWM_CH1
20: Reserved
21: QDEC_CHA
22: QDEC_CHB
23: QDEC_IDX
26-24: Reserved
27: JTAG_TCK/SWCLK
28: JTAG_TDO
29: JTAG_TDI
30: JTAG_TMS/SWDIO
31: JTAG_TRST
32: PWM_CH6
33-47:Reserved
48: Analog(GPADC：P0[7:0]; AUADC: P0[2],P0[3];IR: P0[4])
49: IrDA RX
50: IrDA TX
51: 12M CLOCK
52: SYS_DBG3(for test only)
53: SYS_DBG2(for test only)
54：16M OSC/RC CLOCK
55:{3'h0,GPADC/AUADC DO[1:0],3'h0}(for test only)
56: GPADC/AUADC DO[9:2](for test only)
57: SYS_DBG1(for test only) 
58: CLK32K(for test only)
59: MDM_CLK(for test only)
60: BTDM_DBG0(for test only)  
61: BTDM_DBG1(for test only)
62: BTDM_DBG2(for test only)
63: BTDM_DBG3(for test only) */
#define GPIO_F_PA3_MODE                                         WS_FIELD(0x0040, 24, 29) /* RW 初始值：0 */
/* see PA_L_MODE.pa3_mode */
#define GPIO_F_PA2_MODE                                         WS_FIELD(0x0040, 16, 21) /* RW 初始值：0 */
/* see PA_L_MODE.pa3_mode */
#define GPIO_F_PA1_MODE                                         WS_FIELD(0x0040,  8, 13) /* RW 初始值：0 */
/* see PA_L_MODE.pa3_mode */
#define GPIO_F_PA0_MODE                                         WS_FIELD(0x0040,  0,  5) /* RW 初始值：0 */

/* GPIO A[7:4]模式寄存器  */
#define GPIO_R_PA_H_MODE                                        WS_REG(0x0044)           /* RW 初始值：00000000 */
/* see PA_L_MODE.pa3_mode */
#define GPIO_F_PA7_MODE                                         WS_FIELD(0x0044, 24, 29) /* RW 初始值：0 */
/* see PA_L_MODE.pa3_mode */
#define GPIO_F_PA6_MODE                                         WS_FIELD(0x0044, 16, 21) /* RW 初始值：0 */
/* see PA_L_MODE.pa3_mode */
#define GPIO_F_PA5_MODE                                         WS_FIELD(0x0044,  8, 13) /* RW 初始值：0 */
/* see PA_L_MODE.pa3_mode */
#define GPIO_F_PA4_MODE                                         WS_FIELD(0x0044,  0,  5) /* RW 初始值：0 */

/* GPIO B[3:0]模式寄存器  */
#define GPIO_R_PB_L_MODE                                        WS_REG(0x0048)           /* RW 初始值：0000001B */
/* see PA_L_MODE.pa3_mode */
#define GPIO_F_PB3_MODE                                         WS_FIELD(0x0048, 24, 29) /* RW 初始值：1B */
/* see PA_L_MODE.pa3_mode */
#define GPIO_F_PB2_MODE                                         WS_FIELD(0x0048, 16, 21) /* RW 初始值：0 */
/* see PA_L_MODE.pa3_mode */
#define GPIO_F_PB1_MODE                                         WS_FIELD(0x0048,  8, 13) /* RW 初始值：0 */
/* see PA_L_MODE.pa3_mode
缺省作为JTAG_TDO */
#define GPIO_F_PB0_MODE                                         WS_FIELD(0x0048,  0,  5) /* RW 初始值：0 */

/* GPIO B[7:4]模式寄存器  */
#define GPIO_R_PB_H_MODE                                        WS_REG(0x004C)           /* RW 初始值：00001E00 */
/* see PA_L_MODE.pa3_mode */
#define GPIO_F_PB7_MODE                                         WS_FIELD(0x004C, 24, 29) /* RW 初始值：0 */
/* see PA_L_MODE.pa3_mode */
#define GPIO_F_PB6_MODE                                         WS_FIELD(0x004C, 16, 21) /* RW 初始值：0 */
/* see PA_L_MODE.pa3_mode
缺省作为JTAG_TMS */
#define GPIO_F_PB5_MODE                                         WS_FIELD(0x004C,  8, 13) /* RW 初始值：1E */
/* see PA_L_MODE.pa3_mode */
#define GPIO_F_PB4_MODE                                         WS_FIELD(0x004C,  0,  5) /* RW 初始值：0 */

/* 中断类型配置寄存器  */
#define GPIO_R_INTP_TYPE                                        WS_REG(0x0080)           /* RW 初始值：00000000 */
/* 中断0类型及使能控制位：
00 ：不产生中断；01 ：上升沿中断；
10 ：下降沿中断；11 : 双沿中断 */
#define GPIO_F_INT0_TYPE                                        WS_FIELD(0x0080,  0,  1) /* RW 初始值：0 */

/* 中断源配置寄存器  */
#define GPIO_R_INTP_SRC                                         WS_REG(0x0084)           /* RW 初始值：00000000 */
/* 中断1来源选择寄存器：
0 ：不使能； 1：使能
int0_src[8]：PB[0]中断使能位
int0_src[9]：PB[1]中断使能位
int0_src[10]：PB[2]中断使能位
int0_src[11]：PB[3]中断使能位
int0_src[12]：PB[4]中断使能位
int0_src[13]：PB[5]中断使能位
int0_src[14]：PB[6]中断使能位
int0_src[15]：PB[7]中断使能位 */
#define GPIO_F_INT0_SRC                                         WS_FIELD(0x0084,  8, 15) /* RW 初始值：0 */

/* 中断状态寄存器  */
#define GPIO_R_INTP_STAT                                        WS_REG(0x0088)           /* RO 初始值：00000000 */
/* 中断1状态寄存器，当向该位写1时清除中断1状态：
0 ：未产生中断； 1：产生中断
int1_stat[0]：PB[0]中断状态位
int1_stat[1]：PB[1]中断状态位
int1_stat[2]：PB[2]中断状态位
int1_stat[3]：PB[3]中断状态位
int1_stat[4]：PB[4]中断状态位
int1_stat[5]：PB[5]中断状态位
int1_stat[6]：PB[6]中断状态位
int1_stat[7]：PB[7]中断状态位 */
#define GPIO_F_INT0_STAT                                        WS_FIELD(0x0088,  0, 15) /* WC 初始值：0 */


/*
 * OTP registers and fields
 */
/* OTP区域  */
#define OTP_R_OTP                                               WS_REG(0x0000)           /* RO 初始值：00000000 */
/* OTP数据 */
#define OTP_F_OTP_DATA                                          WS_FIELD(0x0000,  0, 31) /* RW 初始值：0 */

/* OTP配置寄存器  */
#define OTP_R_OTP_CFG                                           WS_REG(0x0000)           /* RW 初始值：00000008 */
/* 对应OTP PTR管脚
0：选择OTP主区域
1：选择OTP Test Row */
#define OTP_F_TEST_ROW_SEL                                      WS_FIELD(0x0000,  5,  5) /* RW 初始值：0 */
/* 000：Initial Margin Read
001：PGM Margin Read
010：HT initial Margin Read
011：HT PGM Margin Read
100/others：普通OTP读取 */
#define OTP_F_OTP_RD_MODE                                       WS_FIELD(0x0000,  2,  4) /* RW 初始值：100 */
/* 00：编程。
01：休眠；
10：唤醒；
11：STOP；
只能在op_start为0时可写。 */
#define OTP_F_OP_SEL                                            WS_FIELD(0x0000,  0,  1) /* RW 初始值：0 */

/* otp控制状态寄存器  */
#define OTP_R_OTP_CS                                            WS_REG(0x0004)           /* RW 初始值：80000000 */
/* 1：可进行OTP读操作
0：OTP处于编程/休眠状态，不可进行读操作 */
#define OTP_F_RD_READY                                          WS_FIELD(0x0004, 31, 31) /* RO 初始值：1 */
/* 操作完成状态，硬件置位软件清0：
000：此次操作无异常；
001：在编程/休眠状态下对OTP进行读操作
010：对隐藏/只读区域进行编程
011：编程范围超出OTP地址范围
100：在休眠状态下进行编程操作
101：在非休眠状态下进行唤醒 */
#define OTP_F_ILLEGAL                                           WS_FIELD(0x0004,  1,  3) /* RW 初始值：0 */
/* 启动对OTP的操作，软件置1硬件清0，软件可通过该位查询操作是否完成。 */
#define OTP_F_OP_START                                          WS_FIELD(0x0004,  0,  0) /* RW 初始值：0 */

/* OTP启动保护寄存器  */
#define OTP_R_OTP_PROT                                          WS_REG(0x0008)           /* RW 初始值：00000000 */
/* 启动保护控制位。
要启动OTP编程，需进行3次连续的写操作，顺序如下：写prot 0xabcd_00a5，写prot 0x1234_005a，将op_start置1。 */
#define OTP_F_PROT                                              WS_FIELD(0x0008,  0, 31) /* WC 初始值：0 */

/* OTP编程擦除地址寄存器  */
#define OTP_R_OTP_ADDR                                          WS_REG(0x000C)           /* RW 初始值：00000000 */
/* 编程地址，按AHB地址分配填写 */
#define OTP_F_OP_ADDR                                           WS_FIELD(0x000C,  0, 31) /* RW 初始值：0 */

/* OTP编程数据寄存器  */
#define OTP_R_OTP_PDATA                                         WS_REG(0x0010)           /* RW 初始值：00000000 */
/* 用于存放32bit数据编程数据。
只能在op_start为0时可写。
启动休眠后，该寄存器会被清0

 */
#define OTP_F_PDATA                                             WS_FIELD(0x0010,  0, 31) /* RW 初始值：0 */

/* OTP主存区只读区域  */
#define OTP_R_OTP_RO                                            WS_REG(0x0014)           /* RW 初始值：FFFFFFFF */
/* 按16KB将整个Flash主存区划分成32个区域（512BYTE），每个区域由ro的1bit控制。
0：可编程/擦除。
1：只读。 */
#define OTP_F_RO                                                WS_FIELD(0x0014,  0, 31) /* RW 初始值：ffffffff */

/* OTP主存区只读锁定  */
#define OTP_R_OTP_ROL                                           WS_REG(0x0018)           /* RW 初始值：00000000 */
/* 用于锁定otp_ro寄存器的配置，一旦锁定，otp_ro对应位不能修改，复位后锁定解除。
0：不锁定。
1：锁定，置1后软件无法清0，只有复位后硬件清0。
ro_lock[0]对应ro[0]，ro_lock[1]对应ro[1]，以此类推。 */
#define OTP_F_RO_LOCK                                           WS_FIELD(0x0018,  0, 31) /* RW 初始值：0 */

/* OTP隐藏寄存器  */
#define OTP_R_OTP_HIDE                                          WS_REG(0x001C)           /* RW 初始值：00000000 */
/* 按16KB将整个Flash主存区划分成32个区域（512BYTE），每个区域由hide的1bit控制。
0：可编程/擦除/读取。
1：隐藏。 */
#define OTP_F_HIDE                                              WS_FIELD(0x001C,  0, 31) /* RW 初始值：0 */

/* OTP时序寄存器  */
#define OTP_R_OTP_TIM                                           WS_REG(0x0020)           /* RW 初始值：00000120 */
/* 1：频率切换开始（关闭OTP编程和读取）
0：频率切换结束 */
#define OTP_F_CFREQ_PERIOD                                      WS_FIELD(0x0020, 12, 12) /* RW 初始值：0 */
/* 选择CBUS读取OTP时钟速率
0：HCLK 2分频读取（主频在16MHz和32Mhz都可使用）
1：HCLK 同频读取（只能在主频16MHz模式下使用）
切换时钟频率需要先将cfreq_period置1再切换时钟，切换完成后软件将cfreq_period清0 */
#define OTP_F_SAME_FREQ                                         WS_FIELD(0x0020, 11, 11) /* RW 初始值：0 */
/* 决定10ns所用的时钟周期，比如32M时，周期为31.25ns，该寄存器写1。
只能在op_start为0时可写。 */
#define OTP_F_CYCLES_10NS                                       WS_FIELD(0x0020,  8, 10) /* RW 初始值：1 */
/* 决定1us所用的时钟周期，比如32M时，周期为31.25ns，为保险起见减去1ns作为余量，所以该寄存器配置为1000ns/31.25ns 得 32。
只能在op_start为0时可写。 */
#define OTP_F_CYCLES_1US                                        WS_FIELD(0x0020,  0,  7) /* RW 初始值：10 */

/* OTP时序寄存器  */
#define OTP_R_OTP_TIM_EN                                        WS_REG(0x0024)           /* RW 初始值：00000000 */
/* 当该寄存器等于A5时，使用otp_tim作为时序参考，否则由系统自动生成 */
#define OTP_F_TIM_EN                                            WS_FIELD(0x0024,  0,  7) /* RW 初始值：00 */

/*
 * QDEC registers and fields
 */
/* Quadrature Control Register  */
#define QDEC_R_QCR                                              WS_REG(0x0000)           /* RW 初始值：00000000 */
/* Index Read Count Bit
0 = Quadrature Read / Write register not affected.
1 = Read the value of the quad. count to the QRW reg, when index event is true. */
#define QDEC_F_INRC                                             WS_FIELD(0x0000,  9,  9) /* RW 初始值：0 */
/* Index Level configuration
0 = Index asserted when quadature index is logic low
1 = Index asserted when quadrature index is logic high */
#define QDEC_F_IDXL                                             WS_FIELD(0x0000,  8,  8) /* RW 初始值：0 */
/* Index Channel B configuration
0 = Index asserted when quadrature channel B logic low
1 = Index asserted when quadrature channel B logic high */
#define QDEC_F_ICHB                                             WS_FIELD(0x0000,  7,  7) /* RW 初始值：0 */
/* Index Channel A configuration
0 = Index asserted when quadrature channel A logic low
1 = Index asserted when quadrature channel A logic high */
#define QDEC_F_ICHA                                             WS_FIELD(0x0000,  6,  6) /* RW 初始值：0 */
/* Quadrature Count Latch
0 = No action.
1 = Latch and store quad count in QRW register, auto cleared */
#define QDEC_F_QLAT                                             WS_FIELD(0x0000,  5,  5) /* RW 初始值：0 */
/* Pre Load Count register
0 = No action.
1 = Load value currently in pre load reg into count reg, auto clear */
#define QDEC_F_PLCT                                             WS_FIELD(0x0000,  4,  4) /* RW 初始值：0 */
/* Index Zero Count Bit
0 = Internal count not affected.
1 = Zero internal quad_count when quad_index is asserted. */
#define QDEC_F_INZC                                             WS_FIELD(0x0000,  3,  3) /* RW 初始值：0 */
/* Index Enable Bit
0 = Index input disabled
1 = Index input enabled */
#define QDEC_F_INEN                                             WS_FIELD(0x0000,  2,  2) /* RW 初始值：0 */
/* Set Count Direction
0 = Counts positive when A leads B
1 = Counts negative when A leads B */
#define QDEC_F_CTDR                                             WS_FIELD(0x0000,  1,  1) /* RW 初始值：0 */
/* Enable Counting
0 = Quadrature counting disabled
1 = Quadrature counting enabled */
#define QDEC_F_ECNT                                             WS_FIELD(0x0000,  0,  0) /* RW 初始值：0 */

/* Quadrature Status Register  */
#define QDEC_R_QSR                                              WS_REG(0x0004)           /* RW 初始值：00000000 */
/* Count Compare Match Event, auto set, user cleared
0 = Compare match event has not occurred
1 = Compare match event occurred, interrupt genetated if enabled */
#define QDEC_F_CMI                                              WS_FIELD(0x0004,  4,  4) /* RW 初始值：0 */
/* Index event, auto set, user cleared
0 = Index event has not occurred
1 = Index event occured, interrupt requested if INIE set */
#define QDEC_F_INI                                              WS_FIELD(0x0004,  3,  3) /* RW 初始值：0 */
/* Counter Underflow, auto set, user cleared
0 = No underflow detected
1 = Counter underflow from 0x0000 to 0xFFFF */
#define QDEC_F_UNI                                              WS_FIELD(0x0004,  2,  2) /* RW 初始值：0 */
/* Counter Overflow, auto set, user cleared
0 = No overflow detected
1 = Counter overflow from 0xFFFF to 0x0000 */
#define QDEC_F_OVI                                              WS_FIELD(0x0004,  1,  1) /* RW 初始值：0 */
/* Quadrature decoder error status, auto set, user cleared
0 = No error
1 = Illegal state transition detected */
#define QDEC_F_QEI                                              WS_FIELD(0x0004,  0,  0) /* RW 初始值：0 */

/* Quadrature Interrupt enable Register  */
#define QDEC_R_QIR                                              WS_REG(0x0008)           /* RW 初始值：00000000 */
/* Compare Match Interrupt Enable
0 = No interrupt generated when a compare match event is asserted.
1 = An external interrupt will be generated when the compare event is asserted.   */
#define QDEC_F_CMIE                                             WS_FIELD(0x0008,  4,  4) /* RW 初始值：0 */
/* Index Interrupt Enable
0 = Index interrupt request disabled
1 = Index interrupt request enabled */
#define QDEC_F_INIE                                             WS_FIELD(0x0008,  3,  3) /* RW 初始值：0 */
/* Underflow Interrupt Enable
0 = Underflow event will not trigger an interrupt.
1 = Underflow event will trigger an interrupt */
#define QDEC_F_UNIE                                             WS_FIELD(0x0008,  2,  2) /* RW 初始值：0 */
/* Overflow Interrupt Enable
0 = Overflow event will not trigger an interrupt
1 = Overflow event will trigger an interrupt */
#define QDEC_F_OVIE                                             WS_FIELD(0x0008,  1,  1) /* RW 初始值：0 */
/* Quadrature Error Interrupt enable
0 = Quadrature Error Interrupt disabled
1 = Quadrature Error Interrupt enabled */
#define QDEC_F_QEIE                                             WS_FIELD(0x0008,  0,  0) /* RW 初始值：0 */

/* Quadrature Count Read / Write Register  */
#define QDEC_R_QRW                                              WS_REG(0x000C)           /* RW 初始值：00000000 */
/* The actual quadrature count value must be latched prior to
reading from this register.  This may be triggered two ways:
1) Writing a '1' to the QCR, bit 5 quadrature count latch
(QLAT), -or-
2) Asserting the external asyncronous input quad_idx_i.
Once either event occurs, the quadrature count value will be
copied to the QRW register.
This register is also used to hold the pre-load count value.
After writing to this register, the pre-load value is
transferred to the quadrature count register by writing a '1'
to the QCR, bit 4, quadrature pre-load count (PLCT)  */
#define QDEC_F_QRW                                              WS_FIELD(0x000C,  0, 31) /* RW 初始值：0 */

/* Quadrature Sample Clock Frequence Selection Register  */
#define QDEC_R_QCK                                              WS_REG(0x0010)           /* RW 初始值：00000000 */
/* 根据以下公式，来定义Quadrature Decoder采样时钟的频率：
F(qdec_clk) = F(PCLK)/(QCK[4:0]+1)
 */
#define QDEC_F_QCK                                              WS_FIELD(0x0010,  0,  4) /* RW 初始值：0 */


/*
 * RFSPI registers and fields
 */
/* SPI master时钟频率分频系数
0：spi_sclk=pclk/2
1: spi_sclk=pclk/2
2: spi_sclk=pclk/3
3: spi_sclk=pclk/4
4: spi_slck=pclk/8  */
#define RFSPI_R_SPI_CLKDIV                                      WS_REG(0x0000)           /* RW 初始化：1 */

#define RFSPI_R_SW_CMD0                                         WS_REG(0x0080)           /* RW 初始值：00000000 */
#define RFSPI_F_SW_CMD0_CMD_VLD                                 WS_FIELD(0x0080, 24, 24) /* RW 初始值：0 */
#define RFSPI_F_SW_CMD0_SPI_RW                                  WS_FIELD(0x0080, 23, 23) /* RW 初始值：0 */
#define RFSPI_F_SW_CMD0_SPI_ADDR                                WS_FIELD(0x0080, 16, 22) /* RW 初始值：0 */
#define RFSPI_F_SW_CMD0_SPI_RWDATA                              WS_FIELD(0x0080,  0, 15) /* RW 初始值：0 */


/*
 * RTC registers and fields
 */
/* 当前计数值寄存器  */
#define RTC_R_CCVR                                              WS_REG(0x0000)           /* RO 初始值：00000000 */
/* RTC当前计数值 */
#define RTC_F_RTC_CCRV                                          WS_FIELD(0x0000,  0, 31) /* RO 初始值：0  */

/* 计数器匹配寄存器  */
#define RTC_R_CMR                                               WS_REG(0x0004)           /* RW 初始值：00000000 */
/* 当计数器值与当前寄存器值相等时参数中断如果中断使能的话 */
#define RTC_F_RTC_CMR                                           WS_FIELD(0x0004,  0, 31) /* RW 初始值：0  */

/* 计数器加载寄存器  */
#define RTC_R_CLR                                               WS_REG(0x0008)           /* RW 初始值：00000000 */
/* 计数器重装载寄存器，当比较寄存器与当前计数值匹配时，将该寄存器的值加载到RTC_CCRV */
#define RTC_F_RTC_CLR                                           WS_FIELD(0x0008,  0, 31) /* RW 初始值：0  */

/* 计数器控制寄存器  */
#define RTC_R_CCR                                               WS_REG(0x000C)           /* RW 初始值：00000000 */
/* 允许用户在匹配发生时强制计数器清零，而不是计数到最大值
0 = disabled
1 = enabled */
#define RTC_F_RTC_WEN                                           WS_FIELD(0x000C,  3,  3) /* RW 初始值：0  */
/* 控制是否开始计数
0 = disabled
1 = enabled */
#define RTC_F_RTC_EN                                            WS_FIELD(0x000C,  2,  2) /* RW 初始值：0  */
/* 中断mask
0 = unmasked
1 = masked */
#define RTC_F_RTC_MASK                                          WS_FIELD(0x000C,  1,  1) /* RW 初始值：0  */
/* 中断使能位
0 = disabled
1 = enabled */
#define RTC_F_RTC_IEN                                           WS_FIELD(0x000C,  0,  0) /* RW 初始值：0  */

/* 中断状态寄存器  */
#define RTC_R_STAT                                              WS_REG(0x0010)           /* RO 初始值：00000000 */
/* Tis register is the masked raw status
0=Interrupt is inactive
1=Interrupt is active(regardless of polarity) */
#define RTC_F_RTC_STAT                                          WS_FIELD(0x0010,  0,  0) /* RO 初始值：0  */

/* 中断原始状态寄存器  */
#define RTC_R_RSTAT                                             WS_REG(0x0014)           /* RO 初始值：00000000 */
/* 0=Interrupt is inactive
1=Interrupt is active(regardless of polarity) */
#define RTC_F_RTC_RSTAT                                         WS_FIELD(0x0014,  0,  0) /* RO 初始值：0  */

/* 中断状态清除寄存器  */
#define RTC_R_EOI                                               WS_REG(0x0018)           /* RO 初始值：00000000 */
/* 通过读该位清除匹配中断 */
#define RTC_F_RTC_EOI                                           WS_FIELD(0x0018,  0,  0) /* RO 初始值：0  */

/* 器件版本寄存器  */
#define RTC_R_COMP_VERSION                                      WS_REG(0x001C)           /* RO 初始值：00000000 */
/* ASCLL value for each number in the version, followed by *.For example 32_30_31_2A represents the version 2.01*. */
#define RTC_F_RTC_COMP_VERISON                                  WS_FIELD(0x001C,  0, 31) /* RO 初始值：0  */

/*
 * SPIM registers and fields
 */
/* 控制寄存器0  */
#define SPIM_R_CTRLR0                                           WS_REG(0x0000)           /* RW 初始值：00000000 */
/* 控制帧长度寄存器
0111：8bit帧格式；
1111：16bit帧格式； */
#define SPIM_F_CFS                                              WS_FIELD(0x0000, 12, 15) /* RW 初始值：0  */
/* 0：正常模式；
1：测试模式； */
#define SPIM_F_SRL                                              WS_FIELD(0x0000, 11, 11) /* RW 初始值：0  */
/* 从机输出使能
0：enabled；
1：disabled； */
#define SPIM_F_SLV_OE                                           WS_FIELD(0x0000, 10, 10) /* RW 初始值：0  */
/* 00：发送和接收；
01：仅发送；
10：仅接收；
11：EEPROM读取 */
#define SPIM_F_TMOD                                             WS_FIELD(0x0000,  8,  9) /* RW 初始值：0  */
#define SPIM_F_SCPOL                                            WS_FIELD(0x0000,  7,  7) /* RW 初始值： */
#define SPIM_F_SCPH                                             WS_FIELD(0x0000,  6,  6) /* RW 初始值： */
#define SPIM_F_FRF                                              WS_FIELD(0x0000,  4,  5) /* RW 初始值：0  */
/* 数据帧长度
0111：8bit帧格式；
1111：16bit帧格式； */
#define SPIM_F_DFS                                              WS_FIELD(0x0000,  0,  3) /* RW 初始值：0x7 */

/* 控制寄存器1  */
#define SPIM_R_CTRLR1                                           WS_REG(0x0004)           /* RW 初始值：00000000 */
/* 当TMOD=10或TMOD=11时，这个寄存器设置数据帧的个数； */
#define SPIM_F_NDF                                              WS_FIELD(0x0004,  0, 15) /* RW 初始值：0  */

/* SPI使能寄存器  */
#define SPIM_R_SSIENR                                           WS_REG(0x0008)           /* RW 初始值：00000000 */
/* SPI使能 */
#define SPIM_F_SSI_EN                                           WS_FIELD(0x0008,  0,  0) /* RW 初始值：0  */

/* Microwire控制寄存器  */
#define SPIM_R_MWCR                                             WS_REG(0x000C)           /* RW 初始值：00000000 */
/* 0:handshaking interface is disabled;
1:handshaking interface is enabled; */
#define SPIM_F_MHS                                              WS_FIELD(0x000C,  2,  2) /* RW 初始值：0  */
/* 0--received by DW_apb_ssi MacroCell from the external device;
1--transmitted from the DW_apb_ssi MacroCell to the external serial device; */
#define SPIM_F_MDD                                              WS_FIELD(0x000C,  1,  1) /* RW 初始值：0  */
/* 0--non-sequential transfer;
1--sequential transfer; */
#define SPIM_F_MWMOD                                            WS_FIELD(0x000C,  0,  0) /* RW 初始值：0  */

/* 从机使能寄存器  */
#define SPIM_R_SER                                              WS_REG(0x0010)           /* RW 初始值：00000000 */
/* Slave Select Enable Flag.
1:Selected;
0:Not Selected; */
#define SPIM_F_SER                                              WS_FIELD(0x0010,  0,  0) /* RW 初始值：0  */

/* 波特率选择寄存器  */
#define SPIM_R_BAUDR                                            WS_REG(0x0014)           /* RW 初始值：00000000 */
/* 配置总线时钟速率
Fssi_clk=Fsclk/SCKDV */
#define SPIM_F_SCKDV                                            WS_FIELD(0x0014,  0, 15) /* RW 初始值：0  */

/* 发送FIFO阈值寄存器  */
#define SPIM_R_TXFTLR                                           WS_REG(0x0018)           /* RW 初始值：00000000 */
/* 发送FIFO将空中断阈值 */
#define SPIM_F_TFT                                              WS_FIELD(0x0018,  0, 15) /* RW 初始值：0  */

/* 接收FIFO阈值寄存器  */
#define SPIM_R_RXFTLR                                           WS_REG(0x001C)           /* RW 初始值：00000000 */
/* 接收FIFO溢出中断阈值 */
#define SPIM_F_RFT                                              WS_FIELD(0x001C,  0, 15) /* RW 初始值：0  */

/* 发送FIFO数据量寄存器  */
#define SPIM_R_TXFLR                                            WS_REG(0x0020)           /* RO 初始值：00000000 */
/* 发送FIFO中当前字节数 */
#define SPIM_F_TXTFL                                            WS_FIELD(0x0020,  0, 15) /* RO 初始值：0  */

/* 接收FIFO数据量寄存器  */
#define SPIM_R_RXFLR                                            WS_REG(0x0024)           /* RO 初始值：00000000 */
/* 接收FIFO中当前字节数 */
#define SPIM_F_RXTDL                                            WS_FIELD(0x0024,  0, 15) /* RO 初始值：0  */

/* 状态寄存器  */
#define SPIM_R_SR                                               WS_REG(0x0028)           /* RO 初始值：00000000 */
/* 数据碰撞错误标志
0：No error；
1：数据碰撞错误； */
#define SPIM_F_DCOL                                             WS_FIELD(0x0028,  6,  6) /* RO 初始值：0  */
/* 0：No error；
1：发送错误； */
#define SPIM_F_TXE                                              WS_FIELD(0x0028,  5,  5) /* RO 初始值：0  */
/* 0：接收FIFO不满；
1：接收FIFO满； */
#define SPIM_F_RFF                                              WS_FIELD(0x0028,  4,  4) /* RO 初始值：0  */
/* 0：接收FIFO为空；
1：接收FIFO不为空 */
#define SPIM_F_RFNE                                             WS_FIELD(0x0028,  3,  3) /* RO 初始值：0  */
/* 0：发送FIFO不为空；
1：发送FIFO为空； */
#define SPIM_F_TFE                                              WS_FIELD(0x0028,  2,  2) /* RO 初始值：1  */
/* 0：发送FIFO满；
1：发送FIFO未满； */
#define SPIM_F_TFNF                                             WS_FIELD(0x0028,  1,  1) /* RO 初始值：1  */
/* SPI忙标志
0：SPI is idle or disabled;
1:SPI is actively transferring data; */
#define SPIM_F_BUSY                                             WS_FIELD(0x0028,  0,  0) /* RO 初始值：0  */

/* 中断Mask寄存器  */
#define SPIM_R_IMR                                              WS_REG(0x002C)           /* RW 初始值：00000000 */
/* Multi-Master Contention Interrupt Mask.
0:Masked;
1:Not Masked; */
#define SPIM_F_MSTIM                                            WS_FIELD(0x002C,  5,  5) /* RW 初始值：1  */
/* Receive FIFO Full Interrupt Mask.
0:Masked;
1:Not masked; */
#define SPIM_F_RXFIM                                            WS_FIELD(0x002C,  4,  4) /* RW 初始值：1  */
/* Receive FIFO Overflow Interupt Mask.
0:Masked;
1:Not masked; */
#define SPIM_F_RXOIM                                            WS_FIELD(0x002C,  3,  3) /* RW 初始值：1  */
/* Receive FIFO Underflow Interrupt Mask.
0:masked;
1:Not masked; */
#define SPIM_F_RXUIM                                            WS_FIELD(0x002C,  2,  2) /* RW 初始值：1  */
/* Transmit FIFO Overflow Interrupt Mask.
0:masked;
1:Not masked; */
#define SPIM_F_TXOIM                                            WS_FIELD(0x002C,  1,  1) /* RW 初始值：1  */
/* Transmit FIFO Empty Interrupt Mask.
0:masked;
1:Not masked; */
#define SPIM_F_TXEIM                                            WS_FIELD(0x002C,  0,  0) /* RW 初始值：1  */

/* 中断状态寄存器  */
#define SPIM_R_ISR                                              WS_REG(0x0030)           /* RO 初始值：00000000 */
/* Multi-Master Contention Interrupt Status.
0:interrupt is not active after masking;
1:interrupt is active after masking; */
#define SPIM_F_MSTIS                                            WS_FIELD(0x0030,  5,  5) /* RO 初始值：0  */
/* Receive FIFO Full Interrupt Status.
0:interrupt is not active after masking;
1:interrupt is active after masking; */
#define SPIM_F_RXFIS                                            WS_FIELD(0x0030,  4,  4) /* RO 初始值：0  */
/* Receive FIFO Overflow Interrupt Status.
0:interrupt is not active after masking;
1:interrupt is active after masking; */
#define SPIM_F_RXOIS                                            WS_FIELD(0x0030,  3,  3) /* RO 初始值：0  */
/* Receive FIFO Underflow Interrupt Status.
0:interrupt is not active after masking;
1:interrupt is active after masking; */
#define SPIM_F_RXUIS                                            WS_FIELD(0x0030,  2,  2) /* RO 初始值：0  */
/* Transmit FIFO Overflow Interrupt Status.
0:interrupt is not active after masking;
1:interrupt is active after masking; */
#define SPIM_F_TXOIS                                            WS_FIELD(0x0030,  1,  1) /* RO 初始值：0  */
/* Transmit FIFO Empty Interrupt Status.
0:interrupt is not active after masking;
1:interrupt is active after masking; */
#define SPIM_F_TXEIS                                            WS_FIELD(0x0030,  0,  0) /* RO 初始值：0  */

/* 原始中断状态寄存器  */
#define SPIM_R_RISR                                             WS_REG(0x0034)           /* RO 初始值：00000000 */
/* Multi-Master Contention Raw Interrupt Status. This bit field is not present if the
DW_apb_ssi is configured as a serial-slave device.
0 = ssi_mst_intr interrupt is not active prior to masking
1 = ssi_mst_intr interrupt is active prior masking */
#define SPIM_F_MSTIR                                            WS_FIELD(0x0034,  5,  5) /* RO 初始值：0  */
/* Receive FIFO Full Raw Interrupt Status
0 = ssi_rxf_intr interrupt is not active prior to masking
1 = ssi_rxf_intr interrupt is active prior to masking */
#define SPIM_F_RXFIR                                            WS_FIELD(0x0034,  4,  4) /* RO 初始值：0  */
/* Receive FIFO Overflow Raw Interrupt Status
0 = ssi_rxo_intr interrupt is not active prior to masking
1 = ssi_rxo_intr interrupt is active prior masking */
#define SPIM_F_RXOIR                                            WS_FIELD(0x0034,  3,  3) /* RO 初始值：0  */
/* Receive FIFO Underflow Raw Interrupt Status
0 = ssi_rxu_intr interrupt is not active prior to masking
1 = ssi_rxu_intr interrupt is active prior to masking */
#define SPIM_F_RXUIR                                            WS_FIELD(0x0034,  2,  2) /* RO 初始值：0  */
/* Transmit FIFO Overflow Raw Interrupt Status
0 = ssi_txo_intr interrupt is not active prior to masking
1 = ssi_txo_intr interrupt is active prior masking */
#define SPIM_F_TXOIR                                            WS_FIELD(0x0034,  1,  1) /* RO 初始值：0  */
/* Transmit FIFO Empty Raw Interrupt Status
0 = ssi_txe_intr interrupt is not active prior to masking
1 = ssi_txe_intr interrupt is active prior masking */
#define SPIM_F_TXEIR                                            WS_FIELD(0x0034,  0,  0) /* RO 初始值：0  */

/* 发送FIFO溢出中断清除寄存器  */
#define SPIM_R_TXOICR                                           WS_REG(0x0038)           /* RO 初始值：00000000 */
/* Clear Transmit FIFO Overflow Interrupt. This register reflects the status of the
interrupt. A read from this register clears the ssi_txo_intr interrupt; writing has no
effect. */
#define SPIM_F_TXOICR                                           WS_FIELD(0x0038,  0,  0) /* RO 初始值：0  */

/* 接收FIFO溢出中断清除寄存器  */
#define SPIM_R_RXOICR                                           WS_REG(0x003C)           /* RO 初始值：00000000 */
/* Clear Receive FIFO Overflow Interrupt. This register reflects the status of the interrupt.
A read from this register clears the ssi_rxo_intr interrupt; writing has no effect. */
#define SPIM_F_RXOICR                                           WS_FIELD(0x003C,  0,  0) /* RO 初始值：0  */

/* 接收FIFO向下溢出中断清除寄存器  */
#define SPIM_R_RXUICR                                           WS_REG(0x0040)           /* RO 初始值：00000000 */
/* Clear Receive FIFO Underflow Interrupt. This register reflects the status of the interrupt.
A read from this register clears the ssi_rxu_intr interrupt; writing has no effect. */
#define SPIM_F_RXUICR                                           WS_FIELD(0x0040,  0,  0) /* RO 初始值：0  */

/* 多主机中断清除寄存器  */
#define SPIM_R_MSTICR                                           WS_REG(0x0044)           /* RO 初始值：00000000 */
/* Clear Multi-Master Contention Interrupt. This register reflects the status of the interrupt.
A read from this register clears the ssi_mst_intr interrupt; writing has no effect. */
#define SPIM_F_MSTICR                                           WS_FIELD(0x0044,  0,  0) /* RO 初始值：0  */

/* 中断清除寄存器  */
#define SPIM_R_ICR                                              WS_REG(0x0048)           /* RO 初始值：00000000 */
/* Clear Interrupts. This register is set if any of the interrupts below are active. A read clears
the ssi_txo_intr, ssi_rxu_intr, ssi_rxo_intr, and the ssi_mst_intr interrupts. Writing to this
register has no effect. */
#define SPIM_F_ICR                                              WS_FIELD(0x0048,  0,  0) /* RO 初始值：0  */

/* DMA控制寄存器  */
#define SPIM_R_DMACR                                            WS_REG(0x004C)           /* RW 初始值：00000000 */
/* Transmit DMA Enable. This bit enables/disables the transmit FIFO DMA channel.
0 = Transmit DMA disabled
1 = Transmit DMA enabled */
#define SPIM_F_TDMAE                                            WS_FIELD(0x004C,  1,  1) /* RW 初始值：0  */
/* Receive DMA Enable. This bit enables/disables the receive FIFO DMA channel
0 = Receive DMA disabled
1 = Receive DMA enabled */
#define SPIM_F_RDMAE                                            WS_FIELD(0x004C,  0,  0) /* RW 初始值：0  */

/* DMA发送数据阈值寄存器  */
#define SPIM_R_DMATDLR                                          WS_REG(0x0050)           /* RW 初始值：00000000 */
/* Transmit Data Level. This bit field controls the level at which a DMA
request is made by the transmit logic. It is equal to the watermark level;
that is, the dma_tx_req signal is generated when the number of valid
data entries in the transmit FIFO is equal to or below this field value, and
TDMAE = 1. Refer to Table 6-2 for the field decode. */
#define SPIM_F_DMATDL                                           WS_FIELD(0x0050,  0,  1) /* RW 初始值：0  */

/* DMA接收阈值寄存器  */
#define SPIM_R_DMARDLR                                          WS_REG(0x0054)           /* RW 初始值：00000000 */
/* Receive Data Level. This bit field controls the level at which a DMA request
is made by the receive logic. The watermark level = DMARDL+1; that is,
dma_rx_req is generated when the number of valid data entries in the
receive FIFO is equal to or above this field value + 1, and RDMAE=1. Refer
to Table 6-7 for the field decode. */
#define SPIM_F_DMARDL                                           WS_FIELD(0x0054,  0,  1) /* RW 初始值：0  */

/* Identification Register  */
#define SPIM_R_IDR                                              WS_REG(0x0058)           /* RO 初始值：00000000 */
/* Identification Code. This register contains the peripherals identification code, which is
written into the register at configuration time using coreConsultant. */
#define SPIM_F_IDCODE                                           WS_FIELD(0x0058,  0, 31) /* RO 初始值：0  */

/* 版本寄存器  */
#define SPIM_R_SSI_COMP_VERSION                                 WS_REG(0x005C)           /* RO 初始值：00000000 */
/* Contains the hex representation of the Synopsys component version.
Consists of ASCII value for each number in the version, followed by *. For
example 32_30_31_2A represents the version 2.01*.
Reset Value: See the releases table in the AMBA 2 release notes */
#define SPIM_F_SSI_COM_VERSION                                  WS_FIELD(0x005C,  0, 31) /* RO 初始值：0  */

/* 数据寄存器  */
#define SPIM_R_DR                                               WS_REG(0x0060)           /* RW 初始值：00000000 */
/* Data Register. When writing to this register, you must right-justify the data. Read
data are automatically right-justified.
Read = Receive FIFO buffer
Write = Transmit FIFO buffer */
#define SPIM_F_DR                                               WS_FIELD(0x0060,  0, 15) /* RW 初始值：0  */

/* DR_ARR0  */
#define SPIM_R_DR_ARR0                                          WS_REG(0x0064)           /* RW 初始值：00000000 */
#define SPIM_F_ARR0                                             WS_FIELD(0x0064,  0, 31) /* RW 初始值：0  */

/* DR_ARR1  */
#define SPIM_R_DR_ARR1                                          WS_REG(0x0068)           /* RW 初始值：00000000 */
#define SPIM_F_ARR1                                             WS_FIELD(0x0068,  0, 31) /* RW 初始值：0  */

/* DR_ARR2  */
#define SPIM_R_DR_ARR2                                          WS_REG(0x006C)           /* RW 初始值：00000000 */
#define SPIM_F_ARR2                                             WS_FIELD(0x006C,  0, 31) /* RW 初始值：0  */

/* DR_ARR3  */
#define SPIM_R_DR_ARR3                                          WS_REG(0x0070)           /* RW 初始值：00000000 */
#define SPIM_F_ARR3                                             WS_FIELD(0x0070,  0, 31) /* RW 初始值：0  */

/* DR_ARR4  */
#define SPIM_R_DR_ARR4                                          WS_REG(0x0074)           /* RW 初始值：00000000 */
#define SPIM_F_ARR4                                             WS_FIELD(0x0074,  0, 31) /* RW 初始值：0  */

/* DR_ARR5  */
#define SPIM_R_DR_ARR5                                          WS_REG(0x0078)           /* RW 初始值：00000000 */
#define SPIM_F_ARR5                                             WS_FIELD(0x0078,  0, 31) /* RW 初始值：0  */

/* DR_ARR6  */
#define SPIM_R_DR_ARR6                                          WS_REG(0x007C)           /* RW 初始值：00000000 */
#define SPIM_F_ARR6                                             WS_FIELD(0x007C,  0, 31) /* RW 初始值：0  */

/* DR_ARR7  */
#define SPIM_R_DR_ARR7                                          WS_REG(0x0080)           /* RW 初始值：00000000 */
#define SPIM_F_ARR7                                             WS_FIELD(0x0080,  0, 31) /* RW 初始值：0  */

/* DR_ARR8  */
#define SPIM_R_DR_ARR8                                          WS_REG(0x0084)           /* RW 初始值：00000000 */
#define SPIM_F_ARR8                                             WS_FIELD(0x0084,  0, 31) /* RW 初始值：0  */

/* DR_ARR9  */
#define SPIM_R_DR_ARR9                                          WS_REG(0x0088)           /* RW 初始值：00000000 */
#define SPIM_F_ARR9                                             WS_FIELD(0x0088,  0, 31) /* RW 初始值：0  */

/* DR_ARR10  */
#define SPIM_R_DR_ARR10                                         WS_REG(0x008C)           /* RW 初始值：00000000 */
#define SPIM_F_ARR10                                            WS_FIELD(0x008C,  0, 31) /* RW 初始值：0  */

/* DR_ARR11  */
#define SPIM_R_DR_ARR11                                         WS_REG(0x0090)           /* RW 初始值：00000000 */
#define SPIM_F_ARR11                                            WS_FIELD(0x0090,  0, 31) /* RW 初始值：0  */

/* DR_ARR12  */
#define SPIM_R_DR_ARR12                                         WS_REG(0x0094)           /* RW 初始值：00000000 */
#define SPIM_F_ARR12                                            WS_FIELD(0x0094,  0, 31) /* RW 初始值：0  */

/* DR_ARR13  */
#define SPIM_R_DR_ARR13                                         WS_REG(0x0098)           /* RW 初始值：00000000 */
#define SPIM_F_ARR13                                            WS_FIELD(0x0098,  0, 31) /* RW 初始值：0  */

/* DR_ARR14  */
#define SPIM_R_DR_ARR14                                         WS_REG(0x009C)           /* RW 初始值：00000000 */
#define SPIM_F_ARR14                                            WS_FIELD(0x009C,  0, 31) /* RW 初始值：0  */

/* DR_ARR15  */
#define SPIM_R_DR_ARR15                                         WS_REG(0x00A0)           /* RW 初始值：00000000 */
#define SPIM_F_ARR15                                            WS_FIELD(0x00A0,  0, 31) /* RW 初始值：0  */

/* DR_ARR16  */
#define SPIM_R_DR_ARR16                                         WS_REG(0x00A4)           /* RW 初始值：00000000 */
#define SPIM_F_ARR16                                            WS_FIELD(0x00A4,  0, 31) /* RW 初始值：0  */

/* DR_ARR17  */
#define SPIM_R_DR_ARR17                                         WS_REG(0x00A8)           /* RW 初始值：00000000 */
#define SPIM_F_ARR17                                            WS_FIELD(0x00A8,  0, 31) /* RW 初始值：0  */

/* DR_ARR18  */
#define SPIM_R_DR_ARR18                                         WS_REG(0x00AC)           /* RW 初始值：00000000 */
#define SPIM_F_ARR18                                            WS_FIELD(0x00AC,  0, 31) /* RW 初始值：0  */

/* DR_ARR19  */
#define SPIM_R_DR_ARR19                                         WS_REG(0x00B0)           /* RW 初始值：00000000 */
#define SPIM_F_ARR19                                            WS_FIELD(0x00B0,  0, 31) /* RW 初始值：0  */

/* DR_ARR20  */
#define SPIM_R_DR_ARR20                                         WS_REG(0x00B4)           /* RW 初始值：00000000 */
#define SPIM_F_ARR20                                            WS_FIELD(0x00B4,  0, 31) /* RW 初始值：0  */

/* DR_ARR21  */
#define SPIM_R_DR_ARR21                                         WS_REG(0x00B8)           /* RW 初始值：00000000 */
#define SPIM_F_ARR21                                            WS_FIELD(0x00B8,  0, 31) /* RW 初始值：0  */

/* DR_ARR22  */
#define SPIM_R_DR_ARR22                                         WS_REG(0x00BC)           /* RW 初始值：00000000 */
#define SPIM_F_ARR22                                            WS_FIELD(0x00BC,  0, 31) /* RW 初始值：0  */

/* DR_ARR23  */
#define SPIM_R_DR_ARR23                                         WS_REG(0x00C0)           /* RW 初始值：00000000 */
#define SPIM_F_ARR23                                            WS_FIELD(0x00C0,  0, 31) /* RW 初始值：0  */

/* DR_ARR24  */
#define SPIM_R_DR_ARR24                                         WS_REG(0x00C4)           /* RW 初始值：00000000 */
#define SPIM_F_ARR24                                            WS_FIELD(0x00C4,  0, 31) /* RW 初始值：0  */

/* DR_ARR25  */
#define SPIM_R_DR_ARR25                                         WS_REG(0x00C8)           /* RW 初始值：00000000 */
#define SPIM_F_ARR25                                            WS_FIELD(0x00C8,  0, 31) /* RW 初始值：0  */

/* DR_ARR26  */
#define SPIM_R_DR_ARR26                                         WS_REG(0x00CC)           /* RW 初始值：00000000 */
#define SPIM_F_ARR26                                            WS_FIELD(0x00CC,  0, 31) /* RW 初始值：0  */

/* DR_ARR27  */
#define SPIM_R_DR_ARR27                                         WS_REG(0x00D0)           /* RW 初始值：00000000 */
#define SPIM_F_ARR27                                            WS_FIELD(0x00D0,  0, 31) /* RW 初始值：0  */

/* DR_ARR28  */
#define SPIM_R_DR_ARR28                                         WS_REG(0x00D4)           /* RW 初始值：00000000 */
#define SPIM_F_ARR28                                            WS_FIELD(0x00D4,  0, 31) /* RW 初始值：0  */

/* DR_ARR29  */
#define SPIM_R_DR_ARR29                                         WS_REG(0x00D8)           /* RW 初始值：00000000 */
#define SPIM_F_ARR29                                            WS_FIELD(0x00D8,  0, 31) /* RW 初始值：0  */

/* DR_ARR30  */
#define SPIM_R_DR_ARR30                                         WS_REG(0x00DC)           /* RW 初始值：00000000 */
#define SPIM_F_ARR30                                            WS_FIELD(0x00DC,  0, 31) /* RW 初始值：0  */

/* DR_ARR31  */
#define SPIM_R_DR_ARR31                                         WS_REG(0x00E0)           /* RW 初始值：00000000 */
#define SPIM_F_ARR31                                            WS_FIELD(0x00E0,  0, 31) /* RW 初始值：0  */

/* DR_ARR32  */
#define SPIM_R_DR_ARR32                                         WS_REG(0x00E4)           /* RW 初始值：00000000 */
#define SPIM_F_ARR32                                            WS_FIELD(0x00E4,  0, 31) /* RW 初始值：0  */

/* DR_ARR33  */
#define SPIM_R_DR_ARR33                                         WS_REG(0x00E8)           /* RW 初始值：00000000 */
#define SPIM_F_ARR33                                            WS_FIELD(0x00E8,  0, 31) /* RW 初始值：0  */

/* DR_ARR34  */
#define SPIM_R_DR_ARR34                                         WS_REG(0x00EC)           /* RW 初始值：00000000 */
#define SPIM_F_ARR34                                            WS_FIELD(0x00EC,  0, 31) /* RW 初始值：0  */

/* 接收采样延迟寄存器  */
#define SPIM_R_RX_SAMPLE_DLY                                    WS_REG(0x00F0)           /* RW 初始值：00000000 */
/* Receive Data (rxd) Sample Delay. This register is used to delay the sample of the rxd
input signal. Each value represents a single ssi_clk delay on the sample of the rxd
signal. */
#define SPIM_F_RSD                                              WS_FIELD(0x00F0,  0,  7) /* RW 初始值：0  */

/*
 * SYSCFG registers and fields
 */
/* 高速时钟源选择寄存器 （Retention） */
#define SYSCFG_R_HFCLK_SEL                                      WS_REG(0x0000)           /* RW 初始值：00000100 */
/* 外部高速晶体振荡器使能
0: 关闭； 1：开启
注意：只有当
HFCLK_SEL.rc16m_en=1和HFCLK_SEL.clk16m_sel=0时，才能关闭外部高速晶体振荡器 */
#define SYSCFG_F_XTAL16M_EN                                     WS_FIELD(0x0000,  9,  9) /* RW 初始值：1 */
/* Select nominal frequency of external crystal for HFCLK. This register bit has to match the actual crystal used in design to enable correct behavior.
0: 16Mhz crystal is used
1: 32Mhz crystal is used  */
#define SYSCFG_F_XTAL_FREQ                                      WS_FIELD(0x0000,  1,  1) /* RW 初始值：0 */

/* 低速时钟源选择寄存器 （Retention） */
#define SYSCFG_R_LFCLK_SEL                                      WS_REG(0x0004)           /* RW 初始值：00000100 */
/* 内部32kHz RC振荡器使能
0: 关闭； 1：开启 */
#define SYSCFG_F_RC32K_EN                                       WS_FIELD(0x0004,  8,  8) /* RW 初始值：1 */

/* 时钟频率选择寄存器 （Retention） */
#define SYSCFG_R_FREQ_SEL                                       WS_REG(0x0008)           /* RW 初始值：00000000 */
/* PCLK分频值（基于HCLK）：
00：PCLK不对输入时钟分频；
01：PCLK对输入时钟2分频；
10：PCLK对输入时钟4分频 */
#define SYSCFG_F_PCLK_DIV_VAL                                   WS_FIELD(0x0008,  2,  3) /* RW 初始值：0 */
/* HCLK分频值（基于系统时钟）
00：HCLK不对输入时钟分频；
01：HCLK对输入时钟2分频；
10：PCLK对输入时钟4分频 */
#define SYSCFG_F_HCLK_DIV_VAL                                   WS_FIELD(0x0008,  0,  1) /* RW 初始值：0 */

/* 时钟校准控制存器 （Retention） */
#define SYSCFG_R_CALIB_EN                                       WS_REG(0x000C)           /* RW 初始值：00000000 */
/* 1：内部32kHz RC振荡器时钟校准使能，当校准完成时，自动清零。
校准之前请置位xtal16m_en和clk16m_sel */
#define SYSCFG_F_CAL32K_EN                                      WS_FIELD(0x000C,  0,  0) /* RW 初始值：0 */

/* 时钟校准值寄存器 （Retention） */
#define SYSCFG_R_CALIB_VAL                                      WS_REG(0x0010)           /* RW 初始值：7ffc0820 */
/* 内部32kHz RC振荡器时钟校准值 */
#define SYSCFG_F_CAL32K_VAL                                     WS_FIELD(0x0010,  1,  5) /* RW 初始值：10 */

/* 时钟校准值寄存器  */
#define SYSCFG_R_CNT32K_VAL                                     WS_REG(0x0014)           /* RW 初始值：00000000 */
/* xt */
#define SYSCFG_F_INT_CNT32K                                     WS_FIELD(0x0014, 29, 29) /* RW 初始值：0 */
/* 计数完成中断使能 */
#define SYSCFG_F_INT_CNT32K_EN                                  WS_FIELD(0x0014, 28, 28) /* RW 初始值：0 */
/* 1：32K时钟计数使能，当计数完成时，自动清零。
计数之前请置位xtal16m_en和clk16m_sel */
#define SYSCFG_F_CNT32K_EN                                      WS_FIELD(0x0014, 27, 27) /* RW 初始值：0 */
/* 32K时钟计数周期数
000: 32 (约1ms)
001: 64 (约2ms)
010: 128 (约4ms)
011: 256 (约8ms)
100: 512 (约16ms)
101: 1024 (约32ms)
110: 2048 (约64ms)
111: 2048 (约64ms) */
#define SYSCFG_F_CNT32K_VAL                                     WS_FIELD(0x0014, 24, 26) /* RW 初始值：0 */
/* 16M时钟计数周期数 */
#define SYSCFG_F_CNT16M_VAL                                     WS_FIELD(0x0014,  0, 20) /* RO 初始值：0 */

/* 系统休眠模式配置寄存器 （Retention） */
#define SYSCFG_R_SLEEP_CTRL                                     WS_REG(0x0020)           /* RW 初始值：00000000 */
/* JTAG TMS信号唤醒类型
0：低电平唤醒
1：高电平唤醒 */
#define SYSCFG_F_JTAG_WKUP_TYPE                                 WS_FIELD(0x0020, 13, 13) /* RW 初始值：0 */
/* JTAG系统休眠唤醒使能 */
#define SYSCFG_F_JTAG_WK_EN                                     WS_FIELD(0x0020, 12, 12) /* RW 初始值：0 */
/* GPIO系统休眠唤醒使能 */
#define SYSCFG_F_GPIO_WK_EN                                     WS_FIELD(0x0020, 10, 10) /* RW 初始值：0 */
/* RTC中断系统休眠唤醒使能 */
#define SYSCFG_F_RTC_WK_EN                                      WS_FIELD(0x0020,  8,  8) /* RW 初始值：0 */
/* 当系统进入休眠时，选择休眠模式的深度：
00：关闭高速时钟振荡器
01：关闭高速时钟振荡器，并且关闭外设供电。
10：关闭高速时钟振荡器，并且关闭外设及CPU供电。 */
#define SYSCFG_F_DEEP_SLP_SEL                                   WS_FIELD(0x0020,  0,  1) /* RW 初始值：0 */

/* 系统休眠唤醒类型配置寄存器 （Retention） */
#define SYSCFG_R_WKUP_SRC_TYPE_A                                WS_REG(0x0024)           /* RW 初始值：00000000 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP7_TYPE                                WS_FIELD(0x0024, 28, 30) /* RW 初始值：0 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP6_TYPE                                WS_FIELD(0x0024, 24, 26) /* RW 初始值：0 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP5_TYPE                                WS_FIELD(0x0024, 20, 22) /* RW 初始值：0 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP4_TYPE                                WS_FIELD(0x0024, 16, 18) /* RW 初始值：0 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP3_TYPE                                WS_FIELD(0x0024, 12, 14) /* RW 初始值：0 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP2_TYPE                                WS_FIELD(0x0024,  8, 10) /* RW 初始值：0 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP1_TYPE                                WS_FIELD(0x0024,  4,  6) /* RW 初始值：0 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP0_TYPE                                WS_FIELD(0x0024,  0,  2) /* RW 初始值：0 */

/* 系统休眠唤醒源配置寄存器 （Retention） */
#define SYSCFG_R_WKUP_SRC_TYPE_B                                WS_REG(0x0028)           /* RW 初始值：00000000 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP15_TYPE                               WS_FIELD(0x0028, 28, 30) /* RW 初始值：0 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP14_TYPE                               WS_FIELD(0x0028, 24, 26) /* RW 初始值：0 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP13_TYPE                               WS_FIELD(0x0028, 20, 22) /* RW 初始值：0 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP12_TYPE                               WS_FIELD(0x0028, 16, 18) /* RW 初始值：0 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP11_TYPE                               WS_FIELD(0x0028, 12, 14) /* RW 初始值：0 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP10_TYPE                               WS_FIELD(0x0028,  8, 10) /* RW 初始值：0 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP9_TYPE                                WS_FIELD(0x0028,  4,  6) /* RW 初始值：0 */
/* GPIO系统休眠唤醒源3类型及使能控制位：
000 ：不唤醒；
001 ：高电平唤醒；
010 ：高电平唤醒（过滤1个32K时钟周期毛刺）011 : 低电平唤醒；
100 ：低电平唤醒（过滤1个32K时钟周期毛刺）
101 ：上升沿唤醒
110 ：下降沿唤醒
111 : 双沿唤醒
注意：沿唤醒是32K采样，对IO宽度有要求。 */
#define SYSCFG_F_GPIO_WKUP8_TYPE                                WS_FIELD(0x0028,  0,  2) /* RW 初始值：0 */

/* MCU休眠唤醒配置寄存器  */
#define SYSCFG_R_WAKE_MCU_CTRL                                  WS_REG(0x0040)           /* RW 初始值：00040000 */
/* 所有Exception，External Interrupt都能唤醒MCU */
#define SYSCFG_F_ALL_WK_EN                                      WS_FIELD(0x0040, 18, 18) /* RW 初始值：1 */
/* QDEC中断唤醒MCU使能 */
#define SYSCFG_F_QDEC_WK_MCU                                    WS_FIELD(0x0040, 16, 16) /* RW 初始值：0 */
/* GPIO中断0唤醒MCU使能 */
#define SYSCFG_F_GPIO_INT0_WK_MCU                               WS_FIELD(0x0040, 11, 11) /* RW 初始值：0 */
/* RTC中断唤醒MCU使能 */
#define SYSCFG_F_RTC_WK_MCU                                     WS_FIELD(0x0040, 10, 10) /* RW 初始值：0 */
/* Timer1中断唤醒MCU使能 */
#define SYSCFG_F_TIMER1_WK_MCU                                  WS_FIELD(0x0040,  7,  7) /* RW 初始值：0 */
/* Timer0中断唤醒MCU使能 */
#define SYSCFG_F_TIMER0_WK_MCU                                  WS_FIELD(0x0040,  6,  6) /* RW 初始值：0 */
/* SPI0中断唤醒MCU使能 */
#define SYSCFG_F_SPI0_WK_MCU                                    WS_FIELD(0x0040,  3,  3) /* RW 初始值：0 */
/* UART0中断唤醒MCU使能 */
#define SYSCFG_F_UART0_WK_MCU                                   WS_FIELD(0x0040,  1,  1) /* RW 初始值：0 */

/* Timer挂起控制寄存器  */
#define SYSCFG_R_TIMER_PAUSE                                    WS_REG(0x0060)           /* RW 初始值：00000000 */
/* Timer1挂起使能：
0 ：不挂起；
1 ：挂起 */
#define SYSCFG_F_TIMER1_PAUSE                                   WS_FIELD(0x0060,  1,  1) /* RW 初始值：0 */
/* Timer0挂起使能：
0 ：不挂起；
1 ：挂起 */
#define SYSCFG_F_TIMER0_PAUSE                                   WS_FIELD(0x0060,  0,  0) /* RW 初始值：0 */

/* 进休眠时Basetimer当前值 （Retention） */
#define SYSCFG_R_RTC_DIV_VAL                                    WS_REG(0x0070)           /* RW 初始值：00000000 */
/* 进休眠时刻，BaseTimer的计数值
bstim_sample_en配置为1开启该功能 */
#define SYSCFG_F_BSTIM_CUR_VAL                                  WS_FIELD(0x0070,  0, 31) /* RW 初始值：0 */

/* 进休眠时RTC当前值 （Retention） */
#define SYSCFG_R_RTC_DIV_CNT                                    WS_REG(0x0074)           /* RO 初始值：00000000 */
/* 进休眠时刻，RTC的计数值
bstim_sample_en配置为1开启该功能 */
#define SYSCFG_F_RTC_CUR_VAL                                    WS_FIELD(0x0074,  0, 31) /* RO 初始值：0 */

#define SYSCFG_F_BSTIM_SAMPLE_EN                                    WS_FIELD(0x00C4,  27, 27) /* RO 初始值：0 */


/*
 * TIMER registers and fields
 */
/* 控制使能寄存器  */
#define TIMER_R_TIMER_EN                                        WS_REG(0x0000)           /* RW 初始值：0000 */
/* PWM5极性 
0：初始状态为低电平； 
1：初始状态为高电平  */
#define TIMER_F_TIMER5_TOG_POL                                  WS_FIELD(0x0000, 13, 13) /* RW 初始值：0 */
/* PWM4极性 
0：初始状态为低电平； 
1：初始状态为高电平  */
#define TIMER_F_TIMER4_TOG_POL                                  WS_FIELD(0x0000, 12, 12) /* RW 初始值：0 */
/* PWM3极性 
0：初始状态为低电平； 
1：初始状态为高电平  
 */
#define TIMER_F_TIMER3_TOG_POL                                  WS_FIELD(0x0000, 11, 11) /* RW 初始值：0 */
/* PWM2极性 
0：初始状态为低电平； 
1：初始状态为高电平  */
#define TIMER_F_TIMER2_TOG_POL                                  WS_FIELD(0x0000, 10, 10) /* RW 初始值：0 */
/* PWM1极性 
0：初始状态为低电平； 
1：初始状态为高电平  */
#define TIMER_F_TIMER1_TOG_POL                                  WS_FIELD(0x0000,  9,  9) /* RW 初始值：0 */
/* PWM0极性 
0：初始状态为低电平； 
1：初始状态为高电平  */
#define TIMER_F_TIMER0_TOG_POL                                  WS_FIELD(0x0000,  8,  8) /* RW 初始值：0 */
/* 使能32位计数器5 
0：禁止计数器； 
1：使能计数器。 
禁止计数器后计数器保持上次计数不变 */
#define TIMER_F_TIMER5_EN                                       WS_FIELD(0x0000,  5,  5) /* RW 初始值：0 */
/* 使能32位计数器4 
0：禁止计数器； 
1：使能计数器。 
禁止计数器后计数器保持上次计数不变 */
#define TIMER_F_TIMER4_EN                                       WS_FIELD(0x0000,  4,  4) /* RW 初始值：0 */
/* 使能32位计数器3
0：禁止计数器； 
1：使能计数器。 
禁止计数器后计数器保持上次计数不变 */
#define TIMER_F_TIMER3_EN                                       WS_FIELD(0x0000,  3,  3) /* RW 初始值：0 */
/* 使能32位计数器2 
0：禁止计数器； 
1：使能计数器。 
禁止计数器后计数器保持上次计数不变 */
#define TIMER_F_TIMER2_EN                                       WS_FIELD(0x0000,  2,  2) /* RW 初始值：0 */
/* 使能32位计数器1  
0：禁止计数器； 
1：使能计数器。 
禁止计数器后计数器保持上次计数不变 */
#define TIMER_F_TIMER1_EN                                       WS_FIELD(0x0000,  1,  1) /* RW 初始值：0 */
/* 使能32位计数器0 
0：禁止计数器； 
1：使能计数器。 
禁止计数器后计数器保持上次计数不变 */
#define TIMER_F_TIMER0_EN                                       WS_FIELD(0x0000,  0,  0) /* RW 初始值：0 */

/* 更新寄存器  */
#define TIMER_R_TIMER_UPDATE_EN                                 WS_REG(0x0004)           /* RW 初始值：0000 */
/* 32位计数器5预设值重新装载使能。软件置位，硬件清零。
当Timer使能时刻或者计数器减到0时刻自动装载LoadCounter和ToggleSetValue到内部寄存器
 */
#define TIMER_F_TIMER5_UPDATAE_EN                               WS_FIELD(0x0004,  5,  5) /* RW 初始值：0 */
/* 32位计数器4预设值重新装载使能。软件置位，硬件清零。
当Timer使能时刻或者计数器减到0时刻自动装载LoadCounter和ToggleSetValue到内部寄存器
 */
#define TIMER_F_TIMER4_UPDATAE_EN                               WS_FIELD(0x0004,  4,  4) /* RW 初始值：0 */
/* 32位计数器3预设值重新装载使能。软件置位，硬件清零。
当Timer使能时刻或者计数器减到0时刻自动装载LoadCounter和ToggleSetValue到内部寄存器
 */
#define TIMER_F_TIMER3_UPDATAE_EN                               WS_FIELD(0x0004,  3,  3) /* RW 初始值：0 */
/* 32位计数器2预设值重新装载使能。软件置位，硬件清零。
当Timer使能时刻或者计数器减到0时刻自动装载LoadCounter和ToggleSetValue到内部寄存器
 */
#define TIMER_F_TIMER2_UPDATAE_EN                               WS_FIELD(0x0004,  2,  2) /* RW 初始值：0 */
/* 32位计数器1预设值重新装载使能。软件置位，硬件清零。
当Timer使能时刻或者计数器减到0时刻自动装载LoadCounter和ToggleSetValue到内部寄存器
 */
#define TIMER_F_TIMER1_UPDATAE_EN                               WS_FIELD(0x0004,  1,  1) /* RW 初始值：0 */
/* 32位计数器0预设值重新装载使能。软件置位，硬件清零。
当Timer使能时刻或者计数器减到0时刻自动装载LoadCounter和ToggleSetValue到内部寄存器
 */
#define TIMER_F_TIMER0_UPDATAE_EN                               WS_FIELD(0x0004,  0,  0) /* RW 初始值：0 */

/* 中断使能寄存器  */
#define TIMER_R_TIMER_INT_EN                                    WS_REG(0x0008)           /* RW 初始值：0000 */
/* 32位计数器5产生中断时间选择
0：计数到0
1：计数到toggle_set_val */
#define TIMER_F_TIMER5_INT_CFG                                  WS_FIELD(0x0008, 13, 13) /* RW 初始值：0 */
/* 32位计数器4产生中断时间选择
0：计数到0
1：计数到toggle_set_val */
#define TIMER_F_TIMER4_INT_CFG                                  WS_FIELD(0x0008, 12, 12) /* RW 初始值：0 */
/* 32位计数器3产生中断时间选择
0：计数到0
1：计数到toggle_set_val */
#define TIMER_F_TIMER3_INT_CFG                                  WS_FIELD(0x0008, 11, 11) /* RW 初始值：0 */
/* 32位计数器2产生中断时间选择
0：计数到0
1：计数到toggle_set_val */
#define TIMER_F_TIMER2_INT_CFG                                  WS_FIELD(0x0008, 10, 10) /* RW 初始值：0 */
/* 32位计数器1产生中断时间选择
0：计数到0
1：计数到toggle_set_val */
#define TIMER_F_TIMER1_INT_CFG                                  WS_FIELD(0x0008,  9,  9) /* RW 初始值：0 */
/* 32位计数器0产生中断时间选择
0：计数到0
1：计数到toggle_set_val */
#define TIMER_F_TIMER0_INT_CFG                                  WS_FIELD(0x0008,  8,  8) /* RW 初始值：0 */
/* 32位计数器5是否产生中断
0：不使能
1：使能 */
#define TIMER_F_TIMER5_INT_EN                                   WS_FIELD(0x0008,  5,  5) /* RW 初始值：0 */
/* 32位计数器4是否产生中断
0：不使能
1：使能 */
#define TIMER_F_TIMER4_INT_EN                                   WS_FIELD(0x0008,  4,  4) /* RW 初始值：0 */
/* 32位计数器3是否产生中断
0：不使能
1：使能 */
#define TIMER_F_TIMER3_INT_EN                                   WS_FIELD(0x0008,  3,  3) /* RW 初始值：0 */
/* 32位计数器2是否产生中断
0：不使能
1：使能 */
#define TIMER_F_TIMER2_INT_EN                                   WS_FIELD(0x0008,  2,  2) /* RW 初始值：0 */
/* 32位计数器1是否产生中断
0：不使能
1：使能 */
#define TIMER_F_TIMER1_INT_EN                                   WS_FIELD(0x0008,  1,  1) /* RW 初始值：0 */
/* 32位计数器0是否产生中断
0：不使能
1：使能 */
#define TIMER_F_TIMER0_INT_EN                                   WS_FIELD(0x0008,  0,  0) /* RW 初始值：0 */

/* 中断状态寄存器  */
#define TIMER_R_TIMER_INT_STATE                                 WS_REG(0x000C)           /* RW 初始值：0000 */
/* 32位计数器5中断状态
写0清除 */
#define TIMER_F_TIMER5_INT                                      WS_FIELD(0x000C,  5,  5) /* RW 初始值：0 */
/* 32位计数器4中断状态
写0清除 */
#define TIMER_F_TIMER4_INT                                      WS_FIELD(0x000C,  4,  4) /* RW 初始值：0 */
/* 32位计数器3中断状态
写0清除 */
#define TIMER_F_TIMER3_INT                                      WS_FIELD(0x000C,  3,  3) /* RW 初始值：0 */
/* 32位计数器2中断状态
写0清除 */
#define TIMER_F_TIMER2_INT                                      WS_FIELD(0x000C,  2,  2) /* RW 初始值：0 */
/* 32位计数器1中断状态
写0清除 */
#define TIMER_F_TIMER1_INT                                      WS_FIELD(0x000C,  1,  1) /* RW 初始值：0 */
/* 32位计数器0中断状态
写0清除 */
#define TIMER_F_TIMER0_INT                                      WS_FIELD(0x000C,  0,  0) /* RW 初始值：0 */

/* TIMER0计数值装载寄存器  */
#define TIMER_R_TIMER0_LOADCOUNTER                              WS_REG(0x0010)           /* RW 初始值：00000000 */
/* 计数器0 Counter装载值 */
#define TIMER_F_TIMER0_RELOAD_VAL                               WS_FIELD(0x0010, 16, 31) /* RW 初始值：0 */
/* 计数器0预分频值
 */
#define TIMER_F_TIMER0_PRESCALER_VAL                            WS_FIELD(0x0010,  0, 15) /* RW 初始值：0 */

/* TIMER0 TOGGLE值装载寄存器  */
#define TIMER_R_TIMER0_TOGSET                                   WS_REG(0x0014)           /* RW 初始值：00000000 */
/* Timer0 PWM电平翻转比较值
当计数器0 Counter自减到等于toggle_set_val时PWM电平翻转，
当计数器0 Counter自减到等于0时PWM电平再次翻转

 */
#define TIMER_F_TIMER0_TOGGLE_VAL                               WS_FIELD(0x0014,  0, 15) /* RW 初始值：0 */

/* TIMER0当前值寄存器  */
#define TIMER_R_TIMER0_CURRENT_VAL                              WS_REG(0x0018)           /* RW 初始值：00000000 */
/* 32位计数器当前值 */
#define TIMER_F_TIMER0_CURRENT_VAL                              WS_FIELD(0x0018,  0, 31) /* RO 初始值：0 */

/* TIMER1计数值装载寄存器  */
#define TIMER_R_TIMER1_LOADCOUNTER                              WS_REG(0x0020)           /* RW 初始值：00000000 */
/* 计数器1 Counter装载值 */
#define TIMER_F_TIMER1_RELOAD_VAL                               WS_FIELD(0x0020, 16, 31) /* RW 初始值：0 */
/* 计数器1预分频值
 */
#define TIMER_F_TIMER1_PRESCALER_VAL                            WS_FIELD(0x0020,  0, 15) /* RW 初始值：0 */

/* TIMER1 TOGGLE值装载寄存器  */
#define TIMER_R_TIMER1_TOGSET                                   WS_REG(0x0024)           /* RW 初始值：00000000 */
/* Timer1 PWM电平翻转比较值
当定时计数器1 Counter自减到等于toggle_set_val时PWM电平翻转，
当计数器1 Counter自减到等于0时PWM电平再次翻转

 */
#define TIMER_F_TIMER1_TOGGLE_VAL                               WS_FIELD(0x0024,  0, 15) /* RW 初始值：0 */

/* TIMER1当前值寄存器  */
#define TIMER_R_TIMER1_CURRENT_VAL                              WS_REG(0x0028)           /* RW 初始值：00000000 */
/* 32位计数器2当前值 */
#define TIMER_F_TIMER1_CURRENT_VAL                              WS_FIELD(0x0028,  0, 31) /* RO 初始值：0 */

/* TIMER2计数值装载寄存器  */
#define TIMER_R_TIMER2_LOADCOUNTER                              WS_REG(0x0030)           /* RW 初始值：00000000 */
/* 计数器2 Counter装载值 */
#define TIMER_F_TIMER2_RELOAD_VAL                               WS_FIELD(0x0030, 16, 31) /* RW 初始值：0 */
/* 计数器2预分频值
 */
#define TIMER_F_TIMER2_PRESCALER_VAL                            WS_FIELD(0x0030,  0, 15) /* RW 初始值：0 */

/* TIMER2 TOGGLE值装载寄存器  */
#define TIMER_R_TIMER2_TOGSET                                   WS_REG(0x0034)           /* RW 初始值：00000000 */
/* Timer2 PWM电平翻转比较值
当定时计数器2 Counter自减到等于toggle_set_val时PWM电平翻转，
当计数器2 Counter自减到等于0时PWM电平再次翻转

 */
#define TIMER_F_TIMER2_TOGGLE_VAL                               WS_FIELD(0x0034,  0, 15) /* RW 初始值：0 */

/* TIMER2当前值寄存器  */
#define TIMER_R_TIMER2_CURRENT_VAL                              WS_REG(0x0038)           /* RW 初始值：00000000 */
/* 32位计数器3当前值 */
#define TIMER_F_TIMER2_CURRENT_VAL                              WS_FIELD(0x0038,  0, 31) /* RO 初始值：0 */

/* TIMER3计数值装载寄存器  */
#define TIMER_R_TIMER3_LOADCOUNTER                              WS_REG(0x0040)           /* RW 初始值：00000000 */
/* 计数器3 Counter装载值 */
#define TIMER_F_TIMER3_RELOAD_VAL                               WS_FIELD(0x0040, 16, 31) /* RW 初始值：0 */
/* 计数器3预分频值
 */
#define TIMER_F_TIMER3_PRESCALER_VAL                            WS_FIELD(0x0040,  0, 15) /* RW 初始值：0 */

/* TIMER3 TOGGLE值装载寄存器  */
#define TIMER_R_TIMER3_TOGSET                                   WS_REG(0x0044)           /* RW 初始值：00000000 */
/* Timer3 PWM电平翻转比较值
当定时计数器3 Counter自减到等于toggle_set_val时PWM电平翻转，
当计数器3 Counter自减到等于0时PWM电平再次翻转

 */
#define TIMER_F_TIMER3_TOGGLE_VAL                               WS_FIELD(0x0044,  0, 15) /* RW 初始值：0 */

/* TIMER3当前值寄存器  */
#define TIMER_R_TIMER3_CURRENT_VAL                              WS_REG(0x0048)           /* RW 初始值：00000000 */
/* 32位计数器3当前值 */
#define TIMER_F_TIMER3_CURRENT_VAL                              WS_FIELD(0x0048,  0, 31) /* RO 初始值：0 */

/* TIMER4计数值装载寄存器  */
#define TIMER_R_TIMER4_LOADCOUNTER                              WS_REG(0x0050)           /* RW 初始值：00000000 */
/* 计数器4 Counter装载值 */
#define TIMER_F_TIMER4_RELOAD_VAL                               WS_FIELD(0x0050, 16, 31) /* RW 初始值：0 */
/* 计数器4预分频值
 */
#define TIMER_F_TIMER4_PRESCALER_VAL                            WS_FIELD(0x0050,  0, 15) /* RW 初始值：0 */

/* TIMER4 TOGGLE值装载寄存器  */
#define TIMER_R_TIMER4_TOGSET                                   WS_REG(0x0054)           /* RW 初始值：00000000 */
/* Timer4 PWM电平翻转比较值
当定时计数器4 Counter自减到等于toggle_set_val时PWM电平翻转，
当计数器4 Counter自减到等于0时PWM电平再次翻转

 */
#define TIMER_F_TIMER4_TOGGLE_VAL                               WS_FIELD(0x0054,  0, 15) /* RW 初始值：0 */

/* TIMER4当前值寄存器  */
#define TIMER_R_TIMER4_CURRENT_VAL                              WS_REG(0x0058)           /* RW 初始值：00000000 */
/* 32位计数器4当前值 */
#define TIMER_F_TIMER4_CURRENT_VAL                              WS_FIELD(0x0058,  0, 31) /* RO 初始值：0 */

/* TIMER5计数值装载寄存器  */
#define TIMER_R_TIMER5_LOADCOUNTER                              WS_REG(0x0060)           /* RW 初始值：00000000 */
/* 计数器5 Counter装载值 */
#define TIMER_F_TIMER5_RELOAD_VAL                               WS_FIELD(0x0060, 16, 31) /* RW 初始值：0 */
/* 计数器5预分频值
 */
#define TIMER_F_TIMER5_PRESCALER_VAL                            WS_FIELD(0x0060,  0, 15) /* RW 初始值：0 */

/* TIMER5 TOGGLE值装载寄存器  */
#define TIMER_R_TIMER5_TOGSET                                   WS_REG(0x0064)           /* RW 初始值：00000000 */
/* Timer5 PWM电平翻转比较值
当定时计数器5 Counter自减到等于toggle_set_val时PWM电平翻转，
当计数器5 Counter自减到等于0时PWM电平再次翻转

 */
#define TIMER_F_TIMER5_TOGGLE_VAL                               WS_FIELD(0x0064,  0, 15) /* RW 初始值：0 */

/* TIMER5当前值寄存器  */
#define TIMER_R_TIMER5_CURRENT_VAL                              WS_REG(0x0068)           /* RW 初始值：00000000 */
/* 32位计数器5当前值 */
#define TIMER_F_TIMER5_CURRENT_VAL                              WS_FIELD(0x0068,  0, 31) /* RO 初始值：0 */

/*
 * UART registers and fields
 */
/* 数据寄存器  */
#define UART_R_DR                                               WS_REG(0x0000)           /* RW 初始值：00000000 */
/* 溢出错误 */
#define UART_F_OE                                               WS_FIELD(0x0000, 11, 11) /* RO 初始值：0 */
/* break错误 */
#define UART_F_BE                                               WS_FIELD(0x0000, 10, 10) /* RO 初始值：0 */
/* 校验错误 */
#define UART_F_PE                                               WS_FIELD(0x0000,  9,  9) /* RO 初始值：0 */
/* 帧错误 */
#define UART_F_FE                                               WS_FIELD(0x0000,  8,  8) /* RO 初始值：0 */
/* 接收/发送数据 */
#define UART_F_DATA                                             WS_FIELD(0x0000,  0,  7) /* RW 初始值：0 */

/* 接收状态错误清除寄存器  */
#define UART_R_RSR_ECR                                          WS_REG(0x0004)           /* RW 初始值：00000000 */
/* 清除溢出错误 */
#define UART_F_CLR_OE                                           WS_FIELD(0x0004,  3,  3) /* RW 初始值：0 */
/* 清除break错误 */
#define UART_F_CLR_BE                                           WS_FIELD(0x0004,  2,  2) /* RW 初始值：0 */
/* 清除校验错误 */
#define UART_F_CLR_PE                                           WS_FIELD(0x0004,  1,  1) /* RW 初始值：0 */
/* 清除帧错误 */
#define UART_F_CLR_FE                                           WS_FIELD(0x0004,  0,  0) /* RW 初始值：0 */

/* 接收超时  */
#define UART_R_TIMEOUT                                          WS_REG(0x000C)           /* RW 初始值：000001FF */
/* 超时周期配置 */
#define UART_F_TIMEOUT_PERIOD                                   WS_FIELD(0x000C,  0, 21) /* RW 初始值：0x1FF */

/* modem配置寄存器5  */
#define UART_R_FR                                               WS_REG(0x0018)           /* RW 初始值：0184300 */
/* Remote transmitter disabled */
#define UART_F_RTXDIS                                           WS_FIELD(0x0018, 13, 13) /* RO 初始值：0  */
/* Trailing edge ring indicator */
#define UART_F_TERI                                             WS_FIELD(0x0018, 12, 12) /* RO 初始值：0  */
/* Delta data carrier detect */
#define UART_F_DDCD                                             WS_FIELD(0x0018, 11, 11) /* RO 初始值：0  */
/* Delta data set ready */
#define UART_F_DDSR                                             WS_FIELD(0x0018, 10, 10) /* RO 初始值：0  */
/* Delta clear to send */
#define UART_F_DCTS                                             WS_FIELD(0x0018,  9,  9) /* RO 初始值：0  */
/* Ring indicator */
#define UART_F_RI                                               WS_FIELD(0x0018,  8,  8) /* RO 初始值：0  */
/* Transmit FIFO empty */
#define UART_F_TXFE                                             WS_FIELD(0x0018,  7,  7) /* RO 初始值：0  */
/* Receive FIFO full */
#define UART_F_RXFF                                             WS_FIELD(0x0018,  6,  6) /* RO 初始值：0  */
/* transmit fifo full */
#define UART_F_TXFF                                             WS_FIELD(0x0018,  5,  5) /* RO 初始值：0  */
/* Receive FIFO empty */
#define UART_F_RXFE                                             WS_FIELD(0x0018,  4,  4) /* RO 初始值：0  */
/* UARTx busy */
#define UART_F_BUSY                                             WS_FIELD(0x0018,  3,  3) /* RO 初始值：0 */
/* Data carrier detect */
#define UART_F_DCD                                              WS_FIELD(0x0018,  2,  2) /* RO 初始值：0 */
/* Data set ready */
#define UART_F_DSR                                              WS_FIELD(0x0018,  1,  1) /* RO 初始值：0 */
/* Clear to send */
#define UART_F_CTS                                              WS_FIELD(0x0018,  0,  0) /* RO 初始值：0 */

/* dig_port_cfg  */
#define UART_R_LCRH_RX                                          WS_REG(0x001C)           /* RW 初始值：00000000 */
/* RX stick parity select */
#define UART_F_SPS_RX                                           WS_FIELD(0x001C,  7,  7) /* RW 初始值：0 */
/* RX word length */
#define UART_F_WLEN_RX                                          WS_FIELD(0x001C,  5,  6) /* RW 初始值：0 */
/* RX Enable FIFOs */
#define UART_F_FEN_RX                                           WS_FIELD(0x001C,  4,  4) /* RW 初始值：0 */
/* RX Two stop bits select */
#define UART_F_STP2_RX                                          WS_FIELD(0x001C,  3,  3) /* RW 初始值：0 */
/* RX even parity select */
#define UART_F_EPS_RX                                           WS_FIELD(0x001C,  2,  2) /* RW 初始值：0 */
/* RX parity enable */
#define UART_F_PEN_RX                                           WS_FIELD(0x001C,  1,  1) /* RW 初始值：0 */

/* modem状态寄存器1  */
#define UART_R_ILPR                                             WS_REG(0x0020)           /* RO 初始值：00000200 */
#define UART_F_ILPDVSR                                          WS_FIELD(0x0020,  0,  7) /* RW 初始值：0 */

/* modem状态寄存器1  */
#define UART_R_IBRD                                             WS_REG(0x0024)           /* RO 初始值：00000000 */
#define UART_F_DIVINT                                           WS_FIELD(0x0024,  0, 15) /* RW 初始值：0  */

/* modem配置寄存器7  */
#define UART_R_FBRD                                             WS_REG(0x0028)           /* RW 初始值：000086B9 */
#define UART_F_DIVFRAC                                          WS_FIELD(0x0028,  0,  5) /* RW 初始值：0 */

/* modem配置寄存器8  */
#define UART_R_LCRH_TX                                          WS_REG(0x002C)           /* RW 初始值：00154000 */
#define UART_F_SPS_TX                                           WS_FIELD(0x002C,  7,  7) /* RW 初始值：1 */
#define UART_F_WLEN_TX                                          WS_FIELD(0x002C,  5,  6) /* RW 初始值：2 */
#define UART_F_FEN_TX                                           WS_FIELD(0x002C,  4,  4) /* RW 初始值：1 */
#define UART_F_STP2_TX                                          WS_FIELD(0x002C,  3,  3) /* RW 初始值：0 */
#define UART_F_EPS_TX                                           WS_FIELD(0x002C,  2,  2) /* RW 初始值：0 */
#define UART_F_PEN_TX                                           WS_FIELD(0x002C,  1,  1) /* RW 初始值：0 */
#define UART_F_BRK                                              WS_FIELD(0x002C,  0,  0) /* RW 初始值：0 */

/* 控制寄存器  */
#define UART_R_CR                                               WS_REG(0x0030)           /* RW 初始值：00000000 */
#define UART_F_CTSEN                                            WS_FIELD(0x0030, 15, 15) /* RW 初始值：0 */
#define UART_F_RTSEN                                            WS_FIELD(0x0030, 14, 14) /* RW 初始值：0 */
#define UART_F_OUT2                                             WS_FIELD(0x0030, 13, 13) /* RW 初始值：0 */
#define UART_F_OUT1                                             WS_FIELD(0x0030, 12, 12) /* RW 初始值：0 */
#define UART_F_RTS                                              WS_FIELD(0x0030, 11, 11) /* RW 初始值：0 */
#define UART_F_DTR                                              WS_FIELD(0x0030, 10, 10) /* RW 初始值：0 */
#define UART_F_RXE                                              WS_FIELD(0x0030,  9,  9) /* RW 初始值：0 */
#define UART_F_TXE                                              WS_FIELD(0x0030,  8,  8) /* RW 初始值：0 */
#define UART_F_LBE                                              WS_FIELD(0x0030,  7,  7) /* RW 初始值：0 */
#define UART_F_OVSFACT                                          WS_FIELD(0x0030,  3,  3) /* RW 初始值：0 */
#define UART_F_SIRLP                                            WS_FIELD(0x0030,  2,  2) /* RW 初始值：0 */
#define UART_F_SIREN                                            WS_FIELD(0x0030,  1,  1) /* RW 初始值：0 */
#define UART_F_UARTEN                                           WS_FIELD(0x0030,  0,  0) /* RW 初始值：0 */

/* modem配置寄存器11  */
#define UART_R_IFLS                                             WS_REG(0x0034)           /* RW 初始值：00000000 */
#define UART_F_RXIFLSEL                                         WS_FIELD(0x0034,  3,  5) /* RW 初始值：0 */
#define UART_F_TXIFLSEL                                         WS_FIELD(0x0034,  0,  2) /* RW 初始值：0 */

/* modem配置寄存器14  */
#define UART_R_IMSC                                             WS_REG(0x0038)           /* RW 初始值：0000002c */
#define UART_F_TXFEIM                                           WS_FIELD(0x0038, 12, 12) /* RW 初始值：0  */
#define UART_F_XOFFIM                                           WS_FIELD(0x0038, 11, 11) /* RW 初始值：0  */
#define UART_F_OEIM                                             WS_FIELD(0x0038, 10, 10) /* RW 初始值：0  */
#define UART_F_BEIM                                             WS_FIELD(0x0038,  9,  9) /* RW 初始值：0  */
#define UART_F_PEIM                                             WS_FIELD(0x0038,  8,  8) /* RW 初始值：0  */
#define UART_F_FEIM                                             WS_FIELD(0x0038,  7,  7) /* RW 初始值：0  */
#define UART_F_RTIM                                             WS_FIELD(0x0038,  6,  6) /* RW 初始值：0  */
#define UART_F_TXIM                                             WS_FIELD(0x0038,  5,  5) /* RW 初始值：0  */
#define UART_F_RXIM                                             WS_FIELD(0x0038,  4,  4) /* RW 初始值：0  */
#define UART_F_DSRMIM                                           WS_FIELD(0x0038,  3,  3) /* RW 初始值：0  */
#define UART_F_DCDMIM                                           WS_FIELD(0x0038,  2,  2) /* RW 初始值：0  */
#define UART_F_CTSMIM                                           WS_FIELD(0x0038,  1,  1) /* RW 初始值：0  */
#define UART_F_RIMIM                                            WS_FIELD(0x0038,  0,  0) /* RW 初始值：0  */

/* modem配置寄存器14  */
#define UART_R_RIS                                              WS_REG(0x003C)           /* RW 初始值：0000002c */
#define UART_F_TXFERIS                                          WS_FIELD(0x003C, 12, 12) /* RO 初始值：0  */
#define UART_F_XOFFRIS                                          WS_FIELD(0x003C, 11, 11) /* RO 初始值：0  */
#define UART_F_OERIS                                            WS_FIELD(0x003C, 10, 10) /* RO 初始值：0  */
#define UART_F_BERIS                                            WS_FIELD(0x003C,  9,  9) /* RO 初始值：0  */
#define UART_F_PERIS                                            WS_FIELD(0x003C,  8,  8) /* RO 初始值：0  */
#define UART_F_FERIS                                            WS_FIELD(0x003C,  7,  7) /* RO 初始值：0  */
#define UART_F_RTRIS                                            WS_FIELD(0x003C,  6,  6) /* RO 初始值：0  */
#define UART_F_TXRIS                                            WS_FIELD(0x003C,  5,  5) /* RO 初始值：0  */
#define UART_F_RXRIS                                            WS_FIELD(0x003C,  4,  4) /* RO 初始值：0  */
#define UART_F_DSRMIS                                           WS_FIELD(0x003C,  3,  3) /* RO 初始值：0  */
#define UART_F_DCDMIS                                           WS_FIELD(0x003C,  2,  2) /* RO 初始值：0  */
#define UART_F_CTSMIS                                           WS_FIELD(0x003C,  1,  1) /* RO 初始值：0  */
#define UART_F_RIMIS                                            WS_FIELD(0x003C,  0,  0) /* RO 初始值：0  */

/* modem配置寄存器14  */
#define UART_R_MIS                                              WS_REG(0x0040)           /* RW 初始值：0000002c */
#define UART_F_TXFEMIS                                          WS_FIELD(0x0040, 12, 12) /* RO 初始值：0 */
#define UART_F_XOFFMIS                                          WS_FIELD(0x0040, 11, 11) /* RO 初始值：0 */
#define UART_F_OEMIS                                            WS_FIELD(0x0040, 10, 10) /* RO 初始值：0 */
#define UART_F_BEMIS                                            WS_FIELD(0x0040,  9,  9) /* RO 初始值：0 */
#define UART_F_PEMIS                                            WS_FIELD(0x0040,  8,  8) /* RO 初始值：0 */
#define UART_F_FEMIS                                            WS_FIELD(0x0040,  7,  7) /* RO 初始值：0 */
#define UART_F_TRMIS                                            WS_FIELD(0x0040,  6,  6) /* RO 初始值：0 */
#define UART_F_TXMIS                                            WS_FIELD(0x0040,  5,  5) /* RO 初始值：0 */
#define UART_F_RXMIS                                            WS_FIELD(0x0040,  4,  4) /* RO 初始值：0 */
#define UART_F_DSRMMIS                                          WS_FIELD(0x0040,  3,  3) /* RO 初始值：0 */
#define UART_F_DCDMMIS                                          WS_FIELD(0x0040,  2,  2) /* RO 初始值：0 */
#define UART_F_CTSMMIS                                          WS_FIELD(0x0040,  1,  1) /* RO 初始值：0 */
#define UART_F_RIMMIS                                           WS_FIELD(0x0040,  0,  0) /* RO 初始值：0 */

/* modem配置寄存器14  */
#define UART_R_ICR                                              WS_REG(0x0044)           /* RW 初始值：0000002c */
#define UART_F_TXFEIC                                           WS_FIELD(0x0044, 12, 12) /* RW 初始值：0 */
#define UART_F_XOFFIC                                           WS_FIELD(0x0044, 11, 11) /* RW 初始值：0 */
#define UART_F_OEIC                                             WS_FIELD(0x0044, 10, 10) /* RW 初始值：0 */
#define UART_F_BEIC                                             WS_FIELD(0x0044,  9,  9) /* RW 初始值：0 */
#define UART_F_PEIC                                             WS_FIELD(0x0044,  8,  8) /* RW 初始值：0 */
#define UART_F_FEIC                                             WS_FIELD(0x0044,  7,  7) /* RW 初始值：0 */
#define UART_F_RTIC                                             WS_FIELD(0x0044,  6,  6) /* RW 初始值：0 */
#define UART_F_TXIC                                             WS_FIELD(0x0044,  5,  5) /* RW 初始值：0 */
#define UART_F_RXIC                                             WS_FIELD(0x0044,  4,  4) /* RW 初始值：0 */
#define UART_F_DSRMIC                                           WS_FIELD(0x0044,  3,  3) /* RW 初始值：0 */
#define UART_F_DCDMIC                                           WS_FIELD(0x0044,  2,  2) /* RW 初始值：0 */
#define UART_F_CTSMIC                                           WS_FIELD(0x0044,  1,  1) /* RW 初始值：0 */
#define UART_F_RIMIC                                            WS_FIELD(0x0044,  0,  0) /* RW 初始值：0 */

/* modem配置寄存器14  */
#define UART_R_DMACR                                            WS_REG(0x0048)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_XFCR                                             WS_REG(0x0050)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_XON1                                             WS_REG(0x0054)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_XON2                                             WS_REG(0x0058)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_XOFF1                                            WS_REG(0x005C)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_XOFF2                                            WS_REG(0x0060)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_ITCR                                             WS_REG(0x0080)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_ITIP                                             WS_REG(0x0084)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_ITOP                                             WS_REG(0x0088)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_TDR                                              WS_REG(0x008C)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_ABCR                                             WS_REG(0x0100)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_ABSR                                             WS_REG(0x0104)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_ABFMT                                            WS_REG(0x0108)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_ABDR                                             WS_REG(0x0150)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_ABDFR                                            WS_REG(0x0154)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_ABMR                                             WS_REG(0x0158)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_ABIMSC                                           WS_REG(0x015C)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_ABRIS                                            WS_REG(0x0160)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_ABMIS                                            WS_REG(0x0164)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_ABICR                                            WS_REG(0x0168)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_ID_PRODUCT_H_XY                                  WS_REG(0x0FD8)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_ID_PROVIDER                                      WS_REG(0x0FDC)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_PERIPHID0                                        WS_REG(0x0FE0)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_PERIPHID1                                        WS_REG(0x0FE4)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_PERIPHID2                                        WS_REG(0x0FE8)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_PERIPHID3                                        WS_REG(0x0FEC)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_PCELLID0                                         WS_REG(0x0FF0)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_PCELLID1                                         WS_REG(0x0FF4)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_PCELLID2                                         WS_REG(0x0FF8)           /* RW 初始值：0000002c */

/* modem配置寄存器14  */
#define UART_R_PCELLID3                                         WS_REG(0x0FFC)           /* RW 初始值：0000002c */

/*
 * WDT registers and fields
 */
/* 控制寄存器  */
#define WDT_R_WDT_CR                                            WS_REG(0x0000)           /* RW 初始值：00000000 */
/* Reset pulse length. */
#define WDT_F_RPL                                               WS_FIELD(0x0000,  2,  4) /* RW 初始值：0  */
/* 0 = Generate a system reset.
1 = First generate an interrupt and if it is not cleared by the time a second timeout
occurs then generate a system reset. */
#define WDT_F_RMOD                                              WS_FIELD(0x0000,  1,  1) /* RW 初始值：0  */
/* 0 = WDT disabled
1 = WDT enabled. */
#define WDT_F_WDT_EN                                            WS_FIELD(0x0000,  0,  0) /* RW 初始值：0  */

/* 超时范围寄存器  */
#define WDT_R_WDT_TORR                                          WS_REG(0x0004)           /* RW 初始值：00000000 */
/* Timeout period for initialization. */
#define WDT_F_TOP_INIT                                          WS_FIELD(0x0004,  4,  7) /* RW 初始值：0  */
/* Timeout period. */
#define WDT_F_TOP                                               WS_FIELD(0x0004,  0,  3) /* RW 初始值：0  */

/* 当前计数值寄存器  */
#define WDT_R_WDT_CCVR                                          WS_REG(0x0008)           /* RW 初始值：00000000 */
/* 看门狗当前计数值 */
#define WDT_F_WDT_CNT                                           WS_FIELD(0x0008,  0, 31) /* RO 初始值：0  */

/* 计时器重置寄存器  */
#define WDT_R_WDT_CRR                                           WS_REG(0x000C)           /* WO 初始值：00000000 */
#define WDT_F_WDT_CRR                                           WS_FIELD(0x000C,  0, 31) /* WO 初始值：0  */

/* 中断状态寄存器  */
#define WDT_R_WDT_STAT                                          WS_REG(0x0010)           /* RO 初始值：00000000 */
/* 
1 = Interrupt is active regardless of polarity.
0 = Interrupt is inactive. */
#define WDT_F_STAT                                              WS_FIELD(0x0010,  0, 31) /* RO 初始值：0  */

/* 中断清除寄存器  */
#define WDT_R_WDT_EOI                                           WS_REG(0x0014)           /* RO 初始值：00000000 */
/* Clears the watchdog interrupt. This can be used to clear the interrupt without restarting */
#define WDT_F_ICR                                               WS_FIELD(0x0014,  0,  0) /* RO 初始值：0  */

/* 重装载寄存器  */
#define WDT_R_WDT_RLD                                           WS_REG(0x001C)           /* RW 初始值：00000000 */
#define WDT_F_RLD                                               WS_FIELD(0x001C,  0, 31) /* RW 初始值：0  */

