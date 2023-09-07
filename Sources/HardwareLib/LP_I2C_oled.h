/*
 * LP_I2C_oled.h
 *
 *  Created on: 2023年8月14日
 *      Author: dengtongbei
 */

#ifndef LP_I2C_OLED_H_
#define LP_I2C_OLED_H_
#include "S32K144.h"


#define lp_oled_CMD  0	//0 写命令
#define lp_oled_DATA 1	//1 写数据


void lp_oled_Init(void);
void lp_oled_Clear(void);                    // 清屏函数
void lp_oled_Refresh(void);                  // 更新显存到OLED
void lp_oled_Display_On(void);
void lp_oled_Display_Off(void);
void lp_oled_ColorTurn(uint8_t i) ;    // 反显函数
void lp_oled_DisplayTurn(uint8_t i);   // 屏幕旋转180度

void LPI2C_init(void);
void LPI2C_Transmit(uint8_t cmd, uint8_t data);
void lp_oled_Write_Byte(uint8_t data, uint8_t mode);

void lp_oled_DrawPoint(uint8_t x,uint8_t y,uint8_t t); //画点
void lp_oled_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode);
// 常用
void lp_oled_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode);
void lp_oled_DrawCircle(uint8_t x,uint8_t y,uint8_t r);
void lp_oled_ShowString(uint8_t x,uint8_t y,char *chr,uint8_t size1,uint8_t mode);
void lp_oled_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode);
void lp_oled_ShowPicture(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode);


#endif /* LP_lp_oled_H_ */
