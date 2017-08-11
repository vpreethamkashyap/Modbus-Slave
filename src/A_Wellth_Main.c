#include"HardwareProfile.h"
#include "Main.h"

void CallSystem(void);


/**********************************************************************************************
						Start Of Main Function
**********************************************************************************************/

int main(void){

	CallSystem();

	while (1){

		//PollMdbResponse();
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

	memset(UART1Buffer,0,255);
	UART1Count = 0;
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
