/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210E-EVAL Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : BAN
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
#define  GUI_MUTEX_PRIO             10
#define  APP_TASK_START_PRIO        20
#define  APP_TASK_TEST_PRIO         23
#define  APP_TASK_GUIAPP_PRIO       25	   //显示程序在显示完成后挂起自己
#define  APP_TASK_TOUCH_PRIO	    28
#define  APP_TASK_TIME_PRIO			30
#define  OS_TASK_TMR_PRIO           (OS_LOWEST_PRIO - 2)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/
#define  APP_TASK_START_STK_SIZE    512
#define  APP_TASK_GUIAPP_STK_SIZE		512
#define  APP_TASK_TOUCH_STK_SIZE		512
#define  APP_TASK_TIME_STK_SIZE			512
#define  APP_TASK_TEST_STK_SIZE			(1*1024)


/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/





#endif
