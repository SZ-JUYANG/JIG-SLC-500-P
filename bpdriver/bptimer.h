#ifndef __BPTIMER_H
#define __BPTIMER_H

#include <include.h>
#include <bp_config.h>

void tim2_Configuration(void);
void tim3_Configuration(void);
void TIM1_Int_Init(uint32_t freq);  //freqµ¥Î»Hz
void TIM6_Int_Init(u16 arr,u16 psc);

#endif 
