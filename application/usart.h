#ifndef __BPUSART_H
#define __BPUSART_H

#include "stm32f10x.h"


void usart_init(void);
void uart1_init(u32);
void uart2_init(u32);
void uart4_init(u32);
void usart_sendstring(USART_TypeDef *, char *);
#endif /* __BPUSART_H */
