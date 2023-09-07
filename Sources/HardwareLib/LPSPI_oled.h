/*
 * LPSPI_oled.h
 *
 *  Created on: 2023年9月5日
 *      Author: dengtongbei
 */

#ifndef LPSPI_OLED_H_
#define LPSPI_OLED_H_

#define SIZE 16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	 

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


#include "S32K144.h"


void LPSPI_OLED_Init(void);
void LPSPI_Clear(void);                    // 清屏函数
void LPSPI_Refresh(void);
void LPSPI_Set_Pos(uint8_t x, uint8_t y); 
void LPSPI_Display_On(void);
void LPSPI_Display_Off(void);

void LPSPI_Write_Byte(uint8_t data, uint8_t mode);
void LPSPI_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void LPSPI_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode);
void LPSPI_ShowString(uint8_t x,uint8_t y,char *chr,uint8_t size1,uint8_t mode);
void LPSPI_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode);
void LPSPI_ShowPicture(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode);


#endif /* HARDWARELIB_LPLPSPI_H_ */
