
#include "spi.h"

//==============================================================================
//Subroutine:	 spi_initn 
//��ʼ��һ��SPIͨ��
//����ֵ  :��   
//==============================================================================

void spi_initn(	SPI_TypeDef * spi_n)
{	 
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						//����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					//SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;							//ʱ�����ո�
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;						//���ݲ����ڵڶ���ʱ����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							//NSS���ⲿ�ܽŹ���
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//������Ԥ��ƵֵΪ256�����٣�
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					//���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;							//����������CRCֵ����Ķ���ʽ



	switch( (u32)spi_n )
	{
		case (u32)SPI1:  // spi 1
			RCC_APB2PeriphClockCmd(	RCC_APB2Periph_SPI1, ENABLE );	

			GPIO_InitStructure.GPIO_Pin = spi1_clk | spi1_mosi | spi1_miso;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(spi1_gpio, &GPIO_InitStructure);

			GPIO_InitStructure.GPIO_Pin = spi1_nss ;  //SPI SELECT
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
			GPIO_Init(spi1_gpio, &GPIO_InitStructure);
	       	break;
    	case (u32)SPI2:  // spi2 ���spi2_selectpinΪ0 ѡ��STM32����FLASH
		                 //      ��֮��spi2�������û�����ʹ�� 		    
			RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE );

			GPIO_InitStructure.GPIO_Pin = spi2_clk | spi2_mosi | spi2_miso;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(spi2_gpio, &GPIO_InitStructure);

			GPIO_InitStructure.GPIO_Pin = spi2_nss ;  //SPI SELECT
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
			GPIO_Init(spi2_gpio, &GPIO_InitStructure);

 			/* Configure I/O for WHO select flash 0 for mcu 1 for ra8875*/
			GPIO_InitStructure.GPIO_Pin = spi2_selectpin ;  //SPI CS  SELECT
			GPIO_Init(spi2_selectgpio, &GPIO_InitStructure);
			GPIO_ResetBits(spi2_selectgpio,spi2_selectpin);
	      	break;
		case (u32)SPI3:	 //spi3	Ĭ��Ϊ��������
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	
			GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//spi3��JLINK���Ÿ���
			
			GPIO_InitStructure.GPIO_Pin = spi3_clk | spi3_mosi | spi3_miso;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(spi3_gpio, &GPIO_InitStructure);

			GPIO_InitStructure.GPIO_Pin = spi3_cs ;  //SPI SELECT
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
			GPIO_Init(spi3_gpio, &GPIO_InitStructure);

		    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
			SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
			SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//1/8 P_CLK
			SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
			 

			break;
		default:
			;
	}
	SPI_Init(spi_n, &SPI_InitStructure); 
	SPI_Cmd(spi_n, ENABLE); //ʹ��SPI����
	spi_sendrece(spi_n,0xFF);	
}  

//==============================================================================
//Subroutine:	 spi_send 
//�������ݣ��漴��������
//����ֵ  :   0��ȷ 1����ʧ�� 2����ʧ��
//==============================================================================

u8 spi_sendrece(SPI_TypeDef * spi_n,u8 data)
	{	
	u8 rece=0;	
	u8 timeout=SPI_TIMEOUT;			 
	while (SPI_I2S_GetFlagStatus(spi_n, SPI_I2S_FLAG_TXE) == RESET) //�ȴ�SPI���ͻ�������
	{
		if((timeout--)<1)
		{
			reporterr("spi send timeout");
			return 0;
		}			
	}			  
	SPI_I2S_SendData(spi_n, data); //ͨ������SPI����һ������

	timeout=SPI_TIMEOUT;	
	rece=rece;
	while (SPI_I2S_GetFlagStatus(spi_n, SPI_I2S_FLAG_RXNE) == RESET) //�Ƿ���յ�����
	{
		if((timeout--)<1)
		{
			reporterr("spi rece timeout");
			return 0;
		}
	}	  						    
	return(SPI_I2S_ReceiveData(spi_n)); //��������
				    
}
void spi_setspeed(SPI_TypeDef * spi_n,u8 speed) 
{
	uint16_t tmpreg = 0;	
	SPI_Cmd(spi_n,DISABLE);
    tmpreg = spi_n->CR1; //ȡ��ԭ����ֵ
	tmpreg &= 0xFFC7; //��8λ 1100 0111 ��5-3λ
	tmpreg |= speed; //
	SPI_Cmd(spi_n,ENABLE);
} 



