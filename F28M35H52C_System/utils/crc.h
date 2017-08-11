//###########################################################################
// FILE:   crc.h
// TITLE:  Prototypes for the CRC functions.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

#ifndef __CRC_H__
#define __CRC_H__

//*****************************************************************************
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// Prototypes for the functions.
//*****************************************************************************
extern unsigned char Crc8CCITT(unsigned char ucCrc,
                               const unsigned char *pucData,
                               unsigned long ulCount);
extern unsigned short Crc16(unsigned short usCrc, const unsigned char *pucData,
                            unsigned long ulCount);
extern unsigned short Crc16Array(unsigned long ulWordLen,
                                 const unsigned long *pulData);
extern void Crc16Array3(unsigned long ulWordLen, const unsigned long *pulData,
                        unsigned short *pusCrc3);

//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __CRC_H__


