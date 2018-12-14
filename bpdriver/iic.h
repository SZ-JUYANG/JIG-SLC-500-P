#ifndef __BPIIC_H
#define __BPIIC_H

#include <include.h>
#include <bp_config.h>
//==============================================================================
//macro: iic_sdain iic_sdaout	  
//设置sda为输入或输出
//方法： 设置CRL 高4位为 1000 位7 sda 上下拉输入  
//		 设置CRL 高4位为 0011 位7 sda 推挽输出 最高频率50M 
//==============================================================================
#define iic_sdain()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0X80000000;}
#define iic_sdaout() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0X30000000;}
#define IICWAITTIMEOUT 250

void iic_init(void);
void iic_start(void);
void iic_stop(void);
INT8U iic_wait4ack(void);
void iic_ack(void);	    
void iic_noack(void);
void iic_sendbyte(INT8U);
INT8U iic_readbyte(unsigned char ack);
INT8U at24c02_readbyte(INT16U addr);
void at24c02_writebyte(INT16U addr,INT8U data);
INT8U at24c02_check(void);
void at24c02_read(INT16U addr,INT8U * buf,INT16U len);
void at24c02_write(INT16U addr,INT8U *buf,INT16U len);




#endif 
