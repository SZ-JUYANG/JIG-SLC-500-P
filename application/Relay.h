

#ifndef _RELAY_H
#define _RELAY_H

#include <stm32f10x.h>



#define RELAY1   0x01<<2
#define RELAY2   0x01<<3
#define RELAY3   0x01<<0
#define RELAY4   0x01<<4
#define RELAY5   0x01<<5
#define RELAYMOC 0x01<<1




void Relay_Set(u8 _setdata);
void Relay_Reset(u8 _setdata);








#endif








