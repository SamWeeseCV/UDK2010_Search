/** @file
  The internal header file that declared a data structure that is shared
  between the SMM IPL and the SMM Core.

  Copyright (c) 2009 - 2010, Intel Corporation.  All rights reserved.<BR>
  This program and the accompanying materials are licensed and made available 
  under the terms and conditions of the BSD License which accompanies this 
  distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef _PI_SMM_CORE_PRIVATE_DATA_H_
#define _PI_SMM_CORE_PRIVATE_DATA_H_

///
/// Signature for the private structure shared between the SMM IPL and the SMM Core
///
#define SMM_CORE_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('s', 'm', 'm', 'c')

///
/// Private structure that is used to share information between the SMM IPL and 
/// the SMM Core.  This structure is allocated from memory of type EfiRuntimeServicesData.
/// Since runtime memory types are converted to available memory when a legacy boot 
/// is performed, the SMM Core must not access any fields of this structure if a legacy 
/// boot is performed.  As a result, the SMM IPL must create an event notification 
/// for the Legacy Boot event and notify the SMM Core that a legacy boot is being 
/// performed.  The SMM Core can then use this information to filter accesses to 
/// thos structure.
///
typedef struct {
  UINTN                           Signature;

  ///
  /// The ImageHandle passed into the entry point of the SMM IPL.  This ImageHandle
  /// is used by the SMM Core to fill in the ParentImageHandle field of the Loaded
  /// Image Protocol for each SMM Driver that is dispatched by the SMM Core.
  ///
  EFI_HANDLE                      SmmIplImageHandle;

  ///
  /// The number of SMRAM ranges passed from the SMM IPL to the SMM Core.  The SMM
  /// Core uses these ranges of SMRAM to initialize the SMM Core memory manager.
  ///
  UINTN                           SmramRangeCount;

  ///
  /// A table of SMRAM ranges passed from the SMM IPL to the SMM Core.  The SMM
  /// Core uses these ranges of SMRAM to initialize the SMM Core memory manager.
  ///
  EFI_SMRAM_DESCRIPTOR            *SmramRanges;

  ///
  /// The SMM Foundation Entry Point.  The SMM Core fills in this field when the 
  /// SMM Core is initialized.  The SMM IPL is responsbile for registering this entry 
  /// point with the SMM Configuration Protocol.  The SMM Configuration Protocol may 
  /// not be available at the time the SMM IPL and SMM Core are started, so the SMM IPL
  /// sets up a protocol notification on the SMM Configuration Protocol and registers 
  /// the SMM Foundation Entry Point as soon as the SMM Configuration Protocol is 
  /// available.
  ///
  EFI_SMM_ENTRY_POINT             SmmEntryPoint;
  
  ///
  /// Boolean flag set to TRUE while an SMI is being processed by the SMM Core.
  /// 
  BOOLEAN                         SmmEntryPointRegistered;

  ///
  /// Boolean flag set to TRUE while an SMI is being processed by the SMM Core.
  /// 
  BOOLEAN                         InSmm;

  ///
  /// This field is set by the SMM Core then the SMM Core is initialized.  This field is
  /// used by the SMM Base 2 Protocol and SMM Communication Protocol implementations in
  /// the SMM IPL.  
  ///
  EFI_SMM_SYSTEM_TABLE2           *Smst;

  ///
  /// This field is used by the SMM Communicatioon Protocol to pass a buffer into 
  /// a software SMI handler and for the software SMI handler to pass a buffer back to
  /// the caller of the SMM Communication Protocol.  
  ///
  VOID                            *CommunicationBuffer;

  ///
  /// This field is used by the SMM Communicatioon Protocol to pass the size of a buffer,
  /// in bytes, into a software SMI handler and for the software SMI handler to pass the 
  /// size, in bytes, of a buffer back to the caller of the SMM Communication Protocol.
  ///
  UINTN                           BufferSize;

  ///
  /// This field is used by the SMM Communication Protocol to pass the return status from
  /// a software SMI handler back to the caller of the SMM Communication Protocol.
  ///
  EFI_STATUS                      ReturnStatus;
} SMM_CORE_PRIVATE_DATA;

#endif
