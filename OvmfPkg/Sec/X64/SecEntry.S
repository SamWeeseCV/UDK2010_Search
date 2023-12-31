#      TITLE   SecEntry.asm
#------------------------------------------------------------------------------
#*
#*   Copyright 2006 - 2009, Intel Corporation
#*   All rights reserved. This program and the accompanying materials
#*   are licensed and made available under the terms and conditions of the BSD License
#*   which accompanies this distribution.  The full text of the license may be found at
#*   http://opensource.org/licenses/bsd-license.php
#*
#*   THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#*   WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#*
#*    CpuAsm.asm
#*
#*   Abstract:
#*
#------------------------------------------------------------------------------


#include <Base.h>

#EXTERN ASM_PFX(SecCoreStartupWithStack)

#
# SecCore Entry Point
#
# Processor is in flat protected mode
#
# @param[in]  RAX   Initial value of the EAX register (BIST: Built-in Self Test)
# @param[in]  DI    'BP': boot-strap processor, or 'AP': application processor
# @param[in]  RBP   Pointer to the start of the Boot Firmware Volume
#
# @return     None  This routine does not return
#
ASM_GLOBAL ASM_PFX(_ModuleEntryPoint)
ASM_PFX(_ModuleEntryPoint):

    #
    # Load temporary stack top at very low memory.  The C code
    # can reload to a better address.
    #
    movq    $BASE_512KB, %rsp 
    nop

    #
    # Setup parameters and call SecCoreStartupWithStack
    #   rcx: BootFirmwareVolumePtr
    #   rdx: TopOfCurrentStack
    #
    movq    %rbp, %rcx
    movq    %rsp, %rdx
    subq    $0x20, %rsp
    call    ASM_PFX(SecCoreStartupWithStack)

