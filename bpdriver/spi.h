#ifndef __BPSPI_H
#define __BPSPI_H

#include <include.h>
#include <bp_config.h>
#define SPI_TIMEOUT 200
void spi_initn(	SPI_TypeDef *);
u8 spi_sendrece(SPI_TypeDef *,u8);
void spi_setspeed(SPI_TypeDef * spi_n,u8 speed); 
#endif /* __BPSPI_H */
