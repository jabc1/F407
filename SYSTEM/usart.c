/**
  *****************************************************************************
  *                           ϵͳ����������صĺ���
  *
  *                       (C) Copyright 2000-2020, ***
  *                            All Rights Reserved
  *****************************************************************************
  *
  * @File    : usart.c
  * @By      : �¹�
  * @Version : V1.0
  * @Date    : 2012 / 10 / 20
  *
  *****************************************************************************
  *                                   Update
  * @Version : V1.0.1
  * @By      : �¹�
  * @Date    : 2014 / 05 / 24
  * @Brief   : ���Ӷ�C++����֧��
  *
  * @Version : V1.1
  * @By      : �¹�
  * @Date    : 2015 / 10 / 03
  * @Brief   : ����103��407֮���л��궨��
  *
  *****************************************************************************
**/

#include "usart.h"


#if _SYSTEM_SUPPORT_ROTS == 1
#if _RTOS_Type_Support == 1  // RAW-OS
#include "raw_api.h"
#else  // uCOS
#include "includes.h"
#endif
#endif

#if Printf_Support_En == 0
#include <stdarg.h>
#endif


/******************************************************************************
           �������´��룬֧��printf������������Ҫѡ��use MicroLIB
******************************************************************************/

#if Printf_Support_En == 1

#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR & 0x40) == 0);	//ѭ�����ͣ�ֱ���������   
	USART1->DR = (uint8_t)ch;	//��������      
	return ch;
}
#else

/**
  *****************************************************************************
  * @Name   : ���ڷ���һ���ֽ�
  *
  * @Brief  : none
  *
  * @Input  : byte�������ַ�
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void uart_sendbyte(uint8_t byte)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	USART_SendData(USART1, byte);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

/**
  *****************************************************************************
  * @Name   : ���ڷ����ַ���
  *
  * @Brief  : none
  *
  * @Input  : *str�������ַ���
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void uart_senfstring(uint8_t * str)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	while (*str != '\0')
	{
		USART_SendData(USART1, *(str++));
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
}

/**
  *****************************************************************************
  * @Name   : ���ڸ�ʽ����ӡ
  *
  * @Brief  : none
  *
  * @Input  : *format����ʽ���ַ���
  *           ...:     �䳤����
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void uart_printf(char *format, ...)
{
	va_list ap;
	char string[512];
	
	va_start(ap, format);
	vsprintf(string, format, ap);
	va_end(ap);
	
	uart_senfstring((uint8_t *)string);
}

#endif

/*************************************   end   *******************************/

/**
  *****************************************************************************
  * @Name   : ��ʼ��IO ����
  *
  * @Brief  : none
  *
  * @Input  : bound��������
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void uart_init(uint32_t bound)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
#if __CORTEX_M == 0x03  //Cortex-M3
	
	//
	//��ʱ��
	//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);
	//
	//��ʼ���ܽ�
	//
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;  //PA.9 TXD
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  //�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	#if EN_USART1_RX == 1  //ʹ�ܽ���
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  //PA.10 RXD
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	#endif
	
#elif __CORTEX_M == 0x04  //Cortex-M4
	
	//
	//��ʱ��
	//
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);  //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);  //ʹ��USART1ʱ��
	//
	//�ܽŸ���
	//
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);  //TXD�ܽ�
	#if EN_USART1_RX == 1  //ʹ�ܽ���
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);  //RXD�ܽ�
	#endif
	//
	//��ʼ���ܽ�
	//
	#if EN_USART1_RX == 1  //ʹ�ܽ���
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
	#else
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;  //ֻ��ʼ��TXD�ܽ�
	#endif
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;  //���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;  //����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
#endif  //end __CORTEX_M
	
	//
	//���ô��ڲ���
	//
	USART_InitStructure.USART_BaudRate            = bound;  //������
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;  //���ݳ���8
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;  //ֹͣλ1
	USART_InitStructure.USART_Parity              = USART_Parity_No;  //����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //������������
	#if EN_USART1_RX == 1  //ʹ�ܽ���
	USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
	#else
	USART_InitStructure.USART_Mode                = USART_Mode_Tx;
	#endif
	USART_Init(USART1, &USART_InitStructure);
	
	//
	//ʹ�ܴ���
	//
	USART_Cmd(USART1, ENABLE);
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	//
	//��ʼ�������ж����ȼ�
	//
	#if EN_USART1_RX == 1  //ʹ�ܽ���
	//
	//���������жϽ���
	//
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;  //�жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;  //��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	#endif
}

#if EN_USART1_RX == 1  //ʹ�ܽ���

//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ��� 

uint8_t USART_RX_BUF[USART_REC_LEN];  //���ջ���,���USART_REC_LEN���ֽ�

//����״̬
//bit15��������ɱ�־
//bit14�����յ�0x0d
//bit13~0�����յ�����Ч�ֽ���Ŀ�����512�ֽ�

uint16_t USART_RX_STA=0;	//����״̬���

/**
  *****************************************************************************
  * @Name   : ����1�����жϷ������
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
void USART1_IRQHandler(void)
{
	uint8_t res;

#if _SYSTEM_SUPPORT_ROTS == 1
	#if _RTOS_Type_Support == 1  // RAW-OS
	raw_enter_interrupt();
	#else  // uCOS
	OSIntEnter();
	#endif
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //���յ�����
	{
		res = USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		if((USART_RX_STA & 0x8000) == 0)//����δ���
		{
			if(USART_RX_STA & 0x4000)//���յ���0x0d
			{
				/***********************************************
                                  �޸���������
                    ���û����ݵ�����0x0d��ʱ�������Ĵ�����ж�
				***********************************************/
				
				if(res != 0x0a)
				{
					USART_RX_BUF[USART_RX_STA & 0x3fff] = 0x0d;	//���϶�ʧ��0x0d����
					USART_RX_STA++;
					USART_RX_BUF[USART_RX_STA & 0x3fff] = res;	//������������
					USART_RX_STA++;
					USART_RX_STA &= 0xbfff;						//���0x0d��־
				}
				
				/***********************************************
                                      �޸����
				***********************************************/
				
				else	USART_RX_STA |= 0x8000;	//���������
			}
			else //��û�յ�0x0d
			{	
				if(res == 0x0d)	USART_RX_STA |= 0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA & 0x3fff] = res;
					USART_RX_STA++;
					if(USART_RX_STA > (USART_REC_LEN - 1))	USART_RX_STA = 0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}	//end ����δ���   		 
	}	//end ���յ�����
#if _SYSTEM_SUPPORT_ROTS == 1
	#if _RTOS_Type_Support == 1  // RAW-OS
	raw_finish_int();
	#else  // uCOS
	OSIntExit();
	#endif
#endif
}

#endif  //end EN_USART1_RX
