
#include <fontop.h>
//---------------------------------------------------------
// loadzk 装载字库到SPIFALSH
// startaddr 字库起始地址
// zkfile    字库文件
// hzzjs     每个汉字字节数
// hzcnt     一次读写汉字数
// 提示 字库文件中有一段空白，如果单纯连续将字库文件写入FLASH，
//		得不到正确的结果，本函数将字库文件转化为RA8875可以识别
//      的高通字库格式写入SPIFLASH  
//      本代码开源可使用，但需保留此处
//      转载或修改请注明！
//      卢有亮 2013/2/4编写	 
//      参考资料 《GT23L32S4W标准汉字字库芯片用户手册》
//---------------------------------------------------------

u8 zkbuf[2048]; 
u8 loadzk(u32 startaddress,char * zkfile,u32 hzzjs,u32 hzcnt)
{
	u8 step;   //指示阶段
	u16	line;
	u8 * p;	   //字节指针
	u32  totalreadcnt,totalhzcnt,totalrealhzcnt;
	//   读取字节总数 读取汉字总数 有效读取汉字总数
   	FIL file;
	FRESULT tff_res;
	u32 dancizjs;
	u32 addr,readcnt;			//一次读取的字节数
	u16 temp;
	dancizjs=hzcnt*hzzjs;	//计算一次读取的字节数	 72

    tff_res = f_open(&file,zkfile,FA_OPEN_EXISTING | FA_READ);	
	if(tff_res!=FR_OK)
    {
		
		reporterrf("%s NOT FOUND!",zkfile);
		//beep_run(2500);
		return(tff_res);
    }
//	p=malloc(dancizjs); 	//缓冲区，存储读取的字库片段
//	if (p==NULL)
//  	{
//		reporterr("malloc failed!");//分配缓冲区失败
//		beep_run(5000);
//		return 2;
//	}
    p=zkbuf;
	LcdClear(WHITE);
	line=10;
	LcdPrintf(10,line,RED,WHITE,"begin to write %s=%d",zkfile,totalrealhzcnt);
	lineadd;
	//开始读取汉字和写入汉字到SPIFLASH
	addr=startaddress;			//设置起始地址
	totalreadcnt=0;
	totalhzcnt=0;
	totalrealhzcnt=0;

	step=1;//1：读1区字符 2：读一区剩余 3：读汉字
    while(1)
	{ 
		f_read(&file, p, dancizjs,  &readcnt);	//读取字库数据到缓冲区
		totalreadcnt+=readcnt;
		totalhzcnt+=hzcnt;
		if (totalhzcnt<=cnt_yqzfs)//如果在一区
		{	
			spiflash_write(p,addr,readcnt);	  //写FLASH
			addr+=readcnt;					  //修改地址
			totalrealhzcnt+=hzcnt;			  //修改真实读取个数
		}
		else
		{
			if (step==1)//大于一区字符数，读到的写有用的部分
			{				
				step=2;
				spiflash_write(p,addr,(cnt_yqzfs-totalrealhzcnt)*hzzjs);
				//写剩余的一区字库内容到FLASH    
		 		totalrealhzcnt=cnt_yqzfs;//写完一区字符
				//beep_run(200);
			}
		} 
		if (step==3)//在第三步，连续读写
		{
			spiflash_write(p,addr,readcnt);	   //写SPIFLASH
			addr+=readcnt;					   //修改地址
			totalrealhzcnt+=hzcnt;	
		}
		
		if ((totalhzcnt>cnt_eqqszj)&&(step==2))
		{
			//如果读到汉字，且在第二步，那么读到的汉字在缓冲区尾部
			addr=startaddress+cnt_yqzfs*hzzjs; //这是字库里第一个汉字“啊”的地址
			temp=totalreadcnt/hzzjs-cnt_eqqszj;//计算包含的汉字数
			if (temp>0) spiflash_write(p+dancizjs-temp*hzzjs,addr,temp*hzzjs);  
			//计算缓冲区开始有汉字的地址为p+dancizjs-temp*hzzjs
			totalrealhzcnt+=temp;
			//beep_run(500);
			step=3;	
			//写入字数 temp
			addr+=temp*hzzjs;	//修改地址
		}
		LcdPrintf(10,line,RED,WHITE,"W25Q64 Write count=%d",totalrealhzcnt);
		lineadd;
		if (totalhzcnt>8177) break; 
		//8177为1区加2区加两区间空数据区的总和可包含的字体个数，最后一次写将超过这
		//个范围不过不影响读写字库
    } 
	//free(p);   //一定要释放缓冲区！否则要造成内存泄漏！
	return 0;
}

u8 loadasc(u32 startaddress,char * zkfile,u32 hzzjs,u32 hzcnt)
{
	//u8 step;   //指示阶段
	u16	line;
	u8 * p;	   //字节指针
	u32  totalreadcnt,totalhzcnt,totalrealhzcnt;
	//   读取字节总数 读取汉字总数 有效读取汉字总数
   	FIL file;
	FRESULT tff_res;
	u32 dancizjs;
	u32 addr,readcnt;			//一次读取的字节数

	dancizjs=hzcnt*hzzjs;	//计算一次读取的字节数

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
	//开始读取汉字和写入汉字到SPIFLASH
	addr=startaddress;			//设置起始地址
	totalreadcnt=0;
	totalhzcnt=0;
	totalrealhzcnt=0;
	LcdPrintf(10,line,RED,WHITE,"begin to write %s=%d",zkfile,totalrealhzcnt);
	lineadd;
	f_read(&file, p, 32*hzzjs,  &readcnt);
    while(1)
	{ 
		f_read(&file, p, dancizjs, &readcnt);	//读取字库数据到缓冲区
		totalreadcnt+=readcnt;
		totalhzcnt+=hzcnt;
		if (totalhzcnt<=96)//如果未读完
		{	
			spiflash_write(p,addr,readcnt);	  //写FLASH
			addr+=readcnt;					  //修改地址
			totalrealhzcnt+=hzcnt;			  //修改真实读取个数
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

