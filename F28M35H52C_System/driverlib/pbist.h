//###########################################################################
// FILE:   pbist.h
// TITLE:  Defines and Macros for the PBIST controller.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

#ifndef PBIST_H_
#define PBIST_H_

#include "inc/hw_types.h"

//*****************************************************************************
// Value of PBIST_ALGO for the ALGO Groups
//*****************************************************************************
#define PBIST_ALGO_TR_G1					0x00000001	// 1 Triple Read: PBIST ROM
#define PBIST_ALGO_TR_G2					0x00000002	// 2 Triple Read: M3 BootROM
#define PBIST_ALGO_MARCH13_SPR_DC			0x00100000	// 21 March 13N: Single Port RAM Distributed Compare
#define PBIST_ALGO_MARCH13_TPR_DC			0x00200000	// 22 March 13N: Two Port RAM Distributed Compare
#define PBIST_ALGO_TRXORR_G3				0x00800000	// 24 Triple Read XOR Read: C28 Self Test ROM
#define PBIST_ALGO_TRXORR_G4				0x01000000	// 25 Triple Read XOR Read: C28 Self Test ROM1
#define PBIST_ALGO_TRXORR_G5				0x02000000	// 26 Triple Read XOR Read: M3 Self Test ROM2
#define PBIST_ALGO_TRXORR_G6				0x04000000	// 27 Triple Read XOR Read: C28 Self Test ROM2
#define PBIST_ALGO_TRXORR_G7				0x08000000	// 28 Triple Read XOR Read: M3 Self Test ROM
#define PBIST_ALGO_TRXORR_G8				0x10000000	// 29 Triple Read XOR Read: M3 Self Test ROM


#define PBIST_ALGO_DEFAULT					0x1FF00001	// Default Algorithm set to run on all Memories


//*****************************************************************************
// Values to select the specific memory groups in PBIST_RINFOL register
//*****************************************************************************
#define PBIST_RINFOL_PBIST_ROM				0x00000001	// 1 PBIST ROM Select
#define PBIST_RINFOL_M3_BOOTROM				0x00000002	// 2 M3 BootROM Select
#define PBIST_RINFOL_C28_B00TROM			0x00000004	// 3 C28 BootROM Select
#define PBIST_RINFOL_C28_SELF_TEST_ROM		0x00000008	// 4 C28 Self Test ROM Select
#define PBIST_RINFOL_C28_SELF_TEST_ROM1		0x00000010	// 5 C28 Self Test ROM1 Select
#define PBIST_RINFOL_M3_SELF_TEST_ROM2		0x00000020	// 6 M3 Self Test ROM2 Select
#define PBIST_RINFOL_C28_SELF_TEST_ROM2		0x00000040	// 7 C28 Self Test ROM2 Select
#define PBIST_RINFOL_M3_SELF_TEST_ROM		0x00000080	// 8 M3 Self Test ROM Select
#define PBIST_RINFOL_C28_M0_SP				0x00000100	// 9 C28_M0 Single Port
#define PBIST_RINFOL_C28_L0_SP				0x00000200	// 10
#define PBIST_RINFOL_C28_L1_SP				0x00000400	// 11
#define PBIST_RINFOL_C28_L2_EAB_SP			0x00000800	// 12
#define PBIST_RINFOL_C28_L2_OAB_SP			0x00001000	// 13
#define PBIST_RINFOL_C28_L3_EAB_SP			0x00002000	// 14
#define PBIST_RINFOL_C28_L3_OAB_SP			0x00004000	// 15
#define PBIST_RINFOL_C28_M1_SP				0x00008000	// 16
#define PBIST_RINFOL_DCAN_0_SP				0x00010000	// 17
#define PBIST_RINFOL_DCAN_1_SP				0x00020000	// 18
#define PBIST_RINFOL_DCAN_2_SP				0x00040000	// 19
#define PBIST_RINFOL_DCAN_3_SP				0x00080000	// 20
#define PBIST_RINFOL_DCAN_4_SP				0x00100000	// 21
#define PBIST_RINFOL_DCAN_5_SP				0x00200000	// 22
#define PBIST_RINFOL_M3_C0_SP				0x00400000	// 23
#define PBIST_RINFOL_M3_C1_SP				0x00800000	// 24
#define PBIST_RINFOL_M3_C2_EAB_SP			0x01000000	// 25
#define PBIST_RINFOL_M3_C2_OAB_SP			0x02000000	// 26
#define PBIST_RINFOL_M3_C3_EAB_SP			0x04000000	// 27
#define PBIST_RINFOL_M3_C3_OAB_SP			0x08000000	// 28
#define PBIST_RINFOL_MSG_M0_SP				0x10000000	// 29
#define PBIST_RINFOL_MSG_M1_SP				0x20000000	// 30
#define PBIST_RINFOL_PEII0_SP				0x40000000	// 31
#define PBIST_RINFOL_PIEI1_SP				0x80000000	// 32
#define PBIST_RINFOL_NONE					0x00000000	// Do not select any Memory defined by RINFOL

//*****************************************************************************
// Values to select the specific memory groups for PBISTMemorySelectU() Function
//*****************************************************************************
#define PBIST_RINFOU_NONE					0x00000000	// Do not select any Memory defined by RINFOU
#define PBIST_RINFOU_SHARED0_SP				0x00000001	// 33
#define PBIST_RINFOU_SHARED1_SP				0x00000002	// 34
#define PBIST_RINFOU_SHARED2_SP				0x00000004	// 35
#define PBIST_RINFOU_SHARED3_SP				0x00000008	// 36
#define PBIST_RINFOU_SHARED4_SP				0x00000010	// 37
#define PBIST_RINFOU_SHARED5_SP				0x00000020	// 38
#define PBIST_RINFOU_SHARED6_SP				0x00000040	// 39
#define PBIST_RINFOU_SHARED7_SP				0x00000080	// 40
#define PBIST_RINFOU_USB_SP					0x00000100	// 41
#define PBIST_RINFOU_EMAC_RX_TP				0x00000200	// 42
#define PBIST_RINFOU_EMAC_TX_TP				0x00000400	// 43
#define PBIST_RINFOU_DC_C28_L0_L1_SP		0x00000800	// 44
#define PBIST_RINFOU_DC_C28_L2_L3_SP		0x00001000	// 45
#define PBIST_RINFOU_DC_C28_M_MSG_0_1_SP	0x00002000	// 46
#define PBIST_RINFOU_DC_DCAN_0_5_SP			0x00004000	// 47
#define PBIST_RINFOU_DC_M3_C0_C1_SP			0x00008000	// 48
#define PBIST_RINFOU_DC_M3_C2_C3_SP			0x00010000	// 49
#define PBIST_RINFOU_DC_PIE_0_1				0x00020000	// 50
#define PBIST_RINFOU_DC_SHARED_0_3_SP		0x00040000	// 51
#define PBIST_RINFOU_DC_SHARED_4_7_SP		0x00080000	// 52
#define PBIST_RINFOU_DC_EMAC_TX_RX_TP		0x00100000	// 53

#define PBIST_INTERRUPT_FLAG_READ           (*(unsigned long *)0xE000E288) // PBIST Interrupt Flag
//*****************************************************************************
// Prototypes for the APIs.
//*****************************************************************************
void PBISTInit(void);
void PBISTDisable(void);
void PBISTAlgoSelect(unsigned long ulALGO);
void PBISTRinfoLSelect(unsigned long ulRINFOL);
void PBISTRinfoUSelect(unsigned long ulRINFOU);
void PBISTOverride(tBoolean bOVER);
void PBISTStart(void);
tBoolean PBISTFailureStatus(void);
void PBISTStop(void);
void PBISTResume(void);

#endif /* PBIST_H_ */
