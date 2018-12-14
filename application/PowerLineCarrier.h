#ifndef _POWERLINECARRIER_H
#define _POWERLINECARRIER_H
#include "usart.h"
#define PLC_BUFFSIZE 250
typedef struct
{
	u8    DeviceCode;//设备码
	s8    Temperature;//温度 单位1C
	float Vi;//单位1V
	float Ii;//单位1A
	float Vo;//单位1V
	float Io;//单位1A
	float ActP;//单位1W
	float PF;//单位1%
	float Bright;//单位1%
	float Ia;//A路输入电流
	float Ib;//B路输入电流
	float Pa;//A路输入功率
	float Pb;//B路输入功率
	u16   Statu;//状态
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

//获取电源参数:输入输出 功率，电流，电压，功率因数，亮度，电源状态
s8 PLC_GetPowerData(u32 _id,PLC_PowerData *_pSturct,void * _msg);

s8 PLC_TurnLight(u32 _id,u8 bright,void * _msg);

s16 PLC_SetFactory(u32 _id,u8 _value,void * _msg);

s8 PLC_SetPower(u32 _id,float _value,void * _msg);

s8 PLC_SetVol(u32 _id,float _value,void * _msg);

s8 PLC_SetCur(u32 _id,u16 _value,void * _msg);

s8 PLC_GetCalData(u32 _id,PLC_CalStruct *_pGetData,void * _msg);

s8 PLC_GetCalStatu(u32 _id,u16 * pd,void * _msg);

//读取电源点参数：功率，电压，电流
s8 PLC_GetNmnPrmt(u32 _id,PLC_PwrPrmtStruct *_pPrmt,void * _msg);

//校正参数：功率，电压，电流
s8 PLC_CalInputP(u32 _id, float _slop,float _intcpt,void * _msg); 
s8 PLC_CalInputV(u32 _id, float _slop,float _intcpt,void * _msg);
s8 PLC_CalInputI(u32 _id, float _slop,float _intcpt,void * _msg);
s8 PLC_CalOutputP(u32 _id, float _slop,float _intcpt,void * _msg);
s8 PLC_CalOutputV(u32 _id, float _slop,float _intcpt,void * _msg);
s8 PLC_CalOutputI(u32 _id, float _slop,float _intcpt,void * _msg);
s8 PLC_CalInputVIP(u32 _id, float P, float U, float I, unsigned short int ui0_10V_Repair, void * _msg); //校正输入电压、电流、功率函数
s8 PLC_ClearEnergy(u32 _id);



/**********************************
获取CSZ7759的测量频率或者采样的ADC值
成功：返回1(频率)或者返回2（AD值）
失败：返回-1
无此项目：返回4       
数据保存在pf里面
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