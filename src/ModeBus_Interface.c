#include "MDB.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include"HardwareProfile.h"

extern void CallSystem(void);
extern unsigned char UART0Count;
extern unsigned char UART0Buffer[255];
extern unsigned char UART1Count;
extern unsigned char UART1Buffer[255];
/***********************************************************************************************************
								Look ModeBus Response
***********************************************************************************************************/
void PollMdbResponse(void){	

	if(UART1Count >=7){

		switch(MdbRes){
			case 0:
				//................Get SLave ID...............................................
				if(UART1Buffer[0] == SLAVE_ID){
					MdbRes = 1;				
					MdbResponse[0] = UART1Buffer[0];
				}
				else{
					ClearUART1Buffer();
					break;
				}

			case 1:
				MdbResponse[0] = UART1Buffer[0];
				//................Get Function Code...............................................
				if((UART1Buffer[1] == FC_READ_COILS)||(UART1Buffer[1] == FC_READ_HOLD_REGS)||(UART1Buffer[1] == FC_WRITE_SINGLE_COIL)||(UART1Buffer[1] == FC_WRITE_SINGLE_REG)){
					MdbResponse[1] = UART1Buffer[1];
					MdbRes = 2;	
				}
				else{
					ClearUART1Buffer();
					break;
				}			

			case 2:
				//................Get Address Code...............................................
				MdbResponse[2] = UART1Buffer[2];
				MdbResponse[3] = UART1Buffer[3];//UART1Buffer[3];
				MdbRes = 3;	

			case 3:
				//................Get Input Data or No of Registers want to access.............
				MdbResponse[4] = UART1Buffer[4];//UART1Buffer[4];
				MdbResponse[5] = UART1Buffer[5];//UART1Buffer[5];
				MdbRes = 4;	

			case 4:
				//................Get CRC Bytes.................................................
				MdbResponse[6] = UART1Buffer[6];//UART1Buffer[6];
				MdbResponse[7] = UART1Buffer[7];//UART1Buffer[7];
				ComputeMdbResponse(MdbResponse[1]);
				MdbRes = 0;	

			break;
			default:
			break;
		}
	}
}

/***********************************************************************************************************
***********************************************************************************************************/

void ComputeMdbResponse(unsigned char FuncSel){
	if(CheckCRC()){
		switch(FuncSel){
			case FC_WRITE_SINGLE_COIL:
				FromWriteSingleCoil();
			break;
			case FC_READ_COILS:
				FromReadCoil();
			break;
			case FC_READ_HOLD_REGS:
				FromReadHoldingReg();
				while(1){
					MdbRes = 0;
					while(1);
				}
			break;
			case FC_WRITE_SINGLE_REG:
				FromWriteSingleReg();
			break;
		}
	}
	else{
		ClearUART1Buffer();
	}
}
/***********************************************************************************************************************************************
	# This Function is used when Master Want to Operate any Sensor Test
	# For Each Test Master has defined the particular Address
	# Slave looks Address of coil & Data value on that Coil
	# Slave Sends Back the Same Response Back to Master 

						Mster Request 				Slave Response		
	Function Name 		Write Single Coil			Write Single Coil
	Slave ID			0x01						0x01
	FunctionCode		0x05						0x05
	Address Of Coil	    0x000A,0x000B,0x000C		0x000A,0x000B,0x000C
	Data				0x0001						0x0001
	Error Check			CRC Bytes					CRC Bytes

************************************************************************************************************************************************/
void FromWriteSingleCoil(void){
	if(MdbResponse[2] == 0x00){
		if(MdbResponse[3] == 0x0A){
			if(MdbResponse[4] == 0x00){
				if(MdbResponse[5] == 0x01){
					SendMDBResponse(FC_WRITE_SINGLE_COIL);
				//	if(TestState ==0)
					{
							// Do BMI Test
						TestState = 1;
					}	
 					ClearUART1Buffer();
				}
			}			
		}
		else if(MdbResponse[3] == 0x0B){
			if(MdbResponse[4] == 0x00){
				if(MdbResponse[5] == 0x01){			
					SendMDBResponse(FC_WRITE_SINGLE_COIL);
					NeglectZeros = 0;
					//DoBPCheck();									// Do BP Test
					TestState = 2;
 					ClearUART1Buffer();
				}
			}				
		}
		else if(MdbResponse[3] == 0x0C){
			if(MdbResponse[4] == 0x00){
				if(MdbResponse[5] == 0x01){			
					SendMDBResponse(FC_WRITE_SINGLE_COIL);
					//DoBLICheck();									// Do BLI Test
					TestState = 3;
 					ClearUART1Buffer();
				}
			}				
		}
	}
}
/***********************************************************************************************************************************************
	# This Function is used when Master Want to know the current status of Sensor Test
	# For Each Test Master has defined the only one Address
	# Slave looks Address of coil & Data value on that Coil
	# Slave Sends Back the Response Back to Master If Sensor Test Completed then It will send 0x0001 
	  if still busy then it will send 0x0000

							Mster Request 				
	Function Name 			FC_READ_COILS				
	Slave ID				0x01						
	FunctionCode			0x01						
	Start Address Coil		0x000F						
	No Of Coils To Read		0x0001						
	Error Check				CRC Bytes					

							Slave Request 				
	Function Name 			FC_READ_COILS				
	Slave ID				0x01						
	FunctionCode			0x01						
	Byte Count 				0x01						
	OutPutStatus Of coil	0x0001 or 0x0000
 	Error Check				CRC Bytes					
   
************************************************************************************************************************************************/
void FromReadCoil(void){
	if(MdbResponse[3] == 0x0F){
		if(MdbResponse[4] == 0x00){
			if(MdbResponse[5] == 0x00){
				memset(MdbReq,0,20);
				MdbReq[0] = SLAVE_ID;
				MdbReq[1] = FC_READ_COILS;
				MdbReq[2] = 0x01;
				if(WriteCoil[0] == 0x00){
					MdbReq[3] = 0;
				}
				else{
					MdbReq[3] = WriteCoil[0];
				}
				CrcSize = 4;
				AppendCrc();
				SendMDBResponse(FC_READ_COILS);
				ClearUART1Buffer();
			}
		}
	}
}
/***********************************************************************************************************************************************
	# This Function is used when Master Want to Operate any Sensor Test
	# For Each Test Master has defined the particular Address
	# Slave looks Address of coil & Looks the Number of Regiters
	# Slave Sends Back the Response with Register Value
	# Register Value Contains the Result of Sensor

						Mster Request 				
	Function Name 		FC_READ_HOLD_REGS			
	Slave ID			0x01						
	FunctionCode		0x03						
	Address Of Coil	    0x000A,0x000B,0x000C		
	No Of Register		0x0001						
	Error Check			CRC Bytes	

						Slave Response 				
	Function Name 		FC_READ_HOLD_REGS			
	Slave ID			0x01						
	FunctionCode		0x03						
	Byte Count			0x01
	Register Value 		0x0001						
	Error Check			CRC Bytes
************************************************************************************************************************************************/
void FromReadHoldingReg(void){

	unsigned char buf[2];
	WriteCoil[0] = 0x00;
	WriteCoil[1] = 0x00;

	if(MdbResponse[3] == 0x0A){
	 	ConvertFloatToHex(&Weight);
	}
	else if(MdbResponse[3] == 0x0B){
		WriteHoldReg[0] = HighBp;
		WriteHoldReg[1] = LowBp;
		WriteHoldReg[2] = PulseRate;
	}
	else if(MdbResponse[3] == 0x0C){
	 	ConvertFloatToHex(&NewIMP);
	}
	
	buf[0] = MdbResponse[5];
	buf[1] = MdbResponse[4];
	NoOfBytes = buf[1]<<8;
	NoOfBytes = (buf[0] | NoOfBytes);

	if(NoOfBytes == 1){
		memset(MdbReq,0,20);
		MdbReq[0] = SLAVE_ID;
		MdbReq[1] = FC_READ_HOLD_REGS;
		MdbReq[2] = 0x02;
		MdbReq[3] = WriteHoldReg[0];
		MdbReq[4] = WriteHoldReg[1];
		CrcSize = 5;
		AppendCrc();
		SendMDBResponse(FC_READ_HOLD_REGS);
	}
	else if(NoOfBytes == 2){
		memset(MdbReq,0,20);
		MdbReq[0] = SLAVE_ID;
		MdbReq[1] = FC_READ_HOLD_REGS;
		MdbReq[2] = 0x04; 			
		MdbReq[3] = WriteHoldReg[0];
		MdbReq[4] = WriteHoldReg[1];
		MdbReq[5] = WriteHoldReg[2];
		MdbReq[6] = WriteHoldReg[3];
		CrcSize = 7;
		AppendCrc();
		SendMDBResponse(FC_READ_HOLD_REGS);
	}
	else if(NoOfBytes == 3){
		memset(MdbReq,0,20);
		MdbReq[0] = SLAVE_ID;
		MdbReq[1] = FC_READ_HOLD_REGS;
		MdbReq[2] = 0x06;
		MdbReq[3] = WriteHoldReg[0];
		MdbReq[4] = WriteHoldReg[1];
		MdbReq[5] = WriteHoldReg[2];
		MdbReq[6] = WriteHoldReg[3];
		MdbReq[7] = WriteHoldReg[4];
		MdbReq[8] = WriteHoldReg[5];
		CrcSize = 9;
		AppendCrc();
		SendMDBResponse(FC_READ_HOLD_REGS);
	}
}

/***********************************************************************************************************************************************
	# This Function is used when Master Want to Operate any Sensor Test
	# For Each Test Master has defined the particular Address
	# Slave looks Address of coil & Data value on that Coil
	# Slave Sends Back the Same Response Back to Master 

						Mster Request 				
	Function Name 		FC_WRITE_SINGLE_REG			
	Slave ID			0x01						
	FunctionCode		0x06						
	Address Of Coil	    0x000A,0x000B,0x000C		
	Register Value		0x0001						
	Error Check			CRC Bytes	

						Slave Response 				
	Function Name 		FC_READ_HOLD_REGS			
	Slave ID			0x01						
	FunctionCode		0x06						
	Address Of Coil	    0x000A,0x000B,0x000C		
	Register Value		0x0001						
	Error Check			CRC Bytes
************************************************************************************************************************************************/
void FromWriteSingleReg(void){
	if(MdbResponse[2] == 0x00){
		if(MdbResponse[3] == 0x0A){
			SendMDBResponse(FC_WRITE_SINGLE_REG);
			ClearUART1Buffer();
		}
		else if(MdbResponse[3] == 0x0B){
			SendMDBResponse(FC_WRITE_SINGLE_REG);		
			ClearUART1Buffer();
		}
		else if(MdbResponse[3] == 0x0C){
			SendMDBResponse(FC_WRITE_SINGLE_REG);
			ClearUART1Buffer();
		}
		else if(MdbResponse[3] == 0x1C){
			SendMDBResponse(FC_WRITE_SINGLE_REG);
			ClearUART1Buffer();
		}
	}
}

/************************************************************************************************************************************************
										Mdb Request
************************************************************************************************************************************************/
void SendMDBResponse(unsigned char FuncSel){
	int i=0;

	if(TestState==0){
		//ClearUART1Buffer();
		//RS485_TXEN = 1;
		switch(FuncSel){
			case FC_WRITE_SINGLE_COIL:
			case FC_WRITE_SINGLE_REG:
				for(i=0;i<=7;i++){
					MdbReq[i] = MdbResponse[i];
					TxDelay = 80000;
					WriteToUART1(MdbReq[i]);
					while(TxDelay>0){
						TxDelay--;
					}	
				}
				MdbReq[8] = 0;			
			break;
			case FC_READ_COILS:
				for(i=0;i<6;i++){
					WriteToUART1(MdbReq[i]);
					TxDelay = 80000;
					while(TxDelay>0){
						TxDelay--;
					}
				}
				MdbReq[6] = 0;
			break;
			case FC_READ_HOLD_REGS:
				if(NoOfBytes == 1){
					for(i=0;i<=6;i++){
						TxDelay = 80000;
						WriteToUART1(MdbReq[i]);
						while(TxDelay>0){
							TxDelay--;
						}
					}
				}
				else if(NoOfBytes == 2){
					for(i=0;i<=8;i++){
						TxDelay = 80000;
						while(TxDelay>0){
							TxDelay--;
						}
						WriteToUART1(MdbReq[i]);
					}					
				}
				else if(NoOfBytes == 3){
					for(i=0;i<=11;i++){
						TxDelay = 80000;
						while(TxDelay>0){
							TxDelay--;
						}
						WriteToUART1(MdbReq[i]);
					}
				}
				WriteToUART1(MdbReq);
			break;
			default:
			break;
		}
		//RS485_TXEN = 0;
	}
}

/************************************************************************************************************************************************
										At Mdb Response Check CRC
************************************************************************************************************************************************/
unsigned char CheckCRC(void){	
	unsigned char Index = 0x00;			
	unsigned char Ctr   = 0x00;
	unsigned char TotalBytes = 6; 		
	
	CRCHi = 0xFF;
	CRCLo = 0xFF;

	for(Ctr=0;Ctr<TotalBytes;Ctr++){ 	
		Index = CRCLo ^ (MdbResponse[Ctr]);
		CRCLo = CRCHi ^ CRCHigh[Index];
		CRCHi = CRCLow[Index];
	}
	if((CRCLo == MdbResponse[TotalBytes]) && (CRCHi == MdbResponse[TotalBytes+1])){
		return TRUE;
	}
	return FALSE;
}

/************************************************************************************************************************************************
										At Mdb Request Append CRC
************************************************************************************************************************************************/
void AppendCrc(void){
	unsigned char CRCHi = 0xFF;	
	unsigned char CRCLo = 0xFF;	
	unsigned int Index;			
	unsigned char ctr;

	for(ctr=0; ctr<CrcSize; ctr++){
		Index = CRCLo ^ MdbReq[ctr];
		CRCLo = CRCHi ^ CRCHigh[Index];
		CRCHi = CRCLow[Index];
	}
	MdbReq[CrcSize] = CRCLo;
	MdbReq[CrcSize+1] = CRCHi;
}

void ClearUART1Buffer(void){
	memset(UART1Buffer,0,255);
	memset(MdbResponse,0,20);
	memset(MdbReq,0,20);
	UART1Count = 0;
}

void CallTxDelay(int TxDelay){

}
