//###########################################################################
// FILE:   ram.h
// TITLE:  Defines and Macros for the IPC controller.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

#ifndef __RAM_H__
#define __RAM_H__

//*****************************************************************************
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// The following are values that can be passed to RamMReqSharedMemAccess() as 
// ulMask parameter to configure MSXMSEL register for master access to Sx
// RAM block.
//*****************************************************************************
#define S0_ACCESS                   0x00000001   // Master Access to S0
#define S1_ACCESS                   0x00000002   // Master Access to S1
#define S2_ACCESS                   0x00000004   // Master Access to S2
#define S3_ACCESS                   0x00000008   // Master Access to S3
#define S4_ACCESS                   0x00000010   // Master Access to S4
#define S5_ACCESS                   0x00000020   // Master Access to S5
#define S6_ACCESS                   0x00000040   // Master Access to S6
#define S7_ACCESS                   0x00000080   // Master Access to S7

//*****************************************************************************
// The following are values that can be passed to RamMReqSharedMemAccess() as 
// usMaster parameter to determine which processor to give master access to 
// Sx blocks.
//*****************************************************************************
#define SX_C28MASTER            0x0001
#define SX_M3MASTER             0x0000

//*****************************************************************************
// API Function prototypes
//*****************************************************************************
extern void RAMMReqSharedMemAccess (unsigned long ulMask, 
                                    unsigned short usMaster);
extern unsigned char RAMControlInitM1MsgRam(void);
extern unsigned char RAMControlInitL0L3Ram(void);

//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif //  __RAM_H__



