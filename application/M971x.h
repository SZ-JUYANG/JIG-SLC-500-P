


#ifndef _M971X_H
#define _M971X_H


#include "stm32f10x.h"


/*****
typedef struct
{
	u16 StartAddr;
	u16 RegNumb;
	u8  ByteNumb;
	union
	{
		u32   LongData;
		float FloatData;
	}RegValue;
}M971x_WriteRegStruct;
*****/

u32 Float2Long(float tdata);
//u8 M971x_WriteCoil(u16 StartAddr,u16 OutputValue);
//u32 M971x_ReadReg(u16 StartAddr,u16 RegNumb);
//u8 M971x_WriteReg(u16 StartAddr,u8 RegNumb,u32 Value);
s8 M971x_I_Set(float IValue);
s8 M971x_U_Set(float IValue);
s8 M971x_P_Set(float IValue);
s8 M971x_R_Set(float IValue);
s8 M971x_GetCurrt(float *pcurrt);
s8 M971x_GetVol(float *pvol);


#endif