/*****************************************************************
* @file    : LPTMR.h 
* @brief   : 
* @author  : aliveforest@163.com 
* @version : v1.0
* @date    : 2023-8-8
*****************************************************************/
#ifndef LPTMR_H_
#define LPTMR_H_
#include "S32K144.h"
#include "device_registers.h"

void LPTMR_init(void);
void Enable_Interrupt (uint32_t vector_number, uint32_t priority) ;
void LPTMR0_IRQHandler (void);
void TMR_delay_us(volatile uint32_t nus);

#endif /* LPTMR_H_ */

