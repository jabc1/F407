/**
  *****************************************************************************
  *                             STM32Ӳ��IIC���ƺ���
  *
  *                       (C) Copyright 2000-2020, ***
  *                             All Rights Reserved
  *****************************************************************************
  *
  * @File    : hardiic.c
  * @By      : �¹�
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
  * @Name   : Ӳ��IIC��ʼ��
  *
  * @Brief  : none
  *
  * @Input  : I2Cx:           IIC��
  *           SlaveAdd:       ��Ϊ���豸ʱʶ���ַ
  *           F103IIC1_Remap: ���F103��IIC1�Ƿ���ӳ��
  *                           0: ����ӳ��
  *                           1: ��ӳ�䡣PB.06 -> PB.08��PB.07 -> PB.09
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void Hard_IIC_Init(I2C_TypeDef* IICx, u8 SlaveAdd, u8 F103IIC1_Remap/* ���F103��IIC1��ӳ��� */)
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
		//��ʼ���ܽ�
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
		//��ʼ���ܽ�
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
	//�ܽŸ���
	//
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
	
#endif
	
	//
	//����IIC����
	//
	I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1         = SlaveAdd;  //���豸��ַ
	I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed          = (100 * 1000);  //SCL���100KHz
	
	
	I2C_Cmd(IICx, ENABLE);
	I2C_Init(IICx, &I2C_InitStructure);
	I2C_AcknowledgeConfig(IICx, ENABLE); 
	
	//
	//��ֹʱ���ӳ�
	//
	IICx->CR1 |= 1<<7;
}

/**
  *****************************************************************************
  * @Name   : Ӳ��IIC�ȴ����豸�ڲ��������
  *
  * @Brief  : none
  *
  * @Input  : I2Cx:     IIC��
  *           SlaveAdd: ��Ϊ���豸ʱʶ���ַ
  *           ReadAdd:  ��ȡ��EEPROM�ڴ��ַ
  *
  * @Output : *err:     ���صĴ���ֵ
  *
  * @Return : ��ȡ��������
  *****************************************************************************
**/
void Hard_IICWaiteStandby(I2C_TypeDef* IICx, uint8_t SlaveAdd)
{
	u16 tmp = 0;
	
	IICx->SR1 &= 0x0000;  //���״̬�Ĵ���1
	
	do
	{
		I2C_GenerateSTART(IICx, ENABLE);  //������ʼ�ź�
		tmp = IICx->SR1;  //��ȡSR1�Ĵ�����Ȼ��д�����ݼĴ������������SBλ��EV5
		I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //���ʹ��豸��ַ
	} while ((IICx->SR1 & 0x0002) == 0x0000);  //��ADDR = 1ʱ������Ӧ���ˣ�����ѭ��
	I2C_ClearFlag(IICx, I2C_FLAG_AF);  //���Ӧ��ʧ�ܱ�־λ
	I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
}

/**
  *****************************************************************************
  * @Name   : Ӳ��IIC����һ���ֽ�����
  *
  * @Brief  : none
  *
  * @Input  : I2Cx:     IIC��
  *           SlaveAdd: ��Ϊ���豸ʱʶ���ַ
  *           WriteAdd: д��EEPROM�ڴ��ַ
  *           Data:     д�������
  *
  * @Output : *err:     ���صĴ���ֵ
  *
  * @Return : none
  *****************************************************************************
**/
void Hard_IICWriteOneByte(I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 WriteAdd, u8 Data, u8 * err)
{
	u16 temp = 0;
	u16 flag = 0;
	
	while (I2C_GetFlagStatus(IICx, I2C_FLAG_BUSY))  //�ȴ�IIC
	{
		temp++;
		if (temp > 800)
		{
			*err |= 1<<0;
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return;
		}
	}
	
	I2C_GenerateSTART(IICx, ENABLE);  //������ʼ�ź�
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return;
		}
	}
	
	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //�����豸��ַ
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return;
		}
	}
	//
	//��ȡSR2״̬�Ĵ���
	//
	flag = IICx->SR2;  //�����ȡSR1�Ĵ�����,��SR2�Ĵ����Ķ����������ADDRλ�������٣�����������������
	
	I2C_SendData(IICx, WriteAdd);  //���ʹ洢��ַ
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return;
		}
	}
	I2C_SendData(IICx, Data);  //��������
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return;
		}
	}
	I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
}

/**
  *****************************************************************************
  * @Name   : Ӳ��IIC��ȡһ���ֽ�����
  *
  * @Brief  : none
  *
  * @Input  : I2Cx:     IIC��
  *           SlaveAdd: ��Ϊ���豸ʱʶ���ַ
  *           ReadAdd:  ��ȡ��EEPROM�ڴ��ַ
  *
  * @Output : *err:     ���صĴ���ֵ
  *
  * @Return : ��ȡ��������
  *****************************************************************************
**/
u8 Hard_IIC_ReadOneByte(I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 ReadAdd, u8 * err)
{
	u16 i = 0;
	u8 temp = 0;
	u16 flag = 0;
	
	while (I2C_GetFlagStatus(IICx, I2C_FLAG_BUSY))  //�ȴ�IIC
	{
		i++;
		if (i > 800)
		{
			*err |= 1<<0;
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return 0;
		}
	}
	I2C_GenerateSTART(IICx, ENABLE);  //������ʼ�ź�
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return 0;
		}
	}
	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //�����豸��ַ
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return 0;
		}
	}
	flag = IICx->SR2;  //�����ȡSR1�Ĵ�����,��SR2�Ĵ����Ķ����������ADDRλ�������٣�����������������
	
	I2C_SendData(IICx, ReadAdd);  //���ʹ洢��ַ
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return 0;
		}
	}
	I2C_GenerateSTART(IICx, ENABLE);  //�����ź�
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return 0;
		}
	}
	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Receiver);  //��ȡ����
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return 0;
		}
	}
	flag = IICx->SR2;
	
	I2C_AcknowledgeConfig(IICx, DISABLE);  //����NACK
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return 0;
		}
	}
	temp = I2C_ReceiveData(IICx);
	I2C_AcknowledgeConfig(IICx, ENABLE);
	
	return temp;
}

/**
  *****************************************************************************
  * @Name   : Ӳ��IIC���Ͷ���ֽ�����
  *
  * @Brief  : none
  *
  * @Input  : I2Cx:       IIC��
  *           SlaveAdd:   ��Ϊ���豸ʱʶ���ַ
  *           WriteAdd:   д��EEPROM�ڴ���ʼ��ַ
  *           NumToWrite: д��������
  *           *pBuffer:   д��������黺��
  *
  * @Output : *err:     ���صĴ���ֵ
  *
  * @Return : none
  *****************************************************************************
**/
void Hard_IIC_WriteNByte(I2C_TypeDef * IICx, u8 SlaveAdd, u8 WriteAdd, u8 NumToWrite, u8 * pBuffer, u8 * err)
{
	u16 sta = 0;
	u16 temp = 0;
	
	while (I2C_GetFlagStatus(IICx, I2C_FLAG_BUSY))  //�ȴ�IIC
	{
		temp++;
		if (temp > 800)
		{
			*err |= 1<<0;
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return;
		}
	}
	I2C_GenerateSTART(IICx, ENABLE);  //������ʼ�ź�
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return;
		}
	}
	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //�����豸��ַ
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return;
		}
	}
	//
	//��ȡSR2״̬�Ĵ���
	//
	sta = IICx->SR2;  //�����ȡSR1�Ĵ�����,��SR2�Ĵ����Ķ����������ADDRλ�������٣�����������������
	I2C_SendData(IICx, WriteAdd);  //���ʹ洢��ַ
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return;
		}
	}
	//
	//ѭ����������
	//
	while (NumToWrite--)
	{
		I2C_SendData(IICx, *pBuffer);  //��������
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
				I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
				return;
			}
		}
	}
	I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
}

/**
  *****************************************************************************
  * @Name   : Ӳ��IIC��ȡ����ֽ�����
  *
  * @Brief  : none
  *
  * @Input  : I2Cx:      IIC��
  *           SlaveAdd:  ��Ϊ���豸ʱʶ���ַ
  *           ReadAdd:   ��ȡ��EEPROM�ڴ���ʼ��ַ
  *           NumToRead: ��ȡ����
  *
  * @Output : *pBuffer: �������������
  *           *err:     ���صĴ���ֵ
  *
  * @Return : ��ȡ��������
  *****************************************************************************
**/
void Hard_IIC_PageRead(I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 ReadAdd, u8 NumToRead, u8 * pBuffer, u8 * err)
{
	u16 i = 0;
	u16 temp = 0;
	u16 sta = 0;
	
	while (I2C_GetFlagStatus(IICx, I2C_FLAG_BUSY))  //�ȴ�IIC
	{
		i++;
		if (i > 800)
		{
			*err |= 1<<0;
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return;
		}
	}
	I2C_GenerateSTART(IICx, ENABLE);  //������ʼ�ź�
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return;
		}
	}
	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //�����豸��ַ
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return;
		}
	}
	sta = IICx->SR2;  //�����ȡSR1�Ĵ�����,��SR2�Ĵ����Ķ����������ADDRλ�������٣�����������������
	
	I2C_SendData(IICx, ReadAdd);  //���ʹ洢��ַ
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return;
		}
	}
	I2C_GenerateSTART(IICx, ENABLE);  //�����ź�
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return;
		}
	}
	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Receiver);  //��ȡ����
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
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
			return;
		}
	}
	sta = IICx->SR2;  //�����ȡSR1�Ĵ�����,��SR2�Ĵ����Ķ����������ADDRλ�������٣�����������������
	//
	//������������
	//
//	while (NumToRead--)
//	{
//		if (NumToRead == 1)  //���һ�������ˣ�������Ӧ���ź�
//		{
//			I2C_AcknowledgeConfig(IICx, DISABLE);  //����NACK
//			I2C_GenerateSTOP(IICx, ENABLE);
//		}
//		//
//		//�ж�RxNE�Ƿ�Ϊ1��EV7
//		//
//		i = 0;
//		while (!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_BYTE_RECEIVED))
//		{
//			i++;
//			if (i > 1000)
//			{
//				*err |= 1<<6;
//				I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
//				return;
//			}
//		}
//		*pBuffer = I2C_ReceiveData(IICx);
//		pBuffer++;
//	}
	while (NumToRead)
	{
		if (NumToRead == 1)  //���һ�������ˣ�������Ӧ���ź�
		{
			I2C_AcknowledgeConfig(IICx, DISABLE);  //����NACK
			I2C_GenerateSTOP(IICx, ENABLE);
		}
		//
		//�ж�RxNE�Ƿ�Ϊ1��EV7
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
  * @Name   : AT24C02ҳд����
  *
  * @Brief  : none
  *
  * @Input  : I2Cx:       IIC��
  *           SlaveAdd:   ��Ϊ���豸ʱʶ���ַ
  *           WriteAdd:   д��EEPROM�ڴ���ʼ��ַ
  *           NumToWrite: д��������
  *           *pBuffer:   д��������黺��
  *
  * @Output : *err:     ���صĴ���ֵ
  *
  * @Return : none
  *****************************************************************************
**/
void Hard_IIC_PageWrite(I2C_TypeDef * IICx, u8 SlaveAdd, u8 WriteAdd, u8 NumToWrite, u8 * pBuffer, u8 * err)
{
	u8 Num_Page = 0;  //����ҳ��С��Ҫд��Ĵ���
	u8 Num_Remain = 0;  //ҳдʣ���ֽ���
	u8 Addr = 0;  //���ݿ��׵�ַΪEEPROMҳ��������
	u8 cnt = 0;  //��������
	
	Addr = WriteAdd % AT24Cxx_PageSize;  //�������ݿ��׵�ַ�Ƿ���ҳ��С��������
	cnt = AT24Cxx_PageSize - Addr;
	Num_Page = NumToWrite / AT24Cxx_PageSize;  //�õ�����
	Num_Remain = NumToWrite % AT24Cxx_PageSize;  //ʣ���ֽ���
	//
	//�ж�д������
	//
	if (Addr == 0)  //������
	{
		if (Num_Page == 0)  //���д������ݿ鳤��С��ҳ��С
		{
			Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //С��һ��ҳ��С����
			Hard_IICWaiteStandby(IICx, SlaveAdd);  //�ȴ��������
		}
		else  //����һҳ����
		{
			while (Num_Page--)  //����ҳ��д
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, AT24Cxx_PageSize, pBuffer, err);
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //�ȴ��������
				WriteAdd += AT24Cxx_PageSize;
				pBuffer += AT24Cxx_PageSize;
			}
			if (Num_Remain != 0)  //����һ��ҳ������
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //С��һ��ҳ��С����
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //�ȴ��������
			}
		}
	}
	else  //����������
	{
		if (Num_Page == 0)  //С��һ��ҳ��������
		{
			Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //д��С��һ��ҳ��������
			Hard_IICWaiteStandby(IICx, SlaveAdd);  //�ȴ��������
		}
		else  //����һ��Ҳ������
		{
			//
			//���¼���
			//
			NumToWrite -= cnt;
			Num_Page = NumToWrite / AT24Cxx_PageSize;
			Num_Remain = NumToWrite % AT24Cxx_PageSize;
			
			if (cnt != 0)
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, cnt, pBuffer, err);
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //�ȴ��������
				WriteAdd += cnt;
				pBuffer += cnt;
			}
			while (Num_Page--)  //����ҳ��д
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, AT24Cxx_PageSize, pBuffer, err);
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //�ȴ��������
				WriteAdd += AT24Cxx_PageSize;
				pBuffer += AT24Cxx_PageSize;
			}
			if (Num_Remain != 0)  //����һ��ҳ������
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //С��һ��ҳ��С����
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //�ȴ��������
			}
		}
	}
}
