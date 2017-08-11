#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include"HardwareProfile.h"

/**********************************************************************************************
						Variables Declarations 
**********************************************************************************************/
unsigned char UART0Count=0;
unsigned char UART0Buffer[255];
unsigned char UART1Count=0;
unsigned char UART1Buffer[255];
unsigned char NeglectZeros = 0;
unsigned char DataReady;



void UART0IntHandler(void)
{

    unsigned long ulStatus;

    // Get the interrupt status.
    ulStatus = UARTIntStatus(UART0_BASE, true);

    // Clear the asserted interrupts.
    UARTIntClear(UART0_BASE, ulStatus);

    UART0Buffer[UART0Count++]  = UARTCharGet(UART0_BASE);

}

void UART1IntHandler(void)
{
    unsigned long ulStatus;

    ulStatus = UARTIntStatus(UART1_BASE, true);

    UARTIntClear(UART1_BASE, ulStatus);

    UART1Buffer[UART1Count++]  = UARTCharGetNonBlocking(UART1_BASE);

    if(UART1Count>254)
    {
    	UART1Count = 0;
    }

}
