
#ifndef NUMKEYBOARD_H
#define NUMKEYBOARD_H

#include "WM.h"

 typedef struct 
	{
		unsigned char Len;
		GUI_HWIN Handle;
		float MaxVal;
		float MinVal;
		float *pData;
		unsigned  int x,y;
	}InputStruct;
GUI_HWIN InputNum(WM_MESSAGE * pMsg,InputStruct *p);	
#endif	