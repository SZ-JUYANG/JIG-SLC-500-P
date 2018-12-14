/*
******************************************************************
**                      uCGUIBuilder                            **
**                  Version:   4.0.0.0                          **
**                     2012 / 04                               **
**                   CpoyRight to: wyl                          **
**              Email:ucguibuilder@163.com                        **
**          This text was Created by uCGUIBuilder               **
******************************************************************/

#include <stddef.h>
#include "GUI.h"
#include "DIALOG.h"
#include "ucos_ii.h"
#include <stdlib.h>
#include <stdio.h>
#include "CommonFunction.h"

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


#include "Flash.h"

static float *pFloatMsg;
static u16 *pU16Msg;
static u8 Restart=0;
extern OS_EVENT * pCal_BoxEvent;
//void *GUICal_GetMsgPiont(void);
void GUICal_InitMsg(void);
#define PLC_EXIT        0x01
#define START           0x0101



//static u32 SetData;
const char *str[6]=
{
	"00.00V/%4.2fV",
	"02.50V/%4.2fV",
	"05.00V/%4.2fV",
	"07.50V/%4.2fV",
	"10.00V/%4.2fV",
	"ID=%08x",
};
static char *ListStr[]=
{
	"请按开始",//1
	"失败，请重试",//2
	"获取ID",//3
	"获取0V",//4
	"获取2.5V",//5
	"获取5.0V",//6
	"获取7.5V",//7
	"获取10.0V",//8
	"电压误差超过范围",//9
}	;


#define STEP_PRESSTART 1
#define STEP_TRY_AGAIN 2
#define STEP_GET_ID    3
#define STEP_GET0V     4
#define STEP_GET25V    5
#define STEP_GET50V    6
#define STEP_GET75V    7
#define STEP_GET100V   8
#define STEP_OVERRANGE 9








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

#define pVol    (&pFloatMsg[6])

//EventsFunctionList
void OnButtonReleased(WM_MESSAGE * pMsg);
//EndofEventsFunctionList


/*********************************************************************
*
*       static data
*
**********************************************************************
*/



/*********************************************************************
*
*       Dialog resource
*
* This table conatins the info required to create the dialog.
* It has been created by ucGUIbuilder.
*/

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
	
    { FRAMEWIN_CreateIndirect,  "0-10V测试",           0,                       0,  0,  480,272,0,0},
    { BUTTON_CreateIndirect,    "开始测试",             GUI_ID_BUTTON0,          64, 202,125,36, 0,0},
    { BUTTON_CreateIndirect,    "返回",              GUI_ID_CANCEL,          263,202,125,36, 0,0},
    { TEXT_CreateIndirect,      "测量点/测量值",             GUI_ID_TEXT0,            300,25, 150,18, 0,0},
    { TEXT_CreateIndirect,      "00.00V/",             GUI_ID_TEXT1,            300,43, 150,18, 0,0},
    { TEXT_CreateIndirect,      "02.50V/",             GUI_ID_TEXT2,            300,61, 150,18, 0,0},
		{ TEXT_CreateIndirect,      "05.00V/",             GUI_ID_TEXT3,            300,79, 150,18, 0,0},
    { TEXT_CreateIndirect,      "07.50V/",             GUI_ID_TEXT4,            300,101, 150,18, 0,0},
    { TEXT_CreateIndirect,      "10.00V/",             GUI_ID_TEXT5,       300,123,150,18, 0,0},
		{ TEXT_CreateIndirect,      "ID=",             GUI_ID_TEXT6,           300,145,150,18, 0,0},
	
};

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


/*****************************************************************
**      FunctionName:void PaintDialog(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback --> WM_PAINT
*****************************************************************/

static void PaintDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
		char str0[30];
	  u8 i;
	  static u8 line=0;
	
	  sprintf(str0,str[5],Float2Long(MsgID));
	  TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT6),str0);
		for(i=0;i<5;i++)
		{
			sprintf(str0,str[i],pFloatMsg[i+6]);
			TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT1+i),str0);
		}
		if(Step>0)
		{
			if(Step==STEP_PRESSTART)
			{
				Restart=1;
				line=0;
			}
			LISTBOX_AddString(LsthWin,ListStr[Step-1]);
			LISTBOX_SetSel(LsthWin,line++);
			if(Msg_Err==1)
			{
				Msg_Err=0;
				LISTBOX_SetBackColor(LsthWin,0,GUI_RED);
			}
			Step=0;
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
	  Msg_Key=0x0100;
	  OSMboxPost(pCal_BoxEvent,pFloatMsg);
    FRAMEWIN_SetClientColor(hWin,0xe0e0e0);
    FRAMEWIN_SetTextAlign(hWin,GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_SetTitleHeight(hWin,18);
	
	  LsthWin=Cal_CreatListbox(pMsg);
    //
    //GUI_ID_OK
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON0),BUTTON_CI_UNPRESSED,0xe0e0e0);
    //
    //GUI_ID_CANCEL
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_CANCEL),BUTTON_CI_UNPRESSED,0xe0e0e0);
		
}




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
											Msg_Key=START;
											if(Restart==1)
											{
												Restart=0;
												Msg_Key=START;
											  GUI_EndDialog(LsthWin, 0);
											  LsthWin=Cal_CreatListbox(pMsg);
											}
										}
                    break;
                case GUI_ID_CANCEL:
                    if(NCode==WM_NOTIFICATION_RELEASED)
										{
											  Exit=PLC_EXIT;
											  GUI_EndDialog(LsthWin, 0);
                        GUI_EndDialog(hWin, 0);
										}
                    break;
            }
            break;
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
void GUI_CalSetup(void) 
{ 
    //GUI_Init();
	  GUICal_InitMsg();
//	  pFloatMsg=(float*)GUICal_GetMsgPiont();
//	  pU16Msg=(U16*)GUICal_GetMsgPiont();
    WM_SetDesktopColor(GUI_WHITE);      /* Automacally update desktop window */
    WM_SetCreateFlags(WM_CF_MEMDEV);  /* Use memory devices on all windows to avoid flicker */
    Handdle=GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
}

