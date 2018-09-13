/**
  *****************************************************************************
  *                            ϵͳ������صĺ���
  *
  *                       (C) Copyright 2000-2020, ***
  *                            All Rights Reserved
  *****************************************************************************
  *
  * @File    : sys.c
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

#include "sys.h"


/**
  *****************************************************************************
  * @Name   : THUMBָ�֧�ֻ������
  *
  * @Brief  : �������·���ʵ��ִ�л��ָ��WFI
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
  * @Name   : �ر������ж�(���ǲ�����fault��NMI�ж�)
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
  * @Name   : ���������ж�
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
  * @Name   : ϵͳ��λ
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
	SCB->AIRCR = 0x05fa0000 | 0x00000004;  //������ο���Cortex-M3Ȩ��ָ��(����).pdf����285ҳ����
}

/**
  *****************************************************************************
  * @Name   : ����GPIOx�ܽ�����ߵ�ƽ
  *
  * @Brief  : none
  *
  * @Input  : GPIOx: IO��
  *           Pin:   IO��
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
  * @Name   : ����GPIOx�ܽ�����͵�ƽ
  *
  * @Brief  : none
  *
  * @Input  : GPIOx: IO��
  *           Pin:   IO��
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
  * @Name   : ��ȡGPIOx�ܽŵ�ƽ
  *
  * @Brief  : none
  *
  * @Input  : GPIOx:  IO��
  *           Pin:    IO��
  *           In_Out: ����Ĵ�����������Ĵ���
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void GPIO_GetLevel(GPIO_TypeDef* GPIOx, u8 Pin, u8 In_Out)
{
	if(In_Out)  //��ȡ��������Ĵ���
	{
		if(GPIOx->ODR & (1<<Pin))  printf("\r\nGPIOx->Pin Out: 1\r\n");
		else                       printf("\r\nGPIOx->Pin Out: 0\r\n");
	}
	else  //��ȡ��������Ĵ���
	{
		if(GPIOx->IDR & (1<<Pin))  printf("\r\nGPIOx->Pin In: 1\r\n");
		else                       printf("\r\nGPIOx->Pin In: 0\r\n");
	}
}

/**
  *****************************************************************************
  * @Name   : ��ȡGPIO�����ַ
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
