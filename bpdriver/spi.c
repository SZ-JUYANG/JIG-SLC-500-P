
#include "spi.h"

//==============================================================================
//Subroutine:	 spi_initn 
//初始化一个SPI通道
//返回值  :无   
//==============================================================================

void spi_initn(	SPI_TypeDef * spi_n)
{	 
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						//设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					//SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;							//时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;						//数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							//NSS由外部管脚管理
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//波特率预分频值为256（慢速）
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					//数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;							//定义了用于CRC值计算的多项式



	switch( (u32)spi_n )
	{
		case (u32)SPI1:  // spi 1
			RCC_APB2PeriphClockCmd(	RCC_APB2Periph_SPI1, ENABLE );	

			GPIO_InitStructure.GPIO_Pin = spi1_clk | spi1_mosi | spi1_miso;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(spi1_gpio, &GPIO_InitStructure);

			GPIO_InitStructure.GPIO_Pin = spi1_nss ;  //SPI SELECT
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
			GPIO_Init(spi1_gpio, &GPIO_InitStructure);
	       	break;
    	case (u32)SPI2:  // spi2 如果spi2_selectpin为0 选择STM32控制FLASH
		                 //      反之，spi2可以由用户自主使用 		    
			RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE );

			GPIO_InitStructure.GPIO_Pin = spi2_clk | spi2_mosi | spi2_miso;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(spi2_gpio, &GPIO_InitStructure);

			GPIO_InitStructure.GPIO_Pin = spi2_nss ;  //SPI SELECT
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
			GPIO_Init(spi2_gpio, &GPIO_InitStructure);

 			/* Configure I/O for WHO select flash 0 for mcu 1 for ra8875*/
			GPIO_InitStructure.GPIO_Pin = spi2_selectpin ;  //SPI CS  SELECT
			GPIO_Init(spi2_selectgpio, &GPIO_InitStructure);
			GPIO_ResetBits(spi2_selectgpio,spi2_selectpin);
	      	break;
		case (u32)SPI3:	 //spi3	默认为网口驱动
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	
			GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//spi3与JLINK引脚复用
			
			GPIO_InitStructure.GPIO_Pin = spi3_clk | spi3_mosi | spi3_miso;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(spi3_gpio, &GPIO_InitStructure);

			GPIO_InitStructure.GPIO_Pin = spi3_cs ;  //SPI SELECT
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
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
	SPI_Cmd(spi_n, ENABLE); //使能SPI外设
	spi_sendrece(spi_n,0xFF);	
}  

//==============================================================================
//Subroutine:	 spi_send 
//发送数据，随即接收数据
//返回值  :   0正确 1发送失败 2接收失败
//==============================================================================

u8 spi_sendrece(SPI_TypeDef * spi_n,u8 data)
	{	
	u8 rece=0;	
	u8 timeout=SPI_TIMEOUT;			 
	while (SPI_I2S_GetFlagStatus(spi_n, SPI_I2S_FLAG_TXE) == RESET) //等待SPI发送缓冲区空
	{
		if((timeout--)<1)
		{
			reporterr("spi send timeout");
			return 0;
		}			
	}			  
	SPI_I2S_SendData(spi_n, data); //通过外设SPI发送一个数据

	timeout=SPI_TIMEOUT;	
	rece=rece;
	while (SPI_I2S_GetFlagStatus(spi_n, SPI_I2S_FLAG_RXNE) == RESET) //是否接收到数据
	{
		if((timeout--)<1)
		{
			reporterr("spi rece timeout");
			return 0;
		}
	}	  						    
	return(SPI_I2S_ReceiveData(spi_n)); //接收数据
				    
}
void spi_setspeed(SPI_TypeDef * spi_n,u8 speed) 
{
	uint16_t tmpreg = 0;	
	SPI_Cmd(spi_n,DISABLE);
    tmpreg = spi_n->CR1; //取得原来的值
	tmpreg &= 0xFFC7; //低8位 1100 0111 清5-3位
	tmpreg |= speed; //
	SPI_Cmd(spi_n,ENABLE);
} 



