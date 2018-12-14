
#include "iic.h"


//===========================================================================
//Subroutine:	 iic_init 
//IIC初始化
//===========================================================================
void iic_init()
{
 	GPIO_InitTypeDef GPIO_InitStructure;
						    
	GPIO_InitStructure.GPIO_Pin = iic_scl|iic_sda;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //iic_scl|iic_sda推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(iic_gpio, &GPIO_InitStructure);
	GPIO_SetBits(iic_gpio,iic_scl|iic_sda);				//设置为1

}
//===========================================================================
//Subroutine:	 iic_start 
//IIC数据传送启动信号
//SCL为高电平的时候，SDA下降沿
//===========================================================================
 void iic_start(void)
{
	iic_sdaout();    				 //SDA线输出
	GPIO_SetBits(iic_gpio,iic_sda);	 //SDA为高电平 	  
	GPIO_SetBits(iic_gpio,iic_scl);	 //SCL为高电平
	delay_us(4);
 	GPIO_ResetBits(iic_gpio,iic_sda); //SDA下降沿
}	  
//===========================================================================
//Subroutine:	 iic_stop 
//IIC数据传送停止信号
//SCL为高电平时，SDA上升沿
//===========================================================================
void iic_stop(void)
{
	iic_sdaout();					  //SDA线输出
	GPIO_ResetBits(iic_gpio,iic_sda); //SDA为低电平
 	delay_us(4);
	GPIO_SetBits(iic_gpio,iic_scl);  //SCL高电平
	GPIO_SetBits(iic_gpio,iic_sda);  //SDA上升沿，I2C总线数据传送结束信号						   	
}
//===========================================================================
//Subroutine:	 iic_wait4ack 
//IIC等待应答
//返回值：1，接收应答失败
//        0，接收应答成功
//===========================================================================

INT8U iic_wait4ack(void)
{
	INT8U times=0;
	iic_sdain();      //SDA设置为输入  
	GPIO_SetBits(iic_gpio,iic_sda);//sda发送高点平
	delay_us(1);	   
	GPIO_SetBits(iic_gpio,iic_scl);	//时钟高电平，上升沿
	delay_us(1);	 
	while(GPIO_ReadInputDataBit(iic_gpio, iic_sda))	//读数据，直到从机给出应答
	{
		if(++times>IICWAITTIMEOUT)
		{
			iic_stop(); //无应答，停止
			return 1;
		}
	}
	GPIO_ResetBits(iic_gpio,iic_scl);//时钟输出低电平，下降沿 	   
	return 0;  
} 
//===========================================================================
//Subroutine:	 iic_act    产生应答
//表示将继续接收数据
//===========================================================================

void iic_ack(void)
{
	GPIO_ResetBits(iic_gpio,iic_scl); //时钟低电平
	iic_sdaout();
	GPIO_ResetBits(iic_gpio,iic_sda); //数据低电平,表示有应答
	delay_us(2);
	GPIO_SetBits(iic_gpio,iic_scl);	  //时钟上升沿
	delay_us(2);
	GPIO_ResetBits(iic_gpio,iic_scl); //时钟下降沿
}

//===========================================================================
//Subroutine:	 iic_noact 	无应答或否认应答
//表示不再接收数据               
//===========================================================================
void iic_noack(void)
{
	GPIO_ResetBits(iic_gpio,iic_scl);  //时钟低电平
	iic_sdaout();
	GPIO_SetBits(iic_gpio,iic_sda);	   //数据高电平,表示无应答
	delay_us(2);
	GPIO_SetBits(iic_gpio,iic_scl);		//时钟上升沿
	delay_us(2);
	GPIO_ResetBits(iic_gpio,iic_scl);	//时钟下降沿
}					 				     

//===========================================================================
//Subroutine:	 iic_sendbyte 
//IIC发送一个字节
//===========================================================================
void iic_sendbyte(INT8U txd)
{                        
    INT8U t;   
	 iic_sdaout(); //SDA设置为输出，因为要发送数据	    
    GPIO_ResetBits(iic_gpio,iic_scl); //时钟低电平，只有SCL为低才可改变数据
    for(t=0;t<8;t++)//循环发送8个字节
    {              
 		if ((txd&0x80)>>7) 	GPIO_SetBits(iic_gpio,iic_sda); //发出第7-t位
		else GPIO_ResetBits(iic_gpio,iic_sda);
       txd<<=1; 	  
		delay_us(2);   
		GPIO_SetBits(iic_gpio,iic_scl); //拉高SCL，发数据
		delay_us(2); 
		GPIO_ResetBits(iic_gpio,iic_scl); //SCL恢复低电平，准备修改SDA	
		delay_us(2);
    }	 
} 	 
 
  
//===========================================================================
//Subroutine:	 iic_readbyte 
//读1个字节，ack=1时，读之后发送ACK，ack=0，读者后发送nACK   
//===========================================================================
INT8U iic_readbyte(INT8U ack)
{
	unsigned char i,receive=0;
	iic_sdain();//准备接收数据，SDA设置为输入
    for(i=0;i<8;i++ )			//循环接收8位
	{
        GPIO_ResetBits(iic_gpio,iic_scl); //SCL为低电平 
        delay_us(2);
		GPIO_SetBits(iic_gpio,iic_scl);   //SCL上升沿，开始读取数据
        receive<<=1;
        if(GPIO_ReadInputDataBit(iic_gpio, iic_sda)) //读取数据
			receive|=0x01; //如果读到的数据为1，receive最低位应为1  
		delay_us(1); 
    }					 
    if (ack) //如果需要应答
        iic_ack();//发送应答
    else
        iic_noack(); //发送无应答   
    return receive;
}

//===========================================================================
//Subroutine:	 at24c02_readbyte 
//在AT24C02指定地址读出一个数据
//返回值  :读到的数据   
//===========================================================================

INT8U at24c02_readbyte(INT16U addr)
{				  
	INT8U temp=0;		  	    																 
    iic_start();  						  //启动
	iic_sendbyte(0XA0);                   //发送器件地址0XA0	 
	iic_wait4ack(); 					  //等待应答
    iic_sendbyte(addr);   		          //发送字地址
	iic_wait4ack();	    				  //等待应答
	iic_start();  	 	   				  //产生一个起动条件
	iic_sendbyte(0XA1);           		  //发送地址0XA1进入接收模式			   
	iic_wait4ack();	 					  //等待应答
    temp=iic_readbyte(0);		   		  //接收数据
    iic_stop();	    					  //产生一个停止条件
	return temp;						  //返回接收的数据
}
//===========================================================================
//Subroutine:	 at24c02_writebyte 
//在AT24C02指定地址写一个数据
//返回值  :无   
//===========================================================================
void at24c02_writebyte(INT16U address,INT8U data)
{				   	  	    																 
    iic_start();  
	iic_sendbyte(0XA0);   		 //发送器件地址0XA0 
	iic_wait4ack();	   			 //等待应答
    iic_sendbyte(address%256);   //发送字地址
	iic_wait4ack(); 	 		 //等待应答								  		   
	iic_sendbyte(data);          //发送字节							   
	iic_wait4ack();  		     //等待应答	   
    iic_stop();					 //产生一个停止条件 
	delay_ms(5);
}


//===========================================================================
//Subroutine:	 at24c02_check 
//检查at24c02是否正常
//返回值: 0 正常；1 异常  
//===========================================================================

INT8U at24c02_check(void)
{
	INT8U temp;
	temp=at24c02_readbyte(verify_iic_address);	//避免每次开机都写at24c02	
	if (ISDEBUG) printf("\n2402 check:read value:%d\n",temp); //调试状态输出到串口显示读到的值		   
	if(temp==verify_iic_value)
		return verify_iic_value;  //正常		   
	else
	{
		at24c02_writebyte(verify_iic_address,85); //在地址255写85
		delay_ms(5);
	    temp=at24c02_readbyte(verify_iic_address); //读出
        //if (ISDEBUG) printf("\n 2402 check wrote! read value:%d\n",temp); 		   			  
		if(temp==verify_iic_value)return verify_iic_value; //读出正常
	}
	return temp; //写和读不一致，异常											  
}

//===========================================================================
//Subroutine:	 at24c02_read2buf 
//在AT24C02指定地址开始读一组数据到数组
//返回值  :无   
//===========================================================================


void at24c02_read(INT16U addr,INT8U *buf,INT16U len)
{
	while(len--)
	{
		*buf++=at24c02_readbyte(addr++);	
	}
}

//===========================================================================
//Subroutine:	 at24c02_writefrombuf 
//在AT24C02指定地址开始写数组内容
//返回值  :无   
//===========================================================================
  
void at24c02_write(INT16U addr,INT8U *buf,INT16U len)
{
	while(len--)
	{
		at24c02_writebyte(addr++,*buf++);
	}
}



