#ifndef _CAL_TEST_H
#define _CAL_TEST_H
#include "main.h"
#define  DC_ERROR        0X01   //设备码（device code） 错误
#define  DC_RIGHT        0X02   //设备码（device code） 正确
#include "stm32f10x.h"
#define  MIN_POWER       20    //unit: W   小于该功率则认为灯没有亮
#define  NOLOAD_POWER    3.5   //unit: W 空载功率；空载功率大于该值则报警

#define  DELTA_VOLTAGE   2.0   //unit: V 输入电压测量误差范围
#define  DELTA_CURRENT   0.03  //unit: A 输入电流测量误差范围
#define  DELTA_POWER     1     //unit: W 输入功率测量误差范围
#define  DELTA_0TO10V    0.1   //unit: V 0~10v
#define  DELTA_PWM       1     //unit: % PWM

//返回类型定义
#define  RETURN_SUCCESS       1   //成功返回
#define  RETURN_FAIL         -1   //常规错误返回
#define  RETURN_LOOK_PARM    -2   //输入电参数测量误差过大错误返回
#define  RETURN_SIGNAL_OUT   -3   //输出信号误差过大错误返回
#define  RETURN_NO_ID        -4   //没有分配ID错误
#define  RETURN_DC_ERROR     -5   //
s8 CalTest_Cal(void *msg);
s8 ProductCheck(void *msg);
s8 ProductParmCheck(void *msg, u8 ucMode);
u32 ReadProductPhyID(void *msg);
void CheckPhyID(unsigned char ucData);
void WritePhyIdToucID(unsigned long ulPhyID);

#ifndef PRODUCT_TYPE
  #error "Please define PRODUCT_TYPE!"
#endif
#if   PRODUCT_TYPE == 2
s8 TestNemaRS485(void);
#endif
#endif












