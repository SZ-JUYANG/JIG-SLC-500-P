
#include "bptimer.h"
void tim2_Configuration(void)
	{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //TIM2ʱ��ʹ��
  
	TIM_DeInit(TIM2);


	/* ��ʱ���ж�ʱ������ */
	TIM_TimeBaseStructure.TIM_Period = 36000-1; //װ�ص���װ�ؼ�������ֵ�����ü������ڣ�0-0XFFFF	��������72��
	TIM_TimeBaseStructure.TIM_Prescaler =1;     //����������TIMxʱ�ӽ���Ԥ��Ƶֵ  0-0XFFFF,����Ƶ��36M  
	//ÿ���ж�����Ϊ 36000/36M=1���� 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  //TIM���¼���ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; //ֻ�Ը߼���ʱ����Ч


	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx

	/* ʹ��TIM2�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��

	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);  //���жϱ�־

	/* �ж�ʹ�� */
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM2, //TIM2
		TIM_IT_Update, // |  //TIM �ж�ԴTIM_IT_Trigger,   //TIM �����ж�Դ 
		ENABLE  //ʹ��
		);
	
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx����
}
//timer3
void tim3_Configuration(void)
	{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //TIM2ʱ��ʹ��
  
	TIM_DeInit(TIM3);

	/* ��ʱ���ж�ʱ������ */
	TIM_TimeBaseStructure.TIM_Period = 36000-1;      
	TIM_TimeBaseStructure.TIM_Prescaler =1;     
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  //TIM���¼���ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; //ֻ�Ը߼���ʱ����Ч
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx

	/* ʹ��TIM2�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  			  //TIM3�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;         //�����ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		      //IRQͨ����ʹ��

	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);  //���жϱ�־

	/* ʹ���ж�*/
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, 
		TIM_IT_Update,
		ENABLE 
		);
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
	}
//ͨ�ö�ʱ��1��ʼ��,���ö�ʱ��1��3ͨ��CC3��ΪADC��ת������Դ
void TIM1_Int_Init(uint32_t freq)  //freq��λHz
{	
 	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	TIM_OCInitTypeDef  TIM_OCInitStructure;
 	uint16_t Period;
 	TIM_Cmd(TIM1, DISABLE);
	/*
	����Ƶ�ʼ��㹫ʽ ��
	period = 72000000 / freq ;  
	1200  ==  60KHz ����Ƶ��
	*/
	Period = 72000000 / freq;  
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 	//ʱ��ʹ��
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);	   		//��ADC_InitStruct�е�ÿһ��������ȱʡֵ����
 	TIM_TimeBaseStructure.TIM_Period = Period;         
	TIM_TimeBaseStructure.TIM_Prescaler = 0;  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;   			  //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);				  //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	//����TIM1 OC3 ΪPWMģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;			  //PWM1ģʽ
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;               
	TIM_OCInitStructure.TIM_Pulse = TIM_TimeBaseStructure.TIM_Period / 2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        
 
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_Cmd(TIM1, ENABLE);//ʹ��TIM1
	TIM_CtrlPWMOutputs(TIM1, ENABLE);//TIM1 main Output Enable
							 
}

//������ʱ��6�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
void TIM6_Int_Init(u16 arr,u16 psc)
{	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode =  TIM_CounterMode_Down;//���¼���ģʽ
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig( TIM6,TIM_IT_Update|TIM_IT_Trigger,ENABLE);//ʹ�ܶ�ʱ��6���´����ж�
 
	TIM_Cmd(TIM6, ENABLE);  //ʹ��TIMx����
 	
  	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM6�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 								 
}


