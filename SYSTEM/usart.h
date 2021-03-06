/**
  *****************************************************************************
  *                           系统串口设置相关的函数
  *
  *                       (C) Copyright 2000-2020, ***
  *                            All Rights Reserved
  *****************************************************************************
  *
  * @File    : usart.h
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

#ifndef _usart_h
#define _usart_h


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

#include "sys.h"

/******************************************************************************
                         定义串口是否接受外部数据
******************************************************************************/

#define Printf_Support_En                        1  //是否支持C标准库printf（）函数
                                                    //0: 不支持
                                                    //1: 支持

#define EN_USART1_RX                             1  //串口接收使能。0：禁止接收
                                                    //             1：允许接收
#define USART_REC_LEN                            64  //定义最大接收字节数

/******************************************************************************
                              外部调用功能函数
******************************************************************************/

#if EN_USART1_RX == 1  //使能接收
extern uint8_t  USART_RX_BUF[USART_REC_LEN];  //接收缓冲,最大USART_REC_LEN个字节
extern uint16_t USART_RX_STA;	//接收状态标记
#endif

void uart_init                                   (uint32_t bound);  //初始化IO 串口

#if Printf_Support_En == 0
void uart_sendbyte                               (uint8_t byte);  //串口发送一个字节
void uart_senfstring                             (uint8_t * str);  //串口发送字符串
void uart_printf                                 (char *format, ...);  //串口格式化打印
#endif


/**
  ****************************** Support C++ **********************************
**/
#ifdef __cplusplus
	}
#endif
/**
  *****************************************************************************
**/


#endif  /* end usart.h */
