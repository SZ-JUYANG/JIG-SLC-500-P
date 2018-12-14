#ifndef __LCD12864_H__
#define __LCD12864_H__

#define  READY                   0
#define  WAIT_START              1
#define  CHECKING                2
#define  GET_ID                  3
#define  READ_PARAM              4
#define  NO_ACK                  5   //DPS没响应
#define  FAIL2GETID              6
#define  ADJUST_VOLTAGE_IN       7
#define  ADJUST_CURRENT_IN       8
#define  ADJUST_POWER_IN         9
#define  WAIT_NEXT               10
#define  FAIL2ADJUST_VOLTAGE_IN  11
#define  FAIL2ADJUST_CURRENT_IN  12
#define  FAIL2ADJUST_POWER_IN    13
#define  FACT_SETTING            14
#define  SUCCESS_FACT_SET        15
#define  FAIL_FACT_SETTING       16
#define  SUCCESS2CHECK           17
#define  READING_PM9805          18
#define  FAIL2READ_PM9805        19
#define  TESTING_0_10V           20
#define  FAIL2TEST_0_10V         21
#define  ERROR_NO                22   
#define  SHOW_NULL               23 
#define  ANY_KEY2BACK            24
#define  TEST_PASS               25
#define  LINKING                 26
#define  FAIL2LINK               27
#define  PHY_ID                  28
#define  ASK_FACT_SET            29
#define  ASK_SURE                30
#define  ASK_PRINT               31
#define  PRINT_FINISH            32
#define  PRINTING                33
#define  PRINT_OR_BACK           34
#define  YES_OR_NO               35
#define  ASK_SET_PHYID           36
#define  SETTING_PHYID           37
#define  FAIL2SET_PHYID          38
#define  SETTING_TEST_MODE       39
#define  NOW_AUTO                40
#define  NOW_HAND                41
#define  ASK_AUTO_HAND           42
#define  NOW_TSET                43
#define  ASK_TSET                44
#define  VOL_SHOW                45
#define  CUR_SHOW                46
#define  POW_SHOW                47
#define  FACT_TEST               48
#define  ANY_KEY2LOOK            49
#define  NOT_CHECK               50
#define  CHECKING_LOAD           51
#define  PHYID_USE_UP            52
#define  ZERO2TEN                53
#define  PWM_PERCENT             54
#define  SET_FACT                55
#define  TEST_NOLOAD_POWER       56
#define  SHOW_BAR                57
#define  KEY1_FUNCTION           58
#define  KEY2_FUNCTION           59
#define  KEY3_FUNCTION           60
#define  KEY4_FUNCTION           61
#define  CHECK_ZHIJU             62
#define  CHECK_SHOW_1            63
#define  CHECK_SHOW_2            64
#define  CHECK_SHOW_3            65
#define  CHECK_SHOW_4            66
#define  CHECK_ID_NUM            67
#define  CHECK_SEG_START         68
#define  CHECK_SEG_END           69
#define  CHECK_ID_NOW            70
#define  GET_LORA_ID_FAIL        71
#define  GET_PLC_ID_FAIL         72
#define  CLEARING_ENERGY         73
#define  DC_ERROR                74
#define  S0_10V_HSI_CAL          75
#define  S0_10V_CAL              76
#define  HSI_CAL                 77
#define  S0_10V_FAIL             78
#define  HSI_FAIL                79
#define  S0_10V_HSI_FAIL         80
#define  TESTING_RS485           81
#define  RS485_FAIL              82
#define  SET_LORA_TEST_MODE      83
#define  LORA_TESTING            84
#define  LORA_TEST_FAIL          85
#define  LORA_TEST_SUCCESS       86
#define  JIG_LORA_ID             87
#define  SET_AUTO_FAIL           88
//#define adjust

//错误代码定义
#define  ERROR_FAIL2GETID                1      //无法读出待测设备ID
#define  ERROR_NO_ACK                    2      //待测设备无数据返回
#define  ERROR_FAIL2READ_PM9805          3      //无法读取功率计PM9805的参数     
#define  ERROR_FAIL2ADJUST_VOLTAGE_IN    4      //校正输入电压失败
#define  ERROR_FAIL2ADJUST_CURRENT_IN    5      //校正输入电流失败
#define  ERROR_FAIL2ADJUST_POWER_IN      6      //校正输入功率失败
#define  ERROR_FAIL2SAVE                 7      //待测设备保存校正参数失败
#define  ERROR_VOLTAGE_IN                8      //输入电压误差过大
#define  ERROR_CURRENT_IN                9      //输入电流误差过大
#define  ERROR_POWER_IN                  10     //输入功率误差过大
#define  ERROR_FAIL2SETFACT              11     //恢复出厂设置失败
#define  ERROR_FAIL2TEST_0_10V           12     //0~10V测试不通过
#define  ERROR_FAIL2LINK                 13     //组网失败
#define  ERROR_FAIL2SET_PHYID            14     //分配ID失败
#define  ERROR_NOT_CHECK                 15     //未分配ID
#define  EEROR_NO_LOAD                   16     //灯不亮
#define  EEROR_PWM                       17     //PWM误差超出范围
#define  EEROR_NOLOAD_POWER              18     //空载功率过大
#define  EEROR_PHYID_USE_UP              19     //测试治具节点ID使用完
#define  EEROR_LORA_FAIL                 20     //无法获得LORA的ID
#define  EEROR_GET_ID_FAIL               21     //无法申请到ID
#define  EEROR_RETURN_ID_FAIL            22     //回应网关失败
#define  EEROR_SET_AUTO_FALL             23     //成功进入安全模式，但是配置auto routing slave模式失败
#define  EEROR_S0_10V_FAIL               24     //0~10V信号线校准失败
#define  EEROR_HSI_FAIL                  25     //HSI信号线校准失败 
#define  EEROR_S0_10V_HSI_FAIL           26     //0~10V和HSI信号线校准失败
#define  EEROR_RS485_FAULT               27     //RS485测试失败
#define  EEROR_SIMPLE_FAULT              28     //设置为simple模式失败
#define  EEROR_LORA_FAULT                29     //LORA测试失败
#define  EEROR_AUTO_FAULT                30     //LORA测试成功后，配置为auto routing模式失败

#define  EEROR_FEEDBACK_ID_FAIL          99     //回复网关ID和将ID设置为默认值失败，需要重新烧写PLC程序


//液晶的行
#define LINE_1                  0X80
#define LINE_2                  0X90
#define LINE_3                  0X88
#define LINE_4                  0X98
void Lcd12864_init(void);
void Lcd12864_clear(void);
void Lcd12864_string(unsigned char ucAdd,const char *p);

#endif