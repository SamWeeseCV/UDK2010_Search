/** @file
  This file defines:
  * the capsule vendor GUID for capsule variables and the HOB.
  * the capsule variable name.
  * the capsule GUID HOB data structure.
  The capsule HOB and variable can be used to store the capsule image start address and length.
  They are used by EDKII implementation of capsule update across a system reset.
  
  @par Note: EDKII implementation of capsule updating has discarded this capsule GUID HOB data
             structure and used one UEFI Capsule HOB (defined in PI Specification 1.2) instead.

Copyright (c) 2006 - 2010, Intel Corporation.  All rights reserved<BR>
This program and the accompanying materials are licensed and made available under 
the terms and conditions of the BSD License that accompanies this distribution.  
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.                                            

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __EFI_CAPSULE_VENDOR_GUID_H__
#define __EFI_CAPSULE_VENDOR_GUID_H__

///
/// This guid is used as a variable GUID for the capsule variable 
/// if the capsule pointer is passed through reset via a variable.
///
/// This guid is also used as a hob GUID for the capsule data 
/// when the capsule pointer is passed from PEI phase to DXE phase.
///
#define EFI_CAPSULE_VENDOR_GUID  \
  { 0x711C703F, 0xC285, 0x4B10, { 0xA3, 0xB0, 0x36, 0xEC, 0xBD, 0x3C, 0x8B, 0xE2 } }

///
/// Name of capsule variable.
/// 
#define EFI_CAPSULE_VARIABLE_NAME L"CapsuleUpdateData"

///
/// The data structure of the capsule guid hob entry.
/// Note: EDKII implementation has discarded this structure and used 
///       UEFI_CAPSULE_HOB instead.
///
typedef struct {
  EFI_PHYSICAL_ADDRESS BaseAddress;  ///< Capsule data start address.
  UINT32 Length;                     ///< Length of capsule data.
} CAPSULE_HOB_INFO;

extern EFI_GUID gEfiCapsuleVendorGuid;

#endif // #ifndef _EFI_CAPSULE_VENDOR_GUID_H_
