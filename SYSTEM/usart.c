/**
  *****************************************************************************
  *                           系统串口设置相关的函数
  *
  *                       (C) Copyright 2000-2020, ***
  *                            All Rights Reserved
  *****************************************************************************
  *
  * @File    : usart.c
  * @By      : 陈桂东
  * @Version : V1.0
  * @Date    : 2012 / 10 / 20
  *
  *****************************************************************************
  *                                   Update
  * @Version : V1.0.1
  * @By      : 陈桂东
  * @Date    : 2014 / 05 / 24
  * @Brief   : 增加对C++环境支持
  *
  * @Version : V1.1
  * @By      : 陈桂东
  * @Date    : 2015 / 10 / 03
  * @Brief   : 增加103和407之间切换宏定义
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
           加入以下代码，支持printf函数，而不需要选择use MicroLIB
******************************************************************************/

#if Printf_Support_En == 1

#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR & 0x40) == 0);	//循环发送，直到发送完毕   
	USART1->DR = (uint8_t)ch;	//发送数据      
	return ch;
}
#else

/**
  *****************************************************************************
  * @Name   : 串口发送一个字节
  *
  * @Brief  : none
  *
  * @Input  : byte：发送字符
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
  * @Name   : 串口发送字符串
  *
  * @Brief  : none
  *
  * @Input  : *str：发送字符串
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
  * @Name   : 串口格式化打印
  *
  * @Brief  : none
  *
  * @Input  : *format：格式化字符串
  *           ...:     变长参数
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
  * @Name   : 初始化IO 串口
  *
  * @Brief  : none
  *
  * @Input  : bound：波特率
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
	//打开时钟
	//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);
	//
	//初始化管脚
	//
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;  //PA.9 TXD
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	#if EN_USART1_RX == 1  //使能接收
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  //PA.10 RXD
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	#endif
	
#elif __CORTEX_M == 0x04  //Cortex-M4
	
	//
	//打开时钟
	//
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);  //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);  //使能USART1时钟
	//
	//管脚复用
	//
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);  //TXD管脚
	#if EN_USART1_RX == 1  //使能接收
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);  //RXD管脚
	#endif
	//
	//初始化管脚
	//
	#if EN_USART1_RX == 1  //使能接收
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
	#else
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;  //只初始化TXD管脚
	#endif
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;  //复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;  //上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
#endif  //end __CORTEX_M
	
	//
	//配置串口参数
	//
	USART_InitStructure.USART_BaudRate            = bound;  //波特率
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;  //数据长度8
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;  //停止位1
	USART_InitStructure.USART_Parity              = USART_Parity_No;  //无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //无数据流控制
	#if EN_USART1_RX == 1  //使能接收
	USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
	#else
	USART_InitStructure.USART_Mode                = USART_Mode_Tx;
	#endif
	USART_Init(USART1, &USART_InitStructure);
	
	//
	//使能串口
	//
	USART_Cmd(USART1, ENABLE);
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	//
	//初始化串口中断优先级
	//
	#if EN_USART1_RX == 1  //使能接收
	//
	//开启串口中断接收
	//
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;  //中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //抢先优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;  //响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	#endif
}

#if EN_USART1_RX == 1  //使能接收

//注意,读取USARTx->SR能避免莫名其妙的错误 

uint8_t USART_RX_BUF[USART_REC_LEN];  //接收缓冲,最大USART_REC_LEN个字节

//接收状态
//bit15：接收完成标志
//bit14：接收到0x0d
//bit13~0：接收到的有效字节数目，最大512字节

uint16_t USART_RX_STA=0;	//接收状态标记

/**
  *****************************************************************************
  * @Name   : 串口1接收中断服务程序
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
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收到数据
	{
		res = USART_ReceiveData(USART1);	//读取接收到的数据
		
		if((USART_RX_STA & 0x8000) == 0)//接收未完成
		{
			if(USART_RX_STA & 0x4000)//接收到了0x0d
			{
				/***********************************************
                                  修改内容如下
                    当用户数据当中有0x0d的时候修正的错误的判断
				***********************************************/
				
				if(res != 0x0a)
				{
					USART_RX_BUF[USART_RX_STA & 0x3fff] = 0x0d;	//补上丢失的0x0d数据
					USART_RX_STA++;
					USART_RX_BUF[USART_RX_STA & 0x3fff] = res;	//继续接收数据
					USART_RX_STA++;
					USART_RX_STA &= 0xbfff;						//清除0x0d标志
				}
				
				/***********************************************
                                      修改完成
				***********************************************/
				
				else	USART_RX_STA |= 0x8000;	//接收完成了
			}
			else //还没收到0x0d
			{	
				if(res == 0x0d)	USART_RX_STA |= 0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA & 0x3fff] = res;
					USART_RX_STA++;
					if(USART_RX_STA > (USART_REC_LEN - 1))	USART_RX_STA = 0;//接收数据错误,重新开始接收	  
				}		 
			}
		}	//end 接收未完成   		 
	}	//end 接收到数据
#if _SYSTEM_SUPPORT_ROTS == 1
	#if _RTOS_Type_Support == 1  // RAW-OS
	raw_finish_int();
	#else  // uCOS
	OSIntExit();
	#endif
#endif
}

#endif  //end EN_USART1_RX
