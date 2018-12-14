
#include "UI2050.h"
#include "UI2000.h"
#include "PowerlineCarrier.h"
#include "ucos_ii.h"   //用到OSTimeDlyHMSM(0,0,0,x)
#include "stm32f10x.h"
#include "M971x.h"
#include "cal_test.h"
#include "CommonFunction.h"
#include "Flash.h"


#include "UpperComputer.h"

#include "DIALOG.h"
#include "WM.h"

#include "ADC.h"
#include <math.h>

#include "RA8875.h"

#include "LCD12864.h"
#define UI_IfExit() if(Exit==PLC_EXIT)return 1
#define DelayMs(x)       OSTimeDlyHMSM(0,0,0,x)
//#define DelayS(x)        OSTimeDlyHMSM(0,0,x,0)

#define DelayS(x)        {u16 jjii;\
                          for(jjii=0;jjii<(x)*10;jjii++)\
                          {DelayMs(100);UI_IfExit();}}

#define CONNET_MAX_TIMES 5
#define PLC_EXIT        0x01
#define START           0x0101

extern unsigned char g_ucLcdLine1;
extern unsigned char g_ucLcdLine2;
extern unsigned char g_ucLcdLine3;
extern unsigned char g_ucLcdLine4;
extern unsigned char g_ucErrorNo;
static float *pFloatMsg;
static u16 *pU16Msg;

#define Msg_Key pU16Msg[0]
#define Handdle pU16Msg[1]
#define Step    pU16Msg[2]
#define Exit    pU16Msg[3]
#define Msg_Err pU16Msg[4]

#define MsgID   pFloatMsg[5]
#define Vol1    pFloatMsg[6]
#define Vol2    pFloatMsg[7]
#define Vol3    pFloatMsg[8]
#define Vol4    pFloatMsg[9]
#define Vol5    pFloatMsg[10]

#define ID      Float2Long(MsgID)










#define STEP_PRESSTART 1
#define STEP_TRY_AGAIN 2
#define STEP_GET_ID    3
#define STEP_GET0V     4
#define STEP_GET25V    5
#define STEP_GET50V    6
#define STEP_GET75V    7
#define STEP_GET100V   8
#define STEP_OVERRANGE 9





static s8 TurnLight(u8 bright)
{
	u8 i;
	for(i=0;i<CONNET_MAX_TIMES;i++)
	{
		if(PLC_TurnLight(ID,bright,&Exit)==1)
			return 1;
		DelayS(1);
		UI_IfExit();
	}
	return -1;
}



static char SendMsg2UI(u8 msgid,u16 _msg)
{
	WM_MESSAGE  Msg;
	Msg.MsgId=msgid;//WM_PAINT;
	Msg.hWin=Handdle;
	Step=_msg;
	if(Exit!=PLC_EXIT)
	{
//		WM_SendMessage(WM_GetClientWindow(Handdle),&Msg);
		return 1;
	}
	return -1;
}


static s8 GetID(void)
{
	u8 i;
	//SendMsg2UI(WM_PAINT,STEP_GET_ID);
	for(i=0;i<CONNET_MAX_TIMES;i++)
	{
		if(PLC_GetID((u32*)(&MsgID),&Exit)==1)
			return 1;
		UI_IfExit();
		DelayS(1);
	}
	return -1;
}








//成功返回等待的时间,单位为ms，超过10分钟返回0
//并通知UI
//msg：通知UI的信息
static u32 WaiteKey(u16 msg)
{
	u32 timeout=0;
	DelayMs(50);
	Msg_Key=0x0100;
//  SendMsg2UI(WM_PAINT,msg);
	while(1)
	{
		if(50*20*60*10<timeout++)
			return -1;
		DelayMs(50);
		if(Msg_Key==START)
		{
			Msg_Key=0x0100;
			return timeout*50;
		}
		UI_IfExit();
	}
}


s8 Test0_10V(void *msg)
{
	u8 i;
	const float bright[5]={0,2.5,5.0,7.5,10.0};
	const float error=0.2;
	
	
	pFloatMsg=(float*)msg;
	pU16Msg=(u16*)msg;
	//WaiteKey(STEP_PRESSTART);
	//LcdPrintf(0,16*0,0xffff,0,"E=%03d",Exit);
	g_ucLcdLine2 = GET_ID;
	if(GetID()!=1) //获取ID
	{
		g_ucLcdLine2 = FAIL2GETID;//SendMsg2UI(WM_PAINT,STEP_TRY_AGAIN);
		return -1;
	}
	for(i=0;i<5;i++) //调光测试0~10V
	{
		//SendMsg2UI(WM_PAINT,STEP_GET0V+i);
		g_ucLcdLine2 = TESTING_0_10V;
		if(TurnLight(bright[i]*20)!=1)
		{
			g_ucLcdLine2 = NO_ACK;//SendMsg2UI(WM_PAINT,STEP_TRY_AGAIN);
			g_ucErrorNo = ERROR_NO_ACK;
			return -1;
		}
		DelayS(1);
		pFloatMsg[i+6]=GetADC(1);
		if(fabs(pFloatMsg[i+6]-bright[i])>error)
		{
			Msg_Err=1;
			g_ucLcdLine2 = FAIL2TEST_0_10V;//SendMsg2UI(WM_PAINT,STEP_OVERRANGE);
		}
		UI_IfExit();
    }
	return 1;
}






















