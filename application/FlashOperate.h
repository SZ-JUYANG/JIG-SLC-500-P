#ifndef __FLASH_OPERATE_H__
#define __FLASH_OPERATE_H__

#define   FLASH_SAVE_ADR          0x08077800
#define   PHY_ID_SEGMENT_NUMBER   1  //ID的段号，从1开始。每个段号有2000个ID
#define   PHY_ID_START            (0X01001388 + 2000 * (PHY_ID_SEGMENT_NUMBER - 1))  //从
#define   PHY_ID_END              (PHY_ID_START + 2000)
#define   PHY_ID_DEFAULT          0X01000001

#define   FLASH_TEST_MODE_ADR     0x08078800//
#define   HAND_MODE               0X00000001      //手动测试模式
#define   AUTO_MODE               0X00000002      //自动测试模式
#define   TEST_MODE               0X00000003      //出厂检测模式
#define   CHECK_ID_MODE           0X00000004      //检测治具使用的ID情况
#define   DEFAULT_MODE            HAND_MODE
#define   MAX_MODE_VALUE          CHECK_ID_MODE       //最大的模式值

void WritePhyId2Flash(unsigned long ulPhyID, unsigned long ulFlashAdr);

unsigned long ReadPhyIdFromFlash(unsigned long ulFlashAdr);

#endif


