

#ifndef _RTC_H
#define _RTC_H



#include <stm32f10x.h>
#include <stm32f10x_it.h>
#include <stm32f10x_conf.h> //所有STM32外设驱动


typedef struct 
{
	u16 Year;
	u8 Month;
	u8 Date;
	u8 Hour;
	u8 Minute;
	u8 Second;
	//u16 Weak;
}TimeStruct;







//u32 RTC_Data2Second(u16 _year,u8 _month,u8 _date,u8 _hour,u8 _min,u8 _second);
char *RTC_GetTimeStr();
void RTC_SaveTime(u16 _year,u8 _month,u8 _date,u8 _hour,u8 _min,u8 _second);
TimeStruct RTC_GetTime();




#endif









