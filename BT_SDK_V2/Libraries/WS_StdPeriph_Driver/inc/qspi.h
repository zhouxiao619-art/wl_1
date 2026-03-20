#ifndef __EDIT_FROM_H__
#define __EDIT_FROM_H__

#include "cpu.h"

                                  
///flash锁定                                  
void flash_lock(void);
///flash解锁
void flash_unlock(void);
///返回flash id
int flash_read_id(void);
///flash扇区擦除单位为4K，addr按扇区对齐
int flash_erase_sector(unsigned int addr);
///操作addr的size必须按字对齐，传入的data按字节传入
int flash_program(unsigned int addr, unsigned int size, const void *data);


#endif

