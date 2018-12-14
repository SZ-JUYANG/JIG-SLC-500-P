
#include <tfcard.h>

					   
extern u8  TFCARDTYPE;//TF�������� 

//===========================================================================
//TF����ʼ����ʱ��,Ӧ�����ڵ���ģʽ
//===========================================================================
void tfcard_slow(void)
{
 	spi_setspeed(tfcard_spi,SPI_BaudRatePrescaler_256);//���õ�����ģʽ	
}
//===========================================================================
//TF������������ʱ��,�����ڸ���ģʽ
//===========================================================================
void tfcard_fast(void)
{
 	spi_setspeed(tfcard_spi,SPI_BaudRatePrescaler_2);//���õ�����ģʽ	
}
//===========================================================================
//ȡ��ѡ��,�ͷ�SPI����
//===========================================================================
void tfcard_disselect(void)
{
	tfcard_disable;
	tfcard_sendrece(0xff);//�ṩ�����8��ʱ��
}
//===========================================================================
//ѡ��TF��,���ҵȴ���׼��OK
//����ֵ:0,�ɹ�;1,ʧ��;
//===========================================================================
u8 tfcard_Select(void)
{
	tfcard_enable;
	if(tfcard_wait4ready()==0)
	{				
		if (ISDEBUG) printf("\ntfcard_waitforready suc��");
		return 0;//�ȴ��ɹ�
	}
	tfcard_disselect();
	if  (ISDEBUG) printf("\n tf card_WAUTREADY fail");	
	return 1;//�ȴ�ʧ��
}
//===========================================================================
//��������ʱ�ӣ����õ�ȫ1��׼����
//����ֵ:0,׼������;1,��ʱ
//===========================================================================
u8 tfcard_wait4ready(void)
{
	u32 timeout=0xFFFFFF;
	do
	{
		//delay_ms(500);
		if(tfcard_sendrece(0XFF)==0xFF) return 0;			  	
	}while(--timeout);
	return 1;          //��ʱ
}
//===========================================================================
//ȡ��TF��ָ����Ӧ������ʱ�޷�ȡ������ʧ��
//����Response:Ҫ�õ��Ļ�Ӧֵ
//����ֵ:0,�ɹ��õ���Ӧ
//       1,δ�õ���ȷ�Ļ�Ӧֵ
//===========================================================================
u8 tfcard_getresponse(u8 response)
{
	u16 timeout=0xFFF;//�ȴ�����	   						  
	while ((tfcard_sendrece(0XFF)!=response)&&timeout) --timeout; //�ȴ��õ���Ӧ  	  
	if (timeout==0) return 1;									  //��ʱ   
	return 0;						                              //�õ���Ӧ
}
//===========================================================================
//��TF����ȡһ�����ݰ�������
//buf:���ݻ�����
//len:Ҫ��ȡ�����ݳ���.
//����ֵ:0,�ɹ�
//       ����,ʧ��;	
//===========================================================================
u8 tfcard_recevedata(u8*buf,u16 len)
{			  	  
	if(tfcard_getresponse(BEGINSIGLE))return 1; //�ȴ�TF������������ʼ�ֽ�
    while(len--)						  		//��ʼ����len������
    {
        *buf=tfcard_sendrece(0xFF);				//8��ʱ�����ڽ���һ���ֽ�
        buf++;
    }
    //����αCRC
    tfcard_sendrece(0xFF);
    tfcard_sendrece(0xFF);									  					    
    return 0;			//��ȡ�ɹ�
}
//===========================================================================
//���������������TF��д��һ���� 512�ֽ�
//��д��ʼλ��Ȼ����512���ֽڣ�Ȼ����2���ֽ�αУ��ֵ�����8��ʱ��  
//buf:���ݻ�����
//beginbyte:��ʼ�ֽ�
//����ֵ:0,�ɹ�;����,ʧ��;	
//===========================================================================
u8 tfcard_sendblock( u8 * buf,u8 beginbyte)
{	
	u16 t;		  	  
	if(tfcard_wait4ready())return 1;//����1 �ȴ���ʱ
	tfcard_sendrece(beginbyte);		//������ʼ�ֽڻ�����ַ�
	if(beginbyte!=MULSTOP)			//������ǽ����ַ�����ִ�з��ͣ�������򷵻�
	{
		for(t=0;t<512;t++)tfcard_sendrece(buf[t]);//����512���ֽ�
	    tfcard_sendrece(0xFF);//αУ��ֵ
	    tfcard_sendrece(0xFF);
		t=tfcard_sendrece(0xFF);//��8��ʱ�ӣ�ȡ��R1����ֵ
		if((t&0x1F)!=0x05)return 2;	//��Ӧ����									  					    
	}						 									  					    
    return 0;//д��ɹ�
}
//===========================================================================
//��TF������һ������
//����: u8 cmd   ���� 
//      u32 arg  4���ֽڵ��������
//      u8 crc   crcУ��ֵ	   
//����ֵ:TF�����ص���Ӧ	
//===========================================================================														  
u8 tfcard_sendcommand(u8 cmd, u32 arg, u8 crc)
{
    u8 r1;	
	u8 timeout=0x1F; 
	tfcard_disselect();//�ø�Ƭѡ
//	if(tfcard_Select())return 0XFF;//����Ƭѡ
    tfcard_sendrece(cmd | 0x40);//д������ ��0x40(0100 0000)��������������λ
    tfcard_sendrece(arg >> 24); //д�������31-24λ
    tfcard_sendrece(arg >> 16);	//д�������23-16λ
    tfcard_sendrece(arg >> 8);	//д�������15-8λ
    tfcard_sendrece(arg);	  	//д�������7-0λ
    tfcard_sendrece(crc); 	    //дУ��λ ��CMD0ָ���ʱ�򣬲�����CRCУ�飬�������д
	if(cmd==CMD12)  tfcard_sendrece(0xff);//�����ֹͣͨ��ָ��෢һ��ʱ��
    //�ȴ���Ӧ����ʱ�˳�
	do
	{
		r1=tfcard_sendrece(0xFF);
	}while((r1&0X80) && timeout--);	//����ʱ��ֱ���ӵ�R1��Ӧ	 
    return r1;	  //����R1��Ӧ
}	
//===========================================================================	    																			  
//��ȡTF����CID��Ϣ��������������Ϣ
//����: u8 *cid_data(���CID���ڴ棬����16Byte��	  
//����ֵ:0��NO_ERR
//		 1������														   
//===========================================================================
u8 tfcard_getid(u8 *cid_data)
{
    u8 r1;	   
    r1=tfcard_sendcommand(CMD10,0,0x01); //��CMD10�����CID
    if(r1==0x00)
		r1=tfcard_recevedata(cid_data,16);//����16���ֽڵ�����	 
	tfcard_disselect();//ȡ��Ƭѡ
	if(r1)return 1;
	else return 0;
}																				
//===========================================================================  
//��ȡTF����CSD��Ϣ�������������ٶ���Ϣ
//����:u8 *cid_data(���CID���ڴ棬����16Byte��	    
//����ֵ:0��NO_ERR
//		 1������														   
//===========================================================================
u8 tfcard_getcsd(u8 *ctfcard_data)
{
    u8 r1;	 
    r1=tfcard_sendcommand(CMD9,0,0x01);//��CMD9�����CSD
    if(r1==0)
    	r1=tfcard_recevedata(ctfcard_data, 16);//����16���ֽڵ����� 
	tfcard_disselect();//ȡ��Ƭѡ
	if(r1)return 1;
	else return 0;
}  
//===========================================================================
//��ȡTF����������������������   
//����ֵ:0�� ȡ�������� 
//       ����:TF��������(������/512�ֽ�)
//ÿ�������ֽ�����Ϊ512����Ϊ�������512�����ʼ������ͨ��.						
//===========================================================================								  
u32 tfcard_getsectorcount(void)
{
    u8 csd[16];
    u32 Capacity;  
    u8 n;
	u16 csize;  					    
	//ȡCSD��Ϣ������ڼ��������0
    if(tfcard_getcsd(csd)!=0) return 0;	    
    //���ΪSDHC�����������淽ʽ����
    if((csd[0]&0xC0)==0x40)	 //V2.00�Ŀ�
    {	
		csize = csd[9] + ((u16)csd[8] << 8) + 1;
		Capacity = (u32)csize << 10;//�õ�������	 		   
    }else//V1.XX�Ŀ�
    {	
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
		Capacity= (u32)csize << (n - 9);//�õ�������   
    }
    return Capacity;
}
//===========================================================================

//�ж�TF���Ƿ����
//===========================================================================
u8 tfcard_judgeidle(void)
{
	u16 i;
	u8 retry;	   	  
    for(i=0;i<0xf00;i++);//����ʱ���ȴ�TF���ϵ����	 
    //�Ȳ���>74�����壬��TF���Լ���ʼ�����
    for(i=0;i<10;i++)tfcard_sendrece(0xFF); 
    //-----------------TF����λ��idle��ʼ-----------------
    //ѭ����������CMD0��ֱ��TF������0x01,����IDLE״̬
    //��ʱ��ֱ���˳�
    retry = 0;
    do
    {	   
        //����CMD0����TF������IDLE״̬������SPIģʽ
        i = tfcard_sendcommand(CMD0, 0, 0x95);
        retry++;
    }while((i!=0x01)&&(retry<200));
    //����ѭ���󣬼��ԭ�򣺳�ʼ���ɹ���or ���Գ�ʱ��
    if(retry==200)return 1; //ʧ��
	return 0;//�ɹ�	 						  
}
//===========================================================================
//��ʼ��TF��
//===========================================================================
u8 tfcard_init(void)
{
    u8 r1;      // ���TF���ķ���ֵ
    u16 timeout;  // �������г�ʱ����
    u8 buf[4];  
	u16 i;

	spi_initn(tfcard_spi);		//��ʼ��IO
 	tfcard_slow();	//���õ�����ģʽ 
    //�����ϵ������SD ������Idle ״̬������74 ��ʱ�����ں���ܿ�ʼ���ߴ��䡣
	for(i=0;i<10;i++)
	{
	//	if (ISDEBUG) printf("tfcard_sendrece times %d\n",i);
		tfcard_sendrece(0XFF);//����80������
	}
	timeout=20;
	do
	{
		r1=tfcard_sendcommand(CMD0,0,0x95);//����IDLE״̬
	}while((r1!=0X01) && timeout--); //�ж���Ӧ���λ�Ƿ�Ϊ1
	TFCARDTYPE=0;	//Ĭ���޿�
	if(r1==0X01)
	{
		if(tfcard_sendcommand(CMD8,0x1AA,0x87)==1)
		{
 			for(i=0;i<4;i++)buf[i]=tfcard_sendrece(0XFF);	//��ÿ��ĵ�ѹ��Ϣ			
			if(buf[2]==0X01&&buf[3]==0XAA)                 //���Ƿ�֧��2.7~3.6V
			{
				timeout=0XFFFF;
				do
				{
					tfcard_sendcommand(CMD55,0,0X01);	         //����CMD55
					r1=tfcard_sendcommand(ACMD41,0x40000000,0X01);//����CMD41
				}while(r1&&timeout--);
			   	if(timeout&&tfcard_sendcommand(CMD58,0,0X01)==0)//����SD2.0���汾��ʼ
				{
					for(i=0;i<4;i++)buf[i]=tfcard_sendrece(0XFF);//�õ�OCRֵ
					if(buf[0]&0x40)TFCARDTYPE=TFCARDTYPE_V2HC;    //���CCS
					else TFCARDTYPE=TFCARDTYPE_V2;   
				}
				if (timeout!=0)
				{
					tfcard_fast();//����
					return 0;// TFCARDTYPE;
				}
			}			
		}else//SD V1.x/ MMC	V3
		{
			tfcard_sendcommand(CMD55,0,0X01);		//����CMD55
			r1=tfcard_sendcommand(ACMD41,0,0X01);	//����CMD41
			if(r1<=1)
			{		
				TFCARDTYPE=TFCARDTYPE_V1;
				timeout=0XFFFE;
				do //�ȴ��˳�IDLEģʽ
				{
					tfcard_sendcommand(CMD55,0,0X01);	//����CMD55
					r1=tfcard_sendcommand(ACMD41,0,0X01);//����CMD41
				}while(r1&&timeout--);
			}else
			{
				TFCARDTYPE=TFCARDTYPE_MMC;//MMC V3
				timeout=0XFFFE;
				do //�ȴ��˳�IDLEģʽ
				{											    
					r1=tfcard_sendcommand(CMD1,0,0X01);//����CMD1
				}while(r1&&timeout--);  
			}
			if(timeout==0||tfcard_sendcommand(CMD16,512,0X01)!=0)TFCARDTYPE=TFCARDTYPE_ERR;//����Ŀ�
			else {tfcard_fast();//����
	              return 0;
				 }  
		}
	}
	tfcard_disselect();//ȡ��Ƭѡ       
	tfcard_fast();//����
	return 1;    //TFCARDTYPE;//����TFCARD�汾

}	
	
//===========================================================================
//��TF��
//buf:���ݻ�����
//sector:����
//cnt:������
//����ֵ:0,ok;����,ʧ��.
//===========================================================================
u8 tfcard_readsectors(u8*buf,u32 sector,u8 cnt)
{
	u8 r1;
	if(TFCARDTYPE!=TFCARDTYPE_V2HC)sector <<= 9;//ת��Ϊ�ֽڵ�ַ
	if(cnt==1)
	{  	    
		r1=tfcard_sendcommand(CMD17,sector,0x01);//������
		if(r1==0)//ָ��ͳɹ�
			r1=tfcard_recevedata(buf,512);//����512���ֽ�	   
	}else
	{
		r1=tfcard_sendcommand(CMD18,sector,0x01);//����������
		do
		{
			r1=tfcard_recevedata(buf,512);//����512���ֽ�	 
			buf+=512;  
		}while(--cnt && r1==0); 	
		tfcard_sendcommand(CMD12,0,0X01);	//����ֹͣ����
	}   
	tfcard_disselect();//ȡ��Ƭѡ
	return r1;//
}

//===========================================================================
//дTF��
//buf:���ݻ�����
//sector:��ʼ����
//cnt:������
//����ֵ:0 �ɹ�
//===========================================================================
u8 tfcard_writesectors( BYTE*buf,u32 sector,u8 cnt)
{
	u8 r1;
	if(TFCARDTYPE!=TFCARDTYPE_V2HC)sector<<=9;//ת��Ϊ�ֽڵ�ַ
	if(cnt==1)
	{
		r1=tfcard_sendcommand(CMD24,sector,0X01);//����д��������
		if(r1==0)//ָ��ͳɹ�
			r1=tfcard_sendblock(buf,BEGINSIGLE);//�������ݿ飬д512���ֽ�	   
	}else
	{
 		r1=tfcard_sendcommand(CMD25,sector,0X01);//����д����
		if(r1==0)
		{
			do
			{
				r1=tfcard_sendblock(buf,BEGINMUL);//����512���ֽ�	 
				buf+=512;  
			}while(--cnt && r1==0);
			r1=tfcard_sendblock(0,MULSTOP);//���ͽ������� 
		}
	}   
	tfcard_disselect();//ȡ��Ƭѡ
	return r1;
}	   
























	  






