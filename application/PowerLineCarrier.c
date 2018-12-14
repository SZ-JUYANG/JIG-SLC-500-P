
#include "PowerlineCarrier.h"
#include "usart.h"
#include "utility.h"
#include <string.h>
#include "ucos_ii.h"
#include "cal_test.h"
#include "GP9034T.H"
#include "main.h"
#define PLC_DEBUG
#ifdef PLC_DEBUG
#include "RA8875.h"
#endif
#ifndef PRODUCT_TYPE
  #error "Please define PRODUCT_TYPE!"
#endif
static void PLC_DelayUs(u32 t)
{
	u8 i;
	u32 tt;
	for(i=0;i<70;i++)
	{ 
		tt=t;
	  while(tt--);
	}
}
#define PLC_COM         USART1
u8 PLC_pRecBuff[PLC_BUFFSIZE];
u16 PLC_RecCtn;
#define MIN_ASK_CMD_LEN  3
#define PLC_DelayMs(x)   OSTimeDlyHMSM(0,0,0,x)
#define TIMEOUTMS        
#define RS485_TX()       GPIO_SetBits(GPIOC,GPIO_Pin_6)
#define RS485_RX()       GPIO_ResetBits(GPIOC,GPIO_Pin_6)
#define RS485_NEMA_TX()  GPIO_SetBits(GPIOC,GPIO_Pin_5)
#define RS485_NEMA_RX()  GPIO_ResetBits(GPIOC,GPIO_Pin_5)
//PLC发送的数据计数
static u32 PLC_TXCouter=0;
//PLC接收的数据计数
static u32 PLC_RXCouter=0;
//其它函数发送过来的消息
static int *PLC_Msg;
//PLC退出发送和接收的消息
#define PLC_IsExit PLC_Msg[0]
#define PLC_EXIT  0x01
//主动退出返回的消息
#define PLC_EXIT_ERR 0x00
//检查是否主动退出
#define PLC_IfExit() if(PLC_IsExit==PLC_EXIT) return PLC_EXIT_ERR
	
typedef struct 
{
	u8 WR;  //只取0xa1和0xa2
	u8 CMD;
	u32 ID;
	u8 Ask;        //应答命令的响应码
	u8 AskDataLen; //应答数据的长度，不包括响应码和响应命令
	u8 AskData[20];
	u8 SendDataLen;//发送数据的长度，不包括命令类型和命令
	u8 SendData[30];
}PLC_CmdStruct;

//整形转换成char
static void PLC_Int2Ch(u8 *_pch,u16 _value)
{
	*_pch=_value>>8;
	*(_pch+1)=_value;
}

//接收到的数据转换成int
static u16 PLC_Ch2Int(u8 *_pch)
{
	return (*_pch<<8)+*(_pch+1);
}

//将char转换成浮点数
static float PLC_Ch2Float(u8 *_pch)
{
	union
	{
		float f;
		long l;
	}_data;
	_data.l=(_pch[0]<<24)+(_pch[1]<<16)+(_pch[2]<<8)+_pch[3];
	return _data.f;
}

//将浮点数转换成char
static void PLC_Float2Ch(u8 *pch,float _value)
{
	union
	{
		float f;
		long l;
	}_data;
	_data.f=_value;
	pch[0]=_data.l>>24;
	pch[1]=_data.l>>16;
	pch[2]=_data.l>>8;
	pch[3]=_data.l>>0;
}

static void PLC_SendID(u32 id,u8 *list)
{
	list[0]=id>>24;
	list[1]=id>>16;
	list[2]=id>>8;
	list[3]=id;
}


//初始化EN引脚
void RS485_Init()
{
	GPIO_InitTypeDef gpc_init;
	gpc_init.GPIO_Mode=GPIO_Mode_Out_PP;
	gpc_init.GPIO_Speed=GPIO_Speed_50MHz;
	gpc_init.GPIO_Pin=GPIO_Pin_6;
	GPIO_Init(GPIOC,&gpc_init);
	//RS485_RX();
#if PRODUCT_TYPE == 2
	gpc_init.GPIO_Pin=GPIO_Pin_5;
	GPIO_Init(GPIOC,&gpc_init);
#endif
}

static u8 PLC_CheckSum(u8 *pData,u8 len) //计算和校验
{
	u8 s_sum=0;
	u8 i=0;
	u8 ucTemp;
	for(i=0,s_sum=0;i<len;i++){
		ucTemp = pData[i];
		s_sum += ucTemp;
	}
	i = 9;
	return (u8)s_sum;
}

static u8 PLC_CheckXor(u8 *pData,u8 len)
{
	u8 s_XorSum=0,i=0;
	for(i=0;i<len;i++)
	{
		s_XorSum^=pData[i];
	}
	return s_XorSum;
}

//发送字符串
static s8 PLC_SendData(u8 *pData,u8 len)
{
	u8 i=0;
	RS485_TX();
	PLC_DelayMs(1);
	//printcmd(pData,len,11);
	for(i=0;i<len;i++)
	{
		USART_SendData(PLC_COM,pData[i]);    //发送一个字节
	  while(USART_GetFlagStatus(PLC_COM,USART_FLAG_TC)==RESET);//检测是否发送成功  
		PLC_IfExit();
	}
	PLC_DelayMs(1);
	RS485_RX();
	return 1;
}

//发送命令结构体
static s8 PLC__SendCmd(PLC_CmdStruct _Cmd)//将命令从串口发送出去
{
	u8 s_SendStr[40];
	u8 i=0,j=0,s_len;
	s_SendStr[i++]=_Cmd.ID>>24;
	s_SendStr[i++]=_Cmd.ID>>16;
	s_SendStr[i++]=_Cmd.ID>>8;
	s_SendStr[i++]=_Cmd.ID;
	if(_Cmd.WR==0xa1 || _Cmd.WR==0xa2)
	{
		s_SendStr[i++]=_Cmd.WR;
	}
	s_SendStr[i++]=_Cmd.CMD;
	for(j=0;j<_Cmd.SendDataLen;j++)
	{
		s_SendStr[i++]=_Cmd.SendData[j];
	}
	s_len=i-4;
	if(_Cmd.WR==0xa1)
	{
		s_SendStr[i++] = PLC_CheckXor(&s_SendStr[4],s_len); //计算异或校验
	}
	s_SendStr[i++] = PLC_CheckSum(&s_SendStr[4],s_len);	  //计算和校验
	s_len=i;
	return PLC_SendData(s_SendStr,s_len); //发送数据
}

//成功返回头码的下标,没有找到返回-1
static s8 PLC_FindHead(u8 _headdata,u8 *_findstr,u16 _len)
{
	u16 i;
	for(i=0;i<_len;i++)
	{
		if(_findstr[i]==_headdata)
		{
			return i;
		}
	}
	return RETURN_FAIL;
}

//接收数据
//_psenddata：保存数据的指针
//成功返回1，失败返回负数
//退出返回0；
static s8 PLC_RecCmd(PLC_CmdStruct *_psenddata) //接收PLC发送来的数据
{
	s16 s_HeadPost;
	u8 s_CkeckSum;
	u8 s_HaveHead,s_len;
	u16 i,j;
	RS485_RX();
	s_HaveHead=_psenddata->WR==0xa1 || _psenddata->WR==0xa2; //判断是否为校正命令
	//应答的命令长度：下面的2是响应码与和校验的长度，s_HaveHead表示响应命令的长度。
	s_len=2+s_HaveHead+_psenddata->AskDataLen;               //校正命令长度比调光命令多一个字节。
	for(j=0;j<400;j++) //等待10ms * 400 = 4s
	{
		//检查接收的数据长度
		if(PLC_RecCtn>=s_len){
			s_HeadPost=PLC_FindHead(_psenddata->Ask,PLC_pRecBuff,PLC_RecCtn);
			//检查头码
			if(s_HeadPost==RETURN_FAIL){ //查找应答命令响应码失败
				return RETURN_FAIL;
			}
			if (_psenddata->Ask != 0x93){
			    s_CkeckSum=PLC_CheckSum(&PLC_pRecBuff[s_HeadPost],_psenddata->AskDataLen+s_HaveHead+1);
				//检查校验和
				if(s_CkeckSum!=PLC_pRecBuff[s_HeadPost+_psenddata->AskDataLen+s_HaveHead+1])
				{
					return RETURN_FAIL;
				}
			}else{ //响应13命令的响应码0X93的处理
			    //新加坡查询指令返回的数据不计算帧头
			    s_CkeckSum=PLC_CheckSum(&PLC_pRecBuff[s_HeadPost + 1],15);
				//检查校验和
				if(s_CkeckSum!=PLC_pRecBuff[s_HeadPost+16])
				{
					return RETURN_FAIL;
				}
			}
			
			//检查响应命令是否对，s_HaveHead=0表示没有响应命令
			if((PLC_pRecBuff[s_HeadPost+s_HaveHead]!=_psenddata->CMD) && s_HaveHead)
			{
				return RETURN_FAIL;
			}
			
			//保存接收到的数据
			for(i=0;i<_psenddata->AskDataLen;i++)
			{
				_psenddata->AskData[i]=PLC_pRecBuff[s_HeadPost+s_HaveHead+1+i];
			}
			return RETURN_SUCCESS;
		}
		PLC_IfExit();
		PLC_DelayMs(10);
  }
	return RETURN_FAIL;
}

static s8 PLC_SendCmd(PLC_CmdStruct *_psenddata)
{
	u32 i=0;
	s8 err;
	//for(i=0;i<10;i++)
	//for(i=0;i<1;i++)
	{   //接收数据长度，接收数据缓冲区清零。
		PLC_RecCtn = 0;
		memset(PLC_pRecBuff,0,PLC_BUFFSIZE);//标准库函数，将PLC_pRecBuff的前PLC_BUFFSIZE个字节用0填充
		//发送命令出去
		PLC__SendCmd(*_psenddata);
		//等待回应
		if((err=PLC_RecCmd(_psenddata))==RETURN_SUCCESS) //等待回应，4s
		{
			#ifdef PLC_DEBUG
			//printcmd(PLC_pRecBuff,PLC_RecCtn);
			#endif
			//接收数据长度，接收数据缓冲区清零。
			PLC_RecCtn=0;
			memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
			//wzh  RS485_TX();
		}
		PLC_TXCouter++;
		if(err==RETURN_SUCCESS)
		{
			PLC_RXCouter++;
			PLC_DelayMs(950);
			return RETURN_SUCCESS;
		}
		PLC_IfExit();
	}
	//wzh  RS485_TX();
	PLC_DelayMs(950);
	return RETURN_FAIL;
}

//获取电源参数:输入输出功率电流电压，功率因数，亮度，电源状态
s8 PLC_GetPowerData(u32 _id,PLC_PowerData *_pSturct,void * _msg)
{
	PLC_CmdStruct s_cmd;
	s_cmd.ID=_id;
	s_cmd.WR=0;
	s_cmd.CMD=0x13;
	s_cmd.SendData[0]=0xc8;
	s_cmd.SendDataLen=1;
	s_cmd.Ask=0x93;
	s_cmd.AskDataLen=15;
	PLC_Msg=(int*)_msg;
	if(PLC_SendCmd(&s_cmd)==RETURN_SUCCESS)//发送命令，并等待回应。
	{  //读取PLC发送来的数据
		_pSturct->Temperature=s_cmd.AskData[0];
		_pSturct->Vi = ((u16)(s_cmd.AskData[6])<<8) | s_cmd.AskData[5];//PLC_Ch2Int(&s_cmd.AskData[2])/10.0;
		_pSturct->Ii = ((u16)(s_cmd.AskData[8])<<8) | s_cmd.AskData[7];//PLC_Ch2Int(&s_cmd.AskData[4])/1000.0;
		_pSturct->Pa = ((u16)(s_cmd.AskData[10])<<8) | s_cmd.AskData[9];//PLC_Ch2Int(&s_cmd.AskData[8])/10.0;
		return RETURN_SUCCESS;
	}
	PLC_IfExit();
	return RETURN_FAIL;
}
//恢复出厂设置
s16 PLC_SetFactory(u32 _id,u8 _value,void * _msg)
{
	PLC_CmdStruct s_cmd;
	s_cmd.ID=_id;
	s_cmd.WR=0xa1;
	s_cmd.CMD=1;
	s_cmd.SendData[0]=_value;
	s_cmd.SendDataLen=1;
	s_cmd.Ask=0x8a;
	s_cmd.AskDataLen=2;
	PLC_Msg=(int*)_msg;
	if(PLC_SendCmd(&s_cmd)==1) //发出命令，等待回应
	{
		return (s_cmd.AskData[0]<<8)+s_cmd.AskData[1];
	}
	PLC_IfExit();
	return -1;
}

//配置输出功率
s8 PLC_SetPower(u32 _id,float _value,void * _msg)
{
	PLC_CmdStruct s_cmd;	
	s_cmd.ID=_id;
	s_cmd.WR=0xa1;
	s_cmd.CMD=2;
	PLC_Int2Ch(&s_cmd.SendData[0],(u16)(_value*10));
	s_cmd.SendDataLen=2;
	s_cmd.Ask=0x8a;
	s_cmd.AskDataLen=1;
	PLC_Msg=(int*)_msg;
	if(PLC_SendCmd(&s_cmd)==1)
	{
		return s_cmd.AskData[0];
	}
	PLC_IfExit();
	return -1;
}

//配置输出电压
s8 PLC_SetVol(u32 _id,float _value,void * _msg)
{
	PLC_CmdStruct s_cmd;
	
	s_cmd.ID=_id;
	s_cmd.WR=0xa1;
	s_cmd.CMD=3;
	PLC_Int2Ch(&s_cmd.SendData[0],(u16)(_value*10));
	s_cmd.SendDataLen=2;
	s_cmd.Ask=0x8a;
	s_cmd.AskDataLen=1;
	PLC_Msg=(int*)_msg;
	if(PLC_SendCmd(&s_cmd)==1)
	{
		return s_cmd.AskData[0];
	}
	PLC_IfExit();
	return -1;
}

//配置输出电流
s8 PLC_SetCur(u32 _id,u16 _value,void * _msg)
{
	PLC_CmdStruct s_cmd;
	
	s_cmd.ID=_id;
	s_cmd.WR=0xa1;
	s_cmd.CMD=4;
	PLC_Int2Ch(&s_cmd.SendData[0],_value);
	s_cmd.SendDataLen=2;
	s_cmd.Ask=0x8a;
	s_cmd.AskDataLen=1;
	PLC_Msg=(int*)_msg;
	if(PLC_SendCmd(&s_cmd)==1)
	{
		return s_cmd.AskData[0];
	}
	PLC_IfExit();
	return -1;
}

//获取校验参数：功率，电流电压的斜率和截距
s8 PLC_GetCalData(u32 _id,PLC_CalStruct *_pGetData,void * _msg)
{
	PLC_CmdStruct s_cmd;
	
	s_cmd.ID=_id;
	s_cmd.WR=0xa2;
	s_cmd.CMD=0x03;
	s_cmd.SendDataLen=0;
	s_cmd.Ask=0x8b;
	s_cmd.AskDataLen=24;
	PLC_Msg=(int*)_msg;
	if(PLC_SendCmd(&s_cmd)==1)
	{
		_pGetData->PSlope=PLC_Ch2Float(&s_cmd.AskData[0]);
		_pGetData->PIntcpt=PLC_Ch2Float(&s_cmd.AskData[4]);
		_pGetData->VSlope=PLC_Ch2Float(&s_cmd.AskData[8]);
		_pGetData->VIntcpt=PLC_Ch2Float(&s_cmd.AskData[12]);
		_pGetData->ISlope=PLC_Ch2Float(&s_cmd.AskData[16]);
		_pGetData->IIntcpt=PLC_Ch2Float(&s_cmd.AskData[20]);
		return 1;
	}
	PLC_IfExit();
	return -1;
}

//获取校验状态，成功返回1，失败返回-1
s8 PLC_GetCalStatu(u32 _id,u16 * pd,void * _msg)
{
	PLC_CmdStruct s_cmd;
	s_cmd.ID=_id;
	s_cmd.WR=0xa2;
	s_cmd.CMD=1;
	s_cmd.SendDataLen=0;
	s_cmd.Ask=0x8b;
	s_cmd.AskDataLen=2;
	PLC_Msg=(int*)_msg;
	if(PLC_SendCmd(&s_cmd)==1)
	{
		*pd=(s_cmd.AskData[0]<<8)+s_cmd.AskData[1];
		return 1;
	}
	PLC_IfExit();
	return -1;
}

//读取电源的额定参数：功率，电压，电流
s8 PLC_GetNmnPrmt(u32 _id,PLC_PwrPrmtStruct *_pPrmt,void * _msg)
{
	PLC_CmdStruct s_cmd;
	
	s_cmd.ID=_id;
	s_cmd.WR=0xa2;
	s_cmd.CMD=2;
	s_cmd.SendDataLen=0;
	s_cmd.Ask=0x8b;
	s_cmd.AskDataLen=6;
	PLC_Msg=(int*)_msg;
	if(PLC_SendCmd(&s_cmd)==1)
	{
		_pPrmt->P=PLC_Ch2Int(&s_cmd.AskData[0])*0.01;
		_pPrmt->V=PLC_Ch2Int(&s_cmd.AskData[2])*0.01;
		_pPrmt->I=PLC_Ch2Int(&s_cmd.AskData[4]);
		return 1;
	}
	PLC_IfExit();
	return -1;
}

/*********************
返回值：
01H:成功
02H:失败
03H:超出范围
04H:无此项目
**********************/
//校正参数：功率，电压，电流,成功返回响应结果，否则返回-1
static s8 PLC_Cal(u32 _id, u8 _cmd, float _slop,float _intcpt)
{
	PLC_CmdStruct s_cmd;
	
	s_cmd.ID=_id;
	s_cmd.WR=0xa1;
	s_cmd.CMD=_cmd;
	s_cmd.SendDataLen=8;
	PLC_Float2Ch(&s_cmd.SendData[0],_slop);
	PLC_Float2Ch(&s_cmd.SendData[4],_intcpt);
	s_cmd.Ask=0x8a;
	s_cmd.AskDataLen=1;
	if(PLC_SendCmd(&s_cmd) == RETURN_SUCCESS)
	{
		return s_cmd.AskData[0];
	}
	PLC_IfExit();
	return RETURN_FAIL;
}
/*********************
返回值：
01H:成功
02H:失败
03H:超出范围
04H:无此项目
**********************/
//校正参数：功率，电压，电流,成功返回响应结果，否则返回-1
static s8 PLC_CalVIP(u32 _id, float P,float U, float I, unsigned short int ui0_10V_Repair)
{
	PLC_CmdStruct s_cmd;	
	s_cmd.ID=_id;
	s_cmd.WR = 0xa1;
	s_cmd.CMD = 0X05;
	s_cmd.SendDataLen = 14;
	PLC_Float2Ch(&s_cmd.SendData[0], P);   //输入功率
	PLC_Float2Ch(&s_cmd.SendData[4], U);   //输入电压
	PLC_Float2Ch(&s_cmd.SendData[8], I);   //输入电流
	s_cmd.SendData[12] = (unsigned char)(ui0_10V_Repair>>8);
	s_cmd.SendData[13] = (unsigned char)ui0_10V_Repair;
	s_cmd.Ask = 0x8a;    //应答码
	s_cmd.AskDataLen = 1;  //应答长度
	if(PLC_SendCmd(&s_cmd) == RETURN_SUCCESS){
		return s_cmd.AskData[0];
	}
	PLC_IfExit();
	return RETURN_FAIL;
}

s8 PLC_CalInputVIP(u32 _id, float P, float U, float I, unsigned short int ui0_10V_Repair, void * _msg) //校正输入电压、电流、功率函数
{
	PLC_Msg=(int*)_msg;
	return PLC_CalVIP(_id, P, U, I, ui0_10V_Repair);
}

s8 PLC_CalTrig(u32 _id,void * _msg) //
{
	PLC_CmdStruct s_cmd;
	PLC_Msg=(int*)_msg;
	
	s_cmd.ID=_id;
	s_cmd.WR=0xa1;
	s_cmd.CMD = 0X06;
	s_cmd.SendDataLen=1;
	s_cmd.SendData[0] = PRINT_DEVICE_CODE;
	s_cmd.Ask=0x8a;
	s_cmd.AskDataLen=1;
	if(PLC_SendCmd(&s_cmd) == RETURN_SUCCESS)
	{
		return s_cmd.AskData[0];
	}
	PLC_IfExit();
	
	
	return 1;
}
/**********************************
获取CSZ7759的测量频率或者采样的ADC值
成功：返回1(频率)或者返回2（AD值）
失败：返回-1
无此项目：返回4       
数据保存在pf里面
*************************************/
s8 PLC_GetInputF(u32 _id,PLC_PwrTestFStruct *pf,void * _msg)
{
	PLC_CmdStruct s_cmd;
	
	s_cmd.ID=_id;
	s_cmd.WR=0xa2;
	s_cmd.CMD=0x05;
	s_cmd.SendDataLen=0;
	s_cmd.Ask=0x8b;
	s_cmd.AskDataLen=7;
	PLC_Msg=(int*)_msg;
	if(PLC_SendCmd(&s_cmd)==RETURN_SUCCESS)
	{
		pf->P =PLC_Ch2Int(&s_cmd.AskData[1]);
		pf->V =PLC_Ch2Int(&s_cmd.AskData[3]);
		pf->I =PLC_Ch2Int(&s_cmd.AskData[5]);
		if(s_cmd.AskData[0]==1 || s_cmd.AskData[0]==2)
			return s_cmd.AskData[0];
	}
	if(s_cmd.AskData[0]==4)
		return 4;
	PLC_IfExit();
	return RETURN_FAIL;
}
/**********************************
获取采样的ADC值
成功：返回1(频率)或者返回2（AD值）
失败：返回-1
无此项目：返回4       
数据保存在pf里面
*************************************/
s8 PLC_GetOutputV(u32 _id,PLC_PwrTestFStruct *pf,void * _msg)
{
	PLC_CmdStruct s_cmd;
	
	s_cmd.ID=_id;
	s_cmd.WR=0xa2;
	s_cmd.CMD=0x06;
	s_cmd.SendDataLen=0;
	s_cmd.Ask=0x8b;
	s_cmd.AskDataLen=5;
	PLC_Msg=(int*)_msg;
	if(PLC_SendCmd(&s_cmd)==1)
	{
		//pf->P =PLC_Ch2Int(&s_cmd.AskData[1]);
		pf->V =PLC_Ch2Int(&s_cmd.AskData[1]);
		pf->I =PLC_Ch2Int(&s_cmd.AskData[3]);
		if(s_cmd.AskData[0]==1)
			return 1;
	}
	if(s_cmd.AskData[0]==4 || s_cmd.AskData[0]==2)
		return s_cmd.AskData[0];
	PLC_IfExit();
	return -1;
}

static u8 PLC_GetFC2(u8 *p,u8 len,u8 sel)
{
	u8 sum1=0,sum2=0,i;
	for(i=0;i<len;i++)
	{
		sum1=(sum1+p[i])%255;
		sum2=(sum1+sum2)%255;
	}
	if(sel==1)
		return sum1;
	else if(sel==2)
		return sum2;
}

static u8 PLC_CmdSeq;
static s8 GC9200_SendCmd(u8 _len,u8 _cmd,u8 _obj,u8 _sub,u32 _value)
{
	u8 cmd[19]= {0x1b,0x67,0x72,0x1d,0xc0,0x6d,0x6d,0x3a};
	const u8 N_FC1=8,N_FC2=9,N_Len=10,N_Seq=11,N_Cmd=12,N_Obj=13,
		       N_Sub=14,N_V0=15,N_V1=16,N_V2=17,N_V3=18;
	u16 i,j,ii;
	s16 pst;
	u8 cmd_len;
	cmd[N_Seq]=0;//++PLC_CmdSeq;
	cmd[N_Cmd]=_cmd;
	cmd[N_Obj]=_obj;
	cmd[N_Sub]=_sub;
	cmd[N_Len]=_len;
	cmd[N_V0]=_value>>24;
	cmd[N_V1]=_value>>16;
	cmd[N_V2]=_value>>8;
	cmd[N_V3]=_value;
	cmd[N_FC1]=PLC_GetFC2(&cmd[N_Len],cmd[N_Len],1); //计算载波机GC9200的1校验
	cmd[N_FC2]=PLC_GetFC2(&cmd[N_Len],cmd[N_Len],2); //计算载波机GC9200的2校验
	cmd_len=10+cmd[N_Len];
	PLC_RecCtn=0;
	memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
	//printcmd(cmd,cmd_len,0);
	PLC_SendData(cmd,cmd_len);
	PLC_IfExit();
	return 1;
}
//_msg=1,退出；成功返回1，失败返回-1
s8 PLC_GetID(u32 *pID,void * _msg)
{
	const u8 N_FC1=8,N_FC2=9,N_Len=10,N_Seq=11,N_Status=12,N_V0=13,N_V1=14,
		       N_V2=15,N_V3=16,N_V4=17,N_V5=18,N_V6=19,N_V7=20;
	u16 i,ii,j;
	s8 post;
	u8 const cmd_len=3+8;
	PLC_Msg=(int*)_msg;
	//for(i=0;i<10;i++)
	{
		GC9200_SendCmd(6,0,9,3,0x02000000);
		PLC_DelayMs(20);
		//printcmd(PLC_pRecBuff,PLC_RecCtn,9);
		for(ii=0;ii<300;ii++)
		{
			if(PLC_RecCtn>=10+cmd_len)
			{			
				post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);
				
				//找不到头码
				if(post == RETURN_FAIL)
					return RETURN_FAIL;

				//FC1校验不对
				if(PLC_pRecBuff[post+N_FC1]!=PLC_GetFC2(&PLC_pRecBuff[post+N_Len],cmd_len,1))
					return RETURN_FAIL;
				
				//FC2校验不对
				if(PLC_pRecBuff[post+N_FC2]!=PLC_GetFC2(&PLC_pRecBuff[post+N_Len],cmd_len,2))
					return RETURN_FAIL;
				
				//返回的结果错误
				if(PLC_pRecBuff[post+N_Status]!=0)
					return RETURN_FAIL;
				
				//命令序列号没有对上
				 if(PLC_pRecBuff[post+N_Seq]!=PLC_CmdSeq)
				 {
					return RETURN_FAIL;
				 }
				*pID=(PLC_pRecBuff[N_V0]<<24)+(PLC_pRecBuff[N_V1]<<16)+(PLC_pRecBuff[N_V2]<<8)+PLC_pRecBuff[N_V3];
				return RETURN_SUCCESS;
			}
			PLC_IfExit();
			PLC_DelayMs(10);
		}
		PLC_IfExit();
	}
	
	//载波机没有返回
	return RETURN_FAIL;
}

s8 PLC_TurnLight(u32 _id,u8 bright,void * _msg) //调光函数
{
	PLC_CmdStruct s_cmd;
	s_cmd.ID=_id;
	s_cmd.WR=0;
	s_cmd.CMD=0x14;
	s_cmd.SendData[0]=bright;
	s_cmd.SendDataLen=1;
	s_cmd.Ask=0x94;
	s_cmd.AskDataLen=1;
	PLC_Msg=(int*)_msg;
	if(PLC_SendCmd(&s_cmd)==RETURN_SUCCESS)
	{
		if(s_cmd.AskData[0]==bright)
			return RETURN_SUCCESS;
	}
	PLC_IfExit();
	return RETURN_FAIL;
}

#ifndef LORA_TEST_EN
	#error "Please define LORA_TEST_EN!"
#elif LORA_TEST_EN == 1
extern unsigned short int g_uiLoraID;
//设置被测产品进入LORA测试模式
s8 PLC_SetLoraTestMode(u32 _id,u8 DeviceCode,void * _msg) 
{
	PLC_CmdStruct s_cmd;
	s_cmd.ID=_id;
	s_cmd.WR=0xa1;
	s_cmd.CMD=0x09;
	s_cmd.SendData[0]=DeviceCode;
	s_cmd.SendData[1]=JIG_ID;
	s_cmd.SendDataLen=2;
	s_cmd.Ask=0x8A;
	s_cmd.AskDataLen=2;
	PLC_Msg=(int*)_msg;
	if(PLC_SendCmd(&s_cmd)==RETURN_SUCCESS){
		//收到的结果为02表示待测产品成功进入LORA测试模式
		if(s_cmd.AskData[0]== JIG_ID && s_cmd.AskData[1]== 0X02){
			return RETURN_SUCCESS;
		}
	}
	PLC_IfExit();
	return RETURN_FAIL;
}
#endif

s8 PLC_SaveCalData(u32 _id,void * _msg)
{
	u8 cmd[8]={0,0,0,0,0xa1,0x0b,0xaa,0xac};
	u16 i;
	u8 sum;
	PLC_Msg=(int*)_msg;
	PLC_SendID(_id,cmd); //将_id装入 cmd
	PLC_RecCtn=0;
	PLC_SendData(cmd,8); //从串口将数据发送出去
	for(i=0;i<1000;i++)
	{
		PLC_IfExit();
		PLC_DelayMs(10);
		if(PLC_RecCtn>=4)
		{
			sum=PLC_CheckSum(PLC_pRecBuff,PLC_RecCtn-1); //等待DPS回应
			if(sum==PLC_pRecBuff[PLC_RecCtn-1] && PLC_pRecBuff[PLC_RecCtn-2]==1)
				return RETURN_SUCCESS;
		}
  }
	return RETURN_FAIL;
}


float PLC_GetSuccessRate(void)  //没有调用
{
	if(PLC_TXCouter==0)
		return 0;
	return (float)PLC_RXCouter/(float)PLC_TXCouter;
}

/***************************************/
//函数名：SendCmd1
//功能：  发送修改ID的第1条命令
//输入参数：
//PhyID：目前的ID
/***************************************/
static s8 SendCmd1(unsigned long PhyID)
{
	u8 cmd[30]= {0x1b,0x67,0x72,0x1d,0xc0,0x6d,0x6d,0x3a};
	const u8 N_FC1=8,N_FC2=9,N_Len=10,N_Seq=11,N_Cmd=12,N_Obj=13,
		       N_Sub=14,N_V0=15,N_V1=16,N_V2=17,N_V3=18;
	u16 i,j,ii;
	s8 post;
	u8 cmd_len;
	cmd[N_Seq]=0;//seq1//++PLC_CmdSeq;
	cmd[N_Cmd]=1;
	cmd[N_Obj]=0x0f;
	cmd[N_Sub]=0;
	cmd[N_Len]=16;
	cmd[N_V0]=PhyID>>24;
	cmd[N_V1]=PhyID>>16;
	cmd[N_V2]=PhyID>>8;
	cmd[N_V3]=PhyID;
	cmd[21] = 5;  //len2
	cmd[22] = 9;  //seq2
	cmd[23] = 0;
	cmd[24] = 0;
	cmd[25] = 5;
	//校验
	cmd[19] = PLC_GetFC2(&cmd[21],cmd[21],1); 
	cmd[20] = PLC_GetFC2(&cmd[21],cmd[21],2);  
	//校验
	cmd[N_FC1] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],1); 
	cmd[N_FC2] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],2);
	cmd_len = 10 + cmd[N_Len];
	PLC_RecCtn = 0;
	memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
	PLC_SendData(cmd,cmd_len);
	//等待回应
	for(ii=0;ii<100;ii++) //等待GC9200的回复
	{
		if(PLC_RecCtn>=13) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//找不到头码
			if(post==RETURN_FAIL)
				return RETURN_FAIL;
			//FC1校验不对
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return RETURN_FAIL;			
			//FC2校验不对
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return RETURN_FAIL;			
			//返回的结果错误
			if(PLC_pRecBuff[post+10]!=3)
				return RETURN_FAIL;
			 PLC_RecCtn = 0;
	         memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
			 break;
		}
		PLC_DelayMs(10);
	}
	if (ii == 100){
		return RETURN_FAIL;
	}
	for(ii=0;ii<300;ii++) //等待节点的回复
	{
		if(PLC_RecCtn>=15) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//找不到头码
			if(post == RETURN_FAIL)
				return RETURN_FAIL;
			//FC1校验不对
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],5,1))
				return RETURN_FAIL;			
			//FC2校验不对
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],5,2))
				return RETURN_FAIL;			
			//返回的结果错误
			if(PLC_pRecBuff[post+10]!=5)
				return RETURN_FAIL;
			 return RETURN_SUCCESS;  //收到回应
		}
		PLC_DelayMs(10);
	}
	return RETURN_FAIL;   //超时
}
/***************************************/
//函数名：SendCmd2
//功能：  发送修改ID的第2条命令
//输入参数：
//PhyID：目前的ID
//PhyID_s：修改之后的ID
/***************************************/
static s8 SendCmd2(unsigned long PhyID, unsigned long PhyID_s)
{
	u8 cmd[30]= {0x1b,0x67,0x72,0x1d,0xc0,0x6d,0x6d,0x3a};
	const u8 N_FC1=8,N_FC2=9,N_Len=10,N_Seq=11,N_Cmd=12,N_Obj=13,
		       N_Sub=14,N_V0=15,N_V1=16,N_V2=17,N_V3=18;
	u16 i,j,ii;
	s8 post;
	u8 cmd_len;
	cmd[N_Len] = 0x14;
	cmd[N_Seq] = 0;//seq1//++PLC_CmdSeq;
	cmd[N_Cmd] = 1;
	cmd[N_Obj] = 0x0f;
	cmd[N_Sub] = 0;
	cmd[N_V0] = PhyID>>24;
	cmd[N_V1] = PhyID>>16;
	cmd[N_V2] = PhyID>>8;
	cmd[N_V3] = PhyID;
	cmd[21] = 9;  //len2
	cmd[22] = 0;  //seq2
	cmd[23] = 1;
	cmd[24] = 0x15;
	cmd[25] = 2;
	cmd[26] = PhyID_s>>24;
	cmd[27] = PhyID_s>>16;
	cmd[28] = PhyID_s>>8;
	cmd[29] = PhyID_s;
	//校验
	cmd[19] = PLC_GetFC2(&cmd[21],cmd[21],1); 
	cmd[20] = PLC_GetFC2(&cmd[21],cmd[21],2);  
	//校验
	cmd[N_FC1] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],1); 
	cmd[N_FC2] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],2);
	cmd_len = 10 + cmd[N_Len];
	PLC_RecCtn = 0;
	memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
	PLC_SendData(cmd,cmd_len);
	//等待回应
	for(ii=0;ii<100;ii++) //等待GC9200的回复
	{
		if(PLC_RecCtn>=13) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//找不到头码
			if(post==RETURN_FAIL)
				return RETURN_FAIL;
			//FC1校验不对
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return RETURN_FAIL;
			//FC2校验不对
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return RETURN_FAIL;
			//返回的结果错误
			if(PLC_pRecBuff[post+10]!=3)
				return RETURN_FAIL;
			 PLC_RecCtn = 0;
	         memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
			 break;
		}
		PLC_DelayMs(10);
	}
	if (ii == 100){
		return RETURN_FAIL;
	}
	for(ii=0;ii<300;ii++) //等待节点的回复
	{
		if(PLC_RecCtn>=13){			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);
			//找不到头码
			if(post==RETURN_FAIL)
				return RETURN_FAIL;
			//FC1校验不对
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return -RETURN_FAIL;			
			//FC2校验不对
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return RETURN_FAIL;			
			//返回的结果错误
			if(PLC_pRecBuff[post+10]!=3)
				return RETURN_FAIL;
			return RETURN_SUCCESS;  //收到回应
		}
		PLC_DelayMs(10);
	}
	return RETURN_FAIL;   //超时
}
/***************************************/
//函数名：SendCmd3
//功能：  发送修改ID的第3条命令
//输入参数：
//PhyID：目前的ID
/***************************************/
static s8 SendCmd3(unsigned long PhyID)
{
	u8 cmd[30]= {0x1b,0x67,0x72,0x1d,0xc0,0x6d,0x6d,0x3a};
	const u8 N_FC1=8,N_FC2=9,N_Len=10,N_Seq=11,N_Cmd=12,N_Obj=13,
		       N_Sub=14,N_V0=15,N_V1=16,N_V2=17,N_V3=18;
	u16 i,j,ii;
	s8 post;
	u8 cmd_len;
	cmd[N_Seq]=0;//seq1//++PLC_CmdSeq;
	cmd[N_Cmd]=1;
	cmd[N_Obj]=0x0f;
	cmd[N_Sub]=0;
	cmd[N_Len]=16;
	cmd[N_V0]=PhyID>>24;
	cmd[N_V1]=PhyID>>16;
	cmd[N_V2]=PhyID>>8;
	cmd[N_V3]=PhyID;
	cmd[21] = 7;  //len2
	cmd[22] = 9;  //seq2
	cmd[23] = 1;
	cmd[24] = 0;
	cmd[25] = 5;
	cmd[26] = 0;
	cmd[27] = 1;
	//校验
	cmd[19] = PLC_GetFC2(&cmd[21],cmd[21],1); 
	cmd[20] = PLC_GetFC2(&cmd[21],cmd[21],2);  
	//校验
	cmd[N_FC1] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],1); 
	cmd[N_FC2] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],2);
	cmd_len = 10 + cmd[N_Len];
	PLC_RecCtn = 0;
	memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
	PLC_SendData(cmd,cmd_len);
	//等待回应
	for(ii=0;ii<100;ii++) //等待GC9200的回复
	{
		if(PLC_RecCtn>=13) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//找不到头码
			if(post==RETURN_FAIL)
				return RETURN_FAIL;
			//FC1校验不对
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return RETURN_FAIL;
			//FC2校验不对
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return RETURN_FAIL;
			//返回的结果错误
			if(PLC_pRecBuff[post+10]!=3)
				return RETURN_FAIL;
			 PLC_RecCtn = 0;
	         memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
			 break;
		}
		PLC_DelayMs(10);
	}
	if (ii == 100){
		return RETURN_FAIL;
	}
	for(ii=0;ii<300;ii++) //等待节点的回复
	{
		if(PLC_RecCtn>=13){			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);
			//找不到头码
			if(post==RETURN_FAIL)
				return RETURN_FAIL;
			//FC1校验不对
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return RETURN_FAIL;			
			//FC2校验不对
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return RETURN_FAIL;			
			//返回的结果错误
			if(PLC_pRecBuff[post+10]!=3)
				return RETURN_FAIL;
			return RETURN_SUCCESS;  //收到回应
		}
		PLC_DelayMs(10);
	}
	return RETURN_FAIL;   //超时
}
/***************************************/
//函数名：WritePhyID2Device
//功能：  通过写ID到PLC模块
//输入参数：
//ulPhyID：需要打印的ID
/***************************************/
s8 WritePhyID2Device(unsigned long ulPhyID, unsigned long ulPhyID_New)
{
   if(RETURN_SUCCESS != SendCmd1(ulPhyID)){
	   return RETURN_FAIL;
   }
   if(RETURN_SUCCESS != SendCmd2(ulPhyID, ulPhyID_New)){
	   return RETURN_FAIL;
   }
   /*if(RETURN_SUCCESS != SendCmd3(ulPhyID)){
	   return RETURN_FAIL;
   }*/
   return RETURN_SUCCESS;
}

///===================================================================================================////////
//修改MasterPhyID
/***************************************/
//函数名：SendCmd1
//功能：  发送修改ID的第1条命令
//输入参数：
//PhyID：目前的ID
/***************************************/
static s8 SendMasterCmd1(unsigned long PhyID)
{
	u8 cmd[30]= {0x1b,0x67,0x72,0x1d,0xc0,0x6d,0x6d,0x3a};
	const u8 N_FC1=8,N_FC2=9,N_Len=10,N_Seq=11,N_Cmd=12,N_Obj=13,
		       N_Sub=14,N_V0=15,N_V1=16,N_V2=17,N_V3=18;
	u16 i,j,ii;
	s8 post;
	u8 cmd_len;
	cmd[N_Seq]=0;//seq1//++PLC_CmdSeq;
	cmd[N_Cmd]=1;
	cmd[N_Obj]=0x0f;
	cmd[N_Sub]=0;
	cmd[N_Len]=16;
	cmd[N_V0]=PhyID>>24;
	cmd[N_V1]=PhyID>>16;
	cmd[N_V2]=PhyID>>8;
	cmd[N_V3]=PhyID;
	cmd[21] = 5;  //len2
	cmd[22] = 9;  //seq2
	cmd[23] = 0;
	cmd[24] = 0;
	cmd[25] = 5;
	//校验
	cmd[19] = PLC_GetFC2(&cmd[21],cmd[21],1); 
	cmd[20] = PLC_GetFC2(&cmd[21],cmd[21],2);  
	//校验
	cmd[N_FC1] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],1); 
	cmd[N_FC2] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],2);
	cmd_len = 10 + cmd[N_Len];
	PLC_RecCtn = 0;
	memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
	PLC_SendData(cmd,cmd_len);
	//等待回应
	for(ii=0;ii<100;ii++) //等待GC9200的回复
	{
		if(PLC_RecCtn>=13) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//找不到头码
			if(post==-1)
				return -2;
			//FC1校验不对
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return -3;			
			//FC2校验不对
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return -4;			
			//返回的结果错误
			if(PLC_pRecBuff[post+10]!=3)
				return -5;
			 PLC_RecCtn = 0;
	         memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
			 break;
		}
		PLC_DelayMs(10);
	}
	if (ii == 100){
		return -1;
	}
	for(ii=0;ii<300;ii++) //等待节点的回复
	{
		if(PLC_RecCtn>=15) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//找不到头码
			if(post==-1)
				return -2;
			//FC1校验不对
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],5,1))
				return -3;			
			//FC2校验不对
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],5,2))
				return -4;			
			//返回的结果错误
			if(PLC_pRecBuff[post+10]!=5)
				return -5;
			 return 1;  //收到回应
		}
		PLC_DelayMs(10);
	}
	return -1;   //超时
}
/***************************************/
//函数名：SendCmd2
//功能：  发送修改ID的第2条命令
//输入参数：
//PhyID：目前的ID
//PhyID_s：修改之后的ID
/***************************************/
static s8 SendMasterCmd2(unsigned long PhyID, unsigned long PhyID_s, unsigned char ucMasterPhyIdNum)
{
	u8 cmd[30]= {0x1b,0x67,0x72,0x1d,0xc0,0x6d,0x6d,0x3a};
	const u8 N_FC1=8,N_FC2=9,N_Len=10,N_Seq=11,N_Cmd=12,N_Obj=13,
		       N_Sub=14,N_V0=15,N_V1=16,N_V2=17,N_V3=18;
	u16 i,j,ii;
	s8 post;
	u8 cmd_len;
	cmd[N_Len] = 0x14;
	cmd[N_Seq] = 0;//seq1//++PLC_CmdSeq;
	cmd[N_Cmd] = 1;
	cmd[N_Obj] = 0x0f;
	cmd[N_Sub] = 0;
	cmd[N_V0] = PhyID>>24;
	cmd[N_V1] = PhyID>>16;
	cmd[N_V2] = PhyID>>8;
	cmd[N_V3] = PhyID;
	cmd[21] = 9;  //len2
	cmd[22] = 0;  //seq2
	cmd[23] = 1;
	cmd[24] = 0x0a;//ddd  修改MasterPhyId
	cmd[25] = ucMasterPhyIdNum;
	cmd[26] = PhyID_s>>24;
	cmd[27] = PhyID_s>>16;
	cmd[28] = PhyID_s>>8;
	cmd[29] = PhyID_s;
	//校验
	cmd[19] = PLC_GetFC2(&cmd[21],cmd[21],1); 
	cmd[20] = PLC_GetFC2(&cmd[21],cmd[21],2);  
	//校验
	cmd[N_FC1] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],1); 
	cmd[N_FC2] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],2);
	cmd_len = 10 + cmd[N_Len];
	PLC_RecCtn = 0;
	memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
	PLC_SendData(cmd,cmd_len);
	//等待回应
	for(ii=0;ii<100;ii++) //等待GC9200的回复
	{
		if(PLC_RecCtn>=13) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//找不到头码
			if(post==-1)
				return -2;
			//FC1校验不对
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return -3;
			//FC2校验不对
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return -4;
			//返回的结果错误
			if(PLC_pRecBuff[post+10]!=3)
				return -5;
			 PLC_RecCtn = 0;
	         memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
			 break;
		}
		PLC_DelayMs(10);
	}
	if (ii == 100){
		return -1;
	}
	for(ii=0;ii<300;ii++) //等待节点的回复
	{
		if(PLC_RecCtn>=13){			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);
			//找不到头码
			if(post==-1)
				return -2;
			//FC1校验不对
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return -3;			
			//FC2校验不对
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return -4;			
			//返回的结果错误
			if(PLC_pRecBuff[post+10]!=3)
				return -5;
			return 1;  //收到回应
		}
		PLC_DelayMs(10);
	}
	return -1;   //超时
}
/***************************************/
//函数名：SendCmd3
//功能：  发送修改ID的第3条命令
//输入参数：
//PhyID：目前的ID
/***************************************/
static s8 SendMasterCmd3(unsigned long PhyID)
{
	u8 cmd[30]= {0x1b,0x67,0x72,0x1d,0xc0,0x6d,0x6d,0x3a};
	const u8 N_FC1=8,N_FC2=9,N_Len=10,N_Seq=11,N_Cmd=12,N_Obj=13,
		       N_Sub=14,N_V0=15,N_V1=16,N_V2=17,N_V3=18;
	u16 i,j,ii;
	s8 post;
	u8 cmd_len;
	cmd[N_Seq]=0;//seq1//++PLC_CmdSeq;
	cmd[N_Cmd]=1;
	cmd[N_Obj]=0x0f;
	cmd[N_Sub]=0;
	cmd[N_Len]=16;
	cmd[N_V0]=PhyID>>24;
	cmd[N_V1]=PhyID>>16;
	cmd[N_V2]=PhyID>>8;
	cmd[N_V3]=PhyID;
	cmd[21] = 7;  //len2
	cmd[22] = 9;  //seq2
	cmd[23] = 1;
	cmd[24] = 0;
	cmd[25] = 5;
	cmd[26] = 0;
	cmd[27] = 1;
	//校验
	cmd[19] = PLC_GetFC2(&cmd[21],cmd[21],1); 
	cmd[20] = PLC_GetFC2(&cmd[21],cmd[21],2);  
	//校验
	cmd[N_FC1] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],1); 
	cmd[N_FC2] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],2);
	cmd_len = 10 + cmd[N_Len];
	PLC_RecCtn = 0;
	memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
	PLC_SendData(cmd,cmd_len);
	//等待回应
	for(ii=0;ii<100;ii++) //等待GC9200的回复
	{
		if(PLC_RecCtn>=13) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//找不到头码
			if(post==-1)
				return -2;
			//FC1校验不对
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return -3;
			//FC2校验不对
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return -4;
			//返回的结果错误
			if(PLC_pRecBuff[post+10]!=3)
				return -5;
			 PLC_RecCtn = 0;
	         memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
			 break;
		}
		PLC_DelayMs(10);
	}
	if (ii == 100){
		return -1;
	}
	for(ii=0;ii<300;ii++) //等待节点的回复
	{
		if(PLC_RecCtn>=13){			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);
			//找不到头码
			if(post==-1)
				return -2;
			//FC1校验不对
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return -3;			
			//FC2校验不对
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return -4;			
			//返回的结果错误
			if(PLC_pRecBuff[post+10]!=3)
				return -5;
			return 1;  //收到回应
		}
		PLC_DelayMs(10);
	}
	return -1;   //超时
}
/***************************************/
//函数名：WritePhyID2Device
//功能：  通过写ID到PLC模块
//输入参数：
//ulPhyID：需要打印的ID
/***************************************/
s8 WriteMasterPhyID2Device(unsigned long ulPhyID, unsigned long ulMasterPhyID1, unsigned long ulMasterPhyID2, unsigned long ulMasterPhyID3)
{
   if(1 != SendMasterCmd1(ulPhyID)){
	   return -1;
   }
   if(1 != SendMasterCmd2(ulPhyID, ulMasterPhyID1, 0)){
	   return -1;
   }
   if(1 != SendMasterCmd2(ulPhyID, ulMasterPhyID2, 1)){
	   return -1;
   }
   if(1 != SendMasterCmd2(ulPhyID, ulMasterPhyID3, 2)){
	   return -1;
   }
   if(1 != SendMasterCmd3(ulPhyID)){
	   return -1;
   }
   return 1;
}

/***************************************/
//函数名：PLC_ClearEnergy
//功能：  通过PLC清除SLC的电能计量
//输入参数：_id：待测SLC的ID
//成功返回：应答码   失败返回：RETURN_FAIL  
/***************************************/
s8 PLC_ClearEnergy(u32 _id)
{
	PLC_CmdStruct s_cmd;	
	s_cmd.ID=_id;
	s_cmd.WR = 0xa1;
	s_cmd.CMD = 0X07;
	s_cmd.SendDataLen = 1;
	s_cmd.SendData[0] = DEVICE_CODE;
	s_cmd.Ask = 0x8a;    //应答码
	s_cmd.AskDataLen = 2;  //应答长度：设备码(1byte) + 响应结果(1byte)
	if(PLC_SendCmd(&s_cmd) == RETURN_SUCCESS){
		return s_cmd.AskData[1]; //返回应答码
	}
	PLC_IfExit();
	return RETURN_FAIL;
}

//发送字符串
s8 NemaSendData(u8 *pData,u8 len)
{
	u8 i=0;
	RS485_NEMA_TX();
	PLC_DelayMs(1);
	for(i=0;i<len;i++){
		USART_SendData(PLC_COM,pData[i]);    //发送一个字节
	    while(USART_GetFlagStatus(PLC_COM,USART_FLAG_TC)==RESET);//检测是否发送成功  
	}
	PLC_DelayMs(1);
	RS485_NEMA_RX();
	return 1;
}


void SendSafteCmd(void)
{
	u8 SafteCmd[2] = {0x7a, 0x7a};
	NemaSendData(SafteCmd, 2);
}
u8 CheckSafteMode(void)
{ short int j;
	const unsigned char s_len = 10;
	const unsigned char ucHead = 0x47;
	u8 ucReciveData[s_len];
	u8 s_HeadPost;
	u8 ucCnt;
	u8 ucAdrShift;
  ucAdrShift = 0;
	if(PLC_RecCtn>=s_len){
		while(1){
			s_HeadPost=PLC_FindHead(ucHead,&(PLC_pRecBuff[ucAdrShift]),PLC_RecCtn - ucAdrShift);
			if (PLC_RecCtn < PLC_BUFFSIZE){
				if(((u16)s_HeadPost + s_len + ucAdrShift) < PLC_RecCtn){
				   for(ucCnt = 0;ucCnt<s_len;ucCnt++){
					   ucReciveData[ucCnt] = PLC_pRecBuff[ucAdrShift + ucCnt + s_HeadPost];
				   }
				   if(ucReciveData[0] == 0x47 && ucReciveData[1] == 0x43 && ucReciveData[2] == 0x30 && 
					  ucReciveData[3] == 0x30 && ucReciveData[4] == 0x0A && ucReciveData[5] == 0x0A && 
					  ucReciveData[6] == 0x0A && ucReciveData[7] == 0x0A && ucReciveData[8] == 0x0A &&
					  ucReciveData[9]){
						PLC_RecCtn=0;
						return(1);
				   }
				   ucAdrShift += s_HeadPost + s_len;
				   if(ucAdrShift >= PLC_RecCtn){
					   return(0);
				   }
				}else{
				   return(0);
				}
			}else{
				PLC_RecCtn=0;
				return(0);
			}
		}
    }else{
		return(0);
	}
}

void MasterReset(void)
{
   const u8 ucRestCmd[15] = {0x1B, 0x67, 0x72, 0x1D, 0xC0, 0x6D, 0x6D, 0x3A, 0x09, 0x1F, 0x05, 0x00, 0x01, 0x00, 0x03};
   PLC_SendData((u8 *)ucRestCmd,15);
}
void NemaReset(void)
{
   const u8 ucRestCmd[15] = {0x1B, 0x67, 0x72, 0x1D, 0xC0, 0x6D, 0x6D, 0x3A, 0x09, 0x1F, 0x05, 0x00, 0x01, 0x00, 0x03};
   NemaSendData((u8 *)ucRestCmd,15);
}

