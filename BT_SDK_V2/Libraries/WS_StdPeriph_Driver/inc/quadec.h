/**
******************************************************************************************************************************************************************************************
*@file                       quadec.h
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

#ifndef QUADEC_H_
#define QUADEC_H_

#include "cpu.h"

/* ENABLE/DISABLE */
#define quadec_en(val)                WS_WRITE_FIELD(QDEC_BASE, QDEC_F_ECNT, val)

/*Index ENABLE/DISABLE */
#define quadec_index_en(val)        WS_WRITE_FIELD(QDEC_BASE, QDEC_F_INEN, val)

#define QDEC_POSITIVE_ALB                0
#define QDEC_NEGATIVE_ALB                1

/* POSITIVE_ALB / NEGATIVE_ALB */
#define quadec_set_direction(val)    WS_WRITE_FIELD(QDEC_BASE, QDEC_F_CTDR, val)

/*
 * 0 : 当CH_x位逻辑0时，Index产生断言
 * 1 : 当CH_x位逻辑1时，Index产生断言
 */
#define quadec_index_ch_cfg(vala, valb, idl)            \
    do{                                                    \
        WS_WRITE_FIELD(QDEC_BASE, QDEC_F_ICHA, vala);    \
        WS_WRITE_FIELD(QDEC_BASE, QDEC_F_ICHB, valb);    \
        WS_WRITE_FIELD(QDEC_BASE, QDEC_F_IDXL, idl);    \
    }while(0)

#define QDEC_NOT_AFFECTED                0
#define QDEC_AFFECTED                    1
/*
 * inrc: 读取QDEC计数值是否受index事件影响；
 * inzc：QDEC内部计数值是否受index断言影响；
 * NOT_AFFECTED / AFFECTED
 */
#define quadec_wrc_izcb(inrc, inzc)                        \
    do{                                                    \
        WS_WRITE_FIELD(QDEC_BASE, QDEC_F_INRC, inrc);    \
        WS_WRITE_FIELD(QDEC_BASE, QDEC_F_INZC, inzc);    \
    }while(0)

/* 设置内部计数值 */
#define quadec_set_internal_counter(val)                   \
    do{                                                    \
        WS_WRITE_FIELD(QDEC_BASE, QDEC_F_QRW, val);        \
        WS_WRITE_FIELD(QDEC_BASE, QDEC_F_PLCT, 1);        \
        while(WS_READ_FIELD(QDEC_BASE, QDEC_F_PLCT));    \
    }while(0)

/*
 * FIELD ->>
 *    QDEC_F_CMIE        比较匹配中断
 *    QDEC_F_INIE        Index中断
 *    QDEC_F_UNIE        下溢中断
 *    QDEC_F_OVIE        上溢中断
 *    QDEC_F_QEIE     错误中断
 *
 * val : ENABLE/DISABLE
 */
#define quadec_it_en(FIELD, val)     (QDEC_BASE, FIELD, val)

/*
 * FIELD ->>
 * QDEC_F_CMI    比较匹配中断
 * QDEC_F_INI    Index中断
 * QDEC_F_UNI    下溢中断
 * QDEC_F_OVI    上溢中断
 * QDEC_F_QEI    错误中断
 */
#define quadec_it_status(FIELD)        WS_READ_FIELD2(QDEC_BASE, FIELD)
#define quadec_it_clear(FIELD)        WS_WRITE_FIELD2(QDEC_BASE, FIELD, ENABLE);

/*
 * QDEC采样频率设置：
 * val -- 0-15
 *         Fclk = PCLK/(val+1)
 */
 #define quadec_sample_frq(val)        WS_WRITE_FIELD(QDEC_BASE, QDEC_F_QCK, val)

/* 读取正交解码器的计数值 */
 static inline uint32_t quadec_counter_cnt()
 {
    WS_WRITE_FIELD(QDEC_BASE, QDEC_F_QLAT, 1);
    while(WS_READ_FIELD(QDEC_BASE, QDEC_F_QLAT));
    return (uint32_t)WS_READ_FIELD(QDEC_BASE, QDEC_F_QRW);
 }

#endif
