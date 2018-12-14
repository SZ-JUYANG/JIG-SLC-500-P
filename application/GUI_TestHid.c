/*
******************************************************************
**                      uCGUIBuilder                            **
**                  Version:   4.0.0.0                          **
**                     2012 / 04                               **
**                   CpoyRight to: wyl                          **
**              Email:ucguibuilder@163.com                        **
**          This text was Created by uCGUIBuilder               **
******************************************************************/

#include <stdio.h>
#include "GUI_TestHid.h"
#include "main.h"
#include "UI2000.h"
#include "CommonFunction.h"



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




static OS_FLAG_GRP *  pOSflag;
static PM9805_IOStruct TestData;
static  enum{eStop=0,eStart}Button0Statu;
static  const char *ON_OFF_ButtonStr[2]={"启动","停止"};
static WM_HWIN thisHwin;


//EventsFunctionList
static void OnxxxRelease(WM_MESSAGE * pMsg){};
static void OnButtonReleased(WM_MESSAGE * pMsg){};
//EndofEventsFunctionList


/*********************************************************************
*
*       static data
*
**********************************************************************
*/

#define GUI_ID_TEXT10   GUI_ID_USER+1


/*********************************************************************
*
*       Dialog resource
*
* This table conatins the info required to create the dialog.
* It has been created by ucGUIbuilder.
*/

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect,  "HID测试",           0,                       0,  0,  480,272,FRAMEWIN_CF_MOVEABLE,0},
    //{ EDIT_CreateIndirect,      "0",                 GUI_ID_EDIT0,            42, 26, 194,28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT1,            111,61, 107, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT2,            111,95, 107, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT3,            111,129,107, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT4,            111,163,107, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT5,            336,26, 107, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT6,            336,61, 107, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT7,            336,95, 107, 28, 0,0},
    { BUTTON_CreateIndirect,    "启动",              GUI_ID_BUTTON0,          336,214,79, 25, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT8,            336,133,107, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT9,            336,167,107, 28, 0,0},
    { TEXT_CreateIndirect,      "ID",                GUI_ID_TEXT0,            17, 26,88, 28, 0,0},
    { TEXT_CreateIndirect,      "输入电压",          GUI_ID_TEXT1,            17, 57, 88, 28, 0,0},
    { TEXT_CreateIndirect,      "输入电流",          GUI_ID_TEXT2,            1,  91, 104,28, 0,0},
    { TEXT_CreateIndirect,      "功率因数",          GUI_ID_TEXT3,            17, 129,88, 28, 0,0},
    { TEXT_CreateIndirect,      "启动电流",          GUI_ID_TEXT4,            17, 163,88, 28, 0,0},
    { TEXT_CreateIndirect,      "额定功率",          GUI_ID_TEXT5,            226,132,104,28, 0,0},
    { TEXT_CreateIndirect,      "点火成功率",         GUI_ID_TEXT6,            242,57, 88, 28, 0,0},
    { TEXT_CreateIndirect,      "效率",               GUI_ID_TEXT7,            242,95, 88, 28, 0,0},
    { TEXT_CreateIndirect,      "PLC",               GUI_ID_TEXT8,            242,21, 88, 28, 0,0},
    { TEXT_CreateIndirect,      "输入功率",          GUI_ID_TEXT9,            242,167,88, 28, 0,0},
		{ TEXT_CreateIndirect,      "0",                 GUI_ID_TEXT10,            111, 26, 130,28, 0,0},
    { BUTTON_CreateIndirect,    "取消",            GUI_ID_CANCEL,           111,214,79, 25, 0,0},
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

}




static void SetEditFont(GUI_HWIN hWin, int _StartID,int Len)
{
	int i=0;
	int s_ID;
	for(i=0;i<Len;i++)
	{
		s_ID=_StartID+i;
		EDIT_SetFont(WM_GetDialogItem(hWin,s_ID),&GUI_Font24B_ASCII);
		EDIT_SetTextAlign(WM_GetDialogItem(hWin,s_ID),GUI_TA_VCENTER|GUI_TA_RIGHT);
		EDIT_SetFloatMode(WM_GetDialogItem(hWin,s_ID),0,0,100,0,0);
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
	  thisHwin = pMsg->hWin;
	  Button0Statu=eStop;
    //
    //FRAMEWIN
    //
    FRAMEWIN_SetBarColor(hWin,1,0xd7ebfa);
    FRAMEWIN_SetClientColor(hWin,0xd7ebfa);
    FRAMEWIN_SetTextColor(hWin,0x000000);
    FRAMEWIN_SetTitleHeight(hWin,15);
    //
    //GUI_ID_EDIT0
    //
	  SetEditFont(hWin,GUI_ID_EDIT0,10);
    //
    //GUI_ID_BUTTON0
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON0),BUTTON_CI_UNPRESSED,0x60a4f4);
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON0),BUTTON_CI_PRESSED,0xb9daff);
    //
    //GUI_ID_TEXT0
    //
    GUI_SetText(hWin,GUI_ID_TEXT0,10);
    //
    //GUI_ID_CANCEL
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_CANCEL),BUTTON_CI_UNPRESSED,0x60a4f4);
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_CANCEL),BUTTON_CI_PRESSED,0xb9daff);
    //
    //GUI_ID_TEXT10
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT10),0xffffff);
    TEXT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_TEXT10),GUI_TA_VCENTER|GUI_TA_RIGHT);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT10),&GUI_Font24B_ASCII);
}


static void DisplayValue(PM9805_IOStruct *pTestData,GUI_HWIN hWin)
{
	  char tp[15];
	  sprintf(tp,"%010ld",(unsigned long)pTestData->ID);
		TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT10),tp);
		sprintf(tp,"%7.3fV",pTestData->Input.Urms);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT1),tp);
		sprintf(tp,"%7.3fA",pTestData->Input.Irms);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT2),tp);
		sprintf(tp,"%5.2f%%",pTestData->Input.PF);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT3),tp);
		sprintf(tp,"%7.3fV",pTestData->Input.CFv);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT4),tp);
		sprintf(tp,"%7.3fw",pTestData->Output.Pout);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT5),tp);
		sprintf(tp,"%5.2fA",pTestData->Input.Cfi);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT6),tp);
		sprintf(tp,"%6.3f%%",(pTestData->Output.Pout)/(pTestData->Input.P));
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT7),tp);
		sprintf(tp,"%5.2f%%",pTestData->PLC);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT8),tp);
		sprintf(tp,"%7.3fw",pTestData->Input.P);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT9),tp);
}

/*********************************************************************
*
*       Dialog callback routine
*/
static void _cbCallback(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
	  char tp[40];
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
                case GUI_ID_OK:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
                case GUI_ID_CANCEL:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
                case GUI_ID_BUTTON0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            OnButtonReleased(pMsg);
												    Button0Statu=Button0Statu==eStop?eStart:eStop;
												    BUTTON_SetText(WM_GetDialogItem(hWin,GUI_ID_BUTTON0),ON_OFF_ButtonStr[Button0Statu]);
                            break;
                    }
                    break;

            }
            break;
        default:
            WM_DefaultProc(pMsg);
    }
		DisplayValue(&TestData,hWin);
}


/*********************************************************************
*
*       MainTask
*
**********************************************************************
*/
WM_HWIN TestHID(WM_MESSAGE * pMsg,void *p_arg)
{ 
	  TestMsgStruct *_p=(TestMsgStruct*)p_arg;
	  pOSflag=_p->pEvent;
	  //pTestData=(TestHIDStruct*)_p->pData;
    return GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, pMsg->hWin, 0, 0);
}



void TestHID_RefreshValue(PM9805_IOStruct _data)
{
	WM_MESSAGE  Msg;
	if(WM_IsWindow(thisHwin))
	{
		WM_SendMessage(WM_GetClientWindow(thisHwin),&Msg);
		TestData=_data;
	}
}

