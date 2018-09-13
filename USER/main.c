/**
  *****************************************************************************
  *                                主控MCU主函数
  *
  *                       (C) Copyright 2016-2020, ***
  *                            All Rights Reserved
  *****************************************************************************
  *
  * @File    : main.c
  * @By      : 陈桂东
  * @Version : V1.0
  * @Date    : 2016 / 04 / 06
  *
  *****************************************************************************
  *                                   Update
  * @Version : V1.*
  * @By      : **
  * @Date    : 20** / ** / **
  * @Brief   : ***
  *
  *****************************************************************************
**/


#include "sys.h"
#include "delay.h"
#include "usart.h"

#include "usmart.h"

#include "bsp_led.h"

#include "hardiic.h"

/**
  *****************************************************************************
  * @Name   : 获取时钟总线频率
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
void RCC_GetCLK(void)
{
	RCC_ClocksTypeDef  rcc_clocks;
	
	RCC_GetClocksFreq(&rcc_clocks);
	printf("SYSCLK      = %dMHz\r\n", rcc_clocks.SYSCLK_Frequency / 1000000);
	printf("HCLK(AHB)   = %dMHz\r\n", rcc_clocks.HCLK_Frequency / 1000000);
	printf("PCLK1(APB1) = %dMHz\r\n", rcc_clocks.PCLK1_Frequency / 1000000);
	printf("PCLK2(APB2) = %dMHz\r\n", rcc_clocks.PCLK2_Frequency / 1000000);
}

void Test_IICSend(u8 slaveadd, u8 writeadd, u8 data)
{
	u8 k = 0;
	
	Hard_IICWriteOneByte(I2C1, slaveadd, writeadd, data, &k);
}

u8 Test_IICRead(u8 slaveadd, u8 readadd)
{
	u8 k = 0;
	u8 temp = 0;
	
	temp = Hard_IIC_ReadOneByte(I2C1, slaveadd, readadd, &k);
	
	return temp;
}


u8 buf[10] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};


/**
  *****************************************************************************
  * @Name   : 主函数
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
int main(void)
{
    u8 i = 0;
	u8 readbuf[64];
      
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	uart_init(115200);
	delay_init();
	
	BSP_LED_Init();  //调试灯
	usmart_dev.init(84);  //调试组件
	
	Hard_IIC_Init(I2C1, 0xA0, 0);
	
	printf ("Hard IIC Test.\r\n");
	
//	Hard_IIC_WriteNByte(I2C1, 0xA0, 0x00, 5, buf, &i);  //-->OK
//	Hard_IIC_PageWrite(I2C1, 0xA0, 0x0A, 10, buf, &i);  //-->OK
	
	Hard_IIC_PageRead(I2C1, 0xA0, 0x00, 20, readbuf, &i);  //-->OK
	
	printf ("Error:0x%x\r\n", i);
	
	for (i = 0;i < 20;i++)
	{
		printf ("Data%d = 0x%x\r\n", i, readbuf[i]);
	}
	
	i = 0;
	
	while (1)
	{
		i++;
		delay_ms(10);
		if (i > 50)
		{
			i = 0;
			LED0 = !LED0;
		}
	}
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif
