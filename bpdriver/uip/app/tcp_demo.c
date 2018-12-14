#include "stm32f10x.h"
#include "usart.h"	 		   
#include "uip.h"	    
#include "enc28j60.h"
#include "tcp_demo.h"


void uip_empty_allcall(void)
{
	;
}

//UDP主函数
void myudp_appcall(void) 
{ 
   if(uip_newdata()) //如果有接收到数据						
   { 
       UDP_newdata();      
   } 
} 

//UDP数据包发送
void myudp_send(char *str,short n) 
{ 

   char   *nptr;   
   nptr = (char *)uip_appdata; //uip_appdata为 uip.c中全局变量，指向发送缓冲区      
   memcpy(nptr, str, n); 	   //将发送字符串拷贝到发送缓冲区
   uip_udp_send(n);   		   //发送n个数据 
} 

//UDP接收数据分析
void UDP_newdata(void) 
{ 
    char   *nptr; 
    nptr = (char *)uip_appdata; //取得数据起始指针 

	if(strncmp(nptr,"on",2)==0){
		control_sta = (control_sta & 254)|((~control_sta)&1);	
	}
	else myudp_send("Unkown command!\n",16); 
} 


//打印日志用
void uip_log(char *m)
{			    
	printf("uIP log:%s\r\n",m);
}

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])	
//uip轮询，由用户主循环调用.
void uip_polling(void)
{
	static struct timer periodic_timer, arp_timer;	   //定义了两个timer类型的结构变量,是分别用来处理TCP超时、ARP超时的定时器
	static u8 timer_ok=0;	 
	if(timer_ok==0)//仅初始化一次
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //创建1个0.5秒的定时器 	 #define CLOCK_CONF_SECOND 1000
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//创建1个10秒的定时器 用于每10秒将ARP表清空
	}				 
	uip_len=tapdev_read();	//从网络设备读取一个IP包,uip_len在uip.c中定义
	if(uip_len>0) 			//如果有数据
	{   
		//处理IP数据包(只有校验通过的IP包才会被接收) 
		if(BUF->type == htons(UIP_ETHTYPE_IP))//是否是IP包? 
		{
			uip_arp_ipin();	//去除以太网头结构，更新ARP表
			uip_input();   	//IP包处理
			//当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
			//需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)		    
			if(uip_len>0)//需要回应数据
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			} 
		}else if (BUF->type==htons(UIP_ETHTYPE_ARP))//处理arp报文,是否是ARP请求包?
		{
			uip_arp_arpin();   //收到ARP包后的的处理
 			//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len(这是2个全局变量)
 			if(uip_len>0)tapdev_send();//需要发送数据,则通过tapdev_send发送	 
		}
	}
/* else if(timer_expired(&periodic_timer))	//0.5秒定时器超时
		{
			led_turn4;
			timer_reset(&periodic_timer);		//复位0.5秒定时器 
#if UIP_UDP	//UIP_UDP 
			uip_udp_periodic(0);	//处理UDP通信事件
	 		//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
			if(uip_len > 0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
#endif 
			//每隔10秒调用1次ARP定时器函数 用于定期ARP处理,ARP表10秒更新一次，旧的条目会被抛弃
*/
			if(timer_expired(&arp_timer))
			{
				led_turn3;
				timer_reset(&arp_timer);
				uip_arp_timer();
			}

}

 //



