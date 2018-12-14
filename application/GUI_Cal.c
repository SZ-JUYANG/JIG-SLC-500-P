/*
******************************************************************
**                      uCGUIBuilder                            **
**                  Version:   4.0.0.0                          **
**                     2012 / 04                               **
**                   CpoyRight to: wyl                          **
**              Email:ucguibuilder@163.com                        **
**          This text was Created by uCGUIBuilder               **
******************************************************************/



#include <stdlib.h>
#include <stdio.h>
#include "GUI_Cal.h"
#include "ucos_ii.h"
#include "CommonFunction.h"
#include "Flash.h"


#include "RA8875.h"



#include <stddef.h>
#include "GUI.h"
#include "DIALOG.h"

#include "WM.h"
#include "BUTTON.h"
#include "CHECKBOX.h"
#include "DROPDOWN.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
#include "LISTBOX.h"
#include "MULTIEDIT.h"
#include "RADIO.h"
#include "SLIDER.h"
#include "TEXT.h"
#include "PROGBAR.h"
#include "SCROLLBAR.h"
#include "LISTVIEW.h"





//EventsFunctionList
static void OnButtonReleased(WM_MESSAGE * pMsg){};
#define MAX_SIZEMSG 25
static float Cal_FloatMsg[MAX_SIZEMSG];
static u16 *Cal_Intmsg=(u16*)Cal_FloatMsg;
#define NEXT_MSG  (WM_USER+1)	
static u8 Restart=0;
static u8 NextStep=0;	
	
	
extern OS_EVENT * pCal_BoxEvent;
//EndofEventsFunctionList

	
typedef struct
{
	u16 line;
	const char *str;
	void *pdata;
}CalStr;
	
static char *ListStr[]=
{
	"请按开始",//1
	"失败，请重试",//2
	"等待下一步",//3
	"获取电源ID",//4
	"获取校正项目",//5
	"等待调整输入电压1",//6
	"获取输入电压频率1",//7
	"获取输入电压1",//8
	"等待调整输入电压2",//9
	"获取输入电压频率2",//10
	"获取输入电压2",//11
	"校正输入电压",//12
	"获取输入功率频率1",//13
	"获取输入功率1",//14
	"获取输入功率频率2",//15
	"获取输入功率2",//16
	"校正输入功率",//17
	"获取输入电流频率1",//18
	"获取输入电流1",//19
	"获取输入电流频率2",//20
	"获取输入电流2",//21
	"校正输入电流",//22
	"获取输出电压频率1",//23
	"获取输出电压1",//24
	"获取输出电压频率2",//25
	"获取输出电压2",//26
	"校正输出电压",//27
	"获取输出电流频率1",//28
	"获取输出电流1",//29
	"获取输出电流频率2",//30
	"获取输出电流2",//31
	"校正输出电流",//32
	"获取内部数据",//33
	"获取仪器数据",//34
	"调整输出电压",//35
	"调整输出电流",//36
	"开始校正",//37
}	;

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




/******************************************
int 1:key, 2:PLC_exit





*********************************************/
//按键值
#define CalTest_Key     Cal_Intmsg[0]

//校正的步骤
#define Msg_Step        Cal_Intmsg[1]

//退出测试命令
#define Msg_PLCExit     Cal_Intmsg[2]

//错误信息
#define Msg_Err         Cal_Intmsg[3]

//采样的点数
#define Msg_TestTimes   Cal_Intmsg[4]

//窗口的句柄
#define Msg_Handle      Cal_Intmsg[6]


#define Msg_Opt         Cal_Intmsg[7]

#define CalTest_NEXT    0x01
#define PLC_EXIT        0x01



#define Msg_ID      Cal_FloatMsg[6]

//电源读出的值
#define Msg_InputP  Cal_FloatMsg[7]
#define Msg_InputV  Cal_FloatMsg[8]
#define Msg_InputI  Cal_FloatMsg[9]
#define Msg_OutputV Cal_FloatMsg[10]
#define Msg_OutputI Cal_FloatMsg[11]

//仪器读出的值
#define Msg_InputP1  Cal_FloatMsg[12]
#define Msg_InputV1  Cal_FloatMsg[13]
#define Msg_InputI1  Cal_FloatMsg[14]
#define Msg_OutputV1 Cal_FloatMsg[15]
#define Msg_OutputI1 Cal_FloatMsg[16]
#define Msg_P_Feq    Cal_FloatMsg[17]
#define Msg_V_Feq    Cal_FloatMsg[18]
#define Msg_I_Feq    Cal_FloatMsg[19]







static int WMH2Int(WM_HWIN h)
{
	union
	{
		WM_HWIN w;
		int i;
	}d;
	d.w=h;
	return d.i;
}




/*********************************************************************
*
*       static data
*
**********************************************************************
*/
static WM_HWIN LsthWin;
static WM_HWIN Cal_CreatListbox(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;
	hWin=LISTBOX_CreateAsChild(NULL,pMsg->hWin,20,29,258,172,WM_CF_SHOW);
	LISTBOX_SetBkColor(hWin,0,0xffffff);
  LISTBOX_SetAutoScrollH(hWin,1);
  LISTBOX_SetAutoScrollV(hWin,1);
	return hWin;
}


WM_HWIN Cal_Handle;

/*********************************************************************
*
*       Dialog resource
*
* This table conatins the info required to create the dialog.
* It has been created by ucGUIbuilder.
*/

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect,  "校正测试",           0,                       0,  0,  480,272,0,0},
    { BUTTON_CreateIndirect,    "开始/下一步",             GUI_ID_BUTTON0,          64, 202,125,36, 0,0},
    { BUTTON_CreateIndirect,    "返回",              GUI_ID_CANCEL,          263,202,125,36, 0,0},
    { TEXT_CreateIndirect,      "输入 内部值/仪器值",             GUI_ID_TEXT0,            300,25, 150,18, 0,0},
    { TEXT_CreateIndirect,      "P=   000.0W/000.0W",             GUI_ID_TEXT1,            300,43, 150,18, 0,0},
    { TEXT_CreateIndirect,      "U=",             GUI_ID_TEXT2,            300,61, 150,18, 0,0},
		{ TEXT_CreateIndirect,      "I=",             GUI_ID_TEXT3,            300,79, 150,18, 0,0},
    { TEXT_CreateIndirect,      "ID=",             GUI_ID_TEXT4,            300,101, 150,18, 0,0},
    { TEXT_CreateIndirect,      "输出 内部值/仪器值",  GUI_ID_TEXT5,       300,123,150,18, 0,0},
    { TEXT_CreateIndirect,      "U=",             GUI_ID_TEXT6,            300,141,150,18, 0,0},
    { TEXT_CreateIndirect,      "I=",             GUI_ID_TEXT7,            300,159,150,18, 0,0}
};



/*****************************************************************
**      FunctionName:void PaintDialog(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback --> WM_PAINT
*****************************************************************/

static void PaintDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
		char str[30];
	  static u8 line=0;
		sprintf(str,"P= %4.1fW/%4.1fW",Msg_InputP,Msg_InputP1);
		TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT1),str);
	
	  sprintf(str,"U= %4.1fV/%4.1fV",Msg_InputV,Msg_InputV1);
		TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT2),str);
	
	  sprintf(str,"I= %5.3fA/%5.3fA",Msg_InputI,Msg_InputI1);
		TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT3),str);
		
		sprintf(str,"ID= %08x",Float2Long(Msg_ID));
		TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT4),str);
	
	 sprintf(str,"U= %4.1fV/%4.1fV",Msg_OutputV,Msg_OutputV1);
		TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT6),str);
	
	  sprintf(str,"I= %5.3fA/%5.3fA",Msg_OutputI,Msg_OutputI1);
		TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT7),str);
	
	if(Msg_Step>0)
	{
		sprintf(str,ListStr[Msg_Step-1],Msg_TestTimes);
		LISTBOX_AddString(LsthWin,str);
		LISTBOX_SetSel(LsthWin,line++);
		if(Msg_Step==STEP_START)
		{
			Restart=1;
			line=0;
		}
		NextStep=Msg_Step;
		Msg_Step=0;
	}
}



/*****************************************************************
**      FunctionName:void InitDialog(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback --> WM_INIT_DIALOG
*****************************************************************/

static void InitDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
    //
    //FRAMEWIN
    //
	  OSMboxPost(pCal_BoxEvent,Cal_FloatMsg);
    FRAMEWIN_SetClientColor(hWin,0xe0e0e0);
    FRAMEWIN_SetTextAlign(hWin,GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_SetTitleHeight(hWin,18);
	  
    //
    //GUI_ID_LISTBOX0
    //
    //LISTBOX_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_LISTBOX0),0,0xffffff);
    //LISTBOX_SetAutoScrollH(WM_GetDialogItem(hWin,GUI_ID_LISTBOX0),1);
    //LISTBOX_SetAutoScrollV(WM_GetDialogItem(hWin,GUI_ID_LISTBOX0),1);
	  LsthWin=Cal_CreatListbox(pMsg);
    //
    //GUI_ID_BUTTON0
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON0),BUTTON_CI_UNPRESSED,0xe0e0e0);
    //
    //GUI_ID_BUTTON1
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_CANCEL),BUTTON_CI_UNPRESSED,0xe0e0e0);
    //
    //GUI_ID_TEXT0
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT0),0xe0e0e0);
    //
    //GUI_ID_TEXT1
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT1),0xe0e0e0);
    //
    //GUI_ID_TEXT2
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT2),0xe0e0e0);
    //
    //GUI_ID_TEXT3
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT3),0xe0e0e0);
    //
    //GUI_ID_TEXT4
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT4),0xe0e0e0);
    //
    //GUI_ID_TEXT5
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT5),0xe0e0e0);
    //
    //GUI_ID_TEXT6
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT6),0xe0e0e0);
		//
    //GUI_ID_TEXT7
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT7),0xe0e0e0);

}



static int ctn=0;
/*********************************************************************
*
*       Dialog callback routine
*/
static void _cbCallback(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
        case WM_PAINT:
            PaintDialog(pMsg);
            break;
        case WM_INIT_DIALOG:
            InitDialog(pMsg);
				    ctn=0;
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_ESCAPE:
                    GUI_EndDialog(hWin, 1);
                    break;
                case GUI_KEY_ENTER:
                    GUI_EndDialog(hWin, 0);
                    break;
            }
            break;
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
                case GUI_ID_BUTTON0:
                    if(NCode==WM_NOTIFICATION_RELEASED)
										{
											CalTest_Key=NextStep;
											NextStep=0;
											if(Restart==1)
											{
												Restart=0;
												GUI_EndDialog(LsthWin, 0);
												LsthWin=Cal_CreatListbox(pMsg);
											}
										}
                    break;
                case GUI_ID_CANCEL:
                    if(NCode==WM_NOTIFICATION_RELEASED)
										{
											  Msg_PLCExit=PLC_EXIT;
                        GUI_EndDialog(hWin, 0);
										}
                    break;

            }
            break;
				case NEXT_MSG:
				{			
					
				}
        default:
            WM_DefaultProc(pMsg);
    }
}


/*********************************************************************
*
*       MainTask
*
**********************************************************************
*/
void GUICal_InitMsg(void)
{
	u8 i;
	for(i=0;i<sizeof(Cal_FloatMsg);i++)
	{
		Cal_FloatMsg[i]=0;
	}
}


void GUICal_InitMsgData(void)
{
	u8 i;
	for(i=0;i<MAX_SIZEMSG-6;i++)
	{
		Cal_FloatMsg[i+6]=0;
	}
}

void GUI_Cal(void) 
{ 
	  GUICal_InitMsg();
    WM_SetDesktopColor(GUI_WHITE);      /* Automacally update desktop window */
    WM_SetCreateFlags(WM_CF_MEMDEV);  /* Use memory devices on all windows to avoid flicker */
    Cal_Handle=GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
	  Msg_Handle=Cal_Handle;
		Msg_Opt=Long2Float(FLASH_ReadCalSet());
}



void *GUICal_GetMsgPiont(void)
{
	return Cal_FloatMsg;
}
