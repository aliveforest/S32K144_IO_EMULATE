/*****************************************************************
 * @file      : Sources\HardwareLib\UART_IO.h 
 * @brief     : 
 * @author    : aliveforest@163.com
 * @version   : v1.0
 * @date      : 2023-8-5
 * @copyright : Copyright aliveforest@163.com 2023
*****************************************************************/

#ifndef UART_IO_H_
#define UART_IO_H_
#include "S32K144.h"
#include "stdbool.h"

typedef struct {
    GPIO_Type * PTx;
    PORT_Type * PORTx;
    uint16_t PORTx_IRQn;
    uint16_t PCC_PORTx_INDEX;
    uint32_t tx_pin;
    uint32_t rx_pin;
}UART_type;

void IO_UART_init(UART_type *uartx, uint8_t _x, uint32_t tx_pin, uint32_t rx_pin, uint32_t baud);
// void uart_TX_Set(UART_type *uartx);
// void uart_TX_Clr(UART_type *uartx);
// bool uart_Read_RX(UART_type *uartx);
void uartSendChar(UART_type *uartx, char data);
void IOUART_printf(char *fmt, ...);
void uartSendStr(UART_type *uartx, char* str);
void uartReceiveChar(UART_type *uartx);
void uartReceiveStr(UART_type *uartx);
void S32_NVIC_EnableIRQ (uint32_t vector_number, uint32_t priority) ;




#endif
