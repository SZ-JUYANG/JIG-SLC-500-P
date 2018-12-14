#include "M971x.h"
#include "usart.h"
#include "CommonFunction.h"
#include "ucos_ii.h"
#include "RA8875.h"
#include "cal_test.h"
//UART4 波特率115200
//串口是TTL电平


#define M971X_ADDR  1
#define DELAYTIME   4
#define M971x_DelayMs(x) OSTimeDlyHMSM(0,0,0,x)
/*
static u16 GetCRC(u8 *pData,u16 Len)
{
	u16 tempCRC=0xffff;
	u8 i=0;
	u16 j=0;
	for(j=0;j<Len;j++)
	{
		tempCRC=tempCRC^pData[j];
		for(i=0;i<8;i++)
		{
			if(tempCRC&0x0001)
			{
				tempCRC=(tempCRC>>1)^0xa001;
			}
			else
			{
				tempCRC=tempCRC>>1;
			}
		}
  }
	return tempCRC;
}
*/
