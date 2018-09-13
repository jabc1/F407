/**
  *****************************************************************************
  *                             STM32硬件IIC控制函数
  *
  *                       (C) Copyright 2000-2020, ***
  *                             All Rights Reserved
  *****************************************************************************
  *
  * @File    : hardiic.c
  * @By      : 陈桂东
  * @Version : V1.0
  * @Date    : 2016 / 03 / 08
  *
  *****************************************************************************
  *
  *                                   Update
  *
  * @Version : V1.*
  * @By      : ***
  * @Date    : 20** / ** / **
  * @Brief   : ***
  *
  *****************************************************************************
**/


#include "hardiic.h"


/**
  *****************************************************************************
  * @Name   : 硬件IIC初始化
  *
  * @Brief  : none
  *
  * @Input  : I2Cx:           IIC组
  *           SlaveAdd:       作为从设备时识别地址
  *           F103IIC1_Remap: 针对F103的IIC1是否重映射
  *                           0: 不重映射
  *                           1: 重映射。PB.06 -> PB.08；PB.07 -> PB.09
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void Hard_IIC_Init(I2C_TypeDef* IICx, u8 SlaveAdd, u8 F103IIC1_Remap/* 针对F103的IIC1重映射的 */)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	
#if __CORTEX_M == 0x03  //Cortex-M3
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	if (IICx == I2C1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
		
		//
		//初始化管脚
		//
		if (F103IIC1_Remap)
		{
			GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;
		}
		else
		{
			GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
		}
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
	else if (IICx == I2C2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
		
		//
		//初始化管脚
		//
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
	
#elif __CORTEX_M == 0x04  //Cortex-M4
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//
	//管脚复用
	//
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
	
#endif
	
	//
	//配置IIC参数
	//
	I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1         = SlaveAdd;  //从设备地址
	I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed          = (100 * 1000);  //SCL最大100KHz
	
	
	I2C_Cmd(IICx, ENABLE);
	I2C_Init(IICx, &I2C_InitStructure);
	I2C_AcknowledgeConfig(IICx, ENABLE); 
	
	//
	//禁止时钟延长
	//
	IICx->CR1 |= 1<<7;
}

/**
  *****************************************************************************
  * @Name   : 硬件IIC等待从设备内部操作完成
  *
  * @Brief  : none
  *
  * @Input  : I2Cx:     IIC组
  *           SlaveAdd: 作为从设备时识别地址
  *           ReadAdd:  读取的EEPROM内存地址
  *
  * @Output : *err:     返回的错误值
  *
  * @Return : 读取到的数据
  *****************************************************************************
**/
void Hard_IICWaiteStandby(I2C_TypeDef* IICx, uint8_t SlaveAdd)
{
	u16 tmp = 0;
	
	IICx->SR1 &= 0x0000;  //清除状态寄存器1
	
	do
	{
		I2C_GenerateSTART(IICx, ENABLE);  //产生起始信号
		tmp = IICx->SR1;  //读取SR1寄存器，然后写入数据寄存器操作来清除SB位，EV5
		I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //发送从设备地址
	} while ((IICx->SR1 & 0x0002) == 0x0000);  //当ADDR = 1时，表明应答了，跳出循环
	I2C_ClearFlag(IICx, I2C_FLAG_AF);  //清除应答失败标志位
	I2C_GenerateSTOP(IICx, ENABLE);  //发送停止信号
}

/**
  *****************************************************************************
  * @Name   : 硬件IIC发送一个字节数据
  *
  * @Brief  : none
  *
  * @Input  : I2Cx:     IIC组
  *           SlaveAdd: 作为从设备时识别地址
  *           WriteAdd: 写入EEPROM内存地址
  *           Data:     写入的数据
  *
  * @Output : *err:     返回的错误值
  *
  * @Return : none
  *****************************************************************************
**/
void Hard_IICWriteOneByte(I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 WriteAdd, u8 Data, u8 * err)
{
	u16 temp = 0;
	u16 flag = 0;
	
	while (I2C_GetFlagStatus(IICx, I2C_FLAG_BUSY))  //等待IIC
	{
		temp++;
		if (temp > 800)
		{
			*err |= 1<<0;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return;
		}
	}
	
	I2C_GenerateSTART(IICx, ENABLE);  //产生起始信号
	temp = 0;
	//
	//EV5
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		temp++;
		if (temp > 800)
		{
			*err |= 1<<1;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return;
		}
	}
	
	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //发送设备地址
	temp = 0;
	//
	//EV6
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		temp++;
		if (temp > 1000)
		{
			*err |= 1<<2;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return;
		}
	}
	//
	//读取SR2状态寄存器
	//
	flag = IICx->SR2;  //软件读取SR1寄存器后,对SR2寄存器的读操作将清除ADDR位，不可少！！！！！！！！！
	
	I2C_SendData(IICx, WriteAdd);  //发送存储地址
	temp = 0;
	//
	//EV8
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
		temp++;
		if (temp > 800)
		{
			*err |= 1<<3;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return;
		}
	}
	I2C_SendData(IICx, Data);  //发送数据
	temp = 0;
	//
	//EV8_2
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		temp++;
		if (temp > 800)
		{
			*err |= 1<<4;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return;
		}
	}
	I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
}

/**
  *****************************************************************************
  * @Name   : 硬件IIC读取一个字节数据
  *
  * @Brief  : none
  *
  * @Input  : I2Cx:     IIC组
  *           SlaveAdd: 作为从设备时识别地址
  *           ReadAdd:  读取的EEPROM内存地址
  *
  * @Output : *err:     返回的错误值
  *
  * @Return : 读取到的数据
  *****************************************************************************
**/
u8 Hard_IIC_ReadOneByte(I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 ReadAdd, u8 * err)
{
	u16 i = 0;
	u8 temp = 0;
	u16 flag = 0;
	
	while (I2C_GetFlagStatus(IICx, I2C_FLAG_BUSY))  //等待IIC
	{
		i++;
		if (i > 800)
		{
			*err |= 1<<0;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return 0;
		}
	}
	I2C_GenerateSTART(IICx, ENABLE);  //发送起始信号
	i = 0;
	//
	//EV5
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		i++;
		if (i > 800)
		{
			*err |= 1<<1;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return 0;
		}
	}
	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //发送设备地址
	i = 0;
	//
	//EV6
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		i++;
		if (i > 800)
		{
			*err |= 1<<2;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return 0;
		}
	}
	flag = IICx->SR2;  //软件读取SR1寄存器后,对SR2寄存器的读操作将清除ADDR位，不可少！！！！！！！！！
	
	I2C_SendData(IICx, ReadAdd);  //发送存储地址
	i = 0;
	//
	//EV8
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
		i++;
		if (i > 2000)
		{
			*err |= 1<<3;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return 0;
		}
	}
	I2C_GenerateSTART(IICx, ENABLE);  //重启信号
	i = 0;
	//
	//EV5
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		i++;
		if (i > 800)
		{
			*err |= 1<<4;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return 0;
		}
	}
	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Receiver);  //读取命令
	i = 0;
	//
	//EV6
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		i++;
		if (i > 800)
		{
			*err |= 1<<5;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return 0;
		}
	}
	flag = IICx->SR2;
	
	I2C_AcknowledgeConfig(IICx, DISABLE);  //发送NACK
	I2C_GenerateSTOP(IICx, ENABLE);
	i = 0;
	//
	//EV7
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_BYTE_RECEIVED))
	{
		i++;
		if (i > 800)
		{
			*err |= 1<<6;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return 0;
		}
	}
	temp = I2C_ReceiveData(IICx);
	I2C_AcknowledgeConfig(IICx, ENABLE);
	
	return temp;
}

/**
  *****************************************************************************
  * @Name   : 硬件IIC发送多个字节数据
  *
  * @Brief  : none
  *
  * @Input  : I2Cx:       IIC组
  *           SlaveAdd:   作为从设备时识别地址
  *           WriteAdd:   写入EEPROM内存起始地址
  *           NumToWrite: 写入数据量
  *           *pBuffer:   写入的数据组缓存
  *
  * @Output : *err:     返回的错误值
  *
  * @Return : none
  *****************************************************************************
**/
void Hard_IIC_WriteNByte(I2C_TypeDef * IICx, u8 SlaveAdd, u8 WriteAdd, u8 NumToWrite, u8 * pBuffer, u8 * err)
{
	u16 sta = 0;
	u16 temp = 0;
	
	while (I2C_GetFlagStatus(IICx, I2C_FLAG_BUSY))  //等待IIC
	{
		temp++;
		if (temp > 800)
		{
			*err |= 1<<0;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return;
		}
	}
	I2C_GenerateSTART(IICx, ENABLE);  //产生起始信号
	temp = 0;
	//
	//EV5
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		temp++;
		if (temp > 800)
		{
			*err |= 1<<1;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return;
		}
	}
	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //发送设备地址
	temp = 0;
	//
	//EV6
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		temp++;
		if (temp > 1000)
		{
			*err |= 1<<2;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return;
		}
	}
	//
	//读取SR2状态寄存器
	//
	sta = IICx->SR2;  //软件读取SR1寄存器后,对SR2寄存器的读操作将清除ADDR位，不可少！！！！！！！！！
	I2C_SendData(IICx, WriteAdd);  //发送存储地址
	temp = 0;
	//
	//EV8
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
		temp++;
		if (temp > 800)
		{
			*err |= 1<<3;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return;
		}
	}
	//
	//循环发送数据
	//
	while (NumToWrite--)
	{
		I2C_SendData(IICx, *pBuffer);  //发送数据
		pBuffer++;
		temp = 0;
		//
		//EV8_2
		//
		while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			temp++;
			if (temp > 800)
			{
				*err |= 1<<4;
				I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
				return;
			}
		}
	}
	I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
}

/**
  *****************************************************************************
  * @Name   : 硬件IIC读取多个字节数据
  *
  * @Brief  : none
  *
  * @Input  : I2Cx:      IIC组
  *           SlaveAdd:  作为从设备时识别地址
  *           ReadAdd:   读取的EEPROM内存起始地址
  *           NumToRead: 读取数量
  *
  * @Output : *pBuffer: 数据输出缓冲区
  *           *err:     返回的错误值
  *
  * @Return : 读取到的数据
  *****************************************************************************
**/
void Hard_IIC_PageRead(I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 ReadAdd, u8 NumToRead, u8 * pBuffer, u8 * err)
{
	u16 i = 0;
	u16 temp = 0;
	u16 sta = 0;
	
	while (I2C_GetFlagStatus(IICx, I2C_FLAG_BUSY))  //等待IIC
	{
		i++;
		if (i > 800)
		{
			*err |= 1<<0;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return;
		}
	}
	I2C_GenerateSTART(IICx, ENABLE);  //发送起始信号
	i = 0;
	//
	//EV5
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		i++;
		if (i > 800)
		{
			*err |= 1<<1;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return;
		}
	}
	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //发送设备地址
	i = 0;
	//
	//EV6
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		i++;
		if (i > 800)
		{
			*err |= 1<<2;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return;
		}
	}
	sta = IICx->SR2;  //软件读取SR1寄存器后,对SR2寄存器的读操作将清除ADDR位，不可少！！！！！！！！！
	
	I2C_SendData(IICx, ReadAdd);  //发送存储地址
	i = 0;
	//
	//EV8
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
		i++;
		if (i > 2000)
		{
			*err |= 1<<3;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return;
		}
	}
	I2C_GenerateSTART(IICx, ENABLE);  //重启信号
	i = 0;
	//
	//EV5
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		i++;
		if (i > 800)
		{
			*err |= 1<<4;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return;
		}
	}
	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Receiver);  //读取命令
	i = 0;
	//
	//EV6
	//
	while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		i++;
		if (i > 800)
		{
			*err |= 1<<5;
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return;
		}
	}
	sta = IICx->SR2;  //软件读取SR1寄存器后,对SR2寄存器的读操作将清除ADDR位，不可少！！！！！！！！！
	//
	//批量接收数据
	//
//	while (NumToRead--)
//	{
//		if (NumToRead == 1)  //最后一个数据了，不发送应答信号
//		{
//			I2C_AcknowledgeConfig(IICx, DISABLE);  //发送NACK
//			I2C_GenerateSTOP(IICx, ENABLE);
//		}
//		//
//		//判断RxNE是否为1，EV7
//		//
//		i = 0;
//		while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_BYTE_RECEIVED))
//		{
//			i++;
//			if (i > 1000)
//			{
//				*err |= 1<<6;
//				I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
//				return;
//			}
//		}
//		*pBuffer = I2C_ReceiveData(IICx);
//		pBuffer++;
//	}
	while (NumToRead)
	{
		if (NumToRead == 1)  //最后一个数据了，不发送应答信号
		{
			I2C_AcknowledgeConfig(IICx, DISABLE);  //发送NACK
			I2C_GenerateSTOP(IICx, ENABLE);
		}
		//
		//判断RxNE是否为1，EV7
		//
		if (I2C_CheckEvent(IICx, I2C_EVENT_MASTER_BYTE_RECEIVED))
		{
			*pBuffer = I2C_ReceiveData(IICx);
			pBuffer++;
			NumToRead--;
		}
	}
	I2C_AcknowledgeConfig(IICx, ENABLE);
}

/**
  *****************************************************************************
  * @Name   : AT24C02页写函数
  *
  * @Brief  : none
  *
  * @Input  : I2Cx:       IIC组
  *           SlaveAdd:   作为从设备时识别地址
  *           WriteAdd:   写入EEPROM内存起始地址
  *           NumToWrite: 写入数据量
  *           *pBuffer:   写入的数据组缓存
  *
  * @Output : *err:     返回的错误值
  *
  * @Return : none
  *****************************************************************************
**/
void Hard_IIC_PageWrite(I2C_TypeDef * IICx, u8 SlaveAdd, u8 WriteAdd, u8 NumToWrite, u8 * pBuffer, u8 * err)
{
	u8 Num_Page = 0;  //按照页大小需要写入的次数
	u8 Num_Remain = 0;  //页写剩余字节数
	u8 Addr = 0;  //数据块首地址为EEPROM页数整数倍
	u8 cnt = 0;  //计数变量
	
	Addr = WriteAdd % AT24Cxx_PageSize;  //计算数据块首地址是否是页大小的整数倍
	cnt = AT24Cxx_PageSize - Addr;
	Num_Page = NumToWrite / AT24Cxx_PageSize;  //得到次数
	Num_Remain = NumToWrite % AT24Cxx_PageSize;  //剩余字节数
	//
	//判断写入数量
	//
	if (Addr == 0)  //整数倍
	{
		if (Num_Page == 0)  //如果写入的数据块长度小于页大小
		{
			Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //小于一个页大小数据
			Hard_IICWaiteStandby(IICx, SlaveAdd);  //等待操作完成
		}
		else  //大于一页数据
		{
			while (Num_Page--)  //按照页来写
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, AT24Cxx_PageSize, pBuffer, err);
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //等待操作完成
				WriteAdd += AT24Cxx_PageSize;
				pBuffer += AT24Cxx_PageSize;
			}
			if (Num_Remain != 0)  //不够一个页的数据
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //小于一个页大小数据
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //等待操作完成
			}
		}
	}
	else  //不是整数倍
	{
		if (Num_Page == 0)  //小于一个页的数据量
		{
			Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //写入小于一个页的数据量
			Hard_IICWaiteStandby(IICx, SlaveAdd);  //等待操作完成
		}
		else  //大于一个也数据量
		{
			//
			//重新计算
			//
			NumToWrite -= cnt;
			Num_Page = NumToWrite / AT24Cxx_PageSize;
			Num_Remain = NumToWrite % AT24Cxx_PageSize;
			
			if (cnt != 0)
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, cnt, pBuffer, err);
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //等待操作完成
				WriteAdd += cnt;
				pBuffer += cnt;
			}
			while (Num_Page--)  //按照页来写
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, AT24Cxx_PageSize, pBuffer, err);
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //等待操作完成
				WriteAdd += AT24Cxx_PageSize;
				pBuffer += AT24Cxx_PageSize;
			}
			if (Num_Remain != 0)  //不够一个页的数据
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //小于一个页大小数据
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //等待操作完成
			}
		}
	}
}
