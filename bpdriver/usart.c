
#include "usart.h"

void usart_init()
{
	   uart1_init(9600);
 	   uart2_init(9600);
	   uart4_init(9600);
}

/**********************************************************************************************************
功能 ：	串口1  初始化
参数 :   Baud  波特率  
串口1端口: TXD PA9/RXD PA10
***********************************************************************************************************/
void uart1_init(INT32U Baud)
{
	USART_InitTypeDef    USART_InitStructure;
	GPIO_InitTypeDef     GPIO_InitStructure;
	//EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	
	USART_InitStructure.USART_BaudRate = Baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	//Enable  clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);
	//Configure USART Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Configure USART Rx as input floating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
	
	USART_GetFlagStatus(USART1, USART_FLAG_TC);//解决第一个字节发不出去

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//USART1中断
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

}
/**********************************************************************************************************
功能 ：	串口2  初始化
参数 :   Baud  波特率 
串口2端口: TXD PA2/RXD PA3
***********************************************************************************************************/
void uart2_init(INT32U Baud)
{
	USART_InitTypeDef    USART_InitStructure;
	GPIO_InitTypeDef     GPIO_InitStructure;
	//EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	
	USART_InitStructure.USART_BaudRate = Baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	//Enable  clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE)	   ;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	//Configure USART Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Configure USART Rx as input floating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	USART_Init(USART2, &USART_InitStructure);
	
	USART_Cmd(USART2, ENABLE);
	
	USART_GetFlagStatus(USART2, USART_FLAG_TC);//解决第一个字节发不出去

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		//USART1中断
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

/**********************************************************************************************************
功能 ：	串口4  初始化
参数 :   Baud  波特率 
串口4端口: TXD PC10/RXD PC11
***********************************************************************************************************/
void uart4_init(INT32U Baud)
{
	USART_InitTypeDef    USART_InitStructure;
	GPIO_InitTypeDef     GPIO_InitStructure;
	//EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	USART_InitStructure.USART_BaudRate = Baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	//Enable  clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE)	   ;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	//Configure USART Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//Configure USART Rx as input floating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	USART_Init(UART4, &USART_InitStructure);
	
	USART_Cmd(UART4, ENABLE);
	
	USART_GetFlagStatus(UART4, USART_FLAG_TC);//解决第一个字节发不出去
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;		//USART4中断
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART4
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

}


void usart_sendstring(USART_TypeDef * usartn, char * format)
{
  while(*format)
	{
	   USART_SendData(usartn,*format);                         //发送一个字节
	   while(USART_GetFlagStatus(usartn,USART_FLAG_TXE)==RESET);//检测是否发送成功  
	   format++;
	}
}
