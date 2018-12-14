#ifndef _MAIN_H
#define _MAIN_H
#include "ucos_ii.h"

#define  JIG_ID                   ((unsigned char)(g_uiLoraID))/* ��LORAģ��ID�ĵ��ֽ���Ϊ���ξ�ID */

#define  CLEAR_ENERGY_EN          1   /* �ڳ�������ʱ�����������SLC�ڵĵ��ܼ������ݡ�0���رգ�1:��� */
#define  S0_10V_CALIBRATE_EN      1   /* У׼SLC���0~10V����ʹ��λ��0:�ر�; 1:ʹ�� */
#define  VERSION                  2   /* 1:ID���ξ��Լ����䣻2:ID��Lora���������� */
#define  FACTORY_TEST_SIGNAL_EN   1   /* �������ԣ������ź��ߣ�0~10V��PWM��ʹ�ܡ� 0�������ԣ�1������ */
#define  PCBA_PRINT_EN            0   /* ������PCBA����������壩����ӡ2��IDʹ��λ��0������ӡ��1����ӡ */
#define  LORA_TEST_EN             1   /* LORA����ʹ��λ��0���ر�  1��ʹ�� */
#define  PRODUCT_TYPE             1   /* 1:SLC-500-P     2:SLC-500-HN-P (NEMA) */
#define  PWM_0_10V_EN             0   /* PWM��0~10VУ������ʹ��λ��0���ر�  1��ʹ�� */
#if PWM_0_10V_EN == 0 /* ��ʹ��0~10У�� */
 #undef  S0_10V_CALIBRATE_EN
 #define S0_10V_CALIBRATE_EN      0
#endif
#if PRODUCT_TYPE == 1
 #define DEVICE_CODE              0XC9
 /* SLC-500-Pû��LORA���ܣ�����ر� */
 #undef  LORA_TEST_EN
 #define LORA_TEST_EN             0  
#elif PRODUCT_TYPE == 2   
 #define RESET_MASTER_MODE        2   /*��λ���ڵ㣨GC9200���ķ�ʽ��1����� 2��Ӳ��  */
 #define DEVICE_CODE              0XCA
 /* NEMA-SLC����ʹ�ܳ��������ź��� */
 #undef  FACTORY_TEST_SIGNAL_EN
 #define FACTORY_TEST_SIGNAL_EN   1 
 /**/
 #define PWM_TIMER                TIM9
 #define PWM_T                    36000
 
#endif

typedef struct
{
	OS_FLAG_GRP *pEvent;
	void*    pData;
}TestMsgStruct;

#define DEBUG_0_10V      1

#define MASTER_PHYID_1         0X06000001
#define MASTER_PHYID_2         0X12345678//0XFFFF0000
#define MASTER_PHYID_3         0X81234567//0X00000000


#define TEST_LED_FLAG          (0x00001<<0)
#define TEST_HID_FLAG          (0x00001<<1)
#define TEST_CTRL_FLAG         (0x00001<<2)

//�����˿ڶ���
#define  KEY_PORT              GPIOB
#define  KEY1_BIT              GPIO_Pin_3
#define  KEY2_BIT              GPIO_Pin_4
#define  KEY3_BIT              GPIO_Pin_5
#define  KEY4_BIT              GPIO_Pin_8
#define  READ_KEY(port,bit)    ((port)->IDR & (bit))
//����ֵ����
#define  PRESS_NO           0
#define  PRESS_KEY1         1
#define  PRESS_KEY2         2
#define  PRESS_KEY3         3
#define  PRESS_KEY4         4
#define  PRESS_KEY1ANDKEY2  12

#define  KEYDATA_TRIG      0XFF
//led�˿ڶ���
#define  LED_PORT         GPIOB
#define  LED1_BIT         GPIO_Pin_12
#define  LED2_BIT         GPIO_Pin_13
#define  LED3_BIT         GPIO_Pin_14
//led��������
#define  LED1_ON          LED_PORT->BSRR = LED1_BIT
#define  LED1_OFF         LED_PORT->BRR =  LED1_BIT
#define  LED1_TURN        LED_PORT->ODR =  (LED_PORT->ODR & ~LED1_BIT) | (~(LED_PORT->ODR) & LED1_BIT) 
#define  LED2_ON          LED_PORT->BSRR = LED2_BIT
#define  LED2_OFF         LED_PORT->BRR =  LED2_BIT
#define  LED2_TURN        LED_PORT->ODR =  (LED_PORT->ODR & ~LED2_BIT) | (~(LED_PORT->ODR) & LED2_BIT) 
#define  LED3_ON          LED_PORT->BSRR = LED3_BIT
#define  LED3_OFF         LED_PORT->BRR =  LED3_BIT
#define  LED3_TURN        LED_PORT->ODR =  (LED_PORT->ODR & ~LED3_BIT) | (~(LED_PORT->ODR) & LED3_BIT) 
//�̵����˿ڶ���
#define  RELAY3_ON        GPIOA->BSRR = GPIO_Pin_11
#define  RELAY3_OFF       GPIOA->BRR =  GPIO_Pin_11
#define  RELAY2_ON        GPIOA->BSRR = GPIO_Pin_12
#define  RELAY2_OFF       GPIOA->BRR =  GPIO_Pin_12
#define  RELAY1_ON        GPIOA->BSRR = GPIO_Pin_15
#define  RELAY1_OFF       GPIOA->BRR =  GPIO_Pin_15
//�̵����˿ڶ���
#define  RELAY_LOAD_ON    GPIOE->BSRR = GPIO_Pin_0
#define  RELAY_LOAD_OFF   GPIOE->BRR =  GPIO_Pin_0
#define  NEMA_ON          GPIOE->BSRR = GPIO_Pin_1
#define  NEMA_OFF         GPIOE->BRR =  GPIO_Pin_1
#define  NEMA_RS485       GPIOE->BRR =  GPIO_Pin_2
#define  NH207_RS485      GPIOE->BSRR = GPIO_Pin_2
//�������˿ڶ���
#define  BEEP_ON          GPIOC->BSRR = GPIO_Pin_8
#define  BEEP_OFF         GPIOC->BRR =  GPIO_Pin_8
#define  BEEP_TURN        GPIOC->ODR =  (GPIOC->ODR & ~GPIO_Pin_8) | (~(GPIOC->ODR) & GPIO_Pin_8) 
void AnyKey2Back(void *msg);
#endif



