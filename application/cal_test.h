#ifndef _CAL_TEST_H
#define _CAL_TEST_H
#include "main.h"
#define  DC_ERROR        0X01   //�豸�루device code�� ����
#define  DC_RIGHT        0X02   //�豸�루device code�� ��ȷ
#include "stm32f10x.h"
#define  MIN_POWER       20    //unit: W   С�ڸù�������Ϊ��û����
#define  NOLOAD_POWER    3.5   //unit: W ���ع��ʣ����ع��ʴ��ڸ�ֵ�򱨾�

#define  DELTA_VOLTAGE   2.0   //unit: V �����ѹ������Χ
#define  DELTA_CURRENT   0.03  //unit: A �������������Χ
#define  DELTA_POWER     1     //unit: W ���빦�ʲ�����Χ
#define  DELTA_0TO10V    0.1   //unit: V 0~10v
#define  DELTA_PWM       1     //unit: % PWM

//�������Ͷ���
#define  RETURN_SUCCESS       1   //�ɹ�����
#define  RETURN_FAIL         -1   //������󷵻�
#define  RETURN_LOOK_PARM    -2   //��������������������󷵻�
#define  RETURN_SIGNAL_OUT   -3   //����ź���������󷵻�
#define  RETURN_NO_ID        -4   //û�з���ID����
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












