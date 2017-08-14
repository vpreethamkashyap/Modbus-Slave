

/**********************************************************************************************
						Main Function
** UART1 ******* USED FOR THE MAIN DEBUG TERMINAL 
** UART2 ******* USED FOR THE WEIGHT LEVEL 
** UART5 ******* USED FOR THE BLOOD PRESSURE
** UART6 ******* USED FOR THE BLOOD Level Impedence
 
**********************************************************************************************/

/**********************************************************************************************
				............Frequency Settings..........
**********************************************************************************************/


/**********************************************************************************************
						Declarations of Functions
**********************************************************************************************/

void DoBPCheck(void);
void DoBLICheck(void);
void GetBPResult(void);
void StartBMITest(void);
void StartBLITest(void);
void StartBPITest(void);
void DoWeightCheck(void);
void CheckAllUarts(void);
void DisplayMessage(void);
void PollMdbResponse(void);
void ConvertFloatToHex(char*);
void InitializeAllUart(void);
float ConvertCharToFloat(void);
void ReadFromSensor(unsigned char);

extern void InitSystem(void);
extern void InitUART0(void);
extern void InitUART1(void);

extern void WriteToUART0(const unsigned char);
extern void WriteToUART1(const unsigned char );


/**********************************************************************************************
						Declarations of Variables
**********************************************************************************************/
extern volatile unsigned char UART0Count;
extern volatile unsigned char UART0Buffer[30];
extern volatile unsigned char UART1Count;
extern volatile unsigned char UART1Buffer[30];
extern unsigned char DataReady;
extern unsigned char NeglectZeros;
extern unsigned char WeightData;
extern unsigned char TestState;
extern unsigned char WriteCoil[2];
extern unsigned char WriteHoldReg[10];
extern unsigned char MdbResponse[20];

float Weight;
short RegValue = 0;
unsigned char LowBp=0;
unsigned char HighBp=0;
unsigned char PulseRate=0;
unsigned char Test6En = 0;
unsigned char GetBpResult;
unsigned int DivisonFactor;
unsigned char CheckBli = 0;
unsigned char CheckBp = 0;
unsigned char CheckWeight =0;
volatile float NewIMP = 0;
volatile float NewRegValue;
unsigned char PointEn = 0,Chars =0;

unsigned char NewBuf[8];
unsigned char TxBuffer[10];
unsigned char TempBuffer[20];
const char Str[] = "UART 1 IS READY TO USE";
