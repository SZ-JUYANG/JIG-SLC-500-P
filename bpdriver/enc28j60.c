

#include "Enc28j60.h"

static u8 Enc28j60Bank;
static u32 NextPacketPtr;	

//==============================================================================
//初始化SPI3 
//==============================================================================
void Enc28j60Spi3Init(void)
{
   	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
   	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE );//PORTB,D,G,SPI3时钟使能 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//spi3与JLINK引脚复用 ，因此需禁止JTAG
 			
	GPIO_InitStructure.GPIO_Pin = spi3_clk | spi3_mosi | spi3_miso;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(spi3_gpio, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = spi3_cs ;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //NSS信号推挽输出
	GPIO_Init(spi3_gpio, &GPIO_InitStructure);
	GPIO_SetBits(spi3_gpio,spi3_cs);


	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  //SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		   //串行同步时钟的空闲状态为低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	   //串行同步时钟的第一个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		   //NSS信号由软件管理
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;	//定义波特率预分频的值:波特率预分频值为2
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式 (dm: 7为复位值)
	SPI_Init(SPI3, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPI3寄存器
 
	SPI_Cmd(SPI3, ENABLE); //使能SPI外设
	
	spi_sendrece(SPI3,0xFF);//启动传输		 
}

//==============================================================================
//读取Enc28j60寄存器 
//op 操作码  
//address 地址
//返回值  :读到的数据  
//==============================================================================
u8 Enc28j60ReadOp(u8 op, u8 address)
{
    u8 dat = 0;
    
    Enc28j60_CSL();    
    dat = op | (address & ADDR_MASK);  //#define ADDR_MASK        0x1F	  后5位

    spi_sendrece(SPI3,dat);
    dat = spi_sendrece(SPI3,0xFF);
    if(address & 0x80)
    {
        dat = spi_sendrece(SPI3,0xFF);
    }
    Enc28j60_CSH();
    return dat;
}


//==============================================================================
//写入Enc28j60寄存器 
//op 操作码  
//address 地址
//data 要写入的数据 
//==============================================================================
void Enc28j60WriteOp(u8 op, u8 address, u8 data)
{
    u8 dat = 0;
    
    Enc28j60_CSL();
    // issue write command
    dat = op | (address & ADDR_MASK);	// #define ADDR_MASK  0x1F  00011111 五位地址
    spi_sendrece(SPI3,dat); 
    // write data
    dat = data;
    spi_sendrece(SPI3,dat);
    Enc28j60_CSH();
}
//Enc28j60ReadBuffer 长度 指针
//读缓冲区
//

//==============================================================================
//读Enc28j60缓冲区 
//len 读取的长度 
//data 目标地址 
//==============================================================================
void Enc28j60ReadBuffer(u32 len, u8* data)
{
    Enc28j60_CSL();
    // issue read command
    spi_sendrece(SPI3,Enc28j60_READ_BUF_MEM); //发送一次读缓冲区指令
    while(len)
    {
        len--;
        // read data
        *data = (u8)spi_sendrece(SPI3,0); 
        data++;
    }
    *data='\0';
    Enc28j60_CSH();
}

//==============================================================================
//写Enc28j60缓冲区 
//len 写入的长度 
//data 要写的内容的首地址 
//==============================================================================
void Enc28j60WriteBuffer(u32 len, u8* data)
{
    Enc28j60_CSL();
    // issue write command
    spi_sendrece(SPI3,Enc28j60_WRITE_BUF_MEM);//发送一次写缓冲区指令
    
    while(len)
    {
        len--;
        spi_sendrece(SPI3,*data);
        data++;
    }
    Enc28j60_CSH();
}
//==============================================================================
//根据地址设置Enc28j60地址段
//address 地址 
//==============================================================================
void Enc28j60SetBank(u8 address)
{
    // set the bank (if needed)
    if((address & BANK_MASK) != Enc28j60Bank)
	
    {
        Enc28j60WriteOp(Enc28j60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
        Enc28j60WriteOp(Enc28j60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
        Enc28j60Bank = (address & BANK_MASK);
    }
}
//==============================================================================
//读取Enc28j60寄存器 
//address 地址 
//==============================================================================
u8 Enc28j60Read(u8 address)	  //读寄存器
{
    Enc28j60SetBank(address);
    return Enc28j60ReadOp(Enc28j60_READ_CTRL_REG, address);
}

//==============================================================================
//写Enc28j60寄存器 
//address 地址 
//data 数据
//==============================================================================
void Enc28j60Write(u8 address, u8 data)
{
    
    Enc28j60SetBank(address);  //段设置
    Enc28j60WriteOp(Enc28j60_WRITE_CTRL_REG, address, data); //写寄存器
}									

//==============================================================================
//向Enc28j60的PHY寄存器写数据 
//address 地址 
//data 数据
//==============================================================================
void Enc28j60PhyWrite(u8 address, u32 data)
{
    // set the PHY register address
    Enc28j60Write(MIREGADR, address); //MIREGADR  MII 寄存器地址
    // write the PHY data
    Enc28j60Write(MIWRL, data);
    Enc28j60Write(MIWRH, data>>8);
    // wait until the PHY write completes
    while(Enc28j60Read(MISTAT) & MISTAT_BUSY);
}

//==============================================================================
//设置Enc28j60时钟输出
//clk 频率 
//==============================================================================
void Enc28j60clkout(u8 clk)
{
    //setup clkout: 2 is 12.5MHz:
    Enc28j60Write(ECOCON, clk & 0x7);
}


//==============================================================================
//初始化
//macaddr mac地址 
//==============================================================================
u8 Enc28j60Init(u8* macaddr)
{   
    Enc28j60Spi3Init();
    Enc28j60_CSH();	 //拉高片选     

    Enc28j60WriteOp(Enc28j60_SOFT_RESET, 0, Enc28j60_SOFT_RESET); //软复位
  
    NextPacketPtr = RXSTART_INIT; //下一包的地址在接收起始地址 #define RXSTART_INIT     0x0
    // Rx start    
    Enc28j60Write(ERXSTL, RXSTART_INIT&0xFF);//向寄存器写起始地址地位 //#define ERXSTL           (0x08|0x00)	 
    Enc28j60Write(ERXSTH, RXSTART_INIT>>8);	 //向寄存器写起始地址高位
    // set receive pointer address     
    Enc28j60Write(ERXRDPTL, RXSTART_INIT&0xFF);	 //向寄存器写接收地址低位
    Enc28j60Write(ERXRDPTH, RXSTART_INIT>>8);	 //	向寄存器写接收地址高位
    // RX end
    Enc28j60Write(ERXNDL, RXSTOP_INIT&0xFF);
    Enc28j60Write(ERXNDH, RXSTOP_INIT>>8);
    // TX start	  1500
    Enc28j60Write(ETXSTL, TXSTART_INIT&0xFF);
    Enc28j60Write(ETXSTH, TXSTART_INIT>>8);
    // TX end
    Enc28j60Write(ETXNDL, TXSTOP_INIT&0xFF);
    Enc28j60Write(ETXNDH, TXSTOP_INIT>>8);

    
    Enc28j60Write(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
	//ERXFCON：接收过滤器控制寄存器 
	//UCEN：单播过滤器使能位 1 = 目标地址与本地MAC 地址不匹配的数据包将被丢弃
	//	当ANDOR = 0 时：     1 = 目标地址与本地MAC 地址匹配的数据包会被接受
	//CRCEN：后过滤器CRC 校验使能位 1 = 所有CRC 无效的数据包都将被丢弃
	//PMEN：格式匹配过滤器使能位 当ANDOR = 1 时：1 = 数据包必须符合格式匹配条件，否则将被丢弃
	//当ANDOR = 0 时： 1 = 符合格式匹配条件的数据包将被接受
    Enc28j60Write(EPMM0, 0x3f);	//EPMM0	  EPMM0 格式匹配屏蔽字节0 
    Enc28j60Write(EPMM1, 0x30);	//
    Enc28j60Write(EPMCSL, 0xf9);// EPMCSL 格式匹配校验和低字节
    Enc28j60Write(EPMCSH, 0xf7);// 高字节    
    Enc28j60Write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS); 
	//将MACON1 中MARXEN 位置1，使能MAC 接
    //收数据帧。 如果使用全双工模式，大多数应用还
    //需将TXPAUS 和RXPAUS 置1，以使能IEEE 定
    //义的流量控制。
    // bring MAC out of reset 
    Enc28j60Write(MACON2, 0x00);
    //将MACON2 中的MARST 位清零，使MAC 退出复位状态。
    Enc28j60WriteOp(Enc28j60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX);

    // set inter-frame gap (non-back-to-back)
	//设置非背对背包间间隔 
    Enc28j60Write(MAIPGL, 0x12); //非背对背包间间隔低字节
    Enc28j60Write(MAIPGH, 0x0C); //非背对背包间间隔高字节
    // set inter-frame gap (back-to-back)
	// 设置背对背包间间隔 
    Enc28j60Write(MABBIPG, 0x15); //背对背包间间隔

    // Set the maximum packet size which the controller will accept
    // Do not send packets longer than MAX_FRAMELEN:
  	//#define MAMXFLL          (0x0A|0x40|0x80)
	//#define MAMXFLH          (0x0B|0x40|0x80)
    Enc28j60Write(MAMXFLL, MAX_FRAMELEN&0xFF);	
    Enc28j60Write(MAMXFLH, MAX_FRAMELEN>>8);
    
	// do bank 3 stuff
    // write MAC address 写MAC地址
    // NOTE: MAC address in Enc28j60 is byte-backward  
    Enc28j60Write(MAADR5, macaddr[0]);	
    Enc28j60Write(MAADR4, macaddr[1]);
    Enc28j60Write(MAADR3, macaddr[2]);
    Enc28j60Write(MAADR2, macaddr[3]);
    Enc28j60Write(MAADR1, macaddr[4]);
    Enc28j60Write(MAADR0, macaddr[5]);

    //配置PHY为全双工  LEDB为拉电流
    Enc28j60PhyWrite(PHCON1, PHCON1_PDPXMD);    
    
    // no loopback of transmitted frames
    Enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);

    // switch to bank 0    
    Enc28j60SetBank(ECON1);

    // enable interrutps
    Enc28j60WriteOp(Enc28j60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);

    // enable packet reception
    Enc28j60WriteOp(Enc28j60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

	if(Enc28j60Read(MAADR5)== macaddr[0])return 0;//初始化成功
	else return 1; 
}

//==============================================================================
//读版本号
//返回值 芯片版本号 
//==============================================================================
u8 Enc28j60getrev(void)
{
    return(Enc28j60Read(EREVID));
}


//==============================================================================
//发送数据包到网络
//len:数据包大小 
//packet:数据包
//==============================================================================
void Enc28j60PacketSend(unsigned int len, unsigned char* packet)
{
    // 设置传输缓冲区首地址
    Enc28j60Write(EWRPTL, TXSTART_INIT&0xFF);
    Enc28j60Write(EWRPTH, TXSTART_INIT>>8);
    
    //设置包大小
    Enc28j60Write(ETXNDL, (TXSTART_INIT+len)&0xFF);
    Enc28j60Write(ETXNDH, (TXSTART_INIT+len)>>8);
    
    //写控制字节
    Enc28j60WriteOp(Enc28j60_WRITE_BUF_MEM, 0, 0x00);
    
    //拷贝包到传输缓冲区
    Enc28j60WriteBuffer(len, packet);
    
    //发送到网络
    Enc28j60WriteOp(Enc28j60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
    
    //复位传输逻辑
    if( (Enc28j60Read(EIR) & EIR_TXERIF) )
        Enc28j60WriteOp(Enc28j60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
}

//==============================================================================
//获取数据包
//maxlen:数据包最大允许接收长度
//packet:接收数据包缓存区
//返回值:收到的数据包长度(字节)
//==============================================================================								  
unsigned int Enc28j60PacketReceive(unsigned int maxlen, unsigned char* packet)
{
    unsigned int rxstat;
    unsigned int len;
    

    if( Enc28j60Read(EPKTCNT) ==0 )  //收到的以太网数据包长度
    {
        return(0);
    }
    
    // 设置缓冲器读指针
    Enc28j60Write(ERDPTL, (NextPacketPtr));
    Enc28j60Write(ERDPTH, (NextPacketPtr)>>8);
    
    // 读下一包的指针
    NextPacketPtr  = Enc28j60ReadOp(Enc28j60_READ_BUF_MEM, 0);
    NextPacketPtr |= Enc28j60ReadOp(Enc28j60_READ_BUF_MEM, 0)<<8;
    
    // 读包长度
    len  = Enc28j60ReadOp(Enc28j60_READ_BUF_MEM, 0);
    len |= Enc28j60ReadOp(Enc28j60_READ_BUF_MEM, 0)<<8;
    
    len-=4; //去掉CRC记数
    //读接收状态
    rxstat  = Enc28j60ReadOp(Enc28j60_READ_BUF_MEM, 0);
    rxstat |= Enc28j60ReadOp(Enc28j60_READ_BUF_MEM, 0)<<8;
    // 接收长度必须小于等于maxlen
    if (len>maxlen-1)
    {
        len=maxlen-1;
    }
    
    if ((rxstat & 0x80)==0)
    {
        len=0;
    }
    else
    {
        Enc28j60ReadBuffer(len, packet); //从缓冲区复制包
    }
    // 移动接收读指针到下一个包
    Enc28j60Write(ERXRDPTL, (NextPacketPtr));
    Enc28j60Write(ERXRDPTH, (NextPacketPtr)>>8);
    
    // 包计数器减一
    Enc28j60WriteOp(Enc28j60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
    return(len);
}



