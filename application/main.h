#ifndef _MAIN_H
#define _MAIN_H
#include "ucos_ii.h"

#define  JIG_ID                   ((unsigned char)(g_uiLoraID))/* 以LORA模块ID的低字节作为本治具ID */

#define  CLEAR_ENERGY_EN          1   /* 在出厂测试时，发命令清除SLC内的电能计量数据。0：关闭；1:清除 */
#define  S0_10V_CALIBRATE_EN      1   /* 校准SLC输出0~10V功能使能位，0:关闭; 1:使能 */
#define  VERSION                  2   /* 1:ID由治具自己分配；2:ID由Lora向网关申请 */
#define  FACTORY_TEST_SIGNAL_EN   1   /* 出厂测试，测试信号线（0~10V和PWM）使能。 0：不测试；1：测试 */
#define  PCBA_PRINT_EN            0   /* 测试完PCBA（测试完裸板），打印2联ID使能位：0：不打印，1：打印 */
#define  LORA_TEST_EN             1   /* LORA测试使能位；0：关闭  1：使能 */
#define  PRODUCT_TYPE             1   /* 1:SLC-500-P     2:SLC-500-HN-P (NEMA) */
#define  PWM_0_10V_EN             0   /* PWM和0~10V校正测试使能位：0：关闭  1：使能 */
#if PWM_0_10V_EN == 0 /* 不使能0~10校正 */
 #undef  S0_10V_CALIBRATE_EN
 #define S0_10V_CALIBRATE_EN      0
#endif
#if PRODUCT_TYPE == 1
 #define DEVICE_CODE              0XC9
 /* SLC-500-P没有LORA功能，必须关闭 */
 #undef  LORA_TEST_EN
 #define LORA_TEST_EN             0  
#elif PRODUCT_TYPE == 2   
 #define RESET_MASTER_MODE        2   /*复位主节点（GC9200）的方式，1：软件 2：硬件  */
 #define DEVICE_CODE              0XCA
 /* NEMA-SLC必须使能出厂测试信号线 */
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

//按键端口定义
#define  KEY_PORT              GPIOB
#define  KEY1_BIT              GPIO_Pin_3
#define  KEY2_BIT              GPIO_Pin_4
#define  KEY3_BIT              GPIO_Pin_5
#define  KEY4_BIT              GPIO_Pin_8
#define  READ_KEY(port,bit)    ((port)->IDR & (bit))
//按键值定义
#define  PRESS_NO           0
#define  PRESS_KEY1         1
#define  PRESS_KEY2         2
#define  PRESS_KEY3         3
#define  PRESS_KEY4         4
#define  PRESS_KEY1ANDKEY2  12

#define  KEYDATA_TRIG      0XFF
//led端口定义
#define  LED_PORT         GPIOB
#define  LED1_BIT         GPIO_Pin_12
#define  LED2_BIT         GPIO_Pin_13
#define  LED3_BIT         GPIO_Pin_14
//led函数定义
#define  LED1_ON          LED_PORT->BSRR = LED1_BIT
#define  LED1_OFF         LED_PORT->BRR =  LED1_BIT
#define  LED1_TURN        LED_PORT->ODR =  (LED_PORT->ODR & ~LED1_BIT) | (~(LED_PORT->ODR) & LED1_BIT) 
#define  LED2_ON          LED_PORT->BSRR = LED2_BIT
#define  LED2_OFF         LED_PORT->BRR =  LED2_BIT
#define  LED2_TURN        LED_PORT->ODR =  (LED_PORT->ODR & ~LED2_BIT) | (~(LED_PORT->ODR) & LED2_BIT) 
#define  LED3_ON          LED_PORT->BSRR = LED3_BIT
#define  LED3_OFF         LED_PORT->BRR =  LED3_BIT
#define  LED3_TURN        LED_PORT->ODR =  (LED_PORT->ODR & ~LED3_BIT) | (~(LED_PORT->ODR) & LED3_BIT) 
//继电器端口定义
#define  RELAY3_ON        GPIOA->BSRR = GPIO_Pin_11
#define  RELAY3_OFF       GPIOA->BRR =  GPIO_Pin_11
#define  RELAY2_ON        GPIOA->BSRR = GPIO_Pin_12
#define  RELAY2_OFF       GPIOA->BRR =  GPIO_Pin_12
#define  RELAY1_ON        GPIOA->BSRR = GPIO_Pin_15
#define  RELAY1_OFF       GPIOA->BRR =  GPIO_Pin_15
//继电器端口定义
#define  RELAY_LOAD_ON    GPIOE->BSRR = GPIO_Pin_0
#define  RELAY_LOAD_OFF   GPIOE->BRR =  GPIO_Pin_0
#define  NEMA_ON          GPIOE->BSRR = GPIO_Pin_1
#define  NEMA_OFF         GPIOE->BRR =  GPIO_Pin_1
#define  NEMA_RS485       GPIOE->BRR =  GPIO_Pin_2
#define  NH207_RS485      GPIOE->BSRR = GPIO_Pin_2
//蜂鸣器端口定义
#define  BEEP_ON          GPIOC->BSRR = GPIO_Pin_8
#define  BEEP_OFF         GPIOC->BRR =  GPIO_Pin_8
#define  BEEP_TURN        GPIOC->ODR =  (GPIOC->ODR & ~GPIO_Pin_8) | (~(GPIOC->ODR) & GPIO_Pin_8) 
void AnyKey2Back(void *msg);
#endif



