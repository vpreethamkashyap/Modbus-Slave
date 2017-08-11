//###########################################################################
// FILE:   ipc_util.c
// TITLE:  Driver for the IPC Module Utility Functions.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

//*****************************************************************************
//! \addtogroup ipc_util_api_m3
//! @{
//*****************************************************************************

#include "inc/hw_ipc.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ram.h"
#include "driverlib/ipc.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/ram.h"
#include "driverlib/sysctl.h"

//*****************************************************************************
//! M3 Acknowledges CtoM IPC Flag.
//!
//! \param ulFlags specifies the IPC flag mask for flags being acknowledged.
//!
//! This function will allow the M3 master system to acknowledge/clear the IPC
//! flag set by the C28 control system. The \e ulFlags parameter can be any of
//! the IPC flag values: \b IPC_FLAG1 - \b IPC_FLAG32.
//!
//! \return None.
//*****************************************************************************
void
IPCCtoMFlagAcknowledge (unsigned long ulFlags)
{
    HWREG(MTOCIPC_BASE + IPC_O_CTOMIPCACK) |= ulFlags;
}

//*****************************************************************************
//! Determines whether the given CtoM IPC flags are busy or not.
//!
//! \param ulFlags specifies CtoM IPC Flag number masks to check the status of.
//!
//! Allows the caller to determine whether the designated IPC flags are
//! pending. The \e ulFlags parameter can be any of the IPC flag values:
//! \b IPC_FLAG1 - \b IPC_FLAG32.
//!
//! \return Returns \b 1 if the IPC flags are busy or \b 0 if designated
//! IPC flags are free.
//*****************************************************************************
unsigned short
IPCCtoMFlagBusy (unsigned long ulFlags)
{
    if ((HWREG(MTOCIPC_BASE + IPC_O_CTOMIPCSTS) & ulFlags)==0)
    {
        return (0);
    }
    else
    {
        return (1);
    }
}

//*****************************************************************************
//! Determines whether the given MtoC IPC flags are busy or not.
//!
//! \param ulFlags specifies MtoC IPC Flag number masks to check the status of.
//!
//! Allows the caller to determine whether the designated IPC flags are
//! available for further master to control system communication. If \b 0 is
//! returned, then all designated tasks have completed and are available.
//! The \e ulFlags parameter can be any of the IPC flag values:
//! \b IPC_FLAG1 - \b IPC_FLAG32.
//!
//! \return Returns \b 1 if the IPC flags are busy or \b 0 if designated
//! IPC flags are free.
//*****************************************************************************
unsigned short
IPCMtoCFlagBusy (unsigned long ulFlags)
{
    if ((HWREG(MTOCIPC_BASE + IPC_O_MTOCIPCFLG) & ulFlags)==0)
    {
        return (0);
    }
    else
    {
        return (1);
    }
}

//*****************************************************************************
//! M3 Sets MtoC IPC Flag
//!
//! \param ulFlags specifies the IPC flag mask for flags being set.
//!
//! This function will allow the M3 master system to set the designated IPC
//! flags to send to the C28 control system. The \e ulFlags parameter can be
//! any of the IPC flag values: \b IPC_FLAG1 - \b IPC_FLAG32.
//!
//! \return None.
//*****************************************************************************
void
IPCMtoCFlagSet (unsigned long ulFlags)
{
    HWREG(MTOCIPC_BASE + IPC_O_MTOCIPCSET) |= ulFlags;
}

//*****************************************************************************
//! M3 Clears MtoC IPC Flag
//!
//! \param ulFlags specifies the IPC flag mask for flags being set.
//!
//! This function will allow the M3 master system to set the designated IPC
//! flags to send to the C28 control system.  The \e ulFlags parameter can be
//! any of the IPC flag values: \b IPC_FLAG1 - \b IPC_FLAG32.
//!
//! \return None.
//*****************************************************************************
void
IPCMtoCFlagClear (unsigned long ulFlags)
{
    HWREG(MTOCIPC_BASE + IPC_O_MTOCIPCCLR) |= ulFlags;
}

//*****************************************************************************
//! Converts a M3 shared RAM address to its C28 equivalent address
//!
//! \param ulShareAddress specifies the M3 address in shared RAM (Sx RAM
//! or MSG RAM).
//!
//! This function will convert a shared RAM address location on the M3 master
//! system to its equivalent C28 control system address (applies to MSG RAM's
//! and Sx shared RAM blocks).
//!
//! \return C28 equivalent address.
//*****************************************************************************
unsigned long
IPCMtoCSharedRamConvert (unsigned long ulShareAddress)
{
    // MSG RAM address conversion
    if (ulShareAddress >= M3_CTOMMSGRAM_START)
    {
        return ((ulShareAddress - 0x20000000)>>1);
    }
    // Sx RAM address conversion
    else
    {
        return ((ulShareAddress - 0x1FFF0000)>>1);
    }
}

//*****************************************************************************
//! Executes a C28 control system bootloader.
//!
//! \param ulBootMode specifies which C28 control system boot mode to execute.
//!
//! This function will allow the M3 master system to boot the C28 control
//! system via the following modes: Boot to RAM, Boot to Flash, Boot via SPI,
//! SCI, I2C, or parallel I/O. Unlike other IPCLite driver functions, this
//! function blocks and waits until the control system boot ROM is configured
//! and ready to receive MTOCIPC INT1 interrupts. It then blocks and waits
//! until IPC INT1 and IPC FLAG32 are available in the C28 boot ROM prior to
//! sending the command to execute the selected bootloader. The \e
//! ulBootMode parameter accepts one of the following values: \b
//! CBROM_MTOC_BOOTMODE_COMMAND_ILLEGAL, \b
//! CBROM_MTOC_BOOTMODE_BOOT_FROM_RAM, \b
//! CBROM_MTOC_BOOTMODE_BOOT_FROM_FLASH, \b
//! CBROM_MTOC_BOOTMODE_BOOT_FROM_SCI, \b CBROM_MTOC_BOOTMODE_BOOT_FROM_SPI,
//! \b CBROM_MTOC_BOOTMODE_BOOT_FROM_I2C, \b
//! CBROM_MTOC_BOOTMODE_BOOT_FROM_PARALLEL.
//!
//! \return 0 (success) if command is sent, or 1 (failure) if boot mode is
//! invalid and command was not sent.
//*****************************************************************************
unsigned short
IPCMtoCBootControlSystem(unsigned long ulBootMode)
{
    // Wait until C28 control system boot ROM is ready to receive MTOCIPC INT1
    // interrupts
    while ((HWREG(MTOCIPC_BASE +
                  IPC_O_CTOMIPCBOOTSTS) &
            CBROM_BOOTSTS_CTOM_CONTROL_SYSTEM_READY)!=
           CBROM_BOOTSTS_CTOM_CONTROL_SYSTEM_READY)
    {
    }

    // Loop until C28 control system IPC flags 1 and 32 are available
    while (HWREG(MTOCIPC_BASE + IPC_O_MTOCIPCFLG) & (IPC_FLAG1 | IPC_FLAG32))
    {
    }

    if (ulBootMode >= CBROM_MTOC_BOOTMODE_BOOT_COMMAND_MAX_SUPPORT_VALUE)
    {
        return STATUS_FAIL;
    }

    // Based on boot mode, enable pull-ups on peripheral pins and give GPIO pin
    // control
    // to C28x control system.
    switch (ulBootMode)
    {
    case CBROM_MTOC_BOOTMODE_BOOT_FROM_SCI:
// SCITXDA ->    PF3_GPIO35
// SCIRXDA ->    PF4_GPIO36
        HWREG(GPIO_PORTF_BASE + GPIO_O_PUR) |= (GPIO_PIN_3 | GPIO_PIN_4);
        HWREG(GPIO_PORTF_BASE + GPIO_O_CSEL) |= (GPIO_PIN_3 | GPIO_PIN_4);
        break;
    case CBROM_MTOC_BOOTMODE_BOOT_FROM_LOOP:
        ulBootMode = CBROM_MTOC_BOOTMODE_BOOT_FROM_SCI;
        break;
    case CBROM_MTOC_BOOTMODE_BOOT_FROM_SPI:
// SPISIMOA ->    PD0_GPIO16
// SPISOMIA ->    PD1_GPIO17
// SPICLKA ->    PD2_GPIO18
// SPISTEA    ->  PD3_GPIO19
        HWREG(GPIO_PORTD_BASE +
              GPIO_O_PUR) |=
            (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
        HWREG(GPIO_PORTD_BASE +
              GPIO_O_CSEL) |=
            (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
        break;
    case CBROM_MTOC_BOOTMODE_BOOT_FROM_I2C:
// SDAA ->    PF0_GPIO32
// SCLA ->    PF1_GPIO33
        HWREG(GPIO_PORTF_BASE + GPIO_O_PUR) |= (GPIO_PIN_0 | GPIO_PIN_1);
        HWREG(GPIO_PORTF_BASE + GPIO_O_CSEL) |= (GPIO_PIN_0 | GPIO_PIN_1);
        break;
    case CBROM_MTOC_BOOTMODE_BOOT_FROM_PARALLEL:
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

// GPIO[7:6] ->    PB1:0_GPIO1:0
// GPIO[5:0] ->    PA5:0_GPIO5:0
        HWREG(GPIO_PORTA_BASE +
              GPIO_O_PUR) |=
            (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
             GPIO_PIN_4 |
             GPIO_PIN_5);
        HWREG(GPIO_PORTB_BASE + GPIO_O_PUR) |= (GPIO_PIN_0 | GPIO_PIN_1);
        HWREG(GPIO_PORTA_BASE +
              GPIO_O_CSEL) |=
            (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
             GPIO_PIN_4 |
             GPIO_PIN_5);
        HWREG(GPIO_PORTB_BASE + GPIO_O_CSEL) |= (GPIO_PIN_0 | GPIO_PIN_1);
// DSP_CTL ->  PE2_GPIO26
// HOST_CTL -> PE3_GPIO27
        HWREG(GPIO_PORTE_BASE + GPIO_O_PUR) |= (GPIO_PIN_2 | GPIO_PIN_3);
        HWREG(GPIO_PORTE_BASE + GPIO_O_CSEL) |= (GPIO_PIN_2 | GPIO_PIN_3);
        break;
    }

    HWREG(MTOCIPC_BASE + IPC_O_MTOCIPCCOM) = IPC_MTOC_EXECUTE_BOOTMODE_CMD;
    HWREG(MTOCIPC_BASE + IPC_O_MTOCIPCBOOTMODE) = ulBootMode;

    // Force IPC event on selected request task and enable status-checking.
    HWREG(MTOCIPC_BASE + IPC_O_MTOCIPCSET) |= (IPC_FLAG1 | IPC_FLAG32);

    return STATUS_PASS;
}

//*****************************************************************************
// Close the Doxygen group.
//! @}
//*****************************************************************************




