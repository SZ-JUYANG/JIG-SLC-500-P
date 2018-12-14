



#ifndef _UI2050_H
#define _UI2050_H


#include "stm32f10x.h"


//结构体成员顺序不能改变
typedef struct
{
	float Urms;
	float Irms;
	float P;
	float PF;
	float Ferq;
	float Hfi;
	float Irmsw;
	float PFw;
	float CFv;
	float Cfi;
	float UrmsZero;
	float IrmsZero;
	float dbUrms;
	float dbIrms;
	float SampleRefFreq;
}UI2050_InputStruct;
 



//结构体成员顺序不能改变
typedef struct
{
	float Ulrms;
	float Ilrms;
	float Pout;
	float IlrmsHi;
	float PowEffi;
	float UlrmsZero;
	float IlrmsZero;
	float dbUlrms;
	float dbIlrms;
	float SampleRefFreq;
}UI2050_OutputStruct;





typedef struct
{
	UI2050_InputStruct  Input;
	UI2050_OutputStruct Output;
	u32 ID;
	float PLC;
}UI2050_IOStruct;


//读取输入数据，数据存在指针所指的结构体里面
//成功返回1，失败返回-1
s8 UI2050_GetInput(UI2050_InputStruct *pStruct);



s8 UI2050_GetOutput(UI2050_OutputStruct *pStruct);
s8 UI2050_GetIO(UI2050_IOStruct *pStruct);



#endif




