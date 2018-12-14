/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "tfcard.h"
/*
u8 tfcard_init(void);
void tfcard_fast(void);
void tfcard_slow(void);
void tfcard_disselect(void);
u8 tfcard_Select(void);
u8 tfcard_wait4ready(void);
u8 tfcard_getresponse(u8 response);
u8 tfcard_recevedata(u8*buf,u16 len);
u8 tfcard_sendblock(u8 * buf,u8 beginu8);
u8 tfcard_sendcommand(u8 cmd, u32 arg, u8 crc);
u8 tfcard_getid(u8 *cid_data);
u8 tfcard_getcsd(u8 *ctfcard_data);
u32 tfcard_getsectorcount(void);
u8 tfcard_judgeidle(void);
u8 tfcard_readsectors(u8*buf,u32 sector,u8 cnt);
u8 tfcard_writesectors(u8*buf,u32 sector,u8 cnt);
*/

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (			   //完成初始化，为0正确
	u8 drv				/* Physical drive nmuber (0..) */
)
{
    u8 state;

    if(drv)
    {
        return STA_NOINIT;  //仅支持磁盘0的操作
    }

    state = tfcard_init();
    if(state == STA_NODISK)
    {
        return STA_NODISK;
    }
    else if(state != 0)
    {
        return STA_NOINIT;  //其他错误：初始化失败
    }
    else
    {
        return 0;           //初始化成功
    }
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	u8 drv		/* Physical drive nmuber (0..) */
)
{
    if(drv)
    {
        return STA_NOINIT;  //仅支持磁盘0操作
    }

    //检查SD卡是否插入	   //无此功能
    //if(!SD_DET())
    //{
    //    return STA_NODISK;
    //}
    return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	u8 drv,		    /* 物理驱动器号 */
	u8 *buff,		/* 存储数据的缓冲区地址 */
	DWORD sector,	/* 扇区号 */
	u8 count		/* 要读的扇区数(1..255) */
)
{
	u8 res=0;
    if (drv || !count)
    {    
        return RES_PARERR;  //仅支持单磁盘操作，count不能等于0，否则返回参数错误
    }  	
    res = tfcard_readsectors(buff,sector,1);  //调用我们前面写好读扇区的函数！                                                
    if(res == 0x00)							  
    {
        return RES_OK;						  //res为0，读取成功
    }
    else
    {
        return RES_ERROR;					  //返回错误信息
    }
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0		
DRESULT disk_write (
	u8 drv,			/* Physical drive nmuber (0..) */
	u8 *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	u8 count			/* Number of sectors to write (1..255) */
)
{
	u8 res;

    if (drv || !count)
    {    
        return RES_PARERR;  //仅支持单磁盘操作，count不能等于0，否则返回参数错误
    }
//    if(!SD_DET())
//    {
//        return RES_NOTRDY;  //没有检测到SD卡，报NOT READY错误
//    }

    // 写操作

    res = tfcard_writesectors(buff,sector,count);

    // 返回值转换
    if(res == 0)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	u8 drv,		/* Physical drive nmuber (0..) */
	u8 ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT res;


    if (drv)
    {    
        return RES_PARERR;  //仅支持单磁盘操作，否则返回参数错误
    }
    
    //FATFS目前版本仅需处理CTRL_SYNC，GET_SECTOR_COUNT，GET_BLOCK_SIZ三个命令
    switch(ctrl)
    {
    case CTRL_SYNC:
        tfcard_Select();
        if(tfcard_wait4ready()==0)
        {
            res = RES_OK;
        }
        else
        {
            res = RES_ERROR;
        }
        tfcard_disselect();
        break;
        
    case GET_BLOCK_SIZE:
        *(WORD*)buff = 512;
        res = RES_OK;
        break;

    case GET_SECTOR_COUNT:
        *(DWORD*)buff = tfcard_getsectorcount();
        res = RES_OK;
        break;
    default:
        res = RES_PARERR;
        break;
    }

    return res;
}


/*-----------------------------------------------------------------------*/
/* User defined function to give a current time to fatfs module          */
/* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{
//    struct tm t;
    DWORD date;
//    t = Time_GetCalendarTime();
//    t.tm_year -= 1980;		//年份改为1980年起
//    t.tm_mon++;         	//0-11月改为1-12月
//    t.tm_sec /= 2;      	//将秒数改为0-29
        date = 0;
//    date = (t.tm_year << 25)|(t.tm_mon<<21)|(t.tm_mday<<16)|\
//            (t.tm_hour<<11)|(t.tm_min<<5)|(t.tm_sec);

    return date;
}
							




