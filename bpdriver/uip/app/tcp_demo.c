#include "stm32f10x.h"
#include "usart.h"	 		   
#include "uip.h"	    
#include "enc28j60.h"
#include "tcp_demo.h"


void uip_empty_allcall(void)
{
	;
}

//UDP������
void myudp_appcall(void) 
{ 
   if(uip_newdata()) //����н��յ�����						
   { 
       UDP_newdata();      
   } 
} 

//UDP���ݰ�����
void myudp_send(char *str,short n) 
{ 

   char   *nptr;   
   nptr = (char *)uip_appdata; //uip_appdataΪ uip.c��ȫ�ֱ�����ָ���ͻ�����      
   memcpy(nptr, str, n); 	   //�������ַ������������ͻ�����
   uip_udp_send(n);   		   //����n������ 
} 

//UDP�������ݷ���
void UDP_newdata(void) 
{ 
    char   *nptr; 
    nptr = (char *)uip_appdata; //ȡ��������ʼָ�� 

	if(strncmp(nptr,"on",2)==0){
		control_sta = (control_sta & 254)|((~control_sta)&1);	
	}
	else myudp_send("Unkown command!\n",16); 
} 


//��ӡ��־��
void uip_log(char *m)
{			    
	printf("uIP log:%s\r\n",m);
}

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])	
//uip��ѯ�����û���ѭ������.
void uip_polling(void)
{
	static struct timer periodic_timer, arp_timer;	   //����������timer���͵Ľṹ����,�Ƿֱ���������TCP��ʱ��ARP��ʱ�Ķ�ʱ��
	static u8 timer_ok=0;	 
	if(timer_ok==0)//����ʼ��һ��
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //����1��0.5��Ķ�ʱ�� 	 #define CLOCK_CONF_SECOND 1000
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//����1��10��Ķ�ʱ�� ����ÿ10�뽫ARP�����
	}				 
	uip_len=tapdev_read();	//�������豸��ȡһ��IP��,uip_len��uip.c�ж���
	if(uip_len>0) 			//���������
	{   
		//����IP���ݰ�(ֻ��У��ͨ����IP���Żᱻ����) 
		if(BUF->type == htons(UIP_ETHTYPE_IP))//�Ƿ���IP��? 
		{
			uip_arp_ipin();	//ȥ����̫��ͷ�ṹ������ARP��
			uip_input();   	//IP������
			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
			//��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)		    
			if(uip_len>0)//��Ҫ��Ӧ����
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			} 
		}else if (BUF->type==htons(UIP_ETHTYPE_ARP))//����arp����,�Ƿ���ARP�����?
		{
			uip_arp_arpin();   //�յ�ARP����ĵĴ���
 			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len(����2��ȫ�ֱ���)
 			if(uip_len>0)tapdev_send();//��Ҫ��������,��ͨ��tapdev_send����	 
		}
	}
/* else if(timer_expired(&periodic_timer))	//0.5�붨ʱ����ʱ
		{
			led_turn4;
			timer_reset(&periodic_timer);		//��λ0.5�붨ʱ�� 
#if UIP_UDP	//UIP_UDP 
			uip_udp_periodic(0);	//����UDPͨ���¼�
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
			if(uip_len > 0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
#endif 
			//ÿ��10�����1��ARP��ʱ������ ���ڶ���ARP����,ARP��10�����һ�Σ��ɵ���Ŀ�ᱻ����
*/
			if(timer_expired(&arp_timer))
			{
				led_turn3;
				timer_reset(&arp_timer);
				uip_arp_timer();
			}

}

 //



