/*
 * RGB_LED.h
 *
 *  Created on: 2023年8月1日
 *      Author: dengtongbei
 */

#ifndef RGB_LED_H_
#define RGB_LED_H_
#include "stdbool.h"

void RGB_LED_KEY_init(void);
void BLUE_toggle(void);
void GREEN_toggle(void);
void RED_toggle(void);
bool SW2_key(void);

#endif /* RGB_LED_H_ */
