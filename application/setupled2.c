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
#include "setupled.h"
#include <stdio.h>
#include "CommonFunction.h"
#include "Flash.h"

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




static int ButtonStatu;

static SetLEDStruct *pSetData;
static SetLEDStruct *ptSetData;
static InputStruct  sInputData;
//static unsigned char EDIT_mutex=0;

static const InputStruct INPUT_INFO[6]=
{
	{5,0,99.99,  0,0,220,20},  //plc
	{6,0,999.999,0,0,220,20},  //ov
	{6,0,999.999, 0,0,220,20},  //uv
	{6,0,999.999, 0,0,80,20},  //dv
	{6,0,999.999,0,0,80, 20},  //ih
	{5,0,999.999,0,0,80, 20},  //il
};


static GUI_HWIN InitIputStruct(WM_MESSAGE * pMsg,GUI_HWIN hWin, int _StartID)
{
	float *pTempData[6];
	pTempData[0]=&ptSetData->PLC;
	pTempData[1]=&ptSetData->OverV;
	pTempData[2]=&ptSetData->UnderV;
	pTempData[3]=&ptSetData->Derate;
	pTempData[4]=&ptSetData->OutIH;
	pTempData[5]=&ptSetData->OutIL;
	sInputData=INPUT_INFO[_StartID-GUI_ID_EDIT0];
	sInputData.pData=pTempData[_StartID-GUI_ID_EDIT0];
	sInputData.Handle=WM_GetDialogItem(hWin,_StartID);
	return InputNum(pMsg,&sInputData);
}
//EventsFunctionList
static void OnButtonReleased(WM_MESSAGE * pMsg){};
static void OnEditClicked(WM_MESSAGE * pMsg){};
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
/****
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect,  "LED设置",           0,                       0,  0,  480,272,0,0},
    { BUTTON_CreateIndirect,    "保存",              GUI_ID_OK,               24, 227,79, 26, 0,0},
    { BUTTON_CreateIndirect,    "取消",            GUI_ID_CANCEL,           381,226,79, 26, 0,0},
    { EDIT_CreateIndirect,      "0",                 GUI_ID_EDIT0,            111,37, 107,28, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT1,            111,85, 107,28, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT2,            111,133,107,28, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT3,            355,37, 107, 28, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT4,            355,85, 107,28, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT5,            355,133,107,28, 0,0},
    { BUTTON_CreateIndirect,    "向前",              GUI_ID_BUTTON0,          206,226,79, 25, 0,0},
    { TEXT_CreateIndirect,      "PLC",               GUI_ID_TEXT0,            1,  37, 107,29, 0,0},
    { TEXT_CreateIndirect,      "过压",             GUI_ID_TEXT1,            1,  85, 107,28, 0,0},
    { TEXT_CreateIndirect,      "欠压",            GUI_ID_TEXT2,            1,  133,107,28, 0,0},
    { TEXT_CreateIndirect,      "降额电压",            GUI_ID_TEXT3,            242,37, 107,28, 0,0},
    { TEXT_CreateIndirect,      "最大输出电流",          GUI_ID_TEXT4,            242,85, 107,28, 0,0},
    { TEXT_CreateIndirect,      "最小输出电流",         GUI_ID_TEXT5,            242,133,107,28, 0,0},
};
****/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect,  "Caption",           0,                       0,  0,  480,272,FRAMEWIN_CF_MOVEABLE,0},
    { BUTTON_CreateIndirect,    "保存",              GUI_ID_OK,               24, 227,79, 26, 0,0},
    { BUTTON_CreateIndirect,    "取消",            GUI_ID_CANCEL,           381,226,79, 26, 0,0},
    { EDIT_CreateIndirect,      "0",                 GUI_ID_EDIT0,           111,38, 107,28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT1,            111,80, 107,28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT2,            111,123,107,28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT3,            355,37, 107, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT4,            355,85, 107,28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT5,            355,133,107,28, 0,0},
    { BUTTON_CreateIndirect,    "向前",              GUI_ID_BUTTON0,          206,226,79, 25, 0,0},
    { TEXT_CreateIndirect,      "PLC",                GUI_ID_TEXT0,            1,  41, 107,28, 0,0},
    { TEXT_CreateIndirect,      "过压",           GUI_ID_TEXT1,                1,  85, 107,28, 0,0},
    { TEXT_CreateIndirect,      "欠压",          GUI_ID_TEXT2,                 1,  129,107,28, 0,0},
    { TEXT_CreateIndirect,      "降额电压",           GUI_ID_TEXT3,            239,41, 107,28, 0,0},
    { TEXT_CreateIndirect,      "最大输出电流",           GUI_ID_TEXT4,        239,85, 107,28, 0,0},
    { TEXT_CreateIndirect,      "最小输出电流",          GUI_ID_TEXT5,         239,129,107,28, 0,0},
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
	  BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON0),BUTTON_CI_UNPRESSED,0x60a4f4);
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON0),BUTTON_CI_PRESSED,0xb9daff);
    //GUI_ID_EDIT
		GUI_SetEditFont(hWin,GUI_ID_EDIT0,6,&GUI_Font24B_ASCII);
    //GUI_ID_TEXT
    GUI_SetText(hWin,GUI_ID_TEXT0,6);

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
											  ButtonStatu=GUI_ID_OK;
                        SaveLEDSetData(ptSetData);												
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
											  ButtonStatu=GUI_ID_CANCEL;
                        GUI_EndDialog(hWin, 0);
										}
                    break;
                case GUI_ID_EDIT0:
								case GUI_ID_EDIT1:
							  case GUI_ID_EDIT2:
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
                case GUI_ID_BUTTON0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            if(WM_IsWindow(s_InputHandle))
														{
															GUI_EndDialog(s_InputHandle, 0);
														}
												    ButtonStatu=GUI_ID_BUTTON0;
												    GUI_EndDialog(hWin, 0);
                            break;
                    }
                    break;

            }
            break;
        default:
            WM_DefaultProc(pMsg);
    }
		sprintf(tp,"%5.2f%%",(*ptSetData).PLC);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT0),tp);
		sprintf(tp,"%7.3fV",(*ptSetData).OverV);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT1),tp);
		sprintf(tp,"%7.3fV",(*ptSetData).UnderV);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT2),tp);
		sprintf(tp,"%7.3fV",(*ptSetData).Derate);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT3),tp);
		sprintf(tp,"%7.3fA",(*ptSetData).OutIH);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT4),tp);
		sprintf(tp,"%7.3fA",(*ptSetData).OutIL);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT5),tp);
}



/*********************************************************************
*
*       MainTask
*
**********************************************************************
*/
int SetupLED2(WM_MESSAGE * pMsg,SetLEDStruct *pt)
{ 
	  ptSetData=pt;
		GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, pMsg->hWin, 0, 0);
	  if(ButtonStatu==GUI_ID_OK)
		{
			GUI_EndDialog(pMsg->hWin, 0);
		}
		else if(ButtonStatu==GUI_ID_CANCEL)
		{
			GUI_EndDialog(pMsg->hWin, 0);
		}
		else if(ButtonStatu==GUI_ID_BUTTON0)
		{
		}
    return 0;
}



