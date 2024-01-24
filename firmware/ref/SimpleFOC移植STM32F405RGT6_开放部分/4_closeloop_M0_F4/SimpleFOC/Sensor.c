

#include "MyProject.h"


/******************************************************************************/
long  cpr;
long  velocity_calc_timestamp;  //速度计时，用于计算速度
long  angle_data_prev;          //获取角度用
float angle_prev;               //获取速度用
float full_rotation_offset;     //角度累加
/******************************************************************************/
uint16_t getRawCount(void);
/******************************************************************************/
//初始化三种SPI接口的编码器的参数, 初始化I2C接口或者SPI接口
void MagneticSensor_Init(void)
{
#if M0_AS5600
	I2C_Init_();                  //AS5600
	cpr = AS5600_CPR;
	printf("M0_AS5600\r\n");
#elif M0_AS5047P
	SPI3_Init_(SPI_CPOL_Low);    //AS5047P
	cpr = AS5047P_CPR;
	printf("M0_AS5047P\r\n");
#elif M0_TLE5012B
	SPI3_Init_(SPI_CPOL_Low);    //TLE5012B
	cpr = TLE5012B_CPR;
	printf("M0_TLE5012B\r\n");
#elif M0_MA730
  SPI3_Init_(SPI_CPOL_High);   //MA730
	cpr = MA730_CPR;
	printf("M0_MA730\r\n");
#elif M0_MT6701
  SPI3_Init_(SPI_CPOL_Low);    //MT6701
	cpr = MT6701_CPR;
	printf("M0_MT6701\r\n");
#elif M0_ABZ
  TIM3_Encoder_Init();           //ABZ
	//EXTI_Encoder_Init();        //Z信号中断
	cpr = M0ABZ_CPR;
	printf("M0_ABZ\r\n");
#endif
	
	delay_ms(10);
	
	angle_data_prev = getRawCount();
	full_rotation_offset = 0;
	velocity_calc_timestamp=_micros();
	delay_ms(5);
	angle_prev = getAngle();
}
/******************************************************************************/
uint16_t getRawCount(void)  //获取编码器的原始值
{
	uint16_t val;
	
#if M0_AS5600
	val = ReadAS5600()&0x0FFF;
#elif M0_AS5047P 
	val = ReadAS5047P()&0x3FFF;
#elif M0_TLE5012B
	val = ReadTLE5012B()&0x7FFF;
#elif M0_MA730
	val = ReadMA730();    //左对齐，低两位补0
#elif M0_MT6701
	val = ReadMT6701();   //左对齐，低两位补0
#elif M0_ABZ
	val = ReadABZ();
#endif
	
	return val;
}
/******************************************************************************/
float getAngle(void)
{
	long angle_data,d_angle;
	
	angle_data = getRawCount();
	
	// tracking the number of rotations 
	// in order to expand angle range form [0,2PI] to basically infinity
	d_angle = angle_data - angle_data_prev;
	// if overflow happened track it as full rotation
	if(abs(d_angle) > (0.8f*cpr) ) full_rotation_offset += (d_angle > 0) ? -_2PI : _2PI; 
	// save the current angle value for the next steps
	// in order to know if overflow happened
	angle_data_prev = angle_data;
	
	
	if(full_rotation_offset >= ( _2PI*2000)) //转动圈数过多后浮点数精度下降，电流增加并最终堵转，每隔一定圈数归零一次
	{                                        //这个问题针对电机长时间连续一个方向转动
		full_rotation_offset = 0;              //速度模式，高速转动时每次归零会导致电机抖动一次
		angle_prev = angle_prev - _2PI*2000;
	}
	if(full_rotation_offset <= (-_2PI*2000))
	{
		full_rotation_offset =  0;
		angle_prev = angle_prev + _2PI*2000;
	}
	
	
	// return the full angle 
	// (number of full rotations)*2PI + current sensor angle 
	return (full_rotation_offset + ( (float)angle_data / cpr * _2PI));
}
/******************************************************************************/
// Shaft velocity calculation
float getVelocity(void)
{
	long now_us;
	float Ts, angle_now, vel;

	// calculate sample time
	now_us = _micros();
	Ts = (now_us - velocity_calc_timestamp) * 1e-6f;
	// quick fix for strange cases (micros overflow)
	if(Ts <= 0 || Ts > 0.5f) Ts = 1e-3f;

	// current angle
	angle_now = getAngle();
	// velocity calculation
	vel = (angle_now - angle_prev)/Ts;

	// save variables for future pass
	angle_prev = angle_now;
	velocity_calc_timestamp = now_us;
	return vel;
}
/******************************************************************************/



