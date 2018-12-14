#ifndef __BPUSART_H
#define __BPUSART_H

#include <include.h>
#include <bp_config.h>

void usart_init(void);
void uart1_init(INT32U);
void uart2_init(INT32U);
void uart4_init(INT32U);
void usart_sendstring(USART_TypeDef *, char *);
#endif /* __BPUSART_H */
