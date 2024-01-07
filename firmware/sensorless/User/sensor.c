#include "sensor.h"

bool adc_isr_flag = 0;
bool opa_cali_flag = 0;

// void getCurrentOffset(void)
// {
//     uint16_t sum_a = 0, sum_b = 0, sum_c = 0;
//     uint16_t cnt;

//     adc_isr_flag = 0;
//     opa_cali_flag = 0;

//     for (cnt = 0; cnt < 8; cnt++)
//     {
//         while (!adc_isr_flag)
//             ;
//         adc_isr_flag = 0;
//         sum_a +=
//     }
// }
