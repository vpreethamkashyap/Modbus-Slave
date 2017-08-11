;//###########################################################################
;// FILE:    dcsm_z2_sec_values.asm
;// TITLE:   F28M3Xx M3 Zone 2 Security Values
;// 
;// DESCRIPTION:
;//         This file is used to specify security values to
;//         program into the Zone 2 CSM/ECSL password locations and 
;//         GRABSECT/GRABRAM/FLASHEXEONLY locations in Flash at 0x27FFDC 
;//         - 0x27FFF3.       
;//
;//         NOTE: This file is NOT part of the driverlib library, but 
;//               must be added to any projects where Zone 2 DCSM
;//               security values must be programmed (i.e. projects which
;//               program code to Flash)
;//   
;//###########################################################################
;// $TI Release: F28M35x Support Library v201 $ 
;// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $ 
;//###########################################################################

;//*****************************************************************************
;//! \addtogroup dcsm_api M3 DCSM API Drivers
;//! @{
;//*****************************************************************************

; The "z2secvalues" section contains the actual security values that will be
; linked and programmed into to the CSM password locations (PWL), ECSL password
; locatsion, GRABSECT, GRABRAM, and FLASHEXEONLY locations in flash.  
; All 0xFFFFFFFF's (erased) is the default value for the password locations (PWL).

; It is recommended that all values be left as 0xFFFFFFFF during code
; development.  Passwords of 0xFFFFFFFF do not activate code security and dummy 
; reads of the security registers isrequired to unlock the CSM.  
; When code development is complete, modify the passwords to activate the
; code security module.

      .sect ".z2secvalues"
      .global z2secvalues
z2secvalues:
      .word    0xFFFFFFFF        ;Flash Execute Only
      .word    0xFFFFFFFF        ;GRABRAM
      .word    0xFFFFFFFF        ;GRABSECT
      .word    0xFFFFFFFF        ;ECSLPSWD1    
      .word    0xFFFFFFFF        ;ECSLPSWD0
      .word    0xFFFFFFFF        ;CSMPSWD3
      .word    0xFFFFFFFF        ;CSMPSWD2
      .word    0xFFFFFFFF        ;CSMPSWD1 	  
      .word    0xFFFFFFFF        ;CSMPSWD0
    
;----------------------------------------------------------------------

; For code security operation, all addresses between 0x27FFD0 and
; 0x27FFDB cannot be used as program code or data.  These locations
; must be programmed to 0x00000000 when the code security password locations
; (PWL) are programmed.  If security is not a concern, then these addresses
; can be used for code or data.  

; The section "z2_csm_rsvd" can be used to program these locations to 0x00000000.

      .sect ".z2_csm_rsvd"
      .global z2_csm_rsvd
z2_csm_rsvd:
      .word 0x00000000 ;0x27FFD0
      .word 0x00000000 ;0x27FFD3 
      .word 0x00000000 ;0x27FFD8

;//*****************************************************************************
;// Close the Doxygen group.
;//! @}
;//*****************************************************************************
      


      


