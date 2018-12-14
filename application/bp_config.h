			

#ifndef __BPCONFIG_H
#define __BPCONFIG_H
#include "include.h"
#include <led.h>
#include <key.h>
#include "tft.h"
#include "RA8875.h"
#include "usart.h"
#include "iic.h"
#include "spi.h"
#include "spiflash.h"
#include "tfcard.h"
#include "tff.h"
#include "diskio.h"
#include "fontop.h"
#include "bptimer.h"
#include "daad.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "timer.h"
#include "ucos_ii.h"
#include "GUI.h"


#define ISDEBUG 1 		//调试状态，串口打印输出调试信息
#define TFT_SUPPORT 1	//4.3寸触摸屏支持
#define ZIKU  1         //1 高通字库(需焊接字库芯片) 0 SPI FLASH字库（需将字库写入SPIFLASH）
//#define PRINTF_SUPPORT 0
//==============================================================================
//亮点STM32开发板
//配置信息->LED
//==============================================================================
#define led_gpio GPIOC//((GPIO_TypeDef *) GPIOC_BASE)
#define led1 GPIO_Pin_6		//led1 连接在GIIOC_6
#define led2 GPIO_Pin_7		//led2 连接在GIIOC_7
#define led3 GPIO_Pin_8		//led3 连接在GIIOC_8
#define led4 GPIO_Pin_9		//led4 连接在GIIOC_9

#define beep_gpio GPIOE

#define beep GPIO_Pin_0	

//==============================================================================
//亮点STM32开发板
//按键配置信息
//==============================================================================
//WAKE UP
#define button_wakeup_gpio GPIOA   	//WAKEUP 连接在GIIOA_0
#define button_wakeup  GPIO_Pin_0
#define button_wakeup_gpio_source	 GPIO_PortSourceGPIOA
#define button_wakeup_source    GPIO_PinSource0
#define button_wakeup_exti_line    EXTI_Line0
#define button_wakeup_irqn EXTI0_IRQn
//NORMAL KEY
#define button_gpio GPIOB
#define button_port_source 	GPIO_PortSourceGPIOB 
#define button_irqn          EXTI9_5_IRQn 	
#define button1  GPIO_Pin_8		  //BUTTON1 连接在GIIOB_8
#define button1_source    GPIO_PinSource8
#define button1_exti_line    EXTI_Line8
#define button2  GPIO_Pin_9
#define button2_source    GPIO_PinSource9	  //BUTTON2 连接在GIIOB_9
#define button2_exti_line    EXTI_Line9

//==============================================================================
//亮点STM32开发板
//IIC配置信息
//==============================================================================
#define iic_gpio              GPIOB
#define iic_scl               GPIO_Pin_6
#define iic_sda               GPIO_Pin_7

//以下为软件配置信息的保存地址，相当于可修改的配置文件
#define verify_iic_address 255 //验证IIC是否正确
#define verify_iic_value 85

#define lcdverify_address 250 //LCD是否较准过，校准过的值为86
#define lcdverify_value 88 //LCD是否较准过，校准过的值为86

#define lcdverify_address_x0 0 //地址0 1 0高位 1低位 
#define lcdverify_address_x1 2 //地址2 3
#define lcdverify_address_y0 4 //地址4 5
#define lcdverify_address_y1 6 //地址6 7

//==============================================================================
//亮点STM32开发板
//TFT配置信息
//==============================================================================
#define TFT_RESOLUTIONX 480
#define TFT_RESOLUTIONY 272

//==============================================================================
//亮点STM32开发板
//SPI配置信息
//SPI分配：SPI1 TF card
//		   SPI2 spiflash
//         SPI3 net
//==============================================================================

#define spi1_gpio GPIOA
#define spi1_clk  GPIO_Pin_5	//clk
#define spi1_mosi  GPIO_Pin_7	//spi1 mosi
#define spi1_miso  GPIO_Pin_6	//spi1 miso
#define spi1_nss  GPIO_Pin_4	//spi1 nss 



#define spi2_gpio GPIOB
#define spi2_clk  GPIO_Pin_13	//clk
#define spi2_mosi  GPIO_Pin_15	//spi2 mosi
#define spi2_miso  GPIO_Pin_14	//spi2 miso
#define spi2_nss  GPIO_Pin_12	//spi2 nss 
#define spi2_selectgpio GPIOC //
#define spi2_selectpin GPIO_Pin_12 //PC12 0 选择STM32控制FLASH 1 选择RA8875控制FLASH

#define spi3_gpio GPIOB
#define spi3_clk  GPIO_Pin_3	//clk
#define spi3_mosi  GPIO_Pin_5	//spi1 mosi
#define spi3_miso  GPIO_Pin_4	//spi1 miso
#define spi3_cs  GPIO_Pin_11	//spi1 nss 


//==============================================================================
//亮点STM32开发板
//ADC配置信息
//==============================================================================
#define  ADC_TIMES   10	     			//每通道采10次
#define  ADC_CHANALS  8	     			//为8个通道

//#define reporterr(s) {LcdClear(WHITE);LCD_ShowString(10,10,s);}
//#define reporterrf(s1,s2) {LcdClear(WHITE);LcdPrintf(10,10,RED,WHITE,s1,s2);}
//#define lineadd if((line+=20)>260){LcdClear(WHITE);line=0;}

#define DataBuff 2048  //缓冲区由两个1024字节的双缓冲组成
#define PacketNum 1
#define    FCLK 72000000 //时钟频率72M
#define    OSFREQ  1000  //中断频率1MS，频率1000Hz


 void exti_config(void); 
 void clock_config(void);
 void HardwareInit(void);
 void bp_selfcheck(u8 type);

#endif 
/* end of __BPCONFIG_H */
