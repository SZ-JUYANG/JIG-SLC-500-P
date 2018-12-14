#ifndef __TCP_DEMO_H__
#define __TCP_DEMO_H__		 
/* Since this file will be included by uip.h, we cannot include uip.h
   here. But we might need to include uipopt.h if we need the u8_t and
   u16_t datatypes. */
#include "uipopt.h"
#include "psock.h"
#include "stm32f10x.h"

extern u8 control_sta;	
extern u8 AdDataBuff1[];	 //20002000-200023FF
extern u8 AdDataBuff2[];	 //20002400-200027Ff
extern u8 buf_sig;  	//buf_sig=0 ������AdDataBuff1д�� buf_sig=1 AdDataBuff2д��

//����UDP��������С
#define DataBuff 2048  //UDP

//����Ӧ�ó���ص����� 
#define UIP_APPCALL uip_empty_allcall    //����ص����� 
#define UIP_UDP_APPCALL myudp_appcall

void uip_empty_allcall(void);

void myudp_appcall(void);
void myudp_send(char *,short);
void UDP_newdata(void);
void uip_polling(void); 
extern unsigned short LPORT;

#endif























