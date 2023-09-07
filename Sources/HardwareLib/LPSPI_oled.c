/*
 * LPSPI_oled.c
 *
 *  Created on: 2023年9月5日
 *      Author: dengtongbei
 */
#include "LPSPI_oled.h"
#include "LPSPI.h"
#include "OLED_FONT.h"
#include "latency.h"
#include "GPIO_init.h"

uint8_t LPSPI_GRAM[144][8];

/* DC  数据/命令选择脚*/
void LPSPI_DC_Clr(void) { PTE->PCOR |= (1 << 2);     /*E2*/}
void LPSPI_DC_Set(void) { PTE->PSOR |= (1 << 2); }
/* RES  复位（OLED上电后需要进行一次复位） */
void LPSPI_RST_Clr(void) { PTE->PCOR |= (1 << 6);  /*E6*/}
void LPSPI_RST_Set(void) { PTE->PSOR |= (1 << 6); }

/* OLED的初始化 */
void LPSPI_OLED_Init(void){
    /* Pin number        | Function
	 * ----------------- |---------------
	 * PTB14             | LPSPI1_SCK  时钟线    D0  使用SPI的时钟线
	 * PTB15             | LPSPI1_SIN  数据输入
	 * PTB16			 | LPSPI1_SOUT 数据输出  D1  使用SPI的数据线 
	 * PTB17			 | LPSPI1_PCS3 片选    */
    LPSPI1_init_master(); /* LPSPI1和对应引脚初始化 */
	GPIO_init('E', 2, 1);
	GPIO_init('E', 6, 1);
    
    LPSPI_RST_Set(); /* OLED上电后需要进行一次复位 */
    later_ms(100);
    LPSPI_RST_Clr();
    later_ms(200);
    LPSPI_RST_Set(); 
                    
	LPSPI_Write_Byte(0xAE,OLED_CMD);//--turn off oled panel
	LPSPI_Write_Byte(0x00,OLED_CMD);//---set low column address
	LPSPI_Write_Byte(0x10,OLED_CMD);//---set high column address
	LPSPI_Write_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	LPSPI_Write_Byte(0x81,OLED_CMD);//--set contrast control register
	LPSPI_Write_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
	LPSPI_Write_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	LPSPI_Write_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	LPSPI_Write_Byte(0xA6,OLED_CMD);//--set normal display
	LPSPI_Write_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	LPSPI_Write_Byte(0x3f,OLED_CMD);//--1/64 duty
	LPSPI_Write_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	LPSPI_Write_Byte(0x00,OLED_CMD);//-not offset
	LPSPI_Write_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	LPSPI_Write_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	LPSPI_Write_Byte(0xD9,OLED_CMD);//--set pre-charge period
	LPSPI_Write_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	LPSPI_Write_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	LPSPI_Write_Byte(0x12,OLED_CMD);
	LPSPI_Write_Byte(0xDB,OLED_CMD);//--set vcomh
	LPSPI_Write_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	LPSPI_Write_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	LPSPI_Write_Byte(0x02,OLED_CMD);//
	LPSPI_Write_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	LPSPI_Write_Byte(0x14,OLED_CMD);//--set(0x10) disable
	LPSPI_Write_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	LPSPI_Write_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	LPSPI_Write_Byte(0xAF,OLED_CMD);//--turn on oled panel
	
	LPSPI_Write_Byte(0xAF,OLED_CMD); /*display ON*/ 
	LPSPI_Clear();
	LPSPI_Set_Pos(0,0); 	
}
/* LPSPI_OLED 发送1字节函数 */
void LPSPI_Write_Byte(uint8_t data, uint8_t mode) {
    if (mode)
        LPSPI_DC_Set();          /* 高电平代表传输数据（Data） */
    else
        LPSPI_DC_Clr();          /* 低电平代表传输命令（Command）*/
    LPSPI1_transmit_8bits(data); /* LPSPI发送1字节(8位)函数 */
}

/* 设置位置 */
void LPSPI_Set_Pos(uint8_t x, uint8_t y) 
{ 
	LPSPI_Write_Byte(0xb0+y,OLED_CMD);
	LPSPI_Write_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	LPSPI_Write_Byte((x&0x0f)|0x01,OLED_CMD); 
}   
/* 开启OLED显示 */   
void LPSPI_Display_On(void) {
	LPSPI_Write_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	LPSPI_Write_Byte(0X14,OLED_CMD);  //DCDC ON
	LPSPI_Write_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
/* 关闭OLED显示 */    
void LPSPI_Display_Off(void)
{
	LPSPI_Write_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	LPSPI_Write_Byte(0X10,OLED_CMD);  //DCDC OFF
	LPSPI_Write_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
/* 更新显存到OLED */
void LPSPI_Refresh(void) {
	uint8_t i,n;
	for(i=0;i<8;i++) {
        LPSPI_Write_Byte(0xb0+i,OLED_CMD);    //设置页地址（0~7）
		LPSPI_Write_Byte(0x00,OLED_CMD);      //设置显示位置—列低地址 0x00
		LPSPI_Write_Byte(0x00,OLED_CMD);      //设置显示位置—列高地址 0x10
        for(n=0;n<128;n++) { /* 一次性写入 */
            LPSPI_Write_Byte(LPSPI_GRAM[n][i], OLED_DATA);
		}
    }
}
/* 清屏函数 */
void LPSPI_Clear(void) {
	uint8_t i,n;
	for(i=0;i<8;i++) {
	   for(n=0;n<128;n++) {
			LPSPI_GRAM[n][i]=0;//清除所有数据
		}
    }
	LPSPI_Refresh();//更新显示
}
/* 画点函数  x:0~127   y:0~63   t:1 填充 0,清空 */
void LPSPI_DrawPoint(uint8_t x,uint8_t y,uint8_t t) {
    uint8_t i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    if(t) { LPSPI_GRAM[x][i] |= n;}
	else {
        LPSPI_GRAM[x][i] = ~LPSPI_GRAM[x][i];
        LPSPI_GRAM[x][i] |= n;
        LPSPI_GRAM[x][i] = ~LPSPI_GRAM[x][i];
    }
}
//在指定位置显示一个字符,包括部分字符 //x:0~127 //y:0~63
//size1:选择字体 6x8/6x12/8x16/12x24
//mode:0,反色显示;1,正常显示
void LPSPI_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode)
{
	uint8_t i,m,temp,size2,chr1;
	uint8_t x0=x,y0=y;
	if(size1==8) size2=6;
	else size2=(size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
	chr1=chr-' ';  //计算偏移后的值
	for(i=0;i<size2;i++) {
		if(size1==8)
		{temp=ASICII_0806[chr1][i];} //调用0806字体
		else if(size1==12)
        {temp=ASICII_1206[chr1][i];} //调用1206字体
		else if(size1==16)
        {temp=ASICII_1608[chr1][i];} //调用1608字体
		else if(size1==24)
        {temp=ASICII_2412[chr1][i];} //调用2412字体
		else return;
		for(m=0;m<8;m++) {
			if(temp&0x01) LPSPI_DrawPoint(x, y, mode);
			else LPSPI_DrawPoint(x, y, !mode);
			temp>>=1;
			y++;
		}
		x++;
		if((size1!=8)&&((x-x0)==size1/2))
		{x=x0;y0=y0+8;}
		y=y0;
    }
}

//显示字符串
//x,y:起点坐标  //size1:字体大小  //*chr:字符串起始地址
//mode:0,反色显示;1,正常显示
void LPSPI_ShowString(uint8_t x,uint8_t y,char *chr,uint8_t size1,uint8_t mode)
{
	while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
	{
		LPSPI_ShowChar(x,y,*chr,size1,mode);
		if(size1==8)x+=6;
		else x+=size1/2;
		chr++;
    }
    LPSPI_Refresh();
}

//m^n
uint32_t LPSPI_Pow(uint8_t m,uint8_t n){
	uint32_t result=1;
	while(n--){
	  result*=m;
	}
	return result;
}


//显示数字
//x,y :起点坐标 //num :要显示的数字 //len :数字的位数
//size:字体大小 8 / 12 / 16 / 24
//mode:0,反色显示;1,正常显示
void LPSPI_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode)
{
    uint8_t t, temp, m = 0;
    if (size1 == 8)
        m = 2;
    for (t = 0; t < len; t++) {
        temp = (num / LPSPI_Pow(10, len - t - 1)) % 10;
        if (temp == 0) {
            LPSPI_ShowChar(x + (size1 / 2 + m) * t, y, '0', size1, mode);
        }else {
            LPSPI_ShowChar(x + (size1 / 2 + m) * t, y, temp + '0', size1, mode);
        }
    }
    LPSPI_Refresh();
}
// 显示图片 //x,y：起点坐标 //sizex,sizey,图片长宽
//BMP[]：要写入的图片数组 //mode:0,反色显示;1,正常显示
void LPSPI_ShowPicture(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode)
{
    uint16_t j = 0;
    uint8_t i, n, temp, m;
    uint8_t x0 = x, y0 = y;
    sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
    for (n = 0; n < sizey; n++) {
        for (i = 0; i < sizex; i++) {
            temp = BMP[j];
            j++;
            for (m = 0; m < 8; m++) {
                if (temp & 0x01)
                    LPSPI_DrawPoint(x, y, mode);
                else
                    LPSPI_DrawPoint(x, y, !mode);
                temp >>= 1;
                y++;
            }
            x++;
            if ((x - x0) == sizex) {
                x = x0;
                y0 = y0 + 8;
            }
            y = y0;
        }
    }
}

