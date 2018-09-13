/**
  *****************************************************************************
  *                            系统设置相关的函数
  *
  *                       (C) Copyright 2000-2020, ***
  *                            All Rights Reserved
  *****************************************************************************
  *
  * @File    : sys.h
  * @By      : 陈桂东
  * @Version : V1.0
  * @Date    : 2012 / 10 / 20
  *
  *****************************************************************************
  *                                   Update
  * @Version : V1.0.1
  * @By      : 陈桂东
  * @Date    : 2014 / 03 / 09
  * @Brief   : 增加GPIO共用初始化函数，方便在移植时直接宏定义即可
  *
  * @Version : V1.0.2
  * @By      : 陈桂东
  * @Date    : 2014 / 05 / 24
  * @Brief   : 增加对C++环境支持
  *
  * @Version : V1.1
  * @By      : 陈桂东
  * @Date    : 2015 / 10 / 03
  * @Brief   : 增加103和407之间切换宏定义，直接调用core_cm3.h 或者 core_cm4.h文件中的 __CORTEX_M 宏定义做区别
  *
  *****************************************************************************
**/

#ifndef _sys_h
#define _sys_h


/**
  ****************************** Support C++ **********************************
**/
#ifdef __cplusplus
	extern "C"{
#endif		
/**
  *****************************************************************************
**/

/******************************************************************************
                                  外部函数头文件
******************************************************************************/

#include "stdio.h"	//printf函数头文件

#if defined (STM32F10X_LD) || defined (STM32F10X_LD_VL) || defined (STM32F10X_MD) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL)

#include "stm32f10x.h"

#endif

#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F401xx) || defined(STM32F411xE) || defined(STM32F446xx)

#include "stm32f4xx.h"

#endif

/******************************************************************************
                         定义是否支持嵌入式操作系统
******************************************************************************/

#define _SYSTEM_SUPPORT_ROTS                     0  /* 嵌入式操作系统支持开关 */
//0：不支持
//1：支持

#define _RTOS_Type_Support                       0
//0: uCOS
//1: RAW-OS

/******************************************************************************
                              位带操作相关宏定义
                       参考《CM3权威指南》第87 ~ 92页
******************************************************************************/

#define BITBAND(addr, bitnum)                    ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr)                           *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)                   MEM_ADDR(BITBAND(addr, bitnum))

/******************************************************************************
                                  GPIO地址映射
                          基地址加上寄存器偏移地址组成
******************************************************************************/

#if __CORTEX_M == 0x03  //Cortex-M3

#define GPIOA_ODR_Addr                           (GPIOA_BASE + 0x0C)  //0x4001080C
#define GPIOB_ODR_Addr                           (GPIOB_BASE + 0x0C)  //0x40010C0C
#define GPIOC_ODR_Addr                           (GPIOC_BASE + 0x0C)  //0x4001100C
#define GPIOD_ODR_Addr                           (GPIOD_BASE + 0x0C)  //0x4001140C
#define GPIOE_ODR_Addr                           (GPIOE_BASE + 0x0C)  //0x4001180C
#define GPIOF_ODR_Addr                           (GPIOF_BASE + 0x0C)  //0x40011A0C
#define GPIOG_ODR_Addr                           (GPIOG_BASE + 0x0C)  //0x40011E0C

#define GPIOA_IDR_Addr                           (GPIOA_BASE + 0x08)  //0x40010808
#define GPIOB_IDR_Addr                           (GPIOB_BASE + 0x08)  //0x40010C08
#define GPIOC_IDR_Addr                           (GPIOC_BASE + 0x08)  //0x40011008
#define GPIOD_IDR_Addr                           (GPIOD_BASE + 0x08)  //0x40011408
#define GPIOE_IDR_Addr                           (GPIOE_BASE + 0x08)  //0x40011808
#define GPIOF_IDR_Addr                           (GPIOF_BASE + 0x08)  //0x40011A08
#define GPIOG_IDR_Addr                           (GPIOG_BASE + 0x08)  //0x40011E08

#elif __CORTEX_M == 0x04  //Cortex-M4

#define GPIOA_ODR_Addr                           (GPIOA_BASE + 0x14)  //0x40020014
#define GPIOB_ODR_Addr                           (GPIOB_BASE + 0x14)  //0x40020414
#define GPIOC_ODR_Addr                           (GPIOC_BASE + 0x14)  //0x40020814
#define GPIOD_ODR_Addr                           (GPIOD_BASE + 0x14)  //0x40020C14
#define GPIOE_ODR_Addr                           (GPIOE_BASE + 0x14)  //0x40021014
#define GPIOF_ODR_Addr                           (GPIOF_BASE + 0x14)  //0x40021414
#define GPIOG_ODR_Addr                           (GPIOG_BASE + 0x14)  //0x40021814

#define GPIOA_IDR_Addr                           (GPIOA_BASE + 0x10)  //0x40020010
#define GPIOB_IDR_Addr                           (GPIOB_BASE + 0x10)  //0x40020410
#define GPIOC_IDR_Addr                           (GPIOC_BASE + 0x10)  //0x40020810
#define GPIOD_IDR_Addr                           (GPIOD_BASE + 0x10)  //0x40020C10
#define GPIOE_IDR_Addr                           (GPIOE_BASE + 0x10)  //0x40021010
#define GPIOF_IDR_Addr                           (GPIOF_BASE + 0x10)  //0x40021410
#define GPIOG_IDR_Addr                           (GPIOG_BASE + 0x10)  //0x40021810

#endif

/******************************************************************************
                       实现单一IO操作，类似于51的IO操作
                              n值要小于IO具体数目
******************************************************************************/ 

#define PAout(n)                                 BIT_ADDR(GPIOA_ODR_Addr, n)  //输出 
#define PAin(n)                                  BIT_ADDR(GPIOA_IDR_Addr, n)  //输入 

#define PBout(n)                                 BIT_ADDR(GPIOB_ODR_Addr, n)  //输出 
#define PBin(n)                                  BIT_ADDR(GPIOB_IDR_Addr, n)  //输入 

#define PCout(n)                                 BIT_ADDR(GPIOC_ODR_Addr, n)  //输出 
#define PCin(n)                                  BIT_ADDR(GPIOC_IDR_Addr, n)  //输入 

#define PDout(n)                                 BIT_ADDR(GPIOD_ODR_Addr, n)  //输出 
#define PDin(n)                                  BIT_ADDR(GPIOD_IDR_Addr, n)  //输入 

#define PEout(n)                                 BIT_ADDR(GPIOE_ODR_Addr, n)  //输出 
#define PEin(n)                                  BIT_ADDR(GPIOE_IDR_Addr, n)  //输入

#define PFout(n)                                 BIT_ADDR(GPIOF_ODR_Addr, n)  //输出 
#define PFin(n)                                  BIT_ADDR(GPIOF_IDR_Addr, n)  //输入

#define PGout(n)                                 BIT_ADDR(GPIOG_ODR_Addr, n)  //输出 
#define PGin(n)                                  BIT_ADDR(GPIOG_IDR_Addr, n)  //输入

//typedef enum {FALSE = 0, TRUE = !FALSE} bool;

/******************************************************************************
                               外部调用功能函数
******************************************************************************/

void INTX_DISABLE                                (void);  //关闭所有中断(但是不包括fault和NMI中断)
void INTX_ENABLE                                 (void);  //开启所有中断

void SYS_SoftReset                               (void);  //系统软复位

void GPIO_SetLevel                               (GPIO_TypeDef* GPIOx, u8 Pin);  //设置GPIOx管脚输出高电平
void GPIO_ResetLevel                             (GPIO_TypeDef* GPIOx, u8 Pin);  //设置GPIOx管脚输出低电平
void GPIO_GetLevel                               (GPIO_TypeDef* GPIOx, u8 Pin, u8 In_Out);  //读取GPIOx管脚电平
void GPIO_GetAddress                             (void);  //读取GPIO分组地址


/**
  ****************************** Support C++ **********************************
**/
#ifdef __cplusplus
	}
#endif
/**
  *****************************************************************************
**/


#endif  /* end sys.h */
