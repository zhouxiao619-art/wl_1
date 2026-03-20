/**
******************************************************************************************************************************************************************************************
*@file                       cpu_nvic.h
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
#ifndef NVIC_H
#define NVIC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cpu.h"



typedef struct
{
  uint32_t NVIC_IRQChannel;                    

  uint32_t NVIC_IRQChannelPreemptionPriority;  
    
  uint32_t NVIC_IRQChannelSubPriority;   
    
  Status   NVIC_IRQChannelCmd;   
    
} NVIC_InitTypeDef;
 


#define NVIC_VectTab_RAM             ((uint32_t)0x20000000)
#define NVIC_VectTab_FLASH           ((uint32_t)0x0000)
#define IS_NVIC_VECTTAB(VECTTAB) (((VECTTAB) == NVIC_VectTab_RAM) || \
                                  ((VECTTAB) == NVIC_VectTab_FLASH))


#define NVIC_PriorityGroup_0         ((uint32_t)0x07) /*!< 0 bits for pre-emption priority
                                                            3 bits for subpriority */
#define NVIC_PriorityGroup_1         ((uint32_t)0x06) /*!< 1 bits for pre-emption priority
                                                            2 bits for subpriority */
#define NVIC_PriorityGroup_2         ((uint32_t)0x05) /*!< 2 bits for pre-emption priority
                                                            1 bits for subpriority */
#define NVIC_PriorityGroup_3         ((uint32_t)0x04) /*!< 3 bits for pre-emption priority
                                                            0 bits for subpriority */

#define IS_NVIC_PRIORITY_GROUP(GROUP) (((GROUP) == NVIC_PriorityGroup_0) || \
                                       ((GROUP) == NVIC_PriorityGroup_1) || \
                                       ((GROUP) == NVIC_PriorityGroup_2) || \
                                       ((GROUP) == NVIC_PriorityGroup_3) )
                                      
#define IS_NVIC_PREEMPTION_PRIORITY(PRIORITY)  ((PRIORITY) < 0x10)

#define IS_NVIC_SUB_PRIORITY(PRIORITY)  ((PRIORITY) < 0x10)

#define IS_NVIC_OFFSET(OFFSET)  ((OFFSET) < 0x000FFFFF)


//void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);
void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct);
void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset);

#ifdef __cplusplus
}
#endif

#endif /* NVIC_H */



