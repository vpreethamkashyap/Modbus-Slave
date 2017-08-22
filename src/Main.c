#include"HardwareProfile.h"
#include "Main.h"

void CallSystem(void);

extern unsigned int TX_cnt;
/**********************************************************************************************
						Start Of Main Function
**********************************************************************************************/


int main(void){

	CallSystem();

	/*WriteToUART1('A');
	WriteToUART1('B');
	WriteToUART1('C');
	WriteToUART1('D');
	WriteToUART1('E');*/
	int loopcnt = 0;

#if 0
	WriteToUART1((loopcnt+0x30));

	while(1)
	{
		if(TX_cnt)
		{
			TX_cnt=0;
			WriteToUART1((loopcnt+0x30));
			loopcnt++;
			if(loopcnt>9)
			{
				loopcnt=0;

			}
		}
	}
#endif

#if 0
	//Switch_OFF_UART_1_RX();
	//Switch_ON_UART_1_TX();
//	IntDisable(INT_UART1);
	//UARTIntDisable(UART1_BASE, UART_INT_RX);

	volatile int TimeDelay;
	while(1)
	{
		WriteToUART1((loopcnt+0x30));
		TimeDelay=900000;
		//while(TimeDelay--);



		loopcnt++;
		if(loopcnt>9)
		{
			loopcnt=0;
			//break;

		}
	}
#endif

	//Switch_OFF_UART_1_TX();

	while(1)
	{
/*		unsigned int status_fr = (HWREG(UART1_BASE + 0x00000008));
		unsigned int status_sr = (HWREG(UART1_BASE + 0x00000004));
		unsigned int status_dr = (HWREG(UART1_BASE + 0x00000000));*/
		//WriteToUART1('A');
		if(UART1Count >=8)
		{
			/*Switch_OFF_UART_1_RX();
			Switch_ON_UART_1_TX();
		    IntDisable(INT_UART1);
		    UARTIntDisable(UART1_BASE, UART_INT_RX);*/

			change_fifo_len_by_1();
			PollMdbResponse();
			change_fifo_len_by_def();
			/*Switch_OFF_UART_1_TX();
			Switch_ON_UART_1_RX();
			IntEnable(INT_UART1);
		    UARTIntEnable(UART1_BASE, UART_INT_RX); */
		}
	}

}

/**********************************************************************************************
						End Of Main Function
**********************************************************************************************/

void CallSystem(void){
	InitSystem();
	InitializeAllUart();
}

void InitializeAllUart(void){

	memset(UART1Buffer,0,30);
	memset(UART1Buffer,0,30);
	UART1Count = 0;

	UART0Count = 0;

	InitUART1();
	InitUART0();
}

void ConvertFloatToHex(char * ff){
	WriteHoldReg[1] = *ff;
	ff++;
	WriteHoldReg[0] = *ff;
	ff++;
	WriteHoldReg[3] = *ff;
	ff++;
	WriteHoldReg[2] = *ff;
}
float ConvertCharToFloat(void){
	short int i =0,j=0,Limit=20;
	PointEn = 0;
	Chars = 0;
	memset(NewBuf,0,8);

	for(i=0;i<8;i++){
		if((TempBuffer[i] != 0x00) &&(TempBuffer[i] != 0x0D)&&(TempBuffer[i] != 0x2B) ){
			if(TempBuffer[i] == 0x2E){
				PointEn = 1;
			}
			else{
				if(PointEn){
					Chars = Chars+ 1;
				}
				NewBuf[j] = TempBuffer[i];
				j++;
			}
		}
	}

	DivisonFactor = 1;
	while(Chars>0){
		DivisonFactor = DivisonFactor *10;
		Chars--;SysCtlClockConfigSet(SYSCTL_USE_PLL | (SYSCTL_SPLLIMULT_M & 0xF) |
	            SYSCTL_SYSDIV_1 | SYSCTL_M3SSDIV_2 |
	            SYSCTL_XCLKDIV_4);
	}
	sscanf(NewBuf,"%d",&RegValue);
	if(DivisonFactor>1){
		NewRegValue =  (float)RegValue / DivisonFactor;
	}
	else{
		NewRegValue =  (float)RegValue;
	}
	return NewRegValue;
}
