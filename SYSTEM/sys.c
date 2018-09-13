/**
  *****************************************************************************
  *                            系统设置相关的函数
  *
  *                       (C) Copyright 2000-2020, ***
  *                            All Rights Reserved
  *****************************************************************************
  *
  * @File    : sys.c
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

#include "sys.h"


/**
  *****************************************************************************
  * @Name   : THUMB指令不支持汇编内联
  *
  * @Brief  : 采用如下方法实现执行汇编指令WFI
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
__asm void WFI_SET(void)
{
	WFI;    
}

/**
  *****************************************************************************
  * @Name   : 关闭所有中断(但是不包括fault和NMI中断)
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}

/**
  *****************************************************************************
  * @Name   : 开启所有中断
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}

/**
  *****************************************************************************
  * @Name   : 系统软复位
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void SYS_SoftReset(void)
{
	SCB->AIRCR = 0x05fa0000 | 0x00000004;  //具体请参考《Cortex-M3权威指南(中文).pdf》第285页内容
}

/**
  *****************************************************************************
  * @Name   : 设置GPIOx管脚输出高电平
  *
  * @Brief  : none
  *
  * @Input  : GPIOx: IO组
  *           Pin:   IO号
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void GPIO_SetLevel(GPIO_TypeDef* GPIOx, u8 Pin)
{
	GPIOx->ODR &= ~(1<<Pin);
	GPIOx->ODR |= 1<<Pin;
}

/**
  *****************************************************************************
  * @Name   : 设置GPIOx管脚输出低电平
  *
  * @Brief  : none
  *
  * @Input  : GPIOx: IO组
  *           Pin:   IO号
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void GPIO_ResetLevel(GPIO_TypeDef* GPIOx, u8 Pin)
{
	GPIOx->ODR &= ~(1<<Pin);
}

/**
  *****************************************************************************
  * @Name   : 读取GPIOx管脚电平
  *
  * @Brief  : none
  *
  * @Input  : GPIOx:  IO组
  *           Pin:    IO号
  *           In_Out: 输入寄存器还是输出寄存器
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void GPIO_GetLevel(GPIO_TypeDef* GPIOx, u8 Pin, u8 In_Out)
{
	if(In_Out)  //读取的是输出寄存器
	{
		if(GPIOx->ODR & (1<<Pin))  printf("\r\nGPIOx->Pin Out: 1\r\n");
		else                       printf("\r\nGPIOx->Pin Out: 0\r\n");
	}
	else  //读取的是输入寄存器
	{
		if(GPIOx->IDR & (1<<Pin))  printf("\r\nGPIOx->Pin In: 1\r\n");
		else                       printf("\r\nGPIOx->Pin In: 0\r\n");
	}
}

/**
  *****************************************************************************
  * @Name   : 读取GPIO分组地址
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void GPIO_GetAddress(void)
{
	printf("\r\nGPIOA Address: 0x%x\r\n", (u32)GPIOA);
	printf("GPIOB Address: 0x%x\r\n", (u32)GPIOB);
	printf("GPIOC Address: 0x%x\r\n", (u32)GPIOC);
	printf("GPIOD Address: 0x%x\r\n", (u32)GPIOD);
	printf("GPIOE Address: 0x%x\r\n", (u32)GPIOE);
	
	printf("GPIOA_Base Address: 0x%x\r\n", (u32)GPIOA_BASE);
	printf("GPIOB_Base Address: 0x%x\r\n", (u32)GPIOB_BASE);
	printf("GPIOC_Base Address: 0x%x\r\n", (u32)GPIOC_BASE);
	printf("GPIOD_Base Address: 0x%x\r\n", (u32)GPIOD_BASE);
	printf("GPIOE_Base Address: 0x%x\r\n", (u32)GPIOE_BASE);
}
