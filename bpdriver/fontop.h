#ifndef __BPFONTOP_H
#define __BPFONTOP_H

#include <include.h>
#include <bp_config.h>


//���¹����ֿ�Ķ���
#define cnt_yqzfs 846 //һ���ַ���
#define cnt_eqqszj 1410	 //	������ʼ�ֽ�
#define cnt_hz 7613		 //�ַ��ܸ���6767+846

#define address16 0x2C9D0
#define hzzjs16 32
#define address24 0x68190
#define hzzjs24 72
#define address32 0xedf00
#define hzzjs32 128

#define addrasc16 0x1dd780
#define asczjs16 16
#define addrasc24 0x1dff00
#define asczjs24 48
#define addrasc32 0x1e5a50
#define asczjs32 64



//#define hzread32 12800

u8 loadzk(u32 startaddress,char * zkfile,u32 hzzjs,u32 hzcnt);


#endif /* __BPLED_H */
