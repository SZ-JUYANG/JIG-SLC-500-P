/*
******************************************************************
**                      uCGUIBuilder                            **
**                  Version:   4.0.0.0                          **
**                     2012 / 04                               **
**                   CpoyRight to: wyl                          **
**              Email:ucguibuilder@163.com                        **
**          This text was Created by uCGUIBuilder               **
******************************************************************/


#include "numkeyboard.h"
#include "setuptime.h"
#include <stdio.h>
#include "RTC.h"
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






//static TimeStruct tSetData;
static InputStruct  sInputData;
static float Year,Month,Date,Hour,Minute,Second;
//static unsigned char EDIT_mutex=0;

static const InputStruct INPUT_INFO[8]=
{
	{4,0,2100,  2000,0,220,20},  //year
	{2,0,12,    1,   0,220,20},  //month
	{2,0,31,    1,   0,220,20},  //date
	{2,0,23,    0,   0,80, 20},  //hour
	{2,0,59,    1,   0,80, 20},  //minute
	{2,0,59,    1,   0,80, 20},  //second
};


static GUI_HWIN InitIputStruct(WM_MESSAGE * pMsg,GUI_HWIN hWin, int _StartID)
{
	float *pTempData[6]={
												&Year,
												&Month,
												&Date,
												&Hour,
												&Minute,
												&Second,
	                    };
	sInputData=INPUT_INFO[_StartID-GUI_ID_EDIT0];
	sInputData.pData=pTempData[_StartID-GUI_ID_EDIT0];
	sInputData.Handle=WM_GetDialogItem(hWin,_StartID);
	return InputNum(pMsg,&sInputData);
}



//EventsFunctionList
static void OnEditClicked(WM_MESSAGE * pMsg){};
static void OnButtonReleased(WM_MESSAGE * pMsg){};
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
    { FRAMEWIN_CreateIndirect,  "Caption",           0,                       0,  0,  480,272,FRAMEWIN_CF_MOVEABLE,0},
    { BUTTON_CreateIndirect,    "保存",              GUI_ID_OK,               24, 227,79, 26, 0,0},
    { BUTTON_CreateIndirect,    "取消",            GUI_ID_CANCEL,           381,226,79, 26, 0,0},
    { EDIT_CreateIndirect,      "0",                 GUI_ID_EDIT0,            111,58, 89, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT1,            111,104,89, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT2,            111,154,89, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT3,            338,58, 89, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT4,            338,104,89, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT5,            338,154,89, 28, 0,0},
    { TEXT_CreateIndirect,      "年",              GUI_ID_TEXT0,            26, 53, 82, 29, 0,0},
    { TEXT_CreateIndirect,      "月",             GUI_ID_TEXT1,            29, 104,79, 26, 0,0},
    { TEXT_CreateIndirect,      "日",              GUI_ID_TEXT2,            21, 154,82, 28, 0,0},
    { TEXT_CreateIndirect,      "时",              GUI_ID_TEXT3,            250,54, 82, 28, 0,0},
    { TEXT_CreateIndirect,      "分",            GUI_ID_TEXT4,            250,100,82, 28, 0,0},
    { TEXT_CreateIndirect,      "秒",            GUI_ID_TEXT5,            250,154,82, 28, 0,0}
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
	  TimeStruct s_time=RTC_GetTime();
		Year=s_time.Year;
		Month=s_time.Month;
		Date=s_time.Date;
		Hour=s_time.Hour;
		Minute=s_time.Minute;
		Second=s_time.Second;
    FRAMEWIN_SetClientColor(hWin,0xd7ebfa);
    FRAMEWIN_SetTitleVis(hWin,0);
    //
    //GUI_ID_OK
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_OK),BUTTON_CI_UNPRESSED,0x60a4f4);
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_OK),BUTTON_CI_PRESSED,0xb9daff);
    //
    //GUI_ID_CANCEL
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_CANCEL),BUTTON_CI_UNPRESSED,0x60a4f4);
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_CANCEL),BUTTON_CI_PRESSED,0xb9daff);
    //
    //GUI_ID_EDIT0
    //
    GUI_SetEditFont(hWin,GUI_ID_EDIT0,6,&GUI_Font24B_ASCII);
    //
    //GUI_ID_TEXT0
    //
    GUI_SetText(hWin,GUI_ID_TEXT0,6);

}

static void SetTime_Display(GUI_HWIN hWin, int _StatrID)
{
	char tp[10];
	sprintf(tp,"%d",(int)Year);
	EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT0),tp);
	sprintf(tp,"%d",(int)Month);
	EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT1),tp);
	sprintf(tp,"%d",(int)Date);
	EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT2),tp);
	sprintf(tp,"%d",(int)Hour);
	EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT3),tp);
	sprintf(tp,"%d",(int)Minute);
	EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT4),tp);
	sprintf(tp,"%d",(int)Second);
	EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT5),tp);
}


/*********************************************************************
*
*       Dialog callback routine
*/
static void _cbCallback(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
	  const u8 s_LeapMonthDay[12]={31,29,31,30,31,30,31,31,30,31,30,31};
    const u8 s_CommMonthDay[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    WM_HWIN hWin = pMsg->hWin;
	  static GUI_HWIN s_InputHandle=0;
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
										{
												if(WM_IsWindow(s_InputHandle))
												{
													GUI_EndDialog(s_InputHandle, 0);
												}
												RTC_SaveTime((u16)Year,(u16)Month,(u16)Date,(u16)Hour,(u16)Minute,(u16)Second);
                        GUI_EndDialog(hWin, 0);
										}
                    break;
                case GUI_ID_CANCEL:
                    if(NCode==WM_NOTIFICATION_RELEASED)
										{
												if(WM_IsWindow(s_InputHandle))
												{
													GUI_EndDialog(s_InputHandle, 0);
												}
                        GUI_EndDialog(hWin, 0);
										}
                    break;
								case GUI_ID_EDIT2:
								{
									u8 const *_pMontData;
									if(((int)Year-2000)%4==0)
									{
										_pMontData=s_LeapMonthDay;
									}
									else
									{
										_pMontData=s_CommMonthDay;
									}
							    sInputData.MaxVal=_pMontData[(u16)Month-1];
								}
                case GUI_ID_EDIT0:
								case GUI_ID_EDIT1:
								case GUI_ID_EDIT3:
								case GUI_ID_EDIT4:
								case GUI_ID_EDIT5:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
													  if(WM_IsWindow(s_InputHandle))
														{
															GUI_EndDialog(s_InputHandle, 0);
														}
														s_InputHandle=InitIputStruct(pMsg,hWin,Id);
                            break;
                    }
                    break;
            }
            break;
        default:
            WM_DefaultProc(pMsg);
    }
		SetTime_Display(hWin,GUI_ID_EDIT0);
}


/*********************************************************************
*
*       MainTask
*
**********************************************************************
*/
WM_HWIN SetupTime(WM_MESSAGE * pMsg)
{ 
	return GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
}
