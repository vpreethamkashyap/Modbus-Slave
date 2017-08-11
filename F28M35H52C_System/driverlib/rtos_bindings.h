//#############################################################################
// FILE: rtos_bindings.h
// TITLE: Macros intended to aid porting of controlSUITE modules
//        for use with an RTOS.
//#############################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//#############################################################################
#ifndef RTOS_BINDINGS_H_
#define RTOS_BINDINGS_H_

#ifdef USE_RTOS
//*****************************************************************************
//
// If an RTOS is in use, implement a header file called "c2000_rtos.h"
// which contains RTOS-specific versions of each of the macros defined below
// and make sure it appears on the include path set when you build your
// project.
//
// Note that there is no default implementation of this header file included
// in controlSUITE - it is your responsibility to create it specifically for
// your RTOS.
//
//*****************************************************************************
#include "c2000_rtos.h"

#else
//*****************************************************************************
//
// When no RTOS is in use, the follow macros compile to either nothing or a
// minimal implementation that works in a bare-metal environment.
//
// Each of these macros must be redefined in c2000_rtos.h if you are using
// StellarisWare under an RTOS.
//
//*****************************************************************************

//*****************************************************************************
//
// A simple macro used to yield within polling loops.  In the default, non-RTOS
// implementation, this compiles to nothing.
//
//*****************************************************************************
#define OS_YIELD()

//*****************************************************************************
//
// A simple macro around the SysCtlDelay function.  The parameter is the number
// of 3 cycle loops to wait before returning (as for SysCtlDelay).  In an RTOS
// implementation, this could be replaced with an OS delay call with
// appropriate parameter scaling.
//
//*****************************************************************************
#define OS_DELAY(ul3Cycles) SysCtlDelay(ul3Cycles)

//*****************************************************************************
//
// Wrappers around low level interrupt control functions.  For information
// on each of these functions, please see the appropriate API documentation
// for the DriverLib Interrupt driver.
//
// The macros defined here represent interrupt-control functions that may be
// called from within StellarisWare code.  It is expected that application
// code will use RTOS-specific functions to control interrupt priority, to
// pend interrupts and to perform runtime vector manipulation.  As a result,
// no macros are defined to wrap any of these functions from interrupt.c.
//
//*****************************************************************************
#define OS_INT_MASTER_ENABLE() IntMasterEnable()
#define OS_INT_MASTER_DISABLE() IntMasterDisable()
#define OS_INT_DISABLE(ulIntID) IntDisable(ulIntID)
#define OS_INT_ENABLE(ulIntID) IntEnable(ulIntID)

#endif // USE_RTOS

#endif // RTOS_BINDINGS_H_
