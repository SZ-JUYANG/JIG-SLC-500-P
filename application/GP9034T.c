
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
	"SIZE 82 mm,10 mm\r\n\0",    //纸张大小
	"GAP 2 mm,0 mm\r\n\0",       //纸张之间的间隔
	"REFERENCE 0,0\r\n\0",       //参考原点
	"SPEED 4.0\r\n\0",           //打印速度 4
	"DENSITY 12\r\n\0",           //字迹浓度：0~15 （15：最浓）
	"SET PEEL OFF\r\n\0",        //
	"SET CUTTER OFF\r\n\0",
	"SET PARTIAL_CUTTER OFF\r\n\0",
	"SET TEAR ON\r\n\0",         //送纸到撕线位置
	"DIRECTION 1\r\n\0",         //方向
	"SHIFT 0\r\n\0",             //设置纵向偏移
	"OFFSET 0 mm\r\n\0",         //定义标签于打印完后额外推出的长度
	"CLS\r\n\0"                  //清除数据缓存； 注意事项：此项指令必须置于SIZE指令之后
};
#define STRLEN      13
//发送字符串
 static s8 GP9034T_SendData(u8 const *pData,u8 len)
{
	u8 i=0;
	for(i=0;i<len;i++)
	{
		USART_SendData(GP9034T_COM,pData[i]);    //发送一个字节
	    while(USART_GetFlagStatus(GP9034T_COM,USART_FLAG_TXE)==RESET);//检测是否发送成功  
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
//函数名：Data2Char
//功能：  将数字数据转换为字符
//输入参数：
//ucData：需要转换的数 0~15
//return：返回转换结果
/***************************************/
u8 Data2Char(u8 ucData)
{
	if (ucData < 0x0a){ //数字转换
		ucData += 0x30;
	}else{  //字符转换
		ucData -= 9;
		ucData += 0x40;
    }
	return ucData;
}

/***************************************/
//函数名：Data2String
//功能：  将数字数据转换为字符串，并填充如字符串
//输入参数：
//pucString：填充的字符串地址（转换好的字符串填充到pucString）
//uiYM: 年月（YY-MM），十进制
//ulPhyID: 设备的ID(十六进制)，高位在前低位在后
//ucDeviceCode：设备码(十六进制)
/***************************************/
static void Data2String(char *pucString, u16 uiYM, unsigned long ulPhyID, u8 ucDeviceCode)
{
	u8 ucCnt;
	u8 ucTemp;
	uiYM = uiYM % 10000;//将大于YYMM的部分去除掉
	ucCnt = 1;  //地址加以避开引号（"），从年处开始填充
    pucString[ucCnt++] = uiYM / 1000 + 0x30; //年的十位
	pucString[ucCnt++] = (uiYM % 1000)/100 + 0x30; //年的个位
	pucString[ucCnt++] = (uiYM % 100)/10 + 0x30; //月的十位
	pucString[ucCnt++] = (uiYM % 10) + 0x30; //月的个位
	pucString[ucCnt++] = 0x20; //空格
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
	pucString[ucCnt++] = 0x20; //空格
	ucTemp = (u8)((ucDeviceCode>>4) & 0x0f);
	pucString[ucCnt++] = Data2Char(ucTemp);
	ucTemp = (u8)(ucDeviceCode & 0x0f);
	pucString[ucCnt++] = Data2Char(ucTemp);
}
/***************************************/
//函数名：GP9034T_SetParm
//功能：  设置打印机的参数
//说明：
//ucPrintString：参数表
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
//函数名：GP9034T_Print4
//功能：  打印机打印4联ID
//输入参数：
//ulPhyID：需要打印的ID
/***************************************/
void GP9034T_Print4(unsigned long ulPhyID)
{
	char *pucPrint1Location = "BARCODE 12,24,\"128M\",60,1,0,2,4,";
	char *pucPrint2Location = "BARCODE 514,24,\"128M\",60,1,0,2,3,";
	char *pucPrintRN = "\r\n";
	char *pcPrintOrder = "PRINT 1,2\r\n";  //发出打印命令
	char pcPrint1[] = "\"1603 01000001 73\"\0";
	char pcPrint2[] = "\"1603 01000001 73\"\0";
	unsigned char ucLen;
	unsigned char ucCnt;
	//装载ID
	Data2String(pcPrint1, PRINT_YEAR_MONTH, ulPhyID, PRINT_DEVICE_CODE);
	Data2String(pcPrint2, PRINT_YEAR_MONTH, ulPhyID, PRINT_DEVICE_CODE);
	GP9034T_SetParm();
	//发送第1、2联的参数
	GP9034T_SendString(pucPrint1Location);
	GP9034T_SendString(pcPrint1);
	GP9034T_SendString(pucPrintRN);
	//发送第3、4联的参数
	GP9034T_SendString(pucPrint2Location);
	GP9034T_SendString(pcPrint2);
	GP9034T_SendString(pucPrintRN);
	//发出打印命令
	GP9034T_SendString(pcPrintOrder);
}
/***************************************/
//函数名：GP9034T_Print2
//功能：  打印机打印4联ID
//输入参数：
//ulPhyID：需要打印的ID
/***************************************/
void GP9034T_Print2(unsigned long ulPhyID)
{
	char *pucPrint1Location = "BARCODE 12,24,\"128M\",60,1,0,2,4,";
	char *pucPrint2Location = "BARCODE 514,24,\"128M\",60,1,0,2,3,";
	char *pucPrintRN = "\r\n";
	char *pcPrintOrder = "PRINT 1,1\r\n";  //发出打印命令
	char pcPrint1[] = "\"1603 01000001 73\"\0";
	char pcPrint2[] = "\"1603 01000001 73\"\0";
	unsigned char ucLen;
	unsigned char ucCnt;
	//装载ID
	Data2String(pcPrint1, PRINT_YEAR_MONTH, ulPhyID, PRINT_DEVICE_CODE);
	Data2String(pcPrint2, PRINT_YEAR_MONTH, ulPhyID, PRINT_DEVICE_CODE);
	GP9034T_SetParm();
	//发送第1联的参数
	GP9034T_SendString(pucPrint1Location);
	GP9034T_SendString(pcPrint1);
	GP9034T_SendString(pucPrintRN);
	//发送第2联的参数
	GP9034T_SendString(pucPrint2Location);
	GP9034T_SendString(pcPrint2);
	GP9034T_SendString(pucPrintRN);
	//发出打印命令
	GP9034T_SendString(pcPrintOrder);
}