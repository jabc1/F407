/**
  *****************************************************************************
  *                            ϵͳ������صĺ���
  *
  *                       (C) Copyright 2000-2020, ***
  *                            All Rights Reserved
  *****************************************************************************
  *
  * @File    : sys.h
  * @By      : �¹�
  * @Version : V1.0
  * @Date    : 2012 / 10 / 20
  *
  *****************************************************************************
  *                                   Update
  * @Version : V1.0.1
  * @By      : �¹�
  * @Date    : 2014 / 03 / 09
  * @Brief   : ����GPIO���ó�ʼ����������������ֲʱֱ�Ӻ궨�弴��
  *
  * @Version : V1.0.2
  * @By      : �¹�
  * @Date    : 2014 / 05 / 24
  * @Brief   : ���Ӷ�C++����֧��
  *
  * @Version : V1.1
  * @By      : �¹�
  * @Date    : 2015 / 10 / 03
  * @Brief   : ����103��407֮���л��궨�壬ֱ�ӵ���core_cm3.h ���� core_cm4.h�ļ��е� __CORTEX_M �궨��������
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
                                  �ⲿ����ͷ�ļ�
******************************************************************************/

#include "stdio.h"	//printf����ͷ�ļ�

#if defined (STM32F10X_LD) || defined (STM32F10X_LD_VL) || defined (STM32F10X_MD) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL)

#include "stm32f10x.h"

#endif

#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F401xx) || defined(STM32F411xE) || defined(STM32F446xx)

#include "stm32f4xx.h"

#endif

/******************************************************************************
                         �����Ƿ�֧��Ƕ��ʽ����ϵͳ
******************************************************************************/

#define _SYSTEM_SUPPORT_ROTS                     0  /* Ƕ��ʽ����ϵͳ֧�ֿ��� */
//0����֧��
//1��֧��

#define _RTOS_Type_Support                       0
//0: uCOS
//1: RAW-OS

/******************************************************************************
                              λ��������غ궨��
                       �ο���CM3Ȩ��ָ�ϡ���87 ~ 92ҳ
******************************************************************************/

#define BITBAND(addr, bitnum)                    ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr)                           *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)                   MEM_ADDR(BITBAND(addr, bitnum))

/******************************************************************************
                                  GPIO��ַӳ��
                          ����ַ���ϼĴ���ƫ�Ƶ�ַ���
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
                       ʵ�ֵ�һIO������������51��IO����
                              nֵҪС��IO������Ŀ
******************************************************************************/ 

#define PAout(n)                                 BIT_ADDR(GPIOA_ODR_Addr, n)  //��� 
#define PAin(n)                                  BIT_ADDR(GPIOA_IDR_Addr, n)  //���� 

#define PBout(n)                                 BIT_ADDR(GPIOB_ODR_Addr, n)  //��� 
#define PBin(n)                                  BIT_ADDR(GPIOB_IDR_Addr, n)  //���� 

#define PCout(n)                                 BIT_ADDR(GPIOC_ODR_Addr, n)  //��� 
#define PCin(n)                                  BIT_ADDR(GPIOC_IDR_Addr, n)  //���� 

#define PDout(n)                                 BIT_ADDR(GPIOD_ODR_Addr, n)  //��� 
#define PDin(n)                                  BIT_ADDR(GPIOD_IDR_Addr, n)  //���� 

#define PEout(n)                                 BIT_ADDR(GPIOE_ODR_Addr, n)  //��� 
#define PEin(n)                                  BIT_ADDR(GPIOE_IDR_Addr, n)  //����

#define PFout(n)                                 BIT_ADDR(GPIOF_ODR_Addr, n)  //��� 
#define PFin(n)                                  BIT_ADDR(GPIOF_IDR_Addr, n)  //����

#define PGout(n)                                 BIT_ADDR(GPIOG_ODR_Addr, n)  //��� 
#define PGin(n)                                  BIT_ADDR(GPIOG_IDR_Addr, n)  //����

//typedef enum {FALSE = 0, TRUE = !FALSE} bool;

/******************************************************************************
                               �ⲿ���ù��ܺ���
******************************************************************************/

void INTX_DISABLE                                (void);  //�ر������ж�(���ǲ�����fault��NMI�ж�)
void INTX_ENABLE                                 (void);  //���������ж�

void SYS_SoftReset                               (void);  //ϵͳ��λ

void GPIO_SetLevel                               (GPIO_TypeDef* GPIOx, u8 Pin);  //����GPIOx�ܽ�����ߵ�ƽ
void GPIO_ResetLevel                             (GPIO_TypeDef* GPIOx, u8 Pin);  //����GPIOx�ܽ�����͵�ƽ
void GPIO_GetLevel                               (GPIO_TypeDef* GPIOx, u8 Pin, u8 In_Out);  //��ȡGPIOx�ܽŵ�ƽ
void GPIO_GetAddress                             (void);  //��ȡGPIO�����ַ


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
