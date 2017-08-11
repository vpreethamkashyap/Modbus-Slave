//###########################################################################
// FILE:   isqrt.h
// TITLE:  Prototype for the integer square root function.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

#ifndef __ISQRT_H__
#define __ISQRT_H__

//*****************************************************************************
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// The prototype for the integer square root function.
//*****************************************************************************
extern unsigned long isqrt(unsigned long ulValue);

//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif


