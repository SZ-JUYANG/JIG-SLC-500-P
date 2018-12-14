
#include "UI2050.h"
#include "UI2000.h"
#include "PowerlineCarrier.h"
#include "ucos_ii.h"   //�õ�OSTimeDlyHMSM(0,0,0,x)
#include "stm32f10x.h"
#include "M971x.h"
#include "cal_test.h"
#include "CommonFunction.h"
#include "Flash.h"
#include "UpperComputer.h"
#include "DIALOG.h"
#include "WM.h"
#include "LCD12864.h"
#include "GP9034T.h"
#include "main.h"
#include "FlashOperate.h"
#include "ADC.h"
#define  UI_IfExit() if(Msg_PLCExit==PLC_EXIT)return 1
#define  CalTest_DelayMs(x)  OSTimeDlyHMSM(0,0,0,x)
//#define CalTest_DelayS(x)   OSTimeDlyHMSM(0,0,x,0)

#define CalTest_DelayS(x)   {u16 jjii;\
                            for(jjii=0;jjii<(x)*10;jjii++)\
                            {CalTest_DelayMs(100);UI_IfExit();}}

//У���ĵ���
#define CAL_POINT 5
#define NEXT_MSG  (WM_USER+1)	

//������Դ���
static const u16 CalTest_MaxTimes=5;


//���յ�UI����Ϣָ��
static float *CalTest_pFmsg;
static u16   *CalTest_pImsg;
static const u8 CalTest_PLCExitMsg[2]={1,0};


static PLC_PwrTestFStruct freq;
static PM9805_InputStruct inputvol;
static PM9805_OutputStruct outputvol;

#define LORA_COM        UART4
#define RECIVE_MAX   50

extern u8 g_ucBeepMode;
extern unsigned char g_ucNoteState; //�ξߵ�״̬�� 
extern unsigned long g_ulPlcID;
extern unsigned short int g_uiLoraID;
unsigned int RquirePLC_ID(unsigned short int uiLoraID);
unsigned char ConfirmPLC_ID(unsigned short int uiLoraID,unsigned char ucResult);
unsigned short int ReadLoraC6000ID(void);
unsigned char CheckReciveData(void);
unsigned int AckMlc(unsigned short int uiLoraID, unsigned char ucCmd);
//static WM_HWIN  *CalTest_pHandle;
//#define Msg_Handle  (*CalTest_pHandle)
/******************************************
int 1:key, 2:PLC_exit
*********************************************/
#define CalTest_Key     CalTest_pImsg[0]
#define Msg_Step        CalTest_pImsg[1]
#define Msg_PLCExit     CalTest_pImsg[2]
#define Msg_Err         CalTest_pImsg[3]
#define Msg_TestTimes   CalTest_pImsg[4]
#define Msg_Handle      CalTest_pImsg[6]
#define Msg_Opt         CalTest_pImsg[7]
#define Msg_Model       CalTest_pImsg[8]
#define CalTest_NEXT    0x01
#define PLC_EXIT        0x01

#define Msg_ID      CalTest_pFmsg[6]
#define Exit        CalTest_pImsg[2]

//��Դ������ֵ
#define Msg_InputP  CalTest_pFmsg[7]
#define Msg_InputV  CalTest_pFmsg[8]
#define Msg_InputI  CalTest_pFmsg[9]
#define Msg_OutputV CalTest_pFmsg[10]
#define Msg_OutputI CalTest_pFmsg[11]

//����������ֵ
#define Msg_InputP1  CalTest_pFmsg[12]
#define Msg_InputV1  CalTest_pFmsg[13]
#define Msg_InputI1  CalTest_pFmsg[14]
#define Msg_OutputV1 CalTest_pFmsg[15]
#define Msg_OutputI1 CalTest_pFmsg[16]

#define Msg_P_Feq    CalTest_pFmsg[17]
#define Msg_V_Feq    CalTest_pFmsg[18]
#define Msg_I_Feq    CalTest_pFmsg[19]

#define CalTest_ID  Float2Long(Msg_ID)
#define CalTest_Opt Float2Long(Msg_Opt)

#define STEP_START  1
#define STEP_TRY_AGAIN  2
#define STEP_NEXT    3
#define STEP_GETID  4
#define STEP_GETOPT  5
#define STEP_TURN_VOL1 6
#define STEP_GETIVX1 7
#define STEP_GETIVY1 8
#define STEP_TURN_VOL2 9
#define STEP_GETIVX2 10
#define STEP_GETIVY2 11
#define STEP_CAL_IV 12
#define STEP_GETIPX1 13
#define STEP_GETIPY1 14
#define STEP_GETIPX2 15
#define STEP_GETIPY2 16
#define STEP_CAL_IP 17
#define STEP_GETIIX1 18
#define STEP_GETIIY1 19
#define STEP_GETIIX2 20
#define STEP_GETIIY2 21
#define STEP_CAL_II  22
#define STEP_GETOVX1 23
#define STEP_GETOVY1 24
#define STEP_GETOVX2 25
#define STEP_GETOVY2 26
#define STEP_CAL_OV  27
#define STEP_GETOIX1 28
#define STEP_GETOIY1 29
#define STEP_GETOIX2 30
#define STEP_GETOIY2 31
#define STEP_CAL_OI  32
#define STEP_GET_INS 33
#define STEP_GET_OUS 34
#define STEP_TURN_OV 35
#define STEP_TURN_OI 36
#define STEP_START_CAL 37

static const u8 StepTV[2]={STEP_TURN_VOL1,STEP_TURN_VOL2};
static const u8 StepGIVX[2]={STEP_GETIVX1,STEP_GETIVX2};
static const u8 StepGIPX[2]={STEP_GETIPX1,STEP_GETIPX2};
static const u8 StepGIIX[2]={STEP_GETIIX1,STEP_GETIIX2};
static const u8 StepGIVY[2]={STEP_GETIVY1,STEP_GETIVY2};
static const u8 StepGIPY[2]={STEP_GETIPY1,STEP_GETIPY2};
static const u8 StepGIIY[2]={STEP_GETIIY1,STEP_GETIIY2};

static const u8 StepGOVX[2]={STEP_GETOVX1,STEP_GETOVX2};
static const u8 StepGOIX[2]={STEP_GETOIX1,STEP_GETOIX2};
static const u8 StepGOVY[2]={STEP_GETOVY1,STEP_GETOVY2};
static const u8 StepGOIY[2]={STEP_GETOIY1,STEP_GETOIY2};

typedef struct
{
	u8 opt;
	u8 brightness[2];
	float outputvol[2];
}ModelStruct;

extern unsigned char g_ucLcdLine1;
extern unsigned char g_ucLcdLine2;
extern unsigned char g_ucLcdLine3;
extern unsigned char g_ucLcdLine4;
extern unsigned char g_ucErrorNo;

extern unsigned char  ucID[17];
extern unsigned char  g_ucVol[17];
extern unsigned char  g_ucCur[17];
extern unsigned char  g_ucPower[17];
extern unsigned char  g_ucZero2Ten[17];
extern unsigned char  g_ucPwmPercent[17];
extern unsigned char  g_ucBar[17];
extern unsigned char  g_ucSegNum[];  
extern unsigned char  g_ucSegStart[]; 
extern unsigned char  g_ucSegEnd[];  
extern unsigned char  g_ucIdNow[];  
u8 Data2Char(u8 ucData);
s8 CheckLoad(void);
s8 ProductParmCheck(void *msg, u8 ucMode);
static void LoadPwmPersentData(float fPwm, float fPwm1);
static void LoadZero2TenData(float fU, float fU1);
#ifndef LORA_TEST_EN
	#error "Please define LORA_TEST_EN!"
#elif LORA_TEST_EN == 1
s8 TestLora(void *msg);
s8 PLC_SetLoraTestMode(u32 _id,u8 DeviceCode,void * _msg);
static s8 SetLoraTestMode(void)
{
	u8 i;
	float fTemp;
	for(i=0;i<5;i++)
	{
		if(PLC_SetLoraTestMode(CalTest_ID, DEVICE_CODE, &Exit)==RETURN_SUCCESS){
			return RETURN_SUCCESS;
		}
		OSTimeDlyHMSM(0,0,1,0);
		fTemp = GetADC(1);
		if(2.7<fTemp && fTemp<3.3){ //��⵽NEMA���3V����ʾ�������LORAģʽ
			return RETURN_SUCCESS;
	  }
		UI_IfExit();
	}
	return RETURN_FAIL;
}
#endif
#if PRODUCT_TYPE == 2
	#ifndef RESET_MASTER_MODE
	  #error "Please define RESET_MASTER_MODE!"
	#endif
#endif
static s8 TurnLight(u8 bright)
{
	u8 i;
	for(i=0;i<5;i++)
	{
		if(PLC_TurnLight(CalTest_ID,bright,&Exit)==RETURN_SUCCESS)
			return RETURN_SUCCESS;
		OSTimeDlyHMSM(0,0,1,0);
		UI_IfExit();
	}
	return RETURN_FAIL;
}

//����PWMռ�ձ�
volatile unsigned char g_ucTestPluseEn = 0; //��������ʹ�ܣ� 0����ʹ�ܣ� 1��ʹ��
volatile char g_cPwm = 0;  //ռ�ձ�
volatile float g_fPwm = 0, g_fPwmFrequency;
////////////////
s8 TestPluse(void)
{
	TIM_Cmd(TIM4, ENABLE);
	g_ucTestPluseEn = 1;
	while(g_ucTestPluseEn){
		OSTimeDlyHMSM(0,0,0,5);
	}
	return(g_cPwm);
}

/***
static u32 CalTest_F2L(float _f)
{
	union
		{
			float f;
			u32 l;
		}d;
		d.f=_f;
		return d.l;
}
****/

#include "RA8875.h"


//�ɹ����صȴ���ʱ��,��λΪms������10���ӷ���0
//��֪ͨUI
//msg��֪ͨUI����Ϣ
static u32 CalTest_WaiteKey(u16 msg)
{
	u32 timeout=0;
	CalTest_DelayMs(50);
  //CalTest_SendMsg2UI(WM_PAINT,msg);//����Ϣ��LCD��ʾ����
	while(1)
	{
		if(50*20*60*10<timeout++)
			return RETURN_FAIL;
		CalTest_DelayMs(50);
		if(CalTest_Key==STEP_NEXT || CalTest_Key==STEP_START)  //STEP_START == 1; STEP_NEXT == 3
		{//wzh#define CalTest_Key     CalTest_pImsg[0] --> key
			CalTest_Key=0;  //�����ֵ
			return timeout*50;
		}
		UI_IfExit();
	}
}



char CalTest_GetID(void)
{
	u16 i;
	g_ucLcdLine1 = SHOW_NULL;
	g_ucLcdLine2 = GET_ID;//CalTest_SendMsg2UI(WM_PAINT,STEP_GETID); // ������ʾLCD
	g_ucLcdLine3 = SHOW_NULL;
	g_ucLcdLine4 = SHOW_NULL;
	for(i=0;i<CalTest_MaxTimes;i++) //CalTest_MaxTimes == 5
	{
		if(PLC_GetID((u32*)(&Msg_ID),&Msg_PLCExit)==RETURN_SUCCESS)//��GC9200��ȡ�ڵ�ID
		{
			if (CalTest_ID != 0xffffffff){
			    return RETURN_SUCCESS; 
			}
		}
		UI_IfExit();
		CalTest_DelayS(1); //��ʱ1s
	}
	g_ucLcdLine2 = FAIL2GETID;//CalTest_SendMsg2UI(WM_PAINT,STEP_TRY_AGAIN);
	return RETURN_FAIL;
}

/****************
���
*****************/
static float GetSum(float *p,int l)
{
	float sum=0;
	int i;
	for(i=0;i<l;i++)
	{
		sum+=p[i];
	}
	return sum;
}



//�����Իع鷽��,pk��б�ʣ�pb���ؾ�
static void CalTest_GetKB(float *px,float *py,int len,float *pk,float *pb)
{
/*	float x_,y_;
	float sxx=0,syy=0,sxy=0;
	float sum=0;
	int i;
	if(len==1)
	{
		*pk=(*py)/(*px);
		*pb=0;
	}
	x_=GetSum(px,len)/len;
	y_=GetSum(py,len)/len;
	for(i=0;i<len;i++)
	{
		sxx+=(px[i]-x_)*(px[i]-x_);
	}
	for(i=0;i<len;i++)
	{
		sxy+=(px[i]-x_)*(py[i]-y_);
	}
	*pk=sxy/sxx;
	*pb=y_-(*pk)*x_;
*/
    /* 2��ʽУ�� */
    float fTemp;
    fTemp = (*py - *(py+1))/(*px - *(px+1));
	*pk = fTemp;
	*pb = *py - fTemp * (*px);
}


#define GetSlop(x1,y1,x2,y2) ((y1)-(y2))/((x1)-(x2))


#define GetIntcpt(k,x,y) ((y)-(x)*(k))

/**********************************
�������ܣ����ü̵�����ѡ�������ѹ
���������0: 110; 1:190V; 2: 220V; 3:250V
RELAY3  RELAY2  RELAY1
  0       X       0      110V
  0       X       1      190V
  1       0       X      220V
  1       1       X      250V
**********************************/
void RelaySet(unsigned char ucSelect)
{
	switch(ucSelect){
		case 0x00:
			RELAY1_OFF;
			RELAY3_OFF;
			break;
		case 0x01:
			RELAY1_ON;
			RELAY3_OFF;
			break;
		case 0x02:
			RELAY2_OFF;
			RELAY3_ON;
			break;
		case 0x03:
			RELAY2_ON;
			RELAY3_ON;
			break;
		default:
			break;
	}
}
/**********************************
�������ܣ����ü̵�����ѡ�������ѹ
���������1: 1·����; 2:2·����
**********************************/
void RelayLoadSet(unsigned char ucSelect)
{
	switch(ucSelect){
		case 0x01:
			RELAY_LOAD_OFF;
			break;
		case 0x02:
			RELAY_LOAD_ON;
			break;
		default:
			break;
	}
}
/**********************************
�������ܣ�У�����ƿ����������ѹ
����ֵ��1�ɹ���-1ʧ��
��������������Ĵ�����
          select��bit0����ʾУ������
                  bit1����ʾУ����ѹ
                  bit2����ʾУ������
**********************************/
s8 CalTest_CalODC(u8 times,u8 select)
{
	s8 err;
	u8 i,ii;
	float x[CAL_POINT],y[CAL_POINT];
	float x1[CAL_POINT],y1[CAL_POINT];
	float x2[CAL_POINT],y2[CAL_POINT];
	float k,b;
	float k1,b1;
	float k2,b2;
	float fVoltageTemp;
	unsigned short int ui0_10V_Parm = 1000;
	const u8 CAL_P=1,CAL_VOL=(0x01<<1),CAL_CURRT=(0x01<<2);		
	for(i=0;i<times;i++){
		if(i == 0){
			//RelaySet(1);
		}else if(i==1){
			RelaySet(3);
			CalTest_DelayS(4);
		}
		g_ucLcdLine1 = SHOW_NULL;
		g_ucLcdLine2 = READING_PM9805;
		g_ucLcdLine3 = SHOW_NULL;
		//g_ucLcdLine4 = SHOW_NULL;
		CalTest_DelayS(2);
		for(ii=0;ii<CalTest_MaxTimes;ii++)  //��ȡ��������ֵ
		{
			if(err=PM9805_GetInput(&inputvol),err==RETURN_SUCCESS)
			{
				y[i]=inputvol.Urms;
				y1[i]=inputvol.Irms;
				y2[i]=inputvol.P;
				Msg_InputP1=inputvol.P;
				Msg_InputV1=inputvol.Urms;
				Msg_InputI1=inputvol.Irms;
				break;
			}
			UI_IfExit();
			CalTest_DelayS(1);
		}
//while(1)
//CalTest_DelayS(2);
		if(ii>=CalTest_MaxTimes)
		{
			g_ucLcdLine2 = FAIL2READ_PM9805;//CalTest_SendMsg2UI(WM_PAINT,STEP_TRY_AGAIN);
			g_ucErrorNo = ERROR_FAIL2READ_PM9805;
			return RETURN_FAIL;
		}
		if (inputvol.P < MIN_POWER){
			g_ucErrorNo = EEROR_NO_LOAD;
			return RETURN_FAIL;
		}
		UI_IfExit();
	}
#if S0_10V_CALIBRATE_EN == 1
	/* ����0~10VУ������ */
	fVoltageTemp = GetADC(1); //����0~10V
	if (fVoltageTemp<8 || fVoltageTemp>12){  //0~10V���̫���ף�ֱ�ӱ���
		g_ucLcdLine1 = FAIL2TEST_0_10V;//SendMsg2UI(WM_PAINT,STEP_OVERRANGE);
		g_ucErrorNo = ERROR_FAIL2TEST_0_10V;
		LoadZero2TenData(10, fVoltageTemp);
		return RETURN_SIGNAL_OUT;
	}
	if (fVoltageTemp<9.95 || fVoltageTemp>10.05){   //��10V�����ѹΪУ��
		ui0_10V_Parm = (unsigned short int)(10.0 / fVoltageTemp * 1000);
	}else{  //�ڹ涨��Χ�ڣ�����У׼
		ui0_10V_Parm = 1000;
	}
#endif
	/**/
	if(1/*CAL_VOL & select*/)//У�������ѹ��������������빦��
	{   g_ucLcdLine2 = ADJUST_VOLTAGE_IN;

		for(i=0;i<CalTest_MaxTimes;i++) //У��
		{
			if(PLC_CalInputVIP(CalTest_ID, inputvol.P, inputvol.Urms, inputvol.Irms, ui0_10V_Parm, &Msg_PLCExit)==1)
				break;
			UI_IfExit();
			CalTest_DelayS(1);
		}
		if(i>=CalTest_MaxTimes)
		{   //У��ʧ��
			g_ucErrorNo = ERROR_FAIL2ADJUST_VOLTAGE_IN;
			g_ucLcdLine2 = FAIL2ADJUST_VOLTAGE_IN;//CalTest_SendMsg2UI(WM_PAINT,STEP_TRY_AGAIN);
			return RETURN_FAIL;
		}
	 }
//while(1)
//CalTest_DelayS(2);
	return RETURN_SUCCESS;
}


static u8 CalTest_GetMainModel(u16 _model) //�����豸���жϳ������Ʒ������
{
	if(_model<=24)
	{
		return 1;//LES
	}
	else if(_model<=53)
	{
		return 2;//LLS
	}
	else if(_model<=59)
	{
		return 3;//HID
	}
	else if(_model<=61)
	{
		return 4;//ODC
	}
}



void GUICal_InitMsgData(void);
float FloatAbs(float fdata)
{
	if (fdata<0){
		return (-fdata);
	}else{
		return (fdata);
	}
}
s8 CalTest_Cal(void *msg)  //
{
	u32 ulPhyID, ulPhyID_Temp;
	static s8 err=0,err1=0,err2=0,err3=0;
	u8 i;
	s8 cReturn;
	u16 s_msg[5];
	float point[2]={20,200};
	PLC_PowerData power_insidata;
	u8 mainmodel;
	u8 select;
	u8 ucCnt;
	s8 cTemp;
	u8 ucComSuccessFlag;  //ͨ�ųɹ���־��0��ʧ�ܣ�1���ɹ�
	float fU,fI,fP, fU1,fI1,fP1,fDelta;
	const u8 LES=1,LLS=2,HID=3,ODC=4;
	const u8 IP=0x01,IV=0x01<<1,II=0x01<<2,OV=0x01<<3,OI=0x01<<4;
	const u8 ICA=0x01<<5,IVA=0x01<<4,IPA=0x01<<3;
	unsigned short int uiLoraID = 0;
	unsigned long ulPlcID;
	CalTest_pFmsg=(float*)msg;
	CalTest_pImsg=(u16*)msg;
	mainmodel = ODC;
	select = IP+IV+II;
#if  VERSION == 1
	//�жϱ������ξߵ�ID�Ƿ�ʹ�����
	ulPhyID_Temp = ReadPhyIdFromFlash(FLASH_SAVE_ADR);
	if(ulPhyID_Temp >= PHY_ID_END || ulPhyID_Temp < PHY_ID_START){//IDʹ�ó�������
		g_ucErrorNo = EEROR_PHYID_USE_UP;
		g_ucLcdLine1 = PHYID_USE_UP;
		g_ucLcdLine3 = PHYID_USE_UP;
		g_ucLcdLine4 = PHYID_USE_UP;
		return RETURN_FAIL;
	}
#endif
	//��ȡ��Ʒ��ID
	if(err=CalTest_GetID(),err != RETURN_SUCCESS){   
		g_ucErrorNo = ERROR_FAIL2GETID;
		return RETURN_FAIL;
	}
	ulPhyID = CalTest_ID;
	WritePhyIdToucID(ulPhyID);
	g_ucLcdLine4 = PHY_ID;
	//У�������ѹ������������,0~10V
	if(mainmodel==ODC && select)  { 
		if(CalTest_CalODC(1,select) != RETURN_SUCCESS){
			return RETURN_FAIL;
		}
	} 
	g_ucLcdLine2 = SUCCESS2CHECK;//CalTest_SendMsg2UI(WM_PAINT,STEP_GET_INS);
//==========================================================================================
	//���Խ׶�
	//CalTest_DelayS(2);
	cReturn = ProductCheck(msg);
	if(RETURN_SUCCESS != cReturn){
		return cReturn;
	}
	//����ID
	ulPhyID = CalTest_ID;
	if (ulPhyID != PHY_ID_DEFAULT){
		g_ucLcdLine1 = SHOW_NULL;
	  g_ucLcdLine2 = ASK_SET_PHYID;
	  g_ucLcdLine3 = YES_OR_NO;
	    //g_ucLcdLine4 = SHOW_NULL;
		g_ucBeepMode = 1;
		while(1){
		  if(CalTest_pImsg[0] == PRESS_KEY1){ //���·���ID
				g_ucBeepMode = 0;
				g_ucLcdLine1 = TEST_PASS;
				g_ucLcdLine2 = SETTING_PHYID;
				g_ucLcdLine3 = SHOW_NULL;
	      g_ucLcdLine4 = SHOW_NULL;
			#ifndef VERSION
	      #error "Please define VERSION!"
       #elif  VERSION == 1
				ulPhyID_Temp = ReadPhyIdFromFlash(FLASH_SAVE_ADR);
				if (RETURN_SUCCESS != WritePhyID2Device(ulPhyID, ulPhyID_Temp)){
					g_ucLcdLine1 = FAIL2SET_PHYID;
					g_ucErrorNo = ERROR_FAIL2SET_PHYID;
					return RETURN_FAIL;
				}
				ulPhyID = ulPhyID_Temp;
				ulPhyID_Temp++;
		    WritePhyId2Flash(ulPhyID_Temp, FLASH_SAVE_ADR);
			#elif  VERSION == 2
				//��ȡLoraģ���ID
				for(ucCnt=0;ucCnt<5;ucCnt++){
					if (0xF1 == g_ucNoteState){
						uiLoraID = g_uiLoraID;
						break;
					}
					OSTimeDlyHMSM(0,0,0,500);
				}
				if (0 ==uiLoraID){
					g_ucLcdLine1 = SHOW_NULL;
					g_ucLcdLine2 = GET_LORA_ID_FAIL;
					g_ucLcdLine3 = SHOW_NULL;
					g_ucLcdLine4 = SHOW_NULL;
					g_ucErrorNo =  EEROR_LORA_FAIL;
					return RETURN_FAIL;
				}
				//����������ID
				g_ucNoteState = 0xF2;
				ulPhyID_Temp = 0;
				while(1){//�ȴ����ػظ�ID
					OSTimeDlyHMSM(0,0,0,110);
					if (g_ucNoteState == 0xF3){ //���뵽PLCģ��ID
						ulPhyID_Temp = g_ulPlcID;
						break;
					}
					if (g_ucNoteState == 0x10){ //δ���뵽PLCģ��ID
						ulPhyID_Temp = 0;
						break;
					}
				}
				if (0 == ulPhyID_Temp){
					g_ucLcdLine1 = SHOW_NULL;
					g_ucLcdLine2 = GET_PLC_ID_FAIL;
					g_ucLcdLine3 = SHOW_NULL;
					g_ucLcdLine4 = SHOW_NULL;
					g_ucErrorNo =  EEROR_GET_ID_FAIL;
					return RETURN_FAIL;
				}
				//��IDд���Ʒ
				if (RETURN_SUCCESS != WritePhyID2Device(ulPhyID, ulPhyID_Temp)){
					//��Ӧ��������IDʹ��ʧ��
					ucComSuccessFlag = 0;
					g_ucNoteState = 0xF5;
					while(1){//�ȴ����ػظ�
						OSTimeDlyHMSM(0,0,0,110);
						if (g_ucNoteState == 0xF6){ //�������ظ�F5����ɹ�
							ucComSuccessFlag = 1;
							break;
						}
						if (g_ucNoteState == 0x10){ //�������ظ�F5����ʧ��
							ucComSuccessFlag = 0;
							break;
						}
				    }
					g_ucLcdLine1 = FAIL2SET_PHYID;
					g_ucErrorNo = ERROR_FAIL2SET_PHYID;
					return RETURN_FAIL;
				}
				//ID���óɹ�����Ӧ������
				ucComSuccessFlag = 0;
				g_ucNoteState = 0xF4;
				while(1){//�ȴ����ػظ�
					OSTimeDlyHMSM(0,0,0,110);
					if (g_ucNoteState == 0xF6){ //�������ظ�F4����ɹ�
						ucComSuccessFlag = 1;
						break;
					}
					if (g_ucNoteState == 0x10){ //�������ظ�F4����ʧ��
						ucComSuccessFlag = 0;
						break;
					}
				}
				if (ucComSuccessFlag==0){ //��Ӧ������ʧ�ܣ���ID����ΪĬ��ֵ
					ulPhyID_Temp = PHY_ID_DEFAULT;
					if (RETURN_SUCCESS != WritePhyID2Device(ulPhyID, ulPhyID_Temp)){
						//����ΪĬ��IDʧ��
						g_ucLcdLine1 = SHOW_NULL;
						g_ucErrorNo = EEROR_FEEDBACK_ID_FAIL;
					}else{
						g_ucLcdLine1 = SHOW_NULL;
						g_ucErrorNo = EEROR_RETURN_ID_FAIL;
					}
					return RETURN_FAIL;
				}
				ulPhyID = ulPhyID_Temp;
			#endif
				OSTimeDlyHMSM(0,0,0,500);
				CalTest_pImsg[0] = 0;
			#ifndef PCBA_PRINT_EN
				#error "Please define PCBA_PRINT_EN!"
			#elif PCBA_PRINT_EN == 1
				GP9034T_Print2(ulPhyID); //��ӡID
			#endif
				break;
			}
			if (CalTest_pImsg[0] == PRESS_KEY2){ //ʹ��ԭ����ID
			#if PCBA_PRINT_EN == 1
				g_ucBeepMode = 0;
				CalTest_pImsg[0] = 0;
				WritePhyIdToucID(ulPhyID);
				g_ucLcdLine2 = PHY_ID;
				g_ucLcdLine3 = ASK_PRINT;
				while(1){
					if (CalTest_pImsg[0] == 1){
						g_ucLcdLine3 = PRINTING;
						GP9034T_Print2(ulPhyID);
						OSTimeDlyHMSM(0,0,2,0);
						g_ucLcdLine1 = ANY_KEY2BACK;
						g_ucLcdLine2 = PRINT_FINISH;
						g_ucLcdLine3 = SHOW_NULL;
						g_ucLcdLine4 = SHOW_NULL;
						break;
					}else if(CalTest_pImsg[0] == 2){
						g_ucLcdLine1 = ANY_KEY2BACK;
						g_ucLcdLine2 = SHOW_NULL;
						g_ucLcdLine3 = SHOW_NULL;
						g_ucLcdLine4 = SHOW_NULL;
						break;
					}
					OSTimeDlyHMSM(0,0,0,50);
				}
			#endif
				g_ucLcdLine1 = SHOW_NULL;
	      g_ucLcdLine2 = SHOW_NULL;
			  g_ucLcdLine3 = SHOW_NULL;
	      g_ucLcdLine4 = SHOW_NULL;
				OSTimeDlyHMSM(0,0,0,500);
				CalTest_pImsg[0] = 0;
				break;//return RETURN_SUCCESS;
			}
			OSTimeDlyHMSM(0,0,0,50);
		}
		g_ucBeepMode = 0;
	}else{
		g_ucLcdLine1 = SHOW_NULL;
		g_ucLcdLine2 = SETTING_PHYID;
		g_ucLcdLine3 = SHOW_NULL;
		g_ucLcdLine4 = SHOW_NULL;
	#if VERSION == 1
		ulPhyID_Temp = ReadPhyIdFromFlash(FLASH_SAVE_ADR);
		ulPhyID_Temp++;
		if (RETURN_SUCCESS != WritePhyID2Device(ulPhyID, ulPhyID_Temp)){
			g_ucLcdLine1 = FAIL2SET_PHYID;
			g_ucErrorNo = ERROR_FAIL2SET_PHYID;
			return RETURN_FAIL;
		}
		ulPhyID = ulPhyID_Temp;
		WritePhyId2Flash(ulPhyID_Temp, FLASH_SAVE_ADR);
	#elif VERSION == 2
		//��ȡLoraģ���ID
		for(ucCnt=0;ucCnt<5;ucCnt++){
			if (0xF1 == g_ucNoteState){
				uiLoraID = g_uiLoraID;
				break;
			}
			OSTimeDlyHMSM(0,0,0,500);
		}
		if (0 ==uiLoraID){
			g_ucLcdLine1 = SHOW_NULL;
			g_ucLcdLine2 = GET_LORA_ID_FAIL;
			g_ucLcdLine3 = SHOW_NULL;
			g_ucLcdLine4 = SHOW_NULL;
			g_ucErrorNo =  EEROR_LORA_FAIL;
			return RETURN_FAIL;
		}
		//����������ID
		g_ucNoteState = 0xF2;
		ulPhyID_Temp = 0;
		while(1){//�ȴ����ػظ�ID
			OSTimeDlyHMSM(0,0,0,110);
			if (g_ucNoteState == 0xF3){ //���뵽PLCģ��ID
				ulPhyID_Temp = g_ulPlcID;
				break;
			}
			if (g_ucNoteState == 0x10){ //δ���뵽PLCģ��ID
				ulPhyID_Temp = 0;
				break;
			}
		}
		if (0 == ulPhyID_Temp){
			g_ucLcdLine1 = SHOW_NULL;
			g_ucLcdLine2 = GET_PLC_ID_FAIL;
			g_ucLcdLine3 = SHOW_NULL;
			g_ucLcdLine4 = SHOW_NULL;
			g_ucErrorNo =  EEROR_GET_ID_FAIL;
			return RETURN_FAIL;
		}
		//��IDд���Ʒ
		if (RETURN_SUCCESS != WritePhyID2Device(ulPhyID, ulPhyID_Temp)){
			//��Ӧ��������IDʹ��ʧ��
			ucComSuccessFlag = 0;
			g_ucNoteState = 0xF5;
			while(1){//�ȴ����ػظ�
				OSTimeDlyHMSM(0,0,0,110);
				if (g_ucNoteState == 0xF6){ //�������ظ�F5����ɹ�
					ucComSuccessFlag = 1;
					break;
				}
				if (g_ucNoteState == 0x10){ //�������ظ�F5����ʧ��
					ucComSuccessFlag = 0;
					break;
				}
			}
			g_ucLcdLine1 = FAIL2SET_PHYID;
			g_ucErrorNo = ERROR_FAIL2SET_PHYID;
			return RETURN_FAIL;
		}
		//ID���óɹ�����Ӧ������
		ucComSuccessFlag = 0;
		g_ucNoteState = 0xF4;
		while(1){//�ȴ����ػظ�
			OSTimeDlyHMSM(0,0,0,110);
			if (g_ucNoteState == 0xF6){ //�������ظ�F4����ɹ�
				ucComSuccessFlag = 1;
				break;
			}
			if (g_ucNoteState == 0x10){ //�������ظ�F4����ʧ��
				ucComSuccessFlag = 0;
				break;
			}
		}
		if (ucComSuccessFlag==0){ //��Ӧ������ʧ�ܣ���ID����ΪĬ��ֵ
			ulPhyID_Temp = PHY_ID_DEFAULT;
			if (RETURN_SUCCESS != WritePhyID2Device(ulPhyID, ulPhyID_Temp)){
				//����ΪĬ��IDʧ��
				g_ucLcdLine1 = SHOW_NULL;
				g_ucErrorNo = EEROR_FEEDBACK_ID_FAIL;
			}else{
				g_ucLcdLine1 = SHOW_NULL;
				g_ucErrorNo = EEROR_RETURN_ID_FAIL;
			}
			return RETURN_FAIL;
		}
	#endif
		ulPhyID = ulPhyID_Temp;
		OSTimeDlyHMSM(0,0,0,500);
	#if PCBA_PRINT_EN == 1
		GP9034T_Print2(ulPhyID); //��ӡID
	#endif
  }
	WritePhyIdToucID(ulPhyID);
/* LORA���Կ�ʼ */
#if LORA_TEST_EN == 1
	cReturn = TestLora(msg);
	if(RETURN_SUCCESS != cReturn){
		return cReturn;
	}
#endif
/* LORA���Խ��� */
	g_ucLcdLine1 = ANY_KEY2LOOK;
	g_ucLcdLine2 = TEST_PASS;
  g_ucLcdLine3 = SHOW_NULL;
  g_ucLcdLine4 = PHY_ID;
	g_ucBeepMode = 1;   //���������𣬱�ʾ���Խ���
	return RETURN_SUCCESS;
}
//////////////////////////////////////////////////////
//fU:  ���õ�ѹ ��λV
//fU1: ʵ�ʵ�ѹ ��λV
//fU,fU1 <10V
static void LoadZero2TenData(float fU, float fU1)
{
	g_ucZero2Ten[3] = 0x20; //�ո�
	g_ucZero2Ten[4] = Data2Char((u16)(fU)%10);
	g_ucZero2Ten[5] = 0x2E; //С����
	g_ucZero2Ten[6] = Data2Char((u16)(fU*10)%10);
	g_ucZero2Ten[7] = Data2Char((u16)(fU*100)%10);
	g_ucZero2Ten[8] = 'V'; //��λ
	g_ucZero2Ten[9] = 0x20; //�ո�
	g_ucZero2Ten[10] = Data2Char((u16)(fU1)%10);
	g_ucZero2Ten[11] = 0x2E; //С����
	g_ucZero2Ten[12] = Data2Char((u16)(fU1*10)%10);
	g_ucZero2Ten[13] = Data2Char((u16)(fU1*100)%10);
	g_ucZero2Ten[14] = 'V'; //��λ
	g_ucZero2Ten[15] = 0x20; //�ո�
	g_ucZero2Ten[16] = 0;
}
//////////////////////////////////////////////////////
//fPwm:  ����PWM����� ��λ %
//fPwm1: ʵ��PWM����� ��λ %
static void LoadPwmPersentData(float fPwm, float fPwm1)
{
	g_ucPwmPercent[4] = 0x20; //�ո�
    g_ucPwmPercent[5] = 0x20; //�ո�
	g_ucPwmPercent[6] = Data2Char((u16)(fPwm)%100/10);
	g_ucPwmPercent[7] = Data2Char((u16)(fPwm)%10);
	g_ucPwmPercent[8] = '%'; //��λ
	g_ucPwmPercent[9] = 0x20; //�ո�
	g_ucPwmPercent[10] = Data2Char((u16)(fPwm1)%100/10);
	g_ucPwmPercent[11] = Data2Char((u16)(fPwm1)%10);
	g_ucPwmPercent[12] = 0x2E; //С����
	g_ucPwmPercent[13] = Data2Char((u16)(fPwm1*10)%10);
	g_ucPwmPercent[14] = '%'; //��λ
	g_ucPwmPercent[15] = 0x20; //�ո�
	g_ucPwmPercent[16] = 0;
}
static void ShowBar(u8 ucBar)
{
	g_ucBar[10] = Data2Char((u16)(ucBar)/100);
	g_ucBar[11] = Data2Char((u16)(ucBar)%100/10);
	g_ucBar[12] = Data2Char((u16)(ucBar)%10);
	g_ucBar[13] = '%'; //��λ
	g_ucBar[14] = 0x20; //�ո�
	g_ucBar[15] = 0x20; //�ո�
	g_ucBar[16] = 0;
}
#if LORA_TEST_EN == 1
s8 TestLora(void *msg)
{
	#define WAITING_LORA_TEST_TIME    70 //unit: 0.6s
	float fTemp;//
	u8 err = 0;
	u16 uiCnt;
	u8 uc4vFlag = 0, uc6vFlag = 0,uc7vFlag = 0, uc8vFlag = 0;
	
	g_ucLcdLine1 = CHECKING;
  g_ucLcdLine4 = PHY_ID;
	g_ucLcdLine1 = CHECKING;
	g_ucLcdLine2 = SET_LORA_TEST_MODE;
	/* �ξ����ô����ƷΪLORA����ģʽ */
	if(SetLoraTestMode() != RETURN_SUCCESS){
		g_ucLcdLine1 = NO_ACK;
		g_ucErrorNo = ERROR_NO_ACK;
		return RETURN_FAIL;
	}
	/* �ȴ�LORA���Խ��� */
	g_ucLcdLine2 = LORA_TESTING;
	for(uiCnt = 0; uiCnt < WAITING_LORA_TEST_TIME; uiCnt++){
	  fTemp = GetADC(1);//ADC����һ�δ�Լ0.6s
		if(3.8<fTemp && fTemp<4.2){
			if(uc4vFlag){
				/* ȷ��ʧ���˳� */
				g_ucLcdLine1 = LORA_TEST_FAIL;
		    g_ucErrorNo = EEROR_LORA_FAULT;
				return RETURN_FAIL;
			}
			uc4vFlag = 1;
		}else{
			uc4vFlag = 0;
		}
		if(5.7<fTemp && fTemp<6.3){
			if(uc6vFlag){
				/* ȷ���ɹ��˳� */
				g_ucLcdLine1 = LORA_TEST_SUCCESS;
				break;
			}
			uc6vFlag = 1;
		}else{
			uc6vFlag = 0;
		}
		if(6.7<fTemp && fTemp<7.3){
			if(uc7vFlag){
				/* ȷ��ʧ���˳� */
				g_ucLcdLine1 = LORA_TEST_FAIL;
		    g_ucErrorNo = EEROR_SIMPLE_FAULT;
				return RETURN_FAIL;
			}
			uc7vFlag = 1;
		}else{
			uc7vFlag = 0;
		}
		if(7.7<fTemp && fTemp<8.3){
			if(uc8vFlag){
				/* ȷ��ʧ���˳� */
				g_ucLcdLine1 = LORA_TEST_FAIL;
		    g_ucErrorNo = EEROR_AUTO_FAULT;
				return RETURN_FAIL;
			}
			uc8vFlag = 1;
		}else{
			uc8vFlag = 0;
		}
	}
	/* �ȴ���ʱ��ʧ�� */
	if(uiCnt == WAITING_LORA_TEST_TIME){
		g_ucLcdLine1 = LORA_TEST_FAIL;
		g_ucErrorNo = EEROR_LORA_FAULT;
		return RETURN_FAIL;
	}
	//�������
	//g_ucLcdLine1 = ANY_KEY2LOOK;
	g_ucLcdLine2 = SHOW_NULL;
	g_ucLcdLine3 = SHOW_NULL;
  g_ucLcdLine4 = PHY_ID;
	return RETURN_SUCCESS;
}
#endif
//����������������Ƿ��ڹ涨ֵ��0~10V
s8 ProductCheck(void *msg)
{
	u8 i;
	const float bright[5]={0,2.5,5.0,7.5,10.0};
	const float error = DELTA_0TO10V;//0.5;
	u8 err = 0;
	PM9805_InputStruct inputvol;	
	u16 s_msg[5];
	float point[2]={20,200};
	PLC_PowerData power_insidata;
	u8 mainmodel;
	u8 select;
	u8 ucCnt;
	float fU,fI,fP, fU1,fI1,fP1,fDelta;
	float fTemp,fDelta2;
	s8 cPwmTem;
	s8 cTemp;
	CalTest_pFmsg=(float*)msg;
	CalTest_pImsg=(u16*)msg;
	g_ucLcdLine1 = CHECKING;
	g_ucLcdLine2 = GET_ID;
	if(err=CalTest_GetID(), err != RETURN_SUCCESS)
	{   g_ucErrorNo = ERROR_FAIL2GETID;
		return RETURN_FAIL;
	}
	WritePhyIdToucID(CalTest_ID);
  g_ucLcdLine4 = PHY_ID;
	//�ı为��������
  RelayLoadSet(1);
	RelaySet(2);//����ʱ��220V
	OSTimeDlyHMSM(0,0,3,0);//�ȴ��ȶ�
	//�����������������
	cTemp = ProductParmCheck(msg, 0);
	if(RETURN_SUCCESS != cTemp){ //���Բ�ͨ��
		return cTemp;
	}
	//0~10V,PWM����
	g_ucLcdLine1 = CHECKING;
	g_ucLcdLine2 = TESTING_0_10V;
	for(i=0;i<5;i++) //�������0~10V
	{
		//SendMsg2UI(WM_PAINT,STEP_GET0V+i);
		if(TurnLight(bright[i]*20) != RETURN_SUCCESS)
		{
			g_ucLcdLine1 = NO_ACK;//SendMsg2UI(WM_PAINT,STEP_TRY_AGAIN);
			g_ucErrorNo = ERROR_NO_ACK;
			return RETURN_FAIL;
		}
		OSTimeDlyHMSM(0,0,0,800);//�ȴ�0~10V�ȶ�
		fTemp = GetADC(1);//pFloatMsg[i+6]=GetADC(1);
		if (i == 0){
			//fTemp -= 0.15;
			//���Կ��ع���
			if(err=PM9805_GetInput(&inputvol),err==RETURN_SUCCESS){
				fP = inputvol.P;
				if (fP > NOLOAD_POWER){ //�жϿ��ع���
					g_ucErrorNo = EEROR_NOLOAD_POWER;
					return RETURN_FAIL;
				}
			}else{
				g_ucLcdLine1 = FAIL2READ_PM9805;
		        g_ucErrorNo = ERROR_FAIL2READ_PM9805;
				return RETURN_FAIL;
			}
		}
		if (fTemp-bright[i] < 0){
			fDelta2 = bright[i] - fTemp;
		}else{
			fDelta2 = fTemp - bright[i];
		}
		if(fDelta2 > error){
			Msg_Err=1;
			g_ucLcdLine1 = FAIL2TEST_0_10V;//SendMsg2UI(WM_PAINT,STEP_OVERRANGE);
			g_ucErrorNo = ERROR_FAIL2TEST_0_10V;
			LoadZero2TenData(i*2.5, fTemp);
			return RETURN_SIGNAL_OUT;
		}
#ifndef PRODUCT_TYPE
  #error "Please define PRODUCT_TYPE!"
#elif   PRODUCT_TYPE == 1
		//PWM����
		switch(i){
			case 0:
				ShowBar(0);
				break;
			case 1: //25%
				cPwmTem = TestPluse();
				if(cPwmTem == RETURN_FAIL){
					g_ucErrorNo = EEROR_PWM;
					LoadPwmPersentData(25, 0);
					return(RETURN_SIGNAL_OUT);
				}else{
					if (cPwmTem < 25 - DELTA_PWM || cPwmTem > 25 + DELTA_PWM){
						g_ucErrorNo = EEROR_PWM;
						LoadPwmPersentData(25, g_fPwm);
						return(RETURN_SIGNAL_OUT);
					}
				}
				ShowBar(25);
		        break;
			case 2: //50%
				cPwmTem = TestPluse();
			    //������Գɹ�����ֻ��ʾ50%�����0~10��PWM
			    LoadZero2TenData(i*2.5, fTemp);
			    LoadPwmPersentData(50, g_fPwm);
				if(cPwmTem == RETURN_FAIL){
					g_ucErrorNo = EEROR_PWM;
					return(RETURN_SIGNAL_OUT);
				}else{
					if (cPwmTem < 50 - DELTA_PWM || cPwmTem > 50 + DELTA_PWM){
						g_ucErrorNo = EEROR_PWM;
						return(RETURN_SIGNAL_OUT);
					}
				}
				ShowBar(50);
				break;
			case 3: //75%
				cPwmTem = TestPluse();
				if (cPwmTem == RETURN_FAIL){
					g_ucErrorNo = EEROR_PWM;
					LoadPwmPersentData(75, 0);
					return(RETURN_SIGNAL_OUT);
				}else{
					if (cPwmTem < 75 - DELTA_PWM || cPwmTem > 75 + DELTA_PWM){
						g_ucErrorNo = EEROR_PWM;
						LoadPwmPersentData(75, g_fPwm);
						return(RETURN_SIGNAL_OUT);
					}
				}
				ShowBar(75);
				break;
			case 4:
				ShowBar(100);
				break;
			default:break;
		}
#elif   PRODUCT_TYPE == 2  /* NEMA-SLC has not PWM signal wire. */
		switch(i){
			case 0:
				ShowBar(0);
				break;
			case 1: //25%
				ShowBar(25);
		        break;
			case 2: //50%
			    LoadZero2TenData(i*2.5, fTemp);
			    LoadPwmPersentData(0, 0);
				ShowBar(50);
				break;
			case 3: //75%
				ShowBar(75);
				break;
			case 4:
				ShowBar(100);
				break;
			default:break;
		}
#endif
		g_ucLcdLine3 = SHOW_BAR;
		UI_IfExit();
    }
	//�������
	g_ucLcdLine1 = ANY_KEY2LOOK;
	g_ucLcdLine2 = SHOW_NULL;
	g_ucLcdLine3 = SHOW_NULL;
  g_ucLcdLine4 = PHY_ID;
	return RETURN_SUCCESS;
}
//���������������
s8 ProductParmCheck(void *msg, u8 ucMode)
{//ucMode: 0: ����������������   1��������⣨�������ID�Ƿ����ã�
	u8 i;
	const float bright[5]={0,2.5,5.0,7.5,10.0};
	const float error = DELTA_0TO10V;
	u8 err = 0;
	s8 cTemp;
	PM9805_InputStruct inputvol;
	u32 ulPhyID;
	u16 s_msg[5];
	float point[2]={20,200};
	PLC_PowerData power_insidata;
	u8 mainmodel;
	u8 select;
	u8 ucCnt;
	float fU,fI,fP, fU1,fI1,fP1,fDelta;
	float fTemp,fDelta2;
	char  cReturn;
#ifndef FACTORY_TEST_SIGNAL_EN
	#error "Please define FACTORY_TEST_SIGNAL_EN!"
#elif FACTORY_TEST_SIGNAL_EN == 1
	char  cPwmTem;
#endif
	CalTest_pFmsg=(float*)msg;
	CalTest_pImsg=(u16*)msg;
  if (ucMode == 1){ //���������Ҫ���ӵ�
		g_ucLcdLine1 = CHECKING;
		g_ucLcdLine2 = GET_ID; 
		//�ı为��������
    RelayLoadSet(1);
		if (err=CalTest_GetID(), err != RETURN_SUCCESS)
		{   g_ucErrorNo = ERROR_FAIL2GETID;
			return RETURN_FAIL;
		}
		ulPhyID = CalTest_ID;
		if (CalTest_ID == PHY_ID_DEFAULT){
			g_ucErrorNo = ERROR_NOT_CHECK;
			return RETURN_NO_ID;
		}
		g_ucLcdLine1 = CHECKING;
		g_ucLcdLine2 = TEST_NOLOAD_POWER; 
		WritePhyIdToucID(ulPhyID);
		g_ucLcdLine4 = PHY_ID;
		//�صƲ������ع���
		if(TurnLight(0) != RETURN_SUCCESS){
			g_ucLcdLine2 = NO_ACK;//SendMsg2UI(WM_PAINT,STEP_TRY_AGAIN);
			g_ucErrorNo = ERROR_NO_ACK;
			return RETURN_FAIL;
		}
		CalTest_DelayS(1); //��ʱ���ܿ�����PLCͨ����ɵĹ��ʲ���
		if(err=PM9805_GetInput(&inputvol),err==RETURN_SUCCESS){
			fP = inputvol.P;
			if (fP > NOLOAD_POWER){ //�жϿ��ع���
				g_ucErrorNo = EEROR_NOLOAD_POWER;
				return RETURN_FAIL;
			}
		}else{
			g_ucLcdLine2 = FAIL2READ_PM9805;
			g_ucErrorNo = ERROR_FAIL2READ_PM9805;
			return RETURN_FAIL;
		}
#if FACTORY_TEST_SIGNAL_EN == 0
		//���Ʋ�����������
		if(TurnLight(200) != RETURN_SUCCESS){
			g_ucLcdLine2 = NO_ACK;//SendMsg2UI(WM_PAINT,STEP_TRY_AGAIN);
			g_ucErrorNo = ERROR_NO_ACK;
			return RETURN_FAIL;
		}
		CalTest_DelayS(3);//��ʱ���ܿ�����PLCͨ����ɵĹ��ʲ���
#elif FACTORY_TEST_SIGNAL_EN == 1
		//?????????
		if(TurnLight(100) != RETURN_SUCCESS){
			g_ucLcdLine2 = NO_ACK;//SendMsg2UI(WM_PAINT,STEP_TRY_AGAIN);
			g_ucErrorNo = ERROR_NO_ACK;
			return RETURN_FAIL;
		}
	   //0~10V,PWM
		g_ucLcdLine1 = CHECKING;
		g_ucLcdLine2 = TESTING_0_10V;
		CalTest_DelayS(3);//??,????PLC?????????
		fTemp = GetADC(1);//pFloatMsg[i+6]=GetADC(1);
		if (fTemp-bright[2] < 0){
			fDelta2 = bright[2] - fTemp;
		}else{
			fDelta2 = fTemp - bright[2];
		}
		if(fDelta2 > error){
			Msg_Err=1;
			g_ucLcdLine1 = FAIL2TEST_0_10V;//SendMsg2UI(WM_PAINT,STEP_OVERRANGE);
			g_ucErrorNo = ERROR_FAIL2TEST_0_10V;
			LoadZero2TenData(5, fTemp);
			return RETURN_SIGNAL_OUT;
		}
   #if  PRODUCT_TYPE == 1 /**/
		//PWM
		cPwmTem = TestPluse();
		//??????,????50%???0~10?PWM
		LoadZero2TenData(5, fTemp);
		LoadPwmPersentData(50, g_fPwm);
		if(cPwmTem == RETURN_FAIL){
			g_ucErrorNo = EEROR_PWM;
			return(RETURN_SIGNAL_OUT);
		}else{
			if (cPwmTem < 50 - DELTA_PWM || cPwmTem > 50 + DELTA_PWM){
				g_ucErrorNo = EEROR_PWM;
				return(RETURN_SIGNAL_OUT);
			}
		}
   #endif
#endif
  }
	//�����������������
	g_ucLcdLine1 = CHECKING; 
	g_ucLcdLine2 = READING_PM9805;
	g_ucLcdLine3 = SHOW_NULL;
	CalTest_DelayS(3); 
	for(i=0;i<2;i++)  //��ȡ��������ֵ
	{
		if(err=PM9805_GetInput(&inputvol),err==RETURN_SUCCESS)
		{
			fU = inputvol.Urms;
			fI = inputvol.Irms;
			fP = inputvol.P;
			break;
		}
		UI_IfExit();
		CalTest_DelayS(1);
	}
	//�ж��Ƿ��ȡ��������ʧ��
	if(i>=2){
		g_ucLcdLine2 = FAIL2READ_PM9805;//CalTest_SendMsg2UI(WM_PAINT,STEP_TRY_AGAIN);
		g_ucErrorNo = ERROR_FAIL2READ_PM9805;
		return RETURN_FAIL;
	}
	//��ȡDPS���������Ĳ���
	g_ucLcdLine1 = CHECKING; 
	g_ucLcdLine2 = READ_PARAM;
	for(i=0;i<3;i++)
	{
		if(err=PLC_GetPowerData(CalTest_ID,&power_insidata,&Msg_PLCExit),err==RETURN_SUCCESS) 
		{
			fU1 = power_insidata.Vi/10.0;
			fI1 = power_insidata.Ii/1000.0;
			fP1 = power_insidata.Pa/10.0;
			break;
		}
		UI_IfExit();
		CalTest_DelayS(1);
    }
	if (i>=3){   //��ȡDPS��������ʧ��
		g_ucLcdLine2 = NO_ACK;//CalTest_SendMsg2UI(WM_PAINT,STEP_TRY_AGAIN);
		g_ucErrorNo = ERROR_NO_ACK;
		return RETURN_FAIL;
	}
#ifndef  CLEAR_ENERGY_EN
	#error "Please define CLEAR_ENERGY_EN!"
#elif CLEAR_ENERGY_EN == 1
	if (ucMode == 1){ //���������Ҫ���ӵ�
		g_ucLcdLine1 = CLEARING_ENERGY;
	  g_ucLcdLine2 = SHOW_NULL;
		for(i=0;i<3;i++){
			cTemp = PLC_ClearEnergy(CalTest_ID);
			if(cTemp == DC_ERROR){ //�豸�벻��
				g_ucLcdLine2 = DC_ERROR;//CalTest_SendMsg2UI(WM_PAINT,STEP_TRY_AGAIN);
		        return RETURN_FAIL;
		    }else if(cTemp == DC_RIGHT){
				break;
		    }
			UI_IfExit();
			CalTest_DelayS(1);
	   }
	   if (i >= 3){
		  g_ucLcdLine2 = NO_ACK;//CalTest_SendMsg2UI(WM_PAINT,STEP_TRY_AGAIN);
		  g_ucErrorNo = ERROR_NO_ACK;
		  return RETURN_FAIL;
	   }
	}
#endif
	//==========��ʾ����
	//��ѹ
	g_ucVol[3] = Data2Char((u16)(fU)%1000/100);
	g_ucVol[4] = Data2Char((u16)(fU)%100/10);
	g_ucVol[5] = Data2Char((u16)(fU)%10);
	g_ucVol[6] = 0x2E; //С����
	g_ucVol[7] = Data2Char((u16)(fU *10)%10);
	g_ucVol[8] = 'V'; //��λ
	g_ucVol[9] = 0x20; //�ո�
	g_ucVol[10] = Data2Char((u16)(fU1)%1000/100);
	g_ucVol[11] = Data2Char((u16)(fU1)%100/10);
	g_ucVol[12] = Data2Char((u16)(fU1)%10);
	g_ucVol[13] = 0x2E; //С����
	g_ucVol[14] = Data2Char((u16)(fU1 *10)%10);
	g_ucVol[15] = 'V'; //��λ
	g_ucVol[16] = 0;

	//����
	g_ucCur[3] = Data2Char((u16)(fI*1000)%10000/1000);
	g_ucCur[4] = 0x2E; //С����
	g_ucCur[5] = Data2Char((u16)(fI*1000)%1000/100);
	g_ucCur[6] = Data2Char((u16)(fI*1000)%100/10);
	g_ucCur[7] = Data2Char((u16)(fI*1000)%10);
	g_ucCur[8] = 'A';  //��λ
	g_ucCur[9] = 0x20; //�ո�
	g_ucCur[10] = Data2Char((u16)(fI1*1000)%10000/1000);
	g_ucCur[11] = 0x2E; //С����
	g_ucCur[12] = Data2Char((u16)(fI1*1000)%1000/100);
	g_ucCur[13] = Data2Char((u16)(fI1*1000)%100/10);
	g_ucCur[14] = Data2Char((u16)(fI1*1000)%10);
	g_ucCur[15] = 'A';  //��λ
	g_ucCur[16] = 0;
	//����
	g_ucPower[3] = Data2Char((u16)(fP)%1000/100);
	g_ucPower[4] = Data2Char((u16)(fP)%100/10);
	g_ucPower[5] = Data2Char((u16)(fP)%10);
	g_ucPower[6] = 0x2E; //С����
	g_ucPower[7] = Data2Char((u16)(fP *10)%10);
	g_ucPower[8] = 'W'; //��λ
	g_ucPower[9] = 0x20; //�ո�
	g_ucPower[10] = Data2Char((u16)(fP1)%1000/100);
	g_ucPower[11] = Data2Char((u16)(fP1)%100/10);
	g_ucPower[12] = Data2Char((u16)(fP1)%10);
	g_ucPower[13] = 0x2E; //С����
	g_ucPower[14] = Data2Char((u16)(fP1 *10)%10);
	g_ucPower[15] = 'W'; //��λ
	g_ucPower[16] = 0;
	//�Ƚϲ����Ƿ�����Χ��,ʧ�ܷ���-2
	if (fU1 > fU){
		fDelta = fU1 - fU;
	}else{
		fDelta = fU - fU1;
	}
	if (fDelta > DELTA_VOLTAGE){//(fDelta/fU > 0.03){ //3%
		g_ucErrorNo = ERROR_VOLTAGE_IN;
		return RETURN_LOOK_PARM;
	}
	if (fI1 > fI){
		fDelta = fI1 - fI;
	}else{
		fDelta = fI - fI1;
	}
	if (fDelta > DELTA_CURRENT){//(fDelta/fI > 0.03){ //3%
		g_ucErrorNo = ERROR_CURRENT_IN;
		return RETURN_LOOK_PARM;
	}
	if (fP1 > fP){
		fDelta = fP1 - fP;
	}else{
		fDelta = fP - fP1;
	}
	if(fDelta > DELTA_POWER){//(fDelta/fP > 0.03){ //3%
		g_ucErrorNo = ERROR_POWER_IN;
		return RETURN_LOOK_PARM;
	}
	if (ucMode == 1){ 
	/* LORA���Կ�ʼ */
	#if LORA_TEST_EN == 1
		cReturn = TestLora(msg);
		if(RETURN_SUCCESS != cReturn){
			return cReturn;
		}
	#endif
		//�������ͨ�����ӡ4��ID
		ulPhyID = CalTest_ID;
		g_ucLcdLine1 = TEST_PASS;
		g_ucLcdLine2 = PRINTING;
		g_ucLcdLine3 = SHOW_NULL;
		WritePhyIdToucID(ulPhyID);
		g_ucLcdLine4 = PHY_ID;
		GP9034T_Print4(ulPhyID);
		g_ucBeepMode = 1;
		OSTimeDlyHMSM(0,0,2,0);
		g_ucLcdLine1 = ANY_KEY2LOOK;
		g_ucLcdLine2 = PRINT_FINISH;
    g_ucLcdLine3 = SHOW_NULL;
		WritePhyIdToucID(ulPhyID);
		g_ucLcdLine4 = PHY_ID;
	}
	return RETURN_SUCCESS;
}

//
s8 CheckLoad(void)
{	
	PM9805_InputStruct inputvol;
	float fU,fI,fP,fU1;
	u8 err = 0;
	u8 i;
	//�����������������
	g_ucLcdLine1 = SHOW_NULL; 
	g_ucLcdLine2 = CHECKING_LOAD; 
	g_ucLcdLine3 = SHOW_NULL;
	g_ucLcdLine4 = SHOW_NULL;
	for(i=0;i<5;i++){
		CalTest_DelayS(1);
		if(err=PM9805_GetInput(&inputvol),err==RETURN_SUCCESS){
			fU = inputvol.Urms;
			fI = inputvol.Irms;
			fP = inputvol.P;
		}else{
			g_ucLcdLine2 = FAIL2READ_PM9805;//CalTest_SendMsg2UI(WM_PAINT,STEP_TRY_AGAIN);
		  g_ucErrorNo = ERROR_FAIL2READ_PM9805;
		  return(RETURN_FAIL);
		}
		if(fP > MIN_POWER){ //����С��20W���ж�Ϊ�Ʋ���
	    return(RETURN_SUCCESS);
		}
		UI_IfExit();
		//CalTest_DelayS(1);
    }
	g_ucErrorNo = EEROR_NO_LOAD;
	return(RETURN_FAIL);
}
#if   PRODUCT_TYPE == 2
/***************************************/
//��������SelectRS485
//���ܣ� ѡ��UART1��RS485��
//���������ucState��1��GC9200��2��NEMA-RS485��3��NH207�նȴ�����
/***************************************/
void SelectRS485(unsigned char ucState)
{
	static unsigned s_ucStateLast = 0;
	switch(ucState){
		case 1:
			/* �ϴ��Ѿ��л�����2����3״̬������Ҫ�л��˿� */
			if(s_ucStateLast == 0 || s_ucStateLast == 2 ||s_ucStateLast == 3){
				GPIO_PinRemapConfig(GPIO_Remap_USART1,DISABLE);
			}
			s_ucStateLast = ucState;
			break;
		case 2:
		case 3:
			/* �ϴ��Ѿ��л�����1״̬�����ǳ��ν����л�������Ҫ�л��˿� */
			if(s_ucStateLast == 0 || s_ucStateLast == 1){
				GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
			}
			/* �л��źż̵��� */
			if(2 == ucState){
				NEMA_RS485;
			}else{
				NH207_RS485;
			}
			s_ucStateLast = ucState;
			break;
        default:break;
	}
}
#if   PRODUCT_TYPE == 2
/* LORA���Դ����� */
#if LORA_TEST_EN == 1 
extern u8  PLC_pRecBuff[];
extern u16 PLC_RecCtn;
s8 NemaSendData(u8 *pData,u8 len);
/*******************************************************************************
* Function Name : CalculateSum
* Description   : �����У��
return:  (u8)sum
*******************************************************************************/
unsigned char CalculateSum(unsigned char *p,unsigned char len)
{
	unsigned char sum1=0, i;
	for(i=0;i<len;i++){	
		sum1 += p[i];
	}
	return sum1;
}
/*******************************************************************************
* Function Name : 
* Description   : ����FC1 FC2
return:  ((FC1<<8) + FC2)
*******************************************************************************/
unsigned short int CalculateFC(unsigned char *p,unsigned char len)
{
	unsigned short int uiReturnVal;
	unsigned char sum1=0,sum2=0,i;
	for(i=10;i<len;i++){	
		sum1=((unsigned short int)sum1+p[i])%255;
		sum2=((unsigned short int)sum1+sum2)%255;
	}
	uiReturnVal = sum1; 
	uiReturnVal <<= 8;
	uiReturnVal |= sum2;
	return uiReturnVal;
}
//////////////////////////////////
/*******************************************************************************
* Function Name : 
* Description   : PLC ��������
*******************************************************************************/
void Slave_SendData(const unsigned char *pString,unsigned char ucLen)
{
	NemaSendData((unsigned char *)pString, ucLen);
}
//����ģʽʹ�õĺ���
/*******************************************************/
//���������ȡGC8828ģ��FLASH��������
void SendReadFlashSaveFlagCmd(void)
{
  const unsigned char ucResetCmd[] = {0x1B, 0x67, 0x72, 0x1D, 0xC0, 0x6D, 0x6D, 0x3A, 
                                      0x2D, 0xAA, 0x05, 0x23, 0x00, 0x00, 0x05};
  Slave_SendData(ucResetCmd,15); 
}
//��ȡGC8828ģ�鷵�ص�FLASH��������
//�ɹ�����1��ʧ�ܷ���0
//puiCode���������ŵ�ַ
unsigned char ReadFlashSaveFlag(unsigned  int *puiCode)
{
#define LONG     15  //�ַ��ܳ���
  unsigned char  ucReturnVal = 0;
  const unsigned char ucHead[] = {0x1B, 0x67, 0x72, 0x1D, 0xC0, 0x6D, 0x6D, 0x3A};
  const unsigned char ucHeadLong = 8;
  const unsigned char ucCmpDataLong = LONG; //�ַ��ܳ���
  unsigned char ucCheckTemp[LONG];
  unsigned char ucCnt, ucTemp;
  unsigned char *pucRecive;	
  unsigned char ucLongMax;	
  unsigned char ucFC1,ucFC2;	
  unsigned short int uiTemp;
  pucRecive = PLC_pRecBuff;
  ucLongMax = PLC_RecCtn;
  /* �ڽ�����������Ѱ�Ҹ�λ��Ӧ�ַ��� */
  for(ucCnt=0; ucCnt <= ucLongMax - ucCmpDataLong; ucCnt++){
    if(pucRecive[ucCnt] != ucHead[0]){
      continue;
    }else{
      /* Ѱ�ҽ������ݵ�ͷ */
      for(ucTemp=1;ucTemp<ucHeadLong; ucTemp++){
        if(ucHead[ucTemp] != pucRecive[ucCnt + ucTemp]){
            break;
        }
      }
      if(ucTemp == ucHeadLong){ //�ҵ��ַ���������У��
        for(ucTemp = 0; ucTemp < ucCmpDataLong; ucTemp++){
            ucCheckTemp[ucTemp] = pucRecive[ucCnt + ucTemp];
        }
        //ucCheckTemp[8] = 0;
        //ucCheckTemp[9] = 0;
        uiTemp = CalculateFC(ucCheckTemp, ucCmpDataLong);
        ucFC1 = (unsigned char)(uiTemp>>8);
        ucFC2 = (unsigned char)(uiTemp);
        ucCheckTemp[8] = ucFC1;
        ucCheckTemp[9] = ucFC2;
        if(pucRecive[ucCnt + 8] == ucFC1 && pucRecive[ucCnt + 9] == ucFC2 && 
             pucRecive[ucCnt + 10] == 5){ 
          /* У��ͨ������ȡFLASH������� */
          uiTemp = pucRecive[ucCnt + 13];
          uiTemp <<= 8;
          uiTemp |= pucRecive[ucCnt + 14];	 
          *puiCode = uiTemp;
          ucReturnVal = 1;
          break;
        }	
      }
    }
  }
  return(ucReturnVal);
}
/***********************************************************/
//����ģʽʹ�õĺ���
//unMode: 0:simple broadcast mode  3:auto routing mode
void SetSlaveOperationalMode(unsigned char ucMode)
{
  unsigned char ucResetCmd[16] = {0x1B, 0x67, 0x72, 0x1D, 0xC0, 0x6D, 0x6D, 0x3A, 
                                  0x00, 0x00, //CF1,CF2
                                  0x06, //long
                                    0x23, //seq
                                  0x01, 0x01, 0x00, 0x00};
  const unsigned char ucLong = 16;
  unsigned short int uiTemp;
  ucResetCmd[15] = ucMode;
  uiTemp = CalculateFC(ucResetCmd, ucLong);
  ucResetCmd[8] = (unsigned char)(uiTemp>>8);
  ucResetCmd[9] = (unsigned char)(uiTemp);
  Slave_SendData(ucResetCmd,ucLong); 
}
//�ж�����ģʽ����Ӧ
//�ɹ�����1��ʧ�ܷ���0
unsigned char CheckSlaveModeSet(void)
{
  unsigned char  ucReturnVal = 0;
  const unsigned char ucHead[] = {0x1B, 0x67, 0x72, 0x1D, 0xC0, 0x6D, 0x6D, 0x3A};
  const unsigned char ucHeadLong = 8;
  const unsigned char ucCmpDataLong = 13; //�ַ��ܳ���
  unsigned char ucCheckTemp[13];
  unsigned char ucCnt, ucTemp;
  unsigned char *pucRecive;	
  unsigned char ucLongMax;	
  unsigned char ucFC1,ucFC2;	
  unsigned short int uiTemp;
  pucRecive = PLC_pRecBuff;
  ucLongMax = PLC_RecCtn;
  /* �ڽ�����������Ѱ�Ҹ�λ��Ӧ�ַ��� */
  for(ucCnt=0; ucCnt <= ucLongMax - ucCmpDataLong; ucCnt++){
    if(pucRecive[ucCnt] != ucHead[0]){
      continue;
    }else{
      /* Ѱ�ҽ������ݵ�ͷ */
      for(ucTemp=1;ucTemp<ucHeadLong; ucTemp++){
        if(ucHead[ucTemp] != pucRecive[ucCnt + ucTemp]){
          break;
        }
      }
      if(ucTemp == ucHeadLong){ //�ҵ��ַ���������У��
        for(ucTemp = 0; ucTemp < ucCmpDataLong; ucTemp++){
            ucCheckTemp[ucTemp] = pucRecive[ucCnt + ucTemp];
        }
        ucCheckTemp[8] = 0;
        ucCheckTemp[9] = 0;
        uiTemp = CalculateFC(ucCheckTemp, ucCmpDataLong);
        ucFC1 = (unsigned char)(uiTemp>>8);
        ucFC2 = (unsigned char)(uiTemp);
        if(pucRecive[ucCnt + 8] == ucFC1 && pucRecive[ucCnt + 9] == ucFC2 && 
             pucRecive[ucCnt + 10] == 3 && pucRecive[ucCnt + 11] == 0x23){ 
          /* У��ͨ�� */
          ucReturnVal = 1;
          break;
        }	
      }
    }
  }
  return(ucReturnVal);
}
/***********************************************************/
//�������úõ�ģʽ��Flash
void SaveModer(unsigned short int uiCode)
{
  unsigned char ucResetCmd[] = {0x1B, 0x67, 0x72, 0x1D, 0xC0, 0x6D, 0x6D, 0x3A, 
                                0x00, 0x00, //FC1, FC2   8��9
                                0x07, //long
                                  0x25, 0x01, 0x00, 0x05, 
                                0x00, 0x00}; //total = 17
  unsigned short int uiFC12;
  ucResetCmd[15] = (unsigned char)(uiCode>>8);
  ucResetCmd[16] = (unsigned char)(uiCode);
  uiFC12 = CalculateFC(ucResetCmd, 17);
  ucResetCmd[8] = (unsigned char)(uiFC12>>8);
  ucResetCmd[9] = (unsigned char)(uiFC12);															
  Slave_SendData(ucResetCmd,17); 
}
//�жϱ���ģʽ����Ӧ
//�ɹ�����1��ʧ�ܷ���0
unsigned char CheckFlashSave(void)
{
  unsigned char  ucReturnVal = 0;
  const unsigned char ucHead[] = {0x1B, 0x67, 0x72, 0x1D, 0xC0, 0x6D, 0x6D, 0x3A};
  const unsigned char ucHeadLong = 8;
  const unsigned char ucCmpDataLong = 13; //�ַ��ܳ���
  unsigned char ucCheckTemp[13];
  unsigned char ucCnt, ucTemp;
  unsigned char *pucRecive;	
  unsigned char ucLongMax;	
  unsigned char ucFC1,ucFC2;	
  unsigned short int uiTemp;
  pucRecive = PLC_pRecBuff;
  ucLongMax = PLC_RecCtn;
 /* �ڽ�����������Ѱ�Ҹ�λ��Ӧ�ַ��� */
  for(ucCnt=0; ucCnt <= ucLongMax - ucCmpDataLong; ucCnt++){
    if(pucRecive[ucCnt] != ucHead[0]){
      continue;
    }else{
      /* Ѱ�ҽ������ݵ�ͷ */
      for(ucTemp=1;ucTemp<ucHeadLong; ucTemp++){
        if(ucHead[ucTemp] != pucRecive[ucCnt + ucTemp]){
          break;
        }
      }
      if(ucTemp == ucHeadLong){ //�ҵ��ַ���������У��
        for(ucTemp = 0; ucTemp < ucCmpDataLong; ucTemp++){
            ucCheckTemp[ucTemp] = pucRecive[ucCnt + ucTemp];
        }
        ucCheckTemp[8] = 0;
        ucCheckTemp[9] = 0;
        uiTemp = CalculateFC(ucCheckTemp, ucCmpDataLong);
        ucFC1 = (unsigned char)(uiTemp>>8);
        ucFC2 = (unsigned char)(uiTemp);
        if(pucRecive[ucCnt + 8] == ucFC1 && pucRecive[ucCnt + 9] == ucFC2 && 
           pucRecive[ucCnt + 10] == 3 && pucRecive[ucCnt + 11] == 0x25){ 
          /* У��ͨ�� */
          if(pucRecive[ucCnt + 12] == 0){
            ucReturnVal = 1;
          }
          break;
        }	
      }
    }
  }
  return(ucReturnVal);
}
/**************************************************************/
//ucMode: 0:simple broacast   3:auto routing slave
//return: 0:������; 1:�ɹ� 2:ʧ��
//100ms����һ��
unsigned char SetRunMode(unsigned char ucMode)
{
  static unsigned char s_ucTaskCase = 0;
  static unsigned char s_ucDelayCnt = 0;
  static unsigned int  s_uiFlashSaveFlag;
  unsigned char ucReturnVal = 0;
#define  g_ucUartReceiveFlag  PLC_RecCtn //��ֲ�滻����
	switch(s_ucTaskCase){
		case 0:
			if(++s_ucDelayCnt > 2){ //��ʱ
				s_ucDelayCnt = 0;
				s_ucTaskCase = 1;	
			}
			break;
		case 1: /* ��ȡFLASH������ */
			s_ucDelayCnt = 0;
			g_ucUartReceiveFlag = 0;
			SendReadFlashSaveFlagCmd();
			s_ucTaskCase = 2;	
			break;
		case 2:/* �ȴ�GC8828A�ظ�FLASH������ */
			if(g_ucUartReceiveFlag){
				if(ReadFlashSaveFlag(&s_uiFlashSaveFlag)){
					s_ucTaskCase = 3;	
				}
				g_ucUartReceiveFlag = 0;
			}
			if((++s_ucDelayCnt) > 10){ // �ȴ�1s�ղ�����Ӧ����Ϊʧ��	
				s_ucDelayCnt = 0;
				s_ucTaskCase = 44;
			}
			break;
		case 3: /* ����GC8828Aģʽ */
			s_ucDelayCnt = 0;
			g_ucUartReceiveFlag = 0;
			SetSlaveOperationalMode(ucMode);/* ������������ */
			s_ucTaskCase = 4;	
			break;
		case 4: /* �ȴ���Ӧ */
			if(g_ucUartReceiveFlag){
				if(CheckSlaveModeSet()){
					 s_ucTaskCase = 5;	
				}
				g_ucUartReceiveFlag = 0;
			}
			if((++s_ucDelayCnt) > 30){ // 
				s_ucDelayCnt = 0;
				s_ucTaskCase = 44;
			}
			break;
		case 5: /* ����ģʽ */
			s_ucDelayCnt = 0;
			g_ucUartReceiveFlag = 0;
			SaveModer(s_uiFlashSaveFlag);
			s_ucTaskCase = 6;	
			break;
		case 6: /* �ȴ�����ģʽ�Ļ�Ӧ */
			if(g_ucUartReceiveFlag){
				if(CheckFlashSave()){
					s_ucDelayCnt = 0;
					s_ucTaskCase = 7;	
				}
				g_ucUartReceiveFlag = 0;
			}
			if((++s_ucDelayCnt) > 10){ // Delay five times	
				s_ucDelayCnt = 0;
				s_ucTaskCase = 44;
			}
			break;
		case 7: /* ��λGC8828Aģ�� */
      s_ucDelayCnt = 0;
			s_ucTaskCase = 0;
		  ucReturnVal = 1;
			break;
		case 44:
			s_ucDelayCnt = 0;
			s_ucTaskCase = 0;
			ucReturnVal = 2;
			break;
	}
  return(ucReturnVal);
}
#endif
/***************************************/
//��������TestNemaRS485
//���ܣ�  ��PHYIDд������ucID��������ʾ��
//���������Ҫ��ʾ��PHYID
/***************************************/
s8 TestNemaRS485(void)
{
	u8 i;
	u16 uiTimeCnt = 0;
	u8 ucTemp;
//�����������������
	g_ucLcdLine1 = SHOW_NULL; 
	g_ucLcdLine2 = TESTING_RS485; 
	g_ucLcdLine3 = SHOW_NULL;
	g_ucLcdLine4 = SHOW_NULL;
	/* �л�RS485ͨ�� */
	SelectRS485(2);
	OSTimeDlyHMSM(0,0,0,30);
	while(1){
		if(uiTimeCnt%50 == 0){ /* ÿ0.5���ӷ�һ�ν��밲ȫģʽ������ */
		   SendSafteCmd();
		}
		if(uiTimeCnt == 0){ /* ��NEMA�ϵ� */
		   NEMA_ON;
		}
	#if 1
		if(CheckSafteMode()){
		#if LORA_TEST_EN == 1 /* ʹ����LORA���ԣ���Ҫ����GC8828AΪauto routing slaveģʽ */		
      /* ��Ϊ�ϴβ����������Ϊsimple broadcastģʽ֮��������Ϊauto routing slaveģʽʧ�ܣ��ͻᵼ���޷����� */
      OSTimeDlyHMSM(0,0,1,800); /* ��ʱ���ȴ�GC8828A�����ȶ�����״̬ */		
      while(1){ /* ����Ϊauto routing slaveģʽ */
				ucTemp = SetRunMode(3);
				if(ucTemp == 1){ /* ģʽ���óɹ� */
					break;
				}else if(ucTemp == 2){ /* ģʽ����ʧ�� */
					g_ucLcdLine1 = SET_AUTO_FAIL;
			    g_ucErrorNo = EEROR_SET_AUTO_FALL;
			    return(RETURN_FAIL);
				}
				OSTimeDlyHMSM(0,0,0,100);
      }				
    #endif			
		#if RESET_MASTER_MODE == 1
			/* ��λGC8828�˳���ȫģʽ */
			OSTimeDlyHMSM(0,0,1,500);
			NemaReset();
		#endif
			/* �л�RS485ͨ�� */
			SelectRS485(1);
		#if RESET_MASTER_MODE == 1
			OSTimeDlyHMSM(0,0,1,500);
		#endif
			return(RETURN_SUCCESS);
		}
		uiTimeCnt++;
		if(uiTimeCnt >= 500){ /* 5s�ղ���PLCģ����밲ȫģʽ���źţ�����RS485����ʧ�� */
			g_ucLcdLine1 = RS485_FAIL;
			g_ucErrorNo = EEROR_RS485_FAULT;
			return(RETURN_FAIL);
		}
	#else
		uiTimeCnt++;
	#endif
		OSTimeDlyHMSM(0,0,0,10);
	}
}
#endif
/***************************************/
//��������SetPWM
//���ܣ�����PWM��ռ�ձ�
//���������ucData��0~100��Ӧ0%~100%
/***************************************/
void SetPWM(u8 ucData)
{
  u16 uiT;
  uiT = PWM_T *(ucData/100.0);
  TIM_SetCompare1(PWM_TIMER,uiT);
}
#endif
/***************************************/
//��������WritePhyIdToucID
//���ܣ�  ��PHYIDд������ucID��������ʾ��
//���������Ҫ��ʾ��PHYID
/***************************************/
void WritePhyIdToucID(unsigned long ulPhyID)
{
	ucID[8]  = Data2Char((u8)((ulPhyID>>28) & 0x0f));
	ucID[9]  = Data2Char((u8)((ulPhyID>>24) & 0x0f));
	ucID[10] = Data2Char((u8)((ulPhyID>>20) & 0x0f));
	ucID[11] = Data2Char((u8)((ulPhyID>>16) & 0x0f));
	ucID[12] = Data2Char((u8)((ulPhyID>>12) & 0x0f));
	ucID[13] = Data2Char((u8)((ulPhyID>>8) & 0x0f));
	ucID[14] = Data2Char((u8)((ulPhyID>>4) & 0x0f));
	ucID[15] = Data2Char((u8)((ulPhyID) & 0x0f));
	ucID[16] = 0;
}

/***************************************/
//��������ReadProductPhyID
//���ܣ�  ������Ʒ��ID��
//���������
//ʧ�ܷ��أ�0xffffffff
//�ɹ����أ�ID
/***************************************/
u32 ReadProductPhyID(void *msg)
{
  u32   ulPhyID;
	CalTest_pFmsg=(float*)msg;
	CalTest_pImsg=(u16*)msg;
	g_ucLcdLine2 = GET_ID;
	if(RETURN_SUCCESS != CalTest_GetID())
	{   g_ucErrorNo = ERROR_FAIL2GETID;
		return 0xffffffff;
	}
	ulPhyID = CalTest_ID;
	WritePhyIdToucID(ulPhyID);
	g_ucLcdLine2 = PHY_ID;
	g_ucLcdLine3 = ASK_PRINT;
	while(1){
		if (CalTest_pImsg[0] == 1){
			g_ucLcdLine3 = PRINTING;
			GP9034T_Print4(ulPhyID);
			OSTimeDlyHMSM(0,0,2,0);
			g_ucLcdLine1 = ANY_KEY2BACK;
	    g_ucLcdLine2 = PRINT_FINISH;
			g_ucLcdLine3 = SHOW_NULL;
	    g_ucLcdLine4 = SHOW_NULL;
			break;
	  }else if(CalTest_pImsg[0] == 2){
			g_ucLcdLine1 = ANY_KEY2BACK;
	    g_ucLcdLine2 = SHOW_NULL;
			g_ucLcdLine3 = SHOW_NULL;
	    g_ucLcdLine4 = SHOW_NULL;
			break;
		}
		OSTimeDlyHMSM(0,0,0,50);
	}
	CalTest_pImsg[0] = 0;
	return(ulPhyID);
}


/***************************************/
//��������CheckPhyID
//���ܣ�  ������Ʒ��ID��
//���������0����ʾ����ģʽ�˵���1����ʾ�ξ߲�����2������2�Ĺ�����ʾ
//ʧ�ܷ��أ�0xffffffff
//�ɹ����أ�ID
/***************************************/
void CheckPhyID(unsigned char ucData)
{
	unsigned long ulPhyID;
	static unsigned char ucTemp[6];
    if (ucData == 0){ //��ʾ��ѯ���˵�
		g_ucLcdLine1 = ucTemp[1];
		g_ucLcdLine2 = ucTemp[2];
		g_ucLcdLine3 = ucTemp[3];
		g_ucLcdLine4 = ucTemp[4];
	}else if(ucData == 1){
		ulPhyID = ReadPhyIdFromFlash(FLASH_SAVE_ADR);
		//��ʾID�Ķκ�
		g_ucSegNum[8] = PHY_ID_SEGMENT_NUMBER / 10000 + 0x30; //��λ
		g_ucSegNum[9] = PHY_ID_SEGMENT_NUMBER % 10000 /1000 + 0x30; //ǧλ
		g_ucSegNum[10] = PHY_ID_SEGMENT_NUMBER % 1000 /100 + 0x30; //��λ
		g_ucSegNum[11] = PHY_ID_SEGMENT_NUMBER % 100 /10 + 0x30; //ʮλ
		g_ucSegNum[12] = PHY_ID_SEGMENT_NUMBER % 10 + 0x30; //��λ
		//��ʾ��ʼID
		g_ucSegStart[8]  = Data2Char((u8)((PHY_ID_START>>28) & 0x0f));
		g_ucSegStart[9]  = Data2Char((u8)((PHY_ID_START>>24) & 0x0f));
		g_ucSegStart[10] = Data2Char((u8)((PHY_ID_START>>20) & 0x0f));
		g_ucSegStart[11] = Data2Char((u8)((PHY_ID_START>>16) & 0x0f));
		g_ucSegStart[12] = Data2Char((u8)((PHY_ID_START>>12) & 0x0f));
		g_ucSegStart[13] = Data2Char((u8)((PHY_ID_START>>8) & 0x0f));
		g_ucSegStart[14] = Data2Char((u8)((PHY_ID_START>>4) & 0x0f));
		g_ucSegStart[15] = Data2Char((u8)((PHY_ID_START) & 0x0f));
		//��ʾ����ID
		g_ucSegEnd[8]  = Data2Char((u8)((PHY_ID_END>>28) & 0x0f));
		g_ucSegEnd[9]  = Data2Char((u8)((PHY_ID_END>>24) & 0x0f));
		g_ucSegEnd[10] = Data2Char((u8)((PHY_ID_END>>20) & 0x0f));
		g_ucSegEnd[11] = Data2Char((u8)((PHY_ID_END>>16) & 0x0f));
		g_ucSegEnd[12] = Data2Char((u8)((PHY_ID_END>>12) & 0x0f));
		g_ucSegEnd[13] = Data2Char((u8)((PHY_ID_END>>8) & 0x0f));
		g_ucSegEnd[14] = Data2Char((u8)((PHY_ID_END>>4) & 0x0f));
		g_ucSegEnd[15] = Data2Char((u8)((PHY_ID_END) & 0x0f));
		//��ʾ��ǰID
		g_ucIdNow[8]  = Data2Char((u8)((ulPhyID>>28) & 0x0f));
		g_ucIdNow[9]  = Data2Char((u8)((ulPhyID>>24) & 0x0f));
		g_ucIdNow[10] = Data2Char((u8)((ulPhyID>>20) & 0x0f));
		g_ucIdNow[11] = Data2Char((u8)((ulPhyID>>16) & 0x0f));
		g_ucIdNow[12] = Data2Char((u8)((ulPhyID>>12) & 0x0f));
		g_ucIdNow[13] = Data2Char((u8)((ulPhyID>>8) & 0x0f));
		g_ucIdNow[14] = Data2Char((u8)((ulPhyID>>4) & 0x0f));
		g_ucIdNow[15] = Data2Char((u8)((ulPhyID) & 0x0f));
		
		//����֮ǰ����ʾֵ
		ucTemp[1] = g_ucLcdLine1;
		ucTemp[2] = g_ucLcdLine2;
		ucTemp[3] = g_ucLcdLine3;
		ucTemp[4] = g_ucLcdLine4;
		//��ʾ
		g_ucLcdLine1 = CHECK_ID_NUM;
		g_ucLcdLine2 = CHECK_SEG_START;
		g_ucLcdLine3 = CHECK_SEG_END;
		g_ucLcdLine4 = CHECK_ID_NOW;
    }else if(ucData == 2){
		
	}
}







