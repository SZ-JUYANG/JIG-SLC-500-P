
#include <led.h>
extern u8 KeyPressed;
void led_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//开启gpio c 时钟信号
	//RCC_APB2PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP |RCC_APB2Periph_GPIOC,ENABLE );	
							 			  
	//设计全部4个 LED指示灯信号为输出
	GPIO_InitStructure.GPIO_Pin = led1 | led2 | led3 | led4 ;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(led_gpio, &GPIO_InitStructure);
    
	//初始化蜂鸣器GPIO
	GPIO_InitStructure.GPIO_Pin = beep ;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(beep_gpio, &GPIO_InitStructure);
}

void beep_run(u16 runtime)
{
	GPIO_SetBits(beep_gpio,beep);
	delay_ms(runtime);
 	GPIO_ResetBits(beep_gpio,beep);
}
void led_beep_test()
{
		while(KeyPressed==0)
		{
			led_turn1;
			led_turn2;
			led_turn3;
			led_turn4;
			beep_run(500);
			delay_ms(500);			
		}
		KeyPressed=0;	
}
