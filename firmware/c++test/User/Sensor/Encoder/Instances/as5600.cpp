#include "as5600.h"
#include "i2c.h"

// void EncoderAS5600Base::Init()
// {
//   i2cInit();
//   // VarInit();
// }

float EncoderAS5600Base::GetRawAngle(void)
{
  uint16_t raw_angle;
  uint8_t buffer[2] = {0};
  uint8_t raw_angle_register = AS5600_RAW_ANGLE_REGISTER;

  i2cWrite(AS5600_ADDR, &raw_angle_register, 1);
  i2cRead(AS5600_ADDR, buffer, 2);
  raw_angle = ((uint16_t)buffer[0] << 8) | (uint16_t)buffer[1];
  return raw_angle;
}
