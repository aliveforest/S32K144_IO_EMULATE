/*
 * LP_I2C_oled.c
 *
 *  Created on: 2023年8月14日
 *      Author: dengtongbei
 */

#include "LP_I2C_oled.h"
#include "latency.h"
#include "OLED_FONT.h"

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

uint8_t lp_oled_GRAM[144][8];

void LPI2C_init(void)
{
    /* Pins definitions
     * ===================================================
     * Pin number        | Function
     * ----------------- |------------------
     * PTA2              | LPI2C_SDA
     * PTA3              | LPI2C_SCL
     */
	PCC->PCCn[PCC_PORTA_INDEX ]|=PCC_PCCn_CGC_MASK;/* Enable PORTA*/
    PORTA->PCR[2]   |= PORT_PCR_MUX(3)      /* Port A2: MUX = ALT3, LPI2C_SDA */
                                               | PORT_PCR_PE_MASK   /* Pull resistor enable */
                                               | PORT_PCR_PS(1);    /* Pull up selected */
    PORTA->PCR[3]   |= PORT_PCR_MUX(3)      /* Port A3: MUX = ALT3, LPI2C_SCL */
                                                | PORT_PCR_PE_MASK  /* Pull resistor enable */
                                                | PORT_PCR_PS(1);   /* Pull up selected */


    PCC->PCCn[PCC_LPI2C0_INDEX] |= PCC_PCCn_PCS(2)      /* 2 Clk src: 8MHz SIRCDIV2_CLK */
                              | PCC_PCCn_CGC_MASK;   	/* Enable clock for LPI2C0 */

    LPI2C0->MCFGR1 = 	LPI2C_MCFGR1_PRESCALE(1)		/* 2^1 Prescale = 2 */
					|LPI2C_MCFGR1_IGNACK_MASK;		/* Ignore NACK*/

	/* SCL_freq = Input_freq / (2^PRESCALER * (CLKLO + CLKHI + 2))*/
    /* SCL频率 = 输入频率 / (2^PRESCALER * (CLKLO + CLKHI + 2)) = 200k*/

    LPI2C0->MCCR0 = LPI2C_MCCR0_CLKLO(12)    /* 时钟线低电平时间 */
                | LPI2C_MCCR0_CLKHI(6)      /* 时钟线高电平时间 */
                | LPI2C_MCCR0_SETHOLD(3)    /* 启动延时 */
                | LPI2C_MCCR0_DATAVD(3);    /* 数据有效延时 数据有效延迟, 用作 SDA 数据保持时间的最小周期数（减去 1）。必须配置为
小于最小SCL低电平周期。 */

    LPI2C0->MFCR = LPI2C_MFCR_TXWATER(0)	/* Transmitter Water mark set to 0*/
                  |LPI2C_MFCR_RXWATER(3);	/* Receiver Water mark set to 3*/

    LPI2C0->MCR |= LPI2C_MCR_MEN_MASK /* Enable LPI2C as master */
            	| LPI2C_MCR_DBGEN_MASK;

}

/* 
10-8 CMD
命令数据
000b - 发送 DATA[7:0］
001b - 接收（DATA[7:0] + 1）字节
010b - 生成 STOP 条件
011b - 接收并丢弃（DATA[7:0] + 1）字节
100b - 在 DATA[7:0] 中生成（重复）START 和发送地址
101b - 在 DATA[7:0] 中生成（重复）开始和发送地址。此传输预计将返回 NACK。
110b - 使用高速模式在 DATA[7:0] 中生成（重复）START 和发送地址
111b - 使用高速模式在 DATA[7:0] 中生成（重复）开始和发送地址。此传输预计将返回 NACK。
7-0 数据
传输数据
对 DATA 执行 8 位写入操作将使 CMD 字段置零。*/
void LPI2C_Transmit(uint8_t cmd, uint8_t data)
{
    while(((LPI2C0->MSR) & LPI2C_MSR_TDF_MASK) == 0) ; // 没有请求发送数据
	LPI2C0->MTDR = (LPI2C_MTDR_CMD(cmd)|LPI2C_MTDR_DATA(data)); // 发送起始信号和地址
    LPI2C0->MSR |= LPI2C_MSR_TDF_MASK; //clear flag
    
//	LPI2C0->MTDR = (0x04<<8)|((0b101<<1)|0); // 起始信号+设备地址（前7位）+写数据位0
}

// 发送一个字节 mode:数据/命令标志 0,表示命令;1,表示数据;
void lp_oled_Write_Byte(uint8_t data, uint8_t mode) {
    LPI2C_Transmit(0x04, 0x78); // i2c start + 设备地址
    if (mode) {
        LPI2C_Transmit(0x00, 0x40); // 寄存器地址
    }else{
        LPI2C_Transmit(0x00, 0x00); // 寄存器地址
    }
    LPI2C_Transmit(0x00, data); // 数据
    LPI2C_Transmit(0x02, 0x00); // i2c stop
}


//OLED的初始化
void lp_oled_Init(void){
    LPI2C_init();
    later_ms(500);
    lp_oled_Write_Byte(0xAE, lp_oled_CMD); //--turn off oled panel
    lp_oled_Write_Byte(0x00, lp_oled_CMD); //---set low column address
    lp_oled_Write_Byte(0x10, lp_oled_CMD); //---set high column address
    lp_oled_Write_Byte(0x40, lp_oled_CMD); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    lp_oled_Write_Byte(0x81, lp_oled_CMD); //-设置对比度控制寄存器--set contrast control register
    lp_oled_Write_Byte(0xCF, lp_oled_CMD); // 设置SEG输出电流亮度Set SEG Output Current Brightness
    lp_oled_Write_Byte(0xA1, lp_oled_CMD); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    lp_oled_Write_Byte(0xC8, lp_oled_CMD); // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    lp_oled_Write_Byte(0xA6, lp_oled_CMD); //--set normal display
    lp_oled_Write_Byte(0xA8, lp_oled_CMD); //--set multiplex ratio(1 to 64)
    lp_oled_Write_Byte(0x3f, lp_oled_CMD); //--1/64 duty
    lp_oled_Write_Byte(0xD3, lp_oled_CMD); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    lp_oled_Write_Byte(0x00, lp_oled_CMD); //-not offset
    lp_oled_Write_Byte(0xd5, lp_oled_CMD); // 设置时钟分频因子,震荡频率//--set display clock divide ratio/oscillator frequency
    lp_oled_Write_Byte(0xf0, lp_oled_CMD); //[3:0],分频因子;[7:4],震荡频率//--set divide ratio, Set Clock as     Frames/Sec
    lp_oled_Write_Byte(0xD9, lp_oled_CMD); //--set pre-charge period
    lp_oled_Write_Byte(0xF1, lp_oled_CMD); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    lp_oled_Write_Byte(0xDA, lp_oled_CMD); //--set com pins hardware configuration
    lp_oled_Write_Byte(0x12, lp_oled_CMD);
    lp_oled_Write_Byte(0xDB, lp_oled_CMD); //--set vcomh
    lp_oled_Write_Byte(0x30, lp_oled_CMD); // Set VCOM Deselect Level
    lp_oled_Write_Byte(0x20, lp_oled_CMD); //-Set Page Addressing Mode (0x00/0x01/0x02)
    lp_oled_Write_Byte(0x02, lp_oled_CMD); //
    lp_oled_Write_Byte(0x8D, lp_oled_CMD); //--set Charge Pump enable/disable
    lp_oled_Write_Byte(0x14, lp_oled_CMD); //--set(0x10) disable
    lp_oled_Clear();
    lp_oled_Write_Byte(0xAF, lp_oled_CMD);
    lp_oled_ColorTurn(0);   // 0正常显示  1 反色显示
    // lp_oled_DisplayTurn(1); // 0正常显示  1 屏幕翻转显示
}

//开启OLED显示
void lp_oled_Display_On(void) {
    lp_oled_Write_Byte(0x8D, lp_oled_CMD); // 电荷泵使能
    lp_oled_Write_Byte(0x14, lp_oled_CMD); // 开启电荷泵
    lp_oled_Write_Byte(0xAF, lp_oled_CMD); // 点亮屏幕
}

// 关闭OLED显示
void lp_oled_Display_Off(void) {
    lp_oled_Write_Byte(0x8D, lp_oled_CMD); // 电荷泵使能
    lp_oled_Write_Byte(0x10, lp_oled_CMD); // 关闭电荷泵
    lp_oled_Write_Byte(0xAE, lp_oled_CMD); // 关闭屏幕
}



// 更新显存到OLED
void lp_oled_Refresh(void) {
	uint8_t i,n;
	for(i=0;i<8;i++) {
        lp_oled_Write_Byte(0xb0 + i, lp_oled_CMD); // 设置行起始地址
        lp_oled_Write_Byte(0x00, lp_oled_CMD);     // 设置低列起始地址
        lp_oled_Write_Byte(0x10, lp_oled_CMD);     // 设置高列起始地址
        LPI2C_Transmit(0x04, 0x78); // i2c start + 设备地址
        LPI2C_Transmit(0x00, 0x40); // 寄存器地址
        for(n=0;n<128;n++) {
			LPI2C_Transmit(0x00, lp_oled_GRAM[n][i]); // 数据
		}
		LPI2C_Transmit(0x02, 0x00); // i2c stop
  }
}

//清屏函数
void lp_oled_Clear(void) {
	uint8_t i,n;
	for(i=0;i<8;i++) {
	   for(n=0;n<128;n++) {
			lp_oled_GRAM[n][i]=0;//清除所有数据
		}
    }
	lp_oled_Refresh();//更新显示
}

// 反显函数
void lp_oled_ColorTurn(uint8_t i) {
    if (i == 0) {
        lp_oled_Write_Byte(0xA6, lp_oled_CMD); // 正常显示
    }
    if (i == 1) {
        lp_oled_Write_Byte(0xA7, lp_oled_CMD); // 反色显示
    }
}

// 屏幕旋转180度函数
void lp_oled_DisplayTurn(uint8_t i) {
    if (i == 0) {
        lp_oled_Write_Byte(0xC8, lp_oled_CMD); // 正常显示
        lp_oled_Write_Byte(0xA1, lp_oled_CMD);
    }
    if (i == 1) {
        lp_oled_Write_Byte(0xC0, lp_oled_CMD); // 反转显示
        lp_oled_Write_Byte(0xA0, lp_oled_CMD);
    }
    lp_oled_Refresh();
}

// 画点函数  x:0~127   y:0~63   t:1 填充 0,清空
void lp_oled_DrawPoint(uint8_t x,uint8_t y,uint8_t t) {
    uint8_t i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    if(t) { lp_oled_GRAM[x][i] |= n;}
	else {
        lp_oled_GRAM[x][i] = ~lp_oled_GRAM[x][i];
        lp_oled_GRAM[x][i] |= n;
        lp_oled_GRAM[x][i] = ~lp_oled_GRAM[x][i];
    }
}

//画线  //x1,y1:起点坐标  //x2,y2:结束坐标
void lp_oled_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode)
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
		lp_oled_DrawPoint(uRow,uCol,mode);//画点
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
    lp_oled_Refresh();
}

//x,y:圆心坐标 //r:圆的半径
void lp_oled_DrawCircle(uint8_t x,uint8_t y,uint8_t r) {
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r) {
        lp_oled_DrawPoint(x + a, y - b,1);
        lp_oled_DrawPoint(x - a, y - b,1);
        lp_oled_DrawPoint(x - a, y + b,1);
        lp_oled_DrawPoint(x + a, y + b,1);

        lp_oled_DrawPoint(x + b, y + a,1);
        lp_oled_DrawPoint(x + b, y - a,1);
        lp_oled_DrawPoint(x - b, y - a,1);
        lp_oled_DrawPoint(x - b, y + a,1);
        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0) {
            b--;
            a--;
        }
    }
    lp_oled_Refresh();
}

//在指定位置显示一个字符,包括部分字符 //x:0~127 //y:0~63
//size1:选择字体 6x8/6x12/8x16/12x24
//mode:0,反色显示;1,正常显示
void lp_oled_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode)
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
			if(temp&0x01) lp_oled_DrawPoint(x, y, mode);
			else lp_oled_DrawPoint(x, y, !mode);
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
void lp_oled_ShowString(uint8_t x,uint8_t y,char *chr,uint8_t size1,uint8_t mode)
{
	while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
	{
		lp_oled_ShowChar(x,y,*chr,size1,mode);
		if(size1==8)x+=6;
		else x+=size1/2;
		chr++;
    }
    lp_oled_Refresh();
}

//m^n
uint32_t lp_oled_Pow(uint8_t m,uint8_t n){
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
void lp_oled_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode)
{
    uint8_t t, temp, m = 0;
    if (size1 == 8)
        m = 2;
    for (t = 0; t < len; t++) {
        temp = (num / lp_oled_Pow(10, len - t - 1)) % 10;
        if (temp == 0) {
            lp_oled_ShowChar(x + (size1 / 2 + m) * t, y, '0', size1, mode);
        }else {
            lp_oled_ShowChar(x + (size1 / 2 + m) * t, y, temp + '0', size1, mode);
        }
    }
    lp_oled_Refresh();
}
// 显示图片 //x,y：起点坐标 //sizex,sizey,图片长宽
//BMP[]：要写入的图片数组 //mode:0,反色显示;1,正常显示
void lp_oled_ShowPicture(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode)
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
                    lp_oled_DrawPoint(x, y, mode);
                else
                    lp_oled_DrawPoint(x, y, !mode);
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



