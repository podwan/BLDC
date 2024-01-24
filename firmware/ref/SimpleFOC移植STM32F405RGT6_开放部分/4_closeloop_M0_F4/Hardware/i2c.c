
#include "i2c.h"

/***************************************************************************/
#define SDA0_IN()  {GPIOB->MODER&=~(3<<(5*2));GPIOB->MODER|=0<<(5*2);}
#define SDA0_OUT() {GPIOB->MODER&=~(3<<(5*2));GPIOB->MODER|=1<<(5*2);}
#define READ_SDA0  (GPIOB->IDR&(1<<5))

#define IIC0_SCL_1  GPIO_SetBits(GPIOB,GPIO_Pin_4)
#define IIC0_SCL_0  GPIO_ResetBits(GPIOB,GPIO_Pin_4)
#define IIC0_SDA_1  GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define IIC0_SDA_0  GPIO_ResetBits(GPIOB,GPIO_Pin_5)
/***************************************************************************/
#define SDA1_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<(7*2);}
#define SDA1_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<(7*2);}
#define READ_SDA1  (GPIOB->IDR&(1<<7))

#define IIC1_SCL_1  GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define IIC1_SCL_0  GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define IIC1_SDA_1  GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define IIC1_SDA_0  GPIO_ResetBits(GPIOB,GPIO_Pin_7)
/*****************************************************************************/
extern  void delay_s(uint32_t i);
/*****************************************************************************/
void I2C_Init_(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_100MHz; 
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP; 
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_5);  //SCL和SDA输出高电平
}
/******************************************************************************/
void IIC0_Start(void)
{
	IIC0_SDA_1;
	IIC0_SCL_1;
	delay_s(16);    //延时4时读不出角度，延时8可以读出
	IIC0_SDA_0;
	delay_s(16);
	IIC0_SCL_0;
}
/***************************************************************************/
void IIC0_Stop(void)
{
	IIC0_SCL_0;
	IIC0_SDA_0;
	delay_s(16);
	IIC0_SCL_1;
	IIC0_SDA_1;
	delay_s(16);
}
/***************************************************************************/
//1-fail,0-success
unsigned char IIC0_Wait_Ack(void)
{
	unsigned char ucErrTime=0;
	
	SDA0_IN();
	IIC0_SDA_1;
	IIC0_SCL_1;
	delay_s(8);
	while(READ_SDA0!=0)
	{
		if(++ucErrTime>250)
			{
				SDA0_OUT();
				IIC0_Stop();
				return 1;
			}
	}
	SDA0_OUT();
	IIC0_SCL_0;
	return 0; 
}
/***************************************************************************/
void IIC0_Ack(void)
{
	IIC0_SCL_0;
	IIC0_SDA_0;
	delay_s(16);
	IIC0_SCL_1;
	delay_s(16);
	IIC0_SCL_0;
}
/***************************************************************************/
void IIC0_NAck(void)
{
	IIC0_SCL_0;
	IIC0_SDA_1;
	delay_s(16);
	IIC0_SCL_1;
	delay_s(16);
	IIC0_SCL_0;
}
/***************************************************************************/
void IIC0_Send_Byte(unsigned char txd)
{
	unsigned long i;
	
	IIC0_SCL_0;
	for(i=0;i<8;i++)
	{
		if((txd&0x80)!=0)IIC0_SDA_1;
		else
			IIC0_SDA_0;
		txd<<=1;
		delay_s(16);
		IIC0_SCL_1;
		delay_s(16);
		IIC0_SCL_0;
		delay_s(16);
	}
}
/***************************************************************************/
unsigned char IIC0_Read_Byte(unsigned char ack)
{
	unsigned char i,rcv=0;
	
	SDA0_IN();
	for(i=0;i<8;i++)
	{
		IIC0_SCL_0; 
		delay_s(16);
		IIC0_SCL_1;
		rcv<<=1;
		if(READ_SDA0!=0)rcv++;
		delay_s(8);
	}
	SDA0_OUT();
	if(!ack)IIC0_NAck();
	else
		IIC0_Ack();
	return rcv;
}
/***************************************************************************/

