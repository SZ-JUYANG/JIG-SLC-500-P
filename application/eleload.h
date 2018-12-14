

#ifndef _ELELOAD_H
#define _ELELOAD_H

#include "stdint.h"
#include "stm32f10x.h"

void SendStaticCMD(u8 _cmd,u32 _value);
u32 RecStaticCMD(u8 _cmd);

#endif