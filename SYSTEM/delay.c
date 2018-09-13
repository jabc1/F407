/**
  *****************************************************************************
  *                            系统延时相关的函数
  * 详细说明请参考《Cortex-M3权威指南(中文)》第133 ~ 134页 第8章 SysTick定时器介绍
  *
  *                       (C) Copyright 2000-2020, ***
  *                            All Rights Reserved
  *****************************************************************************
  *
  * @File    : delay.c
  * @By      : 陈桂东
  * @Version : V1.0
  * @Date    : 2012 / 10 / 20
  *
  *****************************************************************************
  *                                    Update
  * @Version : V1.0.1
  * @By      : 陈桂东
  * @Date    : 2014 / 02 / 26
  * @Brief   : 增加另外一种延时计算方法
  *
  * @Version : V1.1
  * @By      : 陈桂东
  * @Date    : 2014 / 05 / 24
  * @Brief   : A、增加对C++环境支持
  *            B、修改在跑ucos时初始化嘀嗒定时器、延时us和ms函数
  *
  * @Version : V1.2
  * @By      : 陈桂东
  * @Date    : 2015 / 10 / 03
  * @Brief   : 优化延时函数
  *
  *****************************************************************************
**/

#include "delay.h"


/******************************************************************************

* @ SysTick定时器 相关控制寄存器说明

@ 1、SysTick控制及状态寄存器（地址：0xE000_E010）复位值为0

	bit16 COUNTFLAG(R）  -> 如果在上次读取本寄存器后，SysTick已经数到了0，则该位为1。如果读取该位，该位将自动清零
	bit2  CLKSOURCE(R/W) -> 0=外部时钟源(STCLK)。1=内核时钟(FCLK)
	bit1  TICKINT(R/W)   -> 1=SysTick倒数到0时产生SysTick异常请求,0=数到0时无动作 
	bit0  ENABLE(R/W)    -> SysTick定时器的使能位

@ 2、SysTick重装载数值寄存器（地址：0xE000_E014）复位值为0

	[23:0] RELOAD(R/W) -> 当倒数至零时，将被重装载的值

@ 3、SysTick当前数值寄存器（地址：0xE000_E018） 复位值为0

	[23:0] CURRENT(R/Wc) -> 读取时返回当前倒计数的值，写它则使之清零，同时还会清除在SysTick 控制及状态寄存器中的COUNTFLAG标志

@ 4、SysTick校准数值寄存器（地址：0xE000_E01C）复位值: bit31未知。bit30未知。[23:0]为0

	bit32 NOREF(R)    -> 1=没有外部参考时钟（STCLK不可用）。0=外部参考时钟可用
	bit30 SKEW(R)     -> 1=校准值不是准确的10ms。0=校准值是准确的10ms
	[23:0] TENMS(R/W) -> 10ms的时间内倒计数的格数。芯片设计者应该通过Cortex‐M3的输入信号提供该数值。若该值读回零，则表示无法使用校准功能
	
******************************************************************************/ 

#if _SYSTEM_SUPPORT_ROTS == 1

#if _RTOS_Type_Support == 1  //RAW-OS
#include "raw_api.h"
#else  //uCOS
#include "includes.h"
#endif

/**
  *****************************************************************************
  * @Name   : SysTick定时器中断服务函数
  *
  * @Brief  : 在此编写了，则stm32f10x_it.c中就不需要编写，否则stm32f10x_it.c中就要编写
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void SysTick_Handler(void)
{
	/* 编写与SysTick定时器中断操作相关的API函数调用 */
	
	#if _RTOS_Type_Support == 1  //RAW-OS
	
	raw_enter_interrupt();
	#if (CONFIG_RAW_TASK_0 > 0)		// 开启了TASK_0特性	    
	task_0_tick_post();
	#else		
	raw_time_tick();
	#endif
	raw_finish_int();
	
	#else  //uCOS
	
	OSIntEnter();  //ucos进入中断
	OSTimeTick();  //调用ucos的时钟服务函数
	OSIntExit();  //ucos退出中断
	
	#endif
} 

#endif  /* end _SYSTEM_SUPPORT_ROTS */



#if _USER_OtherDelay == 0  //使用嘀嗒定时器做延时时基

/******************************************************************************
                               定义计算变量
******************************************************************************/

static uint8_t  fac_us=0;	//us延时倍乘数
static uint16_t fac_ms=0;	//ms延时倍乘数

#endif

/**
  *****************************************************************************
  * @Name   : 初始化延时函数
  *
  * @Brief  : 主要Cortex-M3内核对系统时钟计数单元
  *           详细见《Cortex-M3权威指南(中文)》第216页 a)  时钟周期（CYCCNT） 的内容
  *           周立功《CM3计数参考手册》第28、29页、第110、125页
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/ 
void delay_init(void)
{
#if _USER_OtherDelay == 1  //使用其他资源做延时时基
	
	DEM_CTRL |= 1<<24;  //该位必须为1，使能跟踪和调试模块的使用。详细见：周立功《CM3计数参考手册》第115页介绍
                        //在没有使用跟踪时，该位使能对功率使用的控制。它能够由应用程序或调试器使能，供ITM使用
                        //在DWT能够使用之前，调试异常和监控控制寄存器的TRCENA(bit24)位必须置位

	DWT_CTRL |= 1<<0;  //使能DWT_CYCCNT计数器。
                       //如果不使能，则计数器不执行计数操作，因此不会产生PC采样或CYCCNTENA事件。
                       //在正常使用时，CYCCNT计数器应由调试器初始化为0。
	
#else  //使用嘀嗒定时器做延时时基
	
	#if _SYSTEM_SUPPORT_ROTS == 1  //运行了OS
	uint32_t RELOAD = 0;  //挡计数器倒数到0时的重装值，有效位:0 ~ 23
	#endif
	
	/* 根据SysTick定时器的时钟分频来确定重装值 */
	/* 8分频时除以8000‘000，1分频时除以1000’000 */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);  //选择外部时钟 HCLK / 8

	fac_us = SystemCoreClock / 8000000;  //系统时钟的 1/8
	
	#if _SYSTEM_SUPPORT_ROTS == 1  //运行了OS
	
	RELOAD = SystemCoreClock / 8000000;	//每秒钟的计数次数，单位Hz
	
	#if _RTOS_Type_Support == 1  // RAW-OS
	
	RELOAD *= 1000000 / RAW_TICKS_PER_SECOND;  //根据操作系统的心跳时长来计算溢出时间，单位：KHz
                                               //RELOAD为24位计数器，最大值为:16777216
	fac_ms = 1000 / RAW_TICKS_PER_SECOND;
	
	#else  // uCOS
	
	RELOAD *= 1000000 / OS_TICKS_PER_SEC;	//根据操作系统的心跳时长来计算溢出时间，单位：KHz
	                                    //RELOAD为24位计数器，最大值为:16777216
	fac_ms = 1000 / OS_TICKS_PER_SEC;
	
	#endif
	
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;	//开启SysTick定时器中断请求
	SysTick->LOAD = RELOAD;	//溢出计数值，每1/TICKINT_CNT秒中断一次
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;      //开始倒数
	
	#else  //没有运行OS
	
	fac_ms = (uint16_t)fac_us*1000;	//ms需要的SysTick时钟数
	
	#endif
	
#endif  //end _USER_OtherDelay
}

/**
  *****************************************************************************
  * @Name   : 延时n个us
  *
  * @Brief  : none
  *
  * @Input  : nus：要延时的us数
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/ 
void delay_us(uint32_t nus)
{
#if _USER_OtherDelay == 1  //使用其他资源做延时时基
	
	uint32_t savecount,endcnt,CPU_cnt;

	savecount = DWT_CYCCNT;	//保存计数器当前数值
	CPU_cnt = nus*(SystemCoreClock/(1000*1000));	//计算达到所需延时值的CPU时钟数。即多少个系统时钟计数
											//得到更精确延时时间，减去前面代码运行的时间即可

	endcnt = savecount + CPU_cnt;	//计算所需延时时间DWT_CYCCNT的计数值，在溢出时返回到0
	
	if(endcnt > savecount)	//所需延时值大于当前计数值
	{
		while(DWT_CYCCNT < endcnt);	//循环等待所需要的延时时间的CPU时钟计数值
	}
	else	//小于当前计数值
	{
		while(DWT_CYCCNT > endcnt);	//等待计数器溢出翻转
		while(DWT_CYCCNT < endcnt);	//等待所需延时时间到达
	}
	
#else  //使用嘀嗒定时器做延时时基
	
	uint32_t temp=0;
	
	#if _SYSTEM_SUPPORT_ROTS == 1  //运行了OS
	
	uint32_t VAL_Prev=0;  //开始计时之前的值
	uint32_t VAL_Now=0;   //当前计时值
	uint32_t VAL_cnt=0;   //计数
	uint32_t Reload=SysTick->LOAD;  //获取到LOAD的值

	temp = nus*fac_us;  //得到延时的节拍数
	
	#if _RTOS_Type_Support == 1  // RAW-OS
	
	raw_disable_sche();  //阻止RAW-OS调度，防止打断us延时
	
	#else  // uCOS
	
	OSSchedLock();  //阻止ucos调度，防止打断us延时
	
	#endif
	
	VAL_Prev = SysTick->VAL;  //保存当前的计数值
	
	while(1)
	{
		VAL_Now = SysTick->VAL;  //读取数值
		if(VAL_Now != VAL_Prev)
		{
			if(VAL_Now < VAL_Prev)  VAL_cnt += VAL_Prev-VAL_Now;  //因为SysTick是一个递减的定时器
			else                      VAL_cnt += Reload - VAL_Now + VAL_Prev;

			VAL_Prev = VAL_Now;  //刷新
			if(VAL_cnt >= temp)  break;  //超过/等于需要的延时值了，则退出循环
		}
	};
	#if _RTOS_Type_Support == 1  // RAW-OS
	
	raw_enable_sche();  //开启RAW-OS调度
	
	#else  // uCOS
	
	OSSchedUnlock();  //开启ucos调度
	
	#endif
	
	#else  //没有运行os
	SysTick->LOAD = nus*fac_us; //时间加载	  		 
	SysTick->VAL = 0x00;        //清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;      //开始倒数 	 
	do
	{
		temp = SysTick->CTRL;
	}while(temp&0x01&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL = 0x00;	//清空计数器
	
	#endif  //end _SYSTEM_SUPPORT_ROTS
	
#endif  //end _USER_OtherDelay
}

/**
  *****************************************************************************
  * @Name   : 延时n个ms
  *
  * @Brief  : 1、延时ms级定义，延时范围：1 ~ 65535ms。延时最大值可变，不爆机uint32_t/1000范围即可
  *           2、SysTick->LOAD为24位寄存器,所以,最大延时为:
  *              nms <= 0xffffff*8*1000/SYSCLK
  *              SYSCLK单位为Hz,nms单位为ms
  *              注意nms的范围 0 ~ 1864(72M情况下)
  *
  * @Input  : nms：要延时的ms数
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void delay_ms(uint16_t nms)
{
#if _USER_OtherDelay == 1  //使用其他资源做延时时基
	
	delay_us((uint32_t)(nms*1000));  //采用普通的延时
	
#else  //使用嘀嗒定时器做延时时基
	
	
	#if _SYSTEM_SUPPORT_ROTS == 1  //运行了OS
	
	#if _RTOS_Type_Support == 1  // RAW-OS
	
	if(raw_os_active == RAW_OS_RUNNING && raw_int_nesting == 0)
	
	#else  //uCOS
	
	if(OSRunning == OS_TRUE && OSLockNesting == 0)  //ucos已经在跑了
	
	#endif
	{
		if(nms > fac_ms)  //延时大于ucos基数
		{
			#if _RTOS_Type_Support == 1  // RAW-OS
			
			raw_sleep(nms / fac_ms);
			
			#else  // uCOS
			
			OSTimeDly(nms/fac_ms);  //采用ucos延时
			
			#endif
		}
		nms %= fac_ms;  //ucos无法提供小于节拍的延时了
	}
	if (nms == 0)  return;  //没意义了，直接退出，不加这句，在运行RAW-OS时会死机
	delay_us((uint32_t)(nms*1000));  //采用普通的延时
	
	#else  //没有运行os
	
	uint32_t temp;
			   
	SysTick->LOAD = (uint32_t)nms*fac_ms;	//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL = 0x00;           //清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;      //开始倒数  
	do
	{
		temp = SysTick->CTRL;
	}while(temp&0x01&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL = 0x00;	//清空计数器
	
	#endif  //end _SYSTEM_SUPPORT_ROTS
	
#endif  //end _USER_OtherDelay
}
