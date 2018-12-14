#ifndef __BPDAAD_H
#define __BPDAAC_H

#include <include.h>
#include <bp_config.h>

 void BP_DAC_Init(void);
 void BP_ADC_DMA_Init(void);
 void BP_ADC_INIT(void);
 void Adc15_Init(void);
 void Adc15DmaConfig(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
 u16 Get_Adc(u8 ch);  //单次模式 
 float AD_GetVolt(u16 advalue); 
  void AD_filter(void);
   void BP_DAC1_InitTriangle(void);
#endif /* __BPLED_H */
