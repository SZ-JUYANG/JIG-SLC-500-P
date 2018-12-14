
#include "utility.h"



void delay_1us(void)
{
 __nop(); 	 //72M时钟频率，那么一个周期是 1/72微秒
 __nop();
 __nop();
 __nop();
 __nop();
 __nop(); 
 __nop();
 __nop();

}

//==============================================================================
//Subroutine:	delay_1us
//==============================================================================
void delay_us(unsigned int Counter)
{
	while(Counter--)
	{
		delay_1us();
	}

}

//==============================================================================
//Subroutine:	Delay100us
//==============================================================================
void Delay_100us(unsigned int Counter)
{
	while(Counter--)
	{	
		delay_us(72);
	}
}

//==============================================================================
//Subroutine:	Delay1ms
//==============================================================================
void delay_ms(unsigned int Counter)
{

		unsigned int i;
		
		for(; Counter !=0; Counter--)
		{
			i = 10301;
			while(i--);
		}
	
}

//==============================================================================
//Subroutine:	Delay10ms
//==============================================================================
void delay_10ms(unsigned int Counter)
{
	while(Counter--)
	{	
		delay_ms(10);			
	}
}  

//==============================================================================
//Subroutine:	Delay100ms
//==============================================================================
void delay_100ms(unsigned int Counter)
{
	while(Counter--)
	{	delay_ms(100);			
	}
}

void delay(unsigned int nCount)
{
	for(; nCount != 0; nCount--);
}

unsigned int pow(unsigned char m,unsigned char n)
{
	unsigned char result=1;	 
	while(n--)result*=m;    
	return result;
}
