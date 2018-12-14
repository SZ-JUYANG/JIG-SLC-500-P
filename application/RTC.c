


#include "RTC.h"
#include <stdio.h>






void RTC_Config(void)
{

    u16 u16_WaitForOscSource;
    if (BKP_ReadBackupRegister(BKP_DR1) != 0x5A5A)
    {
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
      /* Allow access to BKP Domain */
			
      PWR_BackupAccessCmd(ENABLE);
      /* Reset Backup Domain */

       BKP_DeInit();

      /* Enable LSE */
      RCC_LSEConfig(RCC_LSE_ON);
       for(u16_WaitForOscSource=0;u16_WaitForOscSource<5000;u16_WaitForOscSource++)
			 {
				 
			 }

      /* Wait till LSE is ready */
      while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

      /* Select LSE as RTC Clock Source */
      RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

      /* Enable RTC Clock */
      RCC_RTCCLKCmd(ENABLE);

      /* Wait for RTC registers synchronization */
      RTC_WaitForSynchro();

      /* Wait until last write operation on RTC registers has finished */
      RTC_WaitForLastTask();

      /* Enable the RTC Second */
      RTC_ITConfig(RTC_IT_SEC, ENABLE);

      /* Wait until last write operation on RTC registers has finished */
       RTC_WaitForLastTask();

      /* Set RTC prescaler: set RTC period to 1sec */
      RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

      /* Wait until last write operation on RTC registers has finished */
      RTC_WaitForLastTask();
      BKP_WriteBackupRegister(BKP_DR1, 0x5A5A);
      //RTC_Set(2011,01,01,0,0,0);
    }
    else
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
        for(u16_WaitForOscSource=0;u16_WaitForOscSource<5000;u16_WaitForOscSource++);
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        {
        }
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        {
        }
        RCC_ClearFlag();
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        RTC_WaitForLastTask();
    }
    return;
}




//设置32位时钟寄存器的值
static void RTC_Set(u32 _value)
{
	RCC->APB1ENR|=1<<27;
	PWR->CR|=1<<8; 
	PWR_BackupAccessCmd(ENABLE);
	RTC_WaitForLastTask();  
	RTC_EnterConfigMode();
	RTC_SetCounter(_value);
	RTC_WaitForLastTask();
	RTC_ExitConfigMode();
}




static const u8 RTC_LeapMonthDay[12]={31,29,31,30,31,30,31,31,30,31,30,31};
static const u8 RTC_CommMonthDay[12]={31,28,31,30,31,30,31,31,30,31,30,31};

//日期转换成秒
static u32 RTC_Data2Second(u16 _year,u8 _month,u8 _date,u8 _hour,u8 _min,u8 _second)
{

	u16 s_day=0,s_mday=0;
	u8 i;
	u8 const *_p;
	_year=_year-2000;
	s_day=365*_year+_year/4;
	if(_year%4==0)
	{
		_p=RTC_LeapMonthDay;
	}
	else
	{
		_p=RTC_CommMonthDay;
	}
	for(i=0;i<_month-1;i++)
	{
		s_mday=s_mday+_p[i];
	}
	s_mday+=_date-1;
	return (s_day+s_mday)*24*3600+_hour*3600+_min*60+_second;
}


//秒转换成日期
static TimeStruct RTC_Second2Date(u32 _second)
{
	TimeStruct s_Time;
	u16 s_4years;
	u32 s_remainder;
	u8 const *_p=RTC_CommMonthDay;
	u8 i;
	s_4years=_second/((365*4+1)*24*3600);
	s_remainder=_second%((365*4+1)*24*3600);
	s_Time.Year=s_4years*4+s_remainder/(365*24*3600)+2000;
	s_remainder=s_remainder%(365*24*3600);
	if(s_Time.Year%4==0)
	{
		_p=RTC_LeapMonthDay;
	}
	s_Time.Month=1;
	for(i=0;i<12;i++)
	{
		if(s_remainder>_p[i]*24*3600)
		{
			s_Time.Month++;
			s_remainder=s_remainder-_p[i]*24*3600;
		}
		else
		{
			break;
		}
	}
	s_Time.Date=s_remainder/(24*3600)+1;
	s_remainder=s_remainder%(24*3600);
	s_Time.Hour=s_remainder/3600;
	s_remainder=s_remainder%3600;
	s_Time.Minute=s_remainder/60;
	s_Time.Second=s_remainder%60;
	return s_Time;
}



//获取当前时间
char *RTC_GetTimeStr()
{
	TimeStruct s_Time;
	static char s_str[40];
	s_Time=RTC_Second2Date(RTC_GetCounter());
	sprintf(s_str,"%04d-%02d-%02d  %02d:%02d:%02d",
					s_Time.Year,s_Time.Month,s_Time.Date,s_Time.Hour,
					s_Time.Minute,s_Time.Second);
	return s_str;
}


//获取当前时间
TimeStruct RTC_GetTime()
{
	return RTC_Second2Date(RTC_GetCounter());
}



//保存或者设置时间
void RTC_SaveTime(u16 _year,u8 _month,u8 _date,u8 _hour,u8 _min,u8 _second)
{
	RTC_Set(RTC_Data2Second(_year,_month,_date,_hour,_min,_second));
}


