/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bp_config.h"
#include "usart.h"
#include "cal_test.h"
#include "os_cpu.h"
#include <ucos_ii.h>
//#include "testtouch.h"
//extern Pen_Holder Pen_Point;
extern INT8U KeyPressed;
//extern u8 backlight;
extern INT8U backlight;
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{  
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */

 void SysTick_Handler(void)
{
	//SysTick->VAL   =  (0x00);	
}

///******************************************************************************* 
//* Function Name  : EXTI_IRQHandler 
//* Description    : 按键中断
//* Designed       : 亮点
//*******************************************************************************/ 
extern u8 KeyYesPressed; //key BU2
extern u8 KeyNoPressed;  //key BU1 
void EXTI9_5_IRQHandler(void)  //按键1和2的中断服务程序 
{
	EXTI_ClearITPendingBit(button1_exti_line);  //清除EXTI8线路挂起位
	EXTI_ClearITPendingBit(button2_exti_line);  //清除EXTI9线路挂起位
}
			
void EXTI0_IRQHandler(void)
{


}

#include "PowerlineCarrier.h"
extern u8  PLC_pRecBuff[];
extern u16 PLC_RecCtn;
void USART1_IRQHandler(void)	//串口1中断服务程序
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
	{
		PLC_pRecBuff[PLC_RecCtn%PLC_BUFFSIZE] =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据	
		PLC_RecCtn++;
	}	 
}	

#define PM9805_BUFFSIZE 800
extern u8      PM9805_pRecBuff[];
extern u16     PM9805_RecCtn;
extern u8      UC_RecBuff[5];
extern u8      UC_Couter;

u8  ucLoraRecBuff[50];
u8  ucLoraRecCnt = 0;
extern unsigned char  g_ucC6000_RecBuf[]; //串口接收缓存
extern unsigned char  g_ucC6000_RecLen;
extern unsigned char  g_ucC6000_RcvFlag; //串口接收完数据标志
void USART2_IRQHandler(void)	//串口2中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
	{
		if (ucLoraRecCnt==0){
		    TIM_Cmd(TIM6, ENABLE);
        }
		TIM6->CNT = 0;
		Res = USART_ReceiveData(USART2); //读取接收到的数据
		if (ucLoraRecCnt<50){
			ucLoraRecBuff[ucLoraRecCnt] = Res;		
			ucLoraRecCnt++;
		}
	}	 		
}

void USART3_IRQHandler(void)	//串口3中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res = USART_ReceiveData(USART3);//(USART1->DR);	//读取接收到的数据	
		PM9805_pRecBuff[PM9805_RecCtn] = Res;//读取接收到的数据	
		PM9805_RecCtn++;	
	}	 
}	


void UART4_IRQHandler(void)	//串口4中断服务程序，lora通信
{
	u8 Res;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res = USART_ReceiveData(UART4);//读取接收到的数据
	}	 

}

void UART5_IRQHandler(void)	//串口5中断服务程序 GP9034
{
	u8 Res;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收中断
	{
		//UART4_Buff[UART4_Ctn%UART4BUFFSIZE] =USART_ReceiveData(UART5);//(USART1->DR);	//读取接收到的数据	
		//UART4_Ctn++;
		Res = USART_ReceiveData(UART5);
		//USART_SendData(UART5 ,Res);	
	}	 

}

void EXTI2_IRQHandler(void)
{


}


//定时器4中断服务程序	
#include "main.h"
extern volatile unsigned char g_ucTestPluseEn;
extern volatile char g_cPwm;
extern volatile float g_fPwm, g_fPwmFrequency;
void TIM4_IRQHandler(void)	   //输入捕捉，用于测量PWM占空比和频率
{ 	
	static unsigned short int s_uiCh1Value1 = 0, s_uiCh1Value2 = 0, s_uiCh2Value1 = 0, s_uiCh2Value2 = 0;
	static unsigned char s_ucCh1TrigTimes = 0;
	if (TIM4->SR & 0x0002){     //判断CC1IF
		if (g_ucTestPluseEn == 1){
			if ((TIM4->SR & 0x0200) == 0){	//判断CC1OF
                if (s_ucCh1TrigTimes < 100){				
				    s_ucCh1TrigTimes++;
				}
				if (s_ucCh1TrigTimes == 1){
					s_uiCh1Value1 = TIM4->CCR1;
					TIM4->SR &= ~0x0400;    //清除标志位
				}else if(s_ucCh1TrigTimes == 2){
					s_uiCh1Value2 = TIM4->CCR1;
				}
			}
		}
		TIM4->SR &= ~0x0002;    //清除标志位
	}
	if (TIM4->SR & 0x0004){    //判断CC2IF
		if (g_ucTestPluseEn == 1){
			if ((TIM4->SR & 0x0400) == 0){	//判断CC2OF		
				if (s_ucCh1TrigTimes == 1 && s_uiCh2Value1 == 0){
					s_uiCh2Value1 = TIM4->CCR2;
				}
			}
		}
		TIM4->SR &= ~0x0004;    //清除标志位
	}
	if (TIM4->SR & 0x0200){ //判断CC1OF
		TIM4->SR &= ~0x0200;    //清除标志位	
	}
	if (TIM4->SR & 0x0400){ //判断CC2OF
		TIM4->SR &= ~0x0400;    //清除标志位	
	}
	//更新中断
	if (TIM4->SR & 0x0001){ //判断UIF
		if (g_ucTestPluseEn == 1){
			g_ucTestPluseEn = 0;
			if (s_uiCh1Value1 < s_uiCh2Value1 && s_uiCh2Value1 < s_uiCh1Value2 && s_ucCh1TrigTimes >= 2){
				g_fPwmFrequency = 1000000/(s_uiCh1Value2 - s_uiCh1Value1);  //TIM4的时钟频率为1MHz
				g_fPwm = (float)(s_uiCh2Value1 - s_uiCh1Value1) / (float)(s_uiCh1Value2 - s_uiCh1Value1) * 100.0;
				g_cPwm = (char)g_fPwm;
			}else{
				g_cPwm = RETURN_FAIL;
			}
			s_ucCh1TrigTimes = 0;
			s_uiCh1Value1 = 0;
			s_uiCh1Value2 = 0;
			s_uiCh2Value1 = 0;
		}
		TIM4->SR &= ~0x0001;    //清除标志位	
	}
}
//定时器6中断服务程序
//10ms中断一次
void TIM6_IRQHandler(void)	   //
{  OS_CPU_SR  cpu_sr;
   unsigned char ucCnt;
   OS_ENTER_CRITICAL();
   TIM_ClearFlag(TIM6, TIM_FLAG_Update);	 //清除中断标志位
   if (ucLoraRecCnt != 0){
	   g_ucC6000_RecLen = ucLoraRecCnt;
	   for(ucCnt=0;ucCnt<g_ucC6000_RecLen;ucCnt++){
		   g_ucC6000_RecBuf[ucCnt] = ucLoraRecBuff[ucCnt];//g_ucLoraRec[ucCnt] = ucLoraRecBuff[ucCnt];
	   }
	   g_ucC6000_RcvFlag = 1;
	   ucLoraRecCnt = 0;
	   //OSTimeDlyResume(APP_TASK_TIME_PRIO);
   }
   OS_EXIT_CRITICAL();	
}

void DMA1_Channel5_IRQHandler(void)
{
;
}



/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
