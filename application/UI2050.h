



#ifndef _UI2050_H
#define _UI2050_H


#include "stm32f10x.h"


//�ṹ���Ա˳���ܸı�
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
 



//�ṹ���Ա˳���ܸı�
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


//��ȡ�������ݣ����ݴ���ָ����ָ�Ľṹ������
//�ɹ�����1��ʧ�ܷ���-1
s8 UI2050_GetInput(UI2050_InputStruct *pStruct);



s8 UI2050_GetOutput(UI2050_OutputStruct *pStruct);
s8 UI2050_GetIO(UI2050_IOStruct *pStruct);



#endif




