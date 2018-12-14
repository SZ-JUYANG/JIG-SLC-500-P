
#include <key.h>

void key_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//设置按键信号为输入，其中WAKEUP用的按键为下拉，因高有效；其他两个按键上拉，因低有效
 	GPIO_InitStructure.GPIO_Pin = button1 | button2 ;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(button_gpio, &GPIO_InitStructure);

 	GPIO_InitStructure.GPIO_Pin = button_wakeup ;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(button_wakeup_gpio, &GPIO_InitStructure);
}


