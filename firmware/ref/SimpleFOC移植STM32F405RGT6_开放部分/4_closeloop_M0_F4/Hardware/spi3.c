
#include "spi3.h"

/*****************************************************************************/
void SPI3_Init_(uint16_t spi_cpol)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;  //PA0/GPIO1--CS0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_0);     //CS0_H
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;//PC10~12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SPI3); //PC10 复用为 SPI3
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SPI3); //PC11 复用为 SPI3
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SPI3); //PC12 复用为 SPI3
	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,ENABLE);    //复位 SPI3
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,DISABLE);   //停止复位 SPI3
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//SPI3--双线全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;   //16位数据
	SPI_InitStructure.SPI_CPOL = spi_cpol;               //CPOL=0   SPI_CPOL_High  SPI_CPOL_Low
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;         //CPHA=1
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;            //NSS 信号由硬件管理
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;  //8--5.25MHz   APB1域频率42MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;   //CRC 值计算的多项式
	SPI_Init(SPI3, &SPI_InitStructure);
	SPI_Cmd(SPI3, ENABLE);
}
/*****************************************************************************/
uint16_t SPIx_ReadWriteByte(uint16_t byte)
{
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI3, byte);
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI3);
}
/******************************************************************************/

