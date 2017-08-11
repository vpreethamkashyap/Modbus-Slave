//###########################################################################
// FILE:   uartsmc.c
// TITLE:  Utility driver to provide simple UART SmartCard functions.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

#include <stdarg.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/ringbuf.h"
#include "utils/uartsmc.h"

//*****************************************************************************
//! \addtogroup uartsmc_api
//! @{
//*****************************************************************************

//*****************************************************************************
// Defines for keeping track of the smart card state, primarily used when
// card insertion has been detected.
//*****************************************************************************
#define SMC_IDLE                0
#define SMC_DETECTED            1
#define SMC_POWER0              2
#define SMC_POWER1              3
#define SMC_POWER2              4
#define SMC_READY               5

//*****************************************************************************
// Number of clock ticks required before exiting the POWER2 state (that is
// de-asserting the RST signal) and entering the READY state.
//*****************************************************************************
#define SMC_POWER2_EXIT         40000

//*****************************************************************************
//! Run the periodic tick handler for SmartCard processing
//!
//! \param pInstance is the instance of this SmartCard interface.
//! \param ulTickMS is the number of ms since the last call to this handler
//!
//! This function is used to handle the periodic tasks required to support
//! the SmartCard interface.
//!
//! \return None.
//*****************************************************************************
void
UARTSmcTickHandler(tSmartCard *pInstance, unsigned long ulTickMS)
{
    tBoolean bCard;

    // Get the current card-insert status.
    bCard = pInstance->pfnQueryCard(pInstance);

    // If a card is inserted, and we are in the idle state,
    // kick off the state machine.
    if(bCard && (pInstance->ulState == SMC_IDLE))
    {
        pInstance->pfnSetRST(pInstance, true);
        pInstance->pfnSetVPP(pInstance, false);
        pInstance->pfnSetVCC(pInstance, false);
        pInstance->ulState = SMC_DETECTED;
    }

    // If a card has been removed, and we are not in the idle state,
    // reset the state machine.
    else if(!bCard && (pInstance->ulState != SMC_IDLE))
    {
        pInstance->pfnSetRST(pInstance, true);
        pInstance->pfnSetVPP(pInstance, false);
        pInstance->pfnSetVCC(pInstance, false);
        pInstance->ulState = SMC_IDLE;
    }

    // Smart Card State Machine
    switch(pInstance->ulState)
    {
    case SMC_IDLE:
    {
        // Nothing required here, just wait for a card.
        break;
    }

    case SMC_DETECTED:
    {
        // Nothing required here, except to transition to the
        // first Power sequence state.
        pInstance->ulState = SMC_POWER0;
        break;
    }

    case SMC_POWER0:
    {
        // Enable VCC.
        pInstance->pfnSetVCC(pInstance, true);
        pInstance->ulState = SMC_POWER1;
        break;
    }

    case SMC_POWER1:
    {
        // Enable VPP.
        pInstance->pfnSetVPP(pInstance, true);
        pInstance->ulState = SMC_POWER2;
        pInstance->ulDelay = 0;
        break;
    }

    case SMC_POWER2:
    {
        // Wait for appropriate clock cycles before deasserting reset.
        pInstance->ulDelay +=
            (((pInstance->ulBaud * 372) * ulTickMS) / 1000);
        if(pInstance->ulDelay > SMC_POWER2_EXIT)
        {
            pInstance->pfnSetRST(pInstance, false);
            pInstance->ulState = SMC_READY;
            pInstance->ulDelay = 0;
        }
        break;
    }

    case SMC_READY:
    {
        // Nothing (yet) to do here.
        break;
    }

    default:
    {
        // Unknown state, reset to IDLE state.
        pInstance->pfnSetRST(pInstance, true);
        pInstance->pfnSetVPP(pInstance, false);
        pInstance->pfnSetVCC(pInstance, false);
        pInstance->ulState = SMC_IDLE;
        break;
    }
    }
}

//*****************************************************************************
// Take as many bytes from the transmit buffer as we have space for and move
// them into the UART transmit FIFO.
//*****************************************************************************
static void
UARTSmcPrimeTransmit(tSmartCard *pInstance)
{
    // Do we have any data to transmit?
    if(!RingBufEmpty(&pInstance->sTxRingBuf))
    {
        // Disable the UART interrupt. If we don't do this there is a race
        // condition which can cause the read index to be corrupted.
        MAP_IntDisable(pInstance->ulInt);

        // Yes - take some characters out of the transmit buffer and feed
        // them to the UART transmit FIFO.
        while(MAP_UARTSpaceAvail(pInstance->ulPort) &&
              !RingBufEmpty(&pInstance->sTxRingBuf))
        {
            MAP_UARTCharPutNonBlocking(pInstance->ulPort,
                                       RingBufReadOne(&pInstance->sTxRingBuf));
        }

        // Reenable the UART interrupt.
        MAP_IntEnable(pInstance->ulInt);
    }
}

//*****************************************************************************
//! Initializes the UART SmartCard interface.
//!
//! \param pInstance is the Smart Card instance data for this interface.
//!
//! This function will initialize the specified serial port to be used as a
//! SmartCard (ISO-7816) interface.
//!
//! This function must be called prior to using any of the other UART SmartCard
//! functions.  In order for this function to work correctly, SysCtlClockSet()
//! must be called prior to calling this function.
//!
//! \return None.
//*****************************************************************************
void
UARTSmcInit(tSmartCard *pInstance)
{
    // Check the arguments.
    ASSERT(pInstance != 0);
    ASSERT(pInstance->pfnHostInit != 0)

    // Call the Host initialization function prior to configuring
    // the UART.
    pInstance->pfnHostInit(pInstance);

    // Configure the UART for Smart Card Mode.
    MAP_UARTConfigSetExpClk(pInstance->ulPort, MAP_SysCtlClockGet(),
                            (pInstance->ulBaud * 372),
                            (UART_CONFIG_PAR_EVEN | UART_CONFIG_STOP_TWO |
                             UART_CONFIG_WLEN_8));
    UARTSmartCardEnable(pInstance->ulPort);

    // Set the UART to interrupt whenever the TX FIFO is almost empty or
    // when any character is received.
    MAP_UARTFIFOLevelSet(pInstance->ulPort, UART_FIFO_TX1_8,
                         UART_FIFO_RX1_8);

    // Flush both the buffers.
    UARTSmcFlushRx(pInstance);
    UARTSmcFlushTx(pInstance, true);

    // We are configured for buffered output so enable the master interrupt
    // for this UART and the receive interrupts.  We don't actually enable the
    // transmit interrupt in the UART itself until some data has been placed
    // in the transmit buffer.
    MAP_UARTIntDisable(pInstance->ulPort, 0xFFFFFFFF);
    MAP_UARTIntEnable(pInstance->ulPort, UART_INT_RX | UART_INT_RT);
    MAP_IntEnable(pInstance->ulInt);

    // Enable the UART operation.
    MAP_UARTEnable(pInstance->ulPort);
}

//*****************************************************************************
//! Send one or more bytes of data out the SmartCard interface.
//!
//! \param pInstance is the instance of this SmartCard interface.
//! \param pcBuf points to a buffer containing the string to transmit.
//! \param ulLen is the length of the string to transmit.
//!
//! This function will transmit the packet of data to the UART output.  The
//! number of characters transmitted is determined by the \e ulLen parameter.
//!
//! The characters are written to the UART transmit buffer and the call returns
//! immediately.  If insufficient space remains in the transmit buffer,
//! additional characters are discarded.
//!
//! \return Returns the count of characters written.
//*****************************************************************************
int
UARTSmcWrite(tSmartCard *pInstance, const unsigned char *pcBuf,
             unsigned long ulLen)
{
    unsigned int uIdx;

    // Check for valid arguments.
    ASSERT(pcBuf != 0);

    // Send the characters
    for(uIdx = 0; uIdx < ulLen; uIdx++)
    {
        // Send the character to the UART output.
        if(!RingBufFull(&pInstance->sTxRingBuf))
        {
            RingBufWriteOne(&pInstance->sTxRingBuf, pcBuf[uIdx]);
        }
        else
        {
            // Buffer is full - discard remaining characters and return.
            break;
        }
    }

    // If we have anything in the buffer, make sure that the UART is set
    // up to transmit it.
    if(!RingBufEmpty(&pInstance->sTxRingBuf))
    {
        UARTSmcPrimeTransmit(pInstance);
        MAP_UARTIntEnable(pInstance->ulPort, UART_INT_TX);
    }

    // Return the number of characters written.
    return(uIdx);
}

//*****************************************************************************
//! Get one or more bytes of data from the SmartCard interface.
//!
//! \param pInstance is the instance of this SmartCard interface.
//! \param pcBuf points to a buffer for the incoming packet from the UART.
//! \param ulLen is the length of the buffer for storage of the packet.
//!
//! This function will receive \e ulLen bytes of data from the from the UART
//! input and store the characters in the buffer pointed to by \e pcBuf.
//!
//! This function will block until \e ulLen characters are received.
//!
//! \return Returns the count of characters that were stored.
//*****************************************************************************
int
UARTSmcRead(tSmartCard *pInstance, unsigned char *pcBuf, unsigned long ulLen)
{
    unsigned long ulCount;
    char cChar;

    // Check the arguments.
    ASSERT(pcBuf != 0);
    ASSERT(ulLen != 0);

    // Initialize the read count.
    ulCount = 0;

    // Process characters until a newline is received.
    while(ulCount < ulLen)
    {
        // Read the next character from the receive buffer.
        if(!RingBufEmpty(&pInstance->sRxRingBuf))
        {
            cChar = (char)RingBufReadOne(&pInstance->sRxRingBuf);

            // Store the character in the caller supplied buffer.
            pcBuf[ulCount] = cChar;

            // Increment the count of characters received.
            ulCount++;
        }
    }

    // Return the count of chars in the buffer, not counting the trailing 0.
    return(ulCount);
}

//*****************************************************************************
//! Returns the number of bytes available in the receive buffer.
//!
//! \param pInstance is the instance of this SmartCard interface.
//!
//! This function, available only when the module is built to operate in
//! buffered mode using \b UART_BUFFERED, may be used to determine the number
//! of bytes of data currently available in the receive buffer.
//!
//! \return Returns the number of available bytes.
//*****************************************************************************
int
UARTSmcRxBytesAvail(tSmartCard *pInstance)
{
    return(RingBufUsed(&pInstance->sRxRingBuf));
}

//*****************************************************************************
//! Returns the number of bytes free in the transmit buffer.
//!
//! \param pInstance is the instance of this SmartCard interface.
//!
//! This function, available only when the module is built to operate in
//! buffered mode using \b UART_BUFFERED, may be used to determine the amount
//! of space currently available in the transmit buffer.
//!
//! \return Returns the number of free bytes.
//*****************************************************************************
int
UARTSmcTxBytesFree(tSmartCard *pInstance)
{
    return(RingBufFree(&pInstance->sTxRingBuf));
}

//*****************************************************************************
//! Flushes the receive buffer.
//!
//! \param pInstance is the instance of this SmartCard interface.
//!
//! This function, available only when the module is built to operate in
//! buffered mode using \b UART_BUFFERED, may be used to discard any data
//! received from the UART but not yet read using UARTgets().
//!
//! \return None.
//*****************************************************************************
void
UARTSmcFlushRx(tSmartCard *pInstance)
{
    unsigned long ulInt;

    // Temporarily turn off interrupts.
    ulInt = IntMasterDisable();

    // Flush the receive buffer.
    RingBufFlush(&pInstance->sRxRingBuf);

    // If interrupts were enabled when we turned them off, turn them
    // back on again.
    if(!ulInt)
    {
        IntMasterEnable();
    }
}

//*****************************************************************************
//! Flushes the transmit buffer.
//!
//! \param pInstance is the instance of this SmartCard interface.
//! \param bDiscard indicates whether any remaining data in the buffer should
//! be discarded (\b true) or transmitted (\b false).
//!
//! This function may be used to flush the transmit buffer, either discarding
//! or transmitting any data received via calls to UARTSmcWrite() that is
//! waiting to be transmitted.  On return, the transmit buffer will be empty.
//!
//! \return None.
//*****************************************************************************
void
UARTSmcFlushTx(tSmartCard *pInstance, tBoolean bDiscard)
{
    unsigned long ulInt;

    // Should the remaining data be discarded or transmitted?
    if(bDiscard)
    {
        // The remaining data should be discarded, so temporarily turn off
        // interrupts.
        ulInt = IntMasterDisable();

        // Flush the transmit buffer.
        RingBufFlush(&pInstance->sTxRingBuf);

        // If interrupts were enabled when we turned them off, turn them
        // back on again.
        if(!ulInt)
        {
            IntMasterEnable();
        }
    }
    else
    {
        // Wait for all remaining data to be transmitted before returning.
        while(!RingBufEmpty(&pInstance->sTxRingBuf))
        {
        }
    }
}

//*****************************************************************************
//! Handles UART interrupts.
//!
//! \param pInstance is the instance of this SmartCard interface.
//!
//! This function handles interrupts from the UART.  It will copy data from the
//! transmit buffer to the UART transmit FIFO if space is available, and it
//! will copy data from the UART receive FIFO to the receive buffer if data is
//! available.
//!
//! \return None.
//*****************************************************************************
void
UARTSmcIntHandler(tSmartCard *pInstance)
{
    unsigned long ulInts;
    char cChar;
    long lChar;

    // Get and clear the current interrupt source(s)
    ulInts = MAP_UARTIntStatus(pInstance->ulPort, false);
    ulInts |= MAP_UARTIntStatus(pInstance->ulPort, true);
    MAP_UARTIntClear(pInstance->ulPort, ulInts);

    // Workaround: Errata Blah:
    // If we get into this interrupt, but there are no active interrupts, it
    // is probably an indication of a Parity Error interrupt from the Smart
    // Card logic.
    if(ulInts == 0)
    {
        MAP_UARTIntClear(pInstance->ulPort, UART_INT_PE);
    }

    // Are we being interrupted because the TX FIFO has space available?
    if(ulInts & UART_INT_TX)
    {
        // Move as many bytes as we can into the transmit FIFO.
        UARTSmcPrimeTransmit(pInstance);

        // If the output buffer is empty, turn off the transmit interrupt.
        if(RingBufEmpty(&pInstance->sTxRingBuf))
        {
            MAP_UARTIntDisable(pInstance->ulPort, UART_INT_TX);
        }
    }

    // Are we being interrupted due to a received character?
    if(ulInts & (UART_INT_RX | UART_INT_RT))
    {
        // Get all the available characters from the UART.
        while(MAP_UARTCharsAvail(pInstance->ulPort))
        {
            // Read a character
            lChar = MAP_UARTCharGetNonBlocking(pInstance->ulPort);
            cChar = (unsigned char)(lChar & 0xFF);

            // If there is space in the receive buffer, put the character
            // there, otherwise throw it away.
            if(!RingBufFull(&pInstance->sRxRingBuf) && (lChar < 256))
            {
                // Store the new character in the receive buffer
                RingBufWriteOne(&pInstance->sRxRingBuf, cChar);
            }
        }
    }
}

//*****************************************************************************
// Close the Doxygen group.
//! @}
//*****************************************************************************


