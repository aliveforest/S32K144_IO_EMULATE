/*****************************************************************
* @file    : latency.h 
* @brief   : 
* @author  : Dtb 
* @version : 
* @date    : 2023-8-1
*****************************************************************/

#ifndef LATENCY_H_
#define LATENCY_H_
#include "S32K144.h"

void systick_delay_init(void);
void later_100_ns(volatile uint32_t n_100ns);
void later_1_us(void);
void later_us(volatile uint32_t nus);     //使用float类型，实现更精确的us延时
void later_ms(volatile uint32_t nms);



#endif
