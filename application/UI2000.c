


//波特率 19200
//测试HID
#include "UI2000.h"
#include "usart.h"
#include "utility.h"
#include "ucos_ii.h"   //用到OSTimeDlyHMSM(0,0,0,x)
#include "RA8875.h"
#include "cal_test.h"




#define PM9805_COM        USART3
#define PM9805_BUFFSIZE 800
u8      PM9805_pRecBuff[PM9805_BUFFSIZE];
u16     PM9805_RecCtn;
#define PM9805_DelayMs(x) OSTimeDlyHMSM(0,0,0,x)
#define TIMEOUTMS         5*1000



static float PM9805_b2f(u8 *f)
{
		union FDAT
		{
			float f1;
			unsigned char c1[4];
		}fdt1;
		fdt1.c1[0] = *f;
		fdt1.c1[1] = *(f+1);
		fdt1.c1[2] = *(f+2);
		fdt1.c1[3] = *(f+3);
		return fdt1.f1;
}

static void PM9805_Char2FloatStruct(void *pDesData,u8 *pSourceData,u16 FloatLen)
{
	u16 i=0;
	float *s_pDesData=(float *)pDesData;
	while(i<FloatLen)
	{
		*(s_pDesData++)=PM9805_b2f(pSourceData);
		pSourceData=pSourceData+4;
		i++;
	}
}

//发送字符串
 static s8 PM9805_SendData(u8 *pData,u8 len)
{
	u8 i=0;
	for(i=0;i<len;i++)
	{
		USART_SendData(PM9805_COM,pData[i]);    //发送一个字节
	    while(USART_GetFlagStatus(PM9805_COM,USART_FLAG_TXE)==RESET);//检测是否发送成功  
		//PLC_IfExit();
	}
	return 1;
}
/************************
返回-1接收失败，1接收成功
*************************/
s8 PM9805_GetInput(PM9805_InputStruct *pStruct)
{
	u8 ucReadPM9805_Order[4] = {0x55, 0x00, 0x10, 0x65};
	u32 s_Timeout,i=0;
	u8 ucCnt, ucSum;
	PM9805_RecCtn=0;
	PM9805_SendData(ucReadPM9805_Order, 4);//USART_SendData(PM9805_COM,'?');   //发送‘？‘
	s_Timeout=0;
	while(PM9805_RecCtn<24) //共接收24个字节
	{
		s_Timeout++;
		PM9805_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return RETURN_FAIL;
	}
    //计算校验
	ucSum = 0;
	for(ucCnt = 0; ucCnt < 23; ucCnt++){
		ucSum += PM9805_pRecBuff[ucCnt];
    } 
	if(ucSum == PM9805_pRecBuff[23]){ //校验通过
		PM9805_Char2FloatStruct(pStruct,&(PM9805_pRecBuff[3]),5);
	}else{
		return RETURN_FAIL;
	}
	//PM9805_Char2FloatStruct(pStruct,PM9805_pRecBuff,40);
	
	return RETURN_SUCCESS;
}



/************************
返回-1接收失败，1接收成功
*************************/
s8 PM9805_GetOutput(PM9805_OutputStruct *pStruct)
{
	u32 s_Timeout,i=0;
	PM9805_RecCtn=0;
	USART_SendData(PM9805_COM,'?');
	s_Timeout=0;
	while(PM9805_RecCtn<1)
	{
		s_Timeout++;
		PM9805_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	if(PM9805_pRecBuff[0]!='!')
		return -1;
	PM9805_RecCtn=0;
	USART_SendData(PM9805_COM,'1');
	s_Timeout=0;
	while(PM9805_RecCtn<32)
	{
		s_Timeout++;
		PM9805_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	PM9805_Char2FloatStruct(pStruct,PM9805_pRecBuff,32);
	PM9805_RecCtn=0;
	USART_SendData(PM9805_COM,'!');
	s_Timeout=0;
	while(PM9805_RecCtn<300)
	{
		s_Timeout++;
		PM9805_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	PM9805_RecCtn=0;
	USART_SendData(PM9805_COM,'!');
	s_Timeout=0;
	while(PM9805_RecCtn<300)
	{
		s_Timeout++;
		PM9805_DelayMs(1);
		if(s_Timeout>TIMEOUTMS)
			return -1;
	}
	return 1;
}



s8 PM9805_GetIO(PM9805_IOStruct *pStruct)
{
	s8 err1,err2;
	err1=PM9805_GetInput(&(pStruct->Input));
	err2=PM9805_GetOutput(&(pStruct->Output));
  if(err1==-1)
		return -2;
	else if(err2==-1)
		return -3;
	else if(err1==-1 && err2==-1)
		return  -1;
	else if(err1==1 && err2==1)
	  return  1;
}





