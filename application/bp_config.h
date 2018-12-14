			

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


#define ISDEBUG 1 		//����״̬�����ڴ�ӡ���������Ϣ
#define TFT_SUPPORT 1	//4.3�紥����֧��
#define ZIKU  1         //1 ��ͨ�ֿ�(�躸���ֿ�оƬ) 0 SPI FLASH�ֿ⣨�轫�ֿ�д��SPIFLASH��
//#define PRINTF_SUPPORT 0
//==============================================================================
//����STM32������
//������Ϣ->LED
//==============================================================================
#define led_gpio GPIOC//((GPIO_TypeDef *) GPIOC_BASE)
#define led1 GPIO_Pin_6		//led1 ������GIIOC_6
#define led2 GPIO_Pin_7		//led2 ������GIIOC_7
#define led3 GPIO_Pin_8		//led3 ������GIIOC_8
#define led4 GPIO_Pin_9		//led4 ������GIIOC_9

#define beep_gpio GPIOE

#define beep GPIO_Pin_0	

//==============================================================================
//����STM32������
//����������Ϣ
//==============================================================================
//WAKE UP
#define button_wakeup_gpio GPIOA   	//WAKEUP ������GIIOA_0
#define button_wakeup  GPIO_Pin_0
#define button_wakeup_gpio_source	 GPIO_PortSourceGPIOA
#define button_wakeup_source    GPIO_PinSource0
#define button_wakeup_exti_line    EXTI_Line0
#define button_wakeup_irqn EXTI0_IRQn
//NORMAL KEY
#define button_gpio GPIOB
#define button_port_source 	GPIO_PortSourceGPIOB 
#define button_irqn          EXTI9_5_IRQn 	
#define button1  GPIO_Pin_8		  //BUTTON1 ������GIIOB_8
#define button1_source    GPIO_PinSource8
#define button1_exti_line    EXTI_Line8
#define button2  GPIO_Pin_9
#define button2_source    GPIO_PinSource9	  //BUTTON2 ������GIIOB_9
#define button2_exti_line    EXTI_Line9

//==============================================================================
//����STM32������
//IIC������Ϣ
//==============================================================================
#define iic_gpio              GPIOB
#define iic_scl               GPIO_Pin_6
#define iic_sda               GPIO_Pin_7

//����Ϊ���������Ϣ�ı����ַ���൱�ڿ��޸ĵ������ļ�
#define verify_iic_address 255 //��֤IIC�Ƿ���ȷ
#define verify_iic_value 85

#define lcdverify_address 250 //LCD�Ƿ��׼����У׼����ֵΪ86
#define lcdverify_value 88 //LCD�Ƿ��׼����У׼����ֵΪ86

#define lcdverify_address_x0 0 //��ַ0 1 0��λ 1��λ 
#define lcdverify_address_x1 2 //��ַ2 3
#define lcdverify_address_y0 4 //��ַ4 5
#define lcdverify_address_y1 6 //��ַ6 7

//==============================================================================
//����STM32������
//TFT������Ϣ
//==============================================================================
#define TFT_RESOLUTIONX 480
#define TFT_RESOLUTIONY 272

//==============================================================================
//����STM32������
//SPI������Ϣ
//SPI���䣺SPI1 TF card
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
#define spi2_selectpin GPIO_Pin_12 //PC12 0 ѡ��STM32����FLASH 1 ѡ��RA8875����FLASH

#define spi3_gpio GPIOB
#define spi3_clk  GPIO_Pin_3	//clk
#define spi3_mosi  GPIO_Pin_5	//spi1 mosi
#define spi3_miso  GPIO_Pin_4	//spi1 miso
#define spi3_cs  GPIO_Pin_11	//spi1 nss 


//==============================================================================
//����STM32������
//ADC������Ϣ
//==============================================================================
#define  ADC_TIMES   10	     			//ÿͨ����10��
#define  ADC_CHANALS  8	     			//Ϊ8��ͨ��

//#define reporterr(s) {LcdClear(WHITE);LCD_ShowString(10,10,s);}
//#define reporterrf(s1,s2) {LcdClear(WHITE);LcdPrintf(10,10,RED,WHITE,s1,s2);}
//#define lineadd if((line+=20)>260){LcdClear(WHITE);line=0;}

#define DataBuff 2048  //������������1024�ֽڵ�˫�������
#define PacketNum 1
#define    FCLK 72000000 //ʱ��Ƶ��72M
#define    OSFREQ  1000  //�ж�Ƶ��1MS��Ƶ��1000Hz


 void exti_config(void); 
 void clock_config(void);
 void HardwareInit(void);
 void bp_selfcheck(u8 type);

#endif 
/* end of __BPCONFIG_H */
