#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"


/**********************************************************************************************
						Function Declarations
**********************************************************************************************/
void InitSystem(void);
void InitMasterInterrupt(void);
void InitLED2(void);
void InitLED3(void);
void InitUART0(void);
void InitUART1(void);
void WriteToUART0(const unsigned char *pucBuffer);
void WriteToUART1(const unsigned char *pucBuffer);


/**********************************************************************************************
						Interrupt Handler definition
**********************************************************************************************/
void UART0IntHandler(void);
void UART1IntHandler(void);


