




//波特率 19200
//测试LED
#include "UI2050.h"
#include "usart.h"
#include "utility.h"
#include "ucos_ii.h"
#include "RA8875.h"





#define UI2050_COM      USART2
#define  UI2050_pRecBuff PM9805_pRecBuff
#define  UI2050_RecCtn   PM9805_RecCtn
#define UI2050_DelayMs(x) OSTimeDlyHMSM(0,0,0,x)
#define TIMEOUTMS        10*1000


extern  u8  UI2050_pRecBuff[];
extern  u16 UI2050_RecCtn;


//
static float UI2050_Char2Float(u8 *pSourceData)
{
	union
	{
		float fd;
		u8 chd[4];
	}s_undata;
	u8 i;
	for(i=0;i<4;i++)
	{
		s_undata.chd[i]=*(pSourceData++);
	}
	return s_undata.fd;
}

//char数组复制到结构体
static void UI2050_Char2FloatStruct(void *pDesData,u8 *pSourceData,u16 FloatLen)
{
	u16 i=0;
	float *s_pDesData=(float *)pDesData;
	while(i<FloatLen)
	{
		*(s_pDesData++)=UI2050_Char2Float(pSourceData);
		pSourceData=pSourceData+4;
		i++;
	}
}



//读取输入数据，数据存在指针所指的结构体里面
//成功返回1，失败返回-1
s8 UI2050_GetInput(UI2050_InputStruct *pStruct)
{
	u32 s_Timeout,i=0;
	u8 s_ACorDC;
	UI2050_RecCtn=0;
	USART_SendData(UI2050_COM,'?');
	s_Timeout=0;
	while(UI2050_RecCtn<1)
	{
		s_Timeout++;
		UI2050_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	if(UI2050_pRecBuff[0]!='!')
		return -1;
	UI2050_RecCtn=0;
	USART_SendData(UI2050_COM,'0');
	s_Timeout=0;
	while(UI2050_RecCtn<1)
	{
		s_Timeout++;
		UI2050_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	if(UI2050_pRecBuff[0]!=0 && UI2050_pRecBuff[0]!=1)
	{
		return -1;
	}
	s_ACorDC=UI2050_pRecBuff[0];
	UI2050_RecCtn=0;
	USART_SendData(UI2050_COM,'!');
	s_Timeout=0;
	while(UI2050_RecCtn<512)
	{
		s_Timeout++;
		UI2050_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	
	UI2050_RecCtn=0;
	USART_SendData(UI2050_COM,'!');
	s_Timeout=0;
	while(UI2050_RecCtn<512)
	{
		s_Timeout++;
		UI2050_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	UI2050_RecCtn=0;
	USART_SendData(UI2050_COM,'!');
	s_Timeout=0;
	while(UI2050_RecCtn<60)
	{
		s_Timeout++;
		UI2050_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	UI2050_Char2FloatStruct(pStruct,UI2050_pRecBuff,60);
	return s_ACorDC;
}

s8 UI2050_GetOutput(UI2050_OutputStruct *pStruct)
{
	u32 s_Timeout,i=0;
	UI2050_RecCtn=0;
	USART_SendData(UI2050_COM,'?');
	s_Timeout=0;
	while(UI2050_RecCtn<1)
	{
		s_Timeout++;
		UI2050_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	if(UI2050_pRecBuff[0]!='!')
		return -1;
	UI2050_RecCtn=0;
	USART_SendData(UI2050_COM,'1');
	s_Timeout=0;
	while(UI2050_RecCtn<512)
	{
		s_Timeout++;
		UI2050_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	UI2050_RecCtn=0;
	USART_SendData(UI2050_COM,'!');
	s_Timeout=0;
	while(UI2050_RecCtn<512)
	{
		s_Timeout++;
		UI2050_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	UI2050_RecCtn=0;
	USART_SendData(UI2050_COM,'!');
	s_Timeout=0;
	while(UI2050_RecCtn<40)
	{
		s_Timeout++;
		UI2050_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	UI2050_Char2FloatStruct(pStruct,UI2050_pRecBuff,40);
	return 1;
}



s8 UI2050_GetIO(UI2050_IOStruct *pStruct)
{
	s8 err1,err2;
	err1=UI2050_GetInput(&(pStruct->Input));
	err2=UI2050_GetOutput(&(pStruct->Output));
  if(err1==-1)
		return -2;
	else if(err2==-1)
		return -3;
	else if(err1==-1 && err2==-1)
		return  -1;
	else if(err1==1 && err2==1)
	  return  1;
}



/************
s8 UI2050_GetStart(UI2050_StartStruct *pStruct)
{
	u32 s_Timeout,i=0;
	UI2050_RecCtn=0;
	USART_SendData(UI2050_COM,'?');
	s_Timeout=0;
	while(UI2050_RecCtn<1)
	{
		s_Timeout++;
		UI2050_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	
	if(UI2050_pRecBuff[0]!='!')
		return -1;
	UI2050_RecCtn=0;
	USART_SendData(UI2050_COM,'3');
	s_Timeout=0;
	while(UI2050_RecCtn<800)
	{
		s_Timeout++;
		UI2050_DelayMs(1);
		//LcdPrintf(0,15*16,0xffff,0,"ctn=%d,b0=%c",UI2050_RecCtn,UI2050_pRecBuff[0]);
		if(s_Timeout>TIMEOUTMS*2)
			return -1;
	}
	//LcdPrintf(0,15*16,0xffff,0,"ctn=%d,b0=%c",UI2050_RecCtn,UI2050_pRecBuff[0]);
	UI2050_RecCtn=0;
	USART_SendData(UI2050_COM,'!');
	s_Timeout=0;
	while(UI2050_RecCtn<800)
	{
		s_Timeout++;
		UI2050_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	UI2050_RecCtn=0;
	USART_SendData(UI2050_COM,'!');
	s_Timeout=0;
	while(UI2050_RecCtn<28)
	{
		s_Timeout++;
		UI2050_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	UI2050_Cpy2Struct((float*)pStruct,UI2050_pRecBuff,7);
	return 1;
}
*********/











