


#ifndef _LEDINSTRUCTION_H
#define _LEDINSTRUCTION_H

#include <stm32f10x.h>

#define LEDINSTCT_LED1 0x01<<0
#define LEDINSTCT_LED2 0x01<<1
#define LEDINSTCT_LED3 0x01<<2
#define LEDINSTCT_LED4 0x01<<3
#define LEDINSTCT_LED5 0x01<<4



void LedInstrctInit();
void LedInstrctSet(u8 _setdata);
void LedInstrctReset(u8 _setdata);








#endif




