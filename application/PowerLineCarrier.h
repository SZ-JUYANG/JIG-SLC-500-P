#ifndef _POWERLINECARRIER_H
#define _POWERLINECARRIER_H
#include "usart.h"
#define PLC_BUFFSIZE 250
typedef struct
{
	u8    DeviceCode;//�豸��
	s8    Temperature;//�¶� ��λ1C
	float Vi;//��λ1V
	float Ii;//��λ1A
	float Vo;//��λ1V
	float Io;//��λ1A
	float ActP;//��λ1W
	float PF;//��λ1%
	float Bright;//��λ1%
	float Ia;//A·�������
	float Ib;//B·�������
	float Pa;//A·���빦��
	float Pb;//B·���빦��
	u16   Statu;//״̬
}PLC_PowerData;

typedef struct
{
	float PSlope;
	float PIntcpt;
	float VSlope;
	float VIntcpt;
	float ISlope;
	float IIntcpt;
}PLC_CalStruct;


typedef struct
{
	float P;
	float V;
	u16   I;
}PLC_PwrPrmtStruct;


typedef struct
{
	u16 P;
	u16 V;
	u16 I;
}PLC_PwrTestFStruct;






void RS485_Init();

//��ȡ��Դ����:������� ���ʣ���������ѹ���������������ȣ���Դ״̬
s8 PLC_GetPowerData(u32 _id,PLC_PowerData *_pSturct,void * _msg);

s8 PLC_TurnLight(u32 _id,u8 bright,void * _msg);

s16 PLC_SetFactory(u32 _id,u8 _value,void * _msg);

s8 PLC_SetPower(u32 _id,float _value,void * _msg);

s8 PLC_SetVol(u32 _id,float _value,void * _msg);

s8 PLC_SetCur(u32 _id,u16 _value,void * _msg);

s8 PLC_GetCalData(u32 _id,PLC_CalStruct *_pGetData,void * _msg);

s8 PLC_GetCalStatu(u32 _id,u16 * pd,void * _msg);

//��ȡ��Դ����������ʣ���ѹ������
s8 PLC_GetNmnPrmt(u32 _id,PLC_PwrPrmtStruct *_pPrmt,void * _msg);

//У�����������ʣ���ѹ������
s8 PLC_CalInputP(u32 _id, float _slop,float _intcpt,void * _msg); 
s8 PLC_CalInputV(u32 _id, float _slop,float _intcpt,void * _msg);
s8 PLC_CalInputI(u32 _id, float _slop,float _intcpt,void * _msg);
s8 PLC_CalOutputP(u32 _id, float _slop,float _intcpt,void * _msg);
s8 PLC_CalOutputV(u32 _id, float _slop,float _intcpt,void * _msg);
s8 PLC_CalOutputI(u32 _id, float _slop,float _intcpt,void * _msg);
s8 PLC_CalInputVIP(u32 _id, float P, float U, float I, unsigned short int ui0_10V_Repair, void * _msg); //У�������ѹ�����������ʺ���
s8 PLC_ClearEnergy(u32 _id);



/**********************************
��ȡCSZ7759�Ĳ���Ƶ�ʻ��߲�����ADCֵ
�ɹ�������1(Ƶ��)���߷���2��ADֵ��
ʧ�ܣ�����-1
�޴���Ŀ������4       
���ݱ�����pf����
*************************************/
s8 PLC_GetInputF(u32 _id,PLC_PwrTestFStruct *pf,void * _msg);


s8 PLC_GetOutputV(u32 _id,PLC_PwrTestFStruct *pf,void * _msg);



s8 PLC_GetID(u32 *pID,void * _msg);

s8 PLC_SaveCalData(u32 _id,void * _msg);

float PLC_GetSuccessRate(void);

s8 WritePhyID2Device(unsigned long ulPhyID, unsigned long ulPhyID_New);
s8 PLC_0_10V_PWM_Calibrate(u32 _id, u8 index, u16 ui0_10V_Param, u8 DeviceCode);

void SendSafteCmd(void);
u8 CheckSafteMode(void);
void MasterReset(void);
void NemaReset(void);
#endif