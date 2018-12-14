

#ifndef _COMMONFUNCTION_H
#define _COMMONFUNCTION_H


#include "stm32f10x.h"
#include "GUI.h"
#include "DIALOG.h"
#include "WM.h"








u32 Float2Long(float tdata);
float Long2Float(u32 tdata);
void GUI_SetEditFont(GUI_HWIN hWin, int _StartID,int Len,const GUI_FONT *pfont);
void GUI_SetText(GUI_HWIN hWin, int _StartID,int Len);







#endif




