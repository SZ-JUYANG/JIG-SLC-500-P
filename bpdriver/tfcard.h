#ifndef __BPTFCARD_H
#define __BPTFCARD_H

#include <include.h>
#include <bp_config.h>


// SD卡类型定义  
#define TFCARDTYPE_ERR     0X00
#define TFCARDTYPE_MMC     0X01
#define TFCARDTYPE_V1      0X02
#define TFCARDTYPE_V2      0X04
#define TFCARDTYPE_V2HC    0X06	   
// TF卡指令表  	   
#define CMD0    0       //卡复位
#define CMD1    1
#define CMD8    8       //取得SD卡支持工作电压范围数据
#define CMD9    9       //读CSD数据
#define CMD10   10      //读CID数据
#define CMD12   12      //停止数据传输
#define CMD16   16      //命令16，设置SectorSize 应返回0x00
#define CMD17   17      //读扇区
#define CMD18   18      //读多个扇区
#define CMD23   23      //设置多扇区写入前预先擦除N个块
#define CMD24   24      //写扇区
#define CMD25   25      //写多个扇区
#define ACMD41   41     //辅助命令41在发送完CMD55后发送，初始化卡
#define CMD55   55      //命令55 初始化卡
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //使能/禁止校验CRC

#define BEGINSIGLE 0xFE	   //写单块起始字节 也是读块起始字节
#define BEGINMUL 0xFC	   //写多块起始字节
#define MULSTOP 0xFD	   //写多块停止字节


//数据写入回应字意义
#define TF_DATA_OK                0x05
#define TF_DATA_CRC_ERROR         0x0B
#define TF_DATA_WRITE_ERROR       0x0D
#define TF_DATA_OTHER_ERROR       0xFF
//TF卡回应标记字
#define TF_RESPONSE_NO_ERROR      0x00
#define TF_IN_IDLE_STATE          0x01
#define TF_ERASE_RESET            0x02
#define TF_ILLEGAL_COMMAND        0x04
#define TF_COM_CRC_ERROR          0x08
#define TF_ERASE_SEQUENCE_ERROR   0x10
#define TF_ADDRESS_ERROR          0x20
#define TF_PARAMETER_ERROR        0x40
#define TF_RESPONSE_FAILURE       0xFF
 							   					

#define tfcard_spi SPI1 //亮点STM32开发板使用SPI1连接TF卡
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
