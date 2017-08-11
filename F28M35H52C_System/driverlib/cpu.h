//###########################################################################
// FILE:   cpu.h
// TITLE:  Prototypes for the CPU instruction wrapper functions.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

#ifndef __CPU_H__
#define __CPU_H__

//*****************************************************************************
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// Prototypes.
//*****************************************************************************
extern unsigned long CPUcpsid(void);
extern unsigned long CPUcpsie(void);
extern unsigned long CPUprimask(void);
extern void CPUwfi(void);
extern unsigned long CPUbasepriGet(void);
extern void CPUbasepriSet(unsigned long ulNewBasepri);

//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __CPU_H__



