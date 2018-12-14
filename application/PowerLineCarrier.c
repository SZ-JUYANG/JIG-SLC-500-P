
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
//PLC���͵����ݼ���
static u32 PLC_TXCouter=0;
//PLC���յ����ݼ���
static u32 PLC_RXCouter=0;
//�����������͹�������Ϣ
static int *PLC_Msg;
//PLC�˳����ͺͽ��յ���Ϣ
#define PLC_IsExit PLC_Msg[0]
#define PLC_EXIT  0x01
//�����˳����ص���Ϣ
#define PLC_EXIT_ERR 0x00
//����Ƿ������˳�
#define PLC_IfExit() if(PLC_IsExit==PLC_EXIT) return PLC_EXIT_ERR
	
typedef struct 
{
	u8 WR;  //ֻȡ0xa1��0xa2
	u8 CMD;
	u32 ID;
	u8 Ask;        //Ӧ���������Ӧ��
	u8 AskDataLen; //Ӧ�����ݵĳ��ȣ���������Ӧ�����Ӧ����
	u8 AskData[20];
	u8 SendDataLen;//�������ݵĳ��ȣ��������������ͺ�����
	u8 SendData[30];
}PLC_CmdStruct;

//����ת����char
static void PLC_Int2Ch(u8 *_pch,u16 _value)
{
	*_pch=_value>>8;
	*(_pch+1)=_value;
}

//���յ�������ת����int
static u16 PLC_Ch2Int(u8 *_pch)
{
	return (*_pch<<8)+*(_pch+1);
}

//��charת���ɸ�����
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

//��������ת����char
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


//��ʼ��EN����
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

static u8 PLC_CheckSum(u8 *pData,u8 len) //�����У��
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

//�����ַ���
static s8 PLC_SendData(u8 *pData,u8 len)
{
	u8 i=0;
	RS485_TX();
	PLC_DelayMs(1);
	//printcmd(pData,len,11);
	for(i=0;i<len;i++)
	{
		USART_SendData(PLC_COM,pData[i]);    //����һ���ֽ�
	  while(USART_GetFlagStatus(PLC_COM,USART_FLAG_TC)==RESET);//����Ƿ��ͳɹ�  
		PLC_IfExit();
	}
	PLC_DelayMs(1);
	RS485_RX();
	return 1;
}

//��������ṹ��
static s8 PLC__SendCmd(PLC_CmdStruct _Cmd)//������Ӵ��ڷ��ͳ�ȥ
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
		s_SendStr[i++] = PLC_CheckXor(&s_SendStr[4],s_len); //�������У��
	}
	s_SendStr[i++] = PLC_CheckSum(&s_SendStr[4],s_len);	  //�����У��
	s_len=i;
	return PLC_SendData(s_SendStr,s_len); //��������
}

//�ɹ�����ͷ����±�,û���ҵ�����-1
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

//��������
//_psenddata���������ݵ�ָ��
//�ɹ�����1��ʧ�ܷ��ظ���
//�˳�����0��
static s8 PLC_RecCmd(PLC_CmdStruct *_psenddata) //����PLC������������
{
	s16 s_HeadPost;
	u8 s_CkeckSum;
	u8 s_HaveHead,s_len;
	u16 i,j;
	RS485_RX();
	s_HaveHead=_psenddata->WR==0xa1 || _psenddata->WR==0xa2; //�ж��Ƿ�ΪУ������
	//Ӧ�������ȣ������2����Ӧ�����У��ĳ��ȣ�s_HaveHead��ʾ��Ӧ����ĳ��ȡ�
	s_len=2+s_HaveHead+_psenddata->AskDataLen;               //У������ȱȵ��������һ���ֽڡ�
	for(j=0;j<400;j++) //�ȴ�10ms * 400 = 4s
	{
		//�����յ����ݳ���
		if(PLC_RecCtn>=s_len){
			s_HeadPost=PLC_FindHead(_psenddata->Ask,PLC_pRecBuff,PLC_RecCtn);
			//���ͷ��
			if(s_HeadPost==RETURN_FAIL){ //����Ӧ��������Ӧ��ʧ��
				return RETURN_FAIL;
			}
			if (_psenddata->Ask != 0x93){
			    s_CkeckSum=PLC_CheckSum(&PLC_pRecBuff[s_HeadPost],_psenddata->AskDataLen+s_HaveHead+1);
				//���У���
				if(s_CkeckSum!=PLC_pRecBuff[s_HeadPost+_psenddata->AskDataLen+s_HaveHead+1])
				{
					return RETURN_FAIL;
				}
			}else{ //��Ӧ13�������Ӧ��0X93�Ĵ���
			    //�¼��²�ѯָ��ص����ݲ�����֡ͷ
			    s_CkeckSum=PLC_CheckSum(&PLC_pRecBuff[s_HeadPost + 1],15);
				//���У���
				if(s_CkeckSum!=PLC_pRecBuff[s_HeadPost+16])
				{
					return RETURN_FAIL;
				}
			}
			
			//�����Ӧ�����Ƿ�ԣ�s_HaveHead=0��ʾû����Ӧ����
			if((PLC_pRecBuff[s_HeadPost+s_HaveHead]!=_psenddata->CMD) && s_HaveHead)
			{
				return RETURN_FAIL;
			}
			
			//������յ�������
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
	{   //�������ݳ��ȣ��������ݻ��������㡣
		PLC_RecCtn = 0;
		memset(PLC_pRecBuff,0,PLC_BUFFSIZE);//��׼�⺯������PLC_pRecBuff��ǰPLC_BUFFSIZE���ֽ���0���
		//���������ȥ
		PLC__SendCmd(*_psenddata);
		//�ȴ���Ӧ
		if((err=PLC_RecCmd(_psenddata))==RETURN_SUCCESS) //�ȴ���Ӧ��4s
		{
			#ifdef PLC_DEBUG
			//printcmd(PLC_pRecBuff,PLC_RecCtn);
			#endif
			//�������ݳ��ȣ��������ݻ��������㡣
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

//��ȡ��Դ����:����������ʵ�����ѹ���������������ȣ���Դ״̬
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
	if(PLC_SendCmd(&s_cmd)==RETURN_SUCCESS)//����������ȴ���Ӧ��
	{  //��ȡPLC������������
		_pSturct->Temperature=s_cmd.AskData[0];
		_pSturct->Vi = ((u16)(s_cmd.AskData[6])<<8) | s_cmd.AskData[5];//PLC_Ch2Int(&s_cmd.AskData[2])/10.0;
		_pSturct->Ii = ((u16)(s_cmd.AskData[8])<<8) | s_cmd.AskData[7];//PLC_Ch2Int(&s_cmd.AskData[4])/1000.0;
		_pSturct->Pa = ((u16)(s_cmd.AskData[10])<<8) | s_cmd.AskData[9];//PLC_Ch2Int(&s_cmd.AskData[8])/10.0;
		return RETURN_SUCCESS;
	}
	PLC_IfExit();
	return RETURN_FAIL;
}
//�ָ���������
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
	if(PLC_SendCmd(&s_cmd)==1) //��������ȴ���Ӧ
	{
		return (s_cmd.AskData[0]<<8)+s_cmd.AskData[1];
	}
	PLC_IfExit();
	return -1;
}

//�����������
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

//���������ѹ
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

//�����������
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

//��ȡУ����������ʣ�������ѹ��б�ʺͽؾ�
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

//��ȡУ��״̬���ɹ�����1��ʧ�ܷ���-1
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

//��ȡ��Դ�Ķ���������ʣ���ѹ������
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
����ֵ��
01H:�ɹ�
02H:ʧ��
03H:������Χ
04H:�޴���Ŀ
**********************/
//У�����������ʣ���ѹ������,�ɹ�������Ӧ��������򷵻�-1
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
����ֵ��
01H:�ɹ�
02H:ʧ��
03H:������Χ
04H:�޴���Ŀ
**********************/
//У�����������ʣ���ѹ������,�ɹ�������Ӧ��������򷵻�-1
static s8 PLC_CalVIP(u32 _id, float P,float U, float I, unsigned short int ui0_10V_Repair)
{
	PLC_CmdStruct s_cmd;	
	s_cmd.ID=_id;
	s_cmd.WR = 0xa1;
	s_cmd.CMD = 0X05;
	s_cmd.SendDataLen = 14;
	PLC_Float2Ch(&s_cmd.SendData[0], P);   //���빦��
	PLC_Float2Ch(&s_cmd.SendData[4], U);   //�����ѹ
	PLC_Float2Ch(&s_cmd.SendData[8], I);   //�������
	s_cmd.SendData[12] = (unsigned char)(ui0_10V_Repair>>8);
	s_cmd.SendData[13] = (unsigned char)ui0_10V_Repair;
	s_cmd.Ask = 0x8a;    //Ӧ����
	s_cmd.AskDataLen = 1;  //Ӧ�𳤶�
	if(PLC_SendCmd(&s_cmd) == RETURN_SUCCESS){
		return s_cmd.AskData[0];
	}
	PLC_IfExit();
	return RETURN_FAIL;
}

s8 PLC_CalInputVIP(u32 _id, float P, float U, float I, unsigned short int ui0_10V_Repair, void * _msg) //У�������ѹ�����������ʺ���
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
��ȡCSZ7759�Ĳ���Ƶ�ʻ��߲�����ADCֵ
�ɹ�������1(Ƶ��)���߷���2��ADֵ��
ʧ�ܣ�����-1
�޴���Ŀ������4       
���ݱ�����pf����
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
��ȡ������ADCֵ
�ɹ�������1(Ƶ��)���߷���2��ADֵ��
ʧ�ܣ�����-1
�޴���Ŀ������4       
���ݱ�����pf����
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
	cmd[N_FC1]=PLC_GetFC2(&cmd[N_Len],cmd[N_Len],1); //�����ز���GC9200��1У��
	cmd[N_FC2]=PLC_GetFC2(&cmd[N_Len],cmd[N_Len],2); //�����ز���GC9200��2У��
	cmd_len=10+cmd[N_Len];
	PLC_RecCtn=0;
	memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
	//printcmd(cmd,cmd_len,0);
	PLC_SendData(cmd,cmd_len);
	PLC_IfExit();
	return 1;
}
//_msg=1,�˳����ɹ�����1��ʧ�ܷ���-1
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
				
				//�Ҳ���ͷ��
				if(post == RETURN_FAIL)
					return RETURN_FAIL;

				//FC1У�鲻��
				if(PLC_pRecBuff[post+N_FC1]!=PLC_GetFC2(&PLC_pRecBuff[post+N_Len],cmd_len,1))
					return RETURN_FAIL;
				
				//FC2У�鲻��
				if(PLC_pRecBuff[post+N_FC2]!=PLC_GetFC2(&PLC_pRecBuff[post+N_Len],cmd_len,2))
					return RETURN_FAIL;
				
				//���صĽ������
				if(PLC_pRecBuff[post+N_Status]!=0)
					return RETURN_FAIL;
				
				//�������к�û�ж���
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
	
	//�ز���û�з���
	return RETURN_FAIL;
}

s8 PLC_TurnLight(u32 _id,u8 bright,void * _msg) //���⺯��
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
//���ñ����Ʒ����LORA����ģʽ
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
		//�յ��Ľ��Ϊ02��ʾ�����Ʒ�ɹ�����LORA����ģʽ
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
	PLC_SendID(_id,cmd); //��_idװ�� cmd
	PLC_RecCtn=0;
	PLC_SendData(cmd,8); //�Ӵ��ڽ����ݷ��ͳ�ȥ
	for(i=0;i<1000;i++)
	{
		PLC_IfExit();
		PLC_DelayMs(10);
		if(PLC_RecCtn>=4)
		{
			sum=PLC_CheckSum(PLC_pRecBuff,PLC_RecCtn-1); //�ȴ�DPS��Ӧ
			if(sum==PLC_pRecBuff[PLC_RecCtn-1] && PLC_pRecBuff[PLC_RecCtn-2]==1)
				return RETURN_SUCCESS;
		}
  }
	return RETURN_FAIL;
}


float PLC_GetSuccessRate(void)  //û�е���
{
	if(PLC_TXCouter==0)
		return 0;
	return (float)PLC_RXCouter/(float)PLC_TXCouter;
}

/***************************************/
//��������SendCmd1
//���ܣ�  �����޸�ID�ĵ�1������
//���������
//PhyID��Ŀǰ��ID
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
	//У��
	cmd[19] = PLC_GetFC2(&cmd[21],cmd[21],1); 
	cmd[20] = PLC_GetFC2(&cmd[21],cmd[21],2);  
	//У��
	cmd[N_FC1] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],1); 
	cmd[N_FC2] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],2);
	cmd_len = 10 + cmd[N_Len];
	PLC_RecCtn = 0;
	memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
	PLC_SendData(cmd,cmd_len);
	//�ȴ���Ӧ
	for(ii=0;ii<100;ii++) //�ȴ�GC9200�Ļظ�
	{
		if(PLC_RecCtn>=13) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//�Ҳ���ͷ��
			if(post==RETURN_FAIL)
				return RETURN_FAIL;
			//FC1У�鲻��
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return RETURN_FAIL;			
			//FC2У�鲻��
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return RETURN_FAIL;			
			//���صĽ������
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
	for(ii=0;ii<300;ii++) //�ȴ��ڵ�Ļظ�
	{
		if(PLC_RecCtn>=15) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//�Ҳ���ͷ��
			if(post == RETURN_FAIL)
				return RETURN_FAIL;
			//FC1У�鲻��
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],5,1))
				return RETURN_FAIL;			
			//FC2У�鲻��
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],5,2))
				return RETURN_FAIL;			
			//���صĽ������
			if(PLC_pRecBuff[post+10]!=5)
				return RETURN_FAIL;
			 return RETURN_SUCCESS;  //�յ���Ӧ
		}
		PLC_DelayMs(10);
	}
	return RETURN_FAIL;   //��ʱ
}
/***************************************/
//��������SendCmd2
//���ܣ�  �����޸�ID�ĵ�2������
//���������
//PhyID��Ŀǰ��ID
//PhyID_s���޸�֮���ID
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
	//У��
	cmd[19] = PLC_GetFC2(&cmd[21],cmd[21],1); 
	cmd[20] = PLC_GetFC2(&cmd[21],cmd[21],2);  
	//У��
	cmd[N_FC1] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],1); 
	cmd[N_FC2] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],2);
	cmd_len = 10 + cmd[N_Len];
	PLC_RecCtn = 0;
	memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
	PLC_SendData(cmd,cmd_len);
	//�ȴ���Ӧ
	for(ii=0;ii<100;ii++) //�ȴ�GC9200�Ļظ�
	{
		if(PLC_RecCtn>=13) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//�Ҳ���ͷ��
			if(post==RETURN_FAIL)
				return RETURN_FAIL;
			//FC1У�鲻��
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return RETURN_FAIL;
			//FC2У�鲻��
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return RETURN_FAIL;
			//���صĽ������
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
	for(ii=0;ii<300;ii++) //�ȴ��ڵ�Ļظ�
	{
		if(PLC_RecCtn>=13){			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);
			//�Ҳ���ͷ��
			if(post==RETURN_FAIL)
				return RETURN_FAIL;
			//FC1У�鲻��
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return -RETURN_FAIL;			
			//FC2У�鲻��
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return RETURN_FAIL;			
			//���صĽ������
			if(PLC_pRecBuff[post+10]!=3)
				return RETURN_FAIL;
			return RETURN_SUCCESS;  //�յ���Ӧ
		}
		PLC_DelayMs(10);
	}
	return RETURN_FAIL;   //��ʱ
}
/***************************************/
//��������SendCmd3
//���ܣ�  �����޸�ID�ĵ�3������
//���������
//PhyID��Ŀǰ��ID
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
	//У��
	cmd[19] = PLC_GetFC2(&cmd[21],cmd[21],1); 
	cmd[20] = PLC_GetFC2(&cmd[21],cmd[21],2);  
	//У��
	cmd[N_FC1] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],1); 
	cmd[N_FC2] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],2);
	cmd_len = 10 + cmd[N_Len];
	PLC_RecCtn = 0;
	memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
	PLC_SendData(cmd,cmd_len);
	//�ȴ���Ӧ
	for(ii=0;ii<100;ii++) //�ȴ�GC9200�Ļظ�
	{
		if(PLC_RecCtn>=13) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//�Ҳ���ͷ��
			if(post==RETURN_FAIL)
				return RETURN_FAIL;
			//FC1У�鲻��
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return RETURN_FAIL;
			//FC2У�鲻��
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return RETURN_FAIL;
			//���صĽ������
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
	for(ii=0;ii<300;ii++) //�ȴ��ڵ�Ļظ�
	{
		if(PLC_RecCtn>=13){			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);
			//�Ҳ���ͷ��
			if(post==RETURN_FAIL)
				return RETURN_FAIL;
			//FC1У�鲻��
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return RETURN_FAIL;			
			//FC2У�鲻��
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return RETURN_FAIL;			
			//���صĽ������
			if(PLC_pRecBuff[post+10]!=3)
				return RETURN_FAIL;
			return RETURN_SUCCESS;  //�յ���Ӧ
		}
		PLC_DelayMs(10);
	}
	return RETURN_FAIL;   //��ʱ
}
/***************************************/
//��������WritePhyID2Device
//���ܣ�  ͨ��дID��PLCģ��
//���������
//ulPhyID����Ҫ��ӡ��ID
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
//�޸�MasterPhyID
/***************************************/
//��������SendCmd1
//���ܣ�  �����޸�ID�ĵ�1������
//���������
//PhyID��Ŀǰ��ID
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
	//У��
	cmd[19] = PLC_GetFC2(&cmd[21],cmd[21],1); 
	cmd[20] = PLC_GetFC2(&cmd[21],cmd[21],2);  
	//У��
	cmd[N_FC1] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],1); 
	cmd[N_FC2] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],2);
	cmd_len = 10 + cmd[N_Len];
	PLC_RecCtn = 0;
	memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
	PLC_SendData(cmd,cmd_len);
	//�ȴ���Ӧ
	for(ii=0;ii<100;ii++) //�ȴ�GC9200�Ļظ�
	{
		if(PLC_RecCtn>=13) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//�Ҳ���ͷ��
			if(post==-1)
				return -2;
			//FC1У�鲻��
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return -3;			
			//FC2У�鲻��
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return -4;			
			//���صĽ������
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
	for(ii=0;ii<300;ii++) //�ȴ��ڵ�Ļظ�
	{
		if(PLC_RecCtn>=15) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//�Ҳ���ͷ��
			if(post==-1)
				return -2;
			//FC1У�鲻��
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],5,1))
				return -3;			
			//FC2У�鲻��
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],5,2))
				return -4;			
			//���صĽ������
			if(PLC_pRecBuff[post+10]!=5)
				return -5;
			 return 1;  //�յ���Ӧ
		}
		PLC_DelayMs(10);
	}
	return -1;   //��ʱ
}
/***************************************/
//��������SendCmd2
//���ܣ�  �����޸�ID�ĵ�2������
//���������
//PhyID��Ŀǰ��ID
//PhyID_s���޸�֮���ID
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
	cmd[24] = 0x0a;//ddd  �޸�MasterPhyId
	cmd[25] = ucMasterPhyIdNum;
	cmd[26] = PhyID_s>>24;
	cmd[27] = PhyID_s>>16;
	cmd[28] = PhyID_s>>8;
	cmd[29] = PhyID_s;
	//У��
	cmd[19] = PLC_GetFC2(&cmd[21],cmd[21],1); 
	cmd[20] = PLC_GetFC2(&cmd[21],cmd[21],2);  
	//У��
	cmd[N_FC1] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],1); 
	cmd[N_FC2] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],2);
	cmd_len = 10 + cmd[N_Len];
	PLC_RecCtn = 0;
	memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
	PLC_SendData(cmd,cmd_len);
	//�ȴ���Ӧ
	for(ii=0;ii<100;ii++) //�ȴ�GC9200�Ļظ�
	{
		if(PLC_RecCtn>=13) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//�Ҳ���ͷ��
			if(post==-1)
				return -2;
			//FC1У�鲻��
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return -3;
			//FC2У�鲻��
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return -4;
			//���صĽ������
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
	for(ii=0;ii<300;ii++) //�ȴ��ڵ�Ļظ�
	{
		if(PLC_RecCtn>=13){			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);
			//�Ҳ���ͷ��
			if(post==-1)
				return -2;
			//FC1У�鲻��
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return -3;			
			//FC2У�鲻��
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return -4;			
			//���صĽ������
			if(PLC_pRecBuff[post+10]!=3)
				return -5;
			return 1;  //�յ���Ӧ
		}
		PLC_DelayMs(10);
	}
	return -1;   //��ʱ
}
/***************************************/
//��������SendCmd3
//���ܣ�  �����޸�ID�ĵ�3������
//���������
//PhyID��Ŀǰ��ID
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
	//У��
	cmd[19] = PLC_GetFC2(&cmd[21],cmd[21],1); 
	cmd[20] = PLC_GetFC2(&cmd[21],cmd[21],2);  
	//У��
	cmd[N_FC1] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],1); 
	cmd[N_FC2] = PLC_GetFC2(&cmd[N_Len],cmd[N_Len],2);
	cmd_len = 10 + cmd[N_Len];
	PLC_RecCtn = 0;
	memset(PLC_pRecBuff,0,PLC_BUFFSIZE);
	PLC_SendData(cmd,cmd_len);
	//�ȴ���Ӧ
	for(ii=0;ii<100;ii++) //�ȴ�GC9200�Ļظ�
	{
		if(PLC_RecCtn>=13) 
		{			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);			
			//�Ҳ���ͷ��
			if(post==-1)
				return -2;
			//FC1У�鲻��
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return -3;
			//FC2У�鲻��
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return -4;
			//���صĽ������
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
	for(ii=0;ii<300;ii++) //�ȴ��ڵ�Ļظ�
	{
		if(PLC_RecCtn>=13){			
			post=PLC_FindHead(0x1b,PLC_pRecBuff,PLC_RecCtn);
			//�Ҳ���ͷ��
			if(post==-1)
				return -2;
			//FC1У�鲻��
			if(PLC_pRecBuff[post+8]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,1))
				return -3;			
			//FC2У�鲻��
			if(PLC_pRecBuff[post+9]!=PLC_GetFC2(&PLC_pRecBuff[post+10],3,2))
				return -4;			
			//���صĽ������
			if(PLC_pRecBuff[post+10]!=3)
				return -5;
			return 1;  //�յ���Ӧ
		}
		PLC_DelayMs(10);
	}
	return -1;   //��ʱ
}
/***************************************/
//��������WritePhyID2Device
//���ܣ�  ͨ��дID��PLCģ��
//���������
//ulPhyID����Ҫ��ӡ��ID
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
//��������PLC_ClearEnergy
//���ܣ�  ͨ��PLC���SLC�ĵ��ܼ���
//���������_id������SLC��ID
//�ɹ����أ�Ӧ����   ʧ�ܷ��أ�RETURN_FAIL  
/***************************************/
s8 PLC_ClearEnergy(u32 _id)
{
	PLC_CmdStruct s_cmd;	
	s_cmd.ID=_id;
	s_cmd.WR = 0xa1;
	s_cmd.CMD = 0X07;
	s_cmd.SendDataLen = 1;
	s_cmd.SendData[0] = DEVICE_CODE;
	s_cmd.Ask = 0x8a;    //Ӧ����
	s_cmd.AskDataLen = 2;  //Ӧ�𳤶ȣ��豸��(1byte) + ��Ӧ���(1byte)
	if(PLC_SendCmd(&s_cmd) == RETURN_SUCCESS){
		return s_cmd.AskData[1]; //����Ӧ����
	}
	PLC_IfExit();
	return RETURN_FAIL;
}

//�����ַ���
s8 NemaSendData(u8 *pData,u8 len)
{
	u8 i=0;
	RS485_NEMA_TX();
	PLC_DelayMs(1);
	for(i=0;i<len;i++){
		USART_SendData(PLC_COM,pData[i]);    //����һ���ֽ�
	    while(USART_GetFlagStatus(PLC_COM,USART_FLAG_TC)==RESET);//����Ƿ��ͳɹ�  
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

