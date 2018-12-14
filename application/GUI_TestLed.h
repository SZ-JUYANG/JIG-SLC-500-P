


#ifndef TESTLED_H
#define TESTLED_H



#include "WM.h"
#include "UI2050.h"


 typedef struct 
	{
		long  ID;
		float InputVol;
		float InputCur;
		float PF;
		float OutputVol;
		float OutputCur;
		float OutputPwr;
		float RippleVol;
		float RippleCur;
		float Eff;
		float PLC;
	}TestLEDStruct;
	
WM_HWIN TestLED(WM_MESSAGE * pMsg,void *p_arg) ;	
void TestLED_RefreshValue(UI2050_IOStruct _data);



#endif
