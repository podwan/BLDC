#include "as5600.h"
#include "i2c.h"
#define abs(x) ((x) > 0 ? (x) : -(x))
#define _2PI 6.28318530718

static float angle_data_prev;      // 上次位置
static float full_rotation_offset; // 转过的整圈数

void as5600Init(void)
{
  /* init i2c interface */

  /* init var */
  full_rotation_offset = 0;
  angle_data_prev = as5600GetRawAngle();
}

static int i2cWrite(uint8_t dev_addr, uint8_t *pData, uint32_t count)
{
  int status;
  int i2c_time_out = I2C_TIME_OUT_BASE + count * I2C_TIME_OUT_BYTE;

  status = HAL_I2C_Master_Transmit(&AS5600_I2C_HANDLE, dev_addr, pData, count, i2c_time_out);
  return status;
}

static int i2cRead(uint8_t dev_addr, uint8_t *pData, uint32_t count)
{
  int status;
  int i2c_time_out = I2C_TIME_OUT_BASE + count * I2C_TIME_OUT_BYTE;

  status = HAL_I2C_Master_Receive(&AS5600_I2C_HANDLE, (dev_addr | 1), pData, count, i2c_time_out);
  return status;
}

uint16_t as5600GetRawAngle(void)
{
  uint16_t raw_angle;
  uint8_t buffer[2] = {0};
  uint8_t raw_angle_register = AS5600_RAW_ANGLE_REGISTER;

  i2cWrite(AS5600_ADDR, &raw_angle_register, 1);
  i2cRead(AS5600_ADDR, buffer, 2);
  raw_angle = ((uint16_t)buffer[0] << 8) | (uint16_t)buffer[1];
  return raw_angle;
}

float as5600GetAngle(void)
{
  float angle_data = as5600GetRawAngle();

  float d_angle = angle_data - angle_data_prev;
  if (abs(d_angle) > (0.8 * AS5600_RESOLUTION))
  {
    full_rotation_offset += (d_angle > 0 ? -_2PI : _2PI);
  }
  angle_data_prev = angle_data;

  return (full_rotation_offset + (angle_data / (float)AS5600_RESOLUTION) * _2PI);
}

float as5600GetAngleWithoutTrack(void)
{
  return as5600GetRawAngle() * 0.08789f * _PI / 180; // 得到弧度制的角度
}


float zero_electric_angle = 0;

void alignSensor()
{
  // int PP = POLE_PAIRS;
  // int DIR = DIRECTION;
  // setTorque(3, _3PI_2);
  // delay(3000);
  // zero_electric_angle = _electricalAngle();
  // setTorque(0, _3PI_2);
  // Serial.print("0电角度：");
  // Serial.println(zero_electric_angle);
}

float getElectricAngle()
{
  return normalizeAngle((float)(DIRECTION * POLE_PAIRS) * as5600GetAngleWithoutTrack() - zero_electric_angle);
}