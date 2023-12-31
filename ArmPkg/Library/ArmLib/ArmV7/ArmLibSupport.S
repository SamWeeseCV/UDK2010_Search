#------------------------------------------------------------------------------ 
#
# Copyright (c) 2008-2009 Apple Inc. All rights reserved.
#
# All rights reserved. This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#------------------------------------------------------------------------------

.text
.align 2
.globl ASM_PFX(Cp15IdCode)
.globl ASM_PFX(Cp15CacheInfo)
.globl ASM_PFX(ArmEnableInterrupts)
.globl ASM_PFX(ArmDisableInterrupts)
.globl ASM_PFX(ArmGetInterruptState)
.globl ASM_PFX(ArmEnableFiq)
.globl ASM_PFX(ArmDisableFiq)
.globl ASM_PFX(ArmGetFiqState)
.globl ASM_PFX(ArmInvalidateTlb)
.globl ASM_PFX(ArmSetTranslationTableBaseAddress)
.globl ASM_PFX(ArmGetTranslationTableBaseAddress)
.globl ASM_PFX(ArmSetDomainAccessControl)
.globl ASM_PFX(CPSRMaskInsert)
.globl ASM_PFX(CPSRRead)
.globl ASM_PFX(ReadCCSIDR)
.globl ASM_PFX(ReadCLIDR)


#------------------------------------------------------------------------------

ASM_PFX(Cp15IdCode):
  mrc     p15,0,R0,c0,c0,0
  bx      LR

ASM_PFX(Cp15CacheInfo):
  mrc     p15,0,R0,c0,c0,1
  bx      LR

ASM_PFX(ArmEnableInterrupts):
  cpsie   i
	bx      LR

ASM_PFX(ArmDisableInterrupts):
  cpsid   i
	bx      LR

ASM_PFX(ArmGetInterruptState):
	mrs     R0,CPSR
	tst     R0,#0x80	    @Check if IRQ is enabled.
	moveq   R0,#1
	movne   R0,#0
	bx      LR

ASM_PFX(ArmEnableFiq):
  cpsie   f
	bx      LR

ASM_PFX(ArmDisableFiq):
  cpsid   f
	bx      LR

ASM_PFX(ArmGetFiqState):
	mrs     R0,CPSR
	tst     R0,#0x30	    @Check if IRQ is enabled.
	moveq   R0,#1
	movne   R0,#0
	bx      LR

ASM_PFX(ArmInvalidateTlb):
  mov     r0,#0
  mcr     p15,0,r0,c8,c7,0
  isb
  bx      lr

ASM_PFX(ArmSetTranslationTableBaseAddress):
  mcr     p15,0,r0,c2,c0,0
  isb
  bx      lr

ASM_PFX(ArmGetTranslationTableBaseAddress):
  mrc     p15,0,r0,c2,c0,0
  bx      lr


ASM_PFX(ArmSetDomainAccessControl):
  mcr     p15,0,r0,c3,c0,0
  isb
  bx      lr

ASM_PFX(CPSRMaskInsert):    @ on entry, r0 is the mask and r1 is the field to insert
  stmfd   sp!, {r4-r12, lr} @ save all the banked registers
  mov     r3, sp            @ copy the stack pointer into a non-banked register
  mrs     r2, cpsr          @ read the cpsr
  bic     r2, r2, r0        @ clear mask in the cpsr
  and     r1, r1, r0        @ clear bits outside the mask in the input
  orr     r2, r2, r1        @ set field
  msr     cpsr_cxsf, r2     @ write back cpsr (may have caused a mode switch)
  mov     sp, r3            @ restore stack pointer
  ldmfd   sp!, {r4-r12, lr} @ restore registers
  bx      lr                @ return (hopefully thumb-safe!)

ASM_PFX(CPSRRead):
  mrs     r0, cpsr
  bx      lr
  
ASM_PFX(ReadCCSIDR):
  mcr p15,2,r0,c0,c0,0   @ Write Cache Size Selection Register (CSSELR)
  isb
  mrc p15,1,r0,c0,c0,0   @ Read current CP15 Cache Size ID Register (CCSIDR)
  bx  lr
  

ASM_PFX(ReadCLIDR):
  mrc p15,1,r0,c0,c0,1 @ Read CP15 Cache Level ID Register

ASM_FUNCTION_REMOVE_IF_UNREFERENCED
