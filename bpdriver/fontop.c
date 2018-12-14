
#include <fontop.h>
//---------------------------------------------------------
// loadzk װ���ֿ⵽SPIFALSH
// startaddr �ֿ���ʼ��ַ
// zkfile    �ֿ��ļ�
// hzzjs     ÿ�������ֽ���
// hzcnt     һ�ζ�д������
// ��ʾ �ֿ��ļ�����һ�οհף���������������ֿ��ļ�д��FLASH��
//		�ò�����ȷ�Ľ�������������ֿ��ļ�ת��ΪRA8875����ʶ��
//      �ĸ�ͨ�ֿ��ʽд��SPIFLASH  
//      �����뿪Դ��ʹ�ã����豣���˴�
//      ת�ػ��޸���ע����
//      ¬���� 2013/2/4��д	 
//      �ο����� ��GT23L32S4W��׼�����ֿ�оƬ�û��ֲᡷ
//---------------------------------------------------------

u8 zkbuf[2048]; 
u8 loadzk(u32 startaddress,char * zkfile,u32 hzzjs,u32 hzcnt)
{
	u8 step;   //ָʾ�׶�
	u16	line;
	u8 * p;	   //�ֽ�ָ��
	u32  totalreadcnt,totalhzcnt,totalrealhzcnt;
	//   ��ȡ�ֽ����� ��ȡ�������� ��Ч��ȡ��������
   	FIL file;
	FRESULT tff_res;
	u32 dancizjs;
	u32 addr,readcnt;			//һ�ζ�ȡ���ֽ���
	u16 temp;
	dancizjs=hzcnt*hzzjs;	//����һ�ζ�ȡ���ֽ���	 72

    tff_res = f_open(&file,zkfile,FA_OPEN_EXISTING | FA_READ);	
	if(tff_res!=FR_OK)
    {
		
		reporterrf("%s NOT FOUND!",zkfile);
		//beep_run(2500);
		return(tff_res);
    }
//	p=malloc(dancizjs); 	//���������洢��ȡ���ֿ�Ƭ��
//	if (p==NULL)
//  	{
//		reporterr("malloc failed!");//���仺����ʧ��
//		beep_run(5000);
//		return 2;
//	}
    p=zkbuf;
	LcdClear(WHITE);
	line=10;
	LcdPrintf(10,line,RED,WHITE,"begin to write %s=%d",zkfile,totalrealhzcnt);
	lineadd;
	//��ʼ��ȡ���ֺ�д�뺺�ֵ�SPIFLASH
	addr=startaddress;			//������ʼ��ַ
	totalreadcnt=0;
	totalhzcnt=0;
	totalrealhzcnt=0;

	step=1;//1����1���ַ� 2����һ��ʣ�� 3��������
    while(1)
	{ 
		f_read(&file, p, dancizjs,  &readcnt);	//��ȡ�ֿ����ݵ�������
		totalreadcnt+=readcnt;
		totalhzcnt+=hzcnt;
		if (totalhzcnt<=cnt_yqzfs)//�����һ��
		{	
			spiflash_write(p,addr,readcnt);	  //дFLASH
			addr+=readcnt;					  //�޸ĵ�ַ
			totalrealhzcnt+=hzcnt;			  //�޸���ʵ��ȡ����
		}
		else
		{
			if (step==1)//����һ���ַ�����������д���õĲ���
			{				
				step=2;
				spiflash_write(p,addr,(cnt_yqzfs-totalrealhzcnt)*hzzjs);
				//дʣ���һ���ֿ����ݵ�FLASH    
		 		totalrealhzcnt=cnt_yqzfs;//д��һ���ַ�
				//beep_run(200);
			}
		} 
		if (step==3)//�ڵ�������������д
		{
			spiflash_write(p,addr,readcnt);	   //дSPIFLASH
			addr+=readcnt;					   //�޸ĵ�ַ
			totalrealhzcnt+=hzcnt;	
		}
		
		if ((totalhzcnt>cnt_eqqszj)&&(step==2))
		{
			//����������֣����ڵڶ�������ô�����ĺ����ڻ�����β��
			addr=startaddress+cnt_yqzfs*hzzjs; //�����ֿ����һ�����֡������ĵ�ַ
			temp=totalreadcnt/hzzjs-cnt_eqqszj;//��������ĺ�����
			if (temp>0) spiflash_write(p+dancizjs-temp*hzzjs,addr,temp*hzzjs);  
			//���㻺������ʼ�к��ֵĵ�ַΪp+dancizjs-temp*hzzjs
			totalrealhzcnt+=temp;
			//beep_run(500);
			step=3;	
			//д������ temp
			addr+=temp*hzzjs;	//�޸ĵ�ַ
		}
		LcdPrintf(10,line,RED,WHITE,"W25Q64 Write count=%d",totalrealhzcnt);
		lineadd;
		if (totalhzcnt>8177) break; 
		//8177Ϊ1����2��������������������ܺͿɰ�����������������һ��д��������
		//����Χ������Ӱ���д�ֿ�
    } 
	//free(p);   //һ��Ҫ�ͷŻ�����������Ҫ����ڴ�й©��
	return 0;
}

u8 loadasc(u32 startaddress,char * zkfile,u32 hzzjs,u32 hzcnt)
{
	//u8 step;   //ָʾ�׶�
	u16	line;
	u8 * p;	   //�ֽ�ָ��
	u32  totalreadcnt,totalhzcnt,totalrealhzcnt;
	//   ��ȡ�ֽ����� ��ȡ�������� ��Ч��ȡ��������
   	FIL file;
	FRESULT tff_res;
	u32 dancizjs;
	u32 addr,readcnt;			//һ�ζ�ȡ���ֽ���

	dancizjs=hzcnt*hzzjs;	//����һ�ζ�ȡ���ֽ���

    tff_res = f_open(&file,zkfile,FA_OPEN_EXISTING | FA_READ);	
	if(tff_res!=FR_OK)
    {
		
		reporterrf("%s NOT FOUND!",zkfile);
		beep_run(2500);
		return(tff_res);
    }

	p=zkbuf;
	LcdClear(WHITE);
	line=10;
	//��ʼ��ȡ���ֺ�д�뺺�ֵ�SPIFLASH
	addr=startaddress;			//������ʼ��ַ
	totalreadcnt=0;
	totalhzcnt=0;
	totalrealhzcnt=0;
	LcdPrintf(10,line,RED,WHITE,"begin to write %s=%d",zkfile,totalrealhzcnt);
	lineadd;
	f_read(&file, p, 32*hzzjs,  &readcnt);
    while(1)
	{ 
		f_read(&file, p, dancizjs, &readcnt);	//��ȡ�ֿ����ݵ�������
		totalreadcnt+=readcnt;
		totalhzcnt+=hzcnt;
		if (totalhzcnt<=96)//���δ����
		{	
			spiflash_write(p,addr,readcnt);	  //дFLASH
			addr+=readcnt;					  //�޸ĵ�ַ
			totalrealhzcnt+=hzcnt;			  //�޸���ʵ��ȡ����
		}
		else
		{
			spiflash_write(p,addr,96-totalrealhzcnt);
			totalrealhzcnt=96;	
			break;
		}
		LcdPrintf(10,line,RED,WHITE,"W25Q64 Write count=%d",totalrealhzcnt);
		lineadd;	
	}
	return(0);
}

