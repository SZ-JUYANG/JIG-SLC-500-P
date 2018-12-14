/***************************��Ҫ����****************************/

/***************************************************************/	 	 			
#include <stdio.h>
#include "stm32f10x_gpio.h"
#include "utility.h"
///////////////////////////////
const unsigned char  DIS0[]  = "�Ѿ�׼���ã�    ";
const unsigned char  DIS1[]  = "�밴��ʼ����    ";
const unsigned char  DIS2[]  = "���ڼ��飡      ";
const unsigned char  DIS3[]  = "���ڻ�ȡID      ";
const unsigned char  DIS4[]  = "���ڶ�ȡ��������";
const unsigned char  DIS5[]  = "������û��Ӧ    ";
const unsigned char  DIS6[]  = "��ȡIDʧ��      ";
const unsigned char  DIS7[]  = "����У�������ѹ";
const unsigned char  DIS8[]  = "����У���������";
const unsigned char  DIS9[]  = "����У�����빦��";
const unsigned char  DIS10[] = "�밴��һ��      ";
const unsigned char  DIS11[] = "�����ѹУ��ʧ��";
const unsigned char  DIS12[] = "�������У��ʧ��";
const unsigned char  DIS13[] = "���빦��У��ʧ��";
const unsigned char  DIS14[] = "���ڻָ���������";
const unsigned char  DIS15[] = "�ָ��������óɹ�";
const unsigned char  DIS16[] = "�ָ���������ʧ��";
const unsigned char  DIS17[] = "У���ɹ�        ";
const unsigned char  DIS18[] = "��ȡ���ʼƲ���  ";
const unsigned char  DIS19[] = "���ʼ�����Ӧ    ";
const unsigned char  DIS20[] = "���ڲ���0~10V   ";
const unsigned char  DIS21[] = "0~10V ���Բ�ͨ��";
const unsigned char  DIS22[] = "������룺      ";
const unsigned char  DIS23[] = "                ";
const unsigned char  DIS24[] = "�����������    ";
const unsigned char  DIS25[] = "��Ʒ�ϸ񣡣�    ";
const unsigned char  DIS26[] = "������������ȴ�";
const unsigned char  DIS27[] = "������ʱ����    ";
      unsigned char  ucID[17]= "��ƷID:         ";
const unsigned char  DIS29[] = "�ָ��������ã���";
const unsigned char  DIS30[] = "1:ȷ��    2:ȡ��";
const unsigned char  DIS31[] = "1:��ӡ    2:����";
const unsigned char  DIS32[] = "��ӡ���        ";
const unsigned char  DIS33[] = "���ڴ�ӡ....    ";
const unsigned char  DIS34[] = "�Ƿ��ӡ��ƷID  ";
const unsigned char  DIS35[] = "1:��  2:��      ";
const unsigned char  DIS36[] = "�Ƿ����·���ID? ";
const unsigned char  DIS37[] = "���ڷ���ID..... ";
const unsigned char  DIS38[] = "����IDʧ��      ";
const unsigned char  DIS39[] = "  ���ò���ģʽ  ";
const unsigned char  DIS40[] = "��ǰģʽ���Զ�  ";
const unsigned char  DIS41[] = "��ǰģʽ���ֶ�  ";
const unsigned char  DIS42[] = "1:�ֶ�  2:�Զ�  ";
const unsigned char  DIS43[] = "��ģʽ���������";
const unsigned char  DIS44[] = "3:����  4:��ID  ";
unsigned char    g_ucVol[17] = "Vi:             ";  //45
unsigned char    g_ucCur[17] = "Ci:             ";;  //46
unsigned char  g_ucPower[17] = "Pi:             ";;//47
const unsigned char  DIS48[] = "    ʵ��  ����  ";
const unsigned char  DIS49[] = "��������鿴����";
const unsigned char  DIS50[] = "��ƷδУ��������";
const unsigned char  DIS51[] = "���ڼ�鸺��....";
const unsigned char  DIS52[] = "��ƷID�Ѿ�ʹ����";
unsigned char  g_ucZero2Ten[17] = "Vo:             ";     //53
unsigned char  g_ucPwmPercent[17] = "PWM:            ";  //54
const unsigned char  DIS55[] = "    ����  ���  ";
const unsigned char  DIS56[] = "���Կ��ع��ʡ���";
      unsigned char  g_ucBar[17] = "���Խ��ȣ�      ";  //57
const unsigned char  DIS58[] = " 1: У������    ";
const unsigned char  DIS59[] = " 2: ��ʼ����    ";
const unsigned char  DIS60[] = " 3: ��ȡ�ڵ�ID  ";
const unsigned char  DIS61[] = " 4: �ָ���������";
const unsigned char  DIS62[] = "��ģʽ����ѯ�ξ�";
const unsigned char  DIS63[] = " 1: ��ѯIDʹ��  ";
const unsigned char  DIS64[] = " 2: Ԥ��        ";
const unsigned char  DIS65[] = " 3: Ԥ��        ";
const unsigned char  DIS66[] = " 4: Ԥ��        ";
unsigned char  g_ucSegNum[]   = "ID�κţ�        ";  //67
unsigned char  g_ucSegStart[] = "��ʼID��        ";  //68
unsigned char  g_ucSegEnd[]   = "����ID��        ";  //69
unsigned char  g_ucIdNow[]    = "��ǰID��        ";  //70
const unsigned char  DIS71[] = "��ȡLoraIDʧ��  ";
const unsigned char  DIS72[] = "����PLC IDʧ��  ";
const unsigned char  DIS73[] = "�����������....";
const unsigned char  DIS74[] = "  �豸�벻��    ";
const unsigned char  DIS75[] = "У׼0~10V & HSI ";
const unsigned char  DIS76[] = "����У׼0~10V...";
const unsigned char  DIS77[] = "����У׼HSI.....";
const unsigned char  DIS78[] = "У׼0~10V ʧ��  ";
const unsigned char  DIS79[] = "У׼HSI ʧ��    ";
const unsigned char  DIS80[] = " 0~10V&HSI FAIL ";
const unsigned char  DIS81[] = "  ���ڼ��RS485 ";
const unsigned char  DIS82[] = " RS485����ʧ��  ";
const unsigned char  DIS83[] = "����LORA����ģʽ";
const unsigned char  DIS84[] = "  ���ڲ���LORA  ";
const unsigned char  DIS85[] = "  LORA����ʧ��  ";
const unsigned char  DIS86[] = "  LORA���Գɹ�  ";
unsigned char g_ucJigLoraId[]= "JIG-ID��0x      ";  //70
const unsigned char  DIS88[] = "����AUTOģʽʧ��";
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
//��һ���ֽڵ����ݷŵ�LCD��������
void Lcd_wbyte(unsigned char ucData)
{
	unsigned int uiTemp1, uiTemp2;
	unsigned int uiDataTemp;
    uiTemp1 = GPIOD->ODR;
	uiTemp1 &= 0x3ffc;
	uiDataTemp = ucData;
	uiTemp1 |= ((uiDataTemp & 0x0003)<<14); //ȡdata��DB0,DB1
	uiTemp1 |= ((uiDataTemp & 0x000c)>>2);  //ȡdata��DB2,DB3
	uiTemp2 = GPIOE->ODR;
	uiTemp2 &= 0xf87f;
	uiTemp2 |= ((uiDataTemp & 0x00f0)<<3); //ȡdata��DB4~DB7
    GPIOD->ODR = uiTemp1;
	GPIOE->ODR = uiTemp2;
}
//дָ��
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
//д����
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
//Һ����ʼ��
void Lcd12864_init(void)
{ 
	LCD_E_0();//LCD_EN=0;
	LCD_RST_0();//   RST = 0;   
    delay_ms(500);  
    LCD_RST_1(); //   RST = 1;
    delay_ms(100);
    lcd_wcmd(0x30);      //����ָ�����
    delay(1);
	lcd_wcmd(0x30);      //����ָ�����
    delay(1);
	lcd_wcmd(0x08);      //����ָ�����
    delay(1);
    lcd_wcmd(0x01);      //���LCD����ʾ����
    delay(800);
	lcd_wcmd(0x06);      
    delay(5);
    lcd_wcmd(0x0C);      //��ʾ�����ع��
    delay(5);
}
//Һ������
void Lcd12864_clear(void) 
{  
	unsigned char i,j; 
	 //���ϰ���
	for(j=0;j<32;j++) 
	{ 
		for(i=0;i<8;i++) 
		{  
			lcd_wcmd(0x34);//��չָ�
			//����д�����ֽ���ɴ�ֱ��ˮƽ������
			lcd_wcmd(0x80+j);//���趨��ֱ��ַ(0-32) 
			lcd_wcmd(0x80 + i);//���趨ˮƽ��ַ(0-8) 
			lcd_wcmd(0x30);//����ָ�
			lcd_wdat(0x00);//lcd_wdat(0x00);
			lcd_wdat(0x00);//lcd_wdat(0x00);
		}
	}
	  //���°���
	for(j=0;j<32;j++) 
	{ 
		for(i=0;i<8;i++) 
		{  
			lcd_wcmd(0x34);//��չָ�
			//����д�����ֽ���ɴ�ֱ��ˮƽ������
			lcd_wcmd(0x80+j);//���趨��ֱ��ַ(0-32) 
			lcd_wcmd(0x88 + i);//���趨ˮƽ��ַ(0-8) 
			lcd_wcmd(0x30);//����ָ�
			lcd_wdat(0x00);
			lcd_wdat(0x00);
		}
	}
}
//��ʾ�ַ���
void Lcd12864_string(unsigned char ucAdd,const char *p)
{
	unsigned char i;
	i = strlen(p);
	lcd_wcmd(0x30);//����ָ�
	lcd_wcmd(ucAdd);//���õ�ַ
	//lcd_wcmd(0x30);//����ָ�
	for(;i;i--){
		lcd_wdat(*p++);
	}
}

