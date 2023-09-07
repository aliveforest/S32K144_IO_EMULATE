/*
 * Copyright (c) 2014 - 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2016 - 2018, NXP.
 * All rights reserved.
 */
#include "device_registers.h"	/* include peripheral declarations S32K144 */
#include "clocks_and_modes.h"

/**
 * 配置MCU时钟, 使SYS_CLK=80MHz, BUS_CLK=40MHz, FLASH_CLK=20MHz, 用外部晶振EXTAL=8MHz
 * SPLL_CLK=160MHz, VCO_CLK=320MHz
 * 计算公式:
 * BUS_CLK=SYS_CLK / DIVBUS
 * SYS_CLK=SPLL_CLK / DIVCORE
 * SPLL_CLK=(VCO_CLK) / 2
 * VCO_CLK=SPLL_SOURCE / (PREDIV + 1) x (MULT + 16)
 * 选择外部晶振作为SPLL_SOURCE, SPLL_SOURCE=8MHz
 */
void Clock_Init(void)
{
    // 以下初始化顺序不可改变

    SOSC_init_8MHz(); // SOSC&SPLL_SOURCE初始化为输出8M

    SPLL_init_160MHz(); // SPLL&SCG初始化为输出160M

    NormalRUNmode_80MHz(); // sys clock = 80M, bus clock = 40M, flash = 20M

}

void SOSC_init_8MHz(void)
{
	/*!
	 * SOSC Initialization (8 MHz):
	 * ===================================================
	 */
	SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV1(1)|
				   SCG_SOSCDIV_SOSCDIV2(1);  	/* SOSCDIV1 & SOSCDIV2 =1: divide by 1 ->8MHz	*/
	SCG->SOSCCFG  =	SCG_SOSCCFG_RANGE(2)|		/* Range=2: 中等频率 (SOSC betw 1MHz-8MHz) 	*/
					SCG_SOSCCFG_EREFS_MASK;		/* HGO=0:   Config xtal osc for low power 		*/
  	  	  	  	  	  	  	  	  	  	  	  	/* EREFS=1: Input is external XTAL 			*/

  while(SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK); 	/* 确保 SOSCCSR 解锁 */						
  SCG->SOSCCSR = SCG_SOSCCSR_SOSCEN_MASK; 		/* LK=0:          SOSCCSR 可以写入 				*/
												/* SOSCCMRE=0:    如果启用，则 OSC CLK 监控 IRQ 		*/
												/* SOSCCM=0:      禁用 OSC CLK 监控 			*/
												/* SOSCERCLKEN=0: Sys OSC 3V ERCLK output clk disabled */
												/* SOSCLPEN=0:    Sys OSC disabled in VLP modes 		*/
												/* SOSCSTEN=0:    在停止模式下禁用Sys OSC  		*/
												/* SOSCEN=1:      启用振荡器 					*/

while(!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK));	/* Wait for sys OSC clk valid */
}

void SPLL_init_160MHz(void)
{
	/*!
	 * SPLL Initialization (160 MHz):
	 * ===================================================
	 */
  while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK); 	/* Ensure SPLLCSR unlocked 				*/
  SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;  	/* SPLLEN=0: SPLL is disabled (default) 	*/

  SCG->SPLLDIV |= 	SCG_SPLLDIV_SPLLDIV1(2)|	/* SPLLDIV1 divide by 2 ; SPLL1_DIV1_CLK= 160 MHz / 2 = 80MHz*/
					SCG_SPLLDIV_SPLLDIV2(3);  	/* SPLLDIV2 divide by 4 ; SPLL2_DIV2_CLK= 160 MHz / 4 = 40MHz*/

  SCG->SPLLCFG = SCG_SPLLCFG_MULT(24);			/* PREDIV=0: Divide SOSC_CLK by 0+1=1 		*/
  	  	  	  	  	  	  	  	  	  	  		/* MULT=24:  Multiply sys pll by 4+24=40 	*/
										  		/* SPLL_CLK = 8MHz / 1 * 40 / 2 = 160 MHz 	*/

  while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK); 	/* Ensure SPLLCSR unlocked 						*/
  SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK; 		/* LK=0:        SPLLCSR can be written 			*/
                             	 	 	 		/* SPLLCMRE=0:  SPLL CLK monitor IRQ if enabled 	*/
                             	 	 	 	 	/* SPLLCM=0:    SPLL CLK monitor disabled 			*/
                             	 	 	 	 	/* SPLLSTEN=0:  SPLL disabled in Stop modes 		*/
                             	 	 	 	 	/* SPLLEN=1:    Enable SPLL 						*/

  while(!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK)); /* Wait for SPLL valid */
}

void NormalRUNmode_80MHz (void)
{
/*! Slow IRC is enabled with high range (8 MHz) in reset.
 *	Enable SIRCDIV2_CLK and SIRCDIV1_CLK, divide by 1 = 8MHz
 *  asynchronous clock source.
 * ==========================================
*/
	SCG->SIRCDIV = SCG_SIRCDIV_SIRCDIV1(1)  //SIRCDIV2_CLK and SIRCDIV1_CLK, divide by 1 = 8MHz
				 | SCG_SIRCDIV_SIRCDIV2(1);

/*!
 *  Change to normal RUN mode with 8MHz SOSC, 80 MHz PLL:
 *  ====================================================
 */
  SCG->RCCR=SCG_RCCR_SCS(6)      /* Select PLL as clock source 								*/
    |SCG_RCCR_DIVCORE(0b01)      /* DIVCORE=1, div. by 2: Core clock = 160/2 MHz = 80 MHz 		*/
    |SCG_RCCR_DIVBUS(0b01)       /* DIVBUS=1, div. by 2: bus clock = 40 MHz 					*/
    |SCG_RCCR_DIVSLOW(0b11);     /* DIVSLOW=3, div. by 4: SCG slow, flash clock= 20 MHz	*/

  while (((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT ) != 6) {}	/* Wait for sys clk src = SPLL */
}

/*
 *
 * S32K144的时钟分析并设置时钟 - 其他 - 恩智浦技术社区
https://www.nxpic.org.cn/module/forum/thread-621481-1-1.html#:~:text=1%E3%80%81LPO%E4%B8%BAMCU%E5%86%85%E9%83%A8%E9%9B%86%E6%88%90%E4%BD%8E%E5%8A%9F%E8%80%97%E6%97%B6%E9%92%9F%EF%BC%8C%E5%9B%BA%E5%AE%9A%E9%A2%91%E7%8E%87%E4%B8%BA128Khz%E3%80%82%202%E3%80%81Fast%20IRC%E4%B8%BA%E5%BF%AB%E9%80%9F%E5%86%85%E9%83%A8%E6%97%B6%E9%92%9F%E6%BA%90%E3%80%82,3%E3%80%81Slow%20IRC%E4%B8%BA%E6%85%A2%E9%80%9F%E5%86%85%E9%83%A8%E6%97%B6%E9%92%9F%E6%BA%90%E3%80%82%204%E3%80%81OSC%E4%B8%BA%E5%A4%96%E9%83%A8%E6%97%B6%E9%92%9F%E6%BA%90%EF%BC%8C%E8%BF%9E%E6%8E%A5%E5%88%B0%E5%A4%96%E9%83%A8%E6%99%B6%E6%8C%AF%E3%80%82

*/

