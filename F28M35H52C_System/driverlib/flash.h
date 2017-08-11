// FILE:   flash.h
// TITLE:  Prototypes and macros for the flash module.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

#ifndef __FLASH_H__
#define __FLASH_H__

//*****************************************************************************
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// API Function prototypes
//*****************************************************************************
extern void FlashInit();
extern void FlashSetup(unsigned char ucFlashWaitState);
extern void FlashGainPump();
extern void FlashLeavePump();

//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  __FLASH_H__


