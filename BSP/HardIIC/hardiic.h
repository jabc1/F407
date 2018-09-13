/**
  *****************************************************************************
  *                             STM32Ӳ��IIC���ƺ���
  *
  *                       (C) Copyright 2000-2020, ***
  *                             All Rights Reserved
  *****************************************************************************
  *
  * @File    : hardiic.h
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

#ifndef _hardiic_h
#define _hardiic_h

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
                               �ⲿ����ͷ�ļ�
******************************************************************************/

#include "sys.h"


/******************************************************************************
                       ���ݲ�ͬ�Ĵ洢��ҳ��С���ж���
******************************************************************************/

#define AT24Cxx_PageSize                         8  //�洢�����С

/******************************************************************************
                               �ⲿ���ù��ܺ���
******************************************************************************/

void Hard_IIC_Init                               (I2C_TypeDef* I2Cx, u8 SlaveAdd, u8 F103IIC1_Remap/* ���F103��IIC1��ӳ��� */);  //Ӳ��IIC��ʼ��
void Hard_IICWriteOneByte                        (I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 WriteAdd, u8 Data, u8 * err);  //Ӳ��IIC����һ���ֽ�����
u8   Hard_IIC_ReadOneByte                        (I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 ReadAdd, u8 * err);  //Ӳ��IIC��ȡһ���ֽ�����

void Hard_IICWaiteStandby                        (I2C_TypeDef* IICx, uint8_t SlaveAdd);  //Ӳ��IIC�ȴ����豸�ڲ��������

void Hard_IIC_WriteNByte                         (I2C_TypeDef * IICx, u8 SlaveAdd, u8 WriteAdd, u8 NumToWrite, u8 * pBuffer, u8 * err);  //Ӳ��IIC���Ͷ���ֽ�����
void Hard_IIC_PageRead                           (I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 ReadAdd, u8 NumToRead, u8 * pBuffer, u8 * err);  //Ӳ��IIC��ȡ����ֽ�����
void Hard_IIC_PageWrite                          (I2C_TypeDef * IICx, u8 SlaveAdd, u8 WriteAdd, u8 NumToWrite, u8 * pBuffer, u8 * err);  //AT24C02ҳд����


/**
  ****************************** Support C++ **********************************
**/
#ifdef __cplusplus
	}
#endif
/**
  *****************************************************************************
**/


#endif  /* end hardiic.h */
