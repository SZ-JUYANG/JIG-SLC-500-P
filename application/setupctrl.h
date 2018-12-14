


#ifndef SETUPCTRL_H
#define SETUPCTRL_H

#include "WM.h"

 typedef struct 
	{
		float PLC;
		float Vol;
		float PWM;
	}SetCTRLStruct;

	
WM_HWIN SetupCTRL1(WM_MESSAGE * pMsg,SetCTRLStruct *pSetData);



#endif