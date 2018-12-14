

#include "ADC.h"





#define  ADC_TIMES   30	     			//每通道采10次
#define  ADC_CHANALS  2	     			//为8个通道
//extern u16  AD_Value[ADC_TIMES][ADC_CHANALS];   //用来存放ADC转换结果，也是DMA的目标地址
//extern u16  After_filter[ADC_CHANALS];    		 //用来存放求平均值之后的结果




static u16  AD_Value[ADC_TIMES][ADC_CHANALS];   //用来存放ADC转换结果，也是DMA的目标地址


 /*配置DMA*/
static void ADC_DMA_Init(void)
{
	/* ADC1  DMA1 Channel Config */
    DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//使能DMA传输

	DMA_DeInit(DMA1_Channel1);   //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)&ADC1->DR;  //DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //内存作为数据传输的目的地
	DMA_InitStructure.DMA_BufferSize = ADC_TIMES*ADC_CHANALS;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //数据宽度为16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //工作在循环缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道
}

static void ADC__INIT(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	
	ADC_InitTypeDef ADC_InitStructure;  


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟


	/* Configure ADCCLK such as ADCCLK = PCLK2/6 */ 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //72M/6=12,ADC最大时间不能超过14M

	//PC   0/ 1/ 2/ 3/ 4/ 5  pb0 1 作为模拟通道输入引脚 
	//adc  10 11 12 13 14 15  8  9      
	/****
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	*****/

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* Resets ADC1 */ 
	ADC_DeInit(ADC1);  //将外设 ADC1 的全部寄存器重设为缺省值
	
	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; 		//ENABLE	模数转换工作在扫描模式
												 		//DISABLE 	模数转换工作在单次模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//ENABLE	模数转换工作在连续转换模式
														//DISABLE	模数转换工作在单次转换模式ENABLE
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;//ADC_CHANALS;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器
	
	/* ADC1 regular channel11 configuration */ 
	//设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
	//ADC1,ADC通道x,规则采样顺序值为y,采样时间为239.5周期
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5 );
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_239Cycles5 );
	/***
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 3, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 4, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 5, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 6, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 7, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 8, ADC_SampleTime_239Cycles5 );
	***/
	 // 开启ADC的DMA支持（要实现DMA功能，还需独立配置DMA通道等参数）
    ADC_DMACmd(ADC1, ENABLE);	
	
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	
	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);	//重置指定的ADC1的校准寄存器
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));	//获取ADC1重置校准寄存器的状态,设置状态则等待
	
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);		//开始指定ADC1的校准状态
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));		//获取指定ADC1的校准程序,设置状态则等待
	
	/* Start ADC1 Software Conversion */ 
	//ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能
}


void Cal_ADC_Init(void)
{
	ADC__INIT(); 
	ADC_DMA_Init();
	//DMA_Cmd(DMA1_Channel1, ENABLE);	 //启动DMA通道
	//ADC_Cmd(ADC1, ENABLE);  /* Enable ADC1 */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//启动ADC
}

float GetADC(u8 chanal)
{ //注意：参考电压2.5V
	u8 i;
	u32 sum=0;
	float vol;
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//启动转换
	for(i=0;i<ADC_TIMES;i++)
	{
		sum += ADC1->DR;
		OSTimeDlyHMSM(0,0,0,20);
	}
	sum = sum/ADC_TIMES;
	vol = sum/4096.0*2.5*(47+10)/10;//sum*0.002612+0.0163977;
	return vol;
}

