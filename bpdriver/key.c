
#include <key.h>

void key_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//���ð����ź�Ϊ���룬����WAKEUP�õİ���Ϊ�����������Ч�������������������������Ч
 	GPIO_InitStructure.GPIO_Pin = button1 | button2 ;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(button_gpio, &GPIO_InitStructure);

 	GPIO_InitStructure.GPIO_Pin = button_wakeup ;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(button_wakeup_gpio, &GPIO_InitStructure);
}


