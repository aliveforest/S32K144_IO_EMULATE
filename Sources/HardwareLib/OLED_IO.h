/*
 * OLED_IO.h
 *
 *  Created on: 2023年7月31日
 *      Author: dengtongbei
 */

#ifndef OLED_IO_H_
#define OLED_IO_H_
#include "IIC_IO.h"
#include "S32K144.h"

#define OLED_CMD  0	//0 写命令
#define OLED_DATA 1	//1 写数据

void OLED_SCL_Clr(IIC_type *IICx);
void OLED_SCL_Set(IIC_type *IICx);
void OLED_SDA_Clr(IIC_type *IICx);
void OLED_SDA_Set(IIC_type *IICx);

void OLED_Init(IIC_type *IICx, uint8_t _x, uint32_t SDA_pin, uint32_t SCL_pin, uint8_t dl_time);
void OLED_Clear(IIC_type *IICx);                    // 清屏函数
void OLED_Refresh(IIC_type *IICx);                  // 更新显存到OLED
void OLED_Display_On(IIC_type *IICx);
void OLED_Display_Off(IIC_type *IICx);
void OLED_ColorTurn(IIC_type *IICx, uint8_t i) ;    // 反显函数
void OLED_DisplayTurn(IIC_type *IICx, uint8_t i);   // 屏幕旋转180度

void Send_Byte(IIC_type *IICx, uint8_t data);
void OLED_Write_Byte(IIC_type *IICx, uint8_t data, uint8_t mode);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t); //画点
void OLED_ShowChar(IIC_type *IICx, uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode);
// 常用
void OLED_DrawLine(IIC_type *IICx, uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode);
void OLED_DrawCircle(IIC_type *IICx, uint8_t x,uint8_t y,uint8_t r);
void OLED_ShowString(IIC_type *IICx, uint8_t x,uint8_t y,char *chr,uint8_t size1,uint8_t mode);
void OLED_ShowNum(IIC_type *IICx, uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode);
void OLED_ShowPicture(IIC_type *IICx, uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode);




#endif /* OLED_IO_H_ */
