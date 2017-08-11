//###########################################################################
// FILE:   can.c
// TITLE:  Driver for the CAN module.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

//*****************************************************************************
//! \addtogroup can_api
//! @{
//*****************************************************************************

#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_sysctl.h"
#include "driverlib/can.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"

//*****************************************************************************
// This is the maximum number that can be stored as an 11bit Message
// identifier.
//*****************************************************************************
#define CAN_MAX_11BIT_MSG_ID    (0x7ff)

//*****************************************************************************
// This is used as the loop delay for accessing the CAN controller registers.
//*****************************************************************************

// The maximum CAN bit timing divisor is 13.
#define CAN_MAX_BIT_DIVISOR     (13)

// The minimum CAN bit timing divisor is 5.
#define CAN_MIN_BIT_DIVISOR     (5)

// The maximum CAN pre-divisor is 1024.
#define CAN_MAX_PRE_DIVISOR     (1024)

// The minimum CAN pre-divisor is 1.
#define CAN_MIN_PRE_DIVISOR     (1)

//*****************************************************************************
// This table is used by the CANBitRateSet() API as the register defaults for
// the bit timing values.
//*****************************************************************************

static const unsigned short g_usCANBitValues[] =
{
    0x1100, // TSEG2 2, TSEG1 2, SJW 1, Divide 5
    0x1200, // TSEG2 2, TSEG1 3, SJW 1, Divide 6
    0x2240, // TSEG2 3, TSEG1 3, SJW 2, Divide 7
    0x2340, // TSEG2 3, TSEG1 4, SJW 2, Divide 8
    0x3340, // TSEG2 4, TSEG1 4, SJW 2, Divide 9
    0x3440, // TSEG2 4, TSEG1 5, SJW 2, Divide 10
    0x3540, // TSEG2 4, TSEG1 6, SJW 2, Divide 11
    0x3640, // TSEG2 4, TSEG1 7, SJW 2, Divide 12
    0x3740  // TSEG2 4, TSEG1 8, SJW 2, Divide 13
};

//*****************************************************************************
//! \internal
//! Checks a CAN base address.
//!
//! \param ulBase is the base address of the CAN controller.
//!
//! This function determines if a CAN controller base address is valid.
//!
//! \return Returns \b true if the base address is valid and \b false
//! otherwise.
//
//*****************************************************************************
#ifdef DEBUG
static tBoolean
CANBaseValid(unsigned long ulBase)
{
    return((ulBase == CAN0_BASE) || (ulBase == CAN1_BASE));
}

#endif

//*****************************************************************************
//! \internal
//!
//! Returns the CAN controller interrupt number.
//!
//! \param ulBase is the base address of the selected CAN controller
//! \param ucNumber is the interrupt line number requested, valid values are 0
//! or 1
//!
//! Given a CAN controller base address and interrupt line number, returns the
//! corresponding interrupt number.
//!
//! \return Returns a CAN interrupt number, or -1 if \e ulPort is invalid.
//
//*****************************************************************************
static long
CANIntNumberGet(unsigned long ulBase, unsigned char ucNumber)
{
    long lIntNumber;

    // Return the interrupt number for the given CAN controller.
    switch(ulBase)
    {
    // Return the interrupt number for CAN 0
    case CAN0_BASE:
    {
        switch(ucNumber)
        {
        case 0:
        {
            lIntNumber = INT_CAN0INT0;
            break;
        }
        case 1:
        {
            lIntNumber = INT_CAN0INT1;
            break;

        }
        default:
        {
            lIntNumber = -1;
            break;
        }
        }
        break;
    }

    // Return the interrupt number for CAN 1
    case CAN1_BASE:
    {
        switch(ucNumber)
        {
        case 0:
        {
            lIntNumber = INT_CAN1INT0;
            break;
        }
        case 1:
        {
            lIntNumber = INT_CAN1INT1;
            break;

        }
        default:
        {
            lIntNumber = -1;
            break;
        }
        }
    }

    // Return -1 to indicate a bad address was passed in.
    default:
    {
        lIntNumber = -1;
    }
    }
    return(lIntNumber);
}

//*****************************************************************************
//! \internal
//!
//! Copies data from a buffer to the CAN Data registers.
//!
//! \param pucData is a pointer to the data to be written out to the CAN
//! controller's data registers.
//! \param pulRegister is an unsigned long pointer to the first register of the
//! CAN controller's data registers.  For example, in order to use the IF1
//! register set on CAN controller 0, the value would be: \b CAN0_BASE \b +
//! \b CAN_O_IF1DA1.
//! \param iSize is the number of bytes to copy into the CAN controller.
//!
//! This function takes the steps necessary to copy data from a contiguous
//! buffer in memory into the non-contiguous data registers used by the CAN
//! controller.  This function is rarely used outside of the CANMessageSet()
//! function.
//!
//! This function replaces the original CANWriteDataReg() API and performs the
//! same actions.  A macro is provided in <tt>can.h</tt> to map the original
//! API to this API.
//!
//! \return None.
//
//*****************************************************************************
static void
CANDataRegWrite(unsigned char *pucData, unsigned long *pulRegister, int iSize)
{
    int iIdx;
    unsigned char * pucRegister = (unsigned char *) pulRegister;

    // Loop always copies 1 or 2 bytes per iteration.
    for(iIdx = 0; iIdx < iSize; iIdx++ )
    {

        // Write out the data 8 bits at a time.
        HWREGB(pucRegister++) = pucData[iIdx];

    }
}

//*****************************************************************************
//! \internal
//!
//! Copies data from a buffer to the CAN Data registers.
//!
//! \param pucData is a pointer to the location to store the data read from the
//! CAN controller's data registers.
//! \param pulRegister is an unsigned long pointer to the first register of the
//! CAN controller's data registers.  For example, in order to use the IF1
//! register set on CAN controller 1, the value would be: \b CAN0_BASE \b +
//! \b CAN_O_IF1DA1.
//! \param iSize is the number of bytes to copy from the CAN controller.
//!
//! This function takes the steps necessary to copy data to a contiguous buffer
//! in memory from the non-contiguous data registers used by the CAN
//! controller.  This function is rarely used outside of the CANMessageGet()
//! function.
//!
//! This function replaces the original CANReadDataReg() API and performs the
//! same actions.  A macro is provided in <tt>can.h</tt> to map the original
//! API to this API.
//!
//! \return None.
//
//*****************************************************************************
static void
CANDataRegRead(unsigned char *pucData, unsigned long *pulRegister, int iSize)
{
    int iIdx;
    unsigned char * pucRegister = (unsigned char *) pulRegister;

    // Loop always copies 1 or 2 bytes per iteration.
    for(iIdx = 0; iIdx < iSize; iIdx++ )
    {
        // Read out the data
        pucData[iIdx] = HWREGB(pucRegister++);

    }
}

//*****************************************************************************
//! Initializes the CAN controller after reset.
//!
//! \param ulBase is the base address of the CAN controller.
//!
//! After reset, the CAN controller is left in the disabled state.  However,
//! the memory used for message objects contains undefined values and must be
//! cleared prior to enabling the CAN controller the first time.  This prevents
//! unwanted transmission or reception of data before the message objects are
//! configured.  This function must be called before enabling the controller
//! the first time.
//!
//! \return None.
//
//*****************************************************************************
void
CANInit(unsigned long ulBase)
{
    int iMsg;

    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));

    // Place CAN controller in init state, regardless of previous state.  This
    // will put controller in idle, and allow the message object RAM to be
    // programmed.

    HWREG(ulBase + CAN_O_CTL) = CAN_CTL_INIT;
    HWREG(ulBase + CAN_O_CTL) =  CAN_CTL_SWR;

    // Wait for busy bit to clear
    while(HWREG(ulBase + CAN_O_IF1_CMD) & CAN_IF1_CMD_BUSY)
    {
    }

    // Clear the message value bit in the arbitration register.  This indicates
    // the message is not valid and is a "safe" condition to leave the message
    // object.  The same arb reg is used to program all the message objects.
    HWREGH(ulBase + CAN_O_IF1_CMD + 2) = (CAN_IF1_CMD_WR_RD | CAN_IF1_CMD_ARB |
                                          CAN_IF1_CMD_CONTROL) >> 16;
    HWREG(ulBase + CAN_O_IF1_ARB) = 0;
    HWREG(ulBase + CAN_O_IF1_MCTL) = 0;

    HWREGH(ulBase + CAN_O_IF2_CMD + 2) = (CAN_IF2_CMD_WR_RD | CAN_IF2_CMD_ARB |
                                          CAN_IF1_CMD_CONTROL) >> 16;
    HWREG(ulBase + CAN_O_IF2_ARB) = 0;
    HWREG(ulBase + CAN_O_IF2_MCTL) = 0;

    // Loop through to program all 32 message objects
    for(iMsg = 1; iMsg <= 32; iMsg+=2)
    {
        // Wait for busy bit to clear
        while(HWREG(ulBase + CAN_O_IF1_CMD) & CAN_IF1_CMD_BUSY)
        {
        }

        // Initiate programming the message object
        HWREGH(ulBase + CAN_O_IF1_CMD) = iMsg;

        // Wait for busy bit to clear
        while(HWREG(ulBase + CAN_O_IF2_CMD) & CAN_IF2_CMD_BUSY)
        {
        }

        // Initiate programming the message object
        HWREGH(ulBase + CAN_O_IF2_CMD) = iMsg+1;
    }

    // Make sure that the interrupt and new data flags are updated for the
    // message objects.
    HWREGH(ulBase + CAN_O_IF1_CMD + 2) = (CAN_IF1_CMD_TXRQSTNDAT |
                                          CAN_IF1_CMD_CLRINTPND) >> 16;
    HWREGH(ulBase + CAN_O_IF2_CMD + 2) = (CAN_IF2_CMD_TXRQSTNDAT |
                                          CAN_IF2_CMD_CLRINTPND) >> 16;

    // Loop through to program all 32 message objects
    for(iMsg = 1; iMsg <= 32; iMsg+=2)
    {
        // Wait for busy bit to clear.
        while(HWREG(ulBase + CAN_O_IF1_CMD) & CAN_IF1_CMD_BUSY)
        {
        }

        // Initiate programming the message object
        HWREGH(ulBase + CAN_O_IF1_CMD) = iMsg;

        // Wait for busy bit to clear.
        while(HWREG(ulBase + CAN_O_IF2_CMD) & CAN_IF2_CMD_BUSY)
        {
        }

        // Initiate programming the message object
        HWREGH(ulBase + CAN_O_IF2_CMD) = iMsg+1;

    }

    // Acknowledge any pending status interrupts.
    HWREG(ulBase + CAN_O_ES);

}

//*****************************************************************************
//! Enables the CAN controller.
//!
//! \param ulBase is the base address of the CAN controller to enable.
//!
//! Enables the CAN controller for message processing.  Once enabled, the
//! controller will automatically transmit any pending frames, and process any
//! received frames.  The controller can be stopped by calling CANDisable().
//! Prior to calling CANEnable(), CANInit() should have been called to
//! initialize the controller and the CAN bus clock should be configured by
//! calling CANBitTimingSet().
//!
//! \return None.
//
//*****************************************************************************
void
CANEnable(unsigned long ulBase)
{
    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));

    // Clear the init bit in the control register.
    HWREG(ulBase + CAN_O_CTL) =
        HWREG(ulBase + CAN_O_CTL) & ~CAN_CTL_INIT;
}

//*****************************************************************************
//! Disables the CAN controller.
//!
//! \param ulBase is the base address of the CAN controller to disable.
//!
//! Disables the CAN controller for message processing.  When disabled, the
//! controller will no longer automatically process data on the CAN bus.  The
//! controller can be restarted by calling CANEnable().  The state of the CAN
//! controller and the message objects in the controller are left as they were
//! before this call was made.
//!
//! \return None.
//
//*****************************************************************************
void
CANDisable(unsigned long ulBase)
{
    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));

    // Set the init bit in the control register.

    HWREG(ulBase + CAN_O_CTL) =
        HWREG(ulBase + CAN_O_CTL) | CAN_CTL_INIT;
}

//*****************************************************************************
//! Select CAN peripheral clock source
//!
//! \param ulBase is the base address of the CAN controller to disable.
//! \param ucSource is the clock source to select for the given CAN
//!        peripheral: \n
//!        0 - M3/Master Subsystem Clock (default at reset) \n
//!        1 - Oscillator Clock (direct from X1/X2) \n
//!        2 - XClockIn
//!
//! Selects the desired clock source for use with a given CAN peripheral.
//!
//! \return None.
//
//*****************************************************************************
void
CANClkSourceSelect(unsigned long ulBase, unsigned char ucSource)
{

    unsigned long ulMWRAllow;

    ulMWRAllow = HWREG(SYSCTL_MWRALLOW);
    HWREG(SYSCTL_MWRALLOW) =  0xA5A5A5A5;

    switch(ulBase)
    {
    case CAN0_BASE:
    {
        HWREG(SYSCTL_CAN0BCLKSEL) = ucSource;
    }
    case CAN1_BASE:
    {
        HWREG(SYSCTL_CAN1BCLKSEL) = ucSource;
    }
    default:
        break;

    }

    HWREG(SYSCTL_MWRALLOW) = ulMWRAllow;
}

//*****************************************************************************
//! Reads the current settings for the CAN controller bit timing.
//!
//! \param ulBase is the base address of the CAN controller.
//! \param pClkParms is a pointer to a structure to hold the timing parameters.
//!
//! This function reads the current configuration of the CAN controller bit
//! clock timing, and stores the resulting information in the structure
//! supplied by the caller.  Refer to CANBitTimingSet() for the meaning of the
//! values that are returned in the structure pointed to by \e pClkParms.
//!
//! This function replaces the original CANGetBitTiming() API and performs the
//! same actions.  A macro is provided in <tt>can.h</tt> to map the original
//! API to this API.
//!
//! \return None.
//
//*****************************************************************************
void
CANBitTimingGet(unsigned long ulBase, tCANBitClkParms *pClkParms)
{

    unsigned long uBitReg;
    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));
    ASSERT(pClkParms != 0);

    uBitReg = HWREG(ulBase+CAN_O_BTR);

    // Set the phase 2 segment.
    pClkParms->uPhase2Seg = ((uBitReg & CAN_BTR_TSEG2_M) >> 12) + 1;

    // Set the phase 1 segment.
    pClkParms->uSyncPropPhase1Seg = ((uBitReg & CAN_BTR_TSEG1_M) >> 8) + 1;

    // Set the synchronous jump width.
    pClkParms->uSJW = ((uBitReg & CAN_BTR_SJW_M) >> 6) + 1;

    // Set the pre-divider for the CAN bus bit clock.
    pClkParms->uQuantumPrescaler =
        ((uBitReg & CAN_BTR_BRP_M) |
         ((uBitReg & CAN_BTR_BRPE_M) >> 10)) + 1;
}

//*****************************************************************************
//! This function is used to set the CAN bit timing values to a nominal setting
//! based on a desired bit rate.
//!
//! \param ulBase is the base address of the CAN controller.
//! \param ulSourceClock is the clock frequency for the CAN peripheral in Hz.
//! \param ulBitRate is the desired bit rate.
//!
//! This function will set the CAN bit timing for the bit rate passed in the
//! \e ulBitRate parameter based on the \e ulSourceClock parameter.  The CAN
//! bit clock is calculated to be an average timing value that should work for
//! most systems.  If tighter timing requirements are needed, then the
//! CANBitTimingSet() function is available for full customization of all of
//! the CAN bit timing values.  Since not all bit rates can be matched
//! exactly, the bit rate is set to the value closest to the desired bit rate
//! without being higher than the \e ulBitRate value.
//!
//! \return This function returns the bit rate that the CAN controller was
//! configured to use or it returns 0 to indicate that the bit rate was not
//! changed because the requested bit rate was not valid.
//
//*****************************************************************************
unsigned long
CANBitRateSet(unsigned long ulBase, unsigned long ulSourceClock,
              unsigned long ulBitRate)
{
    unsigned long ulDesiredRatio;
    unsigned long ulCANBits;
    unsigned long ulPreDivide;
    unsigned long ulRegValue;
    unsigned short usCANCTL;

    ASSERT(ulBitRate != 0);

    // Calculate the desired clock rate.
    ulDesiredRatio = ulSourceClock / ulBitRate;

    // If the ratio of CAN bit rate to processor clock is too small or too
    // large then return 0 indicating that no bit rate was set.
    ASSERT(ulDesiredRatio <= (CAN_MAX_PRE_DIVISOR * CAN_MAX_BIT_DIVISOR));
    ASSERT(ulDesiredRatio >= (CAN_MIN_PRE_DIVISOR * CAN_MIN_BIT_DIVISOR));

    // Make sure that the Desired Ratio is not too large.  This enforces the
    // requirement that the bit rate is larger than requested.
    if((ulSourceClock / ulDesiredRatio) > ulBitRate)
    {
        ulDesiredRatio += 1;
    }

    // Check all possible values to find a matching value.
    while(ulDesiredRatio <= CAN_MAX_PRE_DIVISOR * CAN_MAX_BIT_DIVISOR)
    {
        // Loop through all possible CAN bit divisors.
        for(ulCANBits = CAN_MAX_BIT_DIVISOR; ulCANBits >= CAN_MIN_BIT_DIVISOR;
            ulCANBits--)
        {
            // For a given CAN bit divisor save the pre divisor.
            ulPreDivide = ulDesiredRatio / ulCANBits;

            // If the calculated divisors match the desired clock ratio then
            // return these bit rate and set the CAN bit timing.
            if((ulPreDivide * ulCANBits) == ulDesiredRatio)
            {
                // Start building the bit timing value by adding the bit timing
                // in time quanta.
                ulRegValue = g_usCANBitValues[ulCANBits - CAN_MIN_BIT_DIVISOR];

                // To set the bit timing register, the controller must be
                // placed
                // in init mode (if not already), and also configuration change
                // bit enabled.  The state of the register should be saved
                // so it can be restored.

                usCANCTL = HWREG(ulBase + CAN_O_CTL);
                HWREG(ulBase + CAN_O_CTL) = usCANCTL | CAN_CTL_INIT |
                                            CAN_CTL_CCE;

                // Now add in the pre-scalar on the bit rate.
                ulRegValue |=
                    ((ulPreDivide - 1) & CAN_BTR_BRP_M) |
                    (((ulPreDivide - 1) << 10) & CAN_BTR_BRPE_M) ;

                // Set the clock bits in the and the bits of the
                // pre-scalar.
                HWREG(ulBase + CAN_O_BTR) = ulRegValue;

                // Restore the saved CAN Control register.
                HWREG(ulBase + CAN_O_CTL) = usCANCTL;

                // Return the computed bit rate.
                return(ulSourceClock / ( ulPreDivide * ulCANBits));
            }
        }

        // Move the divisor up one and look again.  Only in rare cases are
        // more than 2 loops required to find the value.
        ulDesiredRatio++;
    }
    return(0);
}

//*****************************************************************************
//! Configures the CAN controller bit timing.
//!
//! \param ulBase is the base address of the CAN controller.
//! \param pClkParms points to the structure with the clock parameters.
//!
//! Configures the various timing parameters for the CAN bus bit timing:
//! Propagation segment, Phase Buffer 1 segment, Phase Buffer 2 segment, and
//! the Synchronization Jump Width.  The values for Propagation and Phase
//! Buffer 1 segments are derived from the combination
//! \e pClkParms->uSyncPropPhase1Seg parameter.  Phase Buffer 2 is determined
//! from the \e pClkParms->uPhase2Seg parameter.  These two parameters, along
//! with \e pClkParms->uSJW are based in units of bit time quanta.  The actual
//! quantum time is determined by the \e pClkParms->uQuantumPrescaler value,
//! which specifies the divisor for the CAN module clock.
//!
//! The total bit time, in quanta, will be the sum of the two Seg parameters,
//! as follows:
//!
//! bit_time_q = uSyncPropPhase1Seg + uPhase2Seg + 1
//!
//! Note that the Sync_Seg is always one quantum in duration, and will be added
//! to derive the correct duration of Prop_Seg and Phase1_Seg.
//!
//! The equation to determine the actual bit rate is as follows:
//!
//! CAN Clock /
//! ((\e uSyncPropPhase1Seg + \e uPhase2Seg + 1) * (\e uQuantumPrescaler))
//!
//! This means that with \e uSyncPropPhase1Seg = 4, \e uPhase2Seg = 1,
//! \e uQuantumPrescaler = 2 and an 8 MHz CAN clock, that the bit rate will be
//! (8 MHz) / ((5 + 2 + 1) * 2) or 500 Kbit/sec.
//!
//! \return None.
//
//*****************************************************************************
void
CANBitTimingSet(unsigned long ulBase, tCANBitClkParms *pClkParms)
{
    unsigned int uBitReg;
    unsigned int uSavedInit;

    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));
    ASSERT(pClkParms != 0);

    // The phase 1 segment must be in the range from 2 to 16.
    ASSERT((pClkParms->uSyncPropPhase1Seg >= 2) &&
           (pClkParms->uSyncPropPhase1Seg <= 16));

    // The phase 2 segment must be in the range from 1 to 8.
    ASSERT((pClkParms->uPhase2Seg >= 1) && (pClkParms->uPhase2Seg <= 8));

    // The synchronous jump windows must be in the range from 1 to 4.
    ASSERT((pClkParms->uSJW >= 1) && (pClkParms->uSJW <= 4));

    // The CAN clock pre-divider must be in the range from 1 to 1024.
    ASSERT((pClkParms->uQuantumPrescaler <= 1024) &&
           (pClkParms->uQuantumPrescaler >= 1));

    // To set the bit timing register, the controller must be placed in init
    // mode (if not already), and also configuration change bit enabled.  State
    // of the init bit should be saved so it can be restored at the end.
    uSavedInit = HWREG(ulBase + CAN_O_CTL);
    HWREG(ulBase + CAN_O_CTL) = uSavedInit | CAN_CTL_INIT | CAN_CTL_CCE;

    // Set the bit fields of the bit timing register according to the parms.
    uBitReg = ((pClkParms->uPhase2Seg - 1) << 12) & CAN_BTR_TSEG2_M;
    uBitReg |= ((pClkParms->uSyncPropPhase1Seg - 1) << 8) & CAN_BTR_TSEG1_M;
    uBitReg |= ((pClkParms->uSJW - 1) << 6) & CAN_BTR_SJW_M;
    uBitReg |= (pClkParms->uQuantumPrescaler - 1) & CAN_BTR_BRP_M;
    uBitReg |= ((pClkParms->uQuantumPrescaler - 1) << 10)& CAN_BTR_BRPE_M;
    HWREG(ulBase + CAN_O_BTR) = uBitReg;

    // Clear the config change bit, and restore the init bit.
    uSavedInit &= ~CAN_CTL_CCE;

    // If Init was not set before, then clear it.
    if(uSavedInit & CAN_CTL_INIT)
    {
        uSavedInit &= ~CAN_CTL_INIT;
    }
    HWREG(ulBase + CAN_O_CTL) = uSavedInit;
}

//*****************************************************************************
//! Registers an interrupt handler for the CAN controller.
//!
//! \param ulBase is the base address of the CAN controller.
//! \param ucIntNumber is the interrupt line to register (0 or 1).
//! \param pfnHandler is a pointer to the function to be called when the
//! enabled CAN interrupts occur.
//!
//! This function registers the interrupt handler in the interrupt vector
//! table, and enables CAN interrupts on the interrupt controller; specific CAN
//! interrupt sources must be enabled using CANIntEnable().  The interrupt
//! handler being registered must clear the source of the interrupt using
//! CANIntClear().
//!
//! If the application is using a static interrupt vector table stored in
//! flash, then it is not necessary to register the interrupt handler this way.
//! Instead, IntEnable() should be used to enable CAN interrupts on the
//! interrupt controller.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
CANIntRegister(unsigned long ulBase, unsigned char ucIntNumber,
               void (*pfnHandler)(void))
{
    unsigned long ulIntNumber;

    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));

    // Get the actual interrupt number for this CAN controller.
    ulIntNumber = CANIntNumberGet(ulBase, ucIntNumber);

    // Register the interrupt handler.
    IntRegister(ulIntNumber, pfnHandler);

    // Enable the CAN interrupt.
    IntEnable(ulIntNumber);
}

//*****************************************************************************
//! Unregisters an interrupt handler for the CAN controller.
//!
//! \param ulBase is the base address of the controller.
//! \param ucIntNumber is the interrupt line to un-register (0 or 1).
//!
//! This function unregisters the previously registered interrupt handler and
//! disables the interrupt on the interrupt controller.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
CANIntUnregister(unsigned long ulBase, unsigned char ucIntNumber)
{
    unsigned long ulIntNumber;

    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));

    // Get the actual interrupt number for this CAN controller.
    ulIntNumber = CANIntNumberGet(ulBase, ucIntNumber);

    // Register the interrupt handler.
    IntUnregister(ulIntNumber);

    // Disable the CAN interrupt.
    IntDisable(ulIntNumber);
}

//*****************************************************************************
//! Enables individual CAN controller interrupt sources.
//!
//! \param ulBase is the base address of the CAN controller.
//! \param ulIntFlags is the bit mask of the interrupt sources to be enabled.
//!
//! Enables specific interrupt sources of the CAN controller.  Only enabled
//! sources will cause a processor interrupt.
//!
//! The \e ulIntFlags parameter is the logical OR of any of the following:
//!
//! - \b CAN_INT_ERROR - a controller error condition has occurred
//! - \b CAN_INT_STATUS - a message transfer has completed, or a bus error has
//! been detected
//! - \b CAN_INT_IE0 - allow CAN controller to generate interrupts on interrupt
//! line 0
//! - \b CAN_INT_IE1 - allow CAN controller to generate interrupts on interrupt
//! line 1
//!
//! In order to generate status or error interrupts, \b CAN_INT_IE0 must be
//! enabled.
//! Further, for any particular transaction from a message object to generate
//! an interrupt, that message object must have interrupts enabled (see
//! CANMessageSet()).  \b CAN_INT_ERROR will generate an interrupt if the
//! controller enters the ``bus off'' condition, or if the error counters reach
//! a limit.  \b CAN_INT_STATUS will generate an interrupt under quite a few
//! status conditions and may provide more interrupts than the application
//! needs to handle.  When an interrupt occurs, use CANIntStatus() to determine
//! the cause.
//!
//! \return None.
//
//*****************************************************************************
void
CANIntEnable(unsigned long ulBase, unsigned long ulIntFlags)
{
    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));
    ASSERT((ulIntFlags & ~(CAN_INT_ERROR | CAN_INT_STATUS | CAN_INT_IE0 |
                           CAN_INT_IE1)) == 0);

    // Enable the specified interrupts.
    HWREG(ulBase + CAN_O_CTL) =
        HWREG(ulBase + CAN_O_CTL) | ulIntFlags;
}

//*****************************************************************************
//! Disables individual CAN controller interrupt sources.
//!
//! \param ulBase is the base address of the CAN controller.
//! \param ulIntFlags is the bit mask of the interrupt sources to be disabled.
//!
//! Disables the specified CAN controller interrupt sources.  Only enabled
//! interrupt sources can cause a processor interrupt.
//!
//! The \e ulIntFlags parameter has the same definition as in the
//! CANIntEnable() function.
//!
//! \return None.
//
//*****************************************************************************
void
CANIntDisable(unsigned long ulBase, unsigned long ulIntFlags)
{
    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));
    ASSERT((ulIntFlags & ~(CAN_INT_ERROR | CAN_INT_STATUS | CAN_INT_IE0 |
                           CAN_INT_IE1)) == 0);

    // Disable the specified interrupts.
    HWREG(ulBase + CAN_O_CTL) =
        HWREG(ulBase + CAN_O_CTL) & ~(ulIntFlags);
}

//*****************************************************************************
//! Returns the current CAN controller interrupt status.
//!
//! \param ulBase is the base address of the CAN controller.
//! \param eIntStsReg indicates which interrupt status register to read
//!
//! Returns the value of one of two interrupt status registers.  The interrupt
//! status register read is determined by the \e eIntStsReg parameter, which
//! can have one of the following values:
//!
//! - \b CAN_INT_STS_CAUSE - indicates the cause of the interrupt
//! - \b CAN_INT_STS_OBJECT - indicates pending interrupts of all message
//! objects
//!
//! \b CAN_INT_STS_CAUSE returns the value of the controller interrupt register
//! and indicates the cause of the interrupt.  It will be a value of
//! \b CAN_INT_INT0ID_STATUS if the cause is a status interrupt.  In this case,
//! the status register should be read with the CANStatusGet() function.
//! Calling this function to read the status will also clear the status
//! interrupt.  If the value of the interrupt register is in the range 1-32,
//! then this indicates the number of the highest priority message object that
//! has an interrupt pending.  The message object interrupt can be cleared by
//! using the CANIntClear() function, or by reading the message using
//! CANMessageGet() in the case of a received message.  The interrupt handler
//! can read the interrupt status again to make sure all pending interrupts are
//! cleared before returning from the interrupt.
//!
//! \b CAN_INT_STS_OBJECT returns a bit mask indicating which message objects
//! have pending interrupts.  This can be used to discover all of the pending
//! interrupts at once, as opposed to repeatedly reading the interrupt register
//! by using \b CAN_INT_STS_CAUSE.
//!
//! \return Returns the value of one of the interrupt status registers.
//
//*****************************************************************************
unsigned long
CANIntStatus(unsigned long ulBase, tCANIntStsReg eIntStsReg)
{
    unsigned long ulStatus;

    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));

    // See which status the caller is looking for.
    switch(eIntStsReg)
    {
    // The caller wants the global interrupt status for the CAN controller
    // specified by ulBase.
    case CAN_INT_STS_CAUSE:
    {
        ulStatus = HWREG(ulBase + CAN_O_INT);
        break;
    }

    // The caller wants the current message status interrupt for all
    // messages.
    case CAN_INT_STS_OBJECT:
    {
        // Read message object interrupt status
        ulStatus = HWREG(ulBase + CAN_O_INTPND);
        break;
    }

    // Request was for unknown status so just return 0.
    default:
    {
        ulStatus = 0;
        break;
    }
    }
    // Return the interrupt status value
    return(ulStatus);
}

//*****************************************************************************
//! Clears a CAN interrupt source.
//!
//! \param ulBase is the base address of the CAN controller.
//! \param ulIntClr is a value indicating which interrupt source to clear.
//!
//! This function can be used to clear a specific interrupt source.  The
//! \e ulIntClr parameter should be one of the following values:
//!
//! - \b CAN_INT_INTID_STATUS - Clears a status interrupt.
//! - 1-32 - Clears the specified message object interrupt
//!
//! It is not necessary to use this function to clear an interrupt.  This
//! should only be used if the application wants to clear an interrupt source
//! without taking the normal interrupt action.
//!
//! Normally, the status interrupt is cleared by reading the controller status
//! using CANStatusGet().  A specific message object interrupt is normally
//! cleared by reading the message object using CANMessageGet().
//!
//! \note Since there is a write buffer in the Cortex-M3 processor, it may take
//! several clock cycles before the interrupt source is actually cleared.
//! Therefore, it is recommended that the interrupt source be cleared early in
//! the interrupt handler (as opposed to the very last action) to avoid
//! returning from the interrupt handler before the interrupt source is
//! actually cleared.  Failure to do so may result in the interrupt handler
//! being immediately reentered (since NVIC still sees the interrupt source
//! asserted).
//!
//! \return None.
//
//*****************************************************************************
void
CANIntClear(unsigned long ulBase, unsigned long ulIntClr)
{
    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));
    ASSERT((ulIntClr == CAN_INT_INT0ID_STATUS) ||
           ((ulIntClr>=1) && (ulIntClr <=32)));

    if(ulIntClr == CAN_INT_INT0ID_STATUS)
    {
        // Simply read and discard the status to clear the interrupt.
        HWREG(ulBase + CAN_O_ES);
    }
    else
    {
        // Wait to be sure that this interface is not busy.
        while(HWREG(ulBase + CAN_O_IF1_CMD) & CAN_IF1_CMD_BUSY)
        {
        }

        // Only change the interrupt pending state by setting only the
        // CAN_IF1_CMD_CLRINTPND bit.
        HWREGH(ulBase + CAN_O_IF1_CMD+2) = CAN_IF1_CMD_CLRINTPND >> 16;

        // Send the clear pending interrupt command to the CAN controller.
        HWREGH(ulBase + CAN_O_IF1_CMD) = ulIntClr & CAN_IF1_CMD_MSGNUM_M;

        // Wait to be sure that this interface is not busy.
        while(HWREG(ulBase + CAN_O_IF1_CMD) & CAN_IF1_CMD_BUSY)
        {
        }
    }
}

//*****************************************************************************
//! Sets the CAN controller automatic retransmission behavior.
//!
//! \param ulBase is the base address of the CAN controller.
//! \param bAutoRetry enables automatic retransmission.
//!
//! Enables or disables automatic retransmission of messages with detected
//! errors.  If \e bAutoRetry is \b true, then automatic retransmission is
//! enabled, otherwise it is disabled.
//!
//! \return None.
//
//*****************************************************************************
void
CANRetrySet(unsigned long ulBase, tBoolean bAutoRetry)
{
    unsigned long ulCtlReg;

    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));

    ulCtlReg = HWREG(ulBase + CAN_O_CTL);

    // Conditionally set the DAR bit to enable/disable auto-retry.
    if(bAutoRetry)
    {
        // Clearing the DAR bit tells the controller to not disable the
        // auto-retry of messages which were not transmitted or received
        // correctly.
        ulCtlReg &= ~CAN_CTL_DAR;
    }
    else
    {
        // Setting the DAR bit tells the controller to disable the auto-retry
        // of messages which were not transmitted or received correctly.
        ulCtlReg |= CAN_CTL_DAR;
    }

    HWREG(ulBase + CAN_O_CTL) = ulCtlReg;
}

//*****************************************************************************
//! Returns the current setting for automatic retransmission.
//!
//! \param ulBase is the base address of the CAN controller.
//!
//! Reads the current setting for the automatic retransmission in the CAN
//! controller and returns it to the caller.
//!
//! \return Returns \b true if automatic retransmission is enabled, \b false
//! otherwise.
//
//*****************************************************************************
tBoolean
CANRetryGet(unsigned long ulBase)
{
    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));

    // Read the disable automatic retry setting from the CAN controller.
    if(HWREG(ulBase + CAN_O_CTL) & CAN_CTL_DAR)
    {
        // Automatic data retransmission is not enabled.
        return(false);
    }

    // Automatic data retransmission is enabled.
    return(true);
}

//*****************************************************************************
//! Reads one of the controller status registers.
//!
//! \param ulBase is the base address of the CAN controller.
//! \param eStatusReg is the status register to read.
//!
//! Reads a status register of the CAN controller and returns it to the caller.
//! The different status registers are:
//!
//! - \b CAN_STS_CONTROL - the main controller status
//! - \b CAN_STS_TXREQUEST - bit mask of objects pending transmission
//! - \b CAN_STS_NEWDAT - bit mask of objects with new data
//! - \b CAN_STS_MSGVAL - bit mask of objects with valid configuration
//!
//! When reading the main controller status register, a pending status
//! interrupt will be cleared.  This should be used in the interrupt handler
//! for the CAN controller if the cause is a status interrupt.  The controller
//! status register fields are as follows:
//!
//! - \b CAN_STATUS_PDA - controller in local power down mode
//! - \b CAN_STATUS_WAKE_UP - controller initiated system wake up
//! - \b CAN_STATUS_PERR - parity error detected
//! - \b CAN_STATUS_BUS_OFF - controller is in bus-off condition
//! - \b CAN_STATUS_EWARN - an error counter has reached a limit of at least 96
//! - \b CAN_STATUS_EPASS - CAN controller is in the error passive state
//! - \b CAN_STATUS_RXOK - a message was received successfully (independent of
//! any message filtering).
//! - \b CAN_STATUS_TXOK - a message was successfully transmitted
//! - \b CAN_STATUS_LEC_NONE - no error
//! - \b CAN_STATUS_LEC_STUFF - stuffing error detected
//! - \b CAN_STATUS_LEC_FORM - a format error occurred in the fixed format part
//! of a message
//! - \b CAN_STATUS_LEC_ACK - a transmitted message was not acknowledged
//! - \b CAN_STATUS_LEC_BIT1 - dominant level detected when trying to send in
//! recessive mode
//! - \b CAN_STATUS_LEC_BIT0 - recessive level detected when trying to send in
//! dominant mode
//! - \b CAN_STATUS_LEC_CRC - CRC error in received message
//!
//! The remaining status registers are 32-bit bit maps to the message objects.
//! They can be used to quickly obtain information about the status of all the
//! message objects without needing to query each one.  They contain the
//! following information:
//!
//! - \b CAN_STS_TXREQUEST - if a message object's TxRequest bit is set, that
//! means that a transmission is pending on that object.  The application can
//! use this to determine which objects are still waiting to send a message.
//! - \b CAN_STS_NEWDAT - if a message object's NewDat bit is set, that means
//! that a new message has been received in that object, and has not yet been
//! picked up by the host application
//! - \b CAN_STS_MSGVAL - if a message object's MsgVal bit is set, that means
//! it has a valid configuration programmed.  The host application can use this
//! to determine which message objects are empty/unused.
//!
//! \return Returns the value of the status register.
//
//*****************************************************************************
unsigned long
CANStatusGet(unsigned long ulBase, tCANStsReg eStatusReg)
{
    unsigned long ulStatus;

    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));

    switch(eStatusReg)
    {
    // Just return the global CAN status register since that is what was
    // requested.
    case CAN_STS_CONTROL:
    {
        ulStatus = HWREG(ulBase + CAN_O_ES);
        break;
    }

    // Return objects with valid transmit requests
    case CAN_STS_TXREQUEST:
    {
        ulStatus = HWREG(ulBase + CAN_O_TXRQ);
        break;
    }

    // Return messages objects with new data
    case CAN_STS_NEWDAT:
    {
        ulStatus = HWREG(ulBase + CAN_O_NWDAT);
        break;
    }

    // Return valid message objects
    case CAN_STS_MSGVAL:
    {
        ulStatus = HWREG(ulBase + CAN_O_MSGVAL);
        break;
    }

    // Unknown CAN status requested so return 0.
    default:
    {
        ulStatus = 0;
        break;
    }
    }
    return(ulStatus);
}

//*****************************************************************************
//! Reads the CAN controller error counter register.
//!
//! \param ulBase is the base address of the CAN controller.
//! \param pulRxCount is a pointer to storage for the receive error counter.
//! \param pulTxCount is a pointer to storage for the transmit error counter.
//!
//! Reads the error counter register and returns the transmit and receive error
//! counts to the caller along with a flag indicating if the controller receive
//! counter has reached the error passive limit.  The values of the receive and
//! transmit error counters are returned through the pointers provided as
//! parameters.
//!
//! After this call, \e *pulRxCount will hold the current receive error count
//! and \e *pulTxCount will hold the current transmit error count.
//!
//! \return Returns \b true if the receive error count has reached the error
//! passive limit, and \b false if the error count is below the error passive
//! limit.
//
//*****************************************************************************
tBoolean
CANErrCntrGet(unsigned long ulBase, unsigned long *pulRxCount,
              unsigned long *pulTxCount)
{
    unsigned long ulCANError;

    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));

    // Read the current count of transmit/receive errors.
    ulCANError = HWREG(ulBase + CAN_O_ERRC);

    // Extract the error numbers from the register value.
    *pulRxCount = (ulCANError & CAN_ERRC_REC_M) >> CAN_ERRC_REC_S;
    *pulTxCount = (ulCANError & CAN_ERRC_TEC_M) >> CAN_ERRC_TEC_S;

    if(ulCANError & CAN_ERRC_RP)
    {
        return(true);
    }
    return(false);
}

//*****************************************************************************
//! Configures a message object in the CAN controller.
//!
//! \param ulBase is the base address of the CAN controller.
//! \param ulObjID is the object number to configure (1-32).
//! \param pMsgObject is a pointer to a structure containing message object
//! settings.
//! \param eMsgType indicates the type of message for this object.
//!
//! This function is used to configure any one of the 32 message objects in the
//! CAN controller.  A message object can be configured as any type of CAN
//! message object as well as several options for automatic transmission and
//! reception.  This call also allows the message object to be configured to
//! generate interrupts on completion of message receipt or transmission.  The
//! message object can also be configured with a filter/mask so that actions
//! are only taken when a message that meets certain parameters is seen on the
//! CAN bus.
//!
//! The \e eMsgType parameter must be one of the following values:
//!
//! - \b MSG_OBJ_TYPE_TX - CAN transmit message object.
//! - \b MSG_OBJ_TYPE_TX_REMOTE - CAN transmit remote request message object.
//! - \b MSG_OBJ_TYPE_RX - CAN receive message object.
//! - \b MSG_OBJ_TYPE_RX_REMOTE - CAN receive remote request message object.
//! - \b MSG_OBJ_TYPE_RXTX_REMOTE - CAN remote frame receive remote, then
//! transmit message object.
//!
//! The message object pointed to by \e pMsgObject must be populated by the
//! caller, as follows:
//!
//! - \e ulMsgID - contains the message ID, either 11 or 29 bits.
//! - \e ulMsgIDMask - mask of bits from \e ulMsgID that must match if
//! identifier filtering is enabled.
//! - \e ulFlags
//!   - Set \b MSG_OBJ_TX_INT_ENABLE flag to enable interrupt on transmission.
//!   - Set \b MSG_OBJ_RX_INT_ENABLE flag to enable interrupt on receipt.
//!   - Set \b MSG_OBJ_USE_ID_FILTER flag to enable filtering based on the
//!   identifier mask specified by \e ulMsgIDMask.
//! - \e ulMsgLen - the number of bytes in the message data.  This should be
//! non-zero even for a remote frame; it should match the expected bytes of the
//! data responding data frame.
//! - \e pucMsgData - points to a buffer containing up to 8 bytes of data for a
//! data frame.
//!
//! \b Example: To send a data frame or remote frame(in response to a remote
//! request), take the following steps:
//!
//! -# Set \e eMsgType to \b MSG_OBJ_TYPE_TX.
//! -# Set \e pMsgObject->ulMsgID to the message ID.
//! -# Set \e pMsgObject->ulFlags. Make sure to set \b MSG_OBJ_TX_INT_ENABLE to
//! allow an interrupt to be generated when the message is sent.
//! -# Set \e pMsgObject->ulMsgLen to the number of bytes in the data frame.
//! -# Set \e pMsgObject->pucMsgData to point to an array containing the bytes
//! to send in the message.
//! -# Call this function with \e ulObjID set to one of the 32 object buffers.
//!
//! \b Example: To receive a specific data frame, take the following steps:
//!
//! -# Set \e eMsgObjType to \b MSG_OBJ_TYPE_RX.
//! -# Set \e pMsgObject->ulMsgID to the full message ID, or a partial mask to
//! use partial ID matching.
//! -# Set \e pMsgObject->ulMsgIDMask bits that should be used for masking
//! during comparison.
//! -# Set \e pMsgObject->ulFlags as follows:
//!   - Set \b MSG_OBJ_TX_INT_ENABLE flag to be interrupted when the data frame
//!   is received.
//!   - Set \b MSG_OBJ_USE_ID_FILTER flag to enable identifier based filtering.
//! -# Set \e pMsgObject->ulMsgLen to the number of bytes in the expected data
//! frame.
//! -# The buffer pointed to by \e pMsgObject->pucMsgData  and
//! \e pMsgObject->ulMsgLen are not used by this call as no data is present at
//! the time of the call.
//! -# Call this function with \e ulObjID set to one of the 32 object buffers.
//!
//! If you specify a message object buffer that already contains a message
//! definition, it will be overwritten.
//!
//! \return None.
//
//*****************************************************************************
void
CANMessageSet(unsigned long ulBase, unsigned long ulObjID,
              tCANMsgObject *pMsgObject, tMsgObjType eMsgType)
{
    unsigned long usCmdMaskReg;
    unsigned long usMaskReg;
    unsigned long usArbReg;
    unsigned long usMsgCtrl;
    tBoolean bTransferData;
    tBoolean bUseExtendedID;

    bTransferData = 0;

    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));
    ASSERT((ulObjID <= 32) && (ulObjID != 0));
    ASSERT((eMsgType == MSG_OBJ_TYPE_TX) ||
           (eMsgType == MSG_OBJ_TYPE_TX_REMOTE) ||
           (eMsgType == MSG_OBJ_TYPE_RX) ||
           (eMsgType == MSG_OBJ_TYPE_RX_REMOTE) ||
           (eMsgType == MSG_OBJ_TYPE_TX_REMOTE) ||
           (eMsgType == MSG_OBJ_TYPE_RXTX_REMOTE));

    // Wait for busy bit to clear
    while(HWREG(ulBase + CAN_O_IF1_CMD) & CAN_IF1_CMD_BUSY)
    {
    }

    // See if we need to use an extended identifier or not.
    if((pMsgObject->ulMsgID > CAN_MAX_11BIT_MSG_ID) ||
       (pMsgObject->ulFlags & MSG_OBJ_EXTENDED_ID))
    {
        bUseExtendedID = 1;
    }
    else
    {
        bUseExtendedID = 0;
    }

    // This is always a write to the Message object as this call is setting a
    // message object.  This call will also always set all size bits so it sets
    // both data bits.  The call will use the CONTROL register to set control
    // bits so this bit needs to be set as well.
    usCmdMaskReg = (CAN_IF1_CMD_WR_RD | CAN_IF1_CMD_DATAA | CAN_IF1_CMD_DATAB |
                    CAN_IF1_CMD_CONTROL);

    // Initialize the values to a known state before filling them in based on
    // the type of message object that is being configured.
    usArbReg = 0;
    usMsgCtrl = 0;
    usMaskReg = 0;

    switch(eMsgType)
    {
    // Transmit message object.
    case MSG_OBJ_TYPE_TX:
    {
        // Set the TXRQST bit and the reset the rest of the register.
        usMsgCtrl |= CAN_IF1_MCTL_TXRQST;
        usArbReg = CAN_IF1_ARB_DIR;
        bTransferData = 1;
        break;
    }

    // Transmit remote request message object
    case MSG_OBJ_TYPE_TX_REMOTE:
    {
        // Set the TXRQST bit and the reset the rest of the register.
        usMsgCtrl |= CAN_IF1_MCTL_TXRQST;
        usArbReg = 0;
        break;
    }

    // Receive message object.
    case MSG_OBJ_TYPE_RX:
    {
        // This clears the DIR bit along with everything else.  The TXRQST
        // bit was cleared by defaulting usMsgCtrl to 0.
        usArbReg = 0;
        break;
    }

    // Receive remote request message object.
    case MSG_OBJ_TYPE_RX_REMOTE:
    {
        // The DIR bit is set to one for remote receivers.  The TXRQST bit
        // was cleared by defaulting usMsgCtrl to 0.
        usArbReg = CAN_IF1_ARB_DIR;

        // Set this object so that it only indicates that a remote frame
        // was received and allow for software to handle it by sending back
        // a data frame.
        usMsgCtrl = CAN_IF1_MCTL_UMASK;

        // Use the full Identifier by default.
        usMaskReg = CAN_IF1_MSK_MASK_M;

        // Make sure to send the mask to the message object.
        usCmdMaskReg |= CAN_IF1_CMD_MASK;
        break;
    }

    // Remote frame receive remote, with auto-transmit message object.
    case MSG_OBJ_TYPE_RXTX_REMOTE:
    {
        // Oddly the DIR bit is set to one for remote receivers.
        usArbReg = CAN_IF1_ARB_DIR;

        // Set this object to auto answer if a matching identifier is seen.
        usMsgCtrl = CAN_IF1_MCTL_RMTEN | CAN_IF1_MCTL_UMASK;

        // The data to be returned needs to be filled in.
        bTransferData = 1;
        break;
    }

    // This case should never happen due to the ASSERT statement at the
    // beginning of this function.
    default:
    {
        return;
    }
    }

    // Configure the Mask Registers.
    if(pMsgObject->ulFlags & MSG_OBJ_USE_ID_FILTER)
    {
        if(bUseExtendedID)
        {
            // Set the 29 bits of Identifier mask that were requested.
            usMaskReg = pMsgObject->ulMsgIDMask & CAN_IF1_MSK_MASK_M;

        }
        else
        {

            // Put the 11 bit Mask Identifier into the upper bits of the field
            // in the register.
            usMaskReg = ((pMsgObject->ulMsgIDMask << CAN_IF1_ARB_STD_ID_S) &
                         CAN_IF1_ARB_STD_ID_M);
        }
    }

    // If the caller wants to filter on the extended ID bit then set it.
    if((pMsgObject->ulFlags & MSG_OBJ_USE_EXT_FILTER) ==
       MSG_OBJ_USE_EXT_FILTER)
    {
        usMaskReg |= CAN_IF1_MSK_MXTD;
    }

    // The caller wants to filter on the message direction field.
    if((pMsgObject->ulFlags & MSG_OBJ_USE_DIR_FILTER) ==
       MSG_OBJ_USE_DIR_FILTER)
    {
        usMaskReg |= CAN_IF1_MSK_MDIR;
    }

    if(pMsgObject->ulFlags & (MSG_OBJ_USE_ID_FILTER | MSG_OBJ_USE_DIR_FILTER |
                              MSG_OBJ_USE_EXT_FILTER))
    {
        // Set the UMASK bit to enable using the mask register.
        usMsgCtrl |= CAN_IF1_MCTL_UMASK;

        // Set the MASK bit so that this gets transferred to the Message
        // Object.
        usCmdMaskReg |= CAN_IF1_CMD_MASK;
    }

    // Set the Arb bit so that this gets transferred to the Message object.
    usCmdMaskReg |= CAN_IF1_CMD_ARB;

    // Configure the Arbitration registers.
    if(bUseExtendedID)
    {
        // Set the 29 bit version of the Identifier for this message object.
        // Mark the message as valid and set the extended ID bit.
        usArbReg |=
            (pMsgObject->ulMsgID & CAN_IF1_ARB_XTD_ID_M) |
            CAN_IF1_ARB_MSGVAL | CAN_IF1_ARB_XTD;

    }
    else
    {
        // Set the 11 bit version of the Identifier for this message object.
        // The lower 18 bits are set to zero.
        // Mark the message as valid.
        usArbReg |=
            ((pMsgObject->ulMsgID <<
              CAN_IF1_ARB_STD_ID_S) & CAN_IF1_ARB_STD_ID_M)
            | CAN_IF1_ARB_MSGVAL;

    }

    // Set the data length since this is set for all transfers.  This is also a
    // single transfer and not a FIFO transfer so set EOB bit.
    usMsgCtrl |= (pMsgObject->ulMsgLen & CAN_IF1_MCTL_DLC_M);

    // Mark this as the last entry if this is not the last entry in a FIFO.
    if((pMsgObject->ulFlags & MSG_OBJ_FIFO) == 0)
    {
        usMsgCtrl |= CAN_IF1_MCTL_EOB;
    }

    // Enable transmit interrupts if they should be enabled.
    if(pMsgObject->ulFlags & MSG_OBJ_TX_INT_ENABLE)
    {
        usMsgCtrl |= CAN_IF1_MCTL_TXIE;
    }

    // Enable receive interrupts if they should be enabled.
    if(pMsgObject->ulFlags & MSG_OBJ_RX_INT_ENABLE)
    {
        usMsgCtrl |= CAN_IF1_MCTL_RXIE;
    }

    // Write the data out to the CAN Data registers if needed.
    if(bTransferData)
    {
        CANDataRegWrite(pMsgObject->pucMsgData,
                        (unsigned long *)(ulBase + CAN_O_IF1_DATA),
                        pMsgObject->ulMsgLen);
    }

    // Write out the registers to program the message object.
    HWREGH(ulBase + CAN_O_IF1_CMD+2) = usCmdMaskReg >> 16;
    HWREG(ulBase + CAN_O_IF1_MSK) = usMaskReg;
    HWREG(ulBase + CAN_O_IF1_ARB) = usArbReg;
    HWREG(ulBase + CAN_O_IF1_MCTL) = usMsgCtrl;

    // Transfer the message object to the message object specifiec by ulObjID.
    HWREGH(ulBase + CAN_O_IF1_CMD) = ulObjID & CAN_IF1_CMD_MSGNUM_M;

    return;
}

//*****************************************************************************
//! Reads a CAN message from one of the message object buffers.
//!
//! \param ulBase is the base address of the CAN controller.
//! \param ulObjID is the object number to read (1-32).
//! \param pMsgObject points to a structure containing message object fields.
//! \param bClrPendingInt indicates whether an associated interrupt should be
//! cleared.
//!
//! This function is used to read the contents of one of the 32 message objects
//! in the CAN controller, and return it to the caller.  The data returned is
//! stored in the fields of the caller-supplied structure pointed to by
//! \e pMsgObject.  The data consists of all of the parts of a CAN message,
//! plus some control and status information.
//!
//! Normally this is used to read a message object that has received and stored
//! a CAN message with a certain identifier.  However, this could also be used
//! to read the contents of a message object in order to load the fields of the
//! structure in case only part of the structure needs to be changed from a
//! previous setting.
//!
//! When using CANMessageGet, all of the same fields of the structure are
//! populated in the same way as when the CANMessageSet() function is used,
//! with the following exceptions:
//!
//! \e pMsgObject->ulFlags:
//!
//! - \b MSG_OBJ_NEW_DATA indicates if this is new data since the last time it
//! was read
//! - \b MSG_OBJ_DATA_LOST indicates that at least one message was received on
//! this message object, and not read by the host before being overwritten.
//!
//! \return None.
//
//*****************************************************************************
void
CANMessageGet(unsigned long ulBase, unsigned long ulObjID,
              tCANMsgObject *pMsgObject, tBoolean bClrPendingInt)
{
    unsigned long usCmdMaskReg;
    unsigned long usMaskReg;
    unsigned long usArbReg;
    unsigned long usMsgCtrl;

    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));
    ASSERT((ulObjID <= 32) && (ulObjID != 0));

    // This is always a read to the Message object as this call is setting a
    // message object.
    usCmdMaskReg = (CAN_IF2_CMD_DATAA | CAN_IF2_CMD_DATAB |
                    CAN_IF2_CMD_CONTROL | CAN_IF2_CMD_MASK | CAN_IF2_CMD_ARB);

    // Clear a pending interrupt and new data in a message object.
    if(bClrPendingInt)
    {
        usCmdMaskReg |= CAN_IF2_CMD_CLRINTPND;
    }

    // Set up the request for data from the message object.
    HWREGH(ulBase + CAN_O_IF2_CMD+2) =  usCmdMaskReg >> 16;

    // Transfer the message object to the message object specified by ulObjID.
    HWREGH(ulBase + CAN_O_IF2_CMD) = ulObjID & CAN_IF2_CMD_MSGNUM_M;

    // Wait for busy bit to clear
    while(HWREG(ulBase + CAN_O_IF2_CMD) & CAN_IF2_CMD_BUSY)
    {
    }

    // Read out the IF Registers.
    usMaskReg = HWREG(ulBase + CAN_O_IF2_MSK);
    usArbReg = HWREG(ulBase + CAN_O_IF2_ARB);
    usMsgCtrl = HWREG(ulBase + CAN_O_IF2_MCTL);
    pMsgObject->ulFlags = MSG_OBJ_NO_FLAGS;

    // Determine if this is a remote frame by checking the TXRQST and DIR bits.
    if((!(usMsgCtrl & CAN_IF2_MCTL_TXRQST) &&
        (usArbReg & CAN_IF2_ARB_DIR)) ||
       ((usMsgCtrl & CAN_IF2_MCTL_TXRQST) && (!(usArbReg & CAN_IF2_ARB_DIR))))
    {
        pMsgObject->ulFlags |= MSG_OBJ_REMOTE_FRAME;
    }

    // Get the identifier out of the register, the format depends on size of
    // the mask.
    if(usArbReg & CAN_IF2_ARB_XTD)
    {
        // Set the 29 bit version of the Identifier for this message object.
        pMsgObject->ulMsgID = usArbReg & CAN_IF2_ARB_XTD_ID_M;

        pMsgObject->ulFlags |= MSG_OBJ_EXTENDED_ID;
    }
    else
    {
        // The Identifier is an 11 bit value.
        pMsgObject->ulMsgID =
            (usArbReg & CAN_IF2_ARB_STD_ID_M) >> CAN_IF2_ARB_STD_ID_S;
    }

    // Indicate that we lost some data.
    if(usMsgCtrl & CAN_IF2_MCTL_MSGLST)
    {
        pMsgObject->ulFlags |= MSG_OBJ_DATA_LOST;
    }

    // Set the flag to indicate if ID masking was used.
    if(usMsgCtrl & CAN_IF2_MCTL_UMASK)
    {
        if(usArbReg & CAN_IF2_ARB_XTD)
        {
            // The Identifier Mask is assumed to also be a 29 bit value.
            pMsgObject->ulMsgIDMask = (usMaskReg & CAN_IF2_MSK_MASK_M);

            // If this is a fully specified Mask and a remote frame then don't
            // set the MSG_OBJ_USE_ID_FILTER because the ID was not really
            // filtered.
            if((pMsgObject->ulMsgIDMask != 0x1fffffff) ||
               ((pMsgObject->ulFlags & MSG_OBJ_REMOTE_FRAME) == 0))
            {
                pMsgObject->ulFlags |= MSG_OBJ_USE_ID_FILTER;
            }
        }
        else
        {
            // The Identifier Mask is assumed to also be an 11 bit value.
            pMsgObject->ulMsgIDMask = ((usMaskReg & CAN_IF2_MSK_MASK_M) >>
                                       18);

            // If this is a fully specified Mask and a remote frame then don't
            // set the MSG_OBJ_USE_ID_FILTER because the ID was not really
            // filtered.
            if((pMsgObject->ulMsgIDMask != 0x7ff) ||
               ((pMsgObject->ulFlags & MSG_OBJ_REMOTE_FRAME) == 0))
            {
                pMsgObject->ulFlags |= MSG_OBJ_USE_ID_FILTER;
            }
        }

        // Indicate if the extended bit was used in filtering.
        if(usMaskReg & CAN_IF2_MSK_MXTD)
        {
            pMsgObject->ulFlags |= MSG_OBJ_USE_EXT_FILTER;
        }

        // Indicate if direction filtering was enabled.
        if(usMaskReg & CAN_IF2_MSK_MDIR)
        {
            pMsgObject->ulFlags |= MSG_OBJ_USE_DIR_FILTER;
        }
    }

    // Set the interrupt flags.
    if(usMsgCtrl & CAN_IF2_MCTL_TXIE)
    {
        pMsgObject->ulFlags |= MSG_OBJ_TX_INT_ENABLE;
    }
    if(usMsgCtrl & CAN_IF2_MCTL_RXIE)
    {
        pMsgObject->ulFlags |= MSG_OBJ_RX_INT_ENABLE;
    }

    // See if there is new data available.
    if(usMsgCtrl & CAN_IF2_MCTL_NEWDAT)
    {
        // Get the amount of data needed to be read.
        pMsgObject->ulMsgLen = (usMsgCtrl & CAN_IF2_MCTL_DLC_M);

        // Don't read any data for a remote frame, there is nothing valid in
        // that buffer anyway.
        if((pMsgObject->ulFlags & MSG_OBJ_REMOTE_FRAME) == 0)
        {
            // Read out the data from the CAN registers.
            CANDataRegRead(pMsgObject->pucMsgData,
                           (unsigned long *)(ulBase + CAN_O_IF2_DATA),
                           pMsgObject->ulMsgLen);
        }

        // Now clear out the new data flag.
        HWREG(ulBase + CAN_O_IF2_CMD + 2) = CAN_IF2_CMD_TXRQSTNDAT >> 16;

        // Transfer the message object to the message object specified by
        // ulObjID.
        HWREGH(ulBase + CAN_O_IF2_CMD) = ulObjID & CAN_IF2_CMD_MSGNUM_M;

        // Wait for busy bit to clear
        while(HWREG(ulBase + CAN_O_IF2_CMD) & CAN_IF2_CMD_BUSY)
        {
        }

        // Indicate that there is new data in this message.
        pMsgObject->ulFlags |= MSG_OBJ_NEW_DATA;
    }
    else
    {
        // Along with the MSG_OBJ_NEW_DATA not being set the amount of data
        // needs to be set to zero if none was available.
        pMsgObject->ulMsgLen = 0;
    }
}

//*****************************************************************************
//! Clears a message object so that it is no longer used.
//!
//! \param ulBase is the base address of the CAN controller.
//! \param ulObjID is the message object number to disable (1-32).
//!
//! This function frees the specified message object from use.  Once a message
//! object has been ``cleared,'' it will no longer automatically send or
//! receive messages, or generate interrupts.
//!
//! \return None.
//
//*****************************************************************************
void
CANMessageClear(unsigned long ulBase, unsigned long ulObjID)
{
    // Check the arguments.
    ASSERT(CANBaseValid(ulBase));
    ASSERT((ulObjID >= 1) && (ulObjID <= 32));

    // Wait for busy bit to clear
    while(HWREG(ulBase + CAN_O_IF1_CMD) & CAN_IF1_CMD_BUSY)
    {
    }

    // Clear the message value bit in the arbitration register.  This indicates
    // the message is not valid.
    HWREGH(ulBase + CAN_O_IF1_CMD +
           2) = (CAN_IF1_CMD_WR_RD | CAN_IF1_CMD_ARB) >> 16;
    HWREG(ulBase + CAN_O_IF1_ARB) = 0;

    // Initiate programming the message object
    HWREGH(ulBase + CAN_O_IF1_CMD) = ulObjID & CAN_IF1_CMD_MSGNUM_M;
}

//*****************************************************************************
// Close the Doxygen group.
//! @}
//*****************************************************************************


