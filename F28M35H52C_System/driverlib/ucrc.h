//###########################################################################
// FILE:   ucrc.h
// TITLE:  Defines and Macros for the uCRC controller.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

#ifndef __UCRC_H__
#define __UCRC_H__

//*****************************************************************************
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// These definitions are used to configure CRC type using the UCRCConfig API.
//*****************************************************************************
#define UCRC_REMAP_ADDRESS(x)    ((unsigned long) x | 0x04000000) // Remap address for CRC
                                                  // calculation

//*****************************************************************************
// These definitions are used to configure CRC type using the UCRCConfig API.
//*****************************************************************************
#define UCRC_CONFIG_CRC8         0x00000000 // uCRC8 (Polynomial = 0x03)
#define UCRC_CONFIG_CRC16_1      0x00000001 // uCRC16-P1 (Polynomial = 0x8005)
#define UCRC_CONFIG_CRC16_2      0x00000002 // uCRC16-P2 (Polynomial = 0x1021)
#define UCRC_CONFIG_CRC32        0x00000003 // CRC32 (Polynomial = 0x04C11DB7)

//*****************************************************************************
// API Function prototypes
//*****************************************************************************
extern void UCRCClear(unsigned long ulBase);
extern void UCRCConfig(unsigned long ulBase, unsigned long ulType);
extern unsigned long UCRCCalculation(unsigned long ulBase, 
                                     unsigned long ulType, 
                                     unsigned char * pucBuffer, 
                                     unsigned long ulNumBytes);

//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif //  __UCRC_H__







