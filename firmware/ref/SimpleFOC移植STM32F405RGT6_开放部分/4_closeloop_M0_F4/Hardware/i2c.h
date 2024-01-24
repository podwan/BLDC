
#ifndef STM32_I2C_H
#define STM32_I2C_H

/******************************************************************************/
#include "stm32f4xx.h"

/******************************************************************************/
void I2C_Init_(void);

void IIC0_Start(void);
void IIC0_Stop(void);
unsigned char IIC0_Wait_Ack(void);
void IIC0_Ack(void);
void IIC0_NAck(void);
void IIC0_Send_Byte(unsigned char txd);
unsigned char IIC0_Read_Byte(unsigned char ack);
/******************************************************************************/


#endif
