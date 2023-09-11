
#include "device_registers.h"
#include "latency.h"
#include "UART_IO.h"
#include <stdio.h>
#include <string.h>
#include "stdarg.h"
#include <stdint.h>
#include <stdbool.h>


extern UART_type *uart1;
uint32_t interval_t_100ns; // 信号间隔时间 *100ns
uint32_t interval_half_100ns; // 信号间隔时间 *100ns
uint16_t i_c=0, cnt=0;

static float error_100ns = 10.0f; // 实际测量sistick有多了1us的误差

void IO_UART_init(UART_type *uartx, uint8_t _x, uint32_t tx_pin, uint32_t rx_pin, uint32_t baud)
{

    interval_t_100ns = (uint32_t)(10000000.0f/baud-error_100ns);
    interval_half_100ns = (uint32_t)(interval_t_100ns/2);
    switch (_x)
    { // ******初始化IO口 和 rx_pin 外部中断配置 *************/
    case 'A':uartx->PTx = PTA; uartx->PORTx = PORTA; uartx->PCC_PORTx_INDEX = PCC_PORTA_INDEX;
             uartx->PORTx_IRQn = PORTA_IRQn; break;
    case 'B':uartx->PTx = PTB; uartx->PORTx = PORTB; uartx->PCC_PORTx_INDEX = PCC_PORTB_INDEX;
             uartx->PORTx_IRQn = PORTB_IRQn; break;
    case 'C':uartx->PTx = PTC; uartx->PORTx = PORTC; uartx->PCC_PORTx_INDEX = PCC_PORTC_INDEX;
             uartx->PORTx_IRQn = PORTC_IRQn; break;
    case 'D':uartx->PTx = PTD;  uartx->PORTx = PORTD; uartx->PCC_PORTx_INDEX = PCC_PORTD_INDEX;
             uartx->PORTx_IRQn = PORTD_IRQn; break;
    case 'E':uartx->PTx= PTE;  uartx->PORTx = PORTE; uartx->PCC_PORTx_INDEX = PCC_PORTE_INDEX;
             uartx->PORTx_IRQn = PORTE_IRQn; break;
    }
    uartx->tx_pin = tx_pin;
    uartx->rx_pin = rx_pin;
    PCC-> PCCn[uartx->PCC_PORTx_INDEX] |= PCC_PCCn_CGC_MASK; // 开启时钟模块
    uartx->PORTx->PCR[tx_pin] = PORT_PCR_MUX(1) | PORT_PCR_DSE(0);   // 配置TX引脚为GPIO，并设置驱动能力 1->强驱动, 0->低驱动
    uartx->PORTx->PCR[rx_pin] = PORT_PCR_MUX(1) | PORT_PCR_IRQC(0x0a); // 配置RX引脚为GPIO 配置下降沿中断
    uartx->PTx->PDDR |= (1 << tx_pin);   //设置引脚为输出模式
    uartx->PTx->PDDR &= ~(1 << rx_pin);  //设置引脚为输入模式
    uartx->PTx->PSOR |= (1UL << (uartx->tx_pin)); // TX高电平
    uartx->PTx->PCOR |= (1UL << (uartx->rx_pin)); // RX高电平
    S32_NVIC_EnableIRQ(uartx->PORTx_IRQn, 11); /* 使能中断，并设置优先级 */
}
// 发送高电平
void uart_TX_Set(UART_type *uartx) {
    uartx->PTx->PSOR |= (1UL << (uartx->tx_pin)); 
}
// 发送低电平
void uart_TX_Clr(UART_type *uartx) {
    uartx->PTx->PCOR |= (1UL << (uartx->tx_pin)); 
}
// 读取接收脚的电平
bool uart_Read_RX(UART_type *uartx) {
    return(uartx->PTx->PDIR & (1UL << (uartx->rx_pin)));
}

// 发送字符函数
void uartSendChar(UART_type *uartx, char data)
{
    int i;
    // 发送起始位（逻辑低电平）
    uart_TX_Clr(uartx);
    later_100_ns(interval_t_100ns); // 信号间隔时间 *100ns
    // 发送数据位（低位在前，高位在后）
    for (i = 0; i < 8; i++) {
        if (data & 0x01) {
            uart_TX_Set(uartx); // 发送高电平
        }else {
            uart_TX_Clr(uartx); // 发送低电平
        }
        data >>= 1;              // 移动到下一位
        later_100_ns(interval_t_100ns); // 信号间隔时间 *100ns
    }
    // 发送停止位（逻辑高电平）
    uart_TX_Set(uartx);
    later_100_ns(interval_t_100ns); // 信号间隔时间 *100ns
}

// 发送字符串函数
void uartSendStr(UART_type *uartx, char* str){
    // sprintf("%s\r\n",str);
    int n= strlen(str), i=0;
    for(;i<n;++i){
        uartSendChar(uartx, str[i]);
    }
    uartSendChar(uartx, '\r');
    uartSendChar(uartx, '\n');
}
char IOUART_TX_BUF[200];
/* 串口打印输出函数 */
void IOUART_printf(char *fmt, ...) {
	uint32_t bytesRemaining;
	va_list ap;
	va_start(ap, fmt);
	vsprintf((char *)IOUART_TX_BUF, fmt, ap);
	va_end(ap);
	uartSendStr(uart1, IOUART_TX_BUF);
}


char str_buf[100];
char char_buf=0;
// 接收字符函数
void uartReceiveChar(UART_type *uartx) {
    uint8_t i;
    char data=0;
    // 等待接收起始位（逻辑低电平）
    if ((uart_Read_RX(uartx))) {
        return;  //如果不是低电平，则直接退出
    }
    // 延时半位时间，确保在数据位的中间
    later_100_ns(interval_half_100ns); // 信号间隔时间 *100ns
    // 接收8个数据位
    for (i = 0; i < 8; i++) {
        later_100_ns(interval_t_100ns); // 信号间隔时间 *100ns
        if (uart_Read_RX(uartx))
        	data |= (1 << i);     // 接收引脚为高电平，接收到1
        else
        	data &= ~(1 << i);    // 接收引脚为低电平，接收到0
    }
    later_100_ns(interval_t_100ns); // 信号间隔时间 *100ns
    // 等待停止位（逻辑高电平）
    if (!(uart_Read_RX(uartx))){
        return; //如果不是高电平，则直接退出
    }
    // uartSendChar(uartx, data);
    // char_buf = data;
    str_buf[i_c++] = data;
    if(i_c >= 10) {
        i_c=0; 
        uartSendStr(uart1, str_buf);
        later_ms(10);
    }
}

// // 接收字符串函数
// void uartReceiveStr(UART_type *uartx){

//     uartReceiveChar(uartx);
//     str_buf[i_c] = char_buf;
//     if(i_c<strlen(str_buf)-1) ++i_c;
//     else i_c = 0;

//     if(i_c && str_buf[i_c-1] =='\r'&& str_buf[i_c] =='\n') // 回车换行 0x0D,0x0A
//     {
//     	i_c = 0;
//         uartSendStr(uart1, str_buf); // 接收到再发送出去
//     	return;
//     }
// }

// void S32_NVIC_EnableIRQ(IRQn_Type IRQn, int Priority){
//     /* 左移5位是因为取余32，因为ISER[0-7]依次对应0-256位中断 */
// 	S32_NVIC->ICPR[1] = 1 << (IRQn % 32);  /* clr any pending IRQ*/
//     S32_NVIC->ISER[(uint32_t)((int32_t)IRQn) >> 5] = (uint32_t)(1 << ((uint32_t)((int32_t)IRQn) & (uint32_t)0x1F));
//     S32_NVIC->IP[IRQn] = Priority;
// }

// 使能中断
void S32_NVIC_EnableIRQ(IRQn_Type IRQn, int Priority){

	S32_NVIC->ICPR[1] = 1 << (IRQn % 32);  /* clr any pending IRQ*/
    S32_NVIC->ISER[1] = 1 << (IRQn % 32);   /* enable interrupt */
    S32_NVIC->IP[IRQn] = Priority;
}

void PORTA_IRQHandler(void){
    uart1->PORTx->PCR[uart1->rx_pin] |= PORT_PCR_IRQC(0);  //禁用中断状态标志
    uartReceiveChar(uart1); // 接收字符
    uart1->PORTx->PCR[uart1->rx_pin] |= PORT_PCR_ISF_MASK; //清除外部中断
    uart1->PORTx->PCR[uart1->rx_pin] |= PORT_PCR_IRQC(0xa); // 再开启下降沿中断
}

void PORTB_IRQHandler(void)
{
}

void PORTC_IRQHandler(void)
{
}
void PORTD_IRQHandler(void)
{

}
void PORTE_IRQHandler(void)
{

}
