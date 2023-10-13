/*****************************************************************
* @file    : OLED_IO.c 
* @brief   : 
* @author  : Dtb 
* @version : v1.0
* @date    : 2023-7-31
*****************************************************************/

#include "latency.h"
#include "OLED_IO.h"
#include "OLED_FONT.h"
#include "S32K144.h"
#include "IIC_IO.h"
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   


uint8_t OLED_GRAM[144][8];


// SCL输出低电平
void OLED_SCL_Clr(IIC_type *IICx) {
    IICx->PTx->PCOR |= (1UL << (IICx->SCL_pin)); 
}

// SCL输出高电平
void OLED_SCL_Set(IIC_type *IICx) {
    IICx->PTx->PSOR |= (1UL << (IICx->SCL_pin)); 
}

// SDA输出低电平
void OLED_SDA_Clr(IIC_type *IICx) {
    IICx->PTx->PCOR |= (1UL << (IICx->SDA_pin)); 
}

// SDA输出高电平
void OLED_SDA_Set(IIC_type *IICx) {
    IICx->PTx->PSOR |= (1UL << (IICx->SDA_pin)); 
}

//OLED的初始化
void OLED_Init(IIC_type *IICx, uint8_t _x, uint32_t SDA_pin, uint32_t SCL_pin, uint8_t dl_time){
	IO_IIC_init(IICx, _x, SDA_pin, SCL_pin, dl_time); //IIC引脚初始化
    later_ms(500);
    OLED_Write_Byte(IICx, 0xAE, OLED_CMD); //--turn off oled panel
    OLED_Write_Byte(IICx, 0x00, OLED_CMD); //---set low column address
    OLED_Write_Byte(IICx, 0x10, OLED_CMD); //---set high column address
    OLED_Write_Byte(IICx, 0x40, OLED_CMD); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_Write_Byte(IICx, 0x81, OLED_CMD); //-设置对比度控制寄存器--set contrast control register
    OLED_Write_Byte(IICx, 0xCF, OLED_CMD); // 设置SEG输出电流亮度Set SEG Output Current Brightness
    OLED_Write_Byte(IICx, 0xA1, OLED_CMD); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_Write_Byte(IICx, 0xC8, OLED_CMD); // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_Write_Byte(IICx, 0xA6, OLED_CMD); //--set normal display
    OLED_Write_Byte(IICx, 0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
    OLED_Write_Byte(IICx, 0x3f, OLED_CMD); //--1/64 duty
    OLED_Write_Byte(IICx, 0xD3, OLED_CMD); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_Write_Byte(IICx, 0x00, OLED_CMD); //-not offset
    OLED_Write_Byte(IICx, 0xd5, OLED_CMD); // 设置时钟分频因子,震荡频率//--set display clock divide ratio/oscillator frequency
    OLED_Write_Byte(IICx, 0xf0, OLED_CMD); //[3:0],分频因子;[7:4],震荡频率//--set divide ratio, Set Clock as     Frames/Sec
    OLED_Write_Byte(IICx, 0xD9, OLED_CMD); //--set pre-charge period
    OLED_Write_Byte(IICx, 0xF1, OLED_CMD); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_Write_Byte(IICx, 0xDA, OLED_CMD); //--set com pins hardware configuration
    OLED_Write_Byte(IICx, 0x12, OLED_CMD);
    OLED_Write_Byte(IICx, 0xDB, OLED_CMD); //--set vcomh
    OLED_Write_Byte(IICx, 0x30, OLED_CMD); // Set VCOM Deselect Level
    OLED_Write_Byte(IICx, 0x20, OLED_CMD); //-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_Write_Byte(IICx, 0x02, OLED_CMD); //
    OLED_Write_Byte(IICx, 0x8D, OLED_CMD); //--set Charge Pump enable/disable
    OLED_Write_Byte(IICx, 0x14, OLED_CMD); //--set(0x10) disable
    OLED_Clear(IICx);
    OLED_Write_Byte(IICx, 0xAF, OLED_CMD);
    OLED_ColorTurn(IICx, 0);   // 0正常显示  1 反色显示
    // OLED_DisplayTurn(IICx, 1); // 0正常显示  1 屏幕翻转显示
}

//开启OLED显示
void OLED_Display_On(IIC_type *IICx) {
    OLED_Write_Byte(IICx, 0x8D, OLED_CMD); // 电荷泵使能
    OLED_Write_Byte(IICx, 0x14, OLED_CMD); // 开启电荷泵
    OLED_Write_Byte(IICx, 0xAF, OLED_CMD); // 点亮屏幕
}

// 关闭OLED显示
void OLED_Display_Off(IIC_type *IICx) {
    OLED_Write_Byte(IICx, 0x8D, OLED_CMD); // 电荷泵使能
    OLED_Write_Byte(IICx, 0x10, OLED_CMD); // 关闭电荷泵
    OLED_Write_Byte(IICx, 0xAE, OLED_CMD); // 关闭屏幕
}


// 发送一个字节 mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_Write_Byte(IIC_type *IICx, uint8_t data, uint8_t mode) {
    IIC_start(IICx);
    IIC_Send_Byte(IICx, 0x78);
    IIC_wait_Ack(IICx);
    if (mode) {
        IIC_Send_Byte(IICx, 0x40);
    }else{
        IIC_Send_Byte(IICx, 0x00);
    }
    IIC_wait_Ack(IICx);
    IIC_Send_Byte(IICx, data);
    IIC_wait_Ack(IICx);
    IIC_stop(IICx);
}


// 更新显存到OLED
void OLED_Refresh(IIC_type *IICx) {
	uint8_t i,n;
	for(i=0;i<8;i++) {
        OLED_Write_Byte(IICx, 0xb0 + i, OLED_CMD); // 设置行起始地址
        OLED_Write_Byte(IICx, 0x00, OLED_CMD);     // 设置低列起始地址
        OLED_Write_Byte(IICx, 0x10, OLED_CMD);     // 设置高列起始地址
        IIC_start(IICx);
        IIC_Send_Byte(IICx, 0x78);
        IIC_wait_Ack(IICx);
        IIC_Send_Byte(IICx, 0x40);
        IIC_wait_Ack(IICx);
        for(n=0;n<128;n++) {
			IIC_Send_Byte(IICx, OLED_GRAM[n][i]);
			IIC_wait_Ack(IICx);
		}
		IIC_stop(IICx);
  }
}

//清屏函数
void OLED_Clear(IIC_type *IICx) {
	uint8_t i,n;
	for(i=0;i<8;i++) {
	   for(n=0;n<128;n++) {
			OLED_GRAM[n][i]=0;//清除所有数据
		}
    }
	OLED_Refresh(IICx);//更新显示
}

// 反显函数
void OLED_ColorTurn(IIC_type *IICx, uint8_t i) {
    if (i == 0) {
        OLED_Write_Byte(IICx, 0xA6, OLED_CMD); // 正常显示
    }
    if (i == 1) {
        OLED_Write_Byte(IICx, 0xA7, OLED_CMD); // 反色显示
    }
}

// 屏幕旋转180度函数
void OLED_DisplayTurn(IIC_type *IICx, uint8_t i) {
    if (i == 0) {
        OLED_Write_Byte(IICx, 0xC8, OLED_CMD); // 正常显示
        OLED_Write_Byte(IICx, 0xA1, OLED_CMD);
    }
    if (i == 1) {
        OLED_Write_Byte(IICx, 0xC0, OLED_CMD); // 反转显示
        OLED_Write_Byte(IICx, 0xA0, OLED_CMD);
    }
    OLED_Refresh(IICx);
}

// 画点函数  x:0~127   y:0~63   t:1 填充 0,清空
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t) {
    uint8_t i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    if(t) { OLED_GRAM[x][i] |= n;}
	else {
        OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
        OLED_GRAM[x][i] |= n;
        OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
    }
}

//画线  //x1,y1:起点坐标  //x2,y2:结束坐标
void OLED_DrawLine(IIC_type *IICx, uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode)
{
	uint16_t t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向
	else if (delta_x==0)incx=0;//垂直线
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
	else distance=delta_y;
	for(t=0;t<distance+1;t++) {
		OLED_DrawPoint(uRow,uCol,mode);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance) {
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance) {
			yerr-=distance;
			uCol+=incy;
		}
	}
    OLED_Refresh(IICx);
}

//x,y:圆心坐标 //r:圆的半径
void OLED_DrawCircle(IIC_type *IICx, uint8_t x,uint8_t y,uint8_t r) {
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r) {
        OLED_DrawPoint(x + a, y - b,1);
        OLED_DrawPoint(x - a, y - b,1);
        OLED_DrawPoint(x - a, y + b,1);
        OLED_DrawPoint(x + a, y + b,1);

        OLED_DrawPoint(x + b, y + a,1);
        OLED_DrawPoint(x + b, y - a,1);
        OLED_DrawPoint(x - b, y - a,1);
        OLED_DrawPoint(x - b, y + a,1);
        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0) {
            b--;
            a--;
        }
    }
    OLED_Refresh(IICx);
}

//在指定位置显示一个字符,包括部分字符 //x:0~127 //y:0~63
//size1:选择字体 6x8/6x12/8x16/12x24
//mode:0,反色显示;1,正常显示
void OLED_ShowChar(IIC_type *IICx, uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode)
{
	uint8_t i,m,temp,size2,chr1;
	uint8_t x0=x,y0=y;
	if(size1==8)size2=6;
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
			if(temp&0x01) OLED_DrawPoint(x, y, mode);
			else OLED_DrawPoint(x, y, !mode);
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
void OLED_ShowString(IIC_type *IICx, uint8_t x,uint8_t y,char *chr,uint8_t size1,uint8_t mode)
{
	while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
	{
		OLED_ShowChar(IICx, x,y,*chr,size1,mode);
		if(size1==8)x+=6;
		else x+=size1/2;
		chr++;
    }
    OLED_Refresh(IICx);
}

//m^n
uint32_t OLED_Pow(uint8_t m,uint8_t n){
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
void OLED_ShowNum(IIC_type *IICx, uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode)
{
    uint8_t t, temp, m = 0;
    if (size1 == 8)
        m = 2;
    for (t = 0; t < len; t++) {
        temp = (num / OLED_Pow(10, len - t - 1)) % 10;
        if (temp == 0) {
            OLED_ShowChar(IICx, x + (size1 / 2 + m) * t, y, '0', size1, mode);
        }else {
            OLED_ShowChar(IICx, x + (size1 / 2 + m) * t, y, temp + '0', size1, mode);
        }
    }
    OLED_Refresh(IICx);
}
// 显示图片 //x,y：起点坐标 //sizex,sizey,图片长宽
//BMP[]：要写入的图片数组 //mode:0,反色显示;1,正常显示
void OLED_ShowPicture(IIC_type *IICx, uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode)
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
                    OLED_DrawPoint(x, y, mode);
                else
                    OLED_DrawPoint(x, y, !mode);
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


