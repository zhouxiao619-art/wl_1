/*
 * Copyright (c) 2019 Wise Sun
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __REG_UTIL_H_
#define __REG_UTIL_H_

/********************************** macro interface ******************************/

/* Defined Register field range */
#define WS_FIELD(offset, low_bit, high_bit)	            (offset), (low_bit), (high_bit)

#define WS_REG(offset)                                  WS_FIELD(offset, 0, 31)

/* Write Fields */
#define WS_WRITE_FIELDS(DEV_BASE, ...)                  WS_WRITE_FIELD3(DEV_BASE, WS_FIELDS_OFFSET(__VA_ARGS__), WS_FIELDS_MASK(__VA_ARGS__), WS_FIELDS_VAL(__VA_ARGS__))

/* Write fields no readback */
#define WS_WRITE_FIELDS_NO_READBACK(DEV_BASE, ...)      WS_WRITE_FIELD3_NO_READBACK(DEV_BASE, WS_FIELDS_OFFSET(__VA_ARGS__), WS_FIELDS_VAL(__VA_ARGS__))

/* Read Field */
#define WS_READ_FIELD(DEV_BASE, FIELD)		            WS_READ_FIELD2(DEV_BASE, FIELD)

/* Read register */
#define WS_READ_REG(addr)                               (WS_REG_VAL(addr, 0))

/* Read Field by address */
#define WS_READ_REG_FIELD(addr, low, high)              WS_READ_FIELD2(addr, 0, low, high)

/* Write Field */
#define WS_WRITE_FIELD(DEV_BASE, FIELD, VAL)            WS_WRITE_FIELD2(DEV_BASE, FIELD, VAL)

/* Write register */
#define WS_WRITE_REG(addr, val)                         (WS_REG_VAL(addr, 0) = ((unsigned int)(val)))

/* Write Field by address */
#define WS_WRITE_REG_FIELD(addr, low, high, val)        WS_WRITE_FIELD2(addr, 0, low, high, val)

#ifndef BIT
#define BIT(n)   (1UL << n)
#endif

#define WS_WRITE_FIELD_REG(base, field, val)            WS_WRITE_FIELD2(base, WS_FIELD_OFFSET2(field), 0, 31, val)

/********************************* implement detail ********************************/

#define WS_VA_EXPAND_ARGS(...)  __VA_ARGS__

#define WS_VA_NARGS_EXPAND(x)  x

#define WS_VA_NARGS(...)  WS_VA_NARGS_EXPAND(__WS_VA_NARGS(0, ##__VA_ARGS__,\
                                    128, \
                                    127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112, \
                                    111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, \
                                    95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, \
                                    79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, \
                                    63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, \
                                    47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, \
                                    31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, \
                                    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#define __WS_VA_NARGS(  _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, \
                    _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, \
                    _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, \
                    _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, \
                    _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, \
                    _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, \
                    _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, \
                    _112, _113, _114, _115, _116, _117, _118, _119, _120, _121, _122, _123, _124, _125, _126, _127, \
                    _128, N, ...) N

#define __WS_FIELDS_MASK__(offset, low, high, val)          (((1UL << ((high) - (low) + 1)) - 1UL) << (low))
#define __WS_FIELDS_MASK_4(offset, low, high, val)           __WS_FIELDS_MASK__(offset, low, high, val)
#define __WS_FIELDS_MASK_8(offset, low, high, val, ...)      (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_4(__VA_ARGS__))
#define __WS_FIELDS_MASK_12(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_8(__VA_ARGS__))
#define __WS_FIELDS_MASK_16(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_12(__VA_ARGS__))
#define __WS_FIELDS_MASK_20(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_16(__VA_ARGS__))
#define __WS_FIELDS_MASK_24(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_20(__VA_ARGS__))
#define __WS_FIELDS_MASK_28(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_24(__VA_ARGS__))
#define __WS_FIELDS_MASK_32(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_28(__VA_ARGS__))
#define __WS_FIELDS_MASK_36(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_32(__VA_ARGS__))
#define __WS_FIELDS_MASK_40(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_36(__VA_ARGS__))
#define __WS_FIELDS_MASK_44(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_40(__VA_ARGS__))
#define __WS_FIELDS_MASK_48(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_44(__VA_ARGS__))
#define __WS_FIELDS_MASK_52(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_48(__VA_ARGS__))
#define __WS_FIELDS_MASK_56(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_52(__VA_ARGS__))
#define __WS_FIELDS_MASK_60(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_56(__VA_ARGS__))
#define __WS_FIELDS_MASK_64(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_60(__VA_ARGS__))
#define __WS_FIELDS_MASK_68(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_64(__VA_ARGS__))
#define __WS_FIELDS_MASK_72(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_68(__VA_ARGS__))
#define __WS_FIELDS_MASK_76(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_72(__VA_ARGS__))
#define __WS_FIELDS_MASK_80(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_76(__VA_ARGS__))
#define __WS_FIELDS_MASK_84(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_80(__VA_ARGS__))
#define __WS_FIELDS_MASK_88(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_84(__VA_ARGS__))
#define __WS_FIELDS_MASK_92(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_88(__VA_ARGS__))
#define __WS_FIELDS_MASK_96(offset, low, high, val, ...)     (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_92(__VA_ARGS__))
#define __WS_FIELDS_MASK_100(offset, low, high, val, ...)    (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_96(__VA_ARGS__))
#define __WS_FIELDS_MASK_104(offset, low, high, val, ...)    (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_100(__VA_ARGS__))
#define __WS_FIELDS_MASK_108(offset, low, high, val, ...)    (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_104(__VA_ARGS__))
#define __WS_FIELDS_MASK_112(offset, low, high, val, ...)    (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_108(__VA_ARGS__))
#define __WS_FIELDS_MASK_116(offset, low, high, val, ...)    (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_112(__VA_ARGS__))
#define __WS_FIELDS_MASK_120(offset, low, high, val, ...)    (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_116(__VA_ARGS__))
#define __WS_FIELDS_MASK_124(offset, low, high, val, ...)    (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_120(__VA_ARGS__))
#define __WS_FIELDS_MASK_128(offset, low, high, val, ...)    (__WS_FIELDS_MASK__(offset, low, high, val) | __WS_FIELDS_MASK_124(__VA_ARGS__))

#define __WS_FIELDS_VAL__(offset, low, high, val)           (((unsigned int)(val)) << (low))
#define __WS_FIELDS_VAL_4(offset, low, high, val)           __WS_FIELDS_VAL__(offset, low, high, val)
#define __WS_FIELDS_VAL_8(offset, low, high, val, ...)      (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_4(__VA_ARGS__))
#define __WS_FIELDS_VAL_12(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_8(__VA_ARGS__))
#define __WS_FIELDS_VAL_16(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_12(__VA_ARGS__))
#define __WS_FIELDS_VAL_20(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_16(__VA_ARGS__))
#define __WS_FIELDS_VAL_24(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_20(__VA_ARGS__))
#define __WS_FIELDS_VAL_28(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_24(__VA_ARGS__))
#define __WS_FIELDS_VAL_32(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_28(__VA_ARGS__))
#define __WS_FIELDS_VAL_36(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_32(__VA_ARGS__))
#define __WS_FIELDS_VAL_40(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_36(__VA_ARGS__))
#define __WS_FIELDS_VAL_44(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_40(__VA_ARGS__))
#define __WS_FIELDS_VAL_48(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_44(__VA_ARGS__))
#define __WS_FIELDS_VAL_52(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_48(__VA_ARGS__))
#define __WS_FIELDS_VAL_56(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_52(__VA_ARGS__))
#define __WS_FIELDS_VAL_60(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_56(__VA_ARGS__))
#define __WS_FIELDS_VAL_64(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_60(__VA_ARGS__))
#define __WS_FIELDS_VAL_68(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_64(__VA_ARGS__))
#define __WS_FIELDS_VAL_72(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_68(__VA_ARGS__))
#define __WS_FIELDS_VAL_76(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_72(__VA_ARGS__))
#define __WS_FIELDS_VAL_80(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_76(__VA_ARGS__))
#define __WS_FIELDS_VAL_84(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_80(__VA_ARGS__))
#define __WS_FIELDS_VAL_88(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_84(__VA_ARGS__))
#define __WS_FIELDS_VAL_92(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_88(__VA_ARGS__))
#define __WS_FIELDS_VAL_96(offset, low, high, val, ...)     (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_92(__VA_ARGS__))
#define __WS_FIELDS_VAL_100(offset, low, high, val, ...)    (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_96(__VA_ARGS__))
#define __WS_FIELDS_VAL_104(offset, low, high, val, ...)    (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_100(__VA_ARGS__))
#define __WS_FIELDS_VAL_108(offset, low, high, val, ...)    (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_104(__VA_ARGS__))
#define __WS_FIELDS_VAL_112(offset, low, high, val, ...)    (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_108(__VA_ARGS__))
#define __WS_FIELDS_VAL_116(offset, low, high, val, ...)    (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_112(__VA_ARGS__))
#define __WS_FIELDS_VAL_120(offset, low, high, val, ...)    (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_116(__VA_ARGS__))
#define __WS_FIELDS_VAL_124(offset, low, high, val, ...)    (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_120(__VA_ARGS__))
#define __WS_FIELDS_VAL_128(offset, low, high, val, ...)    (__WS_FIELDS_VAL__(offset, low, high, val) | __WS_FIELDS_VAL_124(__VA_ARGS__))

#define __WS_VA_NARGS_EXPAND(...)  __WS_EVAL(__VA_ARGS__)

#define __WS_EVAL(...) __WS_EVAL1(__WS_EVAL1(__WS_EVAL1(__VA_ARGS__)))
#define __WS_EVAL1(...) __VA_ARGS__

#define __WS_MACRO_CAT_2(x, y)  ____WS_MACRO_CAT_2(x, y)
#define ____WS_MACRO_CAT_2(x, y) x##y

#define WS_FIELDS_MASK(...)                             __WS_VA_NARGS_EXPAND(__WS_MACRO_CAT_2(__WS_FIELDS_MASK_, WS_VA_NARGS(__VA_ARGS__))(__VA_ARGS__))
#define WS_FIELDS_VAL(...)                              __WS_VA_NARGS_EXPAND(__WS_MACRO_CAT_2(__WS_FIELDS_VAL_, WS_VA_NARGS(__VA_ARGS__))(__VA_ARGS__))
#define WS_FIELDS_OFFSET(...)                           __WS_VA_NARGS_EXPAND(WS_FIELD_OFFSET2(__VA_ARGS__))

#define WS_REG_VAL(base, offset)                        *(volatile unsigned int *)((unsigned int)(base) + (unsigned int)(offset))

#define WS_OFFSET_OF(type, member)                      ((char *)(&((type *)0)->member) - (char *)0)

#define WS_FIELD_OFFSET2(offset, low, high, ...)        (offset)
#define WS_FIELD_OFFSET(FIELD)                          WS_FIELD_OFFSET2(FIELD)

#define WS_FIELD_LOW_BIT2(offset, low, high)            (low)
#define WS_FIELD_LOW_BIT(FIELD)                         WS_FIELD_LOW_BIT2(FIELD)

#define WS_FIELD_HIGH_BIT2(offset, low, high)           (high)
#define WS_FIELD_HIGH_BIT(FIELD)                        WS_FIELD_HIGH_BIT2(FIELD)

#define WS_FIELD_FIELD2(offset, low_bit, high_bit, field_low_bit, field_high_bit)   \
                (offset), ((low_bit) + (field_low_bit)), ((low_bit) + (field_high_bit))
#define WS_FIELD_FIELD(FIELD, low, high)                WS_FIELD_FIELD2(FIELD, low, high)

/* Field range mask */
#define WS_FIELD_RANGE_MASK(low_bit, high_bit)	       \
    ((1UL << ((high_bit) - (low_bit) + 1)) - 1UL)

/* Read Register */
#define WS_READ_FIELD2(base, offset, low, high)        \
    (((high) - (low) >= 31) ? WS_REG_VAL(base, offset) : ((WS_REG_VAL(base, offset) >> (low)) & WS_FIELD_RANGE_MASK((low), (high))))

/* Read Val field val */
#define WS_VAL_FIELD_READ(val, field)                   WS_VAL_FIELD_READ2(val, field)

#define WS_VAL_FIELD_READ2(val, offset, low, high)     \
    (((high) - (low) >= 31) ? (val) : (((val) >> (low)) & WS_FIELD_RANGE_MASK((low), (high))))

/* Write Register */
#define WS_WRITE_FIELD2(base, offset, low, high, val)  \
	(WS_REG_VAL(base, offset) = ((high) - (low) >= 31) ? ((unsigned int)(val)) : ((WS_REG_VAL(base, offset) & ~(WS_FIELD_RANGE_MASK(low, high) << (low))) | (((unsigned int)(val)) << (low))))

#define WS_WRITE_FIELD2_R(base, offset, low, high, val)  \
    (WS_REG_VAL(base, offset) = (val))

#define WS_WRITE_FIELD3(base, offset, mask, val)       \
    (WS_REG_VAL(base, offset) = ((mask) == 0xFFFFFFFF ? ((unsigned int)(val)) : ((WS_REG_VAL(base, offset) & ~(mask)) | ((unsigned int)(val)))))

#define WS_WRITE_FIELD3_NO_READBACK(base, offset, val) \
    (WS_REG_VAL(base, offset) = ((unsigned int)(val)))

#endif /* _WS_SOC_REG_UTIL_H_ */
