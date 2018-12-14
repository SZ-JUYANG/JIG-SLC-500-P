
#include "spiflash.h"

void spiflash_init(void)
{
	spi_initn(flash_spi);
	GPIO_ResetBits(spi2_selectgpio,spi2_selectpin );
	spiflash_sendrece(0xff);	
}
//读取spiflash的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使能
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
u8 spiflash_readreg(void)   
{  
	u8 rece=0;   
	spiflash_enable;                              //使能   
	spiflash_sendrece(SPIFLASH_ReadStatusReg);    //发送读取状态寄存器命令    
	rece=spiflash_sendrece(0Xff);                 //读取一个字节  
	spiflash_disable;                             //除能     
	return rece;   
} 
//写spiflash状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void spiflash_writereg(u8 data)   
{   
	spiflash_enable;                            //使能   
	spiflash_sendrece(SPIFLASH_WriteStatusReg); //发送写状态寄存器命令    
	spiflash_sendrece(data);               		//写入一个字节  
	spiflash_disable;                           //除能     	      
}   
//spiflash写使能	
//将WEL置位   
void spiflash_writeenable(void)   
{
	spiflash_enable;                            //使能   
    spiflash_sendrece(SPIFLASH_WriteEnable);    //发送写使能  
	spiflash_disable;                           //除能     	      
} 
//spiflash写禁止	
//将WEL清零  
void spiflash_writedisable(void)   
{  
	spiflash_enable;                            //使能   
    spiflash_sendrece(SPIFLASH_WriteDisable);   //发送写除能指令    
	spiflash_disable;                           //除能     	      
} 		
	    
//读取芯片ID SPIFLASH 的ID
u16 spiflash_readid(void)
{
	u16 rece = 0;	  
	spiflash_enable;				    
	spiflash_sendrece(0x90);//发送读取ID命令	    
	spiflash_sendrece(0x00); 	    
	spiflash_sendrece(0x00); 	    
	spiflash_sendrece(0x00); 	 			   
	rece|=spiflash_sendrece(0xFF)<<8;  
	rece|=spiflash_sendrece(0xFF);	 
	spiflash_disable;				    
	return rece;
}   		    
//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//buf:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void spiflash_read2buf(u8* buf,u32 startaddr,u16 len)   
{ 
 	u16 i;    												    
	spiflash_enable;                            //使能   
    spiflash_sendrece(SPIFLASH_ReadData);         //发送读取命令   
    spiflash_sendrece((u8)(startaddr>>16));  //发送24bit地址    
    spiflash_sendrece((u8)(startaddr>>8));   
    spiflash_sendrece((u8)startaddr);   
    for(i=0;i<len;i++)
	{ 
        buf[i]=spiflash_sendrece(0XFF);   //循环读数  
    }
	spiflash_disable;                            //除能     	      
}  
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//buf:数据存储区
//startaddr:开始写入的地址(24bit)
//len:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void spiflash_write2page(u8* buf,u32 startaddr,u16 len)
{
 	u16 i;  
    spiflash_writeenable();                    //写允许 
	spiflash_enable;                            //使能   
    spiflash_sendrece(SPIFLASH_PageProgram);    //发送写页命令   
    spiflash_sendrece((u8)(startaddr>>16)); 	//发送24bit地址    
    spiflash_sendrece((u8)(startaddr>>8));   
    spiflash_sendrece((u8)startaddr);   
    for(i=0;i<len;i++)spiflash_sendrece(buf[i]);//循环写数  
	spiflash_disable;                            //除能 
	spiflash_wait4busy();					   //等待写入结束
} 
//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//buf:数据存储区
//startaddr:开始写入的地址(24bit)
//len:要写入的字节数(最大65535)
//CHECK OK
void spiflash_writewithoutcheck(u8* buf,u32 startaddr,u16 len)   
{ 			 		 
	u16 remain;	   
	remain=256-startaddr%256; //单页剩余的字节数		 	    
	if(len<=remain)remain=len;//不大于256个字节
	while(1)
	{	   
		spiflash_write2page(buf,startaddr,remain);
		if(len==remain)break;//写入结束了
	 	else 
		{
			buf+=remain;
			startaddr+=remain;	

			len-=remain;			  //减去已经写入了的字节数
			if(len>256)remain=256; //一次可以写入256个字节
			else remain=len; 	  //不够256个字节了
		}
	};	    
} 
//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//buf:数据存储区
//startaddr:开始写入的地址(24bit)
//len:要写入的字节数(最大65535)  		   
u8 spiflashbuf[4096];
void spiflash_write(u8* buf,u32 startaddr,u16 len)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	secpos=startaddr/4096;//扇区地址 0~511 for SPIFLASH16
	secoff=startaddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   

	if(len<=secremain)secremain=len;//不大于4096个字节
	while(1) 
	{	
		spiflash_read2buf(spiflashbuf,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(spiflashbuf[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			spiflash_erasesector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				spiflashbuf[i+secoff]=buf[i];	  
			}
			spiflash_writewithoutcheck(spiflashbuf,secpos*4096,4096);//写入整个扇区  

		}else spiflash_writewithoutcheck(buf,startaddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(len==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	buf+=secremain;  //指针偏移
			startaddr+=secremain;//写地址偏移	   
		   	len-=secremain;				//字节数递减
			if(len>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=len;			//下一个扇区可以写完了
		}	 
	};	 	 
}
//擦除整个芯片
//整片擦除时间:
//SPIFLASH16:25s 
//SPIFLASH32:40s 
//SPIFLASH64:40s 
//等待时间超长...
void spiflash_erasechip(void)   
{                                             
    spiflash_writeenable();                  //SET WEL 
    spiflash_wait4busy();   
  	spiflash_enable;                            //使能   
    spiflash_sendrece(SPIFLASH_ChipErase);        //发送片擦除命令  
	spiflash_disable;                            //除能     	      
	spiflash_wait4busy();   				   //等待芯片擦除结束
}   
//擦除一个扇区
//Dst_Addr:扇区地址 0~511 for SPIFLASH16
//擦除一个山区的最少时间:150ms
void spiflash_erasesector(u32 startaddr)   
{   
	startaddr*=4096;
    spiflash_writeenable();                  //SET WEL 	 
    spiflash_wait4busy();   
  	spiflash_enable;                            //使能   
    spiflash_sendrece(SPIFLASH_SectorErase);      //发送扇区擦除指令 
    spiflash_sendrece((u8)((startaddr)>>16));  //发送24bit地址    
    spiflash_sendrece((u8)((startaddr)>>8));   
    spiflash_sendrece((u8)startaddr);  
	spiflash_disable;                            //除能     	      
    spiflash_wait4busy();   				   //等待擦除完成
}  
//等待空闲
void spiflash_wait4busy(void)   
{   
	while ((spiflash_readreg()&0x01)==0x01);   // 等待BUSY位清空
}  
//进入掉电模式
void spiflash_powerdown(void)   
{ 
  	spiflash_enable;                            //使能   
    spiflash_sendrece(SPIFLASH_PowerDown);        //发送掉电命令  
	spiflash_disable;                            //除能     	      
    delay_us(3);                               //等待TPD  
}   
//唤醒
void spiflash_wakeup(void)   
{  
  	spiflash_enable;                            //使能   
    spiflash_sendrece(SPIFLASH_ReleasePowerDown);   //  send SPIFLASH_PowerDown command 0xAB    
	spiflash_disable;                            //除能     	      
    delay_us(3);                               //等待TRES1
}   




