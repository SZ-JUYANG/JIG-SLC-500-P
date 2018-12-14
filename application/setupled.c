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





static SetLEDStruct tSetData;
static InputStruct  sInputData;
//static unsigned char EDIT_mutex=0;
static const InputStruct INPUT_INFO[8]=
{
	{5,0,99.99,  0,0,220,20},  //pf
	{7,0,999.999,0,0,220,20},  //vh
	{7,0,99.999, 0,0,220,20},  //vl
	{6,0,999.99, 0,0,220,20},  //rv
	{6,0,99.999, 0,0,80, 20},  //ri
	{7,0,999.999,0,0,80, 20},  //ph
	{7,0,999.999,0,0,80, 20},  //pl
	{5,0,99.99,  0,0,80, 20},  //ef
};

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
    { EDIT_CreateIndirect,      "0",                 GUI_ID_EDIT0,           111,41, 107,28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT1,            111,85, 107,28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT2,            111,129,107,28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT3,            111,173,107,28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT4,            355,41, 107,28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT5,            355,85, 105,28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT6,            355,129,105,28, 0,0},
    { EDIT_CreateIndirect,       "0",               GUI_ID_EDIT7,            355,173,105,28, 0,0},
    { BUTTON_CreateIndirect,    "下一页",              GUI_ID_BUTTON0,       206,226,79, 25, 0,0},
    { TEXT_CreateIndirect,      "功率因数",                GUI_ID_TEXT0,     1,  41, 107,29, 0,0},
    { TEXT_CreateIndirect,      "最大输入电压",           GUI_ID_TEXT1,      1,  85, 107,26, 0,0},
    { TEXT_CreateIndirect,      "最小输入电压",          GUI_ID_TEXT2,       1,  129,107,28, 0,0},
    { TEXT_CreateIndirect,      "纹波电压",           GUI_ID_TEXT3,          1,  173,107,28, 0,0},
    { TEXT_CreateIndirect,      "纹波电流",           GUI_ID_TEXT4,            239,41, 110,28, 0,0},
    { TEXT_CreateIndirect,      "最大输出功率",          GUI_ID_TEXT5,         239,85, 110,28, 0,0},
    { TEXT_CreateIndirect,      "最小输出功率",         GUI_ID_TEXT6,          239,129,110,28, 0,0},
    { TEXT_CreateIndirect,      "电源效率",               GUI_ID_TEXT7,        239,173,110,28, 0,0},
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



/****
static s8 SaveLEDSetData(SetLEDStruct _savedata)
{
	
	return 1;
}
****/



static GUI_HWIN InitIputStruct(WM_MESSAGE * pMsg,GUI_HWIN hWin, int _StartID)
{
	float *pTempData[8]={
												&tSetData.PF,
												&tSetData.InVH,
												&tSetData.InVL,
												&tSetData.RippV,
												&tSetData.RippI,
												&tSetData.OutPH,
												&tSetData.OutPL,
												&tSetData.Eff,
	                    };
	sInputData=INPUT_INFO[_StartID-GUI_ID_EDIT0];
	sInputData.pData=pTempData[_StartID-GUI_ID_EDIT0];
	sInputData.Handle=WM_GetDialogItem(hWin,_StartID);
	return InputNum(pMsg,&sInputData);
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
	  tSetData=*(SetLEDStruct*)ReadLED();
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

		GUI_SetEditFont(hWin,GUI_ID_EDIT0,8,&GUI_Font24B_ASCII);
    //
    //GUI_ID_BUTTON0
    //
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON0),BUTTON_CI_UNPRESSED,0x60a4f4);
    BUTTON_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_BUTTON0),BUTTON_CI_PRESSED,0xb9daff);
    //
    //GUI_ID_TEXT0
    //
    GUI_SetText(hWin,GUI_ID_TEXT0,8);

}


static void SetLED_Display(GUI_HWIN hWin)
{
	  char tp[10];
		sprintf(tp,"%5.2f%%",tSetData.PF);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT0),tp);
		sprintf(tp,"%7.3fv",tSetData.InVH);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT1),tp);
		sprintf(tp,"%7.3fv",tSetData.InVL);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT2),tp);
		sprintf(tp,"%6.3fv",tSetData.RippV);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT3),tp);
		sprintf(tp,"%6.3fA",tSetData.RippI);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT4),tp);
		sprintf(tp,"%7.3fw",tSetData.OutPH);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT5),tp);
		sprintf(tp,"%7.3fw",tSetData.OutPL);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT6),tp);
		sprintf(tp,"%5.2f%%",tSetData.Eff);
		EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT7),tp);
}

/*********************************************************************
*
*       Dialog callback routine
*/
static void _cbCallback(WM_MESSAGE * pMsg) 
{
  int NCode, Id;
	static GUI_HWIN s_InputHandle=0;
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
												if(WM_IsWindow(s_InputHandle))
												{
													GUI_EndDialog(s_InputHandle, 0);
												}
												SaveLEDSetData(&tSetData);
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
								case GUI_ID_EDIT1:
								case GUI_ID_EDIT2:
								case GUI_ID_EDIT3:
								case GUI_ID_EDIT4:
								case GUI_ID_EDIT5:
								case GUI_ID_EDIT6:
								case GUI_ID_EDIT7:
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
													  SetupLED2(pMsg,&tSetData);
                            OnButtonReleased(pMsg);
                            break;
                    }
                    break;

            }
            break;
        default:
            WM_DefaultProc(pMsg);
    }
		SetLED_Display(hWin);
}





GUI_WIDGET_CREATE_INFO INPUTFR={FRAMEWIN_CreateIndirect,"Caption",0,0,0,227,236,0,0};

WM_HWIN SetupLED1(WM_MESSAGE * pMsg)
{
	
	 //return GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, pMsg->hWin, 0, 0);
   //WM_CreateWindow(180,20,227,236,WM_CF_SHOW,&_cbCallback,0);
//	FRAMEWIN_Create("test",&_cbCallback0,WM_CF_SHOW,180,20,227,227);
	//WM_CreateWindowAsChild(180,20,227,236, pMsg->hWin,WM_CF_SHOW,&_cbCallback0,0);
	return 0;
}





SetLEDStruct GetLEDSetData()
{
	return *(SetLEDStruct*)ReadLED();
}

