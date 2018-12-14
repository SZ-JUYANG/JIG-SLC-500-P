/**
 * \addtogroup uip
 * @{
 */

/**
 * \defgroup uiparp uIP Address Resolution Protocol
 * @{
 *
 * The Address Resolution Protocol ARP is used for mapping between IP
 * addresses and link level addresses such as the Ethernet MAC
 * addresses. ARP uses broadcast queries to ask for the link level
 * address of a known IP address and the host which is configured with
 * the IP address for which the query was meant, will respond with its
 * link level address.
 *
 * \note This ARP implementation only supports Ethernet.
 */
 
/**
 * \file
 * Implementation of the ARP Address Resolution Protocol.
 * \author Adam Dunkels <adam@dunkels.com>
 *
 */

/*
 * Copyright (c) 2001-2003, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: uip_arp.c,v 1.8 2006/06/02 23:36:21 adam Exp $
 *
 */


#include "uip_arp.h"

#include <string.h>

struct arp_hdr {	   //ARP包头(含以太网包头)
  struct uip_eth_hdr ethhdr;
  u16_t hwtype;
  u16_t protocol;
  u8_t hwlen;
  u8_t protolen;
  u16_t opcode;
  struct uip_eth_addr shwaddr;
  u16_t sipaddr[2];
  struct uip_eth_addr dhwaddr;
  u16_t dipaddr[2];
};

struct ethip_hdr {	   //IP包头(含以太网包头)
  struct uip_eth_hdr ethhdr;
  /* IP header. */
  u8_t vhl,
    tos,
    len[2],
    ipid[2],
    ipoffset[2],
    ttl,
    proto;
  u16_t ipchksum;
  u16_t srcipaddr[2],
    destipaddr[2];
};

#define ARP_REQUEST 1
#define ARP_REPLY   2

#define ARP_HWTYPE_ETH 1

struct arp_entry {			 //ARP表的单元结构体
  u16_t ipaddr[2];
  struct uip_eth_addr ethaddr;
  u8_t time;
};

static const struct uip_eth_addr broadcast_ethaddr =   //本地广播的MAC地址
  {{0xff,0xff,0xff,0xff,0xff,0xff}};
static const u16_t broadcast_ipaddr[2] = {0xffff,0xffff};	//本地广播的IP地址

static struct arp_entry arp_table[UIP_ARPTAB_SIZE];		//ARP表的结构数组
static u16_t ipaddr[2];
static u8_t i, c;

static u8_t arptime;									//arp时间计数器
static u8_t tmpage;

#define BUF   ((struct arp_hdr *)&uip_buf[0])
#define IPBUF ((struct ethip_hdr *)&uip_buf[0])
/*-----------------------------------------------------------------------------------*/
/**
 * Initialize the ARP module.
 *
 */
/*-----------------------------------------------------------------------------------*/
void
uip_arp_init(void)
{
  for(i = 0; i < UIP_ARPTAB_SIZE; ++i) {
    memset(arp_table[i].ipaddr, 0, 4);
  }
}
/*-----------------------------------------------------------------------------------*/
/**
 * Periodic ARP processing function.
 *
 * This function performs periodic timer processing in the ARP module
 * and should be called at regular intervals. The recommended interval
 * is 10 seconds between the calls.
 * 每隔10秒钟调用一次该函数,该函数会扫描整个ARP表,查询其中IP地址非0(也就是已经被使用过的)
 * 的单元,检查其生存时间是否超过规定(20分钟),如果是就将其IP地址清零,也即划为未使用的单元
 */
/*-----------------------------------------------------------------------------------*/
void
uip_arp_timer(void)
{
  struct arp_entry *tabptr;
  
  ++arptime;
  for(i = 0; i < UIP_ARPTAB_SIZE; ++i) {
    tabptr = &arp_table[i];
    if((tabptr->ipaddr[0] | tabptr->ipaddr[1]) != 0 &&
       arptime - tabptr->time >= UIP_ARP_MAXAGE) {
      memset(tabptr->ipaddr, 0, 4);
    }
  }

}
/*-----------------------------------------------------------------------------------*/
/* ARP表的更新:如果检查输入的IP地址与ARP表中X单元的IP一致,那么就更新X单元的MAC地址.
   如果ARP表中找不到与输入的IP地址相同的单元,那么就创建新的表单元,存入相应的IP与MAC.
   在创建新的表单元过程中,首先使用空单元来存放,如果找不到空单元, 那么就找一个最老的
   表单元,覆盖它来存放新的IP与对应的MAC.
*/
static void	  
uip_arp_update(u16_t *ipaddr, struct uip_eth_addr *ethaddr)
{
  register struct arp_entry *tabptr;
  /* Walk through the ARP mapping table and try to find an entry to
     update. If none is found, the IP -> MAC address mapping is
     inserted in the ARP table. */
  for(i = 0; i < UIP_ARPTAB_SIZE; ++i) {

    tabptr = &arp_table[i];
    /* Only check those entries that are actually in use. */
    if(tabptr->ipaddr[0] != 0 &&
       tabptr->ipaddr[1] != 0) {

      /* Check if the source IP address of the incoming packet matches
         the IP address in this ARP table entry. */
      if(ipaddr[0] == tabptr->ipaddr[0] &&
	 ipaddr[1] == tabptr->ipaddr[1]) {
	 
	/* An old entry found, update this and return. */
	memcpy(tabptr->ethaddr.addr, ethaddr->addr, 6);
	tabptr->time = arptime;

	return;
      }
    }
  }

  /* If we get here, no existing ARP table entry was found, so we
     create one. */

  /* First, we try to find an unused entry in the ARP table. */
  for(i = 0; i < UIP_ARPTAB_SIZE; ++i) {
    tabptr = &arp_table[i];
    if(tabptr->ipaddr[0] == 0 &&
       tabptr->ipaddr[1] == 0) {
      break;
    }
  }

  /* If no unused entry is found, we try to find the oldest entry and
     throw it away. */
  if(i == UIP_ARPTAB_SIZE) {
    tmpage = 0;
    c = 0;
    for(i = 0; i < UIP_ARPTAB_SIZE; ++i) {
      tabptr = &arp_table[i];
      if(arptime - tabptr->time > tmpage) {
	tmpage = arptime - tabptr->time;
	c = i;
      }
    }
    i = c;
    tabptr = &arp_table[i];
  }

  /* Now, i is the ARP table entry which we will fill with the new
     information. */
  memcpy(tabptr->ipaddr, ipaddr, 4);
  memcpy(tabptr->ethaddr.addr, ethaddr->addr, 6);
  tabptr->time = arptime;
}
/*-----------------------------------------------------------------------------------*/
/**
 * ARP processing for incoming IP packets
 *
 * This function should be called by the device driver when an IP
 * packet has been received. The function will check if the address is
 * in the ARP cache, and if so the ARP cache entry will be
 * refreshed. If no ARP cache entry was found, a new one is created.
 *
 * This function expects an IP packet with a prepended Ethernet header
 * in the uip_buf[] buffer, and the length of the packet in the global
 * variable uip_len.
 */
/*-----------------------------------------------------------------------------------*/
#if 0
void
uip_arp_ipin(void)				 //去除以太网头结构，更新ARP表
{
  uip_len -= sizeof(struct uip_eth_hdr);	   //去除以太网头结构
	
  /* Only insert/update an entry if the source IP address of the
     incoming IP packet comes from a host on the local network. */
  if((IPBUF->srcipaddr[0] & uip_netmask[0]) !=	  //检查接收到的IP包的IP地址在不在
     (uip_hostaddr[0] & uip_netmask[0])) {		  //本地局域网中
    return;
  }
  if((IPBUF->srcipaddr[1] & uip_netmask[1]) !=
     (uip_hostaddr[1] & uip_netmask[1])) {
    return;
  }
  uip_arp_update(IPBUF->srcipaddr, &(IPBUF->ethhdr.src));	//在的话,更新ARP表
  
  return;
}
#endif /* 0 */
/*-----------------------------------------------------------------------------------*/
/**
 * ARP processing for incoming ARP packets.
 *
 * This function should be called by the device driver when an ARP
 * packet has been received. The function will act differently
 * depending on the ARP packet type: if it is a reply for a request
 * that we previously sent out, the ARP cache will be filled in with
 * the values from the ARP reply. If the incoming ARP packet is an ARP
 * request for our IP address, an ARP reply packet is created and put
 * into the uip_buf[] buffer.
 *
 * When the function returns, the value of the global variable uip_len
 * indicates whether the device driver should send out a packet or
 * not. If uip_len is zero, no packet should be sent. If uip_len is
 * non-zero, it contains the length of the outbound packet that is
 * present in the uip_buf[] buffer.
 *
 * This function expects an ARP packet with a prepended Ethernet
 * header in the uip_buf[] buffer, and the length of the packet in the
 * global variable uip_len.
 */
/*-----------------------------------------------------------------------------------*/
void
uip_arp_arpin(void)
{
  
  if(uip_len < sizeof(struct arp_hdr)) {   //检测接受数据长度,若其比ARP包头还小,
    uip_len = 0;						   //说明不是ARP包,于是令uip_len = 0,也
    return;								   //即本函数调用完后不用发包
  }
  uip_len = 0;
  
  switch(BUF->opcode) {					   //检查ARP包操作码
  case HTONS(ARP_REQUEST):				   //操作码为0x0001,即该ARP包是请求MAC地址
    /* ARP request. If it asked for our address, we send out a
       reply. */
    if(uip_ipaddr_cmp(BUF->dipaddr, uip_hostaddr)) {	//若ARP请求包的目的IP是本机,即要请求本机MAC
      /* First, we register the one who made the request in our ARP
	 table, since it is likely that we will do more communication
	 with this host in the future. */					//有ARP包来请求本机的MAC说明此请的源主机将在
      uip_arp_update(BUF->sipaddr, &BUF->shwaddr);		//接下来与本机进行更多的通信,于是可更新ARP表
      
      /* The reply opcode is 2. */
      BUF->opcode = HTONS(2);							//写ARP应答包的操作码 0x0002

      memcpy(BUF->dhwaddr.addr, BUF->shwaddr.addr, 6);	  //ARP包头中,令应答包目的MAC地址为请求包的源MAC地址
      memcpy(BUF->shwaddr.addr, uip_ethaddr.addr, 6);	  //ARP包头中,令应答包源MAC地址为本机MAC地址
      memcpy(BUF->ethhdr.src.addr, uip_ethaddr.addr, 6);  //ETHER包头中,令以太网包源MAC地址为本机MAC地址
      memcpy(BUF->ethhdr.dest.addr, BUF->dhwaddr.addr, 6);//ETHER包头中,令以太网包目的MAC地址为请求包的源MAC地址
      
      BUF->dipaddr[0] = BUF->sipaddr[0];	  //同样,进行ARP应答包IP地址的配置
      BUF->dipaddr[1] = BUF->sipaddr[1];
      BUF->sipaddr[0] = uip_hostaddr[0];
      BUF->sipaddr[1] = uip_hostaddr[1];

      BUF->ethhdr.type = HTONS(UIP_ETHTYPE_ARP);   //设置以太网包类型为ARP包
      uip_len = sizeof(struct arp_hdr);			   //设置要发送的应答包长度uip_len,从而在本函数调用结束后启动发包
    }
    break;
  case HTONS(ARP_REPLY):						   //检查ARP包操作码,为0x0002,收到的为ARP应答包
    /* ARP reply. We insert or update the ARP table if it was meant
       for us. */
    if(uip_ipaddr_cmp(BUF->dipaddr, uip_hostaddr)) {  //若判断此包目的IP是本机,那么就只需更新ARP表,记录下MAC
      uip_arp_update(BUF->sipaddr, &BUF->shwaddr);
    }
    break;
  }

  return;
}
/*-----------------------------------------------------------------------------------*/
/**
 * Prepend Ethernet header to an outbound IP packet and see if we need
 * to send out an ARP request.
 * 给即将发出的IP包加上以太网包头,并且分析是否需要发送ARP请求包
 * This function should be called before sending out an IP packet. The
 * function checks the destination IP address of the IP packet to see
 * what Ethernet MAC address that should be used as a destination MAC
 * address on the Ethernet.
 * 检查这个即将发出的IP包的目的IP地址, 决定给它加上哪个MAC地址
 * If the destination IP address is in the local network (determined
 * by logical ANDing of netmask and our IP address), the function
 * checks the ARP cache to see if an entry for the destination IP
 * address is found. If so, an Ethernet header is prepended and the
 * function returns. If no ARP cache entry is found for the
 * destination IP address, the packet in the uip_buf[] is replaced by
 * an ARP request packet for the IP address. The IP packet is dropped
 * and it is assumed that they higher level protocols (e.g., TCP)
 * eventually will retransmit the dropped packet.
 * 如果目的IP地址在本地局域网内,再检查ARP表内是否有此IP,如果有,那么就
 * 就将此ARP表内对应信息加至太网包头里,返回函数,如果没有,则需要发送ARP
 * 请求,再得到对方的MAC后再加上以太网包头从而发送IP包
 * If the destination IP address is not on the local network, the IP
 * address of the default router is used instead.
 * 如果目的IP地址不在本地局域网内, 则需要用路由地址(即网关地址)	来查找对应
 * 的MAC地址
 * When the function returns, a packet is present in the uip_buf[]
 * buffer, and the length of the packet is in the global variable
 * uip_len.
 */
/*-----------------------------------------------------------------------------------*/
void
uip_arp_out(void)
{
  struct arp_entry *tabptr;
  
  /* Find the destination IP address in the ARP table and construct
     the Ethernet header. If the destination IP addres isn't on the
     local network, we use the default router's IP address instead.

     If not ARP table entry is found, we overwrite the original IP
     packet with an ARP request for the IP address. */

  /* First check if destination is a local broadcast. */
  if(uip_ipaddr_cmp(IPBUF->destipaddr, broadcast_ipaddr)) {		 //如果发送的是本地广播则令以太
    memcpy(IPBUF->ethhdr.dest.addr, broadcast_ethaddr.addr, 6);	 //网包头目的地址为本地广播MAC
  } else {
    /* Check if the destination address is on the local network. */			  //若目的IP不在本地局域网将路
    if(!uip_ipaddr_maskcmp(IPBUF->destipaddr, uip_hostaddr, uip_netmask)) {	  //由地址赋给静态变量ipaddr
      /* Destination address was not on the local network, so we need to	  
	 use the default router's IP address instead of the destination
	 address when determining the MAC address. */
      uip_ipaddr_copy(ipaddr, uip_draddr);        
    } else {
      /* Else, we use the destination IP address. */  	//如果发送的目的IP在本地局域网
      uip_ipaddr_copy(ipaddr, IPBUF->destipaddr);       //则将目的IP赋给静态变量ipaddr
    }
      
    for(i = 0; i < UIP_ARPTAB_SIZE; ++i) {			//在ARP表中查找ipaddr是否有对应IP
      tabptr = &arp_table[i];
      if(uip_ipaddr_cmp(ipaddr, tabptr->ipaddr)) {
	break;
      }
    }

    if(i == UIP_ARPTAB_SIZE) {				//目的IP不在ARP表中
      /* The destination address was not in our ARP table, so we
	 overwrite the IP packet with an ARP request. */
													   //将uip_buf中的IP包替换为ARP请求包
      memset(BUF->ethhdr.dest.addr, 0xff, 6);	       //配置ARP请求包地址等相关参数	 
      memset(BUF->dhwaddr.addr, 0x00, 6);
      memcpy(BUF->ethhdr.src.addr, uip_ethaddr.addr, 6);
      memcpy(BUF->shwaddr.addr, uip_ethaddr.addr, 6);
    
      uip_ipaddr_copy(BUF->dipaddr, ipaddr);
      uip_ipaddr_copy(BUF->sipaddr, uip_hostaddr);
      BUF->opcode = HTONS(ARP_REQUEST); /* ARP request. */
      BUF->hwtype = HTONS(ARP_HWTYPE_ETH);
      BUF->protocol = HTONS(UIP_ETHTYPE_IP);
      BUF->hwlen = 6;
      BUF->protolen = 4;
      BUF->ethhdr.type = HTONS(UIP_ETHTYPE_ARP);

      uip_appdata = &uip_buf[UIP_TCPIP_HLEN + UIP_LLH_LEN];	   //保存原IP包的数据部分指针
    
      uip_len = sizeof(struct arp_hdr);		 //发送ARP请求包
      return;
    }

    /* Build an ethernet header. */
    memcpy(IPBUF->ethhdr.dest.addr, tabptr->ethaddr.addr, 6);  //目的IP在ARP表中,则将IP包的目的
  }															   //MAC地址换为ARP表中相应的MAC.
  memcpy(IPBUF->ethhdr.src.addr, uip_ethaddr.addr, 6);
  
  IPBUF->ethhdr.type = HTONS(UIP_ETHTYPE_IP);

  uip_len += sizeof(struct uip_eth_hdr);     //发送以太网IP广播包
}

/*-----------------------------------------------------------------------------------*/

/** @} */
/** @} */
