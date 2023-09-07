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
void uartSendStr(UART_type *uartx, char* str);
void uartReceiveChar(UART_type *uartx);
void uartReceiveStr(UART_type *uartx);
void S32_NVIC_EnableIRQ(IRQn_Type IRQn, int Priority);




#endif
