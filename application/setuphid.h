


#ifndef SETUPHID_H
#define SETUPHID_H

//#include "stdint.h"
#include "WM.h"

 typedef struct 
	{
		float  PF;
		float PLC;
		float OutVH;
		float OutVL;
		float OutIH;
		float OutIL;
		float OutPH;
		float OutPL;
		float Eff;
		float OverV;
		float UnderV;
		float Derate;
		float InP;
		float StartV;
		float StartI;
	}SetHIDStruct;

	
WM_HWIN SetupHID1(WM_MESSAGE * pMsg, SetHIDStruct *pSetData, unsigned  int *rb);
int SetupHID2(WM_MESSAGE * pMsg, SetHIDStruct *pSetData, SetHIDStruct *ptSetData);


#endif