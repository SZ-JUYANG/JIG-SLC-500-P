
#include "CommonFunction.h"
#include "usart.h"
#include <string.h>
#include "GP9034T.h"
#define  GP9034T_COM    UART5
//const unsigned char ucPrintString[20][60] = {
//"SIZE 100 mm, 150 mm\r\n\0",
//"GAP 3 mm,0\r\n\0",
//"SPEED 6\r\n\0",
//"DENSITY 8\r\n\0",
//"DIRECTION 1\r\n\0",
//"CLS\r\n\0",
//"TEXT 20,50,\"3\",0,1,1,\"GP-1225D TEST\"\r\n\0",
//"QRCODE 10,10,H,4,A,0,\"abcABC\"\r\n\0",
//"PRINT 1,1\r\n\0",
//"SIZE 40 mm, 30 mm\r\n\0",
//"GAP 0 mm,0\r\n\0",
//"SPEED 6\r\n\0",
//"DENSITY 8\r\n\0",
//"DIRECTION 1\r\n\0",
//"CLS\r\n\0",
//"TEXT 20,50,\"3\",0,1,1,\"GP-1225D TEST\"\r\n\0",
//"QRCODE 10,10,H,4,A,0,\"abcABC\"\r\n\0",
//"PRINT 1,1\r\n\0"
//};
u8 const ucPrintString[20][80] = {
	"SIZE 82 mm,10 mm\r\n\0",    //ֽ�Ŵ�С
	"GAP 2 mm,0 mm\r\n\0",       //ֽ��֮��ļ��
	"REFERENCE 0,0\r\n\0",       //�ο�ԭ��
	"SPEED 4.0\r\n\0",           //��ӡ�ٶ� 4
	"DENSITY 12\r\n\0",           //�ּ�Ũ�ȣ�0~15 ��15����Ũ��
	"SET PEEL OFF\r\n\0",        //
	"SET CUTTER OFF\r\n\0",
	"SET PARTIAL_CUTTER OFF\r\n\0",
	"SET TEAR ON\r\n\0",         //��ֽ��˺��λ��
	"DIRECTION 1\r\n\0",         //����
	"SHIFT 0\r\n\0",             //��������ƫ��
	"OFFSET 0 mm\r\n\0",         //�����ǩ�ڴ�ӡ�������Ƴ��ĳ���
	"CLS\r\n\0"                  //������ݻ��棻 ע���������ָ���������SIZEָ��֮��
};
#define STRLEN      13
//�����ַ���
 static s8 GP9034T_SendData(u8 const *pData,u8 len)
{
	u8 i=0;
	for(i=0;i<len;i++)
	{
		USART_SendData(GP9034T_COM,pData[i]);    //����һ���ֽ�
	    while(USART_GetFlagStatus(GP9034T_COM,USART_FLAG_TXE)==RESET);//����Ƿ��ͳɹ�  
		//PLC_IfExit();
	}
	return 1;
}

static void GP9034T_SendString(char *pcString)
{
	unsigned char ucLen;
	unsigned char ucCnt;
	ucLen = strlen(pcString);
    GP9034T_SendData(pcString,ucLen);
}
/***************************************/
//��������Data2Char
//���ܣ�  ����������ת��Ϊ�ַ�
//���������
//ucData����Ҫת������ 0~15
//return������ת�����
/***************************************/
u8 Data2Char(u8 ucData)
{
	if (ucData < 0x0a){ //����ת��
		ucData += 0x30;
	}else{  //�ַ�ת��
		ucData -= 9;
		ucData += 0x40;
    }
	return ucData;
}

/***************************************/
//��������Data2String
//���ܣ�  ����������ת��Ϊ�ַ�������������ַ���
//���������
//pucString�������ַ�����ַ��ת���õ��ַ�����䵽pucString��
//uiYM: ���£�YY-MM����ʮ����
//ulPhyID: �豸��ID(ʮ������)����λ��ǰ��λ�ں�
//ucDeviceCode���豸��(ʮ������)
/***************************************/
static void Data2String(char *pucString, u16 uiYM, unsigned long ulPhyID, u8 ucDeviceCode)
{
	u8 ucCnt;
	u8 ucTemp;
	uiYM = uiYM % 10000;//������YYMM�Ĳ���ȥ����
	ucCnt = 1;  //��ַ���Աܿ����ţ�"�������괦��ʼ���
    pucString[ucCnt++] = uiYM / 1000 + 0x30; //���ʮλ
	pucString[ucCnt++] = (uiYM % 1000)/100 + 0x30; //��ĸ�λ
	pucString[ucCnt++] = (uiYM % 100)/10 + 0x30; //�µ�ʮλ
	pucString[ucCnt++] = (uiYM % 10) + 0x30; //�µĸ�λ
	pucString[ucCnt++] = 0x20; //�ո�
	ucTemp = (u8)((ulPhyID>>28) & 0x0f);
	pucString[ucCnt++] = Data2Char(ucTemp);
	ucTemp = (u8)((ulPhyID>>24) & 0x0f);
	pucString[ucCnt++] = Data2Char(ucTemp);
	ucTemp = (u8)((ulPhyID>>20) & 0x0f);
	pucString[ucCnt++] = Data2Char(ucTemp);
	ucTemp = (u8)((ulPhyID>>16) & 0x0f);
	pucString[ucCnt++] = Data2Char(ucTemp);
	ucTemp = (u8)((ulPhyID>>12) & 0x0f);
	pucString[ucCnt++] = Data2Char(ucTemp);
	ucTemp = (u8)((ulPhyID>>8) & 0x0f);
	pucString[ucCnt++] = Data2Char(ucTemp);
	ucTemp = (u8)((ulPhyID>>4) & 0x0f);
	pucString[ucCnt++] = Data2Char(ucTemp);
	ucTemp = (u8)(ulPhyID & 0x0f);
	pucString[ucCnt++] = Data2Char(ucTemp);
	pucString[ucCnt++] = 0x20; //�ո�
	ucTemp = (u8)((ucDeviceCode>>4) & 0x0f);
	pucString[ucCnt++] = Data2Char(ucTemp);
	ucTemp = (u8)(ucDeviceCode & 0x0f);
	pucString[ucCnt++] = Data2Char(ucTemp);
}
/***************************************/
//��������GP9034T_SetParm
//���ܣ�  ���ô�ӡ���Ĳ���
//˵����
//ucPrintString��������
/***************************************/
static void GP9034T_SetParm(void)
{
	unsigned char ucLen;
	unsigned char ucCnt;
	for(ucCnt = 0; ucCnt < STRLEN; ucCnt++){
	    ucLen = strlen(ucPrintString[ucCnt]);
		GP9034T_SendData(ucPrintString[ucCnt],ucLen);
	}
	
}
/***************************************/
//��������GP9034T_Print4
//���ܣ�  ��ӡ����ӡ4��ID
//���������
//ulPhyID����Ҫ��ӡ��ID
/***************************************/
void GP9034T_Print4(unsigned long ulPhyID)
{
	char *pucPrint1Location = "BARCODE 12,24,\"128M\",60,1,0,2,4,";
	char *pucPrint2Location = "BARCODE 514,24,\"128M\",60,1,0,2,3,";
	char *pucPrintRN = "\r\n";
	char *pcPrintOrder = "PRINT 1,2\r\n";  //������ӡ����
	char pcPrint1[] = "\"1603 01000001 73\"\0";
	char pcPrint2[] = "\"1603 01000001 73\"\0";
	unsigned char ucLen;
	unsigned char ucCnt;
	//װ��ID
	Data2String(pcPrint1, PRINT_YEAR_MONTH, ulPhyID, PRINT_DEVICE_CODE);
	Data2String(pcPrint2, PRINT_YEAR_MONTH, ulPhyID, PRINT_DEVICE_CODE);
	GP9034T_SetParm();
	//���͵�1��2���Ĳ���
	GP9034T_SendString(pucPrint1Location);
	GP9034T_SendString(pcPrint1);
	GP9034T_SendString(pucPrintRN);
	//���͵�3��4���Ĳ���
	GP9034T_SendString(pucPrint2Location);
	GP9034T_SendString(pcPrint2);
	GP9034T_SendString(pucPrintRN);
	//������ӡ����
	GP9034T_SendString(pcPrintOrder);
}
/***************************************/
//��������GP9034T_Print2
//���ܣ�  ��ӡ����ӡ4��ID
//���������
//ulPhyID����Ҫ��ӡ��ID
/***************************************/
void GP9034T_Print2(unsigned long ulPhyID)
{
	char *pucPrint1Location = "BARCODE 12,24,\"128M\",60,1,0,2,4,";
	char *pucPrint2Location = "BARCODE 514,24,\"128M\",60,1,0,2,3,";
	char *pucPrintRN = "\r\n";
	char *pcPrintOrder = "PRINT 1,1\r\n";  //������ӡ����
	char pcPrint1[] = "\"1603 01000001 73\"\0";
	char pcPrint2[] = "\"1603 01000001 73\"\0";
	unsigned char ucLen;
	unsigned char ucCnt;
	//װ��ID
	Data2String(pcPrint1, PRINT_YEAR_MONTH, ulPhyID, PRINT_DEVICE_CODE);
	Data2String(pcPrint2, PRINT_YEAR_MONTH, ulPhyID, PRINT_DEVICE_CODE);
	GP9034T_SetParm();
	//���͵�1���Ĳ���
	GP9034T_SendString(pucPrint1Location);
	GP9034T_SendString(pcPrint1);
	GP9034T_SendString(pucPrintRN);
	//���͵�2���Ĳ���
	GP9034T_SendString(pucPrint2Location);
	GP9034T_SendString(pcPrint2);
	GP9034T_SendString(pucPrintRN);
	//������ӡ����
	GP9034T_SendString(pcPrintOrder);
}