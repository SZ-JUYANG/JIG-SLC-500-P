#ifndef __BPSPIFLASH_H
#define __BPSPIFLASH_H

#include <include.h>
#include <bp_config.h>


#define flash_spi SPI2

#define spiflash_nsspin	 spi2_nss
#define spiflash_nssport spi2_gpio
#define spiflash_disable  GPIO_SetBits(spiflash_nssport,spi2_nss)
#define spiflash_enable  GPIO_ResetBits(spiflash_nssport,spi2_nss)

//÷∏¡Ó±Ì
#define SPIFLASH_ID 0xEF16
#define SPIFLASH_WriteEnable		0x06 
#define SPIFLASH_WriteDisable		0x04 
#define SPIFLASH_ReadStatusReg		0x05 
#define SPIFLASH_WriteStatusReg		0x01 
#define SPIFLASH_ReadData			0x03 
#define SPIFLASH_FastReadData		0x0B 
#define SPIFLASH_FastReadDual		0x3B 
#define SPIFLASH_PageProgram		0x02 
#define SPIFLASH_BlockErase			0xD8 
#define SPIFLASH_SectorErase		0x20 
#define SPIFLASH_ChipErase			0xC7 
#define SPIFLASH_PowerDown			0xB9 
#define SPIFLASH_ReleasePowerDown	0xAB 
#define SPIFLASH_DeviceID			0xAB 
#define SPIFLASH_ManufactDeviceID	0x90 
#define SPIFLASH_JedecDeviceID		0x9F 

#define spiflash_sendrece(data) spi_sendrece(flash_spi,data)

void spiflash_init(void);
u8 spiflash_readreg(void);   
void spiflash_writereg(u8 data);   
void spiflash_writeenable(void);   
void spiflash_writedisable(void);   
u16 spiflash_readid(void);
void spiflash_read2buf(u8* buf,u32 startaddr,u16 len);   
void spiflash_write2page(u8* buf,u32 startaddr,u16 len);
void spiflash_writewithoutcheck(u8* buf,u32 startaddr,u16 len) ;  
void spiflash_write(u8* buf,u32 startaddr,u16 len);   
void spiflash_erasechip(void);   
void spiflash_erasesector(u32 startaddr);   
void spiflash_wait4busy(void);   
void spiflash_powerdown(void);   
void spiflash_wakeup(void);   

#endif /* __BPSPIFALSH */

