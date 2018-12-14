
#include "bptimer.h"
void tim2_Configuration(void)
	{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //TIM2时钟使能
  
	TIM_DeInit(TIM2);


	/* 定时器中断时间设置 */
	TIM_TimeBaseStructure.TIM_Period = 36000-1; //装载到重装载计数器的值，设置计数周期，0-0XFFFF	计数周期72个
	TIM_TimeBaseStructure.TIM_Prescaler =1;     //设置用来对TIMx时钟进行预分频值  0-0XFFFF,计数频率36M  
	//每次中断周期为 36000/36M=1毫秒 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  //TIM向下计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; //只对高级定时器有效


	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx

	/* 使能TIM2中断 */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能

	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);  //清中断标志

	/* 中断使能 */
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM2, //TIM2
		TIM_IT_Update, // |  //TIM 中断源TIM_IT_Trigger,   //TIM 触发中断源 
		ENABLE  //使能
		);
	
	TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设
}
//timer3
void tim3_Configuration(void)
	{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //TIM2时钟使能
  
	TIM_DeInit(TIM3);

	/* 定时器中断时间设置 */
	TIM_TimeBaseStructure.TIM_Period = 36000-1;      
	TIM_TimeBaseStructure.TIM_Prescaler =1;     
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  //TIM向下计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; //只对高级定时器有效
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx

	/* 使能TIM2中断 */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  			  //TIM3中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;         //从优先级2级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		      //IRQ通道被使能

	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);  //清中断标志

	/* 使能中断*/
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM3, 
		TIM_IT_Update,
		ENABLE 
		);
	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
	}
//通用定时器1初始化,配置定时器1第3通道CC3作为ADC的转换触发源
void TIM1_Int_Init(uint32_t freq)  //freq单位Hz
{	
 	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	TIM_OCInitTypeDef  TIM_OCInitStructure;
 	uint16_t Period;
 	TIM_Cmd(TIM1, DISABLE);
	/*
	采样频率计算公式 ：
	period = 72000000 / freq ;  
	1200  ==  60KHz 采样频率
	*/
	Period = 72000000 / freq;  
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 	//时钟使能
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);	   		//把ADC_InitStruct中的每一个参数按缺省值填入
 	TIM_TimeBaseStructure.TIM_Period = Period;         
	TIM_TimeBaseStructure.TIM_Prescaler = 0;  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;   			  //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);				  //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	//配置TIM1 OC3 为PWM模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;			  //PWM1模式
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;               
	TIM_OCInitStructure.TIM_Pulse = TIM_TimeBaseStructure.TIM_Period / 2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        
 
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_Cmd(TIM1, ENABLE);//使能TIM1
	TIM_CtrlPWMOutputs(TIM1, ENABLE);//TIM1 main Output Enable
							 
}

//基本定时器6中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
void TIM6_Int_Init(u16 arr,u16 psc)
{	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode =  TIM_CounterMode_Down;//向下计数模式
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig( TIM6,TIM_IT_Update|TIM_IT_Trigger,ENABLE);//使能定时器6更新触发中断
 
	TIM_Cmd(TIM6, ENABLE);  //使能TIMx外设
 	
  	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 								 
}


