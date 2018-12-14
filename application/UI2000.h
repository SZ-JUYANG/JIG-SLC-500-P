


#ifndef _UI2000_H
#define _UI2000_H

#include "stm32f10x.h"



//结构体里面的成员顺序不能变
typedef struct
{
//原来的
//	float Urms;//有效电压
//	float Irms;
//	float Irmsw;
//	float P;
//	float PF;
//	float PFw;
//	float Ferq;
//	float Hfi;
//	float CFv;
//	float Cfi;
	//PM9805
    float Urms;//有效电压
	float Irms;
	float P;
	float Ferq;
	float PF;
	float Irmsw;
	float PFw;
	float Hfi;
	float CFv;
	float Cfi;
}PM9805_InputStruct;



//结构体里面的成员顺序不能变
typedef struct
{
	float Ulrms;
	float Ilrms;
	float Ulm;
	float Ilm;
	float Ilcf;
	float Pout;
	float fout;
}PM9805_OutputStruct;


typedef struct
{
	PM9805_InputStruct Input;
	PM9805_OutputStruct Output;
	u32 ID;
	float PLC;
}PM9805_IOStruct;




s8 PM9805_GetInput(PM9805_InputStruct *pStruct);
s8 PM9805_GetOutput(PM9805_OutputStruct *pStruct);
s8 PM9805_GetIO(PM9805_IOStruct *pStruct);















#endif



