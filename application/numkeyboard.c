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
#include <stdlib.h>
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





//EventsFunctionList
#include<string.h>
void OnButtonReleased(WM_MESSAGE * pMsg){};
	extern WM_HWIN SetLed_H;
//EndofEventsFunctionList
  
	
	extern WM_HWIN fu,zi;
	static void addstr(char s,unsigned char maxlen,char *ps);
	static void backstr(char *ps);
	static InputStruct *pInputData;
	static char InputData[20];
/*********************************************************************
*
*       static data
*
**********************************************************************
*/

#define GUI_ID_BUTTON_DOT    GUI_ID_USER+1
#define GUI_ID_BUTTON_BACK   GUI_ID_USER+2
//#define GUI_ID_BUTTON12   GUI_ID_USER+1
//#define GUI_ID_BUTTON13   GUI_ID_USER+1


/*********************************************************************
*
*       Dialog resource
*
* This table conatins the info required to create the dialog.
* It has been created by ucGUIbuilder.
*/

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect,  "Caption",           0,                       0,  0,  227,236,FRAMEWIN_CF_MOVEABLE,0},
    { BUTTON_CreateIndirect,    "0",                 GUI_ID_BUTTON0,          2,  174,51, 51, 0,0},
    { BUTTON_CreateIndirect,    "1",                 GUI_ID_BUTTON1,          2,  3,  51, 51, 0,0},
    { BUTTON_CreateIndirect,    "2",                 GUI_ID_BUTTON2,          59, 3,  51, 51, 0,0},
    { BUTTON_CreateIndirect,    "3",                 GUI_ID_BUTTON3,          116,3,  51, 51, 0,0},
    { BUTTON_CreateIndirect,    "4",                 GUI_ID_BUTTON4,          2,  60, 51, 51, 0,0},
    { BUTTON_CreateIndirect,    "5",                 GUI_ID_BUTTON5,          59, 60, 51, 51, 0,0},
    { BUTTON_CreateIndirect,    "6",                 GUI_ID_BUTTON6,          116,60, 51, 51, 0,0},
    { BUTTON_CreateIndirect,    "7",                 GUI_ID_BUTTON7,          2,  117,51, 51, 0,0},
    { BUTTON_CreateIndirect,    "8",                 GUI_ID_BUTTON8,          59, 117,51, 51, 0,0},
    { BUTTON_CreateIndirect,    "9",                 GUI_ID_BUTTON9,          117,117,51, 51, 0,0},
    { BUTTON_CreateIndirect,    ".",                 GUI_ID_BUTTON_DOT,         59, 174,51, 51, 0,0},
    { BUTTON_CreateIndirect,    "CANCLE",            GUI_ID_CANCEL,           117,174,105,51, 0,0},
    { BUTTON_CreateIndirect,    "BACK",              GUI_ID_BUTTON_BACK,         171,3,  51, 79, 0,0},
    { BUTTON_CreateIndirect,    "OK",                GUI_ID_OK,               171,88, 51, 80, 0,0}
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
    FRAMEWIN_SetTitleVis(hWin,0);

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
                case GUI_ID_OK:
                    if(NCode==WM_NOTIFICATION_RELEASED)
										{
											  char *pDotStr;
											  pDotStr=strchr(InputData,'.');
											  if(*(pDotStr+1))
												{
													float TempInputData=atof(InputData);
													if(TempInputData<=pInputData->MinVal)
													{
														*(pInputData->pData)=pInputData->MinVal;
														sprintf(InputData,"%7.3f",pInputData->MinVal);
													}
													else if(TempInputData>=pInputData->MaxVal)
													{
														*(pInputData->pData)=pInputData->MaxVal;
														sprintf(InputData,"%7.3f",pInputData->MaxVal);
													}
													else
														*(pInputData->pData)=TempInputData;
													memset(InputData,0,20);
													GUI_EndDialog(hWin, 0);
												}
										}
                    break;
                case GUI_ID_CANCEL:
                    if(NCode==WM_NOTIFICATION_RELEASED)
										{
												memset(InputData,0,20);
											  GUI_EndDialog(hWin, 0);
										}
                    break;
                case GUI_ID_BUTTON0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            OnButtonReleased(pMsg);
												    addstr('0',pInputData->Len,InputData);
                            break;
                    }
                    break;
                case GUI_ID_BUTTON1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            OnButtonReleased(pMsg);
												     addstr('1',pInputData->Len,InputData);
                            break;
                    }
                    break;
                case GUI_ID_BUTTON2:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            OnButtonReleased(pMsg);
												addstr('2',pInputData->Len,InputData);
                            break;
                    }
                    break;
                case GUI_ID_BUTTON3:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            OnButtonReleased(pMsg);
												addstr('3',pInputData->Len,InputData);
                            break;
                    }
                    break;
                case GUI_ID_BUTTON4:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            OnButtonReleased(pMsg);
												addstr('4',pInputData->Len,InputData);
                            break;
                    }
                    break;
                case GUI_ID_BUTTON5:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            OnButtonReleased(pMsg);
												addstr('5',pInputData->Len,InputData);
                            break;
                    }
                    break;
                case GUI_ID_BUTTON6:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            OnButtonReleased(pMsg);
												addstr('6',pInputData->Len,InputData);
                            break;
                    }
                    break;
                case GUI_ID_BUTTON7:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            OnButtonReleased(pMsg);
												addstr('7',pInputData->Len,InputData);
                            break;
                    }
                    break;
                case GUI_ID_BUTTON8:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            OnButtonReleased(pMsg);
												addstr('8',pInputData->Len,InputData);
                            break;
                    }
                    break;
                case GUI_ID_BUTTON9:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            OnButtonReleased(pMsg);
												addstr('9',pInputData->Len,InputData);
                            break;
                    }
                    break;
								case GUI_ID_BUTTON_DOT:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            OnButtonReleased(pMsg);
												if(strchr(InputData,'.')==NULL)
												{
												   addstr('.',pInputData->Len,InputData);
												}
												if(InputData[0]=='.')
												{
													memset(InputData,0,20);
												}
                            break;
                    }
                    break;
								case GUI_ID_BUTTON_BACK:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            OnButtonReleased(pMsg);
												backstr(InputData);
                            break;
                    }
                    break;

            }
						EDIT_SetText((*pInputData).Handle,InputData);
            break;
        default:
            WM_DefaultProc(pMsg);
    }
}


static void addstr(char s,unsigned char maxlen,char *ps)
{
	unsigned char i=0;
	
	while(*ps)
	{
		i++;
		ps++;
		if(i>=maxlen)
			return;
	}
	*ps=s;
	*(ps+1)=0;
}



static void backstr(char *ps)
{
	if(*ps==0)
		return;
	while(*ps)
	{
		ps++;
	}
	*(ps-1)=0;
}









/*********************************************************************
*
*       MainTask
*
**********************************************************************
*/
GUI_HWIN InputNum(WM_MESSAGE * pMsg,InputStruct *p)
{ 
	  memset(InputData,0,20);
	  pInputData=p;
    return GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, pMsg->hWin, p->x, p->y);
	
}


