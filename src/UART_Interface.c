#include"HardwareProfile.h"


void InitSystem(void)
{
	// Disable Protection
	HWREG(0x400FB980) =  0xA5A5A5A5;
	SysCtlClockConfigSet(SYSCTL_USE_PLL | (SYSCTL_SPLLIMULT_M & 0xF) |
	            SYSCTL_SYSDIV_1 | SYSCTL_M3SSDIV_2 |
	            SYSCTL_XCLKDIV_4);
}

void InitMasterInterrupt(void){

	IntMasterEnable();

}

void InitUART0(void){

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	InitMasterInterrupt();
    GPIOPinConfigure(GPIO_PE4_U0RX);
    GPIOPinConfigure(GPIO_PE5_U0TX);
    GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(SYSTEM_CLOCK_SPEED), 9600,
        	                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
        	                         UART_CONFIG_PAR_NONE));
    // Enable the UART0 interrupt.
    IntRegister(INT_UART0, UART0IntHandler);
   IntEnable(INT_UART0);
   UARTIntEnable(UART0_BASE, UART_INT_RX);

}


void InitUART1(void){

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	InitMasterInterrupt();
    GPIOPinConfigure(GPIO_PD0_U1RX);
    GPIOPinConfigure(GPIO_PD1_U1TX);
    GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(SYSTEM_CLOCK_SPEED), 9600,
        	       	                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
        	       	                         UART_CONFIG_PAR_NONE));

    // Enable the UART0 interrupt.
     IntRegister(INT_UART1, UART1IntHandler);
     IntEnable(INT_UART1);
     UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_TX);
}

void InitLED2(void){

    // Set up the Pin for LED2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_7);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, ~0);

}

void InitLED3(void){

	// Set up the Pin for LED3
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6);
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, ~0);
}

void WriteToUART0(const unsigned char pucBuffer)
{
	// Write the next character to the UART.
	///UARTCharPut(UART0_BASE, pucBuffer);
}

extern unsigned int TX_cnt;
void WriteToUART1(const unsigned char pucBuffer)
{
	//UARTCharPut(UART1_BASE,pucBuffer);

	while(1)
	{
		if(UARTSpaceAvail(UART1_BASE))
		{
			UARTCharPut(UART1_BASE,pucBuffer);
			while(!TX_cnt);
			TX_cnt=0;
			return;
		}
	}
}


