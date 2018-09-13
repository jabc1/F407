/**
  *****************************************************************************
  *                            ϵͳ��ʱ��صĺ���
  * ��ϸ˵����ο���Cortex-M3Ȩ��ָ��(����)����133 ~ 134ҳ ��8�� SysTick��ʱ������
  *
  *                       (C) Copyright 2000-2020, ***
  *                            All Rights Reserved
  *****************************************************************************
  *
  * @File    : delay.c
  * @By      : �¹�
  * @Version : V1.0
  * @Date    : 2012 / 10 / 20
  *
  *****************************************************************************
  *                                    Update
  * @Version : V1.0.1
  * @By      : �¹�
  * @Date    : 2014 / 02 / 26
  * @Brief   : ��������һ����ʱ���㷽��
  *
  * @Version : V1.1
  * @By      : �¹�
  * @Date    : 2014 / 05 / 24
  * @Brief   : A�����Ӷ�C++����֧��
  *            B���޸�����ucosʱ��ʼ����શ�ʱ������ʱus��ms����
  *
  * @Version : V1.2
  * @By      : �¹�
  * @Date    : 2015 / 10 / 03
  * @Brief   : �Ż���ʱ����
  *
  *****************************************************************************
**/

#include "delay.h"


/******************************************************************************

* @ SysTick��ʱ�� ��ؿ��ƼĴ���˵��

@ 1��SysTick���Ƽ�״̬�Ĵ�������ַ��0xE000_E010����λֵΪ0

	bit16 COUNTFLAG(R��  -> ������ϴζ�ȡ���Ĵ�����SysTick�Ѿ�������0�����λΪ1�������ȡ��λ����λ���Զ�����
	bit2  CLKSOURCE(R/W) -> 0=�ⲿʱ��Դ(STCLK)��1=�ں�ʱ��(FCLK)
	bit1  TICKINT(R/W)   -> 1=SysTick������0ʱ����SysTick�쳣����,0=����0ʱ�޶��� 
	bit0  ENABLE(R/W)    -> SysTick��ʱ����ʹ��λ

@ 2��SysTick��װ����ֵ�Ĵ�������ַ��0xE000_E014����λֵΪ0

	[23:0] RELOAD(R/W) -> ����������ʱ��������װ�ص�ֵ

@ 3��SysTick��ǰ��ֵ�Ĵ�������ַ��0xE000_E018�� ��λֵΪ0

	[23:0] CURRENT(R/Wc) -> ��ȡʱ���ص�ǰ��������ֵ��д����ʹ֮���㣬ͬʱ���������SysTick ���Ƽ�״̬�Ĵ����е�COUNTFLAG��־

@ 4��SysTickУ׼��ֵ�Ĵ�������ַ��0xE000_E01C����λֵ: bit31δ֪��bit30δ֪��[23:0]Ϊ0

	bit32 NOREF(R)    -> 1=û���ⲿ�ο�ʱ�ӣ�STCLK�����ã���0=�ⲿ�ο�ʱ�ӿ���
	bit30 SKEW(R)     -> 1=У׼ֵ����׼ȷ��10ms��0=У׼ֵ��׼ȷ��10ms
	[23:0] TENMS(R/W) -> 10ms��ʱ���ڵ������ĸ�����оƬ�����Ӧ��ͨ��Cortex�\M3�������ź��ṩ����ֵ������ֵ�����㣬���ʾ�޷�ʹ��У׼����
	
******************************************************************************/ 

#if _SYSTEM_SUPPORT_ROTS == 1

#if _RTOS_Type_Support == 1  //RAW-OS
#include "raw_api.h"
#else  //uCOS
#include "includes.h"
#endif

/**
  *****************************************************************************
  * @Name   : SysTick��ʱ���жϷ�����
  *
  * @Brief  : �ڴ˱�д�ˣ���stm32f10x_it.c�оͲ���Ҫ��д������stm32f10x_it.c�о�Ҫ��д
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
	/* ��д��SysTick��ʱ���жϲ�����ص�API�������� */
	
	#if _RTOS_Type_Support == 1  //RAW-OS
	
	raw_enter_interrupt();
	#if (CONFIG_RAW_TASK_0 > 0)		// ������TASK_0����	    
	task_0_tick_post();
	#else		
	raw_time_tick();
	#endif
	raw_finish_int();
	
	#else  //uCOS
	
	OSIntEnter();  //ucos�����ж�
	OSTimeTick();  //����ucos��ʱ�ӷ�����
	OSIntExit();  //ucos�˳��ж�
	
	#endif
} 

#endif  /* end _SYSTEM_SUPPORT_ROTS */



#if _USER_OtherDelay == 0  //ʹ����શ�ʱ������ʱʱ��

/******************************************************************************
                               ����������
******************************************************************************/

static uint8_t  fac_us=0;	//us��ʱ������
static uint16_t fac_ms=0;	//ms��ʱ������

#endif

/**
  *****************************************************************************
  * @Name   : ��ʼ����ʱ����
  *
  * @Brief  : ��ҪCortex-M3�ں˶�ϵͳʱ�Ӽ�����Ԫ
  *           ��ϸ����Cortex-M3Ȩ��ָ��(����)����216ҳ a)  ʱ�����ڣ�CYCCNT�� ������
  *           ��������CM3�����ο��ֲᡷ��28��29ҳ����110��125ҳ
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
#if _USER_OtherDelay == 1  //ʹ��������Դ����ʱʱ��
	
	DEM_CTRL |= 1<<24;  //��λ����Ϊ1��ʹ�ܸ��ٺ͵���ģ���ʹ�á���ϸ������������CM3�����ο��ֲᡷ��115ҳ����
                        //��û��ʹ�ø���ʱ����λʹ�ܶԹ���ʹ�õĿ��ơ����ܹ���Ӧ�ó���������ʹ�ܣ���ITMʹ��
                        //��DWT�ܹ�ʹ��֮ǰ�������쳣�ͼ�ؿ��ƼĴ�����TRCENA(bit24)λ������λ

	DWT_CTRL |= 1<<0;  //ʹ��DWT_CYCCNT��������
                       //�����ʹ�ܣ����������ִ�м�����������˲������PC������CYCCNTENA�¼���
                       //������ʹ��ʱ��CYCCNT������Ӧ�ɵ�������ʼ��Ϊ0��
	
#else  //ʹ����શ�ʱ������ʱʱ��
	
	#if _SYSTEM_SUPPORT_ROTS == 1  //������OS
	uint32_t RELOAD = 0;  //��������������0ʱ����װֵ����Чλ:0 ~ 23
	#endif
	
	/* ����SysTick��ʱ����ʱ�ӷ�Ƶ��ȷ����װֵ */
	/* 8��Ƶʱ����8000��000��1��Ƶʱ����1000��000 */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);  //ѡ���ⲿʱ�� HCLK / 8

	fac_us = SystemCoreClock / 8000000;  //ϵͳʱ�ӵ� 1/8
	
	#if _SYSTEM_SUPPORT_ROTS == 1  //������OS
	
	RELOAD = SystemCoreClock / 8000000;	//ÿ���ӵļ�����������λHz
	
	#if _RTOS_Type_Support == 1  // RAW-OS
	
	RELOAD *= 1000000 / RAW_TICKS_PER_SECOND;  //���ݲ���ϵͳ������ʱ�����������ʱ�䣬��λ��KHz
                                               //RELOADΪ24λ�����������ֵΪ:16777216
	fac_ms = 1000 / RAW_TICKS_PER_SECOND;
	
	#else  // uCOS
	
	RELOAD *= 1000000 / OS_TICKS_PER_SEC;	//���ݲ���ϵͳ������ʱ�����������ʱ�䣬��λ��KHz
	                                    //RELOADΪ24λ�����������ֵΪ:16777216
	fac_ms = 1000 / OS_TICKS_PER_SEC;
	
	#endif
	
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;	//����SysTick��ʱ���ж�����
	SysTick->LOAD = RELOAD;	//�������ֵ��ÿ1/TICKINT_CNT���ж�һ��
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;      //��ʼ����
	
	#else  //û������OS
	
	fac_ms = (uint16_t)fac_us*1000;	//ms��Ҫ��SysTickʱ����
	
	#endif
	
#endif  //end _USER_OtherDelay
}

/**
  *****************************************************************************
  * @Name   : ��ʱn��us
  *
  * @Brief  : none
  *
  * @Input  : nus��Ҫ��ʱ��us��
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/ 
void delay_us(uint32_t nus)
{
#if _USER_OtherDelay == 1  //ʹ��������Դ����ʱʱ��
	
	uint32_t savecount,endcnt,CPU_cnt;

	savecount = DWT_CYCCNT;	//�����������ǰ��ֵ
	CPU_cnt = nus*(SystemCoreClock/(1000*1000));	//����ﵽ������ʱֵ��CPUʱ�����������ٸ�ϵͳʱ�Ӽ���
											//�õ�����ȷ��ʱʱ�䣬��ȥǰ��������е�ʱ�伴��

	endcnt = savecount + CPU_cnt;	//����������ʱʱ��DWT_CYCCNT�ļ���ֵ�������ʱ���ص�0
	
	if(endcnt > savecount)	//������ʱֵ���ڵ�ǰ����ֵ
	{
		while(DWT_CYCCNT < endcnt);	//ѭ���ȴ�����Ҫ����ʱʱ���CPUʱ�Ӽ���ֵ
	}
	else	//С�ڵ�ǰ����ֵ
	{
		while(DWT_CYCCNT > endcnt);	//�ȴ������������ת
		while(DWT_CYCCNT < endcnt);	//�ȴ�������ʱʱ�䵽��
	}
	
#else  //ʹ����શ�ʱ������ʱʱ��
	
	uint32_t temp=0;
	
	#if _SYSTEM_SUPPORT_ROTS == 1  //������OS
	
	uint32_t VAL_Prev=0;  //��ʼ��ʱ֮ǰ��ֵ
	uint32_t VAL_Now=0;   //��ǰ��ʱֵ
	uint32_t VAL_cnt=0;   //����
	uint32_t Reload=SysTick->LOAD;  //��ȡ��LOAD��ֵ

	temp = nus*fac_us;  //�õ���ʱ�Ľ�����
	
	#if _RTOS_Type_Support == 1  // RAW-OS
	
	raw_disable_sche();  //��ֹRAW-OS���ȣ���ֹ���us��ʱ
	
	#else  // uCOS
	
	OSSchedLock();  //��ֹucos���ȣ���ֹ���us��ʱ
	
	#endif
	
	VAL_Prev = SysTick->VAL;  //���浱ǰ�ļ���ֵ
	
	while(1)
	{
		VAL_Now = SysTick->VAL;  //��ȡ��ֵ
		if(VAL_Now != VAL_Prev)
		{
			if(VAL_Now < VAL_Prev)  VAL_cnt += VAL_Prev-VAL_Now;  //��ΪSysTick��һ���ݼ��Ķ�ʱ��
			else                      VAL_cnt += Reload - VAL_Now + VAL_Prev;

			VAL_Prev = VAL_Now;  //ˢ��
			if(VAL_cnt >= temp)  break;  //����/������Ҫ����ʱֵ�ˣ����˳�ѭ��
		}
	};
	#if _RTOS_Type_Support == 1  // RAW-OS
	
	raw_enable_sche();  //����RAW-OS����
	
	#else  // uCOS
	
	OSSchedUnlock();  //����ucos����
	
	#endif
	
	#else  //û������os
	SysTick->LOAD = nus*fac_us; //ʱ�����	  		 
	SysTick->VAL = 0x00;        //��ռ�����
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;      //��ʼ���� 	 
	do
	{
		temp = SysTick->CTRL;
	}while(temp&0x01&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
	SysTick->VAL = 0x00;	//��ռ�����
	
	#endif  //end _SYSTEM_SUPPORT_ROTS
	
#endif  //end _USER_OtherDelay
}

/**
  *****************************************************************************
  * @Name   : ��ʱn��ms
  *
  * @Brief  : 1����ʱms�����壬��ʱ��Χ��1 ~ 65535ms����ʱ���ֵ�ɱ䣬������uint32_t/1000��Χ����
  *           2��SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
  *              nms <= 0xffffff*8*1000/SYSCLK
  *              SYSCLK��λΪHz,nms��λΪms
  *              ע��nms�ķ�Χ 0 ~ 1864(72M�����)
  *
  * @Input  : nms��Ҫ��ʱ��ms��
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void delay_ms(uint16_t nms)
{
#if _USER_OtherDelay == 1  //ʹ��������Դ����ʱʱ��
	
	delay_us((uint32_t)(nms*1000));  //������ͨ����ʱ
	
#else  //ʹ����શ�ʱ������ʱʱ��
	
	
	#if _SYSTEM_SUPPORT_ROTS == 1  //������OS
	
	#if _RTOS_Type_Support == 1  // RAW-OS
	
	if(raw_os_active == RAW_OS_RUNNING && raw_int_nesting == 0)
	
	#else  //uCOS
	
	if(OSRunning == OS_TRUE && OSLockNesting == 0)  //ucos�Ѿ�������
	
	#endif
	{
		if(nms > fac_ms)  //��ʱ����ucos����
		{
			#if _RTOS_Type_Support == 1  // RAW-OS
			
			raw_sleep(nms / fac_ms);
			
			#else  // uCOS
			
			OSTimeDly(nms/fac_ms);  //����ucos��ʱ
			
			#endif
		}
		nms %= fac_ms;  //ucos�޷��ṩС�ڽ��ĵ���ʱ��
	}
	if (nms == 0)  return;  //û�����ˣ�ֱ���˳���������䣬������RAW-OSʱ������
	delay_us((uint32_t)(nms*1000));  //������ͨ����ʱ
	
	#else  //û������os
	
	uint32_t temp;
			   
	SysTick->LOAD = (uint32_t)nms*fac_ms;	//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL = 0x00;           //��ռ�����
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;      //��ʼ����  
	do
	{
		temp = SysTick->CTRL;
	}while(temp&0x01&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
	SysTick->VAL = 0x00;	//��ռ�����
	
	#endif  //end _SYSTEM_SUPPORT_ROTS
	
#endif  //end _USER_OtherDelay
}
