#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include"HardwareProfile.h"

/**********************************************************************************************
						Variables Declarations 
**********************************************************************************************/
volatile unsigned char UART0Count=0;
volatile unsigned char UART0Buffer[30];
volatile unsigned char UART1Count=0;
volatile unsigned char UART1Buffer[30];
volatile unsigned char NeglectZeros = 0;
volatile unsigned char DataReady;



void UART0IntHandler(void)
{

    unsigned long ulStatus;

    // Get the interrupt status.
    ulStatus = UARTIntStatus(UART0_BASE, true);

    // Clear the asserted interrupts.
    UARTIntClear(UART0_BASE, ulStatus);

    UART0Buffer[UART0Count++]  = UARTCharGet(UART0_BASE);


}

unsigned int TX_cnt=0;
void UART1IntHandler(void)
{
    unsigned long ulStatus;

    ulStatus = UARTIntStatus(UART1_BASE, true);


    if(0x10 & ulStatus)
    {
        UART1Buffer[UART1Count++]  = UARTCharGet(UART1_BASE);

        if(UART1Count>30)
        {
        	UART1Count = 0;
        }
        TX_cnt = 0;
    }

    if(0x20 & ulStatus)
    {
    	TX_cnt=1;
    }

    UARTIntClear(UART1_BASE, ulStatus);

}
