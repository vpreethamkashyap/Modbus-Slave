//###########################################################################
// FILE:   cpu.c
// TITLE:  Instruction wrappers for special CPU instructions needed by the
//         drivers.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

#include "driverlib/cpu.h"

//*****************************************************************************
// Wrapper function for the CPSID instruction.  Returns the state of PRIMASK
// on entry.
//*****************************************************************************
unsigned long
CPUcpsid(void)
{
    // Read PRIMASK and disable interrupts.
    __asm("    mrs     r0, PRIMASK\n"
          "    cpsid   i\n"
          "    bx      lr\n");

    // The following keeps the compiler happy, because it wants to see a
    // return value from this function.  It will generate code to return
    // a zero.  However, the real return is the "bx lr" above, so the
    // return(0) is never executed and the function returns with the value
    // you expect in R0.
    return(0);
}

//*****************************************************************************
// Wrapper function returning the state of PRIMASK (indicating whether
// interrupts are enabled or disabled).
//*****************************************************************************
unsigned long
CPUprimask(void)
{
    // Read PRIMASK and disable interrupts.
    __asm("    mrs     r0, PRIMASK\n"
          "    bx      lr\n");

    // The following keeps the compiler happy, because it wants to see a
    // return value from this function.  It will generate code to return
    // a zero.  However, the real return is the "bx lr" above, so the
    // return(0) is never executed and the function returns with the value
    // you expect in R0.
    return(0);
}

//*****************************************************************************
// Wrapper function for the CPSIE instruction.  Returns the state of PRIMASK
// on entry.
//*****************************************************************************
unsigned long
CPUcpsie(void)
{
    // Read PRIMASK and enable interrupts.
    __asm("    mrs     r0, PRIMASK\n"
          "    cpsie   i\n"
          "    bx      lr\n");

    // The following keeps the compiler happy, because it wants to see a
    // return value from this function.  It will generate code to return
    // a zero.  However, the real return is the "bx lr" above, so the
    // return(0) is never executed and the function returns with the value
    // you expect in R0.
    return(0);
}

//*****************************************************************************
// Wrapper function for the WFI instruction.
//*****************************************************************************
void
CPUwfi(void)
{
    // Wait for the next interrupt.
    __asm("    wfi\n");
}

//*****************************************************************************
// Wrapper function for writing the BASEPRI register.
//*****************************************************************************
void
CPUbasepriSet(unsigned long ulNewBasepri)
{
    // Set the BASEPRI register
    __asm("    msr     BASEPRI, r0\n");
}

//*****************************************************************************
// Wrapper function for reading the BASEPRI register.
//*****************************************************************************
unsigned long
CPUbasepriGet(void)
{
    // Read BASEPRI
    __asm("    mrs     r0, BASEPRI\n"
          "    bx      lr\n");

    // The following keeps the compiler happy, because it wants to see a
    // return value from this function.  It will generate code to return
    // a zero.  However, the real return is the "bx lr" above, so the
    // return(0) is never executed and the function returns with the value
    // you expect in R0.
    return(0);
}




