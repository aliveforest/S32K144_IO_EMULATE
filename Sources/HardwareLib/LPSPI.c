/*
 * LPSPI.c
 *
 *  Created on: 2023年9月5日
 *      Author: dengtongbei
 */
#include "LPSPI.h"

/* LPSPI1和对应引脚初始化 */
void LPSPI1_init_master(void){
	/* LPSPI1 时钟: */
	PCC->PCCn[PCC_LPSPI1_INDEX] = 0;									 /* 禁用时钟以修改 PCS ( default) 	*/
	PCC->PCCn[PCC_LPSPI1_INDEX] = PCC_PCCn_PR_MASK						 /* (默认）外设存在。			*/
								  | PCC_PCCn_CGC_MASK | PCC_PCCn_PCS(6); /* Enable PCS=SPLL_DIV2 (40 MHz func'l clock) 	*/
																		 /* LPSPI1 初始化: */
	LPSPI1->CR = 0x00000000;											 /* 禁用模块进行配置			*/
	LPSPI1->IER = 0x00000000;											 /* 未使用中断 						*/
	LPSPI1->DER = 0x00000000;											 /* DMA not used 								*/
	LPSPI1->CFGR0 = 0x00000000;											 /* Defaults: 									*/
																		 /* RDM0=0: 正常接收数据到 FIFO 		*/
																		 /* CIRFIFO=0; 循环 FIFO 已禁用 		*/
																		 /* HRSEL, HRPOL, HREN=0: 主机请求禁用 */
	LPSPI1->CFGR1 = LPSPI_CFGR1_MASTER_MASK  /* 配置：主模式									    */
					|LPSPI_CFGR1_SAMPLE(1);	/* PCSCFG=0: PCS[3:2] 启用 						   */
											 /* OUTCFG=0: CS 否定时，输出数据保留最后的值	  		 */
											 /* PINCFG=0: SIN 为输入，SOUT 为输出 			 		*/
											 /* MATCFG=0: 禁用匹配 							 		*/
											 /* PCSPOL=0: PCS 为低电平有效 					 		*/
											 /* NOSTALL=0: 如果 Tx FIFO 空或 Rx FIFO 满，则出现停滞  */
											 /* AUTOPCS=0: 不适用于主模式 							*/
											 /* SAMPLE=1b - 输入数据在延迟的 SCK 边沿采样 			*/
											 /* MASTER=1: 主模式 									*/
	/* TCR：传输命令寄存器 */
	LPSPI1->TCR = LPSPI_TCR_CPHA(1)	   	   /* CPHA=1b - 在 SCK 前沿更改数据，在 SCK 后沿捕获数据          */
				  | LPSPI_TCR_PRESCALE(2)  /* 预分频值: 2^(PRESCALE), 2^2=4 							 */
				  | LPSPI_TCR_PCS(3)	   /* PCS 外设芯片选择 使用 PCS3 传输 							  */
				  | LPSPI_TCR_FRAMESZ(7)   /* FRAMESZ=7: 传输指令，帧宽：7+1=8 位 						  */
				  | LPSPI_TCR_RXMSK(1)	   /* LPSPI_TCR_RXMSK: 1b - 屏蔽接收数据 */
				  |LPSPI_TCR_CPOL(1);	   /* CPOL=1: SCK 非活动状态为高电平 							  */
										   /* LSBF=0: 数据先传输 MSB 									 */
										   /* BYSW=0: 禁用字节交换 										 */
										   /* CONT, CONTC=0: 连续传输禁用 								 */
										   /* RXMSK=0: 正常传输：Rx FIFO 中存储 Rx 数据 				 */
										   /* TXMSK=0: 正常传输：从 tx FIFO 加载数据 				     */
										   /* WIDTH=0: 单比特传输 										*/
	/* 基于 100 纳秒 预分频功能时钟的时钟分频器 	*/
	LPSPI1->CCR = LPSPI_CCR_SCKPCS(4)	/* SCKPCS=4: SCK to PCS delay = 4+1 = 5 (500 nsec) 			*/
				| LPSPI_CCR_PCSSCK(4) 	/* PCSSCK=4: PCS to SCK delay = 9+1 = 10 (1 usec) 				*/
				| LPSPI_CCR_DBT(8) 		/* DBT=8: 两次转运之间的延迟 = 8+2 = 10 (1 usec) 			*/
				| LPSPI_CCR_SCKDIV(8); /* SCKDIV=8: SCK divider =8+2 = 10 (1 usec: 1 MHz baud rate) 	*/
									
	LPSPI1->FCR = LPSPI_FCR_TXWATER(0);	/* RXWATER=0: 当 Rx FIFO >0 时设置 Rx 标志 	*/
										/* TXWATER=3: 当 Tx FIFO <= 3 时设置 Tx 标志 	*/
	/* 启用模块进行操作 			*/
	LPSPI1->CR = LPSPI_CR_MEN_MASK 		/* MEN=1: 模块已启用 				*/
				| LPSPI_CR_DBGEN_MASK;  /* DBGEN=1: 在调试模式下启用模块 	*/
										/* DOZEN=0: 在打盹模式下启用模块 	*/
										/* RST=0: 主逻辑未复位 				*/
	/* Pin number        | Function
	 * ----------------- |---------------
	 * PTB17			 | LPSPI1_PCS3
	 * PTB14             | LPSPI1_SCK  D0
	 * PTB15             | LPSPI1_SIN
	 * PTB16			 | LPSPI1_SOUT D1  */
	PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK; /* 启用 PORTB 的时钟 */
	PORTB->PCR[14] |= PORT_PCR_MUX(3);				 /* Port B14: MUX = ALT3, LPSPI1_SCK */
	PORTB->PCR[15] |= PORT_PCR_MUX(3);				 /* Port B15: MUX = ALT3, LPSPI1_SIN */
	PORTB->PCR[16] |= PORT_PCR_MUX(3);				 /* Port B16: MUX = ALT3, LPSPI1_SOUT */
	PORTB->PCR[17] |= PORT_PCR_MUX(3);				 /* Port B17: MUX = ALT3, LPSPI1_PCS3 */
}
/* 发送1字节(8位)函数 */
void LPSPI1_transmit_8bits(uint8_t send)
{ /* 等待 Tx FIFO 可用 	*/
	while ((LPSPI1->SR & LPSPI_SR_TDF_MASK) >> LPSPI_SR_TDF_SHIFT == 0) ;
	LPSPI1->TDR = send;				 /* 传输数据 */
	LPSPI1->SR |= LPSPI_SR_TDF_MASK; /* Clear TDF flag (清除数据传输标志)	*/
	while ((LPSPI1->SR & LPSPI_SR_TDF_MASK) >> LPSPI_SR_TDF_SHIFT == 0) ;
}
