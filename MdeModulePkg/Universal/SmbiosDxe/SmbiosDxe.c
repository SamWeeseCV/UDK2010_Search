/** @file
  This code produces the Smbios protocol. It also responsible for constructing 
  SMBIOS table into system table.
  
Copyright (c) 2009 - 2010, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#include "SmbiosDxe.h"

//
// Module Global:
// Since this driver will only ever produce one instance of the
// protocol you are not required to dynamically allocate the PrivateData.
//
SMBIOS_INSTANCE mPrivateData;

//
// Chassis for SMBIOS entry point structure that is to be installed into EFI system config table.
//
SMBIOS_TABLE_ENTRY_POINT *EntryPointStructure    = NULL;
SMBIOS_TABLE_ENTRY_POINT EntryPointStructureData = {
  //
  // AnchorString
  //
  {
    0x5f,
    0x53,
    0x4d,
    0x5f
  },
  //
  // EntryPointStructureChecksum,TO BE FILLED
  //
  0,
  //
  // EntryPointStructure Length
  //
  0x1f,
  //
  // MajorVersion
  //
  (UINT8) (FixedPcdGet16 (PcdSmbiosVersion) >> 8),
  //
  // MinorVersion
  //
  (UINT8) (FixedPcdGet16 (PcdSmbiosVersion) & 0x00ff),
  //
  // MaxStructureSize, TO BE FILLED
  //
  0,
  //
  // EntryPointRevision
  //
  0,
  //
  // FormattedArea
  //
  {
    0,
    0,
    0,
    0,
    0
  },
  //
  // IntermediateAnchorString
  //
  {
    0x5f,
    0x44,
    0x4d,
    0x49,
    0x5f
  },
  //
  // IntermediateChecksum, TO BE FILLED
  //
  0,
  //
  // StructureTableLength, TO BE FILLED
  //
  0,
  //
  // StructureTableAddress, TO BE FILLED
  //
  0,
  //
  // NumberOfSmbiosStructures, TO BE FILLED
  //
  0,
  //
  // SmbiosBcdRevision
  //
  0  
};


/**

  Get the full size of smbios structure including optional strings that follow the formatted structure.

  @param Head                   Pointer to the beginning of smbios structure.
  @param Size                   The returned size.
  @param NumberOfStrings        The returned number of optional strings that follow the formatted structure.

  @retval EFI_SUCCESS           Size retured in Size.
  @retval EFI_INVALID_PARAMETER Input smbios structure mal-formed or Size is NULL.
  
**/
EFI_STATUS
EFIAPI
GetSmbiosStructureSize (
  IN   EFI_SMBIOS_TABLE_HEADER          *Head,
  OUT  UINTN                            *Size,
  OUT  UINTN                            *NumberOfStrings
  )
{
  UINTN  FullSize;
  UINT8  StrLen;
  INT8*  CharInStr;
  
  if (Size == NULL || NumberOfStrings == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FullSize = Head->Length;
  CharInStr = (INT8*)Head + Head->Length;
  *Size = FullSize;
  *NumberOfStrings = 0;
  StrLen = 0;
  //
  // look for the two consecutive zeros, check the string limit by the way.
  //
  while (*CharInStr != 0 || *(CharInStr+1) != 0) { 
    if (*CharInStr == 0) {
      *Size += 1;
      CharInStr++;
    }

    for (StrLen = 0 ; StrLen < SMBIOS_STRING_MAX_LENGTH; StrLen++) {
      if (*(CharInStr+StrLen) == 0) {
        break;
      }  
    }

    if (StrLen == SMBIOS_STRING_MAX_LENGTH) {
      return EFI_INVALID_PARAMETER;
    }
    //
    // forward the pointer
    //
    CharInStr += StrLen;
    *Size += StrLen;
    *NumberOfStrings += 1;
  }

  //
  // count ending two zeros.
  //
  *Size += 2;
  return EFI_SUCCESS;  
}

/**

  Determin whether an SmbiosHandle has already in use.

  @param Head        Pointer to the beginning of smbios structure.
  @param Handle      A unique handle will be assigned to the SMBIOS record.

  @retval TRUE       Smbios handle already in use.
  @retval FALSE      Smbios handle is NOT used.
  
**/
BOOLEAN
EFIAPI
CheckSmbiosHandleExistance (
  IN  LIST_ENTRY           *Head,
  IN  EFI_SMBIOS_HANDLE    Handle
  )
{
  LIST_ENTRY              *Link;
  SMBIOS_HANDLE_ENTRY     *HandleEntry;
  
  for (Link = Head->ForwardLink; Link != Head; Link = Link->ForwardLink) {
    HandleEntry = SMBIOS_HANDLE_ENTRY_FROM_LINK(Link);
    if (HandleEntry->SmbiosHandle == Handle) {
      return TRUE;
    }
  }

  return FALSE;
}

/**

  Get the max SmbiosHandle that could be use.

  @param  This           The EFI_SMBIOS_PROTOCOL instance.
  @param  MaxHandle      The max handle that could be assigned to the SMBIOS record.

**/
VOID
EFIAPI
GetMaxSmbiosHandle (
  IN CONST  EFI_SMBIOS_PROTOCOL   *This,
  IN OUT    EFI_SMBIOS_HANDLE     *MaxHandle
  ) 
{
  if (This->MajorVersion == 2 && This->MinorVersion == 0) {
    *MaxHandle = 0xFFFE;
  } else {
    *MaxHandle = 0xFEFF;
  }
}

/**

  Get an SmbiosHandle that could use.

  @param  This                   The EFI_SMBIOS_PROTOCOL instance.
  @param  SmbiosHandle           A unique handle will be assigned to the SMBIOS record.

  @retval EFI_SUCCESS            Smbios handle got.
  @retval EFI_OUT_OF_RESOURCES   Smbios handle is NOT available.
  
**/
EFI_STATUS
EFIAPI
GetAvailableSmbiosHandle (
  IN CONST EFI_SMBIOS_PROTOCOL   *This,
  IN OUT   EFI_SMBIOS_HANDLE     *Handle
  )
{
  LIST_ENTRY              *Head;
  SMBIOS_INSTANCE         *Private;
  EFI_SMBIOS_HANDLE       MaxSmbiosHandle;
  EFI_SMBIOS_HANDLE       AvailableHandle;

  GetMaxSmbiosHandle(This, &MaxSmbiosHandle);

  Private = SMBIOS_INSTANCE_FROM_THIS (This);
  Head = &Private->AllocatedHandleListHead;
  for (AvailableHandle = 1; AvailableHandle < MaxSmbiosHandle; AvailableHandle++) {
    if (!CheckSmbiosHandleExistance(Head, AvailableHandle)) {
      *Handle = AvailableHandle;
      return EFI_SUCCESS;
    }
  }

  return EFI_OUT_OF_RESOURCES;
}


/**
  Add an SMBIOS record.

  @param  This                  The EFI_SMBIOS_PROTOCOL instance.
  @param  ProducerHandle        The handle of the controller or driver associated with the SMBIOS information. NULL
                                means no handle.
  @param  SmbiosHandle          On entry, if non-zero, the handle of the SMBIOS record. If zero, then a unique handle
                                will be assigned to the SMBIOS record. If the SMBIOS handle is already in use
                                EFI_ALREADY_STARTED is returned and the SMBIOS record is not updated.
  @param  Record                The data for the fixed portion of the SMBIOS record. The format of the record is
                                determined by EFI_SMBIOS_TABLE_HEADER.Type. The size of the formatted area is defined 
                                by EFI_SMBIOS_TABLE_HEADER.Length and either followed by a double-null (0x0000) or 
                                a set of null terminated strings and a null.

  @retval EFI_SUCCESS           Record was added.
  @retval EFI_OUT_OF_RESOURCES  Record was not added due to lack of system resources.
  @retval EFI_ALREADY_STARTED   The SmbiosHandle passed in was already in use.

**/
EFI_STATUS
EFIAPI
SmbiosAdd (
  IN CONST EFI_SMBIOS_PROTOCOL  *This,
  IN EFI_HANDLE                 ProducerHandle, OPTIONAL
  IN OUT EFI_SMBIOS_HANDLE      *SmbiosHandle,
  IN EFI_SMBIOS_TABLE_HEADER    *Record
  )
{
  VOID                        *Raw;
  UINTN                       TotalSize;
  UINTN                       RecordSize;
  UINTN                       StructureSize;
  UINTN                       NumberOfStrings;
  EFI_STATUS                  Status;
  LIST_ENTRY                  *Head;
  SMBIOS_INSTANCE             *Private;
  EFI_SMBIOS_ENTRY            *SmbiosEntry;
  EFI_SMBIOS_HANDLE           MaxSmbiosHandle;
  SMBIOS_HANDLE_ENTRY         *HandleEntry;
  EFI_SMBIOS_RECORD_HEADER    *InternalRecord;
  
  if (SmbiosHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  Private = SMBIOS_INSTANCE_FROM_THIS (This);
  //
  // Check whether SmbiosHandle is already in use
  //
  Head = &Private->AllocatedHandleListHead;
  if (*SmbiosHandle != 0 && CheckSmbiosHandleExistance(Head, *SmbiosHandle)) {
    return EFI_ALREADY_STARTED;
  }

  //
  // when SmbiosHandle is zero, an available handle will be assigned
  //
  if (*SmbiosHandle == 0) {
    Status = GetAvailableSmbiosHandle(This, SmbiosHandle);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  } else {
    //
    // Check this handle validity
    //
    GetMaxSmbiosHandle(This, &MaxSmbiosHandle);
    if (*SmbiosHandle > MaxSmbiosHandle) {
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // Calculate record size and string number
  //
  Status = GetSmbiosStructureSize(Record, &StructureSize, &NumberOfStrings);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Enter into critical section
  //  
  Status = EfiAcquireLockOrFail (&Private->DataLock);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  RecordSize  = sizeof (EFI_SMBIOS_RECORD_HEADER) + StructureSize;
  TotalSize   = sizeof (EFI_SMBIOS_ENTRY) + RecordSize;

  //
  // Allocate internal buffer
  //
  SmbiosEntry = AllocateZeroPool (TotalSize);
  if (SmbiosEntry == NULL) {
    EfiReleaseLock (&Private->DataLock);
    return EFI_OUT_OF_RESOURCES;
  }
  HandleEntry = AllocateZeroPool (sizeof(SMBIOS_HANDLE_ENTRY));
  if (HandleEntry == NULL) {
    EfiReleaseLock (&Private->DataLock);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Build Handle Entry and insert into linked list
  //
  HandleEntry->Signature     = SMBIOS_HANDLE_ENTRY_SIGNATURE;
  HandleEntry->SmbiosHandle  = *SmbiosHandle;
  InsertTailList(&Private->AllocatedHandleListHead, &HandleEntry->Link);

  InternalRecord  = (EFI_SMBIOS_RECORD_HEADER *) (SmbiosEntry + 1);
  Raw     = (VOID *) (InternalRecord + 1);

  //
  // Build internal record Header
  //
  InternalRecord->Version     = EFI_SMBIOS_RECORD_HEADER_VERSION;
  InternalRecord->HeaderSize  = (UINT16) sizeof (EFI_SMBIOS_RECORD_HEADER);
  InternalRecord->RecordSize  = RecordSize;
  InternalRecord->ProducerHandle = ProducerHandle;
  InternalRecord->NumberOfStrings = NumberOfStrings;
  //
  // Insert record into the internal linked list
  //
  SmbiosEntry->Signature    = EFI_SMBIOS_ENTRY_SIGNATURE;
  SmbiosEntry->RecordHeader = InternalRecord;
  SmbiosEntry->RecordSize   = TotalSize;
  InsertTailList (&Private->DataListHead, &SmbiosEntry->Link);

  CopyMem (Raw, Record, StructureSize);
  ((EFI_SMBIOS_TABLE_HEADER*)Raw)->Handle = *SmbiosHandle;

  //
  // Leave critical section
  //
  EfiReleaseLock (&Private->DataLock);
  return EFI_SUCCESS;
}

/**
  Update the string associated with an existing SMBIOS record.

  @param  This                  The EFI_SMBIOS_PROTOCOL instance.
  @param  SmbiosHandle          SMBIOS Handle of structure that will have its string updated.
  @param  StringNumber          The non-zero string number of the string to update
  @param  String                Update the StringNumber string with String.

  @retval EFI_SUCCESS           SmbiosHandle had its StringNumber String updated.
  @retval EFI_INVALID_PARAMETER SmbiosHandle does not exist.
  @retval EFI_UNSUPPORTED       String was not added since it's longer than 64 significant characters.
  @retval EFI_NOT_FOUND         The StringNumber.is not valid for this SMBIOS record.

**/
EFI_STATUS
EFIAPI
SmbiosUpdateString (
  IN CONST EFI_SMBIOS_PROTOCOL      *This,
  IN EFI_SMBIOS_HANDLE              *SmbiosHandle,
  IN UINTN                          *StringNumber,
  IN CHAR8                          *String
  )
{
  UINTN                     InputStrLen;
  UINTN                     TargetStrLen;
  UINTN                     StrIndex;
  UINTN                     TargetStrOffset;
  UINTN                     NewEntrySize;
  CHAR8                     *StrStart;
  VOID                      *Raw;
  LIST_ENTRY                *Link;
  LIST_ENTRY                *Head;
  EFI_STATUS                Status;
  SMBIOS_INSTANCE           *Private;
  EFI_SMBIOS_ENTRY          *SmbiosEntry;
  EFI_SMBIOS_ENTRY          *ResizedSmbiosEntry;
  EFI_SMBIOS_HANDLE         MaxSmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER   *Record;
  EFI_SMBIOS_RECORD_HEADER  *InternalRecord;
  
  //
  // Check args validity
  //
  GetMaxSmbiosHandle(This, &MaxSmbiosHandle);

  if (*SmbiosHandle > MaxSmbiosHandle) {
    return EFI_INVALID_PARAMETER;
  }

  if (String == NULL) {
    return EFI_ABORTED;
  }

  if (*StringNumber == 0) {
    return EFI_NOT_FOUND;
  }

  InputStrLen = AsciiStrLen(String);
  if (InputStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }

  Private = SMBIOS_INSTANCE_FROM_THIS (This);
  //
  // Enter into critical section
  //  
  Status = EfiAcquireLockOrFail (&Private->DataLock);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Head = &Private->DataListHead;
  for (Link = Head->ForwardLink; Link != Head; Link = Link->ForwardLink) {
    SmbiosEntry = SMBIOS_ENTRY_FROM_LINK(Link);
    Record = (EFI_SMBIOS_TABLE_HEADER*)(SmbiosEntry->RecordHeader + 1);

    if ((UINTN)Record->Handle == *SmbiosHandle) {
      //
      // Find out the specified Smbios record
      //
      if (*StringNumber > SmbiosEntry->RecordHeader->NumberOfStrings) {
        EfiReleaseLock (&Private->DataLock);
        return EFI_NOT_FOUND;
      }
      //
      // Point to unformed string section
      //
      StrStart = (CHAR8*)Record + Record->Length;
     
      for (StrIndex = 1, TargetStrOffset = 0; StrIndex < *StringNumber; StrStart++, TargetStrOffset++) {
        //
        // A string ends in 00h
        //
        if (*StrStart == 0) {
          StrIndex++;
        }
        
        //
        // String section ends in double-null (0000h)
        //
        if (*StrStart == 0 && *(StrStart + 1) == 0) {
          EfiReleaseLock (&Private->DataLock);
          return EFI_NOT_FOUND;
        } 
      }

      if (*StrStart == 0) {
        StrStart ++;
        TargetStrOffset ++;
      }
      
      //
      // Now we get the string target
      //
      TargetStrLen = AsciiStrLen(StrStart);
      if (InputStrLen == TargetStrLen) {
        AsciiStrCpy(StrStart, String);
        EfiReleaseLock (&Private->DataLock);
        return EFI_SUCCESS;
      }

      //
      // Original string buffer size is not exactly match input string length.
      // Re-allocate buffer is needed.
      //
      NewEntrySize = SmbiosEntry->RecordSize + InputStrLen - TargetStrLen;
      ResizedSmbiosEntry = AllocateZeroPool (NewEntrySize);

      if (ResizedSmbiosEntry == NULL) {
        EfiReleaseLock (&Private->DataLock);
        return EFI_OUT_OF_RESOURCES;
      }

      InternalRecord  = (EFI_SMBIOS_RECORD_HEADER *) (ResizedSmbiosEntry + 1);
      Raw     = (VOID *) (InternalRecord + 1);

      //
      // Build internal record Header
      //
      InternalRecord->Version     = EFI_SMBIOS_RECORD_HEADER_VERSION;
      InternalRecord->HeaderSize  = (UINT16) sizeof (EFI_SMBIOS_RECORD_HEADER);
      InternalRecord->RecordSize  = SmbiosEntry->RecordHeader->RecordSize + InputStrLen - TargetStrLen;
      InternalRecord->ProducerHandle = SmbiosEntry->RecordHeader->ProducerHandle;
      InternalRecord->NumberOfStrings = SmbiosEntry->RecordHeader->NumberOfStrings;

      //
      // Copy smbios structure and optional strings.
      //
      CopyMem (Raw, SmbiosEntry->RecordHeader + 1, Record->Length + TargetStrOffset);
      CopyMem ((VOID*)((UINTN)Raw + Record->Length + TargetStrOffset), String, InputStrLen + 1);
      CopyMem ((CHAR8*)((UINTN)Raw + Record->Length + TargetStrOffset + InputStrLen + 1),
               (CHAR8*)Record + Record->Length + TargetStrOffset + TargetStrLen + 1,
               SmbiosEntry->RecordHeader->RecordSize - sizeof (EFI_SMBIOS_RECORD_HEADER) - Record->Length - TargetStrOffset - TargetStrLen - 1);

      //
      // Insert new record
      //
      ResizedSmbiosEntry->Signature    = EFI_SMBIOS_ENTRY_SIGNATURE;
      ResizedSmbiosEntry->RecordHeader = InternalRecord;
      ResizedSmbiosEntry->RecordSize   = NewEntrySize;
      InsertTailList (Link->ForwardLink, &ResizedSmbiosEntry->Link);

      //
      // Remove old record
      //
      RemoveEntryList(Link);
      FreePool(SmbiosEntry);
      EfiReleaseLock (&Private->DataLock);
      return EFI_SUCCESS;
    }
  }

  EfiReleaseLock (&Private->DataLock);
  return EFI_INVALID_PARAMETER;
}

/**
  Remove an SMBIOS record.

  @param  This                  The EFI_SMBIOS_PROTOCOL instance.
  @param  SmbiosHandle          The handle of the SMBIOS record to remove.

  @retval EFI_SUCCESS           SMBIOS record was removed.
  @retval EFI_INVALID_PARAMETER SmbiosHandle does not specify a valid SMBIOS record.

**/
EFI_STATUS
EFIAPI
SmbiosRemove (
  IN CONST EFI_SMBIOS_PROTOCOL   *This,
  IN EFI_SMBIOS_HANDLE           SmbiosHandle
  )
{
  LIST_ENTRY                 *Link;
  LIST_ENTRY                 *Head;
  EFI_STATUS                 Status;
  EFI_SMBIOS_HANDLE          MaxSmbiosHandle;
  SMBIOS_INSTANCE            *Private;
  EFI_SMBIOS_ENTRY           *SmbiosEntry;
  SMBIOS_HANDLE_ENTRY        *HandleEntry;
  EFI_SMBIOS_TABLE_HEADER    *Record;

  //
  // Check args validity
  //
  GetMaxSmbiosHandle(This, &MaxSmbiosHandle);

  if (SmbiosHandle > MaxSmbiosHandle) {
    return EFI_INVALID_PARAMETER;
  }

  Private = SMBIOS_INSTANCE_FROM_THIS (This);
  //
  // Enter into critical section
  //  
  Status = EfiAcquireLockOrFail (&Private->DataLock);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Head = &Private->DataListHead;
  for (Link = Head->ForwardLink; Link != Head; Link = Link->ForwardLink) {
    SmbiosEntry = SMBIOS_ENTRY_FROM_LINK(Link);
    Record = (EFI_SMBIOS_TABLE_HEADER*)(SmbiosEntry->RecordHeader + 1);
    if ((UINTN)Record->Handle == SmbiosHandle) {
      //
      // Remove specified smobios record from DataList
      //
      RemoveEntryList(Link);
      FreePool(SmbiosEntry);
      // 
      // Remove this handle from AllocatedHandleList
      //
      Head = &Private->AllocatedHandleListHead;
      for (Link = Head->ForwardLink; Link != Head; Link = Link->ForwardLink) {
        HandleEntry = SMBIOS_HANDLE_ENTRY_FROM_LINK(Link);
        if ((UINTN)HandleEntry->SmbiosHandle == SmbiosHandle) {
          RemoveEntryList(Link);
          FreePool(HandleEntry);
          break;
        }
      }
      EfiReleaseLock (&Private->DataLock);
      return EFI_SUCCESS;
    }
  }

  //
  // Leave critical section
  //
  EfiReleaseLock (&Private->DataLock);
  return EFI_INVALID_PARAMETER;
  
}

/**
  Allow the caller to discover all or some of the SMBIOS records.

  @param  This                  The EFI_SMBIOS_PROTOCOL instance.
  @param  SmbiosHandle          On entry, points to the previous handle of the SMBIOS record. On exit, points to the
                                next SMBIOS record handle. If it is zero on entry, then the first SMBIOS record
                                handle will be returned. If it returns zero on exit, then there are no more SMBIOS records.
  @param  Type                  On entry it means return the next SMBIOS record of type Type. If a NULL is passed in 
                                this functionally it ignored. Type is not modified by the GetNext() function.
  @param  Record                On exit, points to the SMBIOS Record consisting of the formatted area followed by
                                the unformatted area. The unformatted area optionally contains text strings.
  @param  ProducerHandle        On exit, points to the ProducerHandle registered by Add(). If no ProducerHandle was passed into Add() NULL is returned. 
                                If a NULL pointer is passed in no data will be returned 
                                
  @retval EFI_SUCCESS           SMBIOS record information was successfully returned in Record.
                                SmbiosHandle is the handle of the current SMBIOS record
  @retval EFI_NOT_FOUND         The SMBIOS record with SmbiosHandle was the last available record.

**/
EFI_STATUS
EFIAPI
SmbiosGetNext (
  IN CONST EFI_SMBIOS_PROTOCOL      *This,
  IN OUT EFI_SMBIOS_HANDLE          *SmbiosHandle,
  IN EFI_SMBIOS_TYPE                *Type,          OPTIONAL
  OUT EFI_SMBIOS_TABLE_HEADER       **Record,
  OUT EFI_HANDLE                    *ProducerHandle OPTIONAL
  )
{
  BOOLEAN                  StartPointFound;
  LIST_ENTRY               *Link;
  LIST_ENTRY               *Head;
  SMBIOS_INSTANCE          *Private;
  EFI_SMBIOS_ENTRY         *SmbiosEntry;
  EFI_SMBIOS_TABLE_HEADER  *SmbiosTableHeader;

  if (SmbiosHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  StartPointFound = FALSE;
  Private = SMBIOS_INSTANCE_FROM_THIS (This);
  Head = &Private->DataListHead;
  for (Link = Head->ForwardLink; Link != Head; Link = Link->ForwardLink) {
    SmbiosEntry = SMBIOS_ENTRY_FROM_LINK(Link);
    SmbiosTableHeader = (EFI_SMBIOS_TABLE_HEADER*)(SmbiosEntry->RecordHeader + 1); 

    //
    // If SmbiosHandle is zero, the first matched SMBIOS record handle will be returned
    //
    if (*SmbiosHandle == 0) {
      if ((Type != NULL) && (*Type != SmbiosTableHeader->Type)) {
        continue;  
      }

      *SmbiosHandle = SmbiosTableHeader->Handle;
      *Record =SmbiosTableHeader;
      if (ProducerHandle != NULL) {
        *ProducerHandle = SmbiosEntry->RecordHeader->ProducerHandle;
      }
      return EFI_SUCCESS;
    }

    //
    // Start this round search from the next Smbios handle
    //
    if (!StartPointFound && (*SmbiosHandle == SmbiosTableHeader->Handle)) {
      StartPointFound = TRUE;
      continue;
    }

    if (StartPointFound) {
      if ((Type != NULL) && (*Type != SmbiosTableHeader->Type)) {
        continue; 
      }
      
      *SmbiosHandle = SmbiosTableHeader->Handle;
      *Record = SmbiosTableHeader; 
      if (ProducerHandle != NULL) {
        *ProducerHandle = SmbiosEntry->RecordHeader->ProducerHandle;
      }

      return EFI_SUCCESS;   
    }
  }

  *SmbiosHandle = 0;
  return EFI_NOT_FOUND;
  
}


/**
  Assembles Smbios table from the SMBIOS protocol. Produce Table
  Entry Point and return the pointer to it.
  
  @param  TableEntryPointStructure   On exit, points to the SMBIOS entrypoint structure.
                                
  @retval EFI_SUCCESS                Structure created sucessfully.
  @retval EFI_NOT_READY              Some of The SMBIOS records was not available yet.
  @retval EFI_OUT_OF_RESOURCES       No enough memory.
  
**/
EFI_STATUS
EFIAPI
SmbiosCreateTable (
  OUT VOID    **TableEntryPointStructure
  )
{
  UINT8                           CheckSum;
  UINT8                           *BufferPointer;
  UINTN                           Index;
  UINTN                           RecordSize;
  UINTN                           NumOfStr;
  EFI_STATUS                      Status;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  EFI_SMBIOS_PROTOCOL             *SmbiosProtocol;
  EFI_PHYSICAL_ADDRESS            PhysicalAddress;
  EFI_SMBIOS_TABLE_HEADER         *SmbiosRecord;
  EFI_SMBIOS_TABLE_END_STRUCTURE  EndStructure;
  
  Status            = EFI_SUCCESS;
  BufferPointer     = NULL;
  CheckSum          = 0;

  //
  // Initialize the EntryPointStructure with initial values.
  //
  if (EntryPointStructure == NULL) {
    //
    // Allocate memory (below 4GB)
    //
    PhysicalAddress = 0xffffffff;
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiReservedMemoryType,
                    EFI_SIZE_TO_PAGES (sizeof (SMBIOS_TABLE_ENTRY_POINT)),
                    &PhysicalAddress
                    );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }

    EntryPointStructure = (SMBIOS_TABLE_ENTRY_POINT *) (UINTN) PhysicalAddress;
    
    CopyMem (
      EntryPointStructure,
      &EntryPointStructureData,
      sizeof (SMBIOS_TABLE_ENTRY_POINT)
      );
  }

  //
  // Free the original image
  //
  if (EntryPointStructure->TableAddress != 0) {
    FreePages (
          (VOID*)(UINTN)EntryPointStructure->TableAddress,
          EFI_SIZE_TO_PAGES (EntryPointStructure->TableLength)
          );
    EntryPointStructure->TableAddress = 0;
  }
  
  //
  // Locate smbios protocol to traverse smbios records.
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &SmbiosProtocol);
  ASSERT_EFI_ERROR (Status);
  ASSERT (SmbiosProtocol != NULL);

  //
  // Make some statistics about all the structures
  //
  EntryPointStructure->NumberOfSmbiosStructures = 0;
  EntryPointStructure->TableLength              = 0;
  EntryPointStructure->MaxStructureSize         = 0;
  SmbiosHandle = 0;

  //
  // Calculate EPS Table Length
  //
  do {
    Status = SmbiosProtocol->GetNext (
                               SmbiosProtocol,
                               &SmbiosHandle,
                               NULL,
                               &SmbiosRecord,
                               NULL
                               );
                               
    if (Status == EFI_SUCCESS) {
      GetSmbiosStructureSize(SmbiosRecord, &RecordSize, &NumOfStr);
      //
      // Record NumberOfSmbiosStructures, TableLength and MaxStructureSize
      //
      EntryPointStructure->NumberOfSmbiosStructures ++;
      EntryPointStructure->TableLength = (UINT16) (EntryPointStructure->TableLength + RecordSize);
      if (RecordSize > EntryPointStructure->MaxStructureSize) {
        EntryPointStructure->MaxStructureSize = (UINT16) RecordSize;
      }
    }
  } while (!EFI_ERROR(Status));
  
  //
  // Create End-Of-Table structure
  //
  GetMaxSmbiosHandle(SmbiosProtocol, &SmbiosHandle);
  EndStructure.Header.Type = EFI_SMBIOS_TYPE_END_OF_TABLE;
  EndStructure.Header.Length = (UINT8) sizeof (EFI_SMBIOS_TABLE_HEADER);
  EndStructure.Header.Handle = SmbiosHandle;
  EndStructure.Tailing[0] = 0;
  EndStructure.Tailing[1] = 0;
  EntryPointStructure->NumberOfSmbiosStructures++;
  EntryPointStructure->TableLength = (UINT16) (EntryPointStructure->TableLength + sizeof (EndStructure));
  if (sizeof (EndStructure) > EntryPointStructure->MaxStructureSize) {
    EntryPointStructure->MaxStructureSize = (UINT16) sizeof (EndStructure);
  }
  
  //
  // Allocate memory (below 4GB)
  //
  PhysicalAddress = 0xffffffff;
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiReservedMemoryType,
                  EFI_SIZE_TO_PAGES (EntryPointStructure->TableLength),
                  &PhysicalAddress
                  );
  if (EFI_ERROR (Status)) {
    FreePages ((VOID*)(UINTN)EntryPointStructure, EFI_SIZE_TO_PAGES (EntryPointStructure->TableLength));
    EntryPointStructure = NULL;
    return EFI_OUT_OF_RESOURCES;
  }

  EntryPointStructure->TableAddress = (UINT32) PhysicalAddress;
  
  //
  // Assemble the tables
  //
  BufferPointer = (UINT8 *) (UINTN) PhysicalAddress;
  SmbiosHandle = 0;
  do {
    Status = SmbiosProtocol->GetNext (
                               SmbiosProtocol,
                               &SmbiosHandle,
                               NULL,
                               &SmbiosRecord,
                               NULL
                               );
    if (Status == EFI_SUCCESS) {
      GetSmbiosStructureSize(SmbiosRecord, &RecordSize, &NumOfStr);
      CopyMem (BufferPointer, SmbiosRecord, RecordSize);
      BufferPointer = BufferPointer + RecordSize;
    }
  } while (!EFI_ERROR(Status));
  
  //
  // Assemble End-Of-Table structure
  //
  CopyMem (BufferPointer, &EndStructure, sizeof (EndStructure));

  //
  // Fixup checksums in the Entry Point Structure
  //
  CheckSum  = 0;
  EntryPointStructure->IntermediateChecksum = 0;
  for (Index = 0x10; Index < EntryPointStructure->EntryPointLength; Index++) {
    CheckSum = (UINT8) (CheckSum + ((UINT8 *) (EntryPointStructure))[Index]);
  }

  EntryPointStructure->IntermediateChecksum         = (UINT8) (0 - CheckSum);

  CheckSum = 0;
  EntryPointStructure->EntryPointStructureChecksum = 0;
  for (Index = 0x0; Index < EntryPointStructure->EntryPointLength; Index++) {
    CheckSum = (UINT8) (CheckSum + ((UINT8 *) (EntryPointStructure))[Index]);
  }

  EntryPointStructure->EntryPointStructureChecksum = (UINT8) (0 - CheckSum);

  //
  // Returns the pointer
  //
  *TableEntryPointStructure = EntryPointStructure;

  return EFI_SUCCESS;
}



/**
  Installs the Smbios Table to the System Table. This function gets called
  when the EFI_EVENT_SIGNAL_READY_TO_BOOT gets signaled
  
  @param  Event                The event to signal
  @param  Context              Event contex

**/
VOID
EFIAPI
SmbiosTableConstruction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  UINT8       *Eps;
  EFI_STATUS  Status;

  Status = SmbiosCreateTable ((VOID **) &Eps);
  if (!EFI_ERROR (Status)) {
    gBS->InstallConfigurationTable (&gEfiSmbiosTableGuid, Eps);
  }
}


/**

  Driver to produce Smbios protocol and register event for constructing SMBIOS table. 

  @param ImageHandle     Module's image handle
  @param SystemTable     Pointer of EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS    Smbios protocol installed
  @retval Other          No protocol installed, unload driver.

**/
EFI_STATUS
EFIAPI
SmbiosDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS            Status;
  EFI_EVENT             ReadyToBootEvent;

  mPrivateData.Signature                = SMBIOS_INSTANCE_SIGNATURE;
  mPrivateData.Smbios.Add               = SmbiosAdd;
  mPrivateData.Smbios.UpdateString      = SmbiosUpdateString;
  mPrivateData.Smbios.Remove            = SmbiosRemove;
  mPrivateData.Smbios.GetNext           = SmbiosGetNext;
  mPrivateData.Smbios.MajorVersion      = (UINT8) (FixedPcdGet16 (PcdSmbiosVersion) >> 8);
  mPrivateData.Smbios.MinorVersion      = (UINT8) (FixedPcdGet16 (PcdSmbiosVersion) & 0x00ff);

  InitializeListHead (&mPrivateData.DataListHead);
  InitializeListHead (&mPrivateData.AllocatedHandleListHead);
  EfiInitializeLock (&mPrivateData.DataLock, TPL_NOTIFY);
  
  //
  // Make a new handle and install the protocol
  //
  mPrivateData.Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &mPrivateData.Handle,
                  &gEfiSmbiosProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivateData.Smbios
                  );

  if (EFI_ERROR(Status)) {
    return Status;
  }
  //
  // Register the event to install SMBIOS Table into EFI System Table
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  SmbiosTableConstruction,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &ReadyToBootEvent
                  );
  
  return Status;
}
