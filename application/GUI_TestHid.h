


#ifndef TESTHID_H
#define TESTHID_H

#include "WM.h"
#include "UI2000.h"


 typedef struct 
	{
		long  ID;
		float InputVol;
		float InputCur;
		float PF;
		float StartCur;
		float ConstPwr;
		float StartRate;
		float Eff;	
		float PLC;
		float InputPwr;
	}TestHIDStruct;
	
WM_HWIN TestHID(WM_MESSAGE * pMsg,void *p_arg);
void TestHID_RefreshValue(PM9805_IOStruct _data);





#endif
