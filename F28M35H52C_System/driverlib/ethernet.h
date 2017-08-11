//###########################################################################
// FILE:   ethernet.h
// TITLE:  Defines and Macros for the ethernet module.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

#ifndef __ETHERNET_H__
#define __ETHERNET_H__

//*****************************************************************************
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// Values that can be passed to EthernetConfigSet as the ulConfig value, and
// returned from EthernetConfigGet.
//*****************************************************************************
#define ETH_CFG_TS_TSEN         0x010000 // Enable Timestamp (CCP)
#define ETH_CFG_RX_BADCRCDIS    0x000800 // Disable RX BAD CRC Packets
#define ETH_CFG_RX_PRMSEN       0x000400 // Enable RX Promiscuous
#define ETH_CFG_RX_AMULEN       0x000200 // Enable RX Multicast
#define ETH_CFG_TX_DPLXEN       0x000010 // Enable TX Duplex Mode
#define ETH_CFG_TX_CRCEN        0x000004 // Enable TX CRC Generation
#define ETH_CFG_TX_PADEN        0x000002 // Enable TX Padding

//*****************************************************************************
// Values that can be passed to EthernetIntEnable, EthernetIntDisable, and
// EthernetIntClear as the ulIntFlags parameter, and returned from
// EthernetIntStatus.
//*****************************************************************************
#define ETH_INT_PHY             0x040 // PHY Event/Interrupt
#define ETH_INT_MDIO            0x020 // Management Transaction
#define ETH_INT_RXER            0x010 // RX Error
#define ETH_INT_RXOF            0x008 // RX FIFO Overrun
#define ETH_INT_TX              0x004 // TX Complete
#define ETH_INT_TXER            0x002 // TX Error
#define ETH_INT_RX              0x001 // RX Complete

//*****************************************************************************
// Helper Macros for Ethernet Processing
//*****************************************************************************
// htonl/ntohl - big endian/little endian byte swapping macros for
// 32-bit (long) values
//*****************************************************************************
#ifndef htonl
#define htonl(a)                    \
    ((((a) >> 24) & 0x000000ff) |   \
     (((a) >>  8) & 0x0000ff00) |   \
     (((a) <<  8) & 0x00ff0000) |   \
     (((a) << 24) & 0xff000000))
#endif

#ifndef ntohl
#define ntohl(a)    htonl((a))
#endif

//*****************************************************************************
// htons/ntohs - big endian/little endian byte swapping macros for
// 16-bit (short) values
//*****************************************************************************
#ifndef htons
#define htons(a)                \
    ((((a) >> 8) & 0x00ff) |    \
     (((a) << 8) & 0xff00))
#endif

#ifndef ntohs
#define ntohs(a)    htons((a))
#endif

//*****************************************************************************
// API Function prototypes
//*****************************************************************************
extern void EthernetInitExpClk(unsigned long ulBase, unsigned long ulEthClk);
extern void EthernetConfigSet(unsigned long ulBase, unsigned long ulConfig);
extern unsigned long EthernetConfigGet(unsigned long ulBase);
extern void EthernetMACAddrSet(unsigned long  ulBase,
                               unsigned char *pucMACAddr);
extern void EthernetMACAddrGet(unsigned long  ulBase,
                               unsigned char *pucMACAddr);
extern void EthernetEnable(unsigned long ulBase);
extern void EthernetDisable(unsigned long ulBase);
extern tBoolean EthernetPacketAvail(unsigned long ulBase);
extern tBoolean EthernetSpaceAvail(unsigned long ulBase);
extern long EthernetPacketGetNonBlocking(unsigned long  ulBase,
                                         unsigned char *pucBuf,
                                         long           lBufLen);
extern long EthernetPacketGet(unsigned long ulBase, unsigned char *pucBuf,
                              long lBufLen);
extern long EthernetPacketPutNonBlocking(unsigned long  ulBase,
                                         unsigned char *pucBuf,
                                         long           lBufLen);
extern long EthernetPacketPut(unsigned long ulBase, unsigned char *pucBuf,
                              long lBufLen);
extern void EthernetIntRegister(unsigned long ulBase,
                                void (*pfnHandler)(void));
extern void EthernetIntUnregister(unsigned long ulBase);
extern void EthernetIntEnable(unsigned long ulBase, unsigned long ulIntFlags);
extern void EthernetIntDisable(unsigned long ulBase, unsigned long ulIntFlags);
extern unsigned long EthernetIntStatus(unsigned long ulBase, tBoolean bMasked);
extern void EthernetIntClear(unsigned long ulBase, unsigned long ulIntFlags);
extern void EthernetPHYWrite(unsigned long ulBase, unsigned char ucRegAddr,
                             unsigned long ulData);
extern unsigned long EthernetPHYRead(unsigned long ulBase,
                                     unsigned char ucRegAddr);
extern void EthernetPHYPowerOff(unsigned long ulBase);
extern void EthernetPHYPowerOn(unsigned long ulBase);

//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  __ETHERNET_H__



