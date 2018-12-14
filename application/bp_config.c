#include "bp_config.h"
#include "LedInstruction.h"
#include "MachineKey.h"
#include "PowerlineCarrier.h"
#include "main.h"
#ifndef PRODUCT_TYPE
  #error "Please define PRODUCT_TYPE!"
#endif
void RelaySet(unsigned char ucSelect);
void RelayLoadSet(unsigned char ucSelect);
void tft_init();
//#ifdef PRINTF_SUPPORT
//Ϊ���PRINTF�ض��������
int SendChar (int ch)  {   

  while (!(USART1->SR & USART_FLAG_TXE)); // USART1 �ɻ����������ͨ�ŵĴ���

  USART1->DR = (ch & 0x1FF);

  return (ch);

}

int GetKey (void)  {

  while (!(USART1->SR & USART_FLAG_RXNE));
  return ((int)(USART1->DR & 0x1FF));
}
//#endif 

u8 AdDataBuff1[DataBuff/2] __attribute__((at(0X20002000)));	 //20002000-200023FF
u8 AdDataBuff2[DataBuff/2] __attribute__((at(0X20002400)));	 //20002400-200027Ff

u8 buf_sig;  	//buf_sig=0 ������AdDataBuff1д�� buf_sig=1 AdDataBuff2д��

u8 control_sta;		


extern u32 uip_timer;
INT8U backlight; //��������
u16  AD_Value[ADC_TIMES][ADC_CHANALS] __attribute__((at(0X20002800)));;   //�������ADCת�������Ҳ��DMA��Ŀ���ַ
u16  After_filter[ADC_CHANALS];    		 //���������ƽ��ֵ֮��Ľ��
u8 KeyPressed; //key wakeup
u8 KeyYesPressed; //key BU2
u8 KeyNoPressed;  //key BU1 

//==============================================================================
//Subroutine:	exti_config
//function:     ��������ϵͳ���ж�
//==============================================================================

 void clock_config(void)
 {
	RCC_HSEConfig(RCC_HSE_ON);  //Enable HSE
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
	RCC_PLLCmd(DISABLE);         //�ر�PLL
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9); //ѡ��HSE��ΪPLL�����룬��������PLL�ı���Ϊ9��
	RCC_PLLCmd(ENABLE);         //ʹ��PLL
	 //RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //ѡ��PLL�����Ϊϵͳʱ��
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div2);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | 
				RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO|RCC_APB1Periph_PWR | RCC_APB1Periph_BKP
				 | RCC_APB2Periph_ADC1,ENABLE );
	 
}
/*�����ж�����*/
void exti_config(void)
{
	//GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//����NVIC���ȼ�����ΪGroup2��2λ��ռ���ȼ���0-2����2λ�����ȼ���0-2��

	GPIO_EXTILineConfig(button_port_source,button1_source);	 //ѡ�񰴼�1��GPIO�ܽţ��˿�+�ܽźţ������ж�Դ
	GPIO_EXTILineConfig(button_port_source,button2_source);	 //ѡ�񰴼�2��GPIO�ܽţ��˿�+�ܽźţ������ж�Դ

	EXTI_InitStructure.EXTI_Line = button1_exti_line|button2_exti_line;	//���ð������е��ⲿ��·
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;					//����Ϊ�ⲿ�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	 			//���ô���ģʽΪ�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;							//ʹ�ܸ���·�ж�
	EXTI_Init(&EXTI_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel = button_irqn;				//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;		    //�����ȼ�15
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); 	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	GPIO_EXTILineConfig(button_wakeup_gpio_source, button_wakeup_source);

	EXTI_InitStructure.EXTI_Line = button_wakeup_exti_line ;	//���ð������е��ⲿ��·
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//�����ⲿ�ж�ģʽ:EXTI��·Ϊ�ж�����
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//ʹ���ⲿ�ж���״̬
	EXTI_Init(&EXTI_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel = button_wakeup_irqn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;			//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);					//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

//
void led_init(void)
{   GPIO_InitTypeDef     GPIO_InitStructure;
	//����PB12(led1)��PB13(led2)��PB14(led3)Ϊ���
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void key_init(void)
{   GPIO_InitTypeDef     GPIO_InitStructure;
	//����PB3(KEY1)��PB4(KEY2)��PB5(KEY3)��PB8(KEY4)Ϊ���
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//��PB3����Ϊ��ͨIO��Ĭ��ΪJTAG��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB , ENABLE); 
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
}
void Beep_init(void)
{   GPIO_InitTypeDef     GPIO_InitStructure;
	//����PC8����Ϊ���
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void RelayInit(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	//����PA11��PA12��PA15Ϊ���
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	RelaySet(2);  //Ĭ��Ϊ220V
	//���ø��ؼ̵�����PE0��PE1��PE2Ϊ���
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);
	RelayLoadSet(2); //Ĭ��Ϊ2·����
#if PRODUCT_TYPE == 2
	NEMA_OFF;
    NEMA_RS485;
#else
	NEMA_ON;
#endif
}
void TimerInit(void)
{
	TIM_TimeBaseInitTypeDef  TIM_Struct;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
#if PRODUCT_TYPE == 2
	GPIO_InitTypeDef     GPIO_InitStructure;
	TIM_OCInitTypeDef    TIM_OCInitStruct;
#endif
  //ʹ��TIM4ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  //���ö�ʱ��Ϊȱʡֵ
	TIM_DeInit(TIM4);
  // ��ʱ����ʼ��
    TIM_Struct.TIM_Period = 65530;    //���ڣ�65530 * 1us = 65.53ms
	TIM_Struct.TIM_Prescaler = 71;    //72��Ƶ��ʱ��Ƶ������Ϊ��1MHz
	TIM_Struct.TIM_ClockDivision = 0; //TIM2_CKD_DIV1; //
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
	//TIM_Struct.TIM_RepetitionCounter = 0;	 //???   STM32F10XXXû���õ���λ
	TIM_TimeBaseInit(TIM4,&TIM_Struct);
	
	//����CH1Ϊ���벶׽        
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;                  //ѡ��ͨ��1
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;       //���������ز���  
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;   //CH1���ӵ�TI1 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;             //ÿ�μ�⵽��������ʹ���һ�β���
	TIM_ICInitStructure.TIM_ICFilter = 0x0;                           //
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	//����CH2Ϊ���벶׽        
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;                  //ѡ��ͨ��2
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;      //���������ز���  
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI; //CH1���ӵ�TI2 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;             //ÿ�μ�⵽��������ʹ���һ�β���
	TIM_ICInitStructure.TIM_ICFilter = 0x0;                           //
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	TIM_CCxCmd(TIM4, TIM_Channel_1, TIM_CCx_Enable);
	TIM_CCxCmd(TIM4, TIM_Channel_2, TIM_CCx_Enable);
  //�ж�����
    TIM_ClearFlag(TIM4, TIM_FLAG_CC1);	     //����жϱ�־λ
	TIM_ClearFlag(TIM4, TIM_FLAG_CC2);	     //����жϱ�־λ
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);	 //����жϱ�־λ
    TIM_ITConfig(TIM4,TIM_IT_CC1,ENABLE);    //ʹ���ж�
	TIM_ITConfig(TIM4,TIM_IT_CC2,ENABLE);    //ʹ���ж�
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //ʹ���ж�
  //�ж�����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;		    //
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	
  //���ö�ʱ��4ʹ���ڲ�ʱ��
    TIM_InternalClockConfig(TIM4);
  //ʹ�ܸ����¼�
    TIM_UpdateDisableConfig(TIM4, DISABLE);	 //ע��
  //ʹ�ܵ���ģʽ
	TIM_SelectOnePulseMode(TIM4, TIM_OPMode_Single);
  //ʹ�ܶ�ʱ��
    TIM_Cmd(TIM4, ENABLE);
	
	//ʹ��TIM6ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  //���ö�ʱ��Ϊȱʡֵ
	TIM_DeInit(TIM6);
  // ��ʱ����ʼ��
    TIM_Struct.TIM_Period = 10000;    //���ڣ�10000 * 1us = 10ms
	TIM_Struct.TIM_Prescaler = 35;    //36��Ƶ��ʱ��Ƶ������Ϊ��1MHz
	TIM_Struct.TIM_ClockDivision = 0; //TIM2_CKD_DIV1; //
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
	//TIM_Struct.TIM_RepetitionCounter = 0;	 //???   STM32F10XXXû���õ���λ
	TIM_TimeBaseInit(TIM6,&TIM_Struct);

  //�ж�����
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);	 //����жϱ�־λ
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); //ʹ���ж�
  //�ж�����
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	
  //���ö�ʱ��6ʹ���ڲ�ʱ��
    //TIM_InternalClockConfig(TIM6);
  //ʹ�ܸ����¼�
    TIM_UpdateDisableConfig(TIM6, DISABLE);	 //ע��
  //ʹ�ܵ���ģʽ
	TIM_SelectOnePulseMode(TIM6, TIM_OPMode_Single);
  //ʹ�ܶ�ʱ��
    //TIM_Cmd(TIM6, ENABLE);
#if PRODUCT_TYPE == 2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOE , ENABLE); 
   //PE5����ΪTIM9_CH1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_PinRemapConfig(GPIO_Remap_TIM9,ENABLE);
   /* TIM9_CH1���������PWM */
    //ʹ��TIM9ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
  //���ö�ʱ��Ϊȱʡֵ
	TIM_DeInit(PWM_TIMER);
  //TIM9_CH1����
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OC1Init(PWM_TIMER, &TIM_OCInitStruct);
  // ��ʱ����ʼ��
    TIM_Struct.TIM_Period = PWM_T;   //���ڣ�360000 * 1/36us = 1ms
	TIM_Struct.TIM_Prescaler = 0;    //����Ƶ��ʱ��Ƶ������Ϊ��36MHz
	TIM_Struct.TIM_ClockDivision = 0; //TIM2_CKD_DIV1; //
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
   //TIM_Struct.TIM_RepetitionCounter = 0;	 //???   STM32F10XXXû���õ���λ
	TIM_TimeBaseInit(PWM_TIMER,&TIM_Struct);
	TIM_Cmd(PWM_TIMER, ENABLE);
#endif
}
//
void HardwareInit(void)
{
	clock_config();	 //ʱ�ӳ�ʼ��
	led_init();
	key_init();
	Beep_init();
	//exti_config();
	usart_init();
	//exti_config();
	//iic_init();
	//spiflash_init();
	tft_init(); 
	RelayInit();
//LcdClear(WHITE);
	//LedInstrctInit();
	//MachKey_Init();
	RS485_Init();
	TimerInit();
	//RelayInit();
} 	

void bp_selfcheck(u8 type)
{
  
}
