#ifndef __GP9034T_H__
#define __GP9034T_H__
#include "main.h"
#define PRINT_YEAR_MONTH     1807            //YY-MM格式，十进制
#define PRINT_DEVICE_CODE    DEVICE_CODE     //十六进制格式
void GP9034T_Print4(unsigned long ulPhyID);
void GP9034T_Print2(unsigned long ulPhyID);
#endif