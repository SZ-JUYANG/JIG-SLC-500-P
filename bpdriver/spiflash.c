
#include "spiflash.h"

void spiflash_init(void)
{
	spi_initn(flash_spi);
	GPIO_ResetBits(spi2_selectgpio,spi2_selectpin );
	spiflash_sendrece(0xff);	
}
//��ȡspiflash��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
u8 spiflash_readreg(void)   
{  
	u8 rece=0;   
	spiflash_enable;                              //ʹ��   
	spiflash_sendrece(SPIFLASH_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
	rece=spiflash_sendrece(0Xff);                 //��ȡһ���ֽ�  
	spiflash_disable;                             //����     
	return rece;   
} 
//дspiflash״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void spiflash_writereg(u8 data)   
{   
	spiflash_enable;                            //ʹ��   
	spiflash_sendrece(SPIFLASH_WriteStatusReg); //����д״̬�Ĵ�������    
	spiflash_sendrece(data);               		//д��һ���ֽ�  
	spiflash_disable;                           //����     	      
}   
//spiflashдʹ��	
//��WEL��λ   
void spiflash_writeenable(void)   
{
	spiflash_enable;                            //ʹ��   
    spiflash_sendrece(SPIFLASH_WriteEnable);    //����дʹ��  
	spiflash_disable;                           //����     	      
} 
//spiflashд��ֹ	
//��WEL����  
void spiflash_writedisable(void)   
{  
	spiflash_enable;                            //ʹ��   
    spiflash_sendrece(SPIFLASH_WriteDisable);   //����д����ָ��    
	spiflash_disable;                           //����     	      
} 		
	    
//��ȡоƬID SPIFLASH ��ID
u16 spiflash_readid(void)
{
	u16 rece = 0;	  
	spiflash_enable;				    
	spiflash_sendrece(0x90);//���Ͷ�ȡID����	    
	spiflash_sendrece(0x00); 	    
	spiflash_sendrece(0x00); 	    
	spiflash_sendrece(0x00); 	 			   
	rece|=spiflash_sendrece(0xFF)<<8;  
	rece|=spiflash_sendrece(0xFF);	 
	spiflash_disable;				    
	return rece;
}   		    
//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//buf:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void spiflash_read2buf(u8* buf,u32 startaddr,u16 len)   
{ 
 	u16 i;    												    
	spiflash_enable;                            //ʹ��   
    spiflash_sendrece(SPIFLASH_ReadData);         //���Ͷ�ȡ����   
    spiflash_sendrece((u8)(startaddr>>16));  //����24bit��ַ    
    spiflash_sendrece((u8)(startaddr>>8));   
    spiflash_sendrece((u8)startaddr);   
    for(i=0;i<len;i++)
	{ 
        buf[i]=spiflash_sendrece(0XFF);   //ѭ������  
    }
	spiflash_disable;                            //����     	      
}  
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//buf:���ݴ洢��
//startaddr:��ʼд��ĵ�ַ(24bit)
//len:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void spiflash_write2page(u8* buf,u32 startaddr,u16 len)
{
 	u16 i;  
    spiflash_writeenable();                    //д���� 
	spiflash_enable;                            //ʹ��   
    spiflash_sendrece(SPIFLASH_PageProgram);    //����дҳ����   
    spiflash_sendrece((u8)(startaddr>>16)); 	//����24bit��ַ    
    spiflash_sendrece((u8)(startaddr>>8));   
    spiflash_sendrece((u8)startaddr);   
    for(i=0;i<len;i++)spiflash_sendrece(buf[i]);//ѭ��д��  
	spiflash_disable;                            //���� 
	spiflash_wait4busy();					   //�ȴ�д�����
} 
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//buf:���ݴ洢��
//startaddr:��ʼд��ĵ�ַ(24bit)
//len:Ҫд����ֽ���(���65535)
//CHECK OK
void spiflash_writewithoutcheck(u8* buf,u32 startaddr,u16 len)   
{ 			 		 
	u16 remain;	   
	remain=256-startaddr%256; //��ҳʣ����ֽ���		 	    
	if(len<=remain)remain=len;//������256���ֽ�
	while(1)
	{	   
		spiflash_write2page(buf,startaddr,remain);
		if(len==remain)break;//д�������
	 	else 
		{
			buf+=remain;
			startaddr+=remain;	

			len-=remain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(len>256)remain=256; //һ�ο���д��256���ֽ�
			else remain=len; 	  //����256���ֽ���
		}
	};	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//buf:���ݴ洢��
//startaddr:��ʼд��ĵ�ַ(24bit)
//len:Ҫд����ֽ���(���65535)  		   
u8 spiflashbuf[4096];
void spiflash_write(u8* buf,u32 startaddr,u16 len)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	secpos=startaddr/4096;//������ַ 0~511 for SPIFLASH16
	secoff=startaddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   

	if(len<=secremain)secremain=len;//������4096���ֽ�
	while(1) 
	{	
		spiflash_read2buf(spiflashbuf,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(spiflashbuf[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			spiflash_erasesector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				spiflashbuf[i+secoff]=buf[i];	  
			}
			spiflash_writewithoutcheck(spiflashbuf,secpos*4096,4096);//д����������  

		}else spiflash_writewithoutcheck(buf,startaddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(len==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	buf+=secremain;  //ָ��ƫ��
			startaddr+=secremain;//д��ַƫ��	   
		   	len-=secremain;				//�ֽ����ݼ�
			if(len>4096)secremain=4096;	//��һ����������д����
			else secremain=len;			//��һ����������д����
		}	 
	};	 	 
}
//��������оƬ
//��Ƭ����ʱ��:
//SPIFLASH16:25s 
//SPIFLASH32:40s 
//SPIFLASH64:40s 
//�ȴ�ʱ�䳬��...
void spiflash_erasechip(void)   
{                                             
    spiflash_writeenable();                  //SET WEL 
    spiflash_wait4busy();   
  	spiflash_enable;                            //ʹ��   
    spiflash_sendrece(SPIFLASH_ChipErase);        //����Ƭ��������  
	spiflash_disable;                            //����     	      
	spiflash_wait4busy();   				   //�ȴ�оƬ��������
}   
//����һ������
//Dst_Addr:������ַ 0~511 for SPIFLASH16
//����һ��ɽ��������ʱ��:150ms
void spiflash_erasesector(u32 startaddr)   
{   
	startaddr*=4096;
    spiflash_writeenable();                  //SET WEL 	 
    spiflash_wait4busy();   
  	spiflash_enable;                            //ʹ��   
    spiflash_sendrece(SPIFLASH_SectorErase);      //������������ָ�� 
    spiflash_sendrece((u8)((startaddr)>>16));  //����24bit��ַ    
    spiflash_sendrece((u8)((startaddr)>>8));   
    spiflash_sendrece((u8)startaddr);  
	spiflash_disable;                            //����     	      
    spiflash_wait4busy();   				   //�ȴ��������
}  
//�ȴ�����
void spiflash_wait4busy(void)   
{   
	while ((spiflash_readreg()&0x01)==0x01);   // �ȴ�BUSYλ���
}  
//�������ģʽ
void spiflash_powerdown(void)   
{ 
  	spiflash_enable;                            //ʹ��   
    spiflash_sendrece(SPIFLASH_PowerDown);        //���͵�������  
	spiflash_disable;                            //����     	      
    delay_us(3);                               //�ȴ�TPD  
}   
//����
void spiflash_wakeup(void)   
{  
  	spiflash_enable;                            //ʹ��   
    spiflash_sendrece(SPIFLASH_ReleasePowerDown);   //  send SPIFLASH_PowerDown command 0xAB    
	spiflash_disable;                            //����     	      
    delay_us(3);                               //�ȴ�TRES1
}   




