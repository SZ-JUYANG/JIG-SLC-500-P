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
#include "setupctrl.h"
#include <stdio.h>



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




static SetCTRLStruct *pSetData;
static SetCTRLStruct TempStructData;
static InputStruct  InputData;
//static unsigned char EDIT_mutex=0;

static const float MAXPLC=99.99,  MINPLC=0;
static const float MAXVOL=99.99,  MINVOL=0;
static const float MAXPWM=99.99,  MINPWM=0;

static const unsigned char PLC_LEN=5;
static const unsigned char VOL_LEN=5;
static const unsigned char PWM_LEN=5;

static const unsigned int RX=250,RY=20,LX=80,LY=20;




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
    { BUTTON_CreateIndirect,    "保存",              GUI_ID_OK,               57, 207,79, 26, 0,0},
    { BUTTON_CreateIndirect,    "取消",            GUI_ID_CANCEL,           354,207,79, 26, 0,0},
    { EDIT_CreateIndirect,      "0",                 GUI_ID_EDIT0,            140,41, 89, 28, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT1,            140,86, 89, 28, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT2,            140,132,89, 28, 0,0},
    { TEXT_CreateIndirect,      "PLC",               GUI_ID_TEXT0,            55, 40, 82, 29, 0,0},
    { TEXT_CreateIndirect,      "电压",               GUI_ID_TEXT1,            55, 83, 79, 26, 0,0},
    { TEXT_CreateIndirect,      "PWM",               GUI_ID_TEXT2,            52, 132,82, 28, 0,0}
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
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT0),&GUI_Font24B_ASCII);
    EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT0),GUI_TA_VCENTER|GUI_TA_RIGHT);
    EDIT_SetFloatMode(WM_GetDialogItem(hWin,GUI_ID_EDIT0),0,0,100,0,0);
		//
    //GUI_ID_EDIT1
    //
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT1),&GUI_Font24B_ASCII);
    EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT1),GUI_TA_VCENTER|GUI_TA_RIGHT);
    EDIT_SetFloatMode(WM_GetDialogItem(hWin,GUI_ID_EDIT1),0,0,100,0,0);
		//
    //GUI_ID_EDIT2
    //
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT2),&GUI_Font24B_ASCII);
    EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT2),GUI_TA_VCENTER|GUI_TA_RIGHT);
    EDIT_SetFloatMode(WM_GetDialogItem(hWin,GUI_ID_EDIT2),0,0,100,0,0);
    //
    //GUI_ID_TEXT0
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT0),0xd7ebfa);
    TEXT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_TEXT0),GUI_TA_VCENTER|GUI_TA_RIGHT);
    //
    //GUI_ID_TEXT1
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT1),0xd7ebfa);
    TEXT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_TEXT1),GUI_TA_VCENTER|GUI_TA_RIGHT);
    //
    //GUI_ID_TEXT2
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT2),0xd7ebfa);
    TEXT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_TEXT2),GUI_TA_VCENTER|GUI_TA_RIGHT);

}




/*********************************************************************
*
*       Dialog callback routine
*/
static void _cbCallback(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
	  char tp[10];
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
										{
											  *pSetData=TempStructData;
                        GUI_EndDialog(hWin, 0);
										}
                    break;
                case GUI_ID_CANCEL:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
                case GUI_ID_EDIT0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            OnEditClicked(pMsg);
												    InputData.pData=&TempStructData.PLC;
												    InputData.Handle=WM_GetDialogItem(hWin,GUI_ID_EDIT0);
														InputData.Len=PLC_LEN;
														InputData.MaxVal=MAXPLC;
														InputData.MinVal=MINPLC;
												    InputData.x=RX;
												    InputData.y=RY;
													  InputNum(pMsg,&InputData);
                            break;
                    }
                    break;
                case GUI_ID_EDIT1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            OnEditClicked(pMsg);
												    InputData.pData=&TempStructData.Vol;
												    InputData.Handle=WM_GetDialogItem(hWin,GUI_ID_EDIT1);
														InputData.Len=VOL_LEN;
														InputData.MaxVal=MAXVOL;
														InputData.MinVal=MINVOL;
												    InputData.x=RX;
												    InputData.y=RY;
													  InputNum(pMsg,&InputData);
                            break;
                    }
                    break;
                case GUI_ID_EDIT2:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            OnEditClicked(pMsg);
												    InputData.pData=&TempStructData.PWM;
												    InputData.Handle=WM_GetDialogItem(hWin,GUI_ID_EDIT2);
														InputData.Len=PWM_LEN;
														InputData.MaxVal=MAXPWM;
														InputData.MinVal=MINPWM;
												    InputData.x=RX;
												    InputData.y=RY;
													  InputNum(pMsg,&InputData);
                            break;
                    }
                    break;

            }
            break;
        default:
            WM_DefaultProc(pMsg);
    }
		sprintf(tp,"%5.2f",TempStructData.PLC);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT0),tp);
		sprintf(tp,"%5.2f",TempStructData.Vol);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT1),tp);
		sprintf(tp,"%5.2f",TempStructData.PWM);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT2),tp);
}


/*********************************************************************
*
*       MainTask
*
**********************************************************************
*/
WM_HWIN SetupCTRL1(WM_MESSAGE * pMsg,SetCTRLStruct *_pSetData)
{ 
	  pSetData=_pSetData;
	  TempStructData=*_pSetData;
    return GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, pMsg->hWin, 0, 0);
}

