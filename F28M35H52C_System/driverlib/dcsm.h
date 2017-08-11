//###########################################################################
// FILE:   dcsm.h
// TITLE:  Defines and Macros for the DCSM controller.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

#ifndef __DCSM_H__
#define __DCSM_H__

//*****************************************************************************
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif


//*****************************************************************************
// DCSM flash offsets from flash base address (0x200000)
// 
//*****************************************************************************
#define ZONE1_CSMPSWD0_OFFSET    0x00000000    
#define ZONE1_CSMPSWD1_OFFSET    0x00000004    
#define ZONE1_CSMPSWD2_OFFSET    0x00000008    
#define ZONE1_CSMPSWD3_OFFSET    0x0000000C    
#define ZONE1_ECSLPSWD0_OFFSET   0x00000010    
#define ZONE1_ECSLPSWD1_OFFSET   0x00000014    
#define ZONE1_GRABSECT_OFFSET    0x00000018    
#define ZONE1_GRABRAM_OFFSET     0x0000001C    
#define ZONE1_EXEONLY_OFFSET     0x00000020    

#define ZONE2_CSMPSWD0_OFFSET    0x002FFFF0   
#define ZONE2_CSMPSWD1_OFFSET    0x002FFFF4   
#define ZONE2_CSMPSWD2_OFFSET    0x002FFFF8    
#define ZONE2_CSMPSWD3_OFFSET    0x002FFFFC    
#define ZONE2_ECSLPSWD0_OFFSET   0x002FFFE8    
#define ZONE2_ECSLPSWD1_OFFSET   0x002FFFEC   
#define ZONE2_GRABSECT_OFFSET    0x002FFFE4    
#define ZONE2_GRABRAM_OFFSET     0x002FFFE0    
#define ZONE2_EXEONLY_OFFSET     0x002FFFDC    
//*****************************************************************************
// OTPSECLOCK offsets from flash base address (0x200000)
// 
//*****************************************************************************

//*****************************************************************************
// Data structures to hold password keys
// 
//*****************************************************************************
// define/declare the following in a common header file
typedef struct 
{
    unsigned long csmkey0;
    unsigned long csmkey1;
    unsigned long csmkey2;
    unsigned long csmkey3;
} CSMPSWDKEY;
// define/declare the following in a common header file
typedef struct 
{
    unsigned long ecslkey0;
    unsigned long ecslkey1;
} ECSLPSWDKEY;


//*****************************************************************************
// DCSM functions
// 
//*****************************************************************************

extern void DCSMUnlockZone1CSM(CSMPSWDKEY *psCMDKey);
extern void DCSMUnlockZone1ECSL(ECSLPSWDKEY *psECSLKey);
extern void DCSMUnlockZone2CSM(CSMPSWDKEY *psCMDKey);
extern void DCSMUnlockZone2ECSL(ECSLPSWDKEY *psECSLKey);
extern void DCSMSecureZone1(void);
extern void DCSMSecureZone2(void);
extern void DCSMGrabFlashSectors(void);
extern void DCSMGrabRAMBlocks(void);
extern void DCSMReadZone1EXEONLY(void);
extern void DCSMReadZone2EXEONLY(void);
extern unsigned long DCSMGetRAMZone(unsigned long ramsection);
extern unsigned long DCSMGetFlashSectorZone(unsigned long sector);
extern unsigned long DCSMGetZone1EXEstatus(unsigned long sector);
extern unsigned long DCSMGetZone2EXEstatus(unsigned long sector);
extern unsigned long DCSMZone1CSMSecurityStatus(void);
extern unsigned long DCSMZone2CSMSecurityStatus(void);
extern unsigned long DCSMZone1ECSLSecurityStatus(void);
extern unsigned long DCSMZone2ECSLSecurityStatus(void);
//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif //  __DCSM_H__



