#include "S32K144.h"
#include "LP_IT_timer.h"
#include "LPUART.h"
#include "UART_IO.h"

extern UART_type *uart1;
int lpit0_ch0_flag_counter = 0; /* LPIT0 chan 0 超时计数器 */

void LPIT0_init(void) {
    /* LPIT Clocking: ============================== */
    PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* 时钟源 = 6 (SPLL2_DIV2_CLK 40MHz)*/
    PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* 使能时钟至 LPIT0 寄存器         */
    /* LPIT Initialization: */
    LPIT0->MCR |= LPIT_MCR_M_CEN_MASK; /* DBG_EN-0: Timer chans stop in Debug mode */
                                       /* DOZE_EN=0: Timer chans are stopped in DOZE mode */
                                       /* SW_RST=0: SW reset does not reset timer chans, regs */
                                       /* M_CEN=1: 启用模块 clk（允许写入其他 LPIT0 寄存器） */
    LPIT0->MIER = LPIT_MIER_TIE0_MASK; /* TIE0=1：为 Chan 0 启用定时器中断*/
    LPIT0->TMR[0].TVAL = (uint32_t)(40000000);     /* Chan 0 Timeout period: 40M clocks 为1s*/
    LPIT0->TMR[0].TCTRL = LPIT_TMR_TCTRL_T_EN(1);       // 开启定时器
                                        /* T_EN=1: 启用定时器通道 */
                                        /* CHAIN=0: 禁用通道链 */
                                        /* MODE=0: 32 周期计数器模式 */
                                        /* TSOT=0: 定时器在重启后立即递减 */
                                        /* TSOI=0：超时后定时器不停止 */
                                        /* TROT=0 定时器不会在触发时重新加载 */
                                        /* TRG_SRC=0: 外部触发源 */
                                        /* TRG_SEL=0: 定时/计数器第 0 个触发源被选中*/
    LPIT_NVIC_init_IRQs(); 		/* 启用所需的中断和优先级 */                                   
}

void LPIT_NVIC_init_IRQs (void) {
	S32_NVIC->ICPR[1] = 1 << (LPIT0_Ch0_IRQn % 32);  /* IRQ48-LPIT0 ch0: clr any pending IRQ*/
	S32_NVIC->ISER[1] = 1 << (LPIT0_Ch0_IRQn % 32);  /* IRQ48-LPIT0 ch0: enable IRQ */
	S32_NVIC->IP[LPIT0_Ch0_IRQn] = 0xA;              /* IRQ48-LPIT0 ch0: priority 10 of 0-15*/
}


void LPIT0_Ch0_IRQHandler(void) {
//    uint16_t data= lpit0_ch0_flag_counter++;
//    char str[10];
//    sprintf(str, "LPIT0 : %d", lpit0_ch0_flag_counter);
//	uartSendStr(uart1, str);
//
//    PTD-> PTOR |= 1<<15; //red
//	PTD-> PTOR |= 1<<1;
    LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /*  清除 LPIT0 定时器标志 0 */
	uartSendStr(uart1, "IT0 Interrupt");

}



