


#ifndef __UTILITY_H
#define __UTILITY_H

#ifndef  OS_CPU_H
/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (数据类型定义与UCOS兼容)
*********************************************************************************************************
*/

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef unsigned int   OS_STK;                   /* Each stack entry is 32-bit wide                    */
typedef unsigned int   OS_CPU_SR;                /* Define size of CPU status register (PSR = 32 bits) */
#endif 


/* 延时程序，纯软延时，不使用中断 -------------------------------------*/ 
 void delay(unsigned int nCount); 
 void delay_1us(void);
 void delay_us(unsigned int Counter);
 void delay_100us(unsigned int Counter);
 void delay_ms(unsigned int Counter);
 void delay_10ms(unsigned int Counter);
 void delay_100ms(unsigned int Counter);
/* 算术程序	  */
 unsigned int pow(unsigned char m,unsigned char n);

/* 算术程序	  */
#endif /*  UTILITY_H */
