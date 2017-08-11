//###########################################################################
// FILE:   device.h
// TITLE:  Device Specific Definitions
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

#ifndef __DEVICE_H__
#define __DEVICE_H__

//*****************************************************************************
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif


#define   TARGET                     1
//---------------------------------------------------------------------------
// User To Select Target Device:


#define   DSP28_F28M35Ex             0
#define   DSP28_F28M35Mx             0
#define   DSP28_F28M35Hx             TARGET


/*-----------------------------------------------------------------------------
      Target device determines CPU frequency
      (for examples) - either 100 MHz (for H devices) 75 MHz
      (for M devices), and 60 (for E devices). User does not have to 
      change anything here.
-----------------------------------------------------------------------------*/
#if DSP28_F28M35Mx                // F28M36Hx device only
  #define CPU_FRQ_60MHZ     0     // 75 MHz CPU Freq (20 MHz input freq)
  #define CPU_FRQ_75MHZ     1
#elif DSP28_F28M35Ex              // F28M36Px device only
  #define CPU_FRQ_60MHZ     1     // 60 MHz CPU Freq (20 MHz input freq)
  #define CPU_FRQ_75MHZ     0
#else                             // CUSTOM CONFIGURATION
  #define CPU_FRQ_60MHZ     0     // User defined Setup
  #define CPU_FRQ_75MHZ     1
  #define CPU_FRQ_100MHZ    0
  #endif

//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif // __DEVICE_H__


