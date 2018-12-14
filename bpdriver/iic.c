
#include "iic.h"


//===========================================================================
//Subroutine:	 iic_init 
//IIC��ʼ��
//===========================================================================
void iic_init()
{
 	GPIO_InitTypeDef GPIO_InitStructure;
						    
	GPIO_InitStructure.GPIO_Pin = iic_scl|iic_sda;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //iic_scl|iic_sda�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(iic_gpio, &GPIO_InitStructure);
	GPIO_SetBits(iic_gpio,iic_scl|iic_sda);				//����Ϊ1

}
//===========================================================================
//Subroutine:	 iic_start 
//IIC���ݴ��������ź�
//SCLΪ�ߵ�ƽ��ʱ��SDA�½���
//===========================================================================
 void iic_start(void)
{
	iic_sdaout();    				 //SDA�����
	GPIO_SetBits(iic_gpio,iic_sda);	 //SDAΪ�ߵ�ƽ 	  
	GPIO_SetBits(iic_gpio,iic_scl);	 //SCLΪ�ߵ�ƽ
	delay_us(4);
 	GPIO_ResetBits(iic_gpio,iic_sda); //SDA�½���
}	  
//===========================================================================
//Subroutine:	 iic_stop 
//IIC���ݴ���ֹͣ�ź�
//SCLΪ�ߵ�ƽʱ��SDA������
//===========================================================================
void iic_stop(void)
{
	iic_sdaout();					  //SDA�����
	GPIO_ResetBits(iic_gpio,iic_sda); //SDAΪ�͵�ƽ
 	delay_us(4);
	GPIO_SetBits(iic_gpio,iic_scl);  //SCL�ߵ�ƽ
	GPIO_SetBits(iic_gpio,iic_sda);  //SDA�����أ�I2C�������ݴ��ͽ����ź�						   	
}
//===========================================================================
//Subroutine:	 iic_wait4ack 
//IIC�ȴ�Ӧ��
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
//===========================================================================

INT8U iic_wait4ack(void)
{
	INT8U times=0;
	iic_sdain();      //SDA����Ϊ����  
	GPIO_SetBits(iic_gpio,iic_sda);//sda���͸ߵ�ƽ
	delay_us(1);	   
	GPIO_SetBits(iic_gpio,iic_scl);	//ʱ�Ӹߵ�ƽ��������
	delay_us(1);	 
	while(GPIO_ReadInputDataBit(iic_gpio, iic_sda))	//�����ݣ�ֱ���ӻ�����Ӧ��
	{
		if(++times>IICWAITTIMEOUT)
		{
			iic_stop(); //��Ӧ��ֹͣ
			return 1;
		}
	}
	GPIO_ResetBits(iic_gpio,iic_scl);//ʱ������͵�ƽ���½��� 	   
	return 0;  
} 
//===========================================================================
//Subroutine:	 iic_act    ����Ӧ��
//��ʾ��������������
//===========================================================================

void iic_ack(void)
{
	GPIO_ResetBits(iic_gpio,iic_scl); //ʱ�ӵ͵�ƽ
	iic_sdaout();
	GPIO_ResetBits(iic_gpio,iic_sda); //���ݵ͵�ƽ,��ʾ��Ӧ��
	delay_us(2);
	GPIO_SetBits(iic_gpio,iic_scl);	  //ʱ��������
	delay_us(2);
	GPIO_ResetBits(iic_gpio,iic_scl); //ʱ���½���
}

//===========================================================================
//Subroutine:	 iic_noact 	��Ӧ������Ӧ��
//��ʾ���ٽ�������               
//===========================================================================
void iic_noack(void)
{
	GPIO_ResetBits(iic_gpio,iic_scl);  //ʱ�ӵ͵�ƽ
	iic_sdaout();
	GPIO_SetBits(iic_gpio,iic_sda);	   //���ݸߵ�ƽ,��ʾ��Ӧ��
	delay_us(2);
	GPIO_SetBits(iic_gpio,iic_scl);		//ʱ��������
	delay_us(2);
	GPIO_ResetBits(iic_gpio,iic_scl);	//ʱ���½���
}					 				     

//===========================================================================
//Subroutine:	 iic_sendbyte 
//IIC����һ���ֽ�
//===========================================================================
void iic_sendbyte(INT8U txd)
{                        
    INT8U t;   
	 iic_sdaout(); //SDA����Ϊ�������ΪҪ��������	    
    GPIO_ResetBits(iic_gpio,iic_scl); //ʱ�ӵ͵�ƽ��ֻ��SCLΪ�Ͳſɸı�����
    for(t=0;t<8;t++)//ѭ������8���ֽ�
    {              
 		if ((txd&0x80)>>7) 	GPIO_SetBits(iic_gpio,iic_sda); //������7-tλ
		else GPIO_ResetBits(iic_gpio,iic_sda);
       txd<<=1; 	  
		delay_us(2);   
		GPIO_SetBits(iic_gpio,iic_scl); //����SCL��������
		delay_us(2); 
		GPIO_ResetBits(iic_gpio,iic_scl); //SCL�ָ��͵�ƽ��׼���޸�SDA	
		delay_us(2);
    }	 
} 	 
 
  
//===========================================================================
//Subroutine:	 iic_readbyte 
//��1���ֽڣ�ack=1ʱ����֮����ACK��ack=0�����ߺ���nACK   
//===========================================================================
INT8U iic_readbyte(INT8U ack)
{
	unsigned char i,receive=0;
	iic_sdain();//׼���������ݣ�SDA����Ϊ����
    for(i=0;i<8;i++ )			//ѭ������8λ
	{
        GPIO_ResetBits(iic_gpio,iic_scl); //SCLΪ�͵�ƽ 
        delay_us(2);
		GPIO_SetBits(iic_gpio,iic_scl);   //SCL�����أ���ʼ��ȡ����
        receive<<=1;
        if(GPIO_ReadInputDataBit(iic_gpio, iic_sda)) //��ȡ����
			receive|=0x01; //�������������Ϊ1��receive���λӦΪ1  
		delay_us(1); 
    }					 
    if (ack) //�����ҪӦ��
        iic_ack();//����Ӧ��
    else
        iic_noack(); //������Ӧ��   
    return receive;
}

//===========================================================================
//Subroutine:	 at24c02_readbyte 
//��AT24C02ָ����ַ����һ������
//����ֵ  :����������   
//===========================================================================

INT8U at24c02_readbyte(INT16U addr)
{				  
	INT8U temp=0;		  	    																 
    iic_start();  						  //����
	iic_sendbyte(0XA0);                   //����������ַ0XA0	 
	iic_wait4ack(); 					  //�ȴ�Ӧ��
    iic_sendbyte(addr);   		          //�����ֵ�ַ
	iic_wait4ack();	    				  //�ȴ�Ӧ��
	iic_start();  	 	   				  //����һ��������
	iic_sendbyte(0XA1);           		  //���͵�ַ0XA1�������ģʽ			   
	iic_wait4ack();	 					  //�ȴ�Ӧ��
    temp=iic_readbyte(0);		   		  //��������
    iic_stop();	    					  //����һ��ֹͣ����
	return temp;						  //���ؽ��յ�����
}
//===========================================================================
//Subroutine:	 at24c02_writebyte 
//��AT24C02ָ����ַдһ������
//����ֵ  :��   
//===========================================================================
void at24c02_writebyte(INT16U address,INT8U data)
{				   	  	    																 
    iic_start();  
	iic_sendbyte(0XA0);   		 //����������ַ0XA0 
	iic_wait4ack();	   			 //�ȴ�Ӧ��
    iic_sendbyte(address%256);   //�����ֵ�ַ
	iic_wait4ack(); 	 		 //�ȴ�Ӧ��								  		   
	iic_sendbyte(data);          //�����ֽ�							   
	iic_wait4ack();  		     //�ȴ�Ӧ��	   
    iic_stop();					 //����һ��ֹͣ���� 
	delay_ms(5);
}


//===========================================================================
//Subroutine:	 at24c02_check 
//���at24c02�Ƿ�����
//����ֵ: 0 ������1 �쳣  
//===========================================================================

INT8U at24c02_check(void)
{
	INT8U temp;
	temp=at24c02_readbyte(verify_iic_address);	//����ÿ�ο�����дat24c02	
	if (ISDEBUG) printf("\n2402 check:read value:%d\n",temp); //����״̬�����������ʾ������ֵ		   
	if(temp==verify_iic_value)
		return verify_iic_value;  //����		   
	else
	{
		at24c02_writebyte(verify_iic_address,85); //�ڵ�ַ255д85
		delay_ms(5);
	    temp=at24c02_readbyte(verify_iic_address); //����
        //if (ISDEBUG) printf("\n 2402 check wrote! read value:%d\n",temp); 		   			  
		if(temp==verify_iic_value)return verify_iic_value; //��������
	}
	return temp; //д�Ͷ���һ�£��쳣											  
}

//===========================================================================
//Subroutine:	 at24c02_read2buf 
//��AT24C02ָ����ַ��ʼ��һ�����ݵ�����
//����ֵ  :��   
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
//��AT24C02ָ����ַ��ʼд��������
//����ֵ  :��   
//===========================================================================
  
void at24c02_write(INT16U addr,INT8U *buf,INT16U len)
{
	while(len--)
	{
		at24c02_writebyte(addr++,*buf++);
	}
}



