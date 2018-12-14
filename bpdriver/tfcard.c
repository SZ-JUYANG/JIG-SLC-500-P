
#include <tfcard.h>

					   
extern u8  TFCARDTYPE;//TF卡的类型 

//===========================================================================
//TF卡初始化的时候,应运行在低速模式
//===========================================================================
void tfcard_slow(void)
{
 	spi_setspeed(tfcard_spi,SPI_BaudRatePrescaler_256);//设置到低速模式	
}
//===========================================================================
//TF卡正常工作的时候,运行在高速模式
//===========================================================================
void tfcard_fast(void)
{
 	spi_setspeed(tfcard_spi,SPI_BaudRatePrescaler_2);//设置到高速模式	
}
//===========================================================================
//取消选择,释放SPI总线
//===========================================================================
void tfcard_disselect(void)
{
	tfcard_disable;
	tfcard_sendrece(0xff);//提供额外的8个时钟
}
//===========================================================================
//选择TF卡,并且等待卡准备OK
//返回值:0,成功;1,失败;
//===========================================================================
u8 tfcard_Select(void)
{
	tfcard_enable;
	if(tfcard_wait4ready()==0)
	{				
		if (ISDEBUG) printf("\ntfcard_waitforready suc！");
		return 0;//等待成功
	}
	tfcard_disselect();
	if  (ISDEBUG) printf("\n tf card_WAUTREADY fail");	
	return 1;//等待失败
}
//===========================================================================
//连续发送时钟，若得到全1则卡准备好
//返回值:0,准备好了;1,超时
//===========================================================================
u8 tfcard_wait4ready(void)
{
	u32 timeout=0xFFFFFF;
	do
	{
		//delay_ms(500);
		if(tfcard_sendrece(0XFF)==0xFF) return 0;			  	
	}while(--timeout);
	return 1;          //超时
}
//===========================================================================
//取得TF卡指定回应，若超时无法取到返回失败
//参数Response:要得到的回应值
//返回值:0,成功得到回应
//       1,未得到正确的回应值
//===========================================================================
u8 tfcard_getresponse(u8 response)
{
	u16 timeout=0xFFF;//等待次数	   						  
	while ((tfcard_sendrece(0XFF)!=response)&&timeout) --timeout; //等待得到回应  	  
	if (timeout==0) return 1;									  //超时   
	return 0;						                              //得到回应
}
//===========================================================================
//从TF卡读取一个数据包的内容
//buf:数据缓存区
//len:要读取的数据长度.
//返回值:0,成功
//       其他,失败;	
//===========================================================================
u8 tfcard_recevedata(u8*buf,u16 len)
{			  	  
	if(tfcard_getresponse(BEGINSIGLE))return 1; //等待TF卡发回数据起始字节
    while(len--)						  		//开始接收len个数据
    {
        *buf=tfcard_sendrece(0xFF);				//8个时钟周期接收一个字节
        buf++;
    }
    //发送伪CRC
    tfcard_sendrece(0xFF);
    tfcard_sendrece(0xFF);									  					    
    return 0;			//读取成功
}
//===========================================================================
//发送数据命令后，向TF卡写入一个块 512字节
//先写起始位，然后是512个字节，然后是2个字节伪校验值，最后发8个时钟  
//buf:数据缓存区
//beginbyte:开始字节
//返回值:0,成功;其他,失败;	
//===========================================================================
u8 tfcard_sendblock( u8 * buf,u8 beginbyte)
{	
	u16 t;		  	  
	if(tfcard_wait4ready())return 1;//返回1 等待超时
	tfcard_sendrece(beginbyte);		//发送起始字节或结束字符
	if(beginbyte!=MULSTOP)			//如果不是结束字符，则执行发送，如果是则返回
	{
		for(t=0;t<512;t++)tfcard_sendrece(buf[t]);//发送512个字节
	    tfcard_sendrece(0xFF);//伪校验值
	    tfcard_sendrece(0xFF);
		t=tfcard_sendrece(0xFF);//发8个时钟，取得R1返回值
		if((t&0x1F)!=0x05)return 2;	//响应错误									  					    
	}						 									  					    
    return 0;//写入成功
}
//===========================================================================
//向TF卡发送一个命令
//输入: u8 cmd   命令 
//      u32 arg  4个字节的命令参数
//      u8 crc   crc校验值	   
//返回值:TF卡返回的响应	
//===========================================================================														  
u8 tfcard_sendcommand(u8 cmd, u32 arg, u8 crc)
{
    u8 r1;	
	u8 timeout=0x1F; 
	tfcard_disselect();//置高片选
//	if(tfcard_Select())return 0XFF;//拉低片选
    tfcard_sendrece(cmd | 0x40);//写入命令 与0x40(0100 0000)即设置主机发送位
    tfcard_sendrece(arg >> 24); //写入参数高31-24位
    tfcard_sendrece(arg >> 16);	//写入参数高23-16位
    tfcard_sendrece(arg >> 8);	//写入参数高15-8位
    tfcard_sendrece(arg);	  	//写入参数高7-0位
    tfcard_sendrece(crc); 	    //写校验位 非CMD0指令的时候，不进行CRC校验，可以随便写
	if(cmd==CMD12)  tfcard_sendrece(0xff);//如果是停止通信指令，多发一个时钟
    //等待响应，或超时退出
	do
	{
		r1=tfcard_sendrece(0xFF);
	}while((r1&0X80) && timeout--);	//发送时钟直到接到R1回应	 
    return r1;	  //返回R1回应
}	
//===========================================================================	    																			  
//获取TF卡的CID信息，包括制造商信息
//输入: u8 *cid_data(存放CID的内存，至少16Byte）	  
//返回值:0：NO_ERR
//		 1：错误														   
//===========================================================================
u8 tfcard_getid(u8 *cid_data)
{
    u8 r1;	   
    r1=tfcard_sendcommand(CMD10,0,0x01); //发CMD10命令，读CID
    if(r1==0x00)
		r1=tfcard_recevedata(cid_data,16);//接收16个字节的数据	 
	tfcard_disselect();//取消片选
	if(r1)return 1;
	else return 0;
}																				
//===========================================================================  
//获取TF卡的CSD信息，包括容量和速度信息
//输入:u8 *cid_data(存放CID的内存，至少16Byte）	    
//返回值:0：NO_ERR
//		 1：错误														   
//===========================================================================
u8 tfcard_getcsd(u8 *ctfcard_data)
{
    u8 r1;	 
    r1=tfcard_sendcommand(CMD9,0,0x01);//发CMD9命令，读CSD
    if(r1==0)
    	r1=tfcard_recevedata(ctfcard_data, 16);//接收16个字节的数据 
	tfcard_disselect();//取消片选
	if(r1)return 1;
	else return 0;
}  
//===========================================================================
//获取TF卡的总扇区数（扇区数）   
//返回值:0： 取容量出错 
//       其他:TF卡的容量(扇区数/512字节)
//每扇区的字节数必为512，因为如果不是512，则初始化不能通过.						
//===========================================================================								  
u32 tfcard_getsectorcount(void)
{
    u8 csd[16];
    u32 Capacity;  
    u8 n;
	u16 csize;  					    
	//取CSD信息，如果期间出错，返回0
    if(tfcard_getcsd(csd)!=0) return 0;	    
    //如果为SDHC卡，按照下面方式计算
    if((csd[0]&0xC0)==0x40)	 //V2.00的卡
    {	
		csize = csd[9] + ((u16)csd[8] << 8) + 1;
		Capacity = (u32)csize << 10;//得到扇区数	 		   
    }else//V1.XX的卡
    {	
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
		Capacity= (u32)csize << (n - 9);//得到扇区数   
    }
    return Capacity;
}
//===========================================================================

//判断TF卡是否空闲
//===========================================================================
u8 tfcard_judgeidle(void)
{
	u16 i;
	u8 retry;	   	  
    for(i=0;i<0xf00;i++);//纯延时，等待TF卡上电完成	 
    //先产生>74个脉冲，让TF卡自己初始化完成
    for(i=0;i<10;i++)tfcard_sendrece(0xFF); 
    //-----------------TF卡复位到idle开始-----------------
    //循环连续发送CMD0，直到TF卡返回0x01,进入IDLE状态
    //超时则直接退出
    retry = 0;
    do
    {	   
        //发送CMD0，让TF卡进入IDLE状态，进入SPI模式
        i = tfcard_sendcommand(CMD0, 0, 0x95);
        retry++;
    }while((i!=0x01)&&(retry<200));
    //跳出循环后，检查原因：初始化成功？or 重试超时？
    if(retry==200)return 1; //失败
	return 0;//成功	 						  
}
//===========================================================================
//初始化TF卡
//===========================================================================
u8 tfcard_init(void)
{
    u8 r1;      // 存放TF卡的返回值
    u16 timeout;  // 用来进行超时计数
    u8 buf[4];  
	u16 i;

	spi_initn(tfcard_spi);		//初始化IO
 	tfcard_slow();	//设置到低速模式 
    //主机上电后，所有SD 卡进入Idle 状态。至少74 个时钟周期后才能开始总线传输。
	for(i=0;i<10;i++)
	{
	//	if (ISDEBUG) printf("tfcard_sendrece times %d\n",i);
		tfcard_sendrece(0XFF);//发送80个脉冲
	}
	timeout=20;
	do
	{
		r1=tfcard_sendcommand(CMD0,0,0x95);//进入IDLE状态
	}while((r1!=0X01) && timeout--); //判断响应最低位是否为1
	TFCARDTYPE=0;	//默认无卡
	if(r1==0X01)
	{
		if(tfcard_sendcommand(CMD8,0x1AA,0x87)==1)
		{
 			for(i=0;i<4;i++)buf[i]=tfcard_sendrece(0XFF);	//获得卡的电压信息			
			if(buf[2]==0X01&&buf[3]==0XAA)                 //卡是否支持2.7~3.6V
			{
				timeout=0XFFFF;
				do
				{
					tfcard_sendcommand(CMD55,0,0X01);	         //发送CMD55
					r1=tfcard_sendcommand(ACMD41,0x40000000,0X01);//发送CMD41
				}while(r1&&timeout--);
			   	if(timeout&&tfcard_sendcommand(CMD58,0,0X01)==0)//鉴别SD2.0卡版本开始
				{
					for(i=0;i<4;i++)buf[i]=tfcard_sendrece(0XFF);//得到OCR值
					if(buf[0]&0x40)TFCARDTYPE=TFCARDTYPE_V2HC;    //检查CCS
					else TFCARDTYPE=TFCARDTYPE_V2;   
				}
				if (timeout!=0)
				{
					tfcard_fast();//高速
					return 0;// TFCARDTYPE;
				}
			}			
		}else//SD V1.x/ MMC	V3
		{
			tfcard_sendcommand(CMD55,0,0X01);		//发送CMD55
			r1=tfcard_sendcommand(ACMD41,0,0X01);	//发送CMD41
			if(r1<=1)
			{		
				TFCARDTYPE=TFCARDTYPE_V1;
				timeout=0XFFFE;
				do //等待退出IDLE模式
				{
					tfcard_sendcommand(CMD55,0,0X01);	//发送CMD55
					r1=tfcard_sendcommand(ACMD41,0,0X01);//发送CMD41
				}while(r1&&timeout--);
			}else
			{
				TFCARDTYPE=TFCARDTYPE_MMC;//MMC V3
				timeout=0XFFFE;
				do //等待退出IDLE模式
				{											    
					r1=tfcard_sendcommand(CMD1,0,0X01);//发送CMD1
				}while(r1&&timeout--);  
			}
			if(timeout==0||tfcard_sendcommand(CMD16,512,0X01)!=0)TFCARDTYPE=TFCARDTYPE_ERR;//错误的卡
			else {tfcard_fast();//高速
	              return 0;
				 }  
		}
	}
	tfcard_disselect();//取消片选       
	tfcard_fast();//高速
	return 1;    //TFCARDTYPE;//返回TFCARD版本

}	
	
//===========================================================================
//读TF卡
//buf:数据缓存区
//sector:扇区
//cnt:扇区数
//返回值:0,ok;其他,失败.
//===========================================================================
u8 tfcard_readsectors(u8*buf,u32 sector,u8 cnt)
{
	u8 r1;
	if(TFCARDTYPE!=TFCARDTYPE_V2HC)sector <<= 9;//转换为字节地址
	if(cnt==1)
	{  	    
		r1=tfcard_sendcommand(CMD17,sector,0x01);//读命令
		if(r1==0)//指令发送成功
			r1=tfcard_recevedata(buf,512);//接收512个字节	   
	}else
	{
		r1=tfcard_sendcommand(CMD18,sector,0x01);//连续读命令
		do
		{
			r1=tfcard_recevedata(buf,512);//接收512个字节	 
			buf+=512;  
		}while(--cnt && r1==0); 	
		tfcard_sendcommand(CMD12,0,0X01);	//发送停止命令
	}   
	tfcard_disselect();//取消片选
	return r1;//
}

//===========================================================================
//写TF卡
//buf:数据缓存区
//sector:起始扇区
//cnt:扇区数
//返回值:0 成功
//===========================================================================
u8 tfcard_writesectors( BYTE*buf,u32 sector,u8 cnt)
{
	u8 r1;
	if(TFCARDTYPE!=TFCARDTYPE_V2HC)sector<<=9;//转换为字节地址
	if(cnt==1)
	{
		r1=tfcard_sendcommand(CMD24,sector,0X01);//发送写扇区命令
		if(r1==0)//指令发送成功
			r1=tfcard_sendblock(buf,BEGINSIGLE);//发送数据块，写512个字节	   
	}else
	{
 		r1=tfcard_sendcommand(CMD25,sector,0X01);//连续写命令
		if(r1==0)
		{
			do
			{
				r1=tfcard_sendblock(buf,BEGINMUL);//发送512个字节	 
				buf+=512;  
			}while(--cnt && r1==0);
			r1=tfcard_sendblock(0,MULSTOP);//发送结束命令 
		}
	}   
	tfcard_disselect();//取消片选
	return r1;
}	   
























	  






