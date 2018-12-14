/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: tapdev.c,v 1.8 2006/06/07 08:39:58 adam Exp $
 */	    
#include "tapdev.h"
#include "uip.h"
#include "Enc28j60.h"


//用于固定IP地址开关打开后的IP设置，未用
#define UIP_DRIPADDR0   192
#define UIP_DRIPADDR1   168
#define UIP_DRIPADDR2   1
#define UIP_DRIPADDR3   200

const u8 mymac[6]={0x03,0x04,0x05,0x06,0x07,0x08};	//MAC地址
																				  

//网络硬件初始化代码，返回值：0，正常；1，失败；
u8 tapdev_init(void)
{   	 
	u8 i,res=0;					  
	res=Enc28j60Init((u8*)mymac);	//初始化Enc28j60					  
	//把MAC地址写入缓存区
 	for (i = 0; i < 6; i++)
		uip_ethaddr.addr[i]=mymac[i];  
	Enc28j60PhyWrite(PHLCON,0x0476);    //写指示灯状态
	return res;	
}
//读取一包数据 
//MAX_FRAMELEN：最大帧长
//uip_buf：数据包缓存区
uint16_t tapdev_read(void)
{	
	return  Enc28j60PacketReceive(MAX_FRAMELEN,uip_buf);
}
//发送一包数据  
void tapdev_send(void)
{
	Enc28j60PacketSend(uip_len,uip_buf); 
}

//












/*---------------------------------------------------------------------------*/
