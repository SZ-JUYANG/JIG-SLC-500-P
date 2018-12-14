


#include "eleload.h"
#include "usart.h"
#include <stdio.h>


extern u8 UART4_Buff[];
extern u8 UART4_Ctn;

#define CMD_LEN 25
#define ELE_COM UART4
#define LOAD_ADDR 1
#define pRecBuff UART4_Buff
#define RecCtn  UART4_Ctn


static u8 GetChckSum(u8 *pBuff,u16 Len)
{
	u8 i=0;
	u16 Sum=0;
	for(i=0;i<Len;i++)
	{
		Sum=Sum+pBuff[i];
	}
	return (u8)(Sum&0x00ff);
}

void SendCMD(u8 *pBuff)
{
	u8 i=0;
	for(i=0;i<CMD_LEN;i++)
	{
		USART_SendData(ELE_COM,pBuff[i]);
		while(USART_GetFlagStatus(UART4,USART_FLAG_TXE)==RESET);
	}
	USART_SendData(ELE_COM,GetChckSum(pBuff,CMD_LEN));
}

void SendStaticCMD(u8 _cmd,u32 _value)
{
	u8 cmd[25];
	memset(cmd,0,sizeof(cmd));
	cmd[0]=0xaa;
	cmd[1]=LOAD_ADDR;
	cmd[2]=_cmd;
	cmd[3]=_value;
	cmd[4]=_value>>8;
	cmd[5]=_value>>16;
	cmd[6]=_value>>24;
	SendCMD(cmd);
}


u8 test[10]={255,255,255,255,255,8,8,8,8,8};
u32 RecStaticCMD(u8 _cmd)
{
	u8 cmd[25];
	u8 CheckSum=0;
	u32 Value=0xffffffff;
	memset(cmd,0,sizeof(cmd));
	cmd[0]=0xaa;
	cmd[1]=LOAD_ADDR;
	cmd[2]=_cmd;
	UART4_Ctn=0;
	SendCMD(cmd);
	while(RecCtn<25)
	{
		LcdPrintf(0,20,0xffff,0,"RecCtn=%d ",RecCtn);
	}
	CheckSum=GetChckSum(pRecBuff,CMD_LEN);
	if(CheckSum==pRecBuff[CMD_LEN] && pRecBuff[0]==0xaa && 
		  pRecBuff[1]==LOAD_ADDR && pRecBuff[CMD_LEN]==_cmd)
	{
		Value=(u32)pRecBuff[3]+((u32)pRecBuff[4]<<8)+((u32)pRecBuff[5]<<16)+((u32)pRecBuff[6]<<24);
	}
	return Value;
}




