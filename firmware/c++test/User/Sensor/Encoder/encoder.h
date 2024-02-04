#ifndef CTRL_STEP_FW_AS5047_M0_STM32_H
#define CTRL_STEP_FW_AS5047_M0_STM32_H

#define AS5600_I2C_HANDLE hi2c1

// #define I2C_TIME_OUT_BASE 10
// #define I2C_TIME_OUT_BYTE 1

#include "userMain.h"

class Encoder::EncoderAS5600Base
{
public:
    Encoder(I2C_HandleTypeDef *_iic)
    {
        iicHandle = _iic;
    }

    I2C_HandleTypeDef *iicHandle;

    const static uint8_t I2C_TIME_OUT_BASE = 10;
    const static uint8_t I2C_TIME_OUT_BYTE = 1;

private:
    virtual void i2cInit();

    virtual int i2cWrite(uint8_t dev_addr, uint8_t *pData, uint32_t count);

    virtual int i2cRead(uint8_t dev_addr, uint8_t *pData, uint32_t count);
};

#endif
