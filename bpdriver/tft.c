
#include <tft.h>
static GPIO_InitTypeDef     GPIO_InitStructure;
extern INT8U backlight;

void tft_init()
{
/* wzh    
	//设置PD口PD 0,1,4,5,7,8,9,10,11,12,14,15为FSMC模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |GPIO_Pin_4 |GPIO_Pin_5|
                                GPIO_Pin_10 |GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_7;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure); 
    //设置PE口PE 7,8,9,10,11,12,13,14,15为FSMC模式
   	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//设置PD口13管脚INT为输入模式，该信号为RA8875发出的中断请求信号
	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
  
	fsmc_init();
	BP_Init_RA8875();
	backlight=0x20;
	PWM1_duty_cycle(backlight);
	//	
*/  
    //设置PA0为输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
    //设置PD0,1,4,5,7,14,15为输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//设置PE7,8,9,10为输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);
}
void SetDataInput(void)
{
	//设置PD0,1,4,5,7,14,15为输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//设置PE7,8,9,10为输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);
}
void SetDataOutput(void)
{
	//设置PD0,1,14,15为输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//设置PE7,8,9,10为输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void fsmc_init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;                      
  //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
/*-- FSMC Configuration ------------------------------------------------------*/
  p.FSMC_AddressSetupTime = 1;//0
  p.FSMC_AddressHoldTime = 0; //0
  p.FSMC_DataSetupTime = 2;   //2
  p.FSMC_BusTurnAroundDuration = 0; //0
  p.FSMC_CLKDivision = 0;//0
  p.FSMC_DataLatency = 0;//0
  //p.FSMC_AccessMode = FSMC_AccessMode_A;
  p.FSMC_AccessMode = FSMC_AccessMode_B;

  //FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;   /*指定的FSMC块*/
  //FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Enable;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;  /*数据宽度*/
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
  
  /* Enable FSMC Bank1_SRAM Bank */
  //FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE); 

}






void draw_touchpoint(u16 x,u16 y)
{
//	LCD_DrawLine(x-1,y,x+1,y);	
//	LCD_DrawLine(x,y-1,x,y+1);	
//	LCD_DrawPoint(x+1,y+1,BLUE);
//	LCD_DrawPoint(x-1,y+1,BLUE);
//	LCD_DrawPoint(x+1,y-1,BLUE);
//	LCD_DrawPoint(x-1,y-1,BLUE);
	//Draw_Circle(x,y,12);//画中心圈
}	  

//画一个大点
//2*2的点			   
void draw_bigpoint(u16 x,u16 y)
{	    
//	LCD_DrawPoint(x,y,BLUE);//中心点 
////	LCD_DrawPoint(x+1,y,BLUE);
//	LCD_DrawPoint(x,y+1,BLUE);
//	LCD_DrawPoint(x+1,y+1,BLUE);	 	  	
}



INT8U touch_calibration(int * x0,int * y0,int * x1,int *y1)
{								 

	u8  cnt=0;	
	u16 d1,d2;
	u16 tem1,tem2;
	float fac; 	   												  
    int x[4];
	int y[4];
	for(;cnt<=3;cnt++)
	{
		x[cnt]=-1;
		y[cnt]=-1;
	}			
//	LcdClear(WHITE);//清屏 
  //LCD_ShowString(60,150,"step1/4:Please Touch left top coner!      ");
	draw_touchpoint(1,1);//画点1 
//	clear_TP_interrupt();
	while(*x<0||*y<0)
	{
//		clear_TP_interrupt();
		delay_ms(2000);
//		tft_gettouchpoint(x,y);
	}

//	LcdClear(WHITE);//清屏 
	delay_ms(500);
  //LCD_ShowString(70,150,"step2/4:Please Touch right top coner!      ");
	draw_touchpoint(479,1);//画点1 
	while(*(x+1)<0||*(y+1)<0)
	{
//		clear_TP_interrupt();
		delay_ms(2000);
//		tft_gettouchpoint(x+1,y+1);
	}
//	LcdClear(WHITE);//清屏 
	delay_ms(500);
  //LCD_ShowString(80,150,"step3/4:Please Touch left bottm coner!      ");
	draw_touchpoint(1,271);//画点1 
//	clear_TP_interrupt();
	while(*(x+2)<0||*(y+2)<0)
	{
//		clear_TP_interrupt();
		delay_ms(2000);
//		tft_gettouchpoint(x+2,y+2);
	}


//	LcdClear(WHITE);//清屏 
	delay_ms(500);
  //LCD_ShowString(90,150,"step4/4:Please Touch right bottom coner!      ");
	draw_touchpoint(479,271);//画点1
	while(*(x+3)<0||*(y+3)<0)
	{
//		clear_TP_interrupt();
		delay_ms(2000);
//		tft_gettouchpoint(x+3,y+3);
	}

//	LcdClear(WHITE);
	delay_ms(500);

	tem1=abs(x[0]-x[3]);
	tem2=abs(y[0]-y[3]);
		//tem1*=tem1;
		//tem2*=tem2;
	d1=tem1+tem2;//sqrt(tem1+tem2);//得到0,3的距离
	//LcdPrintf(35,40,RED,WHITE,"x[0]-x[3]:%d,y[0]-y[3]:%d, tem1:%d,tem2:%d  d:%d",abs(x[0]-x[3]),abs(y[0]-y[3]),tem1,tem2,d1);
	tem1=abs(x[1]-x[2]);
	tem2=abs(y[1]-y[2]);
		//tem1*=tem1;
		//tem2*=tem2;
	d2=tem1+tem2;//得到1,2的距离
	//LcdPrintf(35,60,RED,WHITE,"x[1]-x[2]:%d,y[1]-y[2]:%d, tem1:%d,tem2:%d  d:%d",abs(x[1]-x[2]),abs(y[1]-y[2]),tem1,tem2,d2);
	fac=(float)d1/d2;

	if(fac<0.98||fac>1.02||d1==0||d2==0)//不合格
	{
			cnt=0;

//			LcdPrintStr("Touch Screen Adjust Failed!",35,110,RED,WHITE,1);//校正失败
			//LcdPrintf(35,130,RED,WHITE,"1:%d,%d 2:%d,%d 3:%d,%d 4:%d,%d",x[0],y[0],x[1],y[1],x[2],y[2],x[3],y[3]);
			//LcdPrintf(35,190,RED,WHITE,"d1:%d d2:%d fac:%f",d1,d2,fac);
			//LcdPrintf(35,150,RED,WHITE,"d1:%d d2:%d fac:%f",d1,d2,fac);


			delay_ms(1000);

			return 0;
		}

		*x0=x[0];*y0=y[0]; *x1=x[3];*y1=y[3];


//		LcdPrintStr("Touch Screen Adjust OK!",35,110,RED,WHITE,1);//校正完成

		delay_ms(500);

		return 1;//校正完成				 

}	

