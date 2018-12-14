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
#include "setuphid.h"
#include <stdio.h>
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


static SetHIDStruct *pSetData,*ptSetData;
static InputStruct  sInputData;
static int ButtonStatu=0;
//static unsigned int *pReButton;


static const InputStruct INPUT_INFO[7]=
{
	{5,0,99.99,  0,0,220,20},  //plc
	{7,0,999.999,0,0,220,20},  //sv
	{7,0,999.999,0,0,220,20},  //sc
	{5,0,99.99,  0,0,220,20},  //eff
	{7,0,999.999,0,0,80, 20},  //ov
	{7,0,999.999,0,0,80, 20},  //uv
	{7,0,999.999,0,0,80, 20},  //dv
};
static GUI_HWIN InitIputStruct(WM_MESSAGE * pMsg,GUI_HWIN hWin, int _StartID)
{
	float *pTempData[7];
	pTempData[0]=&ptSetData->PLC;
	pTempData[1]=&ptSetData->StartV;
	pTempData[2]=&ptSetData->StartI;
	pTempData[3]=&ptSetData->Eff;
	pTempData[4]=&ptSetData->OverV;
	pTempData[5]=&ptSetData->UnderV;
	pTempData[6]=&ptSetData->Derate;
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

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect,  "HID设置",           0,                       0,  0,  480,272,0,0},
    { BUTTON_CreateIndirect,    "保存",              GUI_ID_OK,               24, 227,79, 26, 0,0},
    { BUTTON_CreateIndirect,    "取消",            GUI_ID_CANCEL,           381,226,79, 26, 0,0},
    { EDIT_CreateIndirect,      "0",                 GUI_ID_EDIT0,            111,37, 89, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT1,            111,80, 89, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT2,            111,123,89, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT3,            111,168,89, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT4,            355,41, 89, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT5,            355,85, 89, 28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT6,            355,129,89, 28, 0,0},
    { BUTTON_CreateIndirect,    "向前",              GUI_ID_BUTTON0,          206,226,79, 25, 0,0},
    { TEXT_CreateIndirect,      "PLC",               GUI_ID_TEXT0,            26, 37, 82, 29, 0,0},
    { TEXT_CreateIndirect,      "启动电压",            GUI_ID_TEXT1,            29, 82, 79, 26, 0,0},
    { TEXT_CreateIndirect,      "启动电流",            GUI_ID_TEXT2,            26, 123,82, 28, 0,0},
    { TEXT_CreateIndirect,      "效率",               GUI_ID_TEXT3,            26, 168,82, 28, 0,0},
    { TEXT_CreateIndirect,      "过压",             GUI_ID_TEXT4,            267,38, 82, 28, 0,0},
    { TEXT_CreateIndirect,      "欠压",            GUI_ID_TEXT5,            267,85, 82, 28, 0,0},
    { TEXT_CreateIndirect,      "降额电压",           GUI_ID_TEXT6,            267,129,82, 28, 0,0},
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
		GUI_SetEditFont(hWin,GUI_ID_EDIT0,7,&GUI_Font24B_ASCII);
    //
    //GUI_ID_BUTTON0
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON0),BUTTON_CI_UNPRESSED,0x60a4f4);
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON0),BUTTON_CI_PRESSED,0xb9daff);
    //
    //GUI_ID_TEXT0
    GUI_SetText(hWin,GUI_ID_TEXT0,7);

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
            ButtonStatu=Id;	
            switch (Id) 
            {
                case GUI_ID_OK:
                    if(NCode==WM_NOTIFICATION_RELEASED)
										{
											  if(WM_IsWindow(s_InputHandle))
												{
													GUI_EndDialog(s_InputHandle, 0);
												}
                        (*pSetData)=(*ptSetData);													
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
                case GUI_ID_EDIT0:
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
                case GUI_ID_EDIT1:
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
                case GUI_ID_EDIT2:
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
                case GUI_ID_EDIT3:
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
                case GUI_ID_EDIT4:
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
                case GUI_ID_EDIT6:
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
                            OnButtonReleased(pMsg);
												    GUI_EndDialog(hWin, 0);
                            break;
                    }
                    break;

            }
            break;
        default:
            WM_DefaultProc(pMsg);
    }
		sprintf(tp,"%5.2f%%",ptSetData->PLC);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT0),tp);
		sprintf(tp,"%7.3fv",ptSetData->StartV);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT1),tp);
		sprintf(tp,"%7.3fA",ptSetData->StartI);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT2),tp);
		sprintf(tp,"%5.2f%%",ptSetData->Eff);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT3),tp);
		sprintf(tp,"%7.3fv",ptSetData->OverV);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT4),tp);
		sprintf(tp,"%7.3fv",ptSetData->UnderV);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT5),tp);
		sprintf(tp,"%7.3fv",ptSetData->Derate);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT6),tp);
}


/*********************************************************************
*
*       MainTask
*
**********************************************************************
*/
int SetupHID2(WM_MESSAGE * pMsg, SetHIDStruct *_pSetData, SetHIDStruct *_ptSetData)
{ 
   pSetData=_pSetData;
	 ptSetData=_ptSetData;
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

