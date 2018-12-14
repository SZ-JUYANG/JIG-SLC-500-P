#include "stm32f10x.h"
#include "FlashOperate.h"
/*******************************************************************************
* Function Name : ReadDataFromFlash
* Description   : Read data from Flash
* Input         : *RamAdrStar   uiSum: total of bytes reading from FLASH
* Output        : None.
* Return        : None.
*******************************************************************************/
static void ReadDataFromFlash(unsigned char *RamAdrStar, unsigned long ulFlashAdr, unsigned short int uiSum)
{
    unsigned short int uiCnt;
    for(uiCnt = 0; uiCnt < uiSum; uiCnt++){
        *(RamAdrStar + uiCnt) = *(__IO uint8_t*)(ulFlashAdr + uiCnt);
    }
}
/*******************************************************************************
* Function Name : WriteDataToFlash
* Description   : Write data to Flash
* Input         : *RamAdrStar:RAM     uiSum: total of bytes writting to FLASH
* Output        : None.
* Return        : None.
*******************************************************************************/
static void WriteDataToFlash(unsigned char *RamAdrStar, unsigned long ulFlashAdr, unsigned short int uiSum)
{
    unsigned short int uiCnt;
    for(uiCnt = 0; uiCnt < uiSum; uiCnt += 2){    /* write 2 bytes every time */
        FLASH_ProgramHalfWord(ulFlashAdr + uiCnt, (*(RamAdrStar + uiCnt + 1)<<8) + *(RamAdrStar + uiCnt));
    }
}
/*******************************************************************************
* Function Name : WriteHalfWordToFlash
* Description   : Write 16 bit to Flash
* Input         : uiWord
* Output        : None.
* Return        : None.
*******************************************************************************/
static void WriteHalfWordToFlash(unsigned short int uiWord, unsigned long ulFlashAdr)
{
    unsigned char ucTemp[2];
    ucTemp[0] = (unsigned char)uiWord;
    ucTemp[1] = (unsigned char)(uiWord>>8);
    WriteDataToFlash(&ucTemp[0], ulFlashAdr, 2);
}

/*******************************************************************************
* Function Name : WriteHalfWordToFlash
* Description   : Write 16 bit to Flash
* Input         : uiWord
* Output        : None.
* Return        : None.
*******************************************************************************/
void WritePhyId2Flash(unsigned long ulPhyID, unsigned long ulFlashAdr)
{
	unsigned char ucTemp[4];
	unsigned long ulPhyIdTemp;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR |FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(ulFlashAdr);
	ucTemp[0] = (unsigned char)((ulPhyID>>24)&0x000000ff);
	ucTemp[1] = (unsigned char)((ulPhyID>>16)&0x000000ff);
	ucTemp[2] = (unsigned char)((ulPhyID>>8)&0x000000ff);
	ucTemp[3] = (unsigned char)((ulPhyID)&0x000000ff);
	WriteDataToFlash(ucTemp, ulFlashAdr, 4); 
	FLASH_Lock();
}
/*******************************************************************************
* Function Name : ReadPhyIdFromFlash
* Description   : Write 16 bit to Flash
* Input         : uiWord
* Output        : None.
* Return        : None.
*******************************************************************************/
unsigned long ReadPhyIdFromFlash(unsigned long ulFlashAdr)
{
	unsigned long ulPhyID;
	unsigned char ucTemp[6];
	ReadDataFromFlash(ucTemp, ulFlashAdr, 4); 
	ulPhyID = ((unsigned long )(ucTemp[0]) << 24) | ((unsigned long )(ucTemp[1]) << 16) | ((unsigned long )(ucTemp[2]) << 8) | ucTemp[3];
	return ulPhyID;
}
