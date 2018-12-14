//这部分代码可在官网获得

#include "bp_config.h"
#include "RA8875.h"

#define P480x272

#define USE_BLKEJI_LCD 1


	/**************************************************************************************
	函数功能：初始化RA8875

	**************************************************************************************/


	#define RA8875_REG		*((vu16 *)0X60020000)          //命令或状态地址
	#define RA8875_RAM		*((vu16 *)0X60000000)		   //数据读写地址
uint8_t RA8875_ReadReg(uint8_t _ucRegAddr)
{
	RA8875_REG = _ucRegAddr;	/* 设置寄存器地址 */
	return RA8875_RAM;			/* 写入寄存器值 */
}
void RA8875_WriteReg(uint8_t _ucRegAddr, uint8_t _ucRegValue)
{
	RA8875_REG = _ucRegAddr;	/* 设置寄存器地址 */
	RA8875_RAM = _ucRegValue;	/* 写入寄存器值 */
}
static void RA8875_Delaly1us(void)
{
	uint8_t i;

	for (i = 0; i < 10; i++);	/* 延迟1us, 不准 */
}
void Delay(uint16_t nCount)
{
    uint16_t TimingDelay;
    while(nCount--)
    {
      for(TimingDelay=0;TimingDelay<10000;TimingDelay++);
    }
}
/*
*********************************************************************************************************
*	函 数 名: LCD_SetCursor
*	功能说明: 设置光标位置
*	形    参：_usX : X坐标; _usY: Y坐标
*	返 回 值: 无
*********************************************************************************************************
*/
static void LCD_SetCursor(uint16_t _usX, uint16_t _usY)
{
	/* 设置内存写光标的坐标 【0x80-83 是光标图形的坐标】 */
	RA8875_WriteReg(0x46, _usX);
	RA8875_WriteReg(0x47, _usX >> 8);
	RA8875_WriteReg(0x48, _usY);
	RA8875_WriteReg(0x49, _usY >> 8);
}
/*
*********************************************************************************************************
*	函 数 名: LCD_SetDispWin
*	功能说明: 设置显示窗口，进入窗口显示模式。TFT驱动芯片支持窗口显示模式，这是和一般的12864点阵LCD的最大区别。
*	形    参：
*		_usX : 水平坐标
*		_usY : 垂直坐标
*		_usHeight: 窗口高度
*		_usWidth : 窗口宽度
*	返 回 值: 无
*********************************************************************************************************
*/
static void LCD_SetDispWin(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth)
{
		uint16_t usTemp;

		//setting active window X
		RA8875_WriteReg(0x30, _usX);
	    RA8875_WriteReg(0x31, _usX >> 8);

		RA8875_WriteReg(0x32, _usY);
	    RA8875_WriteReg(0x33, _usY >> 8);

		usTemp = _usWidth + _usX - 1;
		RA8875_WriteReg(0x34, usTemp);
	    RA8875_WriteReg(0x35, usTemp >> 8);

		usTemp = _usHeight + _usY - 1;
		RA8875_WriteReg(0x36, usTemp);
	    RA8875_WriteReg(0x37, usTemp >> 8);

	    LCD_SetCursor(_usX, _usY);
}


#if USE_BLKEJI_LCD
void BP_Init_RA8875(void)
{

	  //s_LcdHeight = 272;
		//s_LcdWidth = 480;

		/* 初始化PLL.  晶振频率为25M */
		RA8875_REG = 0x88;
		RA8875_Delaly1us();		/* 延迟1us */
		RA8875_RAM = 10;	  	/* PLLDIVM [7] = 0 ;  PLLDIVN [4:0] = 10 */

	  delay_ms(1);	

		RA8875_REG = 0x89;
		RA8875_Delaly1us();	    /* 延迟1us */
		RA8875_RAM = 2;		    /* PLLDIVK[2:0] = 2, 除以4 */

		/* RA8875 的内部系统频率 (SYS_CLK) 是结合振荡电路及PLL 电路所产生，频率计算公式如下 :
		  SYS_CLK = FIN * ( PLLDIVN [4:0] +1 ) / ((PLLDIVM+1 ) * ( 2^PLLDIVK [2:0] ))
		          = 25M * (10 + 1) / ((0 + 1) * (2 ^ 2))
				  = 68.75MHz
		*/

		/* REG[88h]或REG[89h]被设定后，为保证PLL 输出稳定，须等待一段「锁频时间」(< 100us)。*/
	  Delay(1);

		/*
			配置系统控制寄存器。 中文pdf 第18页:

			bit3:2 色彩深度设定 (Color Depth Setting)
				00b : 8-bpp 的通用TFT 接口， i.e. 256 色。
				1xb : 16-bpp 的通用TFT 接口， i.e. 65K 色。	 【选这个】

			bit1:0 MCUIF 选择
				00b : 8-位MCU 接口。
				1xb : 16-位MCU 接口。 【选这个】
		*/
		RA8875_WriteReg(0x10, (1 <<3 ) | (1 << 1));	/* 配置16位MCU并口，65K色 */

		/* REG[04h] Pixel Clock Setting Register   PCSR
			bit7  PCLK Inversion
				0 : PDAT 是在PCLK 正缘上升 (Rising Edge) 时被取样。
				1 : PDAT 是在PCLK 负缘下降 (Falling Edge) 时被取样。
			bit1:0 PCLK 频率周期设定
				Pixel Clock  PCLK 频率周期设定。
				00b: PCLK 频率周期= 系统频率周期。
				01b: PCLK 频率周期= 2 倍的系统频率周期。
				10b: PCLK 频率周期= 4 倍的系统频率周期。
				11b: PCLK 频率周期= 8 倍的系统频率周期。
		*/
	  RA8875_WriteReg(0x04,0x82);           //通过04H设置采样边沿和PCLK像素时钟(Pixel Clock)
	  Delay(1);
		
	  RA8875_WriteReg(0x14,0x3B);           //水平设置:水平显示宽度(像素) = (HDWR + 1) * 8 = 480
	  RA8875_WriteReg(0x15,0x00);           //水平非显示周期微调宽度设置 (HNDFTR)
		RA8875_WriteReg(0x16,0x01);           //水平非显示宽度(像素) = (HNDR + 1) * 8
		RA8875_WriteReg(0x17,0x00);           //水平同步起始地址宽度 = (HSTR + 1) * 8
		RA8875_WriteReg(0x18, 0x05);          //水平同步信号脉波宽度(像素) = (HPWR + 1) * 8

		RA8875_WriteReg(0x19, 0x0F);          //垂直显示高度低8位 [0-7] （VDHR0） 
		RA8875_WriteReg(0x1A, 0x01);          //垂直显示高度(像素) = VDHR + 1 = 0X10F + 1 = 272
		RA8875_WriteReg(0x1B, 0x02);          //垂直非显示周期位[7:0]（VNDR0） 
		RA8875_WriteReg(0x1C, 0x00);          //垂直非显示周期位[8] （VNDR1）
		RA8875_WriteReg(0x1D, 0x07);          //垂直同步信号起始地址高度位[7:0]  VSTR0 
		RA8875_WriteReg(0x1E, 0x00);          //垂直同步信号起始地址高度位[8]  VSTR1  
		RA8875_WriteReg(0x1F, 0x09);          //垂直同步信号脉波宽度 VPWR


	  //s_LcdHeight = 272;
		//s_LcdWidth = 480;


	/* 设置TFT面板的 DISP  引脚为高，使能面板. 4.3寸TFT模块的DISP引脚连接到RA8875芯片的GP0X脚 */
	RA8875_WriteReg(0xC7, 0x01);	/* DISP = 1 */

	/* LCD 显示/关闭讯号 (LCD Display on) */
	RA8875_WriteReg(0x01, 0x80);

	/* 	REG[40h] Memory Write Control Register 0 (MWCR0)

		Bit 7	显示模式设定
			0 : 绘图模式。
			1 : 文字模式。
 
		Bit 6	文字写入光标/内存写入光标设定
			0 : 设定文字/内存写入光标为不显示。
			1 : 设定文字/内存写入光标为显示。

		Bit 5	文字写入光标/内存写入光标闪烁设定
			0 : 游标不闪烁。
			1 : 游标闪烁。

		Bit 4   NA

		Bit 3-2  绘图模式时的内存写入方向
			00b : 左 -> 右，然后上 -> 下。
			01b : 右 -> 左，然后上 -> 下。
			10b : 上 -> 下，然后左 -> 右。
			11b : 下 -> 上，然后左 -> 右。

		Bit 1 	内存写入光标自动增加功能设定
			0 : 当内存写入时光标位置自动加一。
			1 : 当内存写入时光标位置不会自动加一。

		Bit 0 内存读取光标自动增加功能设定
			0 : 当内存读取时光标位置自动加一。
			1 : 当内存读取时光标位置不会自动加一。
	*/
	RA8875_WriteReg(0x40, 0x00);	/* 选择绘图模式 */


	/* 	REG[41h] Memory Write Control Register1 (MWCR1)
		写入目的位置，选择图层1
	*/
	RA8875_WriteReg(0x41, 0x00);	/* 选择绘图模式, 目的为CGRAM */

	LCD_SetDispWin(0, 0, 272, 480);

	RA8875_REG = 0x20;
	RA8875_RAM = 0x00;

	RA8875_REG = 0x52;
	RA8875_REG = 0x00;
	
		    /* 7 触摸屏	 设置*/
		WriteCommand(0x71);
		WriteData(0x00);		//自动模式				     
		RA8875_WAITSTATUS();
		//TP_auto_mode();			//enable auto mode
		/*
			u8 temp;
  			WriteCommand(0x71);//TPCR1	
			temp = ReadData();
			temp &= cClrD6;//temp |= 0x3F;	AUTO MODE
			
			//temp |= 0x3F;               //外部基准，0x1f内部基准
			WriteData(temp);
			RA8875_WAITSTATUS();

		TPCR1 71H
		7   N/A     
		6   0自动   1手动   
		5   0内参考 1外参考
		43  N/A
		2   0关消弹 1开消弹 
		10  手动模式选择位 00闲置 01侦察		         
		*/


		WriteCommand(0x70);		//设置触摸屏采样时间
		WriteData(0xb3);//0xb3 0xa2		开启1 AD采样4096个周期011 关闭触发事件唤醒模式0 触控面板控制器ADC频率设定系统CLK/8 011
		RA8875_WAITSTATUS();  //等待设置成功	

		Touch_Panel_Interrupt_Enable();	// 使能触摸屏中断
		clear_TP_interrupt();			// 清中断事件
	
			/*8.设置背光 */
	PWM1_enable();
	PWM1_fnuction_sel();
	PWM1_clock_ratio(0x03);		//2MHz PWM1
	PWM1_duty_cycle(0xff);		//占空比100%
}

#else
void BP_Init_RA8875(void)
	{
  		/*1.通过88H,89H寄存器设置时钟频率*/
		WriteCommand(0x88);	
		WriteData(0x4);
		delay_ms(1);		
		WriteCommand(0x89);
		WriteData(0x02);	
		//PLLDIVM=0 PLLDIVN=4 PLLDIVK=2
		//FPLL = FIN * ( PLLDIVN [4:0] +1 ) 必需等于或大于110 MHz
		//FPLL = FIN*(PLLDIVN[4:0] +1)=25*5=	125M>110M
		//SYS_CLK=125/4=31.25M	  
		delay_ms(10);

  		/*2.通过10H设置颜色和接口*/
		WriteCommand(0x10);  
		WriteData(0x0F);     // 3-2位 11 64k色  1-0位 11 16位 MCU 接口  
		delay_ms(1);
		
		/*3.通过04H设置采样边沿和PCLK像素时钟(Pixel Clock)*/
	    WriteCommand(0x04);   //set PCLK invers
	    WriteData(0x82); 	  //7位 1 : PDAT 是在PCLK 负缘下降 (Falling Edge) 时被取样
		                      //1-0位 10b: PCLK 频率周期= 4 倍的系统频率周期。125M 
	   	delay_ms(1);
	
	    /*4:水平设置 */
	    WriteCommand(0x14);                   
	    WriteData(0x3B);	//水平显示宽度(像素) = (HDWR + 1)*8 = 480      
		              
	    WriteCommand(0x15); //水平非显示周期微调宽度设置 (HNDFTR)
	    WriteData(0x00);               
	    
		WriteCommand(0x16); //水平非显示周期宽度（HNDR）                    
	    WriteData(0x01);    //水平非显示宽度(像素) = (HNDR + 1)*8 =18 
		         
	    WriteCommand(0x17); // 水平同步信号 (HSYNC) 起始地址宽度HSTR                                 
	    WriteData(0x00);	//水平同步起始地址宽度 = (HSTR + 1)* 8 =8    
		
	    WriteCommand(0x18); //水平同步信号 (HSYNC) 脉冲宽度（HPWR）                
	    WriteData(0x05);    //水平同步信号脉波宽度 (像素) = (HPWR+ 1)*8=48       

	    /*5:垂直设置*/                                      
	    WriteCommand(0x19);  //垂直显示高度低8位 [0-7] （VDHR0）                       
	    WriteData(0x0f);                                                     
	    WriteCommand(0x1a);  //垂直显示高度高位   （VDHR1）                
	    WriteData(0x01);     // 垂直显示高度(像素)= VDHR+1=0X10F+1=272          
		                 
	    WriteCommand(0x1b);  //垂直非显示周期位[7:0]（VNDR0）                     
	    WriteData(0x02);                           
	    WriteCommand(0x1c);  //垂直非显示周期位[8] （VNDR1）                            
	    WriteData(0x00);                            
	    WriteCommand(0x1d);  //垂直同步信号起始地址高度位[7:0]  VSTR0                             
	    WriteData(0x07);	                           
	    WriteCommand(0x1e);  //垂直同步信号起始地址高度位[8]  VSTR1                    
	    WriteData(0x00);                         
	    WriteCommand(0x1f);  //垂直同步信号脉波宽度 VPWR
	    WriteData(0x09);   
	
	    /*6.活动窗口active window设置*/ 
		
	    WriteCommand(0x30); //水平起点低位 (HSAW0)
	    WriteData(0x00);    
	    WriteCommand(0x31); //水平起点高位 (HSAW1)	   
	    WriteData(0x00);  
	    WriteCommand(0x34); //水平结束点低位 (HEAW0)
	    WriteData(0xDF); 
	    WriteCommand(0x35); //水平结束点高位 (HEAW1)	   
	    WriteData(0x01); 
		//0-479   1DF=479
	
	    WriteCommand(0x32); //垂直起点低位  (VSAW0)
	    WriteData(0x00);    
	    WriteCommand(0x33); //垂直起点高位  (VSAW0)	   
	    WriteData(0x00); 
	    WriteCommand(0x36); //垂直结束点低位 (VEAW0)
	    WriteData(0x0F); 
	    WriteCommand(0x37); //垂直结束点高位 (VEAW1)	   
	    WriteData(0x01); 	 
		//0-271	  10F=271
		
	    /* 7 触摸屏	 设置*/
		WriteCommand(0x71);
		WriteData(0x00);		//自动模式				     
		RA8875_WAITSTATUS();
		//TP_auto_mode();			//enable auto mode
		/*
			u8 temp;
  			WriteCommand(0x71);//TPCR1	
			temp = ReadData();
			temp &= cClrD6;//temp |= 0x3F;	AUTO MODE
			
			//temp |= 0x3F;               //外部基准，0x1f内部基准
			WriteData(temp);
			RA8875_WAITSTATUS();

		TPCR1 71H
		7   N/A     
		6   0自动   1手动   
		5   0内参考 1外参考
		43  N/A
		2   0关消弹 1开消弹 
		10  手动模式选择位 00闲置 01侦察		         
		*/


		WriteCommand(0x70);		//设置触摸屏采样时间
		WriteData(0xb3);//0xb3 0xa2		开启1 AD采样4096个周期011 关闭触发事件唤醒模式0 触控面板控制器ADC频率设定系统CLK/8 011
		//1                    1 0 1                0                            011
		//bit7 开启1 
		//             AD采样4096个周期011 
		//                                   关闭触发事件唤醒模式0 
		//                                                              触控面板控制器ADC频率设定系统CLK/8 011
		RA8875_WAITSTATUS();  //等待设置成功	

		Touch_Panel_Interrupt_Enable();	// 使能触摸屏中断
		clear_TP_interrupt();			// 清中断事件


		Clear_Active_Window();      //清活动窗口 寄存器8E 6位写1
		Text_Foreground_Color(WHITE);  
		Text_Background_Color(RED);
		
		Memory_Clear();	            //清内存 寄存器8E 7位写1
		Display_ON();				//寄存器01 位7写1 开显示
		
		/*8.设置背光 */
		PWM1_enable();
		PWM1_fnuction_sel();
		PWM1_clock_ratio(0x03);		//2MHz PWM1
		PWM1_duty_cycle(0xff);		//占空比100%
		
		/*9 设置数字光标 */
		Text_Cursor_Horizontal_Size(24);   //光标长度设置 寄存器4E 写24
		Text_Cursor_Vertical_Size(4);	   //光标高度设置 寄存器4F 写4
		Text_Blink_Time(0x35);			   //文字闪烁时间设定 寄存器44 写0x35   0011 0101
		Text_Cursor_Blink_Enable();		   //寄存器40 D5=1 游标闪
		Text_Cursor_Disable();			   //

	}
#endif	


void Text_color(u16 color)//0xF800 == RED
{
	WriteCommand(0x63);//放R
	WriteData((color&0xf800)>>11);//5  R
	
	
	WriteCommand(0x64);
	WriteData((color&0x07e0)>>5);//6  G
	
	WriteCommand(0x65);
	WriteData((color&0x001f));//5    B
	
	RA8875_WAITSTATUS();
}

//RA8875等待空闲
void RA8875_WAITSTATUS(void)
{
	u8 temp;
	do
	{
		temp=LCD_StatusRead();
	}while((temp&0x80)==0x80);

}

u8 LCD_StatusRead(void)
{
	u16 Data;
	Data = ReadCOMM();
	return Data;
}
				
//------------------------//
//STATUS 
void Chk_Busy(void)
{
	u8 temp;
	do
	{
		temp=LCD_StatusRead();
	}while((temp&0x80)==0x80);
}
void Chk_Busy_BTE(void)
{
	u8 temp;
   	
	do
	{
		temp=LCD_StatusRead();
	}while((temp&0x40)==0x40);
}

void Chk_Busy_DIR_Access(void)
{
	u8  temp;
   	
	do
	{
		temp=LCD_StatusRead();
	}while((temp&0x01)==0x01);
}

//REG[BFh]
void Chk_DMA_Busy(void)
{
	u8 temp; 	
	do
	{
		WriteCommand(0xBF);
		temp = ReadData();
	}while((temp&0x01)==0x01);
}
//REG[01h]		 

void Display_ON(void)
{	
	WriteCommand(0x01);
	WriteData(0x80);
	RA8875_WAITSTATUS();
}

void Display_OFF(void)
{
	WriteCommand(0x01);
	WriteData(0x00);
}

void Normal_Mode_exitSleep(void)
{	
	WriteCommand(0x01);
	WriteData(0x00);
}
void Sleep_Mode(void)
{
	WriteCommand(0x01);
	WriteData(0x02);
}
void Software_Reset(void)
{	
	WriteCommand(0x01);
	WriteData(0x01);
	WriteData(0x00);
	delay_ms(1);// 
}							   
//-------------------------------//
//REG[04h] for test 
void PCLK_inversion(void)
{
	u8 temp;
  	WriteCommand(0x04);
	temp = ReadData();
	temp |= cSetD7;
	WriteData(temp);
}
void PCLK_non_inversion(void)
{
	u8 temp;
  	WriteCommand(0x04);
	temp = ReadData();
	temp &= cClrD7;
	WriteData(temp);
}

void PCLK_width(u8 setx) //u8[1:0]
{
	u8 temp;
  	WriteCommand(0x04);
	temp = ReadData();
	temp &=0x80;
	temp |= setx;
	WriteData(temp);
}

//-------------------------------
//REG[05h]
//串行FLASH/ROM配置寄存器 CONFIGURATION REGISTER （SROC）
//	/* 05H  REG[05h] Serial Flash/ROM Configuration Register (SROC)
/*
//	S7	
Serial Flash/ROM I/F # 选择
0:选择Serial Flash/ROM 0 接口。[亮点STM32集通字库接在 0 接口]
1:选择Serial Flash/ROM 1 接口。
		6	Serial Flash/ROM 寻址模式
				0: 24 位寻址模式。
				此位必须设为0。
		5	Serial Flash/ROM 波形模式
				0: 波形模式 0。
				1: 波形模式 3。
		4-3	Serial Flash /ROM 读取周期 (Read Cycle)
			00b: 4 bus ?? 无空周期 (No Dummy Cycle)。
			01b: 5 bus ??1 byte 空周期。
			1Xb: 6 bus ??2 byte 空周期。
		2	Serial Flash /ROM 存取模式 (Access Mode)
			0: 字型模式 。
			1: DMA 模式。
		1-0	Serial Flash /ROM I/F Data Latch 选择模式
			0Xb: 单一模式。
			10b: 双倍模式0。
			11b: 双倍模式1。
	*/
//-------------------------------
void Serial_ROM_select0(void)
{
	u8 temp;
  	WriteCommand(0x05);
	temp = ReadData();
	temp &=cClrD7;
	WriteData(temp);
}

void Serial_ROM_select1(void)
{
	u8 temp;
  	WriteCommand(0x05);
	temp = ReadData();
	temp |=cSetD7;
	WriteData(temp);
}

void Serial_ROM_Address_set_24u8(void)
{
	u8 temp;
  	WriteCommand(0x05);
	temp = ReadData();
	temp &=cClrD6;
	WriteData(temp);
}

void Serial_ROM_Address_set_32u8(void)
{
	u8 temp;
  	WriteCommand(0x05);
	temp = ReadData();
	temp |=cSetD6;
	WriteData(temp);
}

void Select_Serial_Waveform_mode0(void)
{	u8 temp;
  	WriteCommand(0x05);
	temp = ReadData();
	temp &=cClrD5;
	WriteData(temp);
}

void Select_Serial_Waveform_mode3(void)
{	u8 temp;
  	WriteCommand(0x05);
	temp = ReadData();
	temp |=cSetD5;
	WriteData(temp);
}

void SERIAL_ROM_Read_Cycle_4bus(void)
{   u8 temp;
  	WriteCommand(0x05);
	temp = ReadData();
	temp &=0xE7;
	WriteData(temp);
}

void SERIAL_ROM_Read_Cycle_5bus(void)
{
	u8 temp;
	WriteCommand(0x05);
	temp = ReadData();
	temp &=0xE7;
	temp |=cSetD3;
	WriteData(temp);
}

void SERIAL_ROM_Read_Cycle_6bus(void)
{
	u8 temp;
	WriteCommand(0x05);
	temp = ReadData();
	temp &=0xE7;
	temp |=cSetD4;
	WriteData(temp);
}

void SERIAL_ROM_Font_mode(void)	  //字型模式
{
	u8 temp;
  	WriteCommand(0x05);
	temp = ReadData();
	temp &=cClrD2;
	WriteData(temp);
}



void SERIAL_ROM_DMA_mode(void)	 //DMA模式
{
	u8 temp;
	WriteCommand(0x05);
	temp = ReadData();
	temp |=cSetD2;
	WriteData(temp);
}

void SERIAL_ROM_Signal_mode(void)  //模式：单一模式
{
	u8 temp;
	WriteCommand(0x05);
	temp = ReadData();
	temp &=0xFC;
	WriteData(temp);
}

void SERIAL_ROM_Dual_mode0(void)	//模式：双倍模式0
{
	u8 temp;
	WriteCommand(0x05);
	temp = ReadData();
	temp &=0xFC;
	temp |=cSetD1;
	WriteData(temp);
}

void SERIAL_ROM_Dual_mode1(void)	//模式：双倍模式1
{
	u8 temp;
	WriteCommand(0x05);
	temp = ReadData();
	temp &=0xFC;
	temp |=cSetD1;
	temp |=cSetD0;
	WriteData(temp);
}

//-------------------------------
//REG[06h]
//串行FLASH/ROM时钟设置寄存器 
// 0X:系统时钟频率
// 10：系统时钟频率/2
// 11：系统时钟频率/4
//-------------------------------
void SROM_CLK_DIV(u8 CLK_DIV)
{
	WriteCommand(0x06);
	WriteData(CLK_DIV);
}


//-------------------------------//
//REG[10h]
void Color_256(void)
{
	u8 temp;
	WriteCommand(0x10);//SYSR
	temp = ReadData();
	temp &= cClrD3D2 ;
	WriteData(temp);
} 

void Color_65K(void)
{
	u8 temp;
	WriteCommand(0x10);//SYSR
	temp = ReadData();
	temp &= cClrD3D2 ;
	temp |= cSetD3ClrD2 ;
	WriteCommand(0x10);//SYSR
	WriteData(temp);
}
void MPU_8u8_Interface(void)
{
	u8 temp;
	WriteCommand(0x10);//SYSR
	temp = ReadData();
	temp &= cClrD1D0 ;
	WriteData(temp);
}

void MPU_16u8_Interface(void)
{
	u8 temp;
	WriteCommand(0x10);//SYSR
	temp = ReadData();
	temp |= cSetD1ClrD0 ;
	WriteData(temp);
} 								 
//--------------------------------//
//REG[12h] 

u8 GPI_data(void)
{
	u8 temp;
	WriteCommand(0x12);//SYSR
	temp = ReadData();
	return temp;
}

void GPO_data(u8 setx)
{
	WriteCommand(0x13); 
	WriteData(setx);
}

							
//---------------------------------//
//REG[20h]

void One_Layer(void)
{
	u8 temp;
	WriteCommand(0x20);//DPCR
	temp = ReadData();
	temp &= cClrD7 ;
	WriteData(temp);
}	
				 
void Two_Layers(void)
{
	u8 temp;
	WriteCommand(0x20);//DPCR
	temp = ReadData();
	temp |= cSetD7 ;
	WriteData(temp);
}

void HDIR_SEG0_SEGn(void)
{
	u8 temp;
	WriteCommand(0x20);//DPCR
	temp = ReadData();
	temp &= cClrD3 ;
	WriteData(temp);
}
void HDIR_SEGn_SEG0(void)
{
	u8 temp;
	WriteCommand(0x20);//DPCR
	temp = ReadData();
	temp |= cSetD3 ;
	WriteData(temp);
	RA8875_WAITSTATUS();
}
void VDIR_COM0_COMn(void)
{
	u8 temp;
	WriteCommand(0x20);//DPCR
	temp = ReadData();
	temp &= cClrD2 ;
	WriteData(temp);
}
void VDIR_COMn_COM0(void)
{
	u8 temp;
	WriteCommand(0x20);//DPCR
	temp = ReadData();
	temp |= cSetD2 ;
	WriteData(temp);
}


//----------------------------------
//REG[21h] 
//格式控制寄存器0 
//----------------------------------

void CGROM_Font(void) //选择CGROM字体
{
	u8 temp;
	WriteCommand(0x21);//FNCR0
	temp = ReadData();
	temp &= cClrD7 ;
	WriteData(temp);
}
void CGRAM_Font(void) //选择CGRAM字体
{
	u8 temp;
	WriteCommand(0x21);//FNCR0
	temp = ReadData();
	temp |= cSetD7 ;
	WriteData(temp);
}

void Internal_CGROM(void)  //选择内部CGROM
{
	u8 temp;
	WriteCommand(0x21);//FNCR0
	temp = ReadData();
	temp &= cClrD5 ;
	WriteData(temp);
}
void External_CGROM(void)  //选择外部CGROM
{
	u8 temp;
	WriteCommand(0x21);//FNCR0
	temp = ReadData();
	temp |= cSetD5 ;
	WriteData(temp);
}

u8 Is_External_CGROM(void)  //判断是否是使用外部CGROM
{
	u8 temp;
	WriteCommand(0x21);//FNCR0
	temp = ReadData();
	return(temp&cSetD5) ;
}



void ISO8859_1(void)
{
	u8 temp;
	WriteCommand(0x21);//FNCR0
	temp = ReadData();
	temp &= cClrD1D0 ;
	WriteData(temp);
}
void ISO8859_2(void)
{
	u8 temp;
	WriteCommand(0x21);//FNCR0
	temp = ReadData();
	temp &= cClrD1D0 ;
	temp |= cClrD1SetD0 ;
	WriteData(temp);
}
void ISO8859_3(void)
{
	u8 temp;
	WriteCommand(0x21);//FNCR0
	temp = ReadData();
	temp &= cClrD1D0 ;
	temp |= cSetD1ClrD0 ;
	WriteData(temp);
}
void ISO8859_4(void)
{
	u8 temp;
	WriteCommand(0x21);//FNCR0
	temp = ReadData();
	temp &= cClrD1D0 ;
	temp |= cSetD1D0 ;
	WriteData(temp);
}								 
//------------------------------------------------
//关于寄存器22h 字体设置寄存器的函数
//------------------------------------------------
//字体不对齐
void No_FullAlignment(void)
{
	u8 temp;
	WriteCommand(0x22);//FNCR1
	temp = ReadData();
	temp &= cClrD7 ;
	WriteData(temp);
}
//字体对齐
void FullAlignment(void)
{
	u8 temp;
	WriteCommand(0x22);//FNCR1
	temp = ReadData();
	temp |= cSetD7 ;
	WriteData(temp);
}
//字体背景不透明
void Font_with_BackgroundColor(void)
{
	u8 temp;
	WriteCommand(0x22);//FNCR1
	temp = ReadData();
	temp &= cClrD6 ;
	WriteData(temp);
}
//字体背景透明
void Font_with_BackgroundTransparency(void)
{
	u8 temp;
	WriteCommand(0x22);//FNCR1
	temp = ReadData();
	temp |= cSetD6 ;
	WriteData(temp);
	RA8875_WAITSTATUS();
}
//字体不旋转
void NoRotate_Font(void)
{
	u8 temp;
	WriteCommand(0x22);//FNCR1
	temp = ReadData();
	temp &= cClrD4 ;
	WriteData(temp);
}
//字体旋转90度
void Rotate90_Font(void)
{
	u8 temp;
	WriteCommand(0x22);//FNCR1
	temp = ReadData();
	temp |= cSetD4 ;
	WriteData(temp);
}

//水平放大1倍
void Horizontal_FontEnlarge_x1(void)   
{
	u8 temp;
	WriteCommand(0x22);//FNCR1
	temp = ReadData();
	temp &= cClrD3D2 ;
	WriteData(temp);
}
//水平放大2倍
void Horizontal_FontEnlarge_x2(void)
{
	u8 temp;
	WriteCommand(0x22);//FNCR1
	temp = ReadData();
	temp &= cClrD3D2 ;
	temp |= cClrD3SetD2 ;
	WriteData(temp);
}
//水平放大3倍
void Horizontal_FontEnlarge_x3(void)
{
	u8 temp;
	WriteCommand(0x22);//FNCR1
	temp = ReadData();
	temp &= cClrD3D2 ;
	temp |= cSetD3ClrD2 ;
	WriteData(temp);
}
//水平放大4倍
void Horizontal_FontEnlarge_x4(void)
{
	u8 temp;
	WriteCommand(0x22);//FNCR1
	temp = ReadData();
	temp &= cClrD3D2 ;
	temp |= cSetD3D2 ;
	WriteData(temp);
}
//垂直放大1倍
void Vertical_FontEnlarge_x1(void)
{
	u8 temp;
	WriteCommand(0x22);//FNCR1
	temp = ReadData();
	temp &= cClrD1D0 ;
	WriteData(temp);
}
//垂直放大2倍
void Vertical_FontEnlarge_x2(void)
{
	u8 temp;
	WriteCommand(0x22);//FNCR1
	temp = ReadData();
	temp &= cClrD1D0 ;
	temp |= cClrD1SetD0 ;
	WriteData(temp);
}
//垂直放大3倍
void Vertical_FontEnlarge_x3(void)
{
	u8 temp;
	WriteCommand(0x22);//FNCR1
	temp = ReadData();
	temp &= cClrD1D0 ;
	temp |= cSetD1ClrD0 ;
	WriteData(temp);
}
//垂直放大4倍
void Vertical_FontEnlarge_x4(void)
{
	u8 temp;
	WriteCommand(0x22);//FNCR1
	temp = ReadData();
	temp &= cClrD1D0 ;
	temp |= cSetD1D0 ;
	WriteData(temp);
}

//--------------------------------------------------
//REG[23h] CGRAM Select Register
//写自造字型位置寄存器	
//CGRAM 文字编号的设定，是用来写入使用者自订的文字位图数
//据到CGRAM 中。连续16 笔数据写入一个8x16 文字位图。注意
//MWCR1（内存写控制寄存器） bit 3-2 先设定为01b(CGRAM)，超过16 笔数据写入，
//会循环回到第一笔数据且覆盖位
//							 
void CGRAM_Select_Number(u8 setx)
{ 
	WriteCommand(0x23);//CGSR
	WriteData(setx);
}	
								
//--------------------------------------------------
//REG[24h]REG[25h]REG[26h]REG[27h]
void Scroll_Offset(u16 X,u16 Y)
{
	WriteCommand(0x24);//HOFS0
	WriteData(X);
	//RA8875_WAITSTATUS();
	WriteCommand(0x25);//HOFS1	   
	WriteData(X>>8);
	//RA8875_WAITSTATUS();
	WriteCommand(0x26);//VOFS0
	WriteData(Y);  
	//RA8875_WAITSTATUS();
	WriteCommand(0x27);//VOFS1	   
	WriteData(Y>>8); 
	RA8875_WAITSTATUS();
}	 
//--------------------------------------------------
//REG[29h]
void Line_distance(u8 setx)
{ 
	WriteCommand(0x29);//LDS
	WriteData(setx);
}	  
//--------------------------------------------------
//REG[2Ah]REG[2Bh]REG[2Ch]REG[2Dh]
void Font_Coordinate(u16 X,u16 Y)
{

	WriteCommand(0x2A);//HOFS0
	WriteData(X);

	WriteCommand(0x2B);//HOFS1	   
	WriteData(X>>8);
	
	WriteCommand(0x2C);//VOFS0
	WriteData(Y);
	
	WriteCommand(0x2D);//VOFS1	   
	WriteData(Y>>8);   
	 
}


//--------------------------------------------------
//REG[2Eh]

void FontSize(u8 font)//font = 16 24 32
{
	External_CGROM(); //选择外字库 21H 5位置1
	SROM_CLK_DIV(3);  //4分之一SYSCLK  06H 写3
	if (ZIKU==0)
		Serial_ROM_select0();		
	else
		Serial_ROM_select1();
	GT_serial_ROM_select_GT23L32S4W();	 //2FH 设置 选择字库类型
	Text_Mode(); //字符模式 40H S7置1
	Font_with_BackgroundTransparency();	

	if(font==16)Fontsize_16x16();
	if(font==32)Fontsize_32x32();
	if(font==24)Fontsize_24x24();
}

void FontSizeGaotong(u8 font)//font = 16 24 32
{
	External_CGROM(); //选择外字库 21H 5位置1
	SROM_CLK_DIV(3);  //4分之一SYSCLK  06H 写3
		Serial_ROM_select0();		//高通字库
	GT_serial_ROM_select_GT23L32S4W();	 //2FH 设置 选择字库类型
	Text_Mode(); //字符模式 40H S7置1
	Font_with_BackgroundTransparency();	

	if(font==16)Fontsize_16x16();
	if(font==32)Fontsize_32x32();
	if(font==24)Fontsize_24x24();
}
void FontSizeSpiflash(u8 font)//font = 16 24 32
{
	External_CGROM(); //选择外字库 21H 5位置1
	SROM_CLK_DIV(3);  //4分之一SYSCLK  06H 写3
	Serial_ROM_select1();		//05H 位7清0 亮点板子上SPIFLASH字库连在SFCS1
	GT_serial_ROM_select_GT23L32S4W();	 //2FH 设置 选择字库类型
	Text_Mode(); //字符模式 40H S7置1
	Font_with_BackgroundTransparency();	

	if(font==16)Fontsize_16x16();
	if(font==32)Fontsize_32x32();
	if(font==24)Fontsize_24x24();
}
void Fontsize_16x16(void) 
{
 	u8 temp;
	WriteCommand(0x2E);//FNCR1
	temp = ReadData();
	temp &= 0x3F;
	WriteData(temp);
}

void Fontsize_24x24(void) 
{
 	u8 temp;
	WriteCommand(0x2E);//FNCR1
	temp = ReadData();
	temp &= 0x3F;
	temp |= 0x40; 
	WriteData(temp);
}

void Fontsize_32x32(void) 
{
 	u8 temp;
	WriteCommand(0x2E);//FNCR1
	temp = ReadData();
	temp &= 0x3f;
	temp |= 0x80; 
	WriteData(temp);
}


void Font_spacing_set(u8 setx) //u8[5:0]
{
	u8 temp,temp1;
	temp1=setx&0x3F;
   	WriteCommand(0x2E);//FNCR1
	temp = ReadData();
	temp &= 0xC0;
	temp |= temp1; 
	WriteData(temp);
}

//--------------------------------------------------
//REG[2Fh]
//串行字体ROM设置
//--------------------------------------------------
void GT_serial_ROM_select_GT21L16T1W(void)
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0x1F;
	WriteData(temp);
}

void GT_serial_ROM_select_GT23L16U2W(void)
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0x1F;
	temp |= 0x20;
	WriteData(temp);
}

void GT_serial_ROM_select_GT23L24T3Y(void)
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0x1F;
	temp |= 0x40;
	WriteData(temp);
}

void GT_serial_ROM_select_GT23L24M1Z(void)
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0x1F;
	temp |= 0x60;
	WriteData(temp);
}

void GT_serial_ROM_select_GT23L32S4W(void) //选择GT23L32S4W
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0x1F;
	temp |= 0x80;
	WriteData(temp);
	RA8875_WriteReg(0x40, (1 << 7));	/* 设置为文本模式 */
	RA8875_WriteReg(0x06, 0x02);	/* 设置为文本模式 */
	RA8875_WriteReg(0x21, (0 << 7) | (1 << 5));
	RA8875_WriteReg(0x05, (0 << 7) | (0 << 6) | (1 << 5) | (1 << 3) | (0 << 2) | (0 << 1));
	RA8875_WriteReg(0x2F, (4 << 5) | (0 << 2) | (1 << 0));
	WriteCommand(0x02); 			/* 用于设定RA8875 进入内存(DDRAM或CGRAM)读取/写入模式 */
}

void Font_code_GB2312(void)
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0xE3;
	WriteData(temp);
}

void Font_code_GB12345(void)
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0xE3;
	temp |= 0x04;
	WriteData(temp);
}

void Font_code_BIG5(void)
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0xE3;
	temp |= 0x08;
	WriteData(temp);
}

void Font_code_UNICODE(void)
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0xE3;
	temp |= 0x0C;
	WriteData(temp);
}

void Font_code_ASCII(void)
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0xE3;
	temp |= 0x10;
	WriteData(temp);
}

void Font_code_UNIJIS(void)
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0xE3;
	temp |= 0x14;
	WriteData(temp);
}

void Font_code_JIS0208(void)
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0xE3;
	temp |= 0x18;
	WriteData(temp);
}

void Font_code_LATIN(void)
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0xE3;
	temp |= 0x1C;
	WriteData(temp);
}

void Font_Standard(void)
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0xFC;
	WriteData(temp);
}

void Font_Arial(void)
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0xFC;
	temp |= 0x01;
	WriteData(temp);
}

void Font_Roman(void)
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= 0xFC;
	temp |= 0x02;
	WriteData(temp);
}

void Font_Bold(void) //for ASCII
{
	u8 temp;
	WriteCommand(0x2F);//FNCR1
	temp = ReadData();
	temp &= cClrD1D0;
	temp |= cSetD1D0;
	WriteData(temp);
}

//--------------------------------------------------
//REG[30h]~REG[37h]
void Active_Window(u16 XL,u16 XR ,u16 YT ,u16 YB)
{
	//setting active window X
	
	WriteCommand(0x30);//HSAW0
	WriteData(XL); 
	WriteCommand(0x31);//HSAW1	   
	WriteData(XL>>8);  //工作窗口水平起始点
	RA8875_WAITSTATUS();//	
	WriteCommand(0x34);//HEAW0
	WriteData(XR);
	WriteCommand(0x35);//HEAW1	   
	WriteData(XR>>8);  //工作窗口水平结束点
	RA8875_WAITSTATUS();//	
	//setting active window Y
	
	WriteCommand(0x32);//VSAW0
	WriteData(YT);  
	WriteCommand(0x33);//VSAW1	   
	WriteData(YT>>8);  //工作窗口垂直起始点
	RA8875_WAITSTATUS();//	
	WriteCommand(0x36);//VEAW0
	WriteData(YB); 
	WriteCommand(0x37);//VEAW1	   
	WriteData(YB>>8);  //工作窗口垂直结束点
	RA8875_WAITSTATUS();//
}
//--------------------------------------------//
//REG[38h]~REG[3Fh]

void Scroll_Window(u16 XL,u16 XR ,u16 YT ,u16 YB)
{
	WriteCommand(0x38);//HSSW0
	WriteData(XL);
	WriteCommand(0x39);//HSSW1	   
	WriteData(XL>>8);
	
	WriteCommand(0x3c);//HESW0
	WriteData(XR);  
	WriteCommand(0x3d);//HESW1	   
	WriteData(XR>>8);   
	
	WriteCommand(0x3a);//VSSW0
	WriteData(YT);
	WriteCommand(0x3b);//VSSW1	   
	WriteData(YT>>8);
	
	WriteCommand(0x3e);//VESW0
	WriteData(YB);  
	WriteCommand(0x3f);//VESW1	   
	WriteData(YB>>8);
}  


//--------------------------------------------
//内存写控制寄存器0 40H MWCR0 说明和程序 
//s7 		      s6			     s5			       s3s2			s1                  s0
//0 : 绘图模式	  0 : 光标为不显示	  0 : 游标不闪烁   00b : 		0:内存写入光标加一	0 : 内存读取光标加一
//1 : 文字模式	  1 : 光标为显示	  1	:游标闪烁	   左右然后上下	1:内存写入光标不加	1 : 内存读取光标不加
//---------------------------------------------
void Graphic_Mode(void)		//常规图形模式，内存写入光标加1 内存读取光标加1
{
	WriteCommand(0x40);//MWCR0
	WriteData(0x00);
}
void Graphic_Mode_Memery_Cursor_NoAdd(void)	  //图形模式，内存写入光标不加
{
	WriteCommand(0x40);//MWCR0
	WriteData(0x02);
}

void Text_Mode(void) //进入文字模式
{
	u8 temp;	
	WriteCommand(0x40);//MWCR0
	temp = ReadData();
	temp |= cSetD7 ;   
	WriteData(temp);	 
} 

void Text_Cursor_Disable(void) //文字光标除能	
{
	u8 temp;
	WriteCommand(0x40);//MWCR0
	temp = ReadData();
	temp &= cClrD6 ;
	WriteData(temp);	//光标不显示
}

void Text_Cursor_Enable(void)  //文字光标使能
{
	u8 temp;
	WriteCommand(0x40);//MWCR0
	temp = ReadData();
	temp |= cSetD6 ;   
	WriteData(temp);   //光标显示
} 

void Text_Cursor_Blink_Disable(void)
{
	u8 temp;
	WriteCommand(0x40);//MWCR0
	temp = ReadData();
	temp &= cClrD5 ;
	WriteData(temp);	 //光标不闪烁
}

void Text_Cursor_Blink_Enable(void)
{
	u8 temp;
	WriteCommand(0x40);//MWCR0
	temp = ReadData();
	temp |= cSetD5 ;
	WriteData(temp);   //光标闪烁
}

void Memory_Write_LeftRight_TopDown(void)
{
	u8 temp;
	WriteCommand(0x40);//MWCR0
	temp = ReadData();
	temp &= cClrD3D2 ;
	WriteData(temp);   //从左到右，从上到下写
}

void Memory_Write_RightLeft_TopDown(void)
{
	u8 temp;
	WriteCommand(0x40);//MWCR0
	temp = ReadData();
	temp &= cClrD3D2 ;
	temp |= cClrD3SetD2 ;
	WriteData(temp);
}

void Memory_Write_DownTop_LeftRight(void)
{
	u8 temp;	
	WriteCommand(0x40);//MWCR0
	temp = ReadData();
	temp &= cClrD3D2 ;
	temp |= cSetD3D2 ;
	WriteData(temp);
}

void Memory_Write_TopDown_LeftRight(void)
{
	u8 temp;
	WriteCommand(0x40);//MWCR0
	temp = ReadData();
	temp &= cClrD3D2 ;
	temp |= cSetD3ClrD2 ;
	WriteData(temp);
} 

void MemoryWrite_Cursor_autoIncrease(void)
{
	u8 temp;
	WriteCommand(0x40);//MWCR0
	temp = ReadData();
	temp &= cClrD1 ;
	WriteData(temp);   //写入光标自动加
}

void MemoryWrite_Cursor_NoautoIncrease(void)
{
	u8 temp;
	WriteCommand(0x40);//MWCR0
	temp = ReadData();
	temp |= cSetD1 ;
	WriteData(temp);   //写入光标不自动加
}

void MemoryRead_Cursor_autoIncrease(void)
{
	u8 temp;
	WriteCommand(0x40);//MWCR0
	temp = ReadData();
	temp &= cClrD0 ;
	WriteData(temp);	//读出光标自动加
}

void MemoryRead_Cursor_NoautoIncrease(void)
{
	u8 temp;
	WriteCommand(0x40);//MWCR0
	temp = ReadData();
	temp |= cSetD0 ;
	WriteData(temp);	//读出光标不自动加
}  										 

//--------------------------------------------//
//REG[41h]
void No_Graphic_Cursor(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp &= cClrD7 ;
	WriteData(temp);
}

void Graphic_Cursor(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp |= cSetD7 ;
	temp |= 0xF0 ;
	WriteData(temp);
}

void Graphic_Cursor_Set1(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp &= cClrD6D5D4;
	WriteData(temp);
}

void Graphic_Cursor_Set2(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp &= cClrD6D5D4 ;
	temp |= cClrD6D5SetD4 ;
	WriteData(temp);
}

void Graphic_Cursor_Set3(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp &= cClrD6D5D4 ;
	temp |= cClrD6SetD5ClrD4 ;
	WriteData(temp);
}

void Graphic_Cursor_Set4(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp &= cClrD6D5D4 ;
	temp |= cClrD6SetD5D4 ;
	WriteData(temp);
}

void Graphic_Cursor_Set5(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp &= cClrD6D5D4 ;
	temp |= cSetD6ClrD5D4 ;
	WriteData(temp);
}

void Graphic_Cursor_Set6(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp &= cClrD6D5D4 ;
	temp |= cSetD6ClrD5SetD4 ;
	WriteData(temp);
}

void Graphic_Cursor_Set7(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp &= cClrD6D5D4 ;
	temp |= cSetD6D5ClrD4 ;
	WriteData(temp);
}

void Graphic_Cursor_Set8(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp &= cClrD6D5D4 ;
	temp |= cSetD6D5D4 ;
	WriteData(temp);
}

void Write_To_Bank1and2(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp &= cClrD3D2 ;
	WriteData(temp);
	RA8875_WAITSTATUS();
}

void Write_To_CGRAM(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp &= 0xf3;
	temp |= cClrD3SetD2 ;
	WriteData(temp);
}

void Write_To_GraphicCursor(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp &= 0xf3;
	temp |= cSetD3ClrD2 ;
	WriteData(temp);
}

void Write_To_Pattern(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp &= 0xf3;
	temp |= cSetD3D2 ;
	WriteData(temp);
}
						   
void Write_To_Bank1(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp &= cClrD0 ;
	WriteData(temp);
}

void Write_To_Bank2(void)
{
	u8 temp;
	WriteCommand(0x41);//MWCR1
	temp = ReadData();
	temp |= cSetD0 ;
	WriteData(temp);  
}	
							
//--------------------------------------------//
//--------------------------------------------//
//REG[44h]
void Text_Blink_Time(u8 setx)
{	
	WriteCommand(0x44);//CURS
	WriteData(setx);
}

//--------------------------------------------//
//REG[45h]  
void Memory_read_LeftRight_TopDown(void)
{
	u8 temp;
	WriteCommand(0x45);//MRCD
	temp = ReadData();
	temp &= cClrD1D0;
	WriteData(temp);
}
void Memory_read_RightLeft_TopDown(void)
{
	u8 temp;
	WriteCommand(0x45);//MRCD
	temp = ReadData();
	temp |= cClrD1SetD0;
	WriteData(temp);
}
void Memory_read_TopDown_LeftRight(void)
{
	u8 temp;
	WriteCommand(0x45);//MRCD
	temp = ReadData();
	temp |= cSetD1ClrD0;
	WriteData(temp);
}
void Memory_read_DownTop_LeftRight(void)
{
	u8 temp;
	WriteCommand(0x45);//MRCD
	temp = ReadData();
	temp |= cSetD1D0;
	WriteData(temp);
}

//--------------------------------------------//
//REG[46h]~REG[49h]
void XY_Coordinate(u16 X,u16 Y)
{
	WriteCommand(0x46);
	WriteData(X);
	WriteCommand(0x47);
	WriteData(X>>8);
	
	WriteCommand(0x48);
	WriteData(Y);
	WriteCommand(0x49);
	WriteData(Y>>8);
}

//--------------------------------------------//
//REG[4Ah]~REG[4Dh]
void Memory_read_Coordinate(u16 X,u16 Y)
{
	WriteCommand(0x4a);
	WriteData(X);   
	WriteCommand(0x4b);	   
	WriteData(X>>8);
	
	WriteCommand(0x4c);
	WriteData(Y);
	WriteCommand(0x4d);	   
	WriteData(Y>>8);
} 
						  
//--------------------------------------------//
//REG[4Eh]
void Text_Cursor_Horizontal_Size(u8 setx)//u8[3:0]
{
	WriteCommand(0x4E);//CURS	   
	WriteData(setx);
} 
								
//REG[4Fh] 
void Text_Cursor_Vertical_Size(u8 setx)//u8[3:0]
{
	WriteCommand(0x4F);//CURS	   
	WriteData(setx);
} 	
	
//--------------------------------------------//
//REG[50h] 
void no_BTE_write(void)
{
	u8 temp;
	WriteCommand(0x50);//BECR0
	temp = ReadData();
	temp &= cClrD7 ;
	WriteData(temp);  
}

void BTE_enable(void)
{
	u8 temp;
	WriteCommand(0x50);//BECR0
	temp = ReadData();
	temp |= cSetD7 ;
	WriteData(temp);  
}

void BTE_contiguous_Data(void)
{
	u8 temp;
	WriteCommand(0x50);//BECR0
	temp = ReadData();
	temp |= cSetD6 ;
	WriteData(temp);  
}

void BTE_rectangular_Data(void)
{
	u8 temp;												   
	WriteCommand(0x50);//BECR0
	temp = ReadData();
	temp &= cClrD6 ;
	WriteData(temp);  
}									  

//---------------------------------------------//
//REG[51h] 
void BTE_ROP_Code(u8 setx)
{	 
	WriteCommand(0x51);//BECR1	   
	WriteData(setx); 
}									   

//---------------------------------------------//
//REG[52h] 
void Layer1_Visible(void)
{
	u8 temp;
	WriteCommand(0x52);//LTPR0
	temp = ReadData();
	temp&=0xf8;
	WriteData(temp);  
}

void Layer2_Visible(void)
{
	u8 temp;
	WriteCommand(0x52);//LTPR0
	temp = ReadData();
	temp&=0xf8;
	temp|=0x01;
	WriteData(temp);  
}	
						
void Transparent_Mode(void)
{
	u8 temp;
	WriteCommand(0x52);//LTPR0
	temp = ReadData();
	temp&=0xf8;
	temp|=0x03;
	WriteData(temp);  
}

void Lighten_Overlay_Mode(void)
{
	u8 temp;
	WriteCommand(0x52);//LTPR0
	temp = ReadData();
	temp&=0xf8;
	temp|=0x02;
	WriteData(temp);  
}	
						
void Boolean_OR(void)
{
	u8 temp;
	WriteCommand(0x52);//LTPR0
	temp = ReadData();
	temp&=0xf8;
	temp|=0x04;
	WriteData(temp);
}

void Boolean_AND(void)
{
	u8 temp;
	WriteCommand(0x52);//LTPR0
	temp = ReadData();
	temp&=0xf8;
	temp|=0x05;
	WriteData(temp);  
}  	 
	
  
void Floating_window_mode(void)
{
	u8 temp;
	WriteCommand(0x52);//LTPR0
	temp = ReadData();
	temp&=0xf8;
	temp|=0x06;
	WriteData(temp);  
} 

void Floating_Window_transparent_with_BGTR_enable(void)
{
	u8 temp;
	WriteCommand(0x52);//LTPR0
	temp = ReadData();
	temp|=cSetD5;
	WriteData(temp);  
}

void Floating_Window_transparent_with_BGTR_disable(void)
{
	u8 temp;
	WriteCommand(0x52);//LTPR0
	temp = ReadData();
	temp&=cClrD5;
	WriteData(temp);  
}


void Layer1_2_scroll(void)
{
	u8 temp;
	WriteCommand(0x52);//LTPR0
	temp = ReadData();
	temp&=0x3f;
	WriteData(temp);  
}  	 

void Layer1_scroll(void)
{
	u8 temp;
	WriteCommand(0x52);//LTPR0
	temp = ReadData();
	temp&=0x3f;
	temp|=0x40;
	WriteData(temp);  
} 

void Layer2_scroll(void)
{
	u8 temp;
	WriteCommand(0x52);//LTPR0
	temp = ReadData();
	temp&=0x3f;
	temp|=0x80;
	WriteData(temp);  
} 

void Buffer_scroll(void)
{
	u8 temp;
	WriteCommand(0x52);//LTPR0
	temp = ReadData();
	temp&=0x3f;
	temp|=0xC0;
	WriteData(temp);  
} 

//---------------------------------------------//
//REG[53h] 
void layer2_1_transparency(u8 setx)
{  
	WriteCommand(0x53);//LTPR1
	WriteData(setx); 
}

//---------------------------------------------//
//REG[54h]~ [5Fh]
void BTE_Source_Destination	(u16 XL,u16 XR ,u16 YT ,u16 YB)
{
	u8 temp,temp1;
	
	temp=XL;   
	WriteCommand(0x54);//HSBE0
	WriteData(temp);
	temp=XL>>8;   
	WriteCommand(0x55);//HSBE1	   
	WriteData(temp);

	temp=XR;   
	WriteCommand(0x58);//HDBE0
	WriteData(temp);
	temp=XR>>8;   
	WriteCommand(0x59);//HDBE1	   
	WriteData(temp); 
    
	temp=YT;   
	WriteCommand(0x56);//VSBE0
	WriteData(temp);
	temp=YT>>8;   
	WriteCommand(0x57);//VSBE1
	temp1 = ReadData();
	temp1 &= 0x80;
	temp=temp|temp1; 
	WriteCommand(0x57);//VSBE1  
	WriteData(temp);


	temp=YB;   
	WriteCommand(0x5a);//VDBE0
	WriteData(temp);
	temp=YB>>8;   
	WriteCommand(0x5b);//VDBE1
	temp1 = ReadData();
	temp1 &= 0x80;
	temp=temp|temp1;	
	WriteCommand(0x5b);//VDBE1   
	WriteData(temp);
}							

void Source_Layer1(void)
{
	u8 temp;	
	WriteCommand(0x57);//VSBE1
	temp = ReadData();
	temp &= cClrD7 ;
	WriteData(temp);
}

void Source_Layer2(void)
{
	u8 temp;	
	WriteCommand(0x57);//VSBE1
	temp = ReadData();
	temp |= cSetD7 ;
	WriteData(temp);
}

void Destination_Layer1(void)
{
	u8 temp;	
	WriteCommand(0x5b);//VDBE1
	temp = ReadData();
	temp &= cClrD7 ;
	WriteData(temp);
}

void Destination_Layer2(void)
{
	u8 temp;	
	WriteCommand(0x5b);//VDBE1
	temp = ReadData();
	temp |= cSetD7 ;
	WriteData(temp);
}
		  
//---------------------------------------------//
//---------------------------------------------//
void BTE_Size_setting(u16 X,u16 Y)
{	 
	WriteCommand(0x5c);//BEWR0
	WriteData(X); 
	WriteCommand(0x5d);//BEWR1	   
	WriteData(X>>8);
  
	WriteCommand(0x5e);//BEHR0
	WriteData(Y);   
	WriteCommand(0x5f);//BEHR1	   
	WriteData(Y>>8);
}									 

//---------------------------------------------//
//REG[60h]~REG[67h]

void Background_color(u16 color)
{
	WriteCommand(0x60);
	WriteData((color&0x001f));
		
	WriteCommand(0x61);
	WriteData((color&0x07e0)>>5);
	
	WriteCommand(0x62);
	WriteData((color&0xf800)>>11);
}

//--------------------------------------------//
void Text_Background_Color(u16 color)
{
	WriteCommand(0x60);//BGCR0
	WriteData((color&0x001f));

	WriteCommand(0x61);//BGCR0
	WriteData((color&0x07e0)>>5);
	
	WriteCommand(0x62);//BGCR0
	WriteData((color&0xf800)>>11);
	
	RA8875_WAITSTATUS();
} 

void Text_Foreground_Color(u16 color)
{	    
	WriteCommand(0x63);//BGCR0
	WriteData((color&0x001f));
	
	WriteCommand(0x64);//BGCR0
	WriteData((color&0x07e0)>>5);
	
	WriteCommand(0x65);//BGCR0
	WriteData((color&0xf800)>>11);
	RA8875_WAITSTATUS();
}


void BTE_Background_color(u8 setR,u8 setG,u8 setB)
{
	WriteCommand(0x60);//BGCR0
	WriteData(setB);
	WriteCommand(0x61);//BGCR0
	WriteData(setG);
	WriteCommand(0x62);//BGCR0
	WriteData(setR);
}


void BTE_Foreground_color(u8 setR,u8 setG,u8 setB)
{
	WriteCommand(0x63);//BGCR0
	WriteData(setB);
	WriteCommand(0x64);//BGCR0
	WriteData(setG);
	WriteCommand(0x65);//BGCR0
	WriteData(setR);
}


//======================================
//for test
void BTE_Background_red(u8 setx)
{	  
	WriteCommand(0x62);//BGCR0
	WriteData(setx);
}	
						
void BTE_Background_green(u8 setx)
{	  
	WriteCommand(0x61);//BGCR1
	WriteData(setx);
}							  

void BTE_Background_blue(u8 setx)
{	 
	WriteCommand(0x60);//BGCR2
	WriteData(setx);
} 								

void BTE_Foreground_red(u8 setx)
{	
	WriteCommand(0x65);//FGCR0
	WriteData(setx);
}								 

void BTE_Foreground_green(u8 setx)
{	  
	WriteCommand(0x64);//FGCR1
	WriteData(setx);
}							 

void BTE_Foreground_blue(u8 setx)
{  
	WriteCommand(0x63);//FGCR2
	WriteData(setx);
} 	

//======================================
	
void Pattern_Set_8x8(void)
{
	u8 temp;
	
	WriteCommand(0x66);//PTNO
	temp = ReadData();
	temp &= cClrD7;
	WriteData(temp);
}	

void Pattern_Set_16x16(void)
{
	u8  temp;

	WriteCommand(0x66);//PTNO
	temp = ReadData();
	temp |= cSetD7; 
	WriteData(temp);
}								


void Pattern_Set_number(u8 setx)
{
	u8 temp;
	
	WriteCommand(0x66);//PTNO
	temp = ReadData();
	temp &= 0x80;    
	WriteData(temp|setx);
} 	
							
void BackgroundColor_TransparentMode(u8 setR,u8 setG,u8 setB)
{
	WriteCommand(0x67);//BGTR_R
	WriteData(setR);

	WriteCommand(0x68);//BGTR_G
	WriteData(setG);

	WriteCommand(0x69);//BGTR_B
	WriteData(setB);
}	  

//REG[70h]~REG[74h]
//==============================================================================
//Subroutine:	Enable_TP 
//==============================================================================
void Enable_TP(void)
{
	u8 temp;	  
	WriteCommand(0x70);//TPCR0	
	temp = ReadData();
	temp |= cSetD7;   
	WriteData(temp);
	RA8875_WAITSTATUS();
}
//==============================================================================
//Subroutine:	Disable_TP 
//==============================================================================
void Disable_TP(void)
{
	u8 temp;
	WriteCommand(0x70);//TPCR0
	temp = ReadData();
	temp &= cClrD7;   
	WriteData(temp);
}

//==============================================================================
//Subroutine:	Enable_Touch_WakeUp
//==============================================================================
void Enable_Touch_WakeUp(void)
{
	u8 temp;
	WriteCommand(0x70);//TPCR0	
	temp = ReadData();
	temp |= cSetD3;   //cSetb3 = 00001000
	WriteData(temp);
}
//==============================================================================
//Subroutine:	Disable_Touch_WakeUp
//==============================================================================
void Disable_Touch_WakeUp(void)
{
	u8 temp;
	WriteCommand(0x70);//TPCR0	
	temp = ReadData();
	temp &= cClrD3;
	WriteData(temp);
}

//==============================================================================
//Subroutine:	TP manual or auto  mode enable
//==============================================================================
void TP_manual_mode(void)
{
	u8 temp;
  	WriteCommand(0x71);//TPCR1	
	temp = ReadData();
	temp |= cSetD6;
	WriteData(temp);
}
void TP_auto_mode(void)
{
	u8 temp;
  	WriteCommand(0x71);//TPCR1	
	temp = ReadData();
	temp &= cClrD6;//temp |= 0x3F;	AUTO MODE
	//temp |= 0x3F;               //外部基准，0x1f内部基准
	WriteData(temp);
	RA8875_WAITSTATUS();
}
//==============================================================================
//Subroutine:	TP_IDLE_mode
//==============================================================================
void TP_IDLE_mode(void)
{
	u8 temp;
	WriteCommand(0x71);//TPCR1
	temp=ReadData(); 
	temp &= 0xf0;
	WriteData(temp);
}
//==============================================================================
//Subroutine:	TP_event_mode
//==============================================================================
void TP_event_mode(void)
{
	u8 temp;
	WriteCommand(0x71);//TPCR1
	temp=ReadData(); 
	temp &= 0xf0;
	temp |= 0x01;
	WriteData(temp);
	//  WriteData(0xc1);
}
//==============================================================================
//Subroutine:	TP_latch_X
//==============================================================================
void TP_latch_X(void)
{
	u8 temp;
	WriteCommand(0x71);//TPCR1
	temp=ReadData(); 
	temp &= 0xc0;
	temp |= 0x02;
	WriteData(temp);
 }
 //==============================================================================
//Subroutine:	TP_latch_Y
//==============================================================================
void TP_latch_Y(void)
{
	u8 temp;
	WriteCommand(0x71);//TPCR1
	temp=ReadData(); 
	temp &= 0xc0;
	temp |= 0x03;
	WriteData(temp);
}
//==============================================================================
//Subroutine:	Detect_TP_X_Y coordinate 
//==============================================================================
u8 ADC_X(void)
{
	u8 temp;
	RA8875_WAITSTATUS();
	WriteCommand(0x72);//TPXH	 X_coordinate high byte
	RA8875_WAITSTATUS();
	temp=ReadData();
	return temp;
}

u8 ADC_Y(void)
{
	u8 temp;
	RA8875_WAITSTATUS();
	WriteCommand(0x73);//TPYH	  Y_coordinate high byte
	RA8875_WAITSTATUS();
	temp=ReadData();
	return temp;
}

u8 ADC_XY(void)
{
	u8 temp;
	RA8875_WAITSTATUS();
 	WriteCommand(0x74);//TPXYL	  u8[3:2] Y_coordinate low byte  u8[1:0] X_coordinate low byte 
	temp=ReadData();
	return temp;
}   



//REG[80h]~REG[83h]
void Graphic_Cursor_Coordinate(u16 X,u16 Y)
{
	WriteCommand(0x80);
	WriteData(X);  
	WriteCommand(0x81);	   
	WriteData(X>>8);
  
	WriteCommand(0x82);
	WriteData(Y);  
	WriteCommand(0x83);	   
	WriteData(Y>>8);
}

//---------------------------------//
//REG[84h]

void Graphic_Cursor_Color_0(u8 setx)
{	  
	WriteCommand(0x84);//GCC0
	WriteData(setx); 
}  									  

void Graphic_Cursor_Color_1(u8 setx)
{	 
	WriteCommand(0x85);//GCC1
	WriteData(setx); 
}  	
 	 						  
//---------------------------------------------//
//REG[8A]
void PWM1_enable(void)
{
	u8 temp;
	WriteCommand(0x8a);//MCLR
	temp = ReadData();
	temp |= 0x80 ;
	WriteData(temp);
	RA8875_WAITSTATUS();
 }

void PWM1_disable(void)
{
	u8 temp;
	WriteCommand(0x8a);//MCLR
	temp = ReadData();
	temp &= 0x7f ;
	WriteData(temp);  
 }

void PWM1_disable_level_low(void)
{
	u8 temp;
	WriteCommand(0x8a);//MCLR
	temp = ReadData();
	temp &= 0xbf ;
	WriteData(temp);  
}

void PWM1_disable_level_high(void)
{
	u8 temp;
	WriteCommand(0x8a);//MCLR
	temp = ReadData();
	temp |= 0x40 ;
	WriteData(temp);  
}

 void PWM1_fnuction_sel(void)
{
	u8 temp;
	WriteCommand(0x8a);//MCLR
	temp = ReadData();
	temp &= 0xef ;
	WriteData(temp);
	RA8875_WAITSTATUS();  
 }

 void PWM1_system_clk_out(void)
{
	u8 temp;
	WriteCommand(0x8a);//MCLR
	temp = ReadData();
	temp |= 0x10 ;
	WriteData(temp);  
 }

 void PWM1_clock_ratio(u8 setx) //u80~3 
{
	u8 temp,temp1;
	temp1= setx&0x0f;
	WriteCommand(0x8a);//MCLR
	temp = ReadData();
	temp &= 0xf0;
	temp |= temp1 ;
	WriteData(temp);
	RA8875_WAITSTATUS(); 
 }

void PWM1_duty_cycle(u8 setx) //u80~7
{
	WriteCommand(0x8b);//PTNO
	WriteData(setx);
	RA8875_WAITSTATUS();
}

void PWM2_enable(void)
{
	u8 temp;
	WriteCommand(0x8c);//MCLR
	temp = ReadData();
	temp |= cSetD7 ;
	WriteData(temp);  
 }

void PWM2_disable(void)
{
	u8 temp;
	WriteCommand(0x8c);//MCLR
	temp = ReadData();
	temp &= cClrD7 ;
	WriteData(temp);  
 }

void PWM2_disable_level_low(void)
{
	u8 temp;
	WriteCommand(0x8c);//MCLR
	temp = ReadData();
	temp &= cClrD6 ;
	WriteData(temp);  
}

void PWM2_disable_level_high(void)
{
	u8 temp;
	WriteCommand(0x8c);//MCLR
	temp = ReadData();
	temp |= cSetD6 ;
	WriteData(temp);  
}

 void PWM2_fnuction_sel(void)
{
	u8 temp;
	WriteCommand(0x8c);//MCLR
	temp = ReadData();
	temp &= cClrD4 ;
	WriteData(temp);  
 }

  void PWM2_system_clk_out(void)
{
	u8 temp;
	WriteCommand(0x8c);//MCLR
	temp = ReadData();
	temp |= cSetD4 ;
	WriteData(temp);  
 }

  void PWM2_clock_ratio(u8 setx) //u80~3 
{
	u8 temp,temp1;
	temp1= setx&0x0f;
	WriteCommand(0x8c);//MCLR
	temp = ReadData();
	temp &= 0xf0;
	temp |= temp1 ;
	WriteData(temp);  
 }
 void PWM2_duty_cycle(u8 setx) //u80~7
{  
    WriteCommand(0x8d);//PTNO
	WriteData(setx);
} 	


//---------------------------------------------//
//REG[8Eh]
void Stop_Memory_Clear(void)
{
	u8 temp;
	WriteCommand(0x8e);//MCLR
	temp = ReadData();
	temp &= cClrD7 ;
	WriteData(temp);
	RA8875_WAITSTATUS(); 
}						 
void Memory_Clear(void)
{
	u8 temp;
	WriteCommand(0x8e);//MCLR
	temp = ReadData();
	temp |= cSetD7 ;
	WriteData(temp); 
	RA8875_WAITSTATUS();
}
void Clear_Full_Window(void)
{
	u8 temp;
	WriteCommand(0x8e);//MCLR
	temp = ReadData();
	temp &= cClrD6 ;
	WriteData(temp);  
}
void Clear_Active_Window(void)
{
	u8 temp;
	WriteCommand(0x8e);//MCLR
	temp = ReadData();
	temp |= cSetD6 ;
	WriteData(temp);
	RA8875_WAITSTATUS();
}

									
//---------------------------------------------//
//REG[90h] 
void Draw_line(void)
{ 
	WriteCommand(0x90);//DCR
	WriteData(0x00);
	WriteData(0x80);
}

void Draw_square(void)
{
	WriteCommand(0x90);//DCR
	WriteData(0x10);
	WriteData(0x90);
}

void Draw_square_fill(void)
{
	WriteCommand(0x90);//DCR
	WriteData(0x10);
	WriteData(0xb0);
}

void Draw_circle(void)
{
	WriteCommand(0x90);//DCR
	WriteData(0x40);
}

void Draw_circle_fill(void)
{
	WriteCommand(0x90);//DCR
	WriteData(0x60);
}

void Draw_Triangle(void)
{
	WriteCommand(0x90);//DCR
	WriteData(0x00);
	WriteData(0x81);
}

void Draw_Triangle_fill(void)
{
	WriteCommand(0x90);//DCR
	WriteData(0x01);
	WriteData(0xA1);
}

//---------------------------------------------//
//REG[90h]~/EG[9Dh]  									
void Geometric_Coordinate(u16 XL,u16 XR ,u16 YT ,u16 YB)
{	  
	WriteCommand(0x91);//Hs
	WriteData(XL);   
	WriteCommand(0x92);	   
	WriteData(XL>>8);
	
	WriteCommand(0x95);//Vs
	WriteData(XR);  
	WriteCommand(0x96);	   
	WriteData(XR>>8);
	
	WriteCommand(0x93);
	WriteData(YT);  
	WriteCommand(0x94);	   
	WriteData(YT>>8);
	
	WriteCommand(0x97);
	WriteData(YB);   
	WriteCommand(0x98);	   
	WriteData(YB>>8);
	RA8875_WAITSTATUS();
}

void Circle_Coordinate_Radius(u16 X,u16 Y,u16 R)
{ 
	WriteCommand(0x99);
	WriteData(X);  
	WriteCommand(0x9a);	   
	WriteData(X>>8);  
	
	WriteCommand(0x9b);
	WriteData(Y); 
	WriteCommand(0x9c);	   
	WriteData(Y>>8);
	
	WriteCommand(0x9d);
	WriteData(R);
	RA8875_WAITSTATUS();
} 

//---------------------------------------------//
//REG[A0h] 
void Draw_Ellipse(void)
{
	WriteCommand(0xA0);//DCR
	WriteData(0x00);
	WriteData(0x80);
	RA8875_WAITSTATUS();
}

void Draw_Ellipse_Curve(void)
{
	u8 temp;

	WriteCommand(0xA0);//DCR
	temp = ReadData();
	temp &=0x1F;
	temp |= 0x90;
	WriteData(temp);
	RA8875_WAITSTATUS();
}

void Draw_Ellipse_Curve_Fill(void)
{
	u8 temp;
	
	WriteCommand(0xA0);//DCR
	temp = ReadData();
	temp &=0x1F;
	temp |= 0xD0;
	WriteData(temp);
	RA8875_WAITSTATUS();
}

void Draw_Ellipse_fill(void)
{
	WriteCommand(0xA0);//DCR
	WriteData(0x40);
	WriteData(0xC0);
}

void Draw_Circle_Square(void)
{
	WriteCommand(0xA0);//DCR
	WriteData(0x20);
	WriteData(0xA0);
}

void Draw_Circle_Square_fill(void)
{
	WriteCommand(0xA0);//DCR
	WriteData(0x60);
	WriteData(0xE0);
}


void Draw_Ellipse_Curve_part(u8 setx)
{
	u8 temp,temp1;
	WriteCommand(0xA0);//DCR
	temp = ReadData();
	temp1 = setx&0x03;
	temp &=0xfc;
	temp |=temp1;
	WriteData(temp);   
}


void Ellipse_Coordinate_setting(u16 X,u16 Y,u16 ELL_A,u16 ELL_B)
{

	WriteCommand(0xA5);
	WriteData(X);
	WriteCommand(0xA6);	   
	WriteData(X>>8);  
	   
	WriteCommand(0xA7);
	WriteData(Y); 
	WriteCommand(0xA8);	   
	WriteData(Y>>8);
	
	WriteCommand(0xA1);
	WriteData(ELL_A);
	WriteCommand(0xA2);
	WriteData(ELL_A>>8);
	
	WriteCommand(0xA3);
	WriteData(ELL_B);
	WriteCommand(0xA4);
	WriteData(ELL_B>>8);
} 



void Circle_Square_Coordinate_setting(u16 XL,u16 XR ,u16 YT ,u16 YB ,u16 ELL_A,u16 ELL_B)
{	    
  
	WriteCommand(0x91);
	WriteData(XL);  
	WriteCommand(0x92);	   
	WriteData(XL>>8);
	
	WriteCommand(0x95);
	WriteData(XR);   
	WriteCommand(0x96);	   
	WriteData(XR>>8);
	     
	WriteCommand(0x93);
	WriteData(YT);  
	WriteCommand(0x94);	   
	WriteData(YT>>8);
	
	WriteCommand(0x97);
	WriteData(YB);  
	WriteCommand(0x98);	   
	WriteData(YB>>8);
	
	WriteCommand(0xA1);
	WriteData(ELL_A);
	WriteCommand(0xA2);
	WriteData(ELL_A>>8);
	
	WriteCommand(0xA3);
	WriteData(ELL_B);
	WriteCommand(0xA4);
	WriteData(ELL_B>>8); 
}

 
void Draw_Triangle_3point_Coordinate(u16 P1_X,u16 P1_Y,u16 P2_X,u16 P2_Y,u16 P3_X,u16 P3_Y)
{
	WriteCommand(0x91);
	WriteData(P1_X);
	WriteCommand(0x92);	   
	WriteData(P1_X>>8);
	
	WriteCommand(0x93);
	WriteData(P1_Y);
	WriteCommand(0x94);	   
	WriteData(P1_Y>>8);
	
	WriteCommand(0x95);
	WriteData(P2_X);
	WriteCommand(0x96);	   
	WriteData(P2_X>>8);
	
	WriteCommand(0x97);
	WriteData(P2_Y);
	WriteCommand(0x98);	   
	WriteData(P2_Y>>8);
	
	WriteCommand(0xA9);
	WriteData(P3_X);
	WriteCommand(0xAA);	   
	WriteData(P3_X>>8);
	
	WriteCommand(0xAB);
	WriteData(P3_Y);
	WriteCommand(0xAC);	   
	WriteData(P3_Y>>8);   
}

//---------------------------------------------//
//REG[B0]~REG[B3]
void DMA_Start_address_setting(u32 set_address)
{ 
	WriteCommand(0xB0);
	WriteData(set_address);
	
	WriteCommand(0xB1);
	WriteData(set_address>>8);
	
	WriteCommand(0xB2);
	WriteData(set_address>>16);
	
	//WriteCommand(0xB3);
	//WriteData(set_address>>24);
}

//input pic data quantity  for example  pic size = 800*480 = 384000
void DMA_Continuous_mode_size_setting(u32 set_size)
{ 
	WriteCommand(0xB4);
	WriteData(set_size);
	
	WriteCommand(0xB6);
	WriteData(set_size>>8);
	
	WriteCommand(0xB8);
	WriteData(set_size>>16);
}

void DMA_block_mode_size_setting(u16 BWR,u16 BHR,u16 SPWR)
{
	WriteCommand(0xB4);
	WriteData(BWR);
	WriteCommand(0xB5);
	WriteData(BWR>>8);
	
	WriteCommand(0xB6);
	WriteData(BHR);
	WriteCommand(0xB7);
	WriteData(BHR>>8);
	
	WriteCommand(0xB8);
	WriteData(SPWR);
	WriteCommand(0xB9);
	WriteData(SPWR>>8);  
}

//---------------------------------------------//
//REG[BFh]
void DMA_Continuous_mode(void)
{ 
	WriteCommand(0xBF);
	WriteData(0x00);
}

void DMA_Block_mode(void)
{ 
	WriteCommand(0xBF);
	WriteData(0x02);
}

void DMA_Start_enable(void)
{
	u8 temp;
	WriteCommand(0xBF);
	temp = ReadData();
	temp |= 0x01;
	WriteData(temp);
}

//---------------------------------------------//
//REG[C0h]
void Key_Scan_Enable(void)
{
	u8 temp;
	WriteCommand(0xC0);
	temp = ReadData();
	temp |= 0x80;
	WriteData(temp);  
}


void Key_Scan_setting(u8 setx)//u8[2:0]
{
	u8 temp;
	WriteCommand(0xC0);
	temp = ReadData();
	temp &= 0xf0;
	temp|= (setx&0x07);
	WriteData(temp);  
} 

void Long_Key_Scan_enable(void)
{
	u8 temp;
	WriteCommand(0xC0);
	temp = ReadData();
	temp |= 0x40;
	WriteData(temp);
}

void Key_Scan_Wakeup_Function_Enable(void)
{
	u8 temp;
	WriteCommand(0xC1);	
	temp = ReadData();
	temp |= 0x80;	// 
	WriteData(temp);
}

void Long_Key_Timing_Adjustment(u8 setx)
{
	u8 temp,temp1;
	temp = setx & 0x1c;
	WriteCommand(0xC1);
	temp1 = ReadData();
	temp1|=temp;
	WriteData(temp1);
}

//---------------------------------------------//
//REG[D0]
void Floating_window_start_point(u16 X,u16 Y)
{ 
	WriteCommand(0xD0);
	WriteData(X);
	WriteCommand(0xD1);
	WriteData(X>>8);
	
	WriteCommand(0xD2);
	WriteData(Y);
	WriteCommand(0xD3);
	WriteData(Y>>8);  
}


void Floating_window_size(u16 width_X,u16 high_Y)
{
	WriteCommand(0xD4);
	WriteData(width_X);
	WriteCommand(0xD5);
	WriteData(width_X>>8);
	
	WriteCommand(0xD6);
	WriteData(high_Y);
	WriteCommand(0xD7);
	WriteData(high_Y>>8);
}

void Floating_window_display_point(u16 X,u16 Y)
{
	WriteCommand(0xD8);
	WriteData(X);
	WriteCommand(0xD9);
	WriteData(X>>8);
	
	WriteCommand(0xDA);
	WriteData(Y);
	WriteCommand(0xDB);
	WriteData(Y>>8);
}


//REG[E0h] 
void Serial_ROM_direct_access_mode_enable(void)
{
	WriteCommand(0xE0);
	WriteData(0x01);
}

void Serial_ROM_direct_access_mode_disable(void)
{
	WriteCommand(0xE0);
	WriteData(0x00);
}

void Direct_access_mode_Address_32u8s(u32 set_address)
{
	u8 temp;  
	temp = set_address>>24;
	WriteCommand(0xE1);
	WriteData(temp);
	temp = set_address>>16;
	WriteData(temp);
	temp = set_address>>8;
	WriteData(temp);
	temp = set_address;
	WriteData(temp);
}

void Direct_access_mode_Address_24u8s(u32 set_address)
{
	u8 temp=0;
	
	temp = set_address>>16;
	WriteCommand(0xE1);
	WriteData(temp);  
	temp = set_address>>8;
	WriteCommand(0xE1);
	WriteData(temp);   
	temp = set_address;
	WriteCommand(0xE1);
	WriteData(temp);
}

u8 Direct_access_mode_Read_Data_buffer(void)
{
	u8 Serial_Data;
	
	WriteCommand(0xE2);
	Serial_Data=ReadData(); 
	return Serial_Data;
}

//---------------------------------------------//



//---------------------------------------------//
//REG[F0h] 

void KEYSCAN_Interrupt_Enable(void)
{
	u8 temp;
	WriteCommand(0xF0);	
	temp = ReadData();
	temp |= 0x10;	// 
	WriteData(temp); 
}

void KEYSCAN_Interrupt_Disable(void)
{
	u8 temp;
	WriteCommand(0xF0);	
	temp = ReadData();
	temp &= 0x0F;	// 
	WriteData(temp); 
}


void DMA_Interrupt_Enable(void)
{
	u8 temp;
	WriteCommand(0xF0);	
	temp = ReadData();
	temp |= 0x08;	// 
	WriteData(temp); 
}

void DMA_Interrupt_Disable(void)
{
	u8 temp;
	WriteCommand(0xF0);	
	temp = ReadData();
	temp &= 0x17;	// 
	WriteData(temp); 
}

void Touch_Panel_Interrupt_Enable(void)
{
	u8 temp;
	WriteCommand(0xF0);//INTC
	temp = ReadData();
	temp |= 0x04;		 //0000 0100 开启触摸屏中断
	WriteData(temp);
	RA8875_WAITSTATUS();
}

void Touch_Panel_Interrupt_Disable(void)
{
	u8 temp;
	WriteCommand(0xF0);//INTC
	temp = ReadData(); 
	temp &= 0x1B;
	WriteData(temp);
}

void BTE_Interrupt_Enable(void)
{
	u8 temp;
	WriteCommand(0xF0);//INTC
	temp=ReadData(); 
	temp |= 0x02;
	WriteData(temp);
}

void BTE_Interrupt_Disable(void)
{
	u8 temp;
	WriteCommand(0xF0);//INTC
	temp = ReadData(); 
	temp &= 0x1D;
	WriteData(temp);
}

void Font_BTE_read_write_Interrupt_Enable(void)
{
	u8 temp;
	WriteCommand(0xF0);//INTC
	temp = ReadData(); 
	temp |= 0x01;
	WriteData(temp);
}

void Font_BTE_read_write_Interrupt_Disable(void)
{
	u8 temp;
	WriteCommand(0xF0);//INTC
	temp = ReadData(); 
	temp &= 0x1E;
	WriteData(temp);
}

//---------------------------------------------//
//REG[F1h] 
void Clear_keyscan_interrupt(void)
{
	u8 temp;
	WriteCommand(0xF1);//INTC
	temp = ReadData(); 
	temp |=cSetD4 ;
	WriteData(temp);
}

u8 Keyscan_Status(void)
{
	u8 temp;
	WriteCommand(0xF1);//INTC	
	temp = ReadData();
	if ((temp&0x10)==0x10)
	return 1;
	else 
	return 0;
}

void Clear_DMA_interrupt(void)
{
	u8 temp;
	WriteCommand(0xF1);//INTC
	temp = ReadData(); 
	temp |=cSetD3 ;
	WriteData(temp);
}

u8 DMA_Status(void)
{
	u8 temp;
	WriteCommand(0xF1);//INTC	
	temp = ReadData();
	if ((temp&0x08)==0x08)
	return 1;
	else 
	return 0;
}

void clear_TP_interrupt(void)		//WRITE 1 TO 0XF1 BIT 2
{
	u8 temp;
	RA8875_WAITSTATUS();
	WriteCommand(0xF1);//INTC
	temp=ReadData(); 
	temp |= cSetD2;
	WriteData(temp);
	RA8875_WAITSTATUS();
 }


u8 Touch_Status(void)
{
	u8 temp;
	RA8875_WAITSTATUS();//???
	WriteCommand(0xF1);//INTC	
	temp = ReadData();
	if((temp&0x04) == 0x04)
		return 1;
	else
		return 0;
}

void Clear_BTE_interrupt(void)
{
	u8 temp;
	WriteCommand(0xF1);//INTC
	temp=ReadData(); 
	temp |= cSetD1;
	WriteData(temp);
}

u8 BTE_Status(void)
{
	u8 temp;
	WriteCommand(0xF1);//INTC	
	temp = ReadData();
	if ((temp&0x02)==0x02)
		return 1;
	else 
		return 0;
}

void Clear_Font_BTE_read_write_Interrupt(void)
{
	u8 temp;
	WriteCommand(0xF1);//INTC
	temp=ReadData(); 
	temp |= cSetD0;
	WriteData(temp);
}

u8 Font_BTE_read_write_Status(void)
{
	u8 temp;
	WriteCommand(0xF1);//INTC	
	temp = ReadData();
	if ((temp&0x01)==0x01)
		return 1;
	else 
		return 0;
}



//void Chk_Wait(void)
//{
	
// while(MCU_WAIT==0);	   
//}


void Show_String(u8 *str,u8 n)
{
	Text_Mode();
	WriteCommand(0x02);
	while(*str != '\0')
	{
		WriteData(*str);
		++str;
		Chk_Busy();
		//Chk_Wait();
		delay_ms(1);
	}
	delay_ms(n);
}
//显示一个字母
void Show_WD(char *str,u8 n)
{
	RA8875_WAITSTATUS();
	Text_Mode();
	WriteCommand(0x02);
	//while(*str != '\0')
	//{
		WriteData(*str);
		//++str;
		//Chk_Busy();
		//Chk_Wait();
		//delay_ms(1);
	//}
	delay_ms(n);
}
void LCD_Reset(void)
{
	GPIOA->BRR  = GPIO_Pin_3; //RA8875 RESET pin
	delay_ms(110);
	GPIOA->BSRR  = GPIO_Pin_3;
	delay_ms(110);
}

u16 Get_X_ADC_Value_10bit(void)
{
	u16 temp,temp1;
	RA8875_WAITSTATUS();
	
	temp=0;
	temp|=ADC_X();
	temp1=(temp<<2);
	temp= temp1|((ADC_XY()&0x03));
	
	return temp;
}


u16 Get_Y_ADC_Value_10bit(void)
{
	u16 temp,temp1;
	RA8875_WAITSTATUS();
	
	temp=0;
	temp|=ADC_Y();
	temp1=(temp<<2);
	temp=temp1|((ADC_XY()>>2)&0x03);
	
	return temp;
}
void delaysometime(void)
{
	u16 i;
	for(i=0;i<65530;i++);
}

/**************************************************************************************
函数功能：清除整个LCD屏幕
输入：    目标颜色
输出：	  无
**************************************************************************************/
void LcdClear(u16 color)
{
	Text_color(color); // 设定颜色
	Geometric_Coordinate(0,479,0,271); // 设定矩形坐标
	WriteCommand(0x90);
	WriteData(0xB0);
	RA8875_WAITSTATUS();
}
void lcd_drawpoint(u16 x, u16 y, u16 color)
{
	XY_Coordinate(x,y);
	WriteCommand(0x02);//MRWC
	WriteData(color);
}												  
/**************************************************************************************
函数功能：	画水平直线
输入	：  WORD  x    		起点的横坐标
	  		WORD  y  	    起点的纵坐标
	  		WORD  width		直线长度
	  		WORD  color		颜色
输出	：	无
**************************************************************************************/

void lcd_hline(u16 x, u16 y, u16 width, u16 color)
{
	Text_color(color); // 设定颜色
	Geometric_Coordinate(x,x+width-1,y,y); // 设置水平线起始点
	WriteCommand(0x90);//写0x90寄存器
	WriteData(0x80);   //向0x90寄存器写数据
	RA8875_WAITSTATUS();
}
/**************************************************************************************
函数功能：  画垂直直线
输入	：  u16  x    	    起点的横坐标
	  		u16  y  	    起点的纵坐标
	  		u16  height		直线高度
	  		u16  color		颜色
输出	：	无
**************************************************************************************/
void lcd_vline(u16 x, u16 y, u16 height, u16 color)
{
	Text_color(color); //设定颜色
	Geometric_Coordinate(x,x,y,y+height-1); // 设置起始点坐标
	WriteCommand(0x90);
	WriteData(0x80);
	RA8875_WAITSTATUS();
}
/**************************************************************************************
函数功能：画一条斜线
输入：    u16  x1   起点的横坐标
	  u16  y1  	    起点的纵坐标
	  u16  x2		结束的横坐标
	  u16  y2		结束的纵坐标
	  u16  color		颜色
输出：	  无
**************************************************************************************/
void lcd_line(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
	Text_color(color); // 设定颜色
	Geometric_Coordinate(x1,x2,y1,y2); // 设置起始点坐标
	WriteCommand(0x90);
	WriteData(0x80);
	RA8875_WAITSTATUS();
}

/**************************************************************************************
函数功能：画矩形
输入：    矩形的左上角和右下角坐标，和目标颜色
		  x1,y1  左上角坐标
		  x2,y2  右上角坐标
		  fill	 0  画矩形框
		  		 1  矩形填充
输出：	  无
**************************************************************************************/
void LcdFillRec(u16 x1, u16 y1, u16 x2, u16 y2, u8 fill, u16 color)
{
	Text_color(color); // 设定颜色
	Geometric_Coordinate(x1,x2,y1,y2); // 设定矩形坐标
	WriteCommand(0x90);
	if (fill)
	{
		WriteData(0xB0);
	}
	else
	{
		WriteData(0x90);
	}
	RA8875_WAITSTATUS();
}

#if 1
/**************************************************************************************
函数功能：在LCD上打印一个汉字
输入：    code       字符内码
          x,y	     目标显示位置坐标
	  	  color	     字体颜色
	  	  bcolor     背景颜色
输出：	  无
**************************************************************************************/
void LcdPrintHz(u16 code,u16 x,u16 y,u16 color,u16 bcolor)
{
	//u16 i,j,ip,fcode;
	;

}

/**************************************************************************************
功能描述: 	在屏幕显示一个字符串
输    入: 	char* str       字符串指针
                u16 x,y		目标显示位置坐标
		u16 color	字体颜色
		u16 bcolor	背景颜色
		u8  mode    模式，0-透明1-非透明
输    出: 	无
**************************************************************************************/

void LcdPrintStr(char *str,u16 x,u16 y,u16 color,u16 bcolor,u8 mode)
{
	Text_Mode();
	if(mode)
	{
		Font_with_BackgroundColor();
		Text_Background_Color(bcolor);
	}
	else
	{
	  Font_with_BackgroundTransparency();
	}
	Text_Foreground_Color(color);
  Font_Coordinate(x,y);//坐标
	WriteCommand(0x02);
	while(*str)
	{
			WriteData((u8)*str++);
			RA8875_WAITSTATUS();
	}
	Graphic_Mode();
}
void LcdPrintWord(u16 *str,u16 x,u16 y,u16 color,u16 bcolor,u8 mode)
{
//	u8 i;
	Text_Mode();
	Font_with_BackgroundTransparency();
	Text_Background_Color(bcolor);
	Text_Foreground_Color(color);
  Font_Coordinate(x,y);//坐标
	while(*str)
	{
		WriteCommand(0x02);
		WriteData(*str++);
		RA8875_WAITSTATUS();
	}
}
/**************************************************************************************
功能描述: 在屏幕按格式显示一个字符串
输    入: 

输    出: 无
**************************************************************************************/
void LcdPrintf(u16 x,u16 y,u16 color,u16 bcolor,char *fmt,...)
{
    va_list ap;
    char str[64];

    va_start(ap,fmt);
    vsprintf(str,fmt,ap);
    LcdPrintStr(str,x,y,color,bcolor,1);
    va_end(ap);
}
void LcdPrintfT(u16 x,u16 y,u16 color,char *fmt,...)
{
    va_list ap;
    char str[64];

    va_start(ap,fmt);
    vsprintf(str,fmt,ap);
    LcdPrintStr(str,x,y,color,0,0);
    va_end(ap);
}
/**************************************************************************************
功能描述: 在屏幕按格式显示一个字符串
输    入: 

输    出: 无
**************************************************************************************/
void LcdPrintf0(u16 x,u16 y,u16 color,u16 bcolor,u8 mode,char *fmt,...)
{
    va_list ap;
    char str[64];

    va_start(ap,fmt);
    vsprintf(str,fmt,ap);
    LcdPrintStr(str,x,y,color,bcolor,mode);
    va_end(ap);
}

/**************************************************************************************
功能描述: 在屏幕显示一 16bit bmp图形
输    入: u16* image    	图形数据数组地址
          u16 x,y			左上角坐标
	  	  u16 width,height	图形的宽度和高度
输    出: 无
**************************************************************************************/
void LcdPrint16bitBmp(u16* image,u16 x,u16 y,u16 widht,u16 height)
{
	u16 w,h;
	
	for(h=0;h<height;h++)
	{
		XY_Coordinate(x,y+h);
		WriteCommand(0x02);		//MRWC  REG[02h] -- Memory Read/Write Command
		for(w = 0; w< widht; w++)
		{
			WriteData(*image++);
		}
	}
}

void LcdPrint8bitBmp(const u8* image,u16 x,u16 y,u16 widht,u16 height)
{
	//u8  color8;
	u16 w,h;
	//u16 r,g,b;
    
	for(h=0;h<height;h++)
	{
		XY_Coordinate(x,y+h);
		WriteCommand(0x02);		//MRWC  REG[02h] -- Memory Read/Write Command
		for(w = 0; w< widht; w++)
		{
			WriteData(*image++);			
		}
	}
}

void RA8875_LcdPrintWord(u16 x,u16 y,u16 c)
{
	Text_Mode();
	Font_with_BackgroundTransparency();
	Text_Foreground_Color(BLACK);
  Font_Coordinate(x,y);//坐标
	WriteCommand(0x02);
	if(c>=128)
	{
			WriteData(c>>8);
			RA8875_WAITSTATUS();
	}
	WriteData(c);
	RA8875_WAITSTATUS();
	Graphic_Mode();
}
#endif






#if 0
/*
*********************************************************************************************************
*	函 数 名: RA8875_DispStr
*	功能说明: 显示字符串，字符点阵来自于RA8875外接的字库芯片，支持汉字.
*			文字颜色、背景颜色、是否通透由其他的函数进行设置
*			void RA8875_SetFrontColor(uint16_t _usColor);
*			void RA8875_SetBackColor(uint16_t _usColor);
*	形    参：_usX, _usY : 显示位置（坐标）
*			 _ptr : AascII字符串，已0结束
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr)
{
	/*
		RA8875 透过使用 — 上海集通公司 (Genitop Inc) 外部串行式字体内存 (Font ROM)，可支持各样
		的文字写入到DDRAM 中。RA8875 与上海集通公司 (Genitop Inc) 兼容的产品包含 :
		GT21L16TW/GT21H16T1W 、GT23L16U2W 、GT23L24T3Y/GT23H24T3Y 、GT23L24M1Z 、
		及 GT23L32S4W/GT23H32S4W。这些字体包含16x16, 24x24, 32x32 点 (Dot) 与不同的字

		模块集成的字库芯片为 集通字库芯片_GT30l32S4W

		GT30L32S4W是一款内含12X12点阵、16X16点、24X24点阵、32X32点阵的汉字库芯片，支持GB2312
		国标汉字（含有国家信标委合法授权）及SCII字符。排列格式为横置横排。用户通过字符内码，利用本手
		册提供的方法计算出该字符点阵在芯片中的地址，可从该地址连续读出字符点阵信息。
	*/

	/* 设置文本显示位置，注意文本模式的写入光标和图形模式的写入光标是不同的寄存器 */
	Font_Coordinate(_usX, _usY);

	RA8875_WriteReg(0x40, (1 << 7));	/* 设置为文本模式 */

	/*
		Serial Flash/ROM 频率频率设定
			0xb: SFCL 频率 = 系统频率频率(当DMA 为致能状态，并且色彩深度为256 色，则SFCL 频率
				固定为=系统频率频率/ 2)
			10b: SFCL 频率 =系统频率频率/ 2
			11b: SFCL 频率 =系统频率频率/ 4

		模块系统频率为 68.75MHz

		GT30L32S4W的访问速度：SPI 时钟频率：80MHz(max.)

		因此需要设置 2 分频, 34.375MHz
	*/
	RA8875_WriteReg(0x06, 0x02);	/* 设置为文本模式 */

	/* 选择外部字体ROM */
	RA8875_WriteReg(0x21, (0 << 7) | (1 << 5));

	/* 05H  REG[05h] Serial Flash/ROM Configuration Register (SROC)
		7	Serial Flash/ROM I/F # 选择
				0:选择Serial Flash/ROM 0 接口。[模块的字库芯片接在 0 接口]
				1:选择Serial Flash/ROM 1 接口。
		6	Serial Flash/ROM 寻址模式
				0: 24 位寻址模式。
				此位必须设为0。
		5	Serial Flash/ROM 波形模式
				0: 波形模式 0。
				1: 波形模式 3。
		4-3	Serial Flash /ROM 读取周期 (Read Cycle)
			00b: 4 bus ?? 无空周期 (No Dummy Cycle)。
			01b: 5 bus ??1 byte 空周期。
			1Xb: 6 bus ??2 byte 空周期。
		2	Serial Flash /ROM 存取模式 (Access Mode)
			0: 字型模式 。
			1: DMA 模式。
		1-0	Serial Flash /ROM I/F Data Latch 选择模式
			0Xb: 单一模式。
			10b: 双倍模式0。
			11b: 双倍模式1。
	*/
	RA8875_WriteReg(0x05, (0 << 7) | (0 << 6) | (1 << 5) | (1 << 3) | (0 << 2) | (0 << 1));

	/*
		设置外部字体芯片型号为 GT30L32S4W, 编码为GB2312,

		Bit1:0 决定ASCII字符的格式：
			0 = NORMAL	 [笔画细, 和汉字顶部对齐]
			1 = Arial		 [笔画粗，和汉字底部对齐]
			2 = Roman		 [笔画细, 和汉字底部对齐]
			3 = Bold		 [乱码,不可用]
	 */
	RA8875_WriteReg(0x2F, (4 << 5) | (0 << 2) | (1 << 0));

	//s_ucRA8875Busy = 1;
	WriteCommand(0x02); 			/* 用于设定RA8875 进入内存(DDRAM或CGRAM)读取/写入模式 */

	/* 开始循环处理字符 */
	while (*_ptr != 0)
	{
		WriteData(*_ptr);
		while ((LCD_StatusRead() & 0x80) == 0x80);
		_ptr++;
	}
	//s_ucRA8875Busy = 0;

	RA8875_WriteReg(0x40, (0 << 7));	/* 还原为图形模式 */
}
#endif



