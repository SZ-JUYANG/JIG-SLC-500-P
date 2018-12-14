#ifndef __BPTFCARD_H
#define __BPTFCARD_H

#include <include.h>
#include <bp_config.h>


// SD�����Ͷ���  
#define TFCARDTYPE_ERR     0X00
#define TFCARDTYPE_MMC     0X01
#define TFCARDTYPE_V1      0X02
#define TFCARDTYPE_V2      0X04
#define TFCARDTYPE_V2HC    0X06	   
// TF��ָ���  	   
#define CMD0    0       //����λ
#define CMD1    1
#define CMD8    8       //ȡ��SD��֧�ֹ�����ѹ��Χ����
#define CMD9    9       //��CSD����
#define CMD10   10      //��CID����
#define CMD12   12      //ֹͣ���ݴ���
#define CMD16   16      //����16������SectorSize Ӧ����0x00
#define CMD17   17      //������
#define CMD18   18      //���������
#define CMD23   23      //���ö�����д��ǰԤ�Ȳ���N����
#define CMD24   24      //д����
#define CMD25   25      //д�������
#define ACMD41   41     //��������41�ڷ�����CMD55���ͣ���ʼ����
#define CMD55   55      //����55 ��ʼ����
#define CMD58   58      //����58����OCR��Ϣ
#define CMD59   59      //ʹ��/��ֹУ��CRC

#define BEGINSIGLE 0xFE	   //д������ʼ�ֽ� Ҳ�Ƕ�����ʼ�ֽ�
#define BEGINMUL 0xFC	   //д�����ʼ�ֽ�
#define MULSTOP 0xFD	   //д���ֹͣ�ֽ�


//����д���Ӧ������
#define TF_DATA_OK                0x05
#define TF_DATA_CRC_ERROR         0x0B
#define TF_DATA_WRITE_ERROR       0x0D
#define TF_DATA_OTHER_ERROR       0xFF
//TF����Ӧ�����
#define TF_RESPONSE_NO_ERROR      0x00
#define TF_IN_IDLE_STATE          0x01
#define TF_ERASE_RESET            0x02
#define TF_ILLEGAL_COMMAND        0x04
#define TF_COM_CRC_ERROR          0x08
#define TF_ERASE_SEQUENCE_ERROR   0x10
#define TF_ADDRESS_ERROR          0x20
#define TF_PARAMETER_ERROR        0x40
#define TF_RESPONSE_FAILURE       0xFF
 							   					

#define tfcard_spi SPI1 //����STM32������ʹ��SPI1����TF��
#define tfcard_disable  ;
#define tfcard_enable  ;

#define tfcard_sendrece(data) spi_sendrece(tfcard_spi,data)
#define led_on1 GPIO_SetBits(led_gpio,led1)

u8 tfcard_init(void);
void tfcard_fast(void);
void tfcard_slow(void);
void tfcard_disselect(void);
u8 tfcard_Select(void);
u8 tfcard_wait4ready(void);
u8 tfcard_getresponse(u8 response);
u8 tfcard_recevedata(u8*buf,u16 len);
u8 tfcard_sendblock(u8 * buf,u8 beginbyte);
u8 tfcard_sendcommand(u8 cmd, u32 arg, u8 crc);
u8 tfcard_getid(u8 *cid_data);
u8 tfcard_getcsd(u8 *ctfcard_data);
u32 tfcard_getsectorcount(void);
u8 tfcard_judgeidle(void);
u8 tfcard_readsectors(u8*buf,u32 sector,u8 cnt);
u8 tfcard_writesectors(u8*buf,u32 sector,u8 cnt);

#endif /* __BPLED_H */
