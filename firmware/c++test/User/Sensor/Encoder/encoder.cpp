#include "encoder.h"
#include "i2c.h"

void Encoder::i2cInit()
{
}

int Encoder::i2cWrite(uint8_t dev_addr, uint8_t *pData, uint32_t count)
{
     int status;
  int i2c_time_out = I2C_TIME_OUT_BASE + count * I2C_TIME_OUT_BYTE;

  status = HAL_I2C_Master_Transmit(&AS5600_I2C_HANDLE, dev_addr, pData, count, i2c_time_out);
  return status;
}

int Encoder::i2cRead(uint8_t dev_addr, uint8_t *pData, uint32_t count)
{
      int status;
  int i2c_time_out = I2C_TIME_OUT_BASE + count * I2C_TIME_OUT_BYTE;

  status = HAL_I2C_Master_Receive(&AS5600_I2C_HANDLE, (dev_addr | 1), pData, count, i2c_time_out);
  return status;
}



