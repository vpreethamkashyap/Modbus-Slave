//###########################################################################
// FILE:   cpu_usage.h
// TITLE:  Prototypes for the CPU utilization routines.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

#ifndef __CPU_USAGE_H__
#define __CPU_USAGE_H__

//*****************************************************************************
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// Prototypes for the CPU utilization routines.
//*****************************************************************************
extern unsigned long CPUUsageTick(void);
extern void CPUUsageInit(unsigned long ulClockRate, unsigned long ulRate,
                         unsigned long ulTimer);

//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __CPU_USAGE_H__


