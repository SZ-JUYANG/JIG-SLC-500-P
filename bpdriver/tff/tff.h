/*--------------------------------------------------------------------------/
/  Tiny-FatFs - FAT file system module include file  R0.06    (C)ChaN, 2008
/---------------------------------------------------------------------------/
/ FatFs module is an experimenal project to implement FAT file system to
/ cheap microcontrollers. This is a free software and is opened for education,
/ research and development under license policy of following trems.
/
/  Copyright (C) 2008, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is no warranty.
/ * You can use, modify and/or redistribute it for personal, non-profit or
/   commercial use without any restriction under your responsibility.
/ * Redistributions of source code must retain the above copyright notice.
/
/---------------------------------------------------------------------------*/

#ifndef _FATFS

#define _MCU_ENDIAN		1
/* The _MCU_ENDIAN defines which access method is used to the FAT structure.
/  1: Enable word access.
/  2: Disable word access and use byte-by-byte access instead.
/  When the architectural byte order of the MCU is big-endian and/or address
/  miss-aligned access results incorrect behavior, the _MCU_ENDIAN must be set to 2.
/  If it is not the case, it can also be set to 1 for good code efficiency. */

#define _FS_READONLY	0
/* Setting _FS_READONLY to 1 defines read only configuration. This removes
/  writing functions, f_write, f_sync, f_unlink, f_mkdir, f_chmod, f_rename,
/  f_truncate, f_getfree and internal writing codes. */

#define _FS_MINIMIZE	1
/* The _FS_MINIMIZE option defines minimization level to remove some functions.
/  0: Full function.
/  1: f_stat, f_getfree, f_unlink, f_mkdir, f_chmod, f_truncate and f_rename are removed.
/  2: f_opendir and f_readdir are removed in addition to level 1.
/  3: f_lseek is removed in addition to level 2. */

#define	_USE_STRFUNC	2
/* To enable string functions, set _USE_STRFUNC to 1 or 2. */

#define	_USE_FORWARD	0
/* To enable f_forward function, set _USE_FORWARD to 1. */

#define _FAT32	1
/* To enable FAT32 support in addition of FAT12/16, set _FAT32 to 1. */

#define _USE_FSINFO	0
/* To enable FSInfo support on FAT32 volume, set _USE_FSINFO to 1. */

#define	_USE_SJIS	1
/* When _USE_SJIS is set to 1, Shift-JIS code transparency is enabled, otherwise
/  only US-ASCII(7bit) code can be accepted as file/directory name. */

#define	_USE_NTFLAG	1
/* When _USE_NTFLAG is set to 1, upper/lower case of the file name is preserved.
/  Note that the files are always accessed in case insensitive. */


#include "integer.h"


/* Type definition for cluster number */
#if _FAT32
typedef DWORD	CLUST;
#else
typedef WORD	CLUST;
#undef _USE_FSINFO
#define _USE_FSINFO	0
#endif


/* File system object structure */
typedef struct _FATFS {
	WORD	id;				/* File system mount ID */
	WORD	n_rootdir;		/* Number of root directory entries */
	DWORD	winsect;		/* Current sector appearing in the win[] */
	DWORD	fatbase;		/* FAT start sector */
	DWORD	dirbase;		/* Root directory start sector */
	DWORD	database;		/* Data start sector */
	CLUST	sects_fat;		/* Sectors per fat */
	CLUST	max_clust;		/* Maximum cluster# + 1 */
#if !_FS_READONLY
	CLUST	last_clust;		/* Last allocated cluster */
	CLUST	free_clust;		/* Number of free clusters */
#if _USE_FSINFO
	DWORD	fsi_sector;		/* fsinfo sector */
	BYTE	fsi_flag;		/* fsinfo dirty flag (1:must be written back) */
	BYTE	pad1;
#endif
#endif
	BYTE	fs_type;		/* FAT sub type */
	BYTE	csize;			/* Number of sectors per cluster */
	BYTE	n_fats;			/* Number of FAT copies */
	BYTE	winflag;		/* win[] dirty flag (1:must be written back) */
	BYTE	win[512];		/* Disk access window for Directory/FAT/File */
} FATFS;


/* Directory object structure */
typedef struct _DIR {
	WORD	id;			/* Owner file system mount ID */
	WORD	index;		/* Current index */
	FATFS*	fs;			/* Pointer to the owner file system object */
	CLUST	sclust;		/* Start cluster */
	CLUST	clust;		/* Current cluster */
	DWORD	sect;		/* Current sector */
} DIR;


/* File object structure */
typedef struct _FIL {
	WORD	id;				/* Owner file system mount ID */
	BYTE	flag;			/* File status flags */
	BYTE	csect;			/* Sector address in the cluster */
	FATFS*	fs;				/* Pointer to owner file system */
	DWORD	fptr;			/* File R/W pointer */
	DWORD	fsize;			/* File size */
	CLUST	org_clust;		/* File start cluster */
	CLUST	curr_clust;		/* Current cluster */
	DWORD	curr_sect;		/* Current sector */
#if !_FS_READONLY
	DWORD	dir_sect;		/* Sector containing the directory entry */
	BYTE*	dir_ptr;		/* Ponter to the directory entry in the window */
#endif
} FIL;


/* File status structure */
typedef struct _FILINFO {
	DWORD fsize;			/* Size */
	WORD fdate;				/* Date */
	WORD ftime;				/* Time */
	BYTE fattrib;			/* Attribute */
	char fname[8+1+3+1];	/* Name (8.3 format) */
} FILINFO;


/* File function return code (FRESULT) */

typedef enum {
	FR_OK = 0,			/* 0 */
	FR_NOT_READY,		/* 1 */
	FR_NO_FILE,			/* 2 */
	FR_NO_PATH,			/* 3 */
	FR_INVALID_NAME,	/* 4 */
	FR_INVALID_DRIVE,	/* 5 */
	FR_DENIED,			/* 6 */
	FR_EXIST,			/* 7 */
	FR_RW_ERROR,		/* 8 */
	FR_WRITE_PROTECTED,	/* 9 */
	FR_NOT_ENABLED,		/* 10 */
	FR_NO_FILESYSTEM,	/* 11 */
	FR_INVALID_OBJECT,	/* 12 */
	FR_MKFS_ABORTED		/* 13 (not used) */
} FRESULT;



/*-----------------------------------------------------*/
/* Tiny-FatFs module application interface             */

FRESULT f_mount (BYTE, FATFS*);						/* 安装或卸载逻辑驱动器 */
FRESULT f_open (FIL*, char*, BYTE);			/* 打开或创建文件 */
FRESULT f_read (FIL*, void*, UINT, UINT*);			/* 读文件*/
FRESULT f_write (FIL*, void*, UINT, UINT*);	/*写数据到文件*/
FRESULT f_lseek (FIL*, DWORD);						/* 移动文件指针 */
FRESULT f_close (FIL*);								/* 关闭一个打开的文件 */
FRESULT f_opendir (DIR*, char*);				/* 打开一个目录*/
FRESULT f_readdir (DIR*, FILINFO*);					/* 读一个目录 */
FRESULT f_stat (char*, FILINFO*);				/* 获得文件信息 */
FRESULT f_getfree (char*, DWORD*, FATFS**);	/* 取得卡上剩余扇区数量 */
FRESULT f_truncate (FIL*);							/* 切断文件 */
FRESULT f_sync (FIL*);								/* Flush cached data of a writing file */
FRESULT f_unlink (char*);						/* 删除文件或目录 */
FRESULT	f_mkdir (char*);						/* 创建新的目录*/
FRESULT f_chmod (char*, BYTE, BYTE);			/*改变文件属性 */
FRESULT f_utime (char*, FILINFO*);		/* 修改文件或目录的时间标签 */
FRESULT f_rename (char*, char*);		/* 重命名文件或目录 */
FRESULT f_forward (FIL*, UINT(*)(BYTE*,UINT), UINT, UINT*);	/* Forward data to the stream */
#if _USE_STRFUNC
#define f_feof(fp) ((fp)->fptr == (fp)->fsize)
#define f_EOF -1
int f_fputc (int, FIL*);								/* Put a character to the file */
int f_fputs (char*, FIL*);						/* Put a string to the file */
int f_fprintf (FIL*, char*, ...);				/* Put a formatted string to the file */
char* f_fgets (char*, int, FIL*);						/* Get a string from the file */
#endif


/* User defined function to give a current time to fatfs module */

DWORD get_fattime (void);	/* 31-25: Year(0-127 +1980), 24-21: Month(1-12), 20-16: Day(1-31) */
							/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */



/* File access control and file status flags (FIL.flag) */

#define	FA_READ				0x01
#define	FA_OPEN_EXISTING	0x00
#if !_FS_READONLY
#define	FA_WRITE			0x02
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08
#define	FA_OPEN_ALWAYS		0x10
#define FA__WRITTEN			0x20
#endif
#define FA__ERROR			0x80


/* FAT sub type (FATFS.fs_type) */

#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3


/* File attribute bits for directory entry */

#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define	AM_VOL	0x08	/* Volume label */
#define AM_LFN	0x0F	/* LFN entry */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */



/* Offset of FAT structure members */

#define BS_jmpBoot			0
#define BS_OEMName			3
#define BPB_BytsPerSec		11
#define BPB_SecPerClus		13
#define BPB_RsvdSecCnt		14
#define BPB_NumFATs			16
#define BPB_RootEntCnt		17
#define BPB_TotSec16		19
#define BPB_Media			21
#define BPB_FATSz16			22
#define BPB_SecPerTrk		24
#define BPB_NumHeads		26
#define BPB_HiddSec			28
#define BPB_TotSec32		32
#define BS_55AA				510

#define BS_DrvNum			36
#define BS_BootSig			38
#define BS_VolID			39
#define BS_VolLab			43
#define BS_FilSysType		54

#define BPB_FATSz32			36
#define BPB_ExtFlags		40
#define BPB_FSVer			42
#define BPB_RootClus		44
#define BPB_FSInfo			48
#define BPB_BkBootSec		50
#define BS_DrvNum32			64
#define BS_BootSig32		66
#define BS_VolID32			67
#define BS_VolLab32			71
#define BS_FilSysType32		82

#define	FSI_LeadSig			0
#define	FSI_StrucSig		484
#define	FSI_Free_Count		488
#define	FSI_Nxt_Free		492

#define MBR_Table			446

#define	DIR_Name			0
#define	DIR_Attr			11
#define	DIR_NTres			12
#define	DIR_CrtTime			14
#define	DIR_CrtDate			16
#define	DIR_FstClusHI		20
#define	DIR_WrtTime			22
#define	DIR_WrtDate			24
#define	DIR_FstClusLO		26
#define	DIR_FileSize		28



/* Multi-byte word access macros  */

#if _MCU_ENDIAN == 1	/* Use word access */
#define	LD_WORD(ptr)		(WORD)(*(WORD*)(BYTE*)(ptr))
#define	LD_DWORD(ptr)		(DWORD)(*(DWORD*)(BYTE*)(ptr))
#define	ST_WORD(ptr,val)	*(WORD*)(BYTE*)(ptr)=(WORD)(val)
#define	ST_DWORD(ptr,val)	*(DWORD*)(BYTE*)(ptr)=(DWORD)(val)
#elif _MCU_ENDIAN == 2	/* Use byte-by-byte access */
#define	LD_WORD(ptr)		(WORD)(((WORD)*(volatile BYTE*)((ptr)+1)<<8)|(WORD)*(volatile BYTE*)(ptr))
#define	LD_DWORD(ptr)		(DWORD)(((DWORD)*(volatile BYTE*)((ptr)+3)<<24)|((DWORD)*(volatile BYTE*)((ptr)+2)<<16)|((WORD)*(volatile BYTE*)((ptr)+1)<<8)|*(volatile BYTE*)(ptr))
#define	ST_WORD(ptr,val)	*(volatile BYTE*)(ptr)=(BYTE)(val); *(volatile BYTE*)((ptr)+1)=(BYTE)((WORD)(val)>>8)
#define	ST_DWORD(ptr,val)	*(volatile BYTE*)(ptr)=(BYTE)(val); *(volatile BYTE*)((ptr)+1)=(BYTE)((WORD)(val)>>8); *(volatile BYTE*)((ptr)+2)=(BYTE)((DWORD)(val)>>16); *(volatile BYTE*)((ptr)+3)=(BYTE)((DWORD)(val)>>24)
#else
#error Do not forget to set _MCU_ENDIAN properly!
#endif



#define _FATFS
#endif /* _FATFS */
