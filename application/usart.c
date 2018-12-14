#include "usart.h"
#include "main.h"

#ifndef PRODUCT_TYPE
  #error "Please define PRODUCT_TYPE!"
#endif
void uart5_init(u32 Baud);
void uart3_init( u32 bound );

void usart_init()
{
   uart1_init(115200); //GC9200¡¢NH207¡¢NEMA-RS485
   uart2_init(115200); //lora
   uart3_init(9600);   //¹¦ÂÊ¼Æ£ºPM9805
   uart4_init(115200); //Ô¤Áô
   uart5_init(9600);   //´òÓ¡»úGP9034
}

/**********************************************************************************************************
¹¦ÄÜ £º	´®¿Ú1  ³õÊ¼»¯
²ÎÊý :   Baud  ²¨ÌØÂÊ  
´®¿Ú1¶Ë¿Ú: TXD PA9/RXD PA10
***********************************************************************************************************/
void uart1_init(u32 Baud)
{
	USART_InitTypeDef    USART_InitStructure;
	GPIO_InitTypeDef     GPIO_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	USART_InitStructure.USART_BaudRate = Baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	//Enable  clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);
	//Configure USART Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//Configure USART Rx as input floating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#if PRODUCT_TYPE == 2
	//Configure USART Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	//Configure USART Rx as input floating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* ¹Ø±ÕÖØÓ³Éä */
	GPIO_PinRemapConfig(GPIO_Remap_USART1,DISABLE);
#endif
	/* ´®¿ÚÅäÖÃ */
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);
	
	USART_GetFlagStatus(USART1, USART_FLAG_TC);//½â¾öµÚÒ»¸ö×Ö½Ú·¢²»³öÈ¥

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//USART1ÖÐ¶Ï
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
	NVIC_Init(&NVIC_InitStructure);	//¸ù¾ÝNVIC_InitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯ÍâÉèNVIC¼Ä´æÆ÷USART1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

}





void uart2_init( u32 bound )
{
    GPIO_InitTypeDef GPIO_InitStructure;//I/O¿ú3õê¼»¯½á11ìå
    NVIC_InitTypeDef NVIC_InitStructure;//ÖD¶Ï3õê¼»¯½á11ìå
    USART_InitTypeDef USART_InitStructure;//'®¿ú3õê¼»¯½á11ìå  
    
    /* Enable the USART2 Pins Software Remapping */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
    
    
    /* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    	   
    USART_InitStructure.USART_BaudRate = bound;                //ò»°ãéèÖÃÎa9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//×Ö3¤Îa8Î»êy¾Y¸ñê½
    USART_InitStructure.USART_StopBits = USART_StopBits_1;     //ò»¸öí?Ö1Î»
    USART_InitStructure.USART_Parity = USART_Parity_No;        //ÎTÆæÅ¼D?ÑéÎ»
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Yá÷¿ØÖÆ
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//êÕ·?Ä?ê½      
    USART_Init(USART2, &USART_InitStructure);
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_GetFlagStatus(USART2, USART_FLAG_TC);//????,??USART_SR,???????
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    //USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	/* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
    /* Enable USART2 */
    USART_Cmd(USART2, ENABLE);
}


void uart3_init( u32 bound )
{
    GPIO_InitTypeDef GPIO_InitStructure;//I/O¿ú3õê¼»¯½á11ìå
    NVIC_InitTypeDef NVIC_InitStructure;//ÖD¶Ï3õê¼»¯½á11ìå
    USART_InitTypeDef USART_InitStructure;//'®¿ú3õê¼»¯½á11ìå  
    
    /* Enable the USART2 Pins Software Remapping */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 
    
    
    /* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
    
    USART_InitStructure.USART_BaudRate = bound;                //ò»°ãéèÖÃÎa9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//×Ö3¤Îa8Î»êy¾Y¸ñê½
    USART_InitStructure.USART_StopBits = USART_StopBits_1;     //ò»¸öí?Ö1Î»
    USART_InitStructure.USART_Parity = USART_Parity_No;        //ÎTÆæÅ¼D?ÑéÎ»
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Yá÷¿ØÖÆ
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//êÕ·?Ä?ê½   
    
    USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    //USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    /* Enable USART2 */
    USART_Cmd(USART3, ENABLE);
}


/**********************************************************************************************************
¹¦ÄÜ £º	´®¿Ú4  ³õÊ¼»¯
²ÎÊý :   Baud  ²¨ÌØÂÊ 
´®¿Ú4¶Ë¿Ú: TXD PC10/RXD PC11
***********************************************************************************************************/
void uart4_init(u32 Baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	/*UART4  TXD*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);  
    /*UART4 RX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
    USART_DeInit(UART4);
    USART_InitStructure.USART_BaudRate = Baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART4, &USART_InitStructure); 
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
    USART_Cmd(UART4, ENABLE);
	USART_ClearFlag(UART4, USART_FLAG_TC);
	USART_GetFlagStatus(UART4, USART_FLAG_TC);//????,??USART_SR,???????
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;		//USART4ÖÐ¶Ï
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
	NVIC_Init(&NVIC_InitStructure);	//¸ù¾ÝNVIC_InitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯ÍâÉèNVIC¼Ä´æÆ÷USART1

}




/**********************************************************************************************************
¹¦ÄÜ £º	´®¿Ú5  ³õÊ¼»¯
²ÎÊý :   Baud  ²¨ÌØÂÊ 
´®¿Ú4¶Ë¿Ú: TXD PC10/RXD PC11
***********************************************************************************************************/
void uart5_init(u32 Baud)
{
	USART_InitTypeDef    USART_InitStructure;
	GPIO_InitTypeDef     GPIO_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	USART_InitStructure.USART_BaudRate = Baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	//Enable  clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD, ENABLE)	   ;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	//Configure USART Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//Configure USART Rx as input floating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	USART_Init(UART5, &USART_InitStructure);
	
	USART_Cmd(UART5, ENABLE);
	
	USART_GetFlagStatus(UART5, USART_FLAG_TC);//½â¾öµÚÒ»¸ö×Ö½Ú·¢²»³öÈ¥
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;		//USART4ÖÐ¶Ï
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
	NVIC_Init(&NVIC_InitStructure);	//¸ù¾ÝNVIC_InitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯ÍâÉèNVIC¼Ä´æÆ÷USART4
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);

}


void usart_sendstring(USART_TypeDef * usartn, char * format)
{
    while(*format)
	{
	   USART_SendData(usartn,*format);                         //·¢ËÍÒ»¸ö×Ö½Ú
	   while(USART_GetFlagStatus(usartn,USART_FLAG_TXE)==RESET);//¼ì²âÊÇ·ñ·¢ËÍ³É¹¦  
	   format++;
	}
}
