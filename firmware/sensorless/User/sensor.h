#ifndef __SENSOR_H
#define __SENSOR_H

#include "common.h"

#define Lq 0.00535f // H
#define Rq 6.97f    // 相电阻
float positionEstimate(float uD, float uQ, float iD, float iQ);
#endif
