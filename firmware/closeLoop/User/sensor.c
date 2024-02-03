#include "sensor.h"
#include "as5600.h"
#include "bldcMotor.h"

Direction countDirection;
int polePairs;
float zeroElectricAngleOffset;
float sensor_offset = 0; // 似乎没用
Error_t error = NO_ERROR;
float angleLast = 0;
uint64_t angleTimestamp = 0;
float velocityLast = 0;
uint64_t velocityTimestamp = 0;
int32_t rotationCount = 0;
int32_t rotationCountLast = 0;
/******************************************************************************/
long cpr;
long velocity_calc_timestamp; // 速度计时，用于计算速度
long angle_data_prev;         // 获取角度用
float angle_prev;             // 获取速度用
float full_rotation_offset;   // 角度累加
/******************************************************************************/
uint16_t getRawCount(void) // 获取编码器的原始值
{
    uint16_t val;

#if AS5600
    val = as5600GetRawAngle() & 0x0FFF;
#elif AS5047P
    val = ReadAS5047P() & 0x3FFF;
#elif TLE5012B
    val = ReadTLE5012B() & 0x7FFF;
#elif MA730
    val = ReadMA730(); // 左对齐，低两位补0
#elif MT6701
    val = ReadMT6701(); // 左对齐，低两位补0
#elif ABZ
    val = ReadABZ();
#endif

    return val;
}

float getLapAngle()
{
    return angleLast;
}

float getFullAngle()
{
    return (float)rotationCount * _2PI + angleLast;
}

void varInit()
{
    // Initialize all the internal variables of EncoderBase
    // to ensure a "smooth" startup (without a 'jump' from zero)
    getRawCount();
    delayMicroSeconds(1);

    velocityLast = getRawCount();
    velocityTimestamp = micros();
    HAL_Delay(1);

    getRawCount();
    delayMicroSeconds(1);

    angleLast = getRawCount();
    angleTimestamp = micros();
}
/******************************************************************************/
// 初始化三种SPI接口的编码器的参数, 初始化I2C接口或者SPI接口
void MagneticSensor_Init(float zero_electric_offset, Direction _sensor_direction)
{
#if AS5600
    cpr = AS5600_CPR;
    printf("M0_AS5600\r\n");
#elif M0_AS5047P
    SPI3_Init_(SPI_CPOL_Low); // AS5047P
    cpr = AS5047P_CPR;
    printf("M0_AS5047P\r\n");
#elif M0_TLE5012B
    SPI3_Init_(SPI_CPOL_Low); // TLE5012B
    cpr = TLE5012B_CPR;
    printf("M0_TLE5012B\r\n");
#elif M0_MA730
    SPI3_Init_(SPI_CPOL_High); // MA730
    cpr = MA730_CPR;
    printf("M0_MA730\r\n");
#elif M0_MT6701
    SPI3_Init_(SPI_CPOL_Low); // MT6701
    cpr = MT6701_CPR;
    printf("M0_MT6701\r\n");
#elif M0_ABZ
    TIM3_Encoder_Init(); // ABZ
    // EXTI_Encoder_Init();        //Z信号中断
    cpr = M0ABZ_CPR;
    printf("M0_ABZ\r\n");
#endif

    delay(10);

    angle_data_prev = getRawCount();
    full_rotation_offset = 0;
    velocity_calc_timestamp = micros();
    delay(5);

    if (zero_electric_offset != 0)
    {
        // abosolute zero offset provided - no need to align
        zeroElectricAngleOffset = zero_electric_offset;
        // set the sensor direction - default CW
        countDirection = _sensor_direction;
    }
    alignSensor(); // 检测零点偏移量和极对数

    // estimateAngle update
    angle_prev = getAngle(); // getVelocity(),make sure velocity=0 after power on
    delay(50);
    estimateVelocity = shaftVelocity(); // 必须调用一次，进入主循环后速度为0
    delay(5);
    estimateAngle = shaftAngle(); // shaft angle
    if (controller == Type_angle)
        target = estimateAngle; // 角度模式，以当前的角度为目标角度，进入主循环后电机静止

    angleLast = getRawCount();
    angleTimestamp = micros();
    delay(200);
    varInit();
}
/******************************************************************************/

float getAngle(void)
{
    long angle_data, d_angle;

    angle_data = getRawCount();
    // tracking the number of rotations
    // in order to expand angle range form [0,2PI] to basically infinity
    d_angle = angle_data - angle_data_prev;
    // if overflow happened track it as full rotation
    if (abs(d_angle) > (0.8f * cpr))
        full_rotation_offset += (d_angle > 0) ? -_2PI : _2PI;
    // save the current angle value for the next steps
    // in order to know if overflow happened
    angle_data_prev = angle_data;

    if (full_rotation_offset >= (_2PI * 2000)) // 转动圈数过多后浮点数精度下降，电流增加并最终堵转，每隔一定圈数归零一次
    {                                          // 这个问题针对电机长时间连续一个方向转动
        full_rotation_offset = 0;              // 速度模式，高速转动时每次归零会导致电机抖动一次
        angle_prev = angle_prev - _2PI * 2000;
    }
    if (full_rotation_offset <= (-_2PI * 2000))
    {
        full_rotation_offset = 0;
        angle_prev = angle_prev + _2PI * 2000;
    }
    // return the full angle
    // (number of full rotations)*2PI + current sensor angle
    return (full_rotation_offset + ((float)angle_data / cpr * _2PI));
}

/******************************************************************************/
// Shaft velocity calculation
float getVelocity(void)
{
    long now_us;
    float Ts, angle_now, vel;

    // calculate sample time
    now_us = micros();
    Ts = (now_us - velocity_calc_timestamp) * 1e-6f;
    // quick fix for strange cases (micros overflow)
    if (Ts <= 0 || Ts > 0.5f)
        Ts = 1e-3f;

    // current angle
    angle_now = getAngle();
    // velocity calculation
    vel = (angle_now - angle_prev) / Ts;

    // save variables for future pass
    angle_prev = angle_now;
    velocity_calc_timestamp = now_us;
    return vel;
}
#if 0
/******************************************************************************/
int alignSensor(void)
{
    long i;
    float angle;
    float mid_angle, end_angle;
    float moved;

    printf("MOT: Align sensor.\r\n");

    if (countDirection == UNKNOWN) // 没有设置，需要检测
    {
        // find natural direction
        // move one electrical revolution forward
        for (i = 0; i <= 500; i++)
        {
            angle = _3PI_2 + _2PI * i / 500.0f;
            setPhaseVoltage(voltageUsedForSensorAlign, 0, angle);
            delay(2);
        }
        mid_angle = getAngle();

        for (i = 500; i >= 0; i--)
        {
            angle = _3PI_2 + _2PI * i / 500.0f;
            setPhaseVoltage(voltageUsedForSensorAlign, 0, angle);
            delay(2);
        }
        end_angle = getAngle();
        setPhaseVoltage(0, 0, 0);
        delay(200);

        printf("mid_angle=%.4f\r\n", mid_angle);
        printf("end_angle=%.4f\r\n", end_angle);

        moved = fabs(mid_angle - end_angle);
        if ((mid_angle == end_angle) || (moved < 0.01f)) // 相等或者几乎没有动
        {
            printf("MOT: Failed to notice movement loop222.\r\n");
            M0_Disable; // 电机检测不正常，关闭驱动
            return 0;
        }
        else if (mid_angle < end_angle)
        {
            printf("MOT: countDirection==CCW\r\n");
            countDirection = CCW;
        }
        else
        {
            printf("MOT: countDirection==CW\r\n");
            countDirection = CW;
        }

        printf("MOT: PP check: ");                 // 计算Pole_Pairs
        if (fabs(moved * polePairs - _2PI) > 0.5f) // 0.5 is arbitrary number it can be lower or higher!
        {
            printf("OK - estimated pp:");
            polePairs = _2PI / moved + 0.5f; // 浮点数转整形，四舍五入
            printf("%d\r\n", polePairs);
        }
        else
            printf("fail!\r\n");
    }
    else
        printf("MOT: Skip dir calib.\r\n");

    if (zeroElectricAngleOffset == 0) // 没有设置，需要检测
    {
        setPhaseVoltage(voltageUsedForSensorAlign, 0, _3PI_2); // 计算零点偏移角度
        delay(700);
        zeroElectricAngleOffset = _normalizeAngle(_electricalAngle(countDirection * getAngle(), polePairs));
        delay(20);
        printf("MOT: Zero elec. angle:");
        printf("%.4f\r\n", zeroElectricAngleOffset);
        setPhaseVoltage(0, 0, 0);
        delay(200);
    }
    else
        printf("MOT: Skip offset calib.\r\n");

    return 1;
}
#endif
void encoderUpdate()
{
    float angle = getRawCount();
    angleTimestamp = micros();

    float deltaAngle = angle - angleLast;
    // If overflow happened track it as full rotation
    if (abs(deltaAngle) > (0.8f * _2PI))
        rotationCount += (deltaAngle > 0) ? -1 : 1;

    angleLast = angle;
}
bool alignSensor()
{
    if (countDirection == UNKNOWN)
    {

        HAL_Delay(100);

        // Find natural direction
        for (int i = 0; i <= 500; i++)
        {
            float angle = _3PI_2 + _2PI * (float)i / 500.0f;
            setPhaseVoltage(voltageUsedForSensorAlign, 0, angle);
            HAL_Delay(2);
        }

        encoderUpdate();
        float midAngle = getFullAngle();

        for (int i = 500; i >= 0; i--)
        {
            float angle = _3PI_2 + _2PI * (float)i / 500.0f;
            setPhaseVoltage(voltageUsedForSensorAlign, 0, angle);
            HAL_Delay(2);
        }

        encoderUpdate();
        float endAngle = getFullAngle();

        setPhaseVoltage(0, 0, 0);
        // SetPowerMotor(false);
        HAL_Delay(200);

        // Determine the direction the sensor moved
        if (midAngle == endAngle)
        {
            error = FAILED_TO_NOTICE_MOVEMENT;
            return false;
        }
        else if (midAngle < endAngle)
        {
            countDirection = CCW;
        }
        else
        {
            countDirection = CW;
        }

        // Check pole pair number
        float deltaAngle = fabs(midAngle - endAngle);
        if (fabs(deltaAngle * (float)polePairs - _2PI) > 0.5f)
        {
            // 0.5f is arbitrary number, it can be tuned
            error = POLE_PAIR_MISMATCH;
            return false;
        }
    }

    // Align the electrical phases of the motor and sensor
    if (!ASSERT(zeroElectricAngleOffset))
    {
        // SetPowerMotor(true);
        // Set angle -90(270 = 3PI/2) degrees
        setPhaseVoltage(voltageUsedForSensorAlign, 0, _3PI_2);

        HAL_Delay(1000);
        encoderUpdate();
        zeroElectricAngleOffset = 0; // Clear offset first
        zeroElectricAngleOffset = getElectricalAngle();

        setPhaseVoltage(0, 0, 0);
        // SetPowerMotor(false);
        HAL_Delay(200);
    }

    return true;
}

/******************************************************************************/
// shaft angle calculation
float shaftAngle(void)
{
    // if no sensor linked return previous value ( for open loop )
    // if(!sensor) return estimateAngle;
    return countDirection * getAngle() - sensor_offset;
}
// shaft velocity calculation
float shaftVelocity(void)
{
    // if no sensor linked return previous value ( for open loop )
    // if(!sensor) return estimateVelocity;
    return countDirection * lowPassFiltering(&LPF_velocity, getVelocity());
    // return countDirection*getVelocity();
}
/******************************************************************************/
float electricalAngle(void)
{
    return _normalizeAngle((estimateAngle + sensor_offset) * polePairs - zeroElectricAngleOffset);
}

float getEstimateVelocity()
{
    // If no sensor linked return previous value (for open-loop case)
    float rawVelocity, estVelocity;
    rawVelocity = (float)countDirection * getVelocity();
    estVelocity = lowPassFiltering(&LPF_velocity, rawVelocity);

    return estVelocity;
}
float getElectricalAngle()
{
    // If no sensor linked return previous value (for open-loop case)
    return _normalizeAngle((float)(countDirection * polePairs) * getLapAngle() - zeroElectricAngleOffset);
}

float getEstimateAngle()
{
    // If no sensor linked return previous value (for open-loop case)
    // if (!encoder)
    //     return estimateAngle;
    float rawAngle, estAngle;
    rawAngle = (float)(countDirection)*getFullAngle();
    estAngle = lowPassFiltering(&lpfAngle, rawAngle);

    return estAngle;
}