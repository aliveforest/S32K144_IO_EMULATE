/*
 * LPTMR.h
 *
 *  Created on: 2023年8月8日
 *      Author: dengtongbei
 */

#ifndef LPTMR_H_
#define LPTMR_H_
#include "S32K144.h"

void LPTMR_init(void);
void Enable_Interrupt(uint8_t vector_number);
void LPTMR0_IRQHandler (void);
void TMR_delay_us(volatile uint32_t nus);

#endif /* LPTMR_H_ */
