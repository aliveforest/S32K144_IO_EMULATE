/*
 * Copyright (c) 2014 - 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2016 - 2018, NXP.
 * All rights reserved.
 */

#ifndef LPUART_H_
#define LPUART_H_

extern char receivebuff[200];

void LPUART1_init(void);
void LPUART1_transmit_char(char send);
void LPUART1_transmit_string(char *data_string);
void LPUART1_printf(char *fmt, ...);
void LPUART1_RX_IRQHandler(void);
void LPUART1_NVIC_init_IRQs (void);
#endif /* LPUART_H_ */
