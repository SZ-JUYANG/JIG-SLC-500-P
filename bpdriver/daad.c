
#include "daad.h"

extern u16  AD_Value[ADC_TIMES][ADC_CHANALS];   //�������ADCת�������Ҳ��DMA��Ŀ���ַ
extern u16  After_filter[ADC_CHANALS];    		 //���������ƽ��ֵ֮��Ľ��


GPIO_InitTypeDef     GPIO_InitStructure;

 void BP_DAC_Init(void)
 {
	  	DAC_InitTypeDef DAC_InitStructure;
 		DAC_DeInit();	  //�ָ�DAC�Ĵ�������ʼֵ

		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5; //DACOUT1ΪPA4DAC OUT2ΪPA5
  		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  		GPIO_Init(GPIOA, &GPIO_InitStructure); 

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//ʹ��DACʱ�� 


		DAC_Cmd(DAC_Channel_1,ENABLE); //ʹ��DACͨ��1
		DAC_Cmd(DAC_Channel_2, ENABLE);//ʹ��DACͨ��2

	    /* ����DACͨ�� */ 
  		DAC_InitStructure.DAC_Trigger = DAC_Trigger_None; //���ô�����ʽ
  		DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;	//���ò��η�����
		DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude=DAC_TriangleAmplitude_4095;//���ǲ����������������Ϊ4095
  		DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable; //���������
  		DAC_Init(DAC_Channel_1, &DAC_InitStructure); 	//��ʼ��DACͨ��1
		DAC_Init(DAC_Channel_2, &DAC_InitStructure); 	//��ʼ��DACͨ��2
 }


  void BP_DAC1_InitTriangle(void)  //��DAC1����Ϊ���ǲ���������ʹ��TIM2��ʱ��Դ
 {
	  	DAC_InitTypeDef DAC_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 		DAC_DeInit();	  //�ָ�DAC�Ĵ�������ʼֵ

		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 ; //DACOUT1ΪPA4
  		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  		GPIO_Init(GPIOA, &GPIO_InitStructure); 

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//ʹ��DACʱ�� 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //TIM2ʱ��ʹ��
		//���ö�ʱ��2Ϊ���ǲ��ṩԴ
		TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
		TIM_TimeBaseStructure.TIM_Period=71; //   ����ֵ71+1=72
		TIM_TimeBaseStructure.TIM_Prescaler=4; // 4+1=5��Ƶ��Ƶ��72/5
		//��ʱ��2���Ƶ��Ϊ5M
		TIM_TimeBaseStructure.TIM_ClockDivision=0x0;
		TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
		TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_Update);//ѡ��ʱ��2Ϊ�������ģʽ
		
		DAC_InitStructure.DAC_Trigger=DAC_Trigger_T2_TRGO; //����DAC1����ԴΪ��ʱ��2
		DAC_InitStructure.DAC_WaveGeneration=DAC_WaveGeneration_Triangle; //���ò��η�����Ϊ���ǲ�������
		DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude=DAC_TriangleAmplitude_2047; //������2047
		DAC_InitStructure.DAC_OutputBuffer=DAC_OutputBuffer_Enable;//�������ʹ��;
		DAC_Init(DAC_Channel_1, &DAC_InitStructure); //����DAC_InitStructure����������DAC1
		DAC_Cmd(DAC_Channel_1,ENABLE); //ʹ��DACͨ��1
		DAC_SetChannel1Data(DAC_Align_12b_R,0);
		TIM_Cmd(TIM2,ENABLE);//ʹ�ܶ�ʱ����������������ǲ�
 }
 /*����DMA*/
void BP_ADC_DMA_Init(void)
{
	/* ADC1  DMA1 Channel Config */
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//ʹ��DMA����

	DMA_DeInit(DMA1_Channel1);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)&ADC1->DR;  //DMA����ADC����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value;  //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //�ڴ���Ϊ���ݴ����Ŀ�ĵ�
	DMA_InitStructure.DMA_BufferSize = ADC_TIMES*ADC_CHANALS;  //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //������ѭ������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�и����ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��
}

void BP_ADC_INIT(void)
{
	ADC_InitTypeDef ADC_InitStructure;  

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��

	/* Configure ADCCLK such as ADCCLK = PCLK2/6 */ 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PC   0/ 1/ 2/ 3/ 4/ 5  pb0 1 ��Ϊģ��ͨ���������� 
	//adc  10 11 12 13 14 15  8  9      
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* Resets ADC1 */ 
	ADC_DeInit(ADC1);  //������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
	
	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; 		//ENABLE	ģ��ת��������ɨ��ģʽ
												 		//DISABLE 	ģ��ת�������ڵ���ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//ENABLE	ģ��ת������������ת��ģʽ
														//DISABLE	ģ��ת�������ڵ���ת��ģʽENABLE
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = ADC_CHANALS;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���
	
	/* ADC1 regular channel11 configuration */ 
	//����ָ��ADC�Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
	//ADC1,ADCͨ��x,�������˳��ֵΪy,����ʱ��Ϊ239.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 3, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 4, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 5, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 6, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 7, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 8, ADC_SampleTime_239Cycles5 );
	
	 // ����ADC��DMA֧�֣�Ҫʵ��DMA���ܣ������������DMAͨ���Ȳ�����
    ADC_DMACmd(ADC1, ENABLE);	
	
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	
	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);	//����ָ����ADC1��У׼�Ĵ���
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));	//��ȡADC1����У׼�Ĵ�����״̬,����״̬��ȴ�
	
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);		//��ʼУ׼
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));		//�ȴ�У׼���
	
	/* Start ADC1 Software Conversion */ 
	//ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������
}

//==============================================================================
//Adc15DmaConfig:ADCͨ��ת����DMA���� 
//DMA_CHx��DMAͨ��
//cpar��������Դ��ADC��DR�Ĵ���
//cmar: ����ȥ���ڴ��ַ
//cndtr: ���� 
//����ֵ  :��   
//==============================================================================
void Adc15DmaConfig(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{

	DMA_InitTypeDef DMA_InitStructure;
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1|RCC_AHBPeriph_DMA2, ENABLE);	//ʹ��DMA����	   |RCC_AHBPeriph_DMA2
	
    DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;                     //DMA����ADC����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;                         //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                   //�ڴ���Ϊ���ݴ����Ŀ�ĵ�
	DMA_InitStructure.DMA_BufferSize = cndtr;                                    //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;             //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  					 //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //�������ݿ��Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;          //�ڴ����ݿ��Ϊ16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  		//��������������ģʽ	
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //DMAͨ�������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���	  	
}		
 u16 Get_Adc(u8 ch)  //����ģʽ 
	{

	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��3,�������˳��ֵΪ1,����ʱ��Ϊ239.5����	  			    

	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
	}

float AD_GetVolt(u16 advalue)   

{
   
   return (float)(advalue * 3.30 / 4095); 

}
 void AD_filter(void)
{
  	int  sum = 0;
	u8  count,i;    
  	for(i=0;i<ADC_CHANALS;i++)

      {

         for ( count=0;count<ADC_TIMES;count++)

          {

           		sum += AD_Value[count][i];

          }

          After_filter[i]=(u16)sum/ADC_TIMES;

          sum=0;
      }

 }



