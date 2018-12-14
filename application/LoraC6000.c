#include "stm32f10x.h"
//#include "includes.h"
#include <string.h>

#define C6000_RCV_NUM 20

extern unsigned long g_ulPlcID;
extern unsigned short int g_uiLoraID;
unsigned char g_ucC6000_RecBuf[C6000_RCV_NUM ];//串口接收缓存
unsigned char g_ucC6000_RecLen = 0;
unsigned char g_ucC6000_RcvFlag = 0;     //串口接收完数据标志

/*******************************************************************************
  * @brief  Uart4Init
  * @param  Uart4 初始化 115200
  * @retval 
*******************************************************************************/
void Uart4Init(void)
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
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure); 
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	USART_Cmd(UART4, ENABLE);
	USART_ClearFlag(UART4, USART_FLAG_TC);
	USART_GetFlagStatus(UART4, USART_FLAG_TC);//初始化时，读取USART_SR，防止首字节丢失	
}

/*******************************************************************************
  * @brief  Uart2_SendByte
  * @param  Uart2 send a byte
  * @retval 
*******************************************************************************/
void Uart2_SendByte(unsigned char byte)   //
{	 
	USART_SendData(USART2, byte);        //
	while( USART_GetFlagStatus(USART2,USART_FLAG_TC)!= SET);
}
/*******************************************************************************
  * @brief  C6000_SendString
  * @param  发送字符串
  * @retval 
*******************************************************************************/
void C6000_SendString(unsigned char *pString,unsigned char ucLen)
{
	unsigned char i;
	USART_GetFlagStatus(USART2, USART_FLAG_TC);
	for(i=0;i<ucLen;i++){
		Uart2_SendByte(pString[i]);
	}
}

/*******************************************************************************
  * @brief  CheckA5Data
  * @param  检查模块返回数据的格式
  * @retval 00:失败，其它值成功
*******************************************************************************/
unsigned char CheckA5Data(unsigned char *pucInputData,unsigned char ucDataLen)
{
    unsigned char ucLen = 0;
	unsigned char ucCs = 0;
	unsigned char i;
	ucLen = pucInputData[1];     // Lenght
	if(pucInputData[0] != 0xA5)  // Header SOF
	  return 0;
	if((ucLen + 4) != ucDataLen){ // Len
	  return 0;
	}
	if(pucInputData[ucDataLen-1] != 0x5A)  // EOF
	  return 0;
	for(i=0;i<ucLen;i++){
	  ucCs += pucInputData[2+i];
	}
	if(ucCs != pucInputData[ucDataLen-2]) //cs
	 return 0;
	return 1;
}
/*******************************************************************************
  * @brief  CheckA5Data
  * @param  检查接收网关消息
  * @retval 00:失败，其它值成功
*******************************************************************************/
unsigned char CheckReceiveNetWorkData(unsigned char *pucInputData,unsigned char ucDataLen)
{
	unsigned char ucLen = 0;
	unsigned char ucCs = 0;
	unsigned char i;
	ucLen = pucInputData[2];     // Lenght
	if(pucInputData[0] != 0x68)  // Header SOF
	  return 0;
	if((ucLen + 5) != ucDataLen){ // Len
	  return 0;
	}
	if(pucInputData[ucDataLen-1] != 0x16)  // EOF
	  return 0;
	for(i=0;i<(ucDataLen-2);i++){
	  ucCs += pucInputData[i];
	}
	if(ucCs != pucInputData[ucDataLen-2]) //cs
		return 0;
	return 1;
}

#define  INTERVAL    (50*4)
//send   A5 01 04 04 5A
//Rceive a5 03 84 01 00 85 5a  //ID 0x0001
/*******************************************************************************
  * @brief  ReadLoraC6000ID
  * @param  读Lora模块的ID
  * @retval 00:无效
*******************************************************************************/ 
unsigned short int ReadLoraC6000ID(void)
{
  unsigned char ucReadID_Cmd[5] = {0xA5,0x01,0x04,0x04,0x5A};
	unsigned char i,j;
	unsigned short int uiTempVal = 0;
	C6000_SendString(ucReadID_Cmd,5);//
  return 0;	
}

/*******************************************************************************
  * @brief  RquirePLC_ID
  * @param  向集中器请求分配ID 
  * @retval 00:失败
*******************************************************************************/ 
/**************************************************
[14:21:23.465]send A5 09 10 68 01 03 10 00 01 7D 16 20 5A (MCU->模块)
[14:21:23.582]rcv  A5 09 90 68 01 03 10 00 01 7D 16 A0 5A (模块受理OK,ignore)
[14:21:24.105]rcv  A5 02 00 01 01 5A                      (网关回应OK,ignore)
[14:21:25.080]rcv  68 81 05 10 01 00 00 02 01 16          (网关分配ID 0x01000002)
***************************************************/
unsigned int RquirePLC_ID(unsigned short int uiLoraID)
{
   //Byte7-8 ->LoraID                                                          cs1       cs2
  unsigned char ucRquireCmd[13]={0xA5,0x09,0x10,0x68,0x01,0x03,0x10,0x00,0x01,0x7D,0x16,0x20,0x5A};
  unsigned char i,j,cs = 0;
	unsigned int uiTempVal;
	ucRquireCmd[7] = (uiLoraID>>8)&0xff;
	ucRquireCmd[8] = uiLoraID&0xff;
	for(i=0;i<6;i++){
	  cs += ucRquireCmd[i+3];
	}
	ucRquireCmd[9] = cs;
	cs = 0;
	for(i=0;i<9;i++){
		cs += ucRquireCmd[i+2];
	}
	ucRquireCmd[11] = cs;
  C6000_SendString(ucRquireCmd,13);	
  return 0;		 
}


/*******************************************************************************
  * @brief  ConfirmPLC_ID
  * @param  向集中器确认分配ID
  * @retval 00:失败 01:成功
*******************************************************************************/ 
/*******************
* send A5 0A 10 68 A1 04 10 00 01 01 1F 16 64 5A (MCU->模块)
* rcv  A5 0A 90 68 A1 04 10 00 01 01 1F 16 E4 5A (模块受理OK,)
* rcv  A5 02 00 01 01 5A                         (网关回应OK,)
******************/
unsigned char ConfirmPLC_ID(unsigned short int uiLoraID,unsigned char ucResult)
{
  unsigned char ucConfirmCmd[14]={0xA5,0x0A,0x10,0x68,0xA1,0x04,0x10,0x00,0x01,0x01,0x1F,0x16,0x64,0x5A};
		//unsigned char ucRespondCmd[6] = {0xA5,0x02,0x00,0x01,0x01,0x5A};
	unsigned char i,j,cs = 0;
	ucConfirmCmd[7] = (uiLoraID>>8)&0xff;
	ucConfirmCmd[8] = uiLoraID&0xff;
	ucConfirmCmd[9] = ucResult&0x01;

	for(i=0;i<7;i++){
		cs += ucConfirmCmd[i+3];
	}
	ucConfirmCmd[10] = cs;
	cs = 0;
	for(i=0;i<10;i++){
		cs += ucConfirmCmd[i+2];
	}
	ucConfirmCmd[12] = cs;	

	for(j = 0;j < 3;j++){
		memset(g_ucC6000_RecBuf,0,C6000_RCV_NUM); 
		g_ucC6000_RcvFlag = 0;
		C6000_SendString(ucConfirmCmd,14);//
		for(i = 0;i < INTERVAL;i++){
			if(g_ucC6000_RcvFlag > 0){ //
				if(CheckReceiveNetWorkData(g_ucC6000_RecBuf,g_ucC6000_RecLen)>0){// 接收数据帧正确
					if((g_ucC6000_RecBuf[1] == 0xA1)&&(g_ucC6000_RecBuf[3] == 0x11)&&(g_ucC6000_RecBuf[4] == 0x01)){
						g_ucC6000_RcvFlag = 0;
						memset(g_ucC6000_RecBuf,0,g_ucC6000_RecLen);
						return 1;
					} 
				}
				g_ucC6000_RcvFlag = 0;
				memset(g_ucC6000_RecBuf,0,g_ucC6000_RecLen);								
			}											
			OSTimeDlyHMSM(0,0,0,20);	
		}			
	}
    return 0; 
}

/*******************************************************************************
  * @brief  CheckReciveData
  * @param  
  * @retval 00:失败 01:成功
*******************************************************************************/ 
/*******************
return：0x01：心跳；0x02：治具收到网关发来的ID；0x03：治具收到网关发来的确认分配ID
        0x11：收到LORA模块的ID。
******************/
unsigned char CheckReciveData(void)
{
	unsigned char ucRecive = 0;
	unsigned short int uiTempVal;
	unsigned long ulTempVal;
	if (g_ucC6000_RcvFlag == 1){
		if(CheckReceiveNetWorkData(g_ucC6000_RecBuf,g_ucC6000_RecLen)>0){// 校验接收到集中器的数据帧是否正确
			if((g_ucC6000_RecBuf[1] == 0x02)&&(g_ucC6000_RecBuf[3] == 0xF1)){
				ucRecive = 0X01; //心跳
			}
      if((g_ucC6000_RecBuf[1] == 0xA2)&&(g_ucC6000_RecBuf[3] == 0xF2)){
		    ulTempVal = g_ucC6000_RecBuf[4];
				ulTempVal = (ulTempVal<<8) + g_ucC6000_RecBuf[5];
				ulTempVal = (ulTempVal<<8) + g_ucC6000_RecBuf[6];
				ulTempVal = (ulTempVal<<8) + g_ucC6000_RecBuf[7];
				g_ulPlcID = ulTempVal;
				ucRecive = 0X02;
			}
      if((g_ucC6000_RecBuf[1] == 0xA2)&&(g_ucC6000_RecBuf[3] == 0xF4)){
				ucRecive = 0X03;
			}				
		}else if(CheckA5Data(g_ucC6000_RecBuf,g_ucC6000_RecLen)>0){// 校验接收Lora模块返回的数据帧是否正确
			if(g_ucC6000_RecBuf[2] == 0x84){
				uiTempVal = g_ucC6000_RecBuf[4];
				uiTempVal = (uiTempVal<<8) + g_ucC6000_RecBuf[3];
				g_uiLoraID = uiTempVal;
				ucRecive = 0X11;
			} 				
		}
		g_ucC6000_RcvFlag = 0;
		memset(g_ucC6000_RecBuf,0,g_ucC6000_RecLen);
	}
	return ucRecive;
}
/**************************************************
[14:21:23.465]send A5 09 10 68 01 03 10 00 01 7D 16 20 5A (MCU->模块)
[14:21:23.582]rcv  A5 09 90 68 01 03 10 00 01 7D 16 A0 5A (模块受理OK,ignore)
[14:21:24.105]rcv  A5 02 00 01 01 5A                      (网关回应OK,ignore)
[14:21:25.080]rcv  68 81 05 10 01 00 00 02 01 16          (网关分配ID 0x01000002)
***************************************************/
unsigned int AckMlc(unsigned short int uiLoraID, unsigned char ucCmd)
{
   //Byte7-8 ->LoraID                                                          cs1       cs2
  unsigned char ucRquireCmd[13]={0xA5,0x09,0x10,0x68,0xA2,0x03,0x10,0x00,0x01,0x7D,0x16,0x20,0x5A};
  unsigned char i,j,cs = 0;
	unsigned int uiTempVal;
	ucRquireCmd[6] = ucCmd;
	ucRquireCmd[7] = (uiLoraID>>8)&0xff;
	ucRquireCmd[8] = uiLoraID&0xff;
	for(i=0;i<6;i++){
	  cs += ucRquireCmd[i+3];
	}
	ucRquireCmd[9] = cs;
	cs = 0;
	for(i=0;i<9;i++){
		cs += ucRquireCmd[i+2];
	}
	ucRquireCmd[11] = cs;
  C6000_SendString(ucRquireCmd,13);	
  return 0;		 
}
/**************************
*流程
*1、ReadLoraC6000ID(void)//读Lora模块ID
*2、RquirePLC_ID(unsigned short int uiLoraID) //向网关请求ID
*3、ConfirmPLC_ID(unsigned short int uiLoraID,unsigned char ucResult)//向网关确认ID
*************************/

