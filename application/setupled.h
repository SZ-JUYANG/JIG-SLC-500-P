
#ifndef SETUPLED_H
#define SETUPLED_H

//#include "stdint.h"
#include "WM.h"

 typedef struct 
	{
		float  PF;  //1
		float InVH;//2
		float InVL;//3
		float RippV;//4
		float RippI;//5
		float OutPH;//6
		float OutPL;//7
		float Eff;//8
		float OutIH;//9
		float OutIL;//10
		float OverV;//11
		float UnderV;//12
		float Derate;//13
		float PLC;//14
		
	}SetLEDStruct;

	
WM_HWIN SetupLED1(WM_MESSAGE * pMsg);
int     SetupLED2(WM_MESSAGE * pMsg,SetLEDStruct *ptSetData);



#endif