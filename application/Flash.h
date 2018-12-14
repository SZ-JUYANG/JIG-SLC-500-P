
#ifndef _FLASH_H
#define _FLASH_H

#include <stm32f10x.h>
#include "setupled.h"

#define FLASH_BASE_ADDRESS  0x0807F800
#define FLASH_ADDRESS0  (FLASH_BASE_ADDRESS-0x800*0)
#define FLASH_ADDRESS1  (FLASH_BASE_ADDRESS-0x800*1)
#define FLASH_ADDRESS2  (FLASH_BASE_ADDRESS-0x800*2)
#define FLASH_ADDRESS3  (FLASH_BASE_ADDRESS-0x800*3)
#define FLASH_ADDRESS4  (FLASH_BASE_ADDRESS-0x800*4)
#define FLASH_ADDRESS5  (FLASH_BASE_ADDRESS-0x800*5)
#define FLASH_ADDRESS6  (FLASH_BASE_ADDRESS-0x800*6)

#define CAL_SETUP_ADDR   FLASH_ADDRESS3


s8 _FLASH_SaveStruct(u16 _offset,void *_pdata,u8 _type);
SetLEDStruct FLASF_ReadStruct(u16 _offset,u8 _type);
u16 FLASH_SaveSetData(u32 _offset,void *_psavedata,u8 _type);
void* FLASH_ReadSetData(u32 _offset,u8 _type);






#define SaveLEDSetData(x)   FLASH_SaveSetData(FLASH_ADDRESS0,x,1)
#define SaveHID(x)          FLASH_SaveSetData(FLASH_ADDRESS1,x,2)
#define SaveCTRL(x)         FLASH_SaveSetData(FLASH_ADDRESS2,x,3)

#define ReadLED()           FLASH_ReadSetData(FLASH_ADDRESS0,1);
#define ReadHID()           FLASH_ReadSetData(FLASH_ADDRESS1,2);
#define ReadCTRL()          FLASH_ReadSetData(FLASH_ADDRESS2,3);



#define CAL_INPUT_P   0x00000001
#define CAL_INPUT_V  (0x00000001<<1)
#define CAL_INPUT_I  (0x00000001<<2)
#define CAL_OUTPUT_V (0x00000001<<3)
#define CAL_OUTPUT_I (0x00000001<<4)
char FLASH_SaveCalSet(u32 _data);
u32 FLASH_ReadCalSet(void);
#endif











