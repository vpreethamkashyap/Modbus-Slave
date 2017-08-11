
    ; Thumb instruction set
    .thumb

    ; C defines (for register addresses and offsets)
    .cdecls C, LIST, WARN
    %{
        #include "inc/hw_memmap.h"
        #include "inc/hw_hwbist.h"
        #include "inc/hw_sysctl.h"
        #include "inc/hw_nvic.h"
    %}

    ; NOP Sequence Macro (100 NOPs)
seqnops .macro
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        .endm
    
    ; Globals
    .global  bistspsave
    .global  HWBISTASMSelfTestRun
    .global  HWBISTASMResetHandler

    ; RAM section globals
    .sect ".data"
bistspsave  .word   0x5a5a5a5a                      ; SP save location (initialized to 0x5a5a5a5a for testing)

    ; Code section
    .sect ".text"

    ; Fields that can be accessed with LDR
ptrbistsave .field  bistspsave, 32                  ; Address of bistspsave

    ; Self Test Run function
    ; This function is C-Callable
    ; Note: This function assumes that the processor is running in priviliged mode
HWBISTASMSelfTestRun:
    .asmfunc

    ; Note: We assume that MSP is being used (the CPU must be running in priviliged mode)
    ; Users that use things like SYS/BIOS and other RTOSes must be careful to ensure priviliged mode is used

    ; The ARM architecture is different from the C28 in that it has two different stacks
    ; therefore we must save both the MSP and PSP (the two stack pointers) to ensure that user context will be
    ; restored correctly.
    
    ; Note: r13 is inherently stored when we save the MSP/PSP because r13 is actually the 'active' stack pointer in ARM
    
    PUSH    {r0-r2}                                 ; Save registers we will modify (this will be restored at the final BX lr after BIST reset
    
;   PUSH = r1 = HWREG(SYSCTL_MWRALLOW);
    MOV     r0, #(SYSCTL_MWRALLOW & 0xFFFF)
    MOVT    r0, #(SYSCTL_MWRALLOW >> 16)
    LDR     r1, [r0]                                ; Save previous EALLOW state
    PUSH    {r1}                                    ; Save EALLOW state to stack for after-BIST recovery
    
;   HWREG(SYSCTL_MWRALLOW) = 0xA5A5A5A5;    
    MOV     r1, #0xA5A5
    MOVT    r1, #0xA5A5
    STR     r1, [r0]
    
;   HWREG(HWBIST_BASE + HWBIST_O_MSTCGCR3) = 0x0A   ; Start logging interrupts
    MOV     r0, #((HWBIST_BASE + HWBIST_O_MSTCGCR3) & 0xFFFF)
    MOVT    r0, #((HWBIST_BASE + HWBIST_O_MSTCGCR3) >> 16)
    MOV     r1, #0x0A
    STR     r1, [r0]
    
    NOP                                             ; 9 NOP pipeline flush
    NOP                                             ; Note: Cortex-M3 actually has a 3 stage pipeline,
    NOP                                             ; but design says use 8 NOPs (and we use 9 because that's safer)
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    
    ; BEGIN ContextSave
    PUSH    {lr}                                    ; lr (r14)
    PUSH    {r0-r12}                                ; r0-r12
    PUSH    {apsr}                                  ; apsr
    PUSH    {ipsr}                                  ; ipsr
    PUSH    {epsr}                                  ; epsr
    PUSH    {psp}                                   ; psp (r13 in user mode)
    
    MOV     r0, #(NVIC_VTABLE & 0xFFFF)
    MOVT    r0, #(NVIC_VTABLE >> 16)
    LDR     r1, [r0]
    PUSH    {r1}                                    ; Save NVIC_VTABLE
    
    MRS     r0, msp                                 ;
    LDR     r1, ptrbistsave                         ;
    STR     r0, [r1]                                ; mem[bistspsave] <- msp
    ; END ContextSave
    
    NOP                                             ; 9 NOP pipeline flush
    NOP                                             ; Note: Cortex-M3 actually has a 3 stage pipeline,
    NOP                                             ; but design says use 8 NOPs (and we use 9 because that's safer)
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP

;   HWREG(HWBIST_BASE + HWBIST_O_MSTCGCR4) = 0x0A   ; Start BIST Self Test (BIST_GO)
    MOV     r0, #((HWBIST_BASE + HWBIST_O_MSTCGCR4) & 0xFFFF)
    MOVT    r0, #((HWBIST_BASE + HWBIST_O_MSTCGCR4) >> 16)
    MOV     r1, #0x0A
    STR     r1, [r0]

    NOP                                             ; 9 NOP pipeline flush
    NOP                                             ; Note: Cortex-M3 actually has a 3 stage pipeline,
    NOP                                             ; but design says use 8 NOPs (and we use 9 because that's safer)
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP

    B       $                                       ; infinite loop (CPU will be stuck here if HW BIST is disabled)
    
HWBISTASMResetHandler:
    seqnops                                         ; NOP sequence

    ; BEGIN ContextRestore
    LDR     r0, ptrbistsave                         ;
    LDR     r0, [r0]                                ;
    MSR     msp, r0                                 ; msp <- mem[bistspsave]
    
    ISB                                             ; Instruction Synchronization Barrier
                                                    ; Cortex-M3 core requires this if the MSP/PSP is changed 
    
    POP     {r1}
    MOV     r0, #(NVIC_VTABLE & 0xFFFF)
    MOVT    r0, #(NVIC_VTABLE >> 16)
    STR     r1, [r0]                                ; Restore NVIC_VTABLE
    
    POP     {psp}
    POP     {epsr}
    POP     {ipsr}
    POP     {apsr}
    POP     {r0-r12}
    POP     {lr}
    ; END ContextRestore
    
;   HWREG(SYSCTL_MWRALLOW) = 0xA5A5A5A5;            ; EALLOW
    MOV     r0, #(SYSCTL_MWRALLOW & 0xFFFF)
    MOVT    r0, #(SYSCTL_MWRALLOW >> 16)
    MOV     r1, #0xA5A5
    MOVT    r1, #0xA5A5
    STR     r1, [r0]
    
;   HWREG(SYSCTL_RESC) = 0x00                       ; Reset HW BIST reset cause bits
    MOV     r2, #(SYSCTL_RESC & 0xFFFF)
    MOVT    r2, #(SYSCTL_RESC >> 16)
    MOV     r1, #0x00
    STR     r1, [r2]
    
;   HWREG(HWBIST_BASE + HWBIST_O_MSTCCRD) = 0x0A    ; Context restore done, re-issue logged interrupts
    MOV     r2, #((HWBIST_BASE + HWBIST_O_MSTCCRD) & 0xFFFF)
    MOVT    r2, #((HWBIST_BASE + HWBIST_O_MSTCCRD) >> 16)
    MOV     r1, #0x0A
    STR     r1, [r2]

    seqnops                                         ; NOP sequence
    
;   HWREG(SYSCTL_MWRALLOW) = POP;
    POP     {r1}
    STR     r1, [r0]                                ; r0 already contains address of SYSCTL_MWRALLOW
    
    POP     {r0-r2}                                 ; Restore modified registers from C call
    BX      lr                                      ; Return to main application code
    .endasmfunc
    
