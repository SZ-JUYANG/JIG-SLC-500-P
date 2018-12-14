





#include <stm32f10x.h>
#include <stm32f10x_it.h>
#include <stm32f10x_conf.h> //所有STM32外设驱动
#include "Flash.h"

#include "setupled.h"
#include "setuphid.h"
#include "setupctrl.h"

#ifdef DEBUG_ON
		#include "RA8875.h"	
#endif





#define HEAD_DATA 0xaa
#define FLASH_LED_TYPE 0x01
#define FLASH_HID_TYPE 0x02
#define FLASH_CTRL_TYPE 0x03


#define FLASH_LED_LEN  14
#define FLASH_HID_LEN  15
#define FLASH_CTRL_LEN  3


/***
#define FLASH_LED_ADDR   1
#define FLASH_HID_ADDR   FLASH_LED_LEN+1+1
#define FLASH_CTRL_ADDR  FLASH_HID_ADDR+FLASH_HID_LEN+1+1
***/


//#define FLASH_Read(_Offset) (*(__IO uint32_t*)(FLASH_BASE_ADDRESS+(_Offset)))
#define FLASH_Read(_Offset) (*(__IO uint32_t*)(_Offset))
//#define PrintErr() LcdPrintf0(8*0,0,BLACK,WHITE,1,"保存数据错误")

/********************************************
保存数据
成功返回1，失败返回-1
********************************************/
static char _FLASH_Write(u32 _Offset,u32 _data)
{
	FLASH_ProgramWord(_Offset,_data);
	if(*(__IO uint32_t*)(_Offset)==_data)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

/***********************************
_offset:保存数据的地址
_psavedata:保存的数据指针，必须是都是
           long或者float的结构体或数组
_type:保存的类型：LED，HID，CTRL
返回：16位校验和
************************************/
static const u16 FLASH_STRUCT_LEN[4]={0,FLASH_LED_LEN,FLASH_HID_LEN,FLASH_CTRL_LEN};

u16 FLASH_SaveSetData(u32 _offset,void *_psavedata,u8 _type)
{
	u32 *_p=(u32*)_psavedata;
	u16 i;
	u32 s_sum32=0;
	u16 s_sum16=0;
	
	FLASH_UnlockBank1();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(_offset);
	for(i=0;i<FLASH_STRUCT_LEN[_type&0x03];i++)
	{
		s_sum32+=*_p;
		if(_FLASH_Write(_offset+i*4,*(_p++))==-1)
		{
			#ifdef DEBUG_ON
			PrintErr();
			#endif
		}
	}
	s_sum16=(s_sum32>>16)+(s_sum32&0xffff);
	return s_sum16;
}



//读取数据，返回void指针
//数据必须是float或者long
void* FLASH_ReadSetData(u32 _offset,u8 _type)
{
	static u32 s_readdata[30];
	u16 i;
	for(i=0;i<FLASH_STRUCT_LEN[_type&0x03];i++)
	{
		s_readdata[i]=FLASH_Read(_offset+i*4);
	}
	return s_readdata;
}


char FLASH_SaveCalSet(u32 _data)
{
	//int i;
	FLASH_UnlockBank1();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(CAL_SETUP_ADDR);
	//for(i=0;i<len;i++)
	{
		if(_FLASH_Write(CAL_SETUP_ADDR,_data)!=1)
			return -1;
	}
	return 1;
}

u32 FLASH_ReadCalSet(void)
{
	return (u32)FLASH_Read(CAL_SETUP_ADDR);
}






