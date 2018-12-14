

#include "Enc28j60.h"

static u8 Enc28j60Bank;
static u32 NextPacketPtr;	

//==============================================================================
//��ʼ��SPI3 
//==============================================================================
void Enc28j60Spi3Init(void)
{
   	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
   	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE );//PORTB,D,G,SPI3ʱ��ʹ�� 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//spi3��JLINK���Ÿ��� ��������ֹJTAG
 			
	GPIO_InitStructure.GPIO_Pin = spi3_clk | spi3_mosi | spi3_miso;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(spi3_gpio, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = spi3_cs ;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //NSS�ź��������
	GPIO_Init(spi3_gpio, &GPIO_InitStructure);
	GPIO_SetBits(spi3_gpio,spi3_cs);


	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  //SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		   //����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	   //����ͬ��ʱ�ӵĵ�һ�������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		   //NSS�ź����������
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;	//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ2
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ (dm: 7Ϊ��λֵ)
	SPI_Init(SPI3, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPI3�Ĵ���
 
	SPI_Cmd(SPI3, ENABLE); //ʹ��SPI����
	
	spi_sendrece(SPI3,0xFF);//��������		 
}

//==============================================================================
//��ȡEnc28j60�Ĵ��� 
//op ������  
//address ��ַ
//����ֵ  :����������  
//==============================================================================
u8 Enc28j60ReadOp(u8 op, u8 address)
{
    u8 dat = 0;
    
    Enc28j60_CSL();    
    dat = op | (address & ADDR_MASK);  //#define ADDR_MASK        0x1F	  ��5λ

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
//д��Enc28j60�Ĵ��� 
//op ������  
//address ��ַ
//data Ҫд������� 
//==============================================================================
void Enc28j60WriteOp(u8 op, u8 address, u8 data)
{
    u8 dat = 0;
    
    Enc28j60_CSL();
    // issue write command
    dat = op | (address & ADDR_MASK);	// #define ADDR_MASK  0x1F  00011111 ��λ��ַ
    spi_sendrece(SPI3,dat); 
    // write data
    dat = data;
    spi_sendrece(SPI3,dat);
    Enc28j60_CSH();
}
//Enc28j60ReadBuffer ���� ָ��
//��������
//

//==============================================================================
//��Enc28j60������ 
//len ��ȡ�ĳ��� 
//data Ŀ���ַ 
//==============================================================================
void Enc28j60ReadBuffer(u32 len, u8* data)
{
    Enc28j60_CSL();
    // issue read command
    spi_sendrece(SPI3,Enc28j60_READ_BUF_MEM); //����һ�ζ�������ָ��
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
//дEnc28j60������ 
//len д��ĳ��� 
//data Ҫд�����ݵ��׵�ַ 
//==============================================================================
void Enc28j60WriteBuffer(u32 len, u8* data)
{
    Enc28j60_CSL();
    // issue write command
    spi_sendrece(SPI3,Enc28j60_WRITE_BUF_MEM);//����һ��д������ָ��
    
    while(len)
    {
        len--;
        spi_sendrece(SPI3,*data);
        data++;
    }
    Enc28j60_CSH();
}
//==============================================================================
//���ݵ�ַ����Enc28j60��ַ��
//address ��ַ 
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
//��ȡEnc28j60�Ĵ��� 
//address ��ַ 
//==============================================================================
u8 Enc28j60Read(u8 address)	  //���Ĵ���
{
    Enc28j60SetBank(address);
    return Enc28j60ReadOp(Enc28j60_READ_CTRL_REG, address);
}

//==============================================================================
//дEnc28j60�Ĵ��� 
//address ��ַ 
//data ����
//==============================================================================
void Enc28j60Write(u8 address, u8 data)
{
    
    Enc28j60SetBank(address);  //������
    Enc28j60WriteOp(Enc28j60_WRITE_CTRL_REG, address, data); //д�Ĵ���
}									

//==============================================================================
//��Enc28j60��PHY�Ĵ���д���� 
//address ��ַ 
//data ����
//==============================================================================
void Enc28j60PhyWrite(u8 address, u32 data)
{
    // set the PHY register address
    Enc28j60Write(MIREGADR, address); //MIREGADR  MII �Ĵ�����ַ
    // write the PHY data
    Enc28j60Write(MIWRL, data);
    Enc28j60Write(MIWRH, data>>8);
    // wait until the PHY write completes
    while(Enc28j60Read(MISTAT) & MISTAT_BUSY);
}

//==============================================================================
//����Enc28j60ʱ�����
//clk Ƶ�� 
//==============================================================================
void Enc28j60clkout(u8 clk)
{
    //setup clkout: 2 is 12.5MHz:
    Enc28j60Write(ECOCON, clk & 0x7);
}


//==============================================================================
//��ʼ��
//macaddr mac��ַ 
//==============================================================================
u8 Enc28j60Init(u8* macaddr)
{   
    Enc28j60Spi3Init();
    Enc28j60_CSH();	 //����Ƭѡ     

    Enc28j60WriteOp(Enc28j60_SOFT_RESET, 0, Enc28j60_SOFT_RESET); //��λ
  
    NextPacketPtr = RXSTART_INIT; //��һ���ĵ�ַ�ڽ�����ʼ��ַ #define RXSTART_INIT     0x0
    // Rx start    
    Enc28j60Write(ERXSTL, RXSTART_INIT&0xFF);//��Ĵ���д��ʼ��ַ��λ //#define ERXSTL           (0x08|0x00)	 
    Enc28j60Write(ERXSTH, RXSTART_INIT>>8);	 //��Ĵ���д��ʼ��ַ��λ
    // set receive pointer address     
    Enc28j60Write(ERXRDPTL, RXSTART_INIT&0xFF);	 //��Ĵ���д���յ�ַ��λ
    Enc28j60Write(ERXRDPTH, RXSTART_INIT>>8);	 //	��Ĵ���д���յ�ַ��λ
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
	//ERXFCON�����չ��������ƼĴ��� 
	//UCEN������������ʹ��λ 1 = Ŀ���ַ�뱾��MAC ��ַ��ƥ������ݰ���������
	//	��ANDOR = 0 ʱ��     1 = Ŀ���ַ�뱾��MAC ��ַƥ������ݰ��ᱻ����
	//CRCEN���������CRC У��ʹ��λ 1 = ����CRC ��Ч�����ݰ�����������
	//PMEN����ʽƥ�������ʹ��λ ��ANDOR = 1 ʱ��1 = ���ݰ�������ϸ�ʽƥ�����������򽫱�����
	//��ANDOR = 0 ʱ�� 1 = ���ϸ�ʽƥ�����������ݰ���������
    Enc28j60Write(EPMM0, 0x3f);	//EPMM0	  EPMM0 ��ʽƥ�������ֽ�0 
    Enc28j60Write(EPMM1, 0x30);	//
    Enc28j60Write(EPMCSL, 0xf9);// EPMCSL ��ʽƥ��У��͵��ֽ�
    Enc28j60Write(EPMCSH, 0xf7);// ���ֽ�    
    Enc28j60Write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS); 
	//��MACON1 ��MARXEN λ��1��ʹ��MAC ��
    //������֡�� ���ʹ��ȫ˫��ģʽ�������Ӧ�û�
    //�轫TXPAUS ��RXPAUS ��1����ʹ��IEEE ��
    //����������ơ�
    // bring MAC out of reset 
    Enc28j60Write(MACON2, 0x00);
    //��MACON2 �е�MARST λ���㣬ʹMAC �˳���λ״̬��
    Enc28j60WriteOp(Enc28j60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX);

    // set inter-frame gap (non-back-to-back)
	//���÷Ǳ��Ա������� 
    Enc28j60Write(MAIPGL, 0x12); //�Ǳ��Ա����������ֽ�
    Enc28j60Write(MAIPGH, 0x0C); //�Ǳ��Ա����������ֽ�
    // set inter-frame gap (back-to-back)
	// ���ñ��Ա������� 
    Enc28j60Write(MABBIPG, 0x15); //���Ա�������

    // Set the maximum packet size which the controller will accept
    // Do not send packets longer than MAX_FRAMELEN:
  	//#define MAMXFLL          (0x0A|0x40|0x80)
	//#define MAMXFLH          (0x0B|0x40|0x80)
    Enc28j60Write(MAMXFLL, MAX_FRAMELEN&0xFF);	
    Enc28j60Write(MAMXFLH, MAX_FRAMELEN>>8);
    
	// do bank 3 stuff
    // write MAC address дMAC��ַ
    // NOTE: MAC address in Enc28j60 is byte-backward  
    Enc28j60Write(MAADR5, macaddr[0]);	
    Enc28j60Write(MAADR4, macaddr[1]);
    Enc28j60Write(MAADR3, macaddr[2]);
    Enc28j60Write(MAADR2, macaddr[3]);
    Enc28j60Write(MAADR1, macaddr[4]);
    Enc28j60Write(MAADR0, macaddr[5]);

    //����PHYΪȫ˫��  LEDBΪ������
    Enc28j60PhyWrite(PHCON1, PHCON1_PDPXMD);    
    
    // no loopback of transmitted frames
    Enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);

    // switch to bank 0    
    Enc28j60SetBank(ECON1);

    // enable interrutps
    Enc28j60WriteOp(Enc28j60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);

    // enable packet reception
    Enc28j60WriteOp(Enc28j60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

	if(Enc28j60Read(MAADR5)== macaddr[0])return 0;//��ʼ���ɹ�
	else return 1; 
}

//==============================================================================
//���汾��
//����ֵ оƬ�汾�� 
//==============================================================================
u8 Enc28j60getrev(void)
{
    return(Enc28j60Read(EREVID));
}


//==============================================================================
//�������ݰ�������
//len:���ݰ���С 
//packet:���ݰ�
//==============================================================================
void Enc28j60PacketSend(unsigned int len, unsigned char* packet)
{
    // ���ô��仺�����׵�ַ
    Enc28j60Write(EWRPTL, TXSTART_INIT&0xFF);
    Enc28j60Write(EWRPTH, TXSTART_INIT>>8);
    
    //���ð���С
    Enc28j60Write(ETXNDL, (TXSTART_INIT+len)&0xFF);
    Enc28j60Write(ETXNDH, (TXSTART_INIT+len)>>8);
    
    //д�����ֽ�
    Enc28j60WriteOp(Enc28j60_WRITE_BUF_MEM, 0, 0x00);
    
    //�����������仺����
    Enc28j60WriteBuffer(len, packet);
    
    //���͵�����
    Enc28j60WriteOp(Enc28j60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
    
    //��λ�����߼�
    if( (Enc28j60Read(EIR) & EIR_TXERIF) )
        Enc28j60WriteOp(Enc28j60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
}

//==============================================================================
//��ȡ���ݰ�
//maxlen:���ݰ����������ճ���
//packet:�������ݰ�������
//����ֵ:�յ������ݰ�����(�ֽ�)
//==============================================================================								  
unsigned int Enc28j60PacketReceive(unsigned int maxlen, unsigned char* packet)
{
    unsigned int rxstat;
    unsigned int len;
    

    if( Enc28j60Read(EPKTCNT) ==0 )  //�յ�����̫�����ݰ�����
    {
        return(0);
    }
    
    // ���û�������ָ��
    Enc28j60Write(ERDPTL, (NextPacketPtr));
    Enc28j60Write(ERDPTH, (NextPacketPtr)>>8);
    
    // ����һ����ָ��
    NextPacketPtr  = Enc28j60ReadOp(Enc28j60_READ_BUF_MEM, 0);
    NextPacketPtr |= Enc28j60ReadOp(Enc28j60_READ_BUF_MEM, 0)<<8;
    
    // ��������
    len  = Enc28j60ReadOp(Enc28j60_READ_BUF_MEM, 0);
    len |= Enc28j60ReadOp(Enc28j60_READ_BUF_MEM, 0)<<8;
    
    len-=4; //ȥ��CRC����
    //������״̬
    rxstat  = Enc28j60ReadOp(Enc28j60_READ_BUF_MEM, 0);
    rxstat |= Enc28j60ReadOp(Enc28j60_READ_BUF_MEM, 0)<<8;
    // ���ճ��ȱ���С�ڵ���maxlen
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
        Enc28j60ReadBuffer(len, packet); //�ӻ��������ư�
    }
    // �ƶ����ն�ָ�뵽��һ����
    Enc28j60Write(ERXRDPTL, (NextPacketPtr));
    Enc28j60Write(ERXRDPTH, (NextPacketPtr)>>8);
    
    // ����������һ
    Enc28j60WriteOp(Enc28j60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
    return(len);
}



