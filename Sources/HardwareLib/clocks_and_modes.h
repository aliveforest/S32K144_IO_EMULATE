/*
 * Copyright (c) 2014 - 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2016 - 2018, NXP.
 * All rights reserved.

 */

#ifndef CLOCKS_AND_MODES_H_
#define CLOCKS_AND_MODES_H_

void Clock_Init(void);
void SOSC_init_8MHz (void);
void SPLL_init_160MHz (void);
void NormalRUNmode_80MHz (void);

#endif /* CLOCKS_AND_MODES_H_ */

