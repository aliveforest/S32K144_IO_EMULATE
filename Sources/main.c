/* ###################################################################
**     Filename    : main.c
**     Processor   : S32K1xx
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.00
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including necessary module. Cpu.h contains other modules needed for compiling.*/
#include "Cpu.h"

  volatile int exit_code = 0;

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "stdlib.h"
#include "stdio.h"
#include <stdint.h>
#include <stdbool.h>
#include "string.h"
#include "HardwareLib.h"

UART_type uart_1;       // 定义IO_UART类型
UART_type *uart1 = &uart_1;       // 定义IO_UART指针

IIC_type IIC_1;			//定义IO_IIC类型
IIC_type *IIC1 = &IIC_1;		  //定义IO_IIC指针

void WDOG_disable (void) {
	WDOG->CNT=0xD928C520;     /* Unlock watchdog 		*/
	WDOG->TOVAL=0x0000FFFF;   /* Maximum timeout value 	*/
	WDOG->CS = 0x00002100;    /* Disable watchdog 		*/
}

int main(void)
{
  /* Write your local variable definition here */
	// 内存分配 
	// uart1 = (UART_type *)malloc(sizeof(UART_type));  // 头文件位 stdlib.h
	// uart1->PTx = (GPIO_Type *)malloc(sizeof(GPIO_Type));
	// uart1->PORTx = (PORT_Type *)malloc(sizeof(PORT_Type));
	// IIC1 = (IIC_type *)malloc(sizeof(IIC_type));
	// IIC1->PTx = (GPIO_Type *)malloc(sizeof(GPIO_Type));
	// IIC1->PORTx = (PORT_Type *)malloc(sizeof(PORT_Type));

	int x=0,y=0;
	int numcnt = 1; 

	WDOG_disable();
	CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
				   g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
	CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);
	systick_delay_init(); // systick latency延时

	// RGB_LED_KEY_init();
	// IO_UART_init(uart1, 'A', 9u, 8u, 115200);   // IO_UART初始�??????????????? TX  RX
	// OLED_Init(IIC1, 'C', 17u, 16u, 2u);		 // SDA  SCL  delay_time us
	// OLED_Clear(IIC1);
	SPI_OLED_Init();
	LPUART1_init();
	// LPIT0_init();      // 初始化LPIT0
	// LPTMR_init();
	// LPSPI1_init_master();
	LPSPI_OLED_Init();
	GPIO_init('D', 2, 1);

/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
#ifdef PEX_RTOS_INIT
	PEX_RTOS_INIT(); /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
#endif
/*** End of Processor Expert internal initialization.                    ***/

	/* Write your code here */
	/* For example: for(;;) { } */
	for(;;) {
		SPI_OLED_ShowString(10, y++, "LPSPI !",16,1);
        if(y>=50) {y=0; SPI_OLED_Clear(); LPSPI_Clear();}
		LPUART1_printf("LPSPI ! \r\n");
		later_ms(200);
		LPSPI_ShowString(10, y++, "LPSPI !",16,1);
	}

/*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;) {
    if(exit_code != 0) {
      break;
    }
  }
  return exit_code;
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.1 [05.21]
**     for the NXP S32K series of microcontrollers.
**
** ###################################################################
*/
