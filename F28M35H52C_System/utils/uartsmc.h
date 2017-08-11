//###########################################################################
// FILE:   uartsmc.h
// TITLE:  Prototypes for the UART SmartCard (ISO-7816) Utility Driver.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

#ifndef __UARTSMC_H__
#define __UARTSMC_H__

//*****************************************************************************
//! \addtogroup uartsmc_api
//! @{
//*****************************************************************************

//*****************************************************************************
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//! This structure defines all configuration and run time data necessary to
//! support the operation of a UART in Smart Card (ISO-7816) mode.
//*****************************************************************************
typedef struct
{
        //! The Smart Card instance.  This will be ignored by the Smart Card
        //! module, but will be passed through unchanged to the Host callback
        //! routines.
        long lInstance;

        //! The UART port being used for the Smart Card interface.
        unsigned long ulPort;

        //! The UART interrupt being used for the Smart Card interface.
        unsigned long ulInt;

        //! The UART baud rate to be configured for the Smart Card interface.
        unsigned long ulBaud;

        //! A pointer to the function that will initialize all of the host
        //! hardware (uarts, gpio, etc.) for the Smart Card interface.
        void (*pfnHostInit)(void *pInstance);

        //! A pointer to the function that will query the presence of a
        //! card.
        tBoolean (*pfnQueryCard)(void *pInstance);

        //! A pointer to the function that will set VCC for the Smart Card
        //! interface.
        void (*pfnSetVCC)(void *pInstance, tBoolean bEnabled);

        //! A pointer to the function that will set VPP for the Smart Card
        //! interface.
        void (*pfnSetVPP)(void *pInstance, tBoolean bEnabled);

        //! A pointer to the function that will assert RST for the Smart Card
        //! interface.
        void (*pfnSetRST)(void *pInstance, tBoolean bEnabled);

        //! A ring buffer object for the SmartCard transmitter.
        tRingBufObject sTxRingBuf;

        //! A ring buffer object for the SmartCard receiver.
        tRingBufObject sRxRingBuf;

        //! Instance variable for keeping track of SmartCard state.
        unsigned long ulState;

        //! Instance variable for keeping track of delays required in
        //! SmartCard processing.
        unsigned long ulDelay;
}
tSmartCard;

//*****************************************************************************
// Prototypes for the UART SmartCard Utility Driver functions.
//*****************************************************************************
void UARTSmcInit(tSmartCard *pInstance);
void UARTSmcTickHandler(tSmartCard *pInstance, unsigned long ulTickMS);
int UARTSmcWrite(tSmartCard *pInstance, const unsigned char *pucBuf,
                 unsigned long ulLen);
int UARTSmcRead(tSmartCard *pInstance, unsigned char *pucBuf,
                unsigned long ulLen);
int UARTSmcRxBytesAvail(tSmartCard *pInstance);
int UARTSmcTxBytesFree(tSmartCard *pInstance);
void UARTSmcFlushRx(tSmartCard *pInstance);
void UARTSmcFlushTx(tSmartCard *pInstance, tBoolean bDiscard);
void UARTSmcIntHandler(tSmartCard *pInstance);

//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif

//*****************************************************************************
// Close the Doxygen group.
//! @}
//*****************************************************************************

#endif // __GRLIB_H__


