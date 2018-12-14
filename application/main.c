/***************************简要介绍****************************/

/***************************************************************/	 	 			
#include "bp_config.h"
#include "desktop.h"
#include "GUI.h"
#include "DIALOG.h"
#include "WM.h"
#include <stdio.h>
#include "M971x.h"
#include "UI2050.h"
#include "UI2000.h"
#include "main.h"
#include "GUI_Test.h"
#include "GUI_TestLed.h"
#include "LedInstruction.h"
#include "MachineKey.h"
#include "PowerlineCarrier.h"
#include "RTC.h"
#include "CommonFunction.h"
#include "UpperComputer.h"

#include "ADC.h"
#include "stdio.h"
#include "cal_test.h"
#include "Test0_10V.h"

#include "LCD12864.h"
#include "GP9034T.h"
#include "FlashOperate.h"
void SetDataInput(void);
void SetDataOutput(void);
void *GUICal_GetMsgPiont(void);
void RelaySet(unsigned char ucSelect);
unsigned short int ReadLoraC6000ID(void);
unsigned char CheckReciveData(void);
unsigned int AckMlc(unsigned short int uiLoraID, unsigned char ucCmd);
unsigned char g_ucLcdLine1 = SHOW_NULL;
unsigned char g_ucLcdLine2 = CHECKING_LOAD;//READY;  //LCD显示代码
unsigned char g_ucLcdLine3 = SHOW_NULL;
unsigned char g_ucLcdLine4 = SHOW_NULL;  //LCD显示代码
unsigned long g_ulTestMode;
unsigned char g_ucBeepMode = 0;  //蜂鸣器响的模式： 0：不响； 1：间断响； 2：长鸣
unsigned char g_ucErrorNo = 0;  //错误代码
extern const unsigned char *pucDisTable[];
extern unsigned char g_ucJigLoraId[];
static unsigned char fg_ucLinkState = 2;  //组网标志：0：未组网；1：组网  2:初始
u8 fg_ucAutoTrig = 0; //自动触发
//0X01: 治具读取Lora模块ID；F1：未请求PLC ID；F2：请求PLC ID；F3：请求到PLC ID；
//F4：使用PLC ID成功；F5:使用PLC ID失败；F6：确认成功使用PLC ID; 0X10:等待超时
unsigned char g_ucNoteState = 0X01; //治具的状态； 
unsigned long g_ulPlcID = 0;
unsigned short int g_uiLoraID = 0;

#define MAX_SIZEMSG 25
static float Cal_FloatMsg[MAX_SIZEMSG];
//OS_EVENT *GUI_Mutex; 
/*
*********************************************************************************************************
*                                            TASK STACK 
*                           
*********************************************************************************************************
*/
__align(8) static  OS_STK App_TaskStartStk[APP_TASK_START_STK_SIZE];  //启动任务的堆栈
__align(8) static  OS_STK App_TaskGuiAppStk[APP_TASK_GUIAPP_STK_SIZE];  //GUI示例任务堆栈
__align(8) static  OS_STK App_TaskTouchStk[APP_TASK_TOUCH_STK_SIZE];	//触摸任务堆栈
__align(8) static  OS_STK App_TaskTimeStk[APP_TASK_TIME_STK_SIZE];	//定时任务堆栈
__align(8) static  OS_STK App_TaskTestStk[APP_TASK_TEST_STK_SIZE];	//定时任务堆栈
static  void  App_TaskStart(void *p_arg);
static  void  App_TaskTouch(void *p_arg);//触摸识别任务
static  void  App_TaskGuiApp(void *p_arg); //GUI实例任务
static  void  App_TaskTime(void *p_arg);   //定时任务
static  void  App_TaskTest(void *p_arg);

long TimeCtn;

static UI2050_IOStruct LedTestData;
static TestMsgStruct   TestMsg;


OS_EVENT * pCal_BoxEvent;
//OS_EVENT * p0_10V_BoxEvent;

WM_HWIN Main_H;
int Val;
void AnyKey2Back(void *msg);

void *GUICal_GetMsgPiont(void)
{
	return Cal_FloatMsg;
}
//扫描键盘程序
static unsigned char ScanKey(void)
{
	unsigned char ucKeyData = PRESS_NO;
	if ((READ_KEY(KEY_PORT,KEY1_BIT) == 0) && (READ_KEY(KEY_PORT,KEY2_BIT) == 0)){
	   //组合键
		ucKeyData = PRESS_KEY1ANDKEY2;
	}else{//单键
		if (READ_KEY(KEY_PORT,KEY1_BIT) == 0){
			ucKeyData = PRESS_KEY1;
		}else if (READ_KEY(KEY_PORT,KEY2_BIT) == 0){
			ucKeyData = PRESS_KEY2;
		}else if (READ_KEY(KEY_PORT,KEY3_BIT) == 0){
			ucKeyData = PRESS_KEY3;
		}else if (READ_KEY(KEY_PORT,KEY4_BIT) == 0){
			ucKeyData = PRESS_KEY4;
		}
	}
	return(ucKeyData);
}
///////////////////////////////////
unsigned char ucData = 0x01,ucCnt;
int main(void)
{															
	INT8U  os_err;
	delay_ms(100);
	HardwareInit();//初始化硬件
/////////////////////////////////////////
    Lcd12864_init();  //初始化LCD12864
    delay_ms(100);
    Lcd12864_clear();//清屏
    delay_ms(10);	
/////////////////////////////////////////
	Cal_ADC_Init();
/*  */
//PLC_0_10V_PWM_Calibrate(0X01000001,1,2,3);
/*  */
	OSInit(); //初始化UCOS
	pCal_BoxEvent=OSMboxCreate(NULL);

	TestMsg.pEvent=OSFlagCreate(0,&os_err);
	OSTaskCreate((void (*)(void *)) App_TaskStart,  // 创建启动任务
                          (void          * ) 0,				 
                          (OS_STK        * )&App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],	   //堆栈地址
                          (INT8U           ) APP_TASK_START_PRIO); //优先级20，最高

	OSTaskCreate((void (*)(void *)) App_TaskTouch,  // 创建触摸屏检测任务
                          (void          * ) 0,				 
                          (OS_STK        * )&App_TaskTouchStk[APP_TASK_TOUCH_STK_SIZE - 1],	   //堆栈地址
                          (INT8U           ) APP_TASK_TOUCH_PRIO); //优先级28
																			  
	OSTaskCreate((void (*)(void *)) App_TaskGuiApp,  //创建GUI显示任务
                          &TestMsg,
                          (OS_STK        * )&App_TaskGuiAppStk[APP_TASK_GUIAPP_STK_SIZE - 1],  //堆栈地址
                          (INT8U           ) APP_TASK_GUIAPP_PRIO);			//优先级25
	OSTaskCreate((void (*)(void *)) App_TaskTime,  //创建定时任务
                          (void          * ) 0,
                          (OS_STK        * )&App_TaskTimeStk[APP_TASK_TIME_STK_SIZE-1],  //堆栈地址
                          (INT8U           ) APP_TASK_TIME_PRIO);			//优先级22
	OSTaskCreate((void (*)(void *)) App_TaskTest,  //创建测试任务
                          &TestMsg,
                          (OS_STK        * )&App_TaskTestStk[APP_TASK_TEST_STK_SIZE-1],  //堆栈地址
                          (INT8U           ) APP_TASK_TEST_PRIO);			//优先级23
	OSStart();  
	return(0);
}	
//任务App_TaskStart启动SysTick定时器，然后创建信号量，等待信号量以阻塞自己
static  void  App_TaskStart(void *p_arg)
{
	WM_MESSAGE  Msg;
	INT8U err;
	OS_CPU_SysTickInit();  //启动SysTick定时器，该定时器每次中断进入操作系统服务程序                                           
	OSTimeDlyHMSM(0,0,3,0);
	while(1)
	{
		//Val++;
		Msg.MsgId=WM_PAINT;
	    OSTimeDlyHMSM(0,0,1,0);
	}
}	 

static void _cbCallback0(WM_MESSAGE * pMsg) {}
	
#include "GUI_Cal.h"
	unsigned char g_ucR2;
static  void  App_TaskGuiApp(void *p_arg)	//显示任务
{
	INT8U err;
	u8 ucShowTemp[17] = {0};
	u8 ucCnt;
	//Gui_App(0);
	OSTimeDly(50);//增加延时时间		
	while(1)
	{   
		LED1_TURN;
		if (g_ucErrorNo == 0){
			Lcd12864_string(LINE_1,pucDisTable[g_ucLcdLine1]);
		  Lcd12864_string(LINE_2,pucDisTable[g_ucLcdLine2]);
			Lcd12864_string(LINE_3,pucDisTable[g_ucLcdLine3]);
			Lcd12864_string(LINE_4,pucDisTable[g_ucLcdLine4]);
		}else{
			for(ucCnt = 0;ucCnt<16;ucCnt++){
				ucShowTemp[ucCnt] = pucDisTable[ERROR_NO][ucCnt];
			}
			ucShowTemp[16] = 0;
			ucShowTemp[10] = g_ucErrorNo/10 +0x30;
			ucShowTemp[11] = g_ucErrorNo%10 +0x30;
			//g_ucLcdLine1 = ANY_KEY2BACK;
			Lcd12864_string(LINE_1,pucDisTable[g_ucLcdLine1]);
		  Lcd12864_string(LINE_2,ucShowTemp);
			Lcd12864_string(LINE_3,pucDisTable[g_ucLcdLine3]);
			Lcd12864_string(LINE_4,pucDisTable[g_ucLcdLine4]);
			g_ucBeepMode = 2;
		}
		OSTimeDlyHMSM(0,0,0,200);
		if (g_ucBeepMode == 0){
			BEEP_OFF;
		}else if(g_ucBeepMode == 1){
			BEEP_TURN;
		}else if(g_ucBeepMode == 2){
			BEEP_ON;
		}
	}
}
 
int GUI_TouchX,GUI_TouchY;
static  void  App_TaskTouch(void *p_arg)
{
	unsigned char ucKeyDataTemp;
	u16 *msg;
	const u8 start=1,stop=2,next=3,restart=4;
	unsigned long ulMlcPhyId = 0X06000000 + 60;
	unsigned char ucMlcCnt;
	u8 err;
	OSTimeDly(100);
	
	msg=(u16*)GUICal_GetMsgPiont();
	#define key   msg[0]
	#define exit  msg[2]
	#define model msg[7]
	g_ulTestMode = ReadPhyIdFromFlash(FLASH_TEST_MODE_ADR);
	if (g_ulTestMode > MAX_MODE_VALUE){//默认为手动模式
		WritePhyId2Flash(DEFAULT_MODE, FLASH_TEST_MODE_ADR);
		g_ulTestMode = DEFAULT_MODE;
	}
	if (g_ulTestMode == HAND_MODE || g_ulTestMode == AUTO_MODE){
		RelaySet(1);//自动校正和手动校正时设置初始电压为190V
	}
	ucKeyDataTemp = ScanKey();
 	if (ucKeyDataTemp == PRESS_KEY1ANDKEY2){ //配置：自动模式、手动模式、出厂检测模式
		g_ucLcdLine1 = SETTING_TEST_MODE;
		switch(g_ulTestMode){
			case HAND_MODE:
				g_ucLcdLine2 = NOW_HAND;
			  g_ucLcdLine3 = ASK_AUTO_HAND;
			  g_ucLcdLine4 = ASK_TSET;
				break;
			case AUTO_MODE:
				g_ucLcdLine2 = NOW_AUTO;
			  g_ucLcdLine3 = ASK_AUTO_HAND;
			  g_ucLcdLine4 = ASK_TSET;
				break;
			case TEST_MODE:
				g_ucLcdLine2 = NOW_TSET;
			  g_ucLcdLine3 = ASK_AUTO_HAND;
			  g_ucLcdLine4 = ASK_TSET;
				break;
			case CHECK_ID_MODE:
				g_ucLcdLine2 = CHECK_ZHIJU;
			  g_ucLcdLine3 = ASK_AUTO_HAND;
			  g_ucLcdLine4 = ASK_TSET;
				break;
			default:break;
		}
		while(1){ //等待按键放开
			OSTimeDlyHMSM(0,0,0,30);
      ucKeyDataTemp = ScanKey();
			if(ucKeyDataTemp == PRESS_NO){
				break;
			} 			
		}
		while(1){ 
			OSTimeDlyHMSM(0,0,0,30);
      ucKeyDataTemp = ScanKey();
      if (ucKeyDataTemp != PRESS_NO){
				OSTimeDlyHMSM(0,0,0,30);
				if (ucKeyDataTemp == ScanKey()){ //去抖动
					while(1){ //等待按键放开
						OSTimeDlyHMSM(0,0,0,20);
						if (ScanKey() == PRESS_NO){ //等待按键放开
							break;
						}
			    }
					if(ucKeyDataTemp == PRESS_KEY1){ //手动
						if (g_ulTestMode != HAND_MODE){//默认为手动模式
							WritePhyId2Flash(HAND_MODE, FLASH_TEST_MODE_ADR);
							g_ulTestMode = HAND_MODE;
						}
						break;
					}else if(ucKeyDataTemp == PRESS_KEY2){ //自动
						if (g_ulTestMode != AUTO_MODE){//默认为手动模式
							WritePhyId2Flash(AUTO_MODE, FLASH_TEST_MODE_ADR);
							g_ulTestMode = AUTO_MODE;
						}
						break;
					}else if(ucKeyDataTemp == PRESS_KEY3){ //出厂设置模式
						if (g_ulTestMode != TEST_MODE){
							WritePhyId2Flash(TEST_MODE, FLASH_TEST_MODE_ADR);
							g_ulTestMode = TEST_MODE;
						}
						break;
					}else if(ucKeyDataTemp == PRESS_KEY4){ //查询模式
//						for(ucMlcCnt = 0;ucMlcCnt < 155; ucMlcCnt++){
//						    //GP9034T_Print(ulMlcPhyId + ucMlcCnt);
//						    //OSTimeDlyHMSM(0,0,3,0);
//						}
						if (g_ulTestMode != CHECK_ID_MODE){
							WritePhyId2Flash(CHECK_ID_MODE, FLASH_TEST_MODE_ADR);
							g_ulTestMode = CHECK_ID_MODE;
						}
						break;
					}
				}
      }				
		}
	}

	if (g_ulTestMode == CHECK_ID_MODE){
		g_ucLcdLine1 = CHECK_SHOW_1;
		g_ucLcdLine2 = CHECK_SHOW_2;
		g_ucLcdLine3 = CHECK_SHOW_3;
		g_ucLcdLine4 = CHECK_SHOW_4;
	}else{
		g_ucLcdLine1 = SHOW_NULL;
		g_ucLcdLine2 = LINKING;
		g_ucLcdLine3 = SHOW_NULL;
		g_ucLcdLine4 = SHOW_NULL;
	}
	while(1)
	{  
		ucKeyDataTemp = ScanKey();
		err = 0;
		if (ucKeyDataTemp != PRESS_NO){
			OSTimeDlyHMSM(0,0,0,30);   //去抖动
			if (ucKeyDataTemp == ScanKey()){
				while(1){
					OSTimeDlyHMSM(0,0,0,20);
					if (ScanKey() == PRESS_NO){ //等待按键放开
						err = 1;
						key = ucKeyDataTemp;
						break;
					}
				}
			}
		}
		if(g_ulTestMode == HAND_MODE || g_ulTestMode == AUTO_MODE || g_ulTestMode == TEST_MODE){
			if(fg_ucLinkState == 2){ //触发组网
				fg_ucLinkState = 0;
				err = 1;
				key = KEYDATA_TRIG;
			}
			if(fg_ucAutoTrig == 1){
				fg_ucAutoTrig = 0;
				err = 1;
				if (g_ulTestMode == AUTO_MODE){
					key = PRESS_KEY1;
				}else if(g_ulTestMode == TEST_MODE){
					key = PRESS_KEY2;
				}
			}
		}
		if(err==1)
		{ g_ucR2 = key;
			OSMboxPost(pCal_BoxEvent,msg);
		}
		OSTimeDlyHMSM(0,0,0,10);
	}
}				  

#include "RTC.h"
extern u8 UC_RecBuff[5];
extern u8 UC_Couter;
void RTC_Config(void);
static u32 STime=0,CTime=0;
extern unsigned char  g_ucC6000_RcvFlag; 
static void App_TaskTime(void *p_arg) //定时任务
{
	s8 err;
  u16 *msg;
	static unsigned char s_ucCnt = 0;
	static unsigned char s_ucLoopTime = 0; //
	static unsigned char s_ucSendEn = 0;   //发送使能
	static unsigned char s_ucSendTimes = 0; //发送的次数
	static unsigned short int s_uiTime = 0;
	static unsigned char s_ucCmd = 0;      //
	const u8 start=1,stop=2,next=3,restart=4;
	RTC_Config();	
	msg=(u16*)GUICal_GetMsgPiont();
	#define key   msg[0]
	#define exit  msg[2]
	#define model msg[7]
	while(1)
	{
		if (s_ucSendEn == 0){       //没有发送任务的时候判断接收
		  err = CheckReciveData();//接收来自UART的命令
		}else{
			err = 0;
		}
		switch(g_ucNoteState){
			case 0x01:
				if (err==0x11){
					LED3_ON;
					g_ucNoteState = 0xF1;
				}else{
					s_ucCnt++;
					if (s_ucCnt >= 10){
						s_ucCnt = 0;
						ReadLoraC6000ID(); //
					}
					LED3_TURN;
				}
				break;
			case 0xf1:
				if (err != 0){
					LED3_TURN;
				}
				if (err == 0X01){ //收到心跳
					s_ucCmd = 0xf1;
					s_ucSendEn = 1;
				}
				break;
			case 0xf2:
			    if (err != 0){
					LED3_TURN;
				}
				if (err == 0X01){ //收到心跳
					if (s_ucSendTimes < 4){
						s_ucSendTimes++;
						s_uiTime = 0;
					}else{
						g_ucNoteState = 0X10;
					}
					s_ucCmd = 0xf2;
				    s_ucSendEn = 1;
				}else if(err == 0x02){ //收到网关发过来的ID
					g_ucNoteState = 0xF3;
				}
				if (s_uiTime < 200){ //20s
				    if (++s_uiTime >= 200){
						g_ucNoteState = 0X10;
					}
				}
				break;
			case 0xf3:
				if (err != 0){
					LED3_TURN;
				}
				if (err == 0X01){ //收到心跳
					s_ucCmd = 0xf3;
					s_ucSendEn = 1;
				}
				break;
			case 0xf4:
				if (err != 0){
					LED3_TURN;
				}
				if (err == 0X01){ //收到心跳
					if (s_ucSendTimes < 4){
						s_ucSendTimes++;
						s_uiTime = 0;
					}else{
						g_ucNoteState = 0X10;
					}
					//发送成功使用PLC ID
					s_ucCmd = 0xf4;
				    s_ucSendEn = 1;
				}else if(err == 0x03){ //收到网关发过来的确认分配ID
					g_ucNoteState = 0xF6;
				}
				if (s_uiTime < 200){ //20s
				    if (++s_uiTime >= 200){
						g_ucNoteState = 0X10;
					}
				}
				break;
			case 0xf5:
				if (err != 0){
					LED3_TURN;
				}
				if (err == 0X01){ //收到心跳
					if (s_ucSendTimes < 4){
						s_ucSendTimes++;
						s_uiTime = 0;
					}else{
						g_ucNoteState = 0X10;
					}
					//发送使用PLC ID失败
					s_ucCmd = 0xf5;
				    s_ucSendEn = 1;
				}else if(err == 0x03){ //收到网关发过来的确认分配ID
					g_ucNoteState = 0xF6;
				}
				if (s_uiTime < 200){ //20s
				    if (++s_uiTime >= 200){
						g_ucNoteState = 0X10;
					}
				}
				break;
			case 0xf6:
				if (err != 0){
					LED3_TURN;
				}
				if (err == 0X01){ //收到心跳
					s_ucCmd = 0xf6;
					s_ucSendEn = 1;
				}
				break;
			case 0x10:
				if (err != 0){
					LED3_TURN;
				}
			default:break;
		}
		if (fg_ucLinkState == 1){
		    LED2_TURN;
		}
		OSTimeDlyHMSM(0,0,0,100);
		if (s_ucSendEn == 1){
			s_ucLoopTime++;
			if (s_ucLoopTime >= 3){
				AckMlc(g_uiLoraID,s_ucCmd);
				s_ucSendEn = 0;
				s_ucLoopTime = 0;
			}
		}
	}
}

#include "CommonFunction.h"
#define NEXT_MSG  (WM_USER+1)	
s8 Cal_Other(void *msg);
s8 GetLink(void *msg);
s8 CheckLoad(void);

static void LookUpParm(float *p_osfmsg, s8 cRetrnValue)
{   u8 ucTemp1, ucTemp2;
	u16 *p_osimsg;
	u8 ucLcdLineTemp[4];
	p_osimsg=(u16*)p_osfmsg;
	if (cRetrnValue == RETURN_LOOK_PARM){
		while(1){
			p_osimsg[0]= PRESS_NO;
			AnyKey2Back(p_osfmsg);
			ucTemp1 = g_ucBeepMode;
			ucTemp2 = g_ucErrorNo;
			ucLcdLineTemp[0] = g_ucLcdLine1;
			ucLcdLineTemp[1] = g_ucLcdLine2;
			ucLcdLineTemp[2] = g_ucLcdLine3;
			ucLcdLineTemp[3] = g_ucLcdLine4;
			g_ucBeepMode = 0;
			g_ucErrorNo = 0;
			if (cRetrnValue == RETURN_LOOK_PARM){
				g_ucLcdLine1 = FACT_TEST;
				g_ucLcdLine2 = VOL_SHOW;
				g_ucLcdLine3 = CUR_SHOW;
				g_ucLcdLine4 = POW_SHOW;
			}else{
				g_ucLcdLine1 = SET_FACT;
				g_ucLcdLine2 = ZERO2TEN;
				g_ucLcdLine3 = PWM_PERCENT;
				g_ucLcdLine4 = SHOW_NULL;
			}
			OSTimeDlyHMSM(0,0,0,800);
			AnyKey2Back(p_osfmsg);
			g_ucBeepMode = ucTemp1;
			g_ucErrorNo = ucTemp2;
			g_ucLcdLine1 = ucLcdLineTemp[0];
			g_ucLcdLine2 = ucLcdLineTemp[1];
			g_ucLcdLine3 = ucLcdLineTemp[2];
			g_ucLcdLine4 = ucLcdLineTemp[3];
			OSTimeDlyHMSM(0,0,0,800);
		}
	}else if(cRetrnValue == RETURN_SUCCESS || cRetrnValue == RETURN_SIGNAL_OUT){
		while(1){ //校验成功后可以查看输入测试参数和0~10V和PWM
			p_osimsg[0]= PRESS_NO;
			AnyKey2Back(p_osfmsg);
			p_osimsg[0]= PRESS_NO;
			ucTemp1 = g_ucBeepMode;
			ucTemp2 = g_ucErrorNo;
			ucLcdLineTemp[0] = g_ucLcdLine1;
			ucLcdLineTemp[1] = g_ucLcdLine2;
			ucLcdLineTemp[2] = g_ucLcdLine3;
			ucLcdLineTemp[3] = g_ucLcdLine4;
			g_ucBeepMode = 0;
			g_ucErrorNo = 0;
			g_ucLcdLine1 = FACT_TEST;
			g_ucLcdLine2 = VOL_SHOW;
			g_ucLcdLine3 = CUR_SHOW;
			g_ucLcdLine4 = POW_SHOW;
			OSTimeDlyHMSM(0,0,0,800);
			AnyKey2Back(p_osfmsg);
			p_osimsg[0]= PRESS_NO;
			g_ucLcdLine1 = SET_FACT;
			g_ucLcdLine2 = ZERO2TEN;
			g_ucLcdLine3 = PWM_PERCENT;
			g_ucLcdLine4 = SHOW_NULL;
			OSTimeDlyHMSM(0,0,0,800);
			AnyKey2Back(p_osfmsg);
			p_osimsg[0]= PRESS_NO;
			g_ucBeepMode = ucTemp1;
			g_ucErrorNo = ucTemp2;
			g_ucLcdLine1 = ucLcdLineTemp[0];
			g_ucLcdLine2 = ucLcdLineTemp[1];
			g_ucLcdLine3 = ucLcdLineTemp[2];
			g_ucLcdLine4 = ucLcdLineTemp[3];
			OSTimeDlyHMSM(0,0,0,800);
		}
	}else if(cRetrnValue == RETURN_FAIL){
		g_ucBeepMode = 2;
    }else if(RETURN_NO_ID == cRetrnValue){    //产品没有分配ID
		g_ucBeepMode = 2;
		g_ucLcdLine1 = NOT_CHECK;
		g_ucLcdLine2 = NOT_CHECK;
		g_ucLcdLine3 = NOT_CHECK;
		g_ucLcdLine4 = NOT_CHECK;
	}
}
/****************************
int 0:pkey, 1:key_value, 2,3:测试项目, 4,5:ID,
6:句柄 ,7,8,9:保留,  10,11:输入功率 
******************************/
static void App_TaskTest(void *p_arg)
{
	INT8U err;
	s8 cRetrnValue;
	u8 ucTemp1, ucTemp2;
	float *p_osfmsg;
	float point[20];
	u16 *p_osimsg;
	u8 msg[10];
	PLC_PowerData powerdata;
	u32 ID;
	u32 PhyID;
	u16 step=0;
	s8 cTemp;
	WM_MESSAGE  Msg;
	#define Msg_Handle  p_osimsg[6]
  //PLC_TurnLight(0x00000000,30,msg);
	PhyID = ReadPhyIdFromFlash(FLASH_SAVE_ADR);
	if (PhyID == 0xffffffff){
		WritePhyId2Flash(PHY_ID_START, FLASH_SAVE_ADR);
	}
	while(1)
	{   
		p_osfmsg=(float*)OSMboxPend(pCal_BoxEvent,0,&err); //等待消息
		OSTimeDlyHMSM(0,0,0,100);
	  p_osimsg=(u16*)p_osfmsg;
		if(p_osimsg[0] == PRESS_KEY1) //按键1
		{ g_ucBeepMode = 0;
			if (g_ulTestMode == HAND_MODE || g_ulTestMode == AUTO_MODE){
				if (RETURN_FAIL == CheckLoad()){ //检测到灯不亮
					while(1){
						OSTimeDlyHMSM(0,0,0,500);
					}
				}
				p_osimsg[0]= PRESS_NO;
				cRetrnValue = CalTest_Cal(p_osfmsg);
				LookUpParm(p_osfmsg, cRetrnValue);
				while(1){
					p_osimsg[0]= PRESS_NO;
					OSTimeDlyHMSM(0,0,0,800);
				}
			}else if(g_ulTestMode == CHECK_ID_MODE){  //查詢治具模式
				CheckPhyID(1);
				p_osimsg[0]= PRESS_NO;
				while(1){
					if (p_osimsg[0] != PRESS_NO){
						CheckPhyID(0);
						break;
					}
					OSTimeDlyHMSM(0,0,0,100);
				}
			}
			p_osimsg[0]= PRESS_NO;
	  }else if(p_osimsg[0] == PRESS_KEY2/*p_osimsg[0]<0x01ff*/){//按键2
			g_ucBeepMode = 0;
			if (g_ulTestMode == TEST_MODE){
				if (RETURN_FAIL == CheckLoad()){ //检测到灯不亮
					while(1){
						OSTimeDlyHMSM(0,0,0,500);
					}
				}
				p_osimsg[0] = PRESS_NO;
				cTemp = ProductParmCheck(p_osfmsg, 1);  //出厂测试
				LookUpParm(p_osfmsg, cTemp);
				while(1){
					OSTimeDlyHMSM(0,0,0,500);
				}
			}else if(g_ulTestMode == HAND_MODE){
				if (RETURN_FAIL == CheckLoad()){ //检测到灯不亮
					while(1){
						OSTimeDlyHMSM(0,0,0,500);
					}
				}
				p_osimsg[0] = PRESS_NO;
				cRetrnValue = ProductCheck(p_osfmsg);
				if(RETURN_SUCCESS == cRetrnValue){
					g_ucBeepMode = 1;
				}
				LookUpParm(p_osfmsg, cRetrnValue);
			}else if(g_ulTestMode == CHECK_ID_MODE){  //查詢治具模式
				CheckPhyID(2);
				p_osimsg[0]= PRESS_NO;
				while(1){
					if (p_osimsg[0] != PRESS_NO){
						CheckPhyID(0);
						break;
					}
					OSTimeDlyHMSM(0,0,0,100);
				}
			}
			p_osimsg[0] = PRESS_NO;
		}else if(p_osimsg[0] == PRESS_KEY3/*p_osimsg[0]<0x01ff*/){  //按键3
			g_ucBeepMode = 0;
			while(1)
			{
				if(g_ulTestMode == HAND_MODE){
					ReadProductPhyID(p_osfmsg);
					p_osimsg[0] = PRESS_NO;
					AnyKey2Back(p_osfmsg);
					g_ucErrorNo = PRESS_NO;
					g_ucLcdLine1 = KEY1_FUNCTION;
					g_ucLcdLine2 = KEY2_FUNCTION;
					g_ucLcdLine3 = KEY3_FUNCTION;
					g_ucLcdLine4 = KEY4_FUNCTION;
					OSTimeDlyHMSM(0,0,0,500);
					//打印集中器ID
					/*GP9034T_Print2(0X06000063); //打印ID
					OSTimeDlyHMSM(0,0,1,500);
					GP9034T_Print2(0X0600005E); //打印ID
					OSTimeDlyHMSM(0,0,1,500);*/
				}
				p_osimsg[0] = PRESS_NO;
				break;
			}
		}
		else if(p_osimsg[0] == PRESS_KEY4) //按键4
		{   g_ucBeepMode = 0;
			while(1)
			{
				if(g_ulTestMode == HAND_MODE){
					p_osimsg[0] = PRESS_NO;      //清除按键值
					Cal_Other(p_osfmsg);  //按键功能
					p_osimsg[0] = PRESS_NO;
					AnyKey2Back(p_osfmsg); //等待按键退出
					g_ucErrorNo = 0;
					g_ucLcdLine1 = KEY1_FUNCTION;
					g_ucLcdLine2 = KEY2_FUNCTION;
					g_ucLcdLine3 = KEY3_FUNCTION;
					g_ucLcdLine4 = KEY4_FUNCTION;
				}
				p_osimsg[0] = PRESS_NO;      //清除按键值
				break;
			}
		}else if(p_osimsg[0] == KEYDATA_TRIG)//组网
		{
			p_osimsg[0] = PRESS_NO;
#if   PRODUCT_TYPE == 2
			if (RETURN_SUCCESS == TestNemaRS485()){
		#if RESET_MASTER_MODE == 1
			  MasterReset(); //复位载波机
		#elif RESET_MASTER_MODE == 2
			/* 硬件断电复位NEMA和载波机 */
			  NEMA_OFF;
			  OSTimeDlyHMSM(0,0,1,500);
			  NEMA_ON;
				OSTimeDlyHMSM(0,0,2,500);
				MasterReset(); //复位载波机
		#endif
			}else{
			  while(1){
				  OSTimeDlyHMSM(0,0,0,500);
			  }
      }	
#else
      MasterReset(); /* 复位载波机 */			
#endif
#if VERSION == 2
			if(g_ucNoteState !=0x01){
				g_ucJigLoraId[10] = (unsigned char)(g_uiLoraID>>24) + 0x30;
				g_ucJigLoraId[11] = (unsigned char)(g_uiLoraID>>16) + 0x30;
				g_ucJigLoraId[12] = (unsigned char)(g_uiLoraID>>8) + 0x30;
				g_ucJigLoraId[13] = (unsigned char)(g_uiLoraID) + 0x30;
				g_ucLcdLine1 = JIG_LORA_ID;
			}
#endif
			if (fg_ucLinkState == 0){  //等待组网
				//组网
				if (RETURN_SUCCESS == GetLink(p_osfmsg)){ //
				   fg_ucLinkState = 1;
				   if (g_ulTestMode == AUTO_MODE || g_ulTestMode == TEST_MODE){
					   fg_ucAutoTrig = 1; //组网成功，发出自动测量命令
				   }else{
					   g_ucBeepMode = 1;
				   }
				   g_ucLcdLine1 = KEY1_FUNCTION;
				   g_ucLcdLine2 = KEY2_FUNCTION;
				   g_ucLcdLine3 = KEY3_FUNCTION;
				   g_ucLcdLine4 = KEY4_FUNCTION;
				}else{
					g_ucBeepMode = 2;
					AnyKey2Back(p_osfmsg); //等待按键退出
					g_ucLcdLine1 = SHOW_NULL;
				  g_ucLcdLine2 = LINKING;
				  g_ucLcdLine3 = SHOW_NULL;
				  g_ucLcdLine4 = SHOW_NULL;
					g_ucErrorNo = 0;
					fg_ucLinkState = 2; //再组网
					g_ucBeepMode = 0;
				}
			}
		}	
	}
}

//按任意按键返回子程序
void AnyKey2Back(void *msg)
{
	#define KEY   pMsg[0]
	short int *pMsg;
	pMsg = (short int*)msg;
	while(1){
		if(KEY != PRESS_NO){
			KEY = PRESS_NO;
			break;
		}
		OSTimeDlyHMSM(0,0,0,50);
	}
	g_ucBeepMode = 0;
}








