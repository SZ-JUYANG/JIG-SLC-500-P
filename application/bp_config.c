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
//为解决PRINTF重定向的问题
int SendChar (int ch)  {   

  while (!(USART1->SR & USART_FLAG_TXE)); // USART1 可换成你程序中通信的串口

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

u8 buf_sig;  	//buf_sig=0 表明是AdDataBuff1写满 buf_sig=1 AdDataBuff2写满

u8 control_sta;		


extern u32 uip_timer;
INT8U backlight; //背光亮度
u16  AD_Value[ADC_TIMES][ADC_CHANALS] __attribute__((at(0X20002800)));;   //用来存放ADC转换结果，也是DMA的目标地址
u16  After_filter[ADC_CHANALS];    		 //用来存放求平均值之后的结果
u8 KeyPressed; //key wakeup
u8 KeyYesPressed; //key BU2
u8 KeyNoPressed;  //key BU1 

//==============================================================================
//Subroutine:	exti_config
//function:     配置整个系统的中断
//==============================================================================

 void clock_config(void)
 {
	RCC_HSEConfig(RCC_HSE_ON);  //Enable HSE
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
	RCC_PLLCmd(DISABLE);         //关闭PLL
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9); //选择HSE作为PLL的输入，并且设置PLL的倍率为9倍
	RCC_PLLCmd(ENABLE);         //使能PLL
	 //RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //选择PLL输出作为系统时钟
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div2);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | 
				RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO|RCC_APB1Periph_PWR | RCC_APB1Periph_BKP
				 | RCC_APB2Periph_ADC1,ENABLE );
	 
}
/*配置中断引脚*/
void exti_config(void)
{
	//GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//设置NVIC优先级分组为Group2：2位先占优先级（0-2），2位子优先级（0-2）

	GPIO_EXTILineConfig(button_port_source,button1_source);	 //选择按键1的GPIO管脚（端口+管脚号）做外中断源
	GPIO_EXTILineConfig(button_port_source,button2_source);	 //选择按键2的GPIO管脚（端口+管脚号）做外中断源

	EXTI_InitStructure.EXTI_Line = button1_exti_line|button2_exti_line;	//设置按键所有的外部线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;					//设置为外部中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	 			//设置触发模式为下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;							//使能该线路中断
	EXTI_Init(&EXTI_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel = button_irqn;				//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;		    //从优先级15
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	GPIO_EXTILineConfig(button_wakeup_gpio_source, button_wakeup_source);

	EXTI_InitStructure.EXTI_Line = button_wakeup_exti_line ;	//设置按键所有的外部线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//设外外部中断模式:EXTI线路为中断请求
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//使能外部中断新状态
	EXTI_Init(&EXTI_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel = button_wakeup_irqn;			//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;			//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//从优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);					//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

//
void led_init(void)
{   GPIO_InitTypeDef     GPIO_InitStructure;
	//设置PB12(led1)、PB13(led2)、PB14(led3)为输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void key_init(void)
{   GPIO_InitTypeDef     GPIO_InitStructure;
	//设置PB3(KEY1)、PB4(KEY2)、PB5(KEY3)、PB8(KEY4)为输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//将PB3配置为普通IO（默认为JTAG）
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB , ENABLE); 
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
}
void Beep_init(void)
{   GPIO_InitTypeDef     GPIO_InitStructure;
	//设置PC8设置为输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void RelayInit(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	//设置PA11、PA12、PA15为输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	RelaySet(2);  //默认为220V
	//配置负载继电器，PE0、PE1、PE2为输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);
	RelayLoadSet(2); //默认为2路负载
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
  //使能TIM4时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  //设置定时器为缺省值
	TIM_DeInit(TIM4);
  // 定时器初始化
    TIM_Struct.TIM_Period = 65530;    //周期：65530 * 1us = 65.53ms
	TIM_Struct.TIM_Prescaler = 71;    //72分频，时钟频率设置为：1MHz
	TIM_Struct.TIM_ClockDivision = 0; //TIM2_CKD_DIV1; //
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
	//TIM_Struct.TIM_RepetitionCounter = 0;	 //???   STM32F10XXX没有用到该位
	TIM_TimeBaseInit(TIM4,&TIM_Struct);
	
	//配置CH1为输入捕捉        
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;                  //选择通道1
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;       //输入上升沿捕获  
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;   //CH1连接到TI1 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;             //每次检测到捕获输入就触发一次捕获
	TIM_ICInitStructure.TIM_ICFilter = 0x0;                           //
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	//配置CH2为输入捕捉        
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;                  //选择通道2
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;      //输入下升沿捕获  
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI; //CH1连接到TI2 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;             //每次检测到捕获输入就触发一次捕获
	TIM_ICInitStructure.TIM_ICFilter = 0x0;                           //
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	TIM_CCxCmd(TIM4, TIM_Channel_1, TIM_CCx_Enable);
	TIM_CCxCmd(TIM4, TIM_Channel_2, TIM_CCx_Enable);
  //中断设置
    TIM_ClearFlag(TIM4, TIM_FLAG_CC1);	     //清除中断标志位
	TIM_ClearFlag(TIM4, TIM_FLAG_CC2);	     //清除中断标志位
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);	 //清除中断标志位
    TIM_ITConfig(TIM4,TIM_IT_CC1,ENABLE);    //使能中断
	TIM_ITConfig(TIM4,TIM_IT_CC2,ENABLE);    //使能中断
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //使能中断
  //中断配置
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;		    //
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	
  //设置定时器4使用内部时钟
    TIM_InternalClockConfig(TIM4);
  //使能更新事件
    TIM_UpdateDisableConfig(TIM4, DISABLE);	 //注意
  //使能单次模式
	TIM_SelectOnePulseMode(TIM4, TIM_OPMode_Single);
  //使能定时器
    TIM_Cmd(TIM4, ENABLE);
	
	//使能TIM6时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  //设置定时器为缺省值
	TIM_DeInit(TIM6);
  // 定时器初始化
    TIM_Struct.TIM_Period = 10000;    //周期：10000 * 1us = 10ms
	TIM_Struct.TIM_Prescaler = 35;    //36分频，时钟频率设置为：1MHz
	TIM_Struct.TIM_ClockDivision = 0; //TIM2_CKD_DIV1; //
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
	//TIM_Struct.TIM_RepetitionCounter = 0;	 //???   STM32F10XXX没有用到该位
	TIM_TimeBaseInit(TIM6,&TIM_Struct);

  //中断设置
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);	 //清除中断标志位
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); //使能中断
  //中断配置
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	
  //设置定时器6使用内部时钟
    //TIM_InternalClockConfig(TIM6);
  //使能更新事件
    TIM_UpdateDisableConfig(TIM6, DISABLE);	 //注意
  //使能单次模式
	TIM_SelectOnePulseMode(TIM6, TIM_OPMode_Single);
  //使能定时器
    //TIM_Cmd(TIM6, ENABLE);
#if PRODUCT_TYPE == 2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOE , ENABLE); 
   //PE5配置为TIM9_CH1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_PinRemapConfig(GPIO_Remap_TIM9,ENABLE);
   /* TIM9_CH1产生调光的PWM */
    //使能TIM9时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
  //设置定时器为缺省值
	TIM_DeInit(PWM_TIMER);
  //TIM9_CH1配置
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OC1Init(PWM_TIMER, &TIM_OCInitStruct);
  // 定时器初始化
    TIM_Struct.TIM_Period = PWM_T;   //周期：360000 * 1/36us = 1ms
	TIM_Struct.TIM_Prescaler = 0;    //不分频，时钟频率设置为：36MHz
	TIM_Struct.TIM_ClockDivision = 0; //TIM2_CKD_DIV1; //
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
   //TIM_Struct.TIM_RepetitionCounter = 0;	 //???   STM32F10XXX没有用到该位
	TIM_TimeBaseInit(PWM_TIMER,&TIM_Struct);
	TIM_Cmd(PWM_TIMER, ENABLE);
#endif
}
//
void HardwareInit(void)
{
	clock_config();	 //时钟初始化
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
