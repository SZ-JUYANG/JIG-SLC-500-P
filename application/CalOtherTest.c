


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




#define UI_IfExit() if(Msg_Exit==PLC_EXIT)return 1
#define DelayMs(x)       OSTimeDlyHMSM(0,0,0,x)
//#define DelayS(x)        OSTimeDlyHMSM(0,0,x,0)

#define DelayS(x)        {u16 jjii;\
                          for(jjii=0;jjii<(x)*10;jjii++)\
                          {DelayMs(100);UI_IfExit();}}

#define CONNET_MAX_TIMES 5
#define PLC_EXIT        0x01
#define GETPWR          0x0201
#define SETFACT         0x0202

static float *pFloatMsg;
static u16 *pU16Msg;

#define Msg_Key     pU16Msg[0]
#define Msg_Handdle pU16Msg[1]
#define Msg_Step    pU16Msg[2]
#define Msg_Exit    pU16Msg[3]


#define Msg_ID      pFloatMsg[5]
#define Msg_Pi      pFloatMsg[6]
#define Msg_Ui      pFloatMsg[7]
#define Msg_Ii      pFloatMsg[8]


#define this_ID     Float2Long(Msg_ID)



#define STEP_GET_INS   1
#define STEP_TRY_AGAIN 2
#define STEP_GET_ID    3
#define STEP_SETFACT   4
#define STEP_GETOK     5
#define STEP_SETOK     6
#include "LCD12864.h"
extern unsigned char g_ucLcdLine1;
extern unsigned char g_ucLcdLine2;
extern unsigned char g_ucLcdLine3;
extern unsigned char g_ucLcdLine4;
extern unsigned char g_ucErrorNo;
static void InitData(void)
{
	Msg_ID=0;
	Msg_Pi=0;
	Msg_Ui=0;
	Msg_Ii=0;
}

static s8 GetID(void)
{
	u8 i;
	for(i=0;i<CONNET_MAX_TIMES;i++)
	{
		if(PLC_GetID((u32*)(&Msg_ID),&Msg_Exit)==RETURN_SUCCESS)
		{
			if (Float2Long(Msg_ID) != 0xffffffff)
			   return RETURN_SUCCESS;
		}
		UI_IfExit();
		DelayS(1);
	}
	return RETURN_FAIL;
}


s8 static WaiteKey(void)
{
	u16 i;
	for(i=0;i<10*600;i++)
	{
		if(Msg_Key>0x0200)
			return 1;
		UI_IfExit();
		DelayMs(100);
	}
	return -1;
}


static s8 SetFact(void)
{
	u8 i;
	Msg_Key=0;
	DelayMs(100);
	for(i=0;i<CONNET_MAX_TIMES;i++)
	{
		if(PLC_SetFactory(this_ID,0,&Msg_Exit)==1)
		{
			return 1;
		}
		UI_IfExit();
		DelayS(1);
	}
	return -1;
}

static s8 GetInputData(void)
{
	u8 i;
	PLC_PowerData _idata;
	Msg_Key=0;
	for(i=0;i<CONNET_MAX_TIMES;i++)
	{
		if(PLC_GetPowerData(this_ID,&_idata,&Msg_Exit)==1)
		{
			Msg_Pi=_idata.Pa;
			Msg_Ui=_idata.Vi;
			Msg_Ii=_idata.Ii;
			return 1;
		}
		UI_IfExit();
		DelayS(1);
	}
	return -1;
}


char CalTest_GetID(void);
s8 Cal_Other(void *msg) //按键功能
{
  signed char sReturnValue;
  pFloatMsg=(float*)msg;
	pU16Msg=(u16*)msg;
	g_ucLcdLine2 = GET_ID;
  if (GetID()!=1){
		g_ucLcdLine2 = FAIL2GETID;
	  return -1;
	}
	g_ucLcdLine1 = SHOW_NULL;
	g_ucLcdLine2 = FACT_SETTING;//SendMsg2UI(WM_PAINT,STEP_SETFACT);
	g_ucLcdLine3 = SHOW_NULL;
	g_ucLcdLine4 = SHOW_NULL;
	sReturnValue = SetFact();
	g_ucLcdLine1 = ANY_KEY2BACK;
	if (1 == sReturnValue){
		g_ucLcdLine2 = SUCCESS_FACT_SET;//SendMsg2UI(WM_PAINT,STEP_SETOK);
	}else{
		g_ucErrorNo = ERROR_FAIL2SETFACT;
		g_ucLcdLine2 = FAIL_FACT_SETTING;//SendMsg2UI(WM_PAINT,STEP_TRY_AGAIN);
	}
  return (sReturnValue); 
}
/*******************************************
函数名：GetLink
功能：  通过读取GC9200节点ID的方式判断组网是否成功
return  0:组网超时   1：组网成功
********************************************/
s8 GetLink(void *msg) 
{
  signed char sReturnValue;
	u8 ucCnt;
  pFloatMsg=(float*)msg;
	pU16Msg=(u16*)msg;
	g_ucLcdLine2 = LINKING;
	for(ucCnt = 0; ucCnt < 6; ucCnt++){
		if (GetID()== RETURN_SUCCESS){
			g_ucLcdLine2 = READY;
			return RETURN_SUCCESS;
		}
		OSTimeDlyHMSM(0,0,1,0);
  }
	g_ucLcdLine3 = FAIL2LINK;
	g_ucErrorNo = ERROR_FAIL2LINK;
  return (0); 
}
































