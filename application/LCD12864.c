/***************************简要介绍****************************/

/***************************************************************/	 	 			
#include <stdio.h>
#include "stm32f10x_gpio.h"
#include "utility.h"
///////////////////////////////
const unsigned char  DIS0[]  = "已经准备好！    ";
const unsigned char  DIS1[]  = "请按开始键！    ";
const unsigned char  DIS2[]  = "正在检验！      ";
const unsigned char  DIS3[]  = "正在获取ID      ";
const unsigned char  DIS4[]  = "正在读取测量参数";
const unsigned char  DIS5[]  = "控制器没响应    ";
const unsigned char  DIS6[]  = "获取ID失败      ";
const unsigned char  DIS7[]  = "正在校正输入电压";
const unsigned char  DIS8[]  = "正在校正输入电流";
const unsigned char  DIS9[]  = "正在校正输入功率";
const unsigned char  DIS10[] = "请按下一步      ";
const unsigned char  DIS11[] = "输入电压校正失败";
const unsigned char  DIS12[] = "输入电流校正失败";
const unsigned char  DIS13[] = "输入功率校正失败";
const unsigned char  DIS14[] = "正在恢复出厂设置";
const unsigned char  DIS15[] = "恢复出厂设置成功";
const unsigned char  DIS16[] = "恢复出厂设置失败";
const unsigned char  DIS17[] = "校正成功        ";
const unsigned char  DIS18[] = "读取功率计参数  ";
const unsigned char  DIS19[] = "功率计无响应    ";
const unsigned char  DIS20[] = "正在测试0~10V   ";
const unsigned char  DIS21[] = "0~10V 测试不通过";
const unsigned char  DIS22[] = "错误代码：      ";
const unsigned char  DIS23[] = "                ";
const unsigned char  DIS24[] = "按任意键返回    ";
const unsigned char  DIS25[] = "产品合格！！    ";
const unsigned char  DIS26[] = "正在组网，请等待";
const unsigned char  DIS27[] = "组网超时！！    ";
      unsigned char  ucID[17]= "产品ID:         ";
const unsigned char  DIS29[] = "恢复出厂设置？？";
const unsigned char  DIS30[] = "1:确认    2:取消";
const unsigned char  DIS31[] = "1:打印    2:返回";
const unsigned char  DIS32[] = "打印完成        ";
const unsigned char  DIS33[] = "正在打印....    ";
const unsigned char  DIS34[] = "是否打印产品ID  ";
const unsigned char  DIS35[] = "1:是  2:否      ";
const unsigned char  DIS36[] = "是否重新分配ID? ";
const unsigned char  DIS37[] = "正在分配ID..... ";
const unsigned char  DIS38[] = "分配ID失败      ";
const unsigned char  DIS39[] = "  设置测试模式  ";
const unsigned char  DIS40[] = "当前模式：自动  ";
const unsigned char  DIS41[] = "当前模式：手动  ";
const unsigned char  DIS42[] = "1:手动  2:自动  ";
const unsigned char  DIS43[] = "现模式：出厂检测";
const unsigned char  DIS44[] = "3:出厂  4:查ID  ";
unsigned char    g_ucVol[17] = "Vi:             ";  //45
unsigned char    g_ucCur[17] = "Ci:             ";;  //46
unsigned char  g_ucPower[17] = "Pi:             ";;//47
const unsigned char  DIS48[] = "    实际  测量  ";
const unsigned char  DIS49[] = "按任意键查看参数";
const unsigned char  DIS50[] = "产品未校正！！！";
const unsigned char  DIS51[] = "正在检查负载....";
const unsigned char  DIS52[] = "产品ID已经使用完";
unsigned char  g_ucZero2Ten[17] = "Vo:             ";     //53
unsigned char  g_ucPwmPercent[17] = "PWM:            ";  //54
const unsigned char  DIS55[] = "    设置  输出  ";
const unsigned char  DIS56[] = "测试空载功率。。";
      unsigned char  g_ucBar[17] = "测试进度：      ";  //57
const unsigned char  DIS58[] = " 1: 校正测试    ";
const unsigned char  DIS59[] = " 2: 开始测试    ";
const unsigned char  DIS60[] = " 3: 读取节点ID  ";
const unsigned char  DIS61[] = " 4: 恢复出厂设置";
const unsigned char  DIS62[] = "现模式：查询治具";
const unsigned char  DIS63[] = " 1: 查询ID使用  ";
const unsigned char  DIS64[] = " 2: 预留        ";
const unsigned char  DIS65[] = " 3: 预留        ";
const unsigned char  DIS66[] = " 4: 预留        ";
unsigned char  g_ucSegNum[]   = "ID段号：        ";  //67
unsigned char  g_ucSegStart[] = "起始ID：        ";  //68
unsigned char  g_ucSegEnd[]   = "结束ID：        ";  //69
unsigned char  g_ucIdNow[]    = "当前ID：        ";  //70
const unsigned char  DIS71[] = "获取LoraID失败  ";
const unsigned char  DIS72[] = "申请PLC ID失败  ";
const unsigned char  DIS73[] = "正在清零电能....";
const unsigned char  DIS74[] = "  设备码不对    ";
const unsigned char  DIS75[] = "校准0~10V & HSI ";
const unsigned char  DIS76[] = "正在校准0~10V...";
const unsigned char  DIS77[] = "正在校准HSI.....";
const unsigned char  DIS78[] = "校准0~10V 失败  ";
const unsigned char  DIS79[] = "校准HSI 失败    ";
const unsigned char  DIS80[] = " 0~10V&HSI FAIL ";
const unsigned char  DIS81[] = "  正在检测RS485 ";
const unsigned char  DIS82[] = " RS485测试失败  ";
const unsigned char  DIS83[] = "设置LORA测试模式";
const unsigned char  DIS84[] = "  正在测试LORA  ";
const unsigned char  DIS85[] = "  LORA测试失败  ";
const unsigned char  DIS86[] = "  LORA测试成功  ";
unsigned char g_ucJigLoraId[]= "JIG-ID：0x      ";  //70
const unsigned char  DIS88[] = "配置AUTO模式失败";
const unsigned char *pucDisTable[] = {DIS0,DIS1,DIS2,DIS3,DIS4,DIS5,DIS6,DIS7,DIS8,DIS9,DIS10,
          DIS11,DIS12,DIS13,DIS14,DIS15,DIS16,DIS17,DIS18,DIS19,DIS20,DIS21,DIS22,DIS23,DIS24,
          DIS25,DIS26,DIS27,ucID,DIS29,DIS30,DIS31,DIS32,DIS33,DIS34,DIS35,DIS36,DIS37,DIS38,
          DIS39,DIS40,DIS41,DIS42,DIS43,DIS44,g_ucVol,g_ucCur,g_ucPower,DIS48,DIS49,DIS50,DIS51,DIS52,
          g_ucZero2Ten,g_ucPwmPercent,DIS55,DIS56,g_ucBar,DIS58,DIS59,DIS60,DIS61,DIS62,DIS63,DIS64,DIS65,DIS66,
          g_ucSegNum, g_ucSegStart, g_ucSegEnd, g_ucIdNow,DIS71,DIS72,DIS73,DIS74,DIS75,DIS76,
          DIS77,DIS78,DIS79,DIS80,DIS81,DIS82,DIS83,DIS84,DIS85,DIS86,g_ucJigLoraId,DIS88};
///////////////////////////////
#define LCD_E_0()    GPIOD->BRR = GPIO_Pin_7
#define LCD_E_1()    GPIOD->BSRR = GPIO_Pin_7
#define LCD_WR_0()   GPIOD->BRR = GPIO_Pin_5
#define LCD_WR_1()   GPIOD->BSRR = GPIO_Pin_5
#define LCD_RS_0()   GPIOD->BRR = GPIO_Pin_4
#define LCD_RS_1()   GPIOD->BSRR = GPIO_Pin_4
#define LCD_RST_0()  GPIOA->BRR =  GPIO_Pin_0
#define LCD_RST_1()  GPIOA->BSRR = GPIO_Pin_0
//将一个字节的数据放到LCD数据总线
void Lcd_wbyte(unsigned char ucData)
{
	unsigned int uiTemp1, uiTemp2;
	unsigned int uiDataTemp;
    uiTemp1 = GPIOD->ODR;
	uiTemp1 &= 0x3ffc;
	uiDataTemp = ucData;
	uiTemp1 |= ((uiDataTemp & 0x0003)<<14); //取data的DB0,DB1
	uiTemp1 |= ((uiDataTemp & 0x000c)>>2);  //取data的DB2,DB3
	uiTemp2 = GPIOE->ODR;
	uiTemp2 &= 0xf87f;
	uiTemp2 |= ((uiDataTemp & 0x00f0)<<3); //取data的DB4~DB7
    GPIOD->ODR = uiTemp1;
	GPIOE->ODR = uiTemp2;
}
//写指令
void lcd_wcmd(unsigned char cmd)
{ 	
   LCD_RS_0();//LCD_RS = 0;
   LCD_WR_0();//LCD_RW = 0;
   LCD_E_1();//LCD_EN = 1; 
   delay(600);
   Lcd_wbyte(cmd);//LcdDataPort = cmd;
   delay(600);
   LCD_E_0();//LCD_EN = 0;  
}
//写数据
void lcd_wdat(unsigned char  dat)
{
   LCD_RS_1();//LCD_RS=1;
   LCD_WR_0();//LCD_RW=0;
   LCD_E_1();//LCD_EN=1;
   delay(600);
   Lcd_wbyte(dat);//LcdDataPort=dat; 
   delay(600);
   LCD_E_0();//LCD_EN = 0; 
}
//液晶初始化
void Lcd12864_init(void)
{ 
	LCD_E_0();//LCD_EN=0;
	LCD_RST_0();//   RST = 0;   
    delay_ms(500);  
    LCD_RST_1(); //   RST = 1;
    delay_ms(100);
    lcd_wcmd(0x30);      //基本指令操作
    delay(1);
	lcd_wcmd(0x30);      //基本指令操作
    delay(1);
	lcd_wcmd(0x08);      //基本指令操作
    delay(1);
    lcd_wcmd(0x01);      //清除LCD的显示内容
    delay(800);
	lcd_wcmd(0x06);      
    delay(5);
    lcd_wcmd(0x0C);      //显示开，关光标
    delay(5);
}
//液晶清屏
void Lcd12864_clear(void) 
{  
	unsigned char i,j; 
	 //清上半屏
	for(j=0;j<32;j++) 
	{ 
		for(i=0;i<8;i++) 
		{  
			lcd_wcmd(0x34);//扩展指令集
			//连续写两个字节完成垂直于水平的坐标
			lcd_wcmd(0x80+j);//先设定垂直地址(0-32) 
			lcd_wcmd(0x80 + i);//再设定水平地址(0-8) 
			lcd_wcmd(0x30);//基本指令集
			lcd_wdat(0x00);//lcd_wdat(0x00);
			lcd_wdat(0x00);//lcd_wdat(0x00);
		}
	}
	  //清下半屏
	for(j=0;j<32;j++) 
	{ 
		for(i=0;i<8;i++) 
		{  
			lcd_wcmd(0x34);//扩展指令集
			//连续写两个字节完成垂直于水平的坐标
			lcd_wcmd(0x80+j);//先设定垂直地址(0-32) 
			lcd_wcmd(0x88 + i);//再设定水平地址(0-8) 
			lcd_wcmd(0x30);//基本指令集
			lcd_wdat(0x00);
			lcd_wdat(0x00);
		}
	}
}
//显示字符串
void Lcd12864_string(unsigned char ucAdd,const char *p)
{
	unsigned char i;
	i = strlen(p);
	lcd_wcmd(0x30);//基本指令集
	lcd_wcmd(ucAdd);//设置地址
	//lcd_wcmd(0x30);//基本指令集
	for(;i;i--){
		lcd_wdat(*p++);
	}
}

