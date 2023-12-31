/** @file
  MTRR setting library

  Copyright (c) 2008 - 2010, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>

#include <Library/MtrrLib.h>
#include <Library/BaseLib.h>
#include <Library/CpuLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

//
// This table defines the offset, base and length of the fixed MTRRs
//
STATIC
FIXED_MTRR    MtrrLibFixedMtrrTable[] = {
  {
    MTRR_LIB_IA32_MTRR_FIX64K_00000,
    0,
    SIZE_64KB
  },
  {
    MTRR_LIB_IA32_MTRR_FIX16K_80000,
    0x80000,
    SIZE_16KB
  },
  {
    MTRR_LIB_IA32_MTRR_FIX16K_A0000,
    0xA0000,
    SIZE_16KB
  },
  {
    MTRR_LIB_IA32_MTRR_FIX4K_C0000,
    0xC0000,
    SIZE_4KB
  },
  {
    MTRR_LIB_IA32_MTRR_FIX4K_C8000,
    0xC8000,
    SIZE_4KB
  },
  {
    MTRR_LIB_IA32_MTRR_FIX4K_D0000,
    0xD0000,
    SIZE_4KB
  },
  {
    MTRR_LIB_IA32_MTRR_FIX4K_D8000,
    0xD8000,
    SIZE_4KB
  },
  {
    MTRR_LIB_IA32_MTRR_FIX4K_E0000,
    0xE0000,
    SIZE_4KB
  },
  {
    MTRR_LIB_IA32_MTRR_FIX4K_E8000,
    0xE8000,
    SIZE_4KB
  },
  {
    MTRR_LIB_IA32_MTRR_FIX4K_F0000,
    0xF0000,
    SIZE_4KB
  },
  {
    MTRR_LIB_IA32_MTRR_FIX4K_F8000,
    0xF8000,
    SIZE_4KB
  },
};

/**
  Returns the variable MTRR count for the CPU.

  @return Variable MTRR count

**/
UINT32
GetVariableMtrrCount (
  VOID
  )
{
  UINT32  VariableMtrrCount;

  if (!IsMtrrSupported ()) {
    return 0;
  }

  VariableMtrrCount = (UINT32)(AsmReadMsr64 (MTRR_LIB_IA32_MTRR_CAP) & MTRR_LIB_IA32_MTRR_CAP_VCNT_MASK);
  ASSERT (VariableMtrrCount <= MTRR_NUMBER_OF_VARIABLE_MTRR);

  return VariableMtrrCount;
}

/**
  Returns the firmware usable variable MTRR count for the CPU.

  @return Firmware usable variable MTRR count

**/
UINT32
GetFirmwareVariableMtrrCount (
  VOID
  )
{
  UINT32  VariableMtrrCount;

  VariableMtrrCount = GetVariableMtrrCount ();
  if (VariableMtrrCount < RESERVED_FIRMWARE_VARIABLE_MTRR_NUMBER) {
    return 0;
  }

  return VariableMtrrCount - RESERVED_FIRMWARE_VARIABLE_MTRR_NUMBER;
}

/**
  Returns the default MTRR cache type for the system.

  @return  MTRR default type

**/
UINT64
GetMtrrDefaultMemoryType (
  VOID
)
{
  return (AsmReadMsr64 (MTRR_LIB_IA32_MTRR_DEF_TYPE) & 0xff);
}


/**
  Preparation before programming MTRR.

  This function will do some preparation for programming MTRRs:
  disable cache, invalid cache and disable MTRR caching functionality

  @return  CR4 value before changing.

**/
UINTN
PreMtrrChange (
  VOID
  )
{
  UINTN  Value;

  //
  // Enter no fill cache mode, CD=1(Bit30), NW=0 (Bit29)
  //
  AsmDisableCache ();

  //
  // Save original CR4 value and clear PGE flag (Bit 7)
  //
  Value = AsmReadCr4 ();
  AsmWriteCr4 (Value & (~BIT7));

  //
  // Flush all TLBs
  //
  CpuFlushTlb ();

  //
  // Disable Mtrrs
  //
  AsmMsrBitFieldWrite64 (MTRR_LIB_IA32_MTRR_DEF_TYPE, 10, 11, 0);

  //
  // Return original CR4 value
  //
  return Value;
}


/**
  Cleaning up after programming MTRRs.

  This function will do some clean up after programming MTRRs:
  enable MTRR caching functionality, and enable cache

  @param  Cr4  CR4 value to restore

**/
VOID
PostMtrrChange (
  UINTN Cr4
  )
{
  //
  // Enable Cache MTRR
  //
  AsmMsrBitFieldWrite64 (MTRR_LIB_IA32_MTRR_DEF_TYPE, 10, 11, 3);

  //
  // Flush all TLBs 
  //
  CpuFlushTlb ();

  //
  // Enable Normal Mode caching CD=NW=0, CD(Bit30), NW(Bit29)
  //
  AsmEnableCache ();

  //
  // Restore original CR4 value
  //
  AsmWriteCr4 (Cr4);
}


/**
  Programs fixed MTRRs registers.

  @param  MemoryCacheType  The memory type to set.
  @param  Base             The base address of memory range.
  @param  Length           The length of memory range.

  @retval RETURN_SUCCESS      The cache type was updated successfully
  @retval RETURN_UNSUPPORTED  The requested range or cache type was invalid
                              for the fixed MTRRs.

**/
RETURN_STATUS
ProgramFixedMtrr (
  IN     UINT64     MemoryCacheType,
  IN OUT UINT64     *Base,
  IN OUT UINT64     *Length
  )
{
  UINT32  MsrNum;
  UINT32  ByteShift;
  UINT64  TempQword;
  UINT64  OrMask;
  UINT64  ClearMask;

  TempQword = 0;
  OrMask    = 0;
  ClearMask = 0;

  for (MsrNum = 0; MsrNum < MTRR_NUMBER_OF_FIXED_MTRR; MsrNum++) {
    if ((*Base >= MtrrLibFixedMtrrTable[MsrNum].BaseAddress) &&
        (*Base <
            (
              MtrrLibFixedMtrrTable[MsrNum].BaseAddress +
              (8 * MtrrLibFixedMtrrTable[MsrNum].Length)
            )
          )
        ) {
      break;
    }
  }

  if (MsrNum == MTRR_NUMBER_OF_FIXED_MTRR) {
    return RETURN_UNSUPPORTED;
  }

  //
  // We found the fixed MTRR to be programmed
  //
  for (ByteShift = 0; ByteShift < 8; ByteShift++) {
    if (*Base ==
         (
           MtrrLibFixedMtrrTable[MsrNum].BaseAddress +
           (ByteShift * MtrrLibFixedMtrrTable[MsrNum].Length)
         )
       ) {
      break;
    }
  }

  if (ByteShift == 8) {
    return RETURN_UNSUPPORTED;
  }

  for (
        ;
        ((ByteShift < 8) && (*Length >= MtrrLibFixedMtrrTable[MsrNum].Length));
        ByteShift++
      ) {
    OrMask |= LShiftU64 ((UINT64) MemoryCacheType, (UINT32) (ByteShift * 8));
    ClearMask |= LShiftU64 ((UINT64) 0xFF, (UINT32) (ByteShift * 8));
    *Length -= MtrrLibFixedMtrrTable[MsrNum].Length;
    *Base += MtrrLibFixedMtrrTable[MsrNum].Length;
  }

  if (ByteShift < 8 && (*Length != 0)) {
    return RETURN_UNSUPPORTED;
  }

  TempQword =
    (AsmReadMsr64 (MtrrLibFixedMtrrTable[MsrNum].Msr) & ~ClearMask) | OrMask;
  AsmWriteMsr64 (MtrrLibFixedMtrrTable[MsrNum].Msr, TempQword);
  return RETURN_SUCCESS;
}


/**
  Get the attribute of variable MTRRs.

  This function shadows the content of variable MTRRs into an
  internal array: VariableMtrr.

  @param  MtrrValidBitsMask     The mask for the valid bit of the MTRR
  @param  MtrrValidAddressMask  The valid address mask for MTRR
  @param  VariableMtrr          The array to shadow variable MTRRs content

  @return                       The return value of this paramter indicates the
                                number of MTRRs which has been used.

**/
UINT32
EFIAPI
MtrrGetMemoryAttributeInVariableMtrr (
  IN  UINT64                    MtrrValidBitsMask,
  IN  UINT64                    MtrrValidAddressMask,
  OUT VARIABLE_MTRR             *VariableMtrr
  )
{
  UINTN   Index;
  UINT32  MsrNum;
  UINT32  UsedMtrr;
  UINT32  FirmwareVariableMtrrCount;
  UINT32  VariableMtrrEnd;

  if (!IsMtrrSupported ()) {
    return 0;
  }

  FirmwareVariableMtrrCount = GetFirmwareVariableMtrrCount ();
  VariableMtrrEnd = MTRR_LIB_IA32_VARIABLE_MTRR_BASE + (2 * GetVariableMtrrCount ()) - 1;

  ZeroMem (VariableMtrr, sizeof (VARIABLE_MTRR) * MTRR_NUMBER_OF_VARIABLE_MTRR);
  UsedMtrr = 0;

  for (MsrNum = MTRR_LIB_IA32_VARIABLE_MTRR_BASE, Index = 0;
       (
         (MsrNum < VariableMtrrEnd) &&
         (Index < FirmwareVariableMtrrCount)
       );
       MsrNum += 2
      ) {
    if ((AsmReadMsr64 (MsrNum + 1) & MTRR_LIB_CACHE_MTRR_ENABLED) != 0) {
      VariableMtrr[Index].Msr          = MsrNum;
      VariableMtrr[Index].BaseAddress  = (AsmReadMsr64 (MsrNum) &
                                          MtrrValidAddressMask);
      VariableMtrr[Index].Length       = ((~(AsmReadMsr64 (MsrNum + 1) &
                                             MtrrValidAddressMask)
                                          ) &
                                          MtrrValidBitsMask
                                         ) + 1;
      VariableMtrr[Index].Type         = (AsmReadMsr64 (MsrNum) & 0x0ff);
      VariableMtrr[Index].Valid        = TRUE;
      VariableMtrr[Index].Used         = TRUE;
      UsedMtrr = UsedMtrr  + 1;
      Index++;
    }
  }
  return UsedMtrr;
}


/**
  Checks overlap between given memory range and MTRRs.

  @param  Start            The start address of memory range.
  @param  End              The end address of memory range.
  @param  VariableMtrr     The array to shadow variable MTRRs content

  @retval TRUE             Overlap exists.
  @retval FALSE            No overlap.

**/
BOOLEAN
CheckMemoryAttributeOverlap (
  IN PHYSICAL_ADDRESS     Start,
  IN PHYSICAL_ADDRESS     End,
  IN VARIABLE_MTRR      *VariableMtrr
  )
{
  UINT32  Index;

  for (Index = 0; Index < 6; Index++) {
    if (
         VariableMtrr[Index].Valid &&
         !(
           (Start > (VariableMtrr[Index].BaseAddress +
                     VariableMtrr[Index].Length - 1)
           ) ||
           (End < VariableMtrr[Index].BaseAddress)
         )
       ) {
      return TRUE;
    }
  }

  return FALSE;
}


/**
  Marks a variable MTRR as non-valid.

  @param  Index         The index of the array VariableMtrr to be invalidated
  @param  VariableMtrr  The array to shadow variable MTRRs content
  @param  UsedMtrr      The number of MTRRs which has already been used

**/
VOID
InvalidateShadowMtrr (
  IN   UINTN              Index,
  IN   VARIABLE_MTRR      *VariableMtrr,
  OUT  UINT32             *UsedMtrr
  )
{
  VariableMtrr[Index].Valid = FALSE;
  *UsedMtrr = *UsedMtrr - 1;
}


/**
  Combine memory attributes.

  If overlap exists between given memory range and MTRRs, try to combine them.

  @param  Attributes             The memory type to set.
  @param  Base                   The base address of memory range.
  @param  Length                 The length of memory range.
  @param  VariableMtrr           The array to shadow variable MTRRs content
  @param  UsedMtrr               The number of MTRRs which has already been used
  @param  OverwriteExistingMtrr  Returns whether an existing MTRR was used

  @retval EFI_SUCCESS            Memory region successfully combined.
  @retval EFI_ACCESS_DENIED      Memory region cannot be combined.

**/
RETURN_STATUS
CombineMemoryAttribute (
  IN     UINT64             Attributes,
  IN OUT UINT64             *Base,
  IN OUT UINT64             *Length,
  IN     VARIABLE_MTRR      *VariableMtrr,
  IN OUT UINT32             *UsedMtrr,
  OUT    BOOLEAN            *OverwriteExistingMtrr
  )
{
  UINT32  Index;
  UINT64  CombineStart;
  UINT64  CombineEnd;
  UINT64  MtrrEnd;
  UINT64  EndAddress;
  UINT32  FirmwareVariableMtrrCount;

  FirmwareVariableMtrrCount = GetFirmwareVariableMtrrCount ();

  *OverwriteExistingMtrr = FALSE;
  EndAddress = *Base +*Length - 1;

  for (Index = 0; Index < FirmwareVariableMtrrCount; Index++) {

    MtrrEnd = VariableMtrr[Index].BaseAddress + VariableMtrr[Index].Length - 1;
    if (
         !VariableMtrr[Index].Valid ||
         (
           *Base > (MtrrEnd) ||
           (EndAddress < VariableMtrr[Index].BaseAddress)
         )
       ) {
      continue;
    }

    //
    // Combine same attribute MTRR range
    //
    if (Attributes == VariableMtrr[Index].Type) {
      //
      // if the Mtrr range contain the request range, return RETURN_SUCCESS
      //
      if (VariableMtrr[Index].BaseAddress <= *Base && MtrrEnd >= EndAddress) {
        *Length = 0;
        return RETURN_SUCCESS;
      }
      //
      // invalid this MTRR, and program the combine range
      //
      CombineStart  =
        (*Base) < VariableMtrr[Index].BaseAddress ?
          (*Base) :
          VariableMtrr[Index].BaseAddress;
      CombineEnd    = EndAddress > MtrrEnd ? EndAddress : MtrrEnd;

      //
      // Record the MTRR usage status in VariableMtrr array.
      //
      InvalidateShadowMtrr (Index, VariableMtrr, UsedMtrr);
      *Base       = CombineStart;
      *Length     = CombineEnd - CombineStart + 1;
      EndAddress  = CombineEnd;
      *OverwriteExistingMtrr = TRUE;
      continue;
    } else {
      //
      // The cache type is different, but the range is convered by one MTRR
      //
      if (VariableMtrr[Index].BaseAddress == *Base && MtrrEnd == EndAddress) {
        InvalidateShadowMtrr (Index, VariableMtrr, UsedMtrr);
        continue;
      }

    }

    if ((Attributes== MTRR_CACHE_WRITE_THROUGH &&
         VariableMtrr[Index].Type == MTRR_CACHE_WRITE_BACK) ||
        (Attributes == MTRR_CACHE_WRITE_BACK &&
         VariableMtrr[Index].Type == MTRR_CACHE_WRITE_THROUGH) ||
        (Attributes == MTRR_CACHE_UNCACHEABLE) ||
        (VariableMtrr[Index].Type == MTRR_CACHE_UNCACHEABLE)
     ) {
      *OverwriteExistingMtrr = TRUE;
      continue;
    }
    //
    // Other type memory overlap is invalid
    //
    return RETURN_ACCESS_DENIED;
  }

  return RETURN_SUCCESS;
}


/**
  Calculate the maximum value which is a power of 2, but less the MemoryLength.

  @param  MemoryLength        The number to pass in.
  @return The maximum value which is align to power of 2 and less the MemoryLength

**/
UINT64
Power2MaxMemory (
  IN UINT64                     MemoryLength
  )
{
  UINT64  Result;

  if (RShiftU64 (MemoryLength, 32)) {
    Result = LShiftU64 (
               (UINT64) GetPowerOfTwo32 (
                          (UINT32) RShiftU64 (MemoryLength, 32)
                          ),
               32
               );
  } else {
    Result = (UINT64) GetPowerOfTwo32 ((UINT32) MemoryLength);
  }

  return Result;
}


/**
  Check the direction to program variable MTRRs.

  This function determines which direction of programming the variable
  MTRRs will use fewer MTRRs.

  @param  Input       Length of Memory to program MTRR
  @param  MtrrNumber  Pointer to the number of necessary MTRRs

  @retval TRUE        Positive direction is better.
          FALSE       Negtive direction is better.

**/
BOOLEAN
GetDirection (
  IN UINT64      Input,
  IN UINTN       *MtrrNumber
  )
{
  UINT64  TempQword;
  UINT32  Positive;
  UINT32  Subtractive;

  TempQword   = Input;
  Positive    = 0;
  Subtractive = 0;

  do {
    TempQword -= Power2MaxMemory (TempQword);
    Positive++;
  } while (TempQword != 0);

  TempQword = Power2MaxMemory (LShiftU64 (Input, 1)) - Input;
  Subtractive++;
  do {
    TempQword -= Power2MaxMemory (TempQword);
    Subtractive++;
  } while (TempQword != 0);

  if (Positive <= Subtractive) {
    *MtrrNumber = Positive;
    return TRUE;
  } else {
    *MtrrNumber = Subtractive;
    return FALSE;
  }
}

/**
  Invalid variable MTRRs according to the value in the shadow array.

  This function programs MTRRs according to the values specified
  in the shadow array.

  @param  VariableMtrr   The array to shadow variable MTRRs content

**/
STATIC
VOID
InvalidateMtrr (
   IN     VARIABLE_MTRR      *VariableMtrr
   )
{
  UINTN Index;
  UINTN Cr4;
  UINTN VariableMtrrCount;

  Cr4 = PreMtrrChange ();
  Index = 0;
  VariableMtrrCount = GetVariableMtrrCount ();
  while (Index < VariableMtrrCount) {
    if (VariableMtrr[Index].Valid == FALSE && VariableMtrr[Index].Used == TRUE ) {
       AsmWriteMsr64 (VariableMtrr[Index].Msr, 0);
       AsmWriteMsr64 (VariableMtrr[Index].Msr + 1, 0);
       VariableMtrr[Index].Used = FALSE;
    }
    Index ++;
  }
  PostMtrrChange (Cr4);
}


/**
  Programs variable MTRRs

  This function programs variable MTRRs

  @param  MtrrNumber            Index of MTRR to program.
  @param  BaseAddress           Base address of memory region.
  @param  Length                Length of memory region.
  @param  MemoryCacheType       Memory type to set.
  @param  MtrrValidAddressMask  The valid address mask for MTRR

**/
STATIC
VOID
ProgramVariableMtrr (
  IN UINTN                    MtrrNumber,
  IN PHYSICAL_ADDRESS         BaseAddress,
  IN UINT64                   Length,
  IN UINT64                   MemoryCacheType,
  IN UINT64                   MtrrValidAddressMask
  )
{
  UINT64  TempQword;
  UINTN   Cr4;

  Cr4 = PreMtrrChange ();

  //
  // MTRR Physical Base
  //
  TempQword = (BaseAddress & MtrrValidAddressMask) | MemoryCacheType;
  AsmWriteMsr64 ((UINT32) MtrrNumber, TempQword);

  //
  // MTRR Physical Mask
  //
  TempQword = ~(Length - 1);
  AsmWriteMsr64 (
    (UINT32) (MtrrNumber + 1),
    (TempQword & MtrrValidAddressMask) | MTRR_LIB_CACHE_MTRR_ENABLED
    );

  PostMtrrChange (Cr4);
}


/**
  Convert the Memory attibute value to MTRR_MEMORY_CACHE_TYPE.

  @param  MtrrType  MTRR memory type

  @return The enum item in MTRR_MEMORY_CACHE_TYPE

**/
STATIC
MTRR_MEMORY_CACHE_TYPE
GetMemoryCacheTypeFromMtrrType (
  IN UINT64                MtrrType
  )
{
  switch (MtrrType) {
  case MTRR_CACHE_UNCACHEABLE:
    return CacheUncacheable;
  case MTRR_CACHE_WRITE_COMBINING:
    return CacheWriteCombining;
  case MTRR_CACHE_WRITE_THROUGH:
    return CacheWriteThrough;
  case MTRR_CACHE_WRITE_PROTECTED:
    return CacheWriteProtected;
  case MTRR_CACHE_WRITE_BACK:
    return CacheWriteBack;
  default:
    //
    // MtrrType is MTRR_CACHE_INVALID_TYPE, that means
    // no mtrr covers the range
    //
    return CacheUncacheable;
  }
}

/**
  Initializes the valid bits mask and valid address mask for MTRRs.

  This function initializes the valid bits mask and valid address mask for MTRRs.

  @param  MtrrValidBitsMask     The mask for the valid bit of the MTRR
  @param  MtrrValidAddressMask  The valid address mask for the MTRR

**/
STATIC
VOID
MtrrLibInitializeMtrrMask (
  OUT UINT64 *MtrrValidBitsMask,
  OUT UINT64 *MtrrValidAddressMask
  )
{
  UINT32                              RegEax;
  UINT8                               PhysicalAddressBits;

  AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);

  if (RegEax >= 0x80000008) {
    AsmCpuid (0x80000008, &RegEax, NULL, NULL, NULL);

    PhysicalAddressBits = (UINT8) RegEax;

    *MtrrValidBitsMask    = LShiftU64 (1, PhysicalAddressBits) - 1;
    *MtrrValidAddressMask = *MtrrValidBitsMask & 0xfffffffffffff000ULL;
  } else {
    *MtrrValidBitsMask    = MTRR_LIB_CACHE_VALID_ADDRESS;
    *MtrrValidAddressMask = 0xFFFFFFFF;
  }
}


/**
  Determing the real attribute of a memory range.

  This function is to arbitrate the real attribute of the memory when
  there are 2 MTRR covers the same memory range.  For further details,
  please refer the IA32 Software Developer's Manual, Volume 3,
  Section 10.11.4.1.

  @param  MtrrType1    the first kind of Memory type
  @param  MtrrType2    the second kind of memory type

**/
UINT64
MtrrPrecedence (
  UINT64    MtrrType1,
  UINT64    MtrrType2
  )
{
  UINT64 MtrrType;

  MtrrType = MTRR_CACHE_INVALID_TYPE;
  switch (MtrrType1) {
  case MTRR_CACHE_UNCACHEABLE:
    MtrrType = MTRR_CACHE_UNCACHEABLE;
    break;
  case MTRR_CACHE_WRITE_COMBINING:
    if (
         MtrrType2==MTRR_CACHE_WRITE_COMBINING ||
         MtrrType2==MTRR_CACHE_UNCACHEABLE
       ) {
      MtrrType = MtrrType2;
    }
    break;
  case MTRR_CACHE_WRITE_THROUGH:
    if (
         MtrrType2==MTRR_CACHE_WRITE_THROUGH ||
         MtrrType2==MTRR_CACHE_WRITE_BACK
       ) {
      MtrrType = MTRR_CACHE_WRITE_THROUGH;
    } else if(MtrrType2==MTRR_CACHE_UNCACHEABLE) {
      MtrrType = MTRR_CACHE_UNCACHEABLE;
    }
    break;
  case MTRR_CACHE_WRITE_PROTECTED:
    if (MtrrType2 == MTRR_CACHE_WRITE_PROTECTED ||
        MtrrType2 == MTRR_CACHE_UNCACHEABLE) {
      MtrrType = MtrrType2;
    }
    break;
  case MTRR_CACHE_WRITE_BACK:
    if (
         MtrrType2== MTRR_CACHE_UNCACHEABLE ||
         MtrrType2==MTRR_CACHE_WRITE_THROUGH ||
         MtrrType2== MTRR_CACHE_WRITE_BACK
       ) {
      MtrrType = MtrrType2;
    }
    break;
  case MTRR_CACHE_INVALID_TYPE:
    MtrrType = MtrrType2;
    break;
  default:
    break;
  }

  if (MtrrType2 == MTRR_CACHE_INVALID_TYPE) {
    MtrrType = MtrrType1;
  }
  return MtrrType;
}


/**
  This function attempts to set the attributes for a memory range.

  @param  BaseAddress            The physical address that is the start
                                 address of a memory region.
  @param  Length                 The size in bytes of the memory region.
  @param  Attributes             The bit mask of attributes to set for the
                                 memory region.

  @retval RETURN_SUCCESS            The attributes were set for the memory
                                    region.
  @retval RETURN_INVALID_PARAMETER  Length is zero.
  @retval RETURN_UNSUPPORTED        The processor does not support one or
                                    more bytes of the memory resource range
                                    specified by BaseAddress and Length.
  @retval RETURN_UNSUPPORTED        The bit mask of attributes is not support
                                    for the memory resource range specified
                                    by BaseAddress and Length.
  @retval RETURN_ACCESS_DENIED      The attributes for the memory resource
                                    range specified by BaseAddress and Length
                                    cannot be modified.
  @retval RETURN_OUT_OF_RESOURCES   There are not enough system resources to
                                    modify the attributes of the memory
                                    resource range.

**/
RETURN_STATUS
EFIAPI
MtrrSetMemoryAttribute (
  IN PHYSICAL_ADDRESS        BaseAddress,
  IN UINT64                  Length,
  IN MTRR_MEMORY_CACHE_TYPE  Attribute
  )
{
  UINT64                    TempQword;
  RETURN_STATUS             Status;
  UINT64                    MemoryType;
  UINT64                    Remainder;
  BOOLEAN                   OverLap;
  BOOLEAN                   Positive;
  UINT32                    MsrNum;
  UINTN                     MtrrNumber;
  VARIABLE_MTRR             VariableMtrr[MTRR_NUMBER_OF_VARIABLE_MTRR];
  UINT32                    UsedMtrr;
  UINT64                    MtrrValidBitsMask;
  UINT64                    MtrrValidAddressMask;
  UINTN                     Cr4;
  BOOLEAN                   OverwriteExistingMtrr;
  UINT32                    FirmwareVariableMtrrCount;
  UINT32                    VariableMtrrEnd;

  if (!IsMtrrSupported ()) {
    return RETURN_UNSUPPORTED;
  }

  FirmwareVariableMtrrCount = GetFirmwareVariableMtrrCount ();
  VariableMtrrEnd = MTRR_LIB_IA32_VARIABLE_MTRR_BASE + (2 * GetVariableMtrrCount ()) - 1;

  MtrrLibInitializeMtrrMask(&MtrrValidBitsMask, &MtrrValidAddressMask);

  TempQword = 0;
  MemoryType = (UINT64)Attribute;
  OverwriteExistingMtrr = FALSE;

  //
  // Check for an invalid parameter
  //
  if (Length == 0) {
    return RETURN_INVALID_PARAMETER;
  }

  if (
       (BaseAddress &~MtrrValidAddressMask) != 0 ||
       (Length &~MtrrValidAddressMask) != 0
     ) {
    return RETURN_UNSUPPORTED;
  }

  //
  // Check if Fixed MTRR
  //
  Status = RETURN_SUCCESS;
  while ((BaseAddress < BASE_1MB) && (Length > 0) && Status == RETURN_SUCCESS) {
    Cr4 = PreMtrrChange ();
    Status = ProgramFixedMtrr (MemoryType, &BaseAddress, &Length);
    PostMtrrChange (Cr4);
    if (RETURN_ERROR (Status)) {
      return Status;
    }
  }

  if (Length == 0) {
    //
    // A Length of 0 can only make sense for fixed MTTR ranges.
    // Since we just handled the fixed MTRRs, we can skip the
    // variable MTRR section.
    //
    goto Done;
  }

  //
  // Since memory ranges below 1MB will be overridden by the fixed MTRRs,
  // we can set the bade to 0 to save variable MTRRs.
  //
  if (BaseAddress == BASE_1MB) {
    BaseAddress = 0;
    Length += SIZE_1MB;
  }

  //
  // Check memory base address alignment
  //
  DivU64x64Remainder (BaseAddress, Power2MaxMemory (LShiftU64 (Length, 1)), &Remainder);
  if (Remainder != 0) {
    DivU64x64Remainder (BaseAddress, Power2MaxMemory (Length), &Remainder);
    if (Remainder != 0) {
      Status = RETURN_UNSUPPORTED;
      goto Done;
    }
  }

  //
  // Check for overlap
  //
  UsedMtrr = MtrrGetMemoryAttributeInVariableMtrr (MtrrValidBitsMask, MtrrValidAddressMask, VariableMtrr);
  OverLap = CheckMemoryAttributeOverlap (BaseAddress, BaseAddress + Length - 1, VariableMtrr);
  if (OverLap) {
    Status = CombineMemoryAttribute (MemoryType, &BaseAddress, &Length, VariableMtrr, &UsedMtrr, &OverwriteExistingMtrr);
    if (RETURN_ERROR (Status)) {
      goto Done;
    }

    if (Length == 0) {
      //
      // Combined successfully
      //
      Status = RETURN_SUCCESS;
      goto Done;
    }
  }

  //
  // Program Variable MTRRs
  //
  // Avoid hardcode here and read data dynamically
  //
  if (UsedMtrr >= FirmwareVariableMtrrCount) {
    Status = RETURN_OUT_OF_RESOURCES;
    goto Done;
  }

  //
  // The memory type is the same with the type specified by
  // MTRR_LIB_IA32_MTRR_DEF_TYPE.
  //
  if ((!OverwriteExistingMtrr) && (Attribute == GetMtrrDefaultMemoryType ())) {
    //
    // Invalidate the now-unused MTRRs
    //
    InvalidateMtrr(VariableMtrr);
    goto Done;
  }

  TempQword = Length;


  if (TempQword == Power2MaxMemory (TempQword)) {
    //
    // Invalidate the now-unused MTRRs
    //
    InvalidateMtrr(VariableMtrr);

    //
    // Find first unused MTRR
    //
    for (MsrNum = MTRR_LIB_IA32_VARIABLE_MTRR_BASE;
         MsrNum < VariableMtrrEnd;
         MsrNum += 2
        ) {
      if ((AsmReadMsr64 (MsrNum + 1) & MTRR_LIB_CACHE_MTRR_ENABLED) == 0) {
        break;
      }
    }

    ProgramVariableMtrr (
      MsrNum,
      BaseAddress,
      Length,
      MemoryType,
      MtrrValidAddressMask
      );
  } else {

    Positive = GetDirection (TempQword, &MtrrNumber);

    if ((UsedMtrr + MtrrNumber) > FirmwareVariableMtrrCount) {
      Status = RETURN_OUT_OF_RESOURCES;
      goto Done;
    }

    //
    // Invalidate the now-unused MTRRs
    //
    InvalidateMtrr(VariableMtrr);

    //
    // Find first unused MTRR
    //
    for (MsrNum = MTRR_LIB_IA32_VARIABLE_MTRR_BASE;
         MsrNum < VariableMtrrEnd;
         MsrNum += 2
        ) {
      if ((AsmReadMsr64 (MsrNum + 1) & MTRR_LIB_CACHE_MTRR_ENABLED) == 0) {
        break;
      }
    }

    if (!Positive) {
      Length = Power2MaxMemory (LShiftU64 (TempQword, 1));
      ProgramVariableMtrr (
        MsrNum,
        BaseAddress,
        Length,
        MemoryType,
        MtrrValidAddressMask
        );
      BaseAddress += Length;
      TempQword   = Length - TempQword;
      MemoryType  = MTRR_CACHE_UNCACHEABLE;
    }

    do {
      //
      // Find unused MTRR
      //
      for (; MsrNum < VariableMtrrEnd; MsrNum += 2) {
        if ((AsmReadMsr64 (MsrNum + 1) & MTRR_LIB_CACHE_MTRR_ENABLED) == 0) {
          break;
        }
      }

      Length = Power2MaxMemory (TempQword);
      if (!Positive) {
        BaseAddress -= Length;
      }

      ProgramVariableMtrr (
        MsrNum,
        BaseAddress,
        Length,
        MemoryType,
        MtrrValidAddressMask
        );

      if (Positive) {
        BaseAddress += Length;
      }
      TempQword -= Length;

    } while (TempQword > 0);
  }

Done:
  return Status;

}


/**
  This function will get the memory cache type of the specific address.

  This function is mainly for debug purpose.

  @param  Address   The specific address

  @return Memory cache type of the sepcific address

**/
MTRR_MEMORY_CACHE_TYPE
EFIAPI
MtrrGetMemoryAttribute (
  IN PHYSICAL_ADDRESS   Address
  )
{
  UINT64                  TempQword;
  UINTN                   Index;
  UINTN                   SubIndex;
  UINT64                  MtrrType;
  UINT64                  TempMtrrType;
  MTRR_MEMORY_CACHE_TYPE  CacheType;
  VARIABLE_MTRR           VariableMtrr[MTRR_NUMBER_OF_VARIABLE_MTRR];
  UINT64                  MtrrValidBitsMask;
  UINT64                  MtrrValidAddressMask;
  UINTN                   VariableMtrrCount;

  if (!IsMtrrSupported ()) {
    return CacheUncacheable;
  }

  //
  // Check if MTRR is enabled, if not, return UC as attribute
  //
  TempQword = AsmReadMsr64 (MTRR_LIB_IA32_MTRR_DEF_TYPE);
  MtrrType = MTRR_CACHE_INVALID_TYPE;

  if ((TempQword & MTRR_LIB_CACHE_MTRR_ENABLED) == 0) {
    return CacheUncacheable;
  }

  //
  // If address is less than 1M, then try to go through the fixed MTRR
  //
  if (Address < BASE_1MB) {
    if ((TempQword & MTRR_LIB_CACHE_FIXED_MTRR_ENABLED) != 0) {
      //
      // Go through the fixed MTRR
      //
      for (Index = 0; Index < MTRR_NUMBER_OF_FIXED_MTRR; Index++) {
         if (Address >= MtrrLibFixedMtrrTable[Index].BaseAddress &&
             Address  < (
                          MtrrLibFixedMtrrTable[Index].BaseAddress +
                          (MtrrLibFixedMtrrTable[Index].Length * 8)
                        )
            ) {
           SubIndex =
             ((UINTN)Address - MtrrLibFixedMtrrTable[Index].BaseAddress) /
               MtrrLibFixedMtrrTable[Index].Length;
           TempQword = AsmReadMsr64 (MtrrLibFixedMtrrTable[Index].Msr);
           MtrrType =  RShiftU64 (TempQword, SubIndex * 8) & 0xFF;
           return GetMemoryCacheTypeFromMtrrType (MtrrType);
         }
      }
    }
  }
  MtrrLibInitializeMtrrMask(&MtrrValidBitsMask, &MtrrValidAddressMask);
  MtrrGetMemoryAttributeInVariableMtrr(
    MtrrValidBitsMask,
    MtrrValidAddressMask,
    VariableMtrr
    );

  //
  // Go through the variable MTRR
  //
  VariableMtrrCount = GetVariableMtrrCount ();
  ASSERT (VariableMtrrCount <= MTRR_NUMBER_OF_VARIABLE_MTRR);

  for (Index = 0; Index < VariableMtrrCount; Index++) {
    if (VariableMtrr[Index].Valid) {
      if (Address >= VariableMtrr[Index].BaseAddress &&
          Address < VariableMtrr[Index].BaseAddress+VariableMtrr[Index].Length) {
        TempMtrrType = VariableMtrr[Index].Type;
        MtrrType = MtrrPrecedence (MtrrType, TempMtrrType);
      }
    }
  }
  CacheType = GetMemoryCacheTypeFromMtrrType (MtrrType);

  return CacheType;
}


/**
  This function will get the raw value in variable MTRRs

  @param  VariableSettings   A buffer to hold variable MTRRs content.

  @return The VariableSettings input pointer

**/
MTRR_VARIABLE_SETTINGS*
EFIAPI
MtrrGetVariableMtrr (
  OUT MTRR_VARIABLE_SETTINGS         *VariableSettings
  )
{
  UINT32  Index;
  UINT32  VariableMtrrCount;

  if (!IsMtrrSupported ()) {
    return VariableSettings;
  }

  VariableMtrrCount = GetVariableMtrrCount ();
  ASSERT (VariableMtrrCount <= MTRR_NUMBER_OF_VARIABLE_MTRR);

  for (Index = 0; Index < VariableMtrrCount; Index++) {
    VariableSettings->Mtrr[Index].Base =
      AsmReadMsr64 (MTRR_LIB_IA32_VARIABLE_MTRR_BASE + (Index << 1));
    VariableSettings->Mtrr[Index].Mask =
      AsmReadMsr64 (MTRR_LIB_IA32_VARIABLE_MTRR_BASE + (Index << 1) + 1);
  }

  return  VariableSettings;
}


/**
  Worker function setting variable MTRRs

  @param  VariableSettings   A buffer to hold variable MTRRs content.

**/
VOID
MtrrSetVariableMtrrWorker (
  IN MTRR_VARIABLE_SETTINGS         *VariableSettings
  )
{
  UINT32  Index;
  UINT32  VariableMtrrCount;

  VariableMtrrCount = GetVariableMtrrCount ();
  ASSERT (VariableMtrrCount <= MTRR_NUMBER_OF_VARIABLE_MTRR);

  for (Index = 0; Index < VariableMtrrCount; Index++) {
    AsmWriteMsr64 (
      MTRR_LIB_IA32_VARIABLE_MTRR_BASE + (Index << 1),
      VariableSettings->Mtrr[Index].Base
      );
    AsmWriteMsr64 (
      MTRR_LIB_IA32_VARIABLE_MTRR_BASE + (Index << 1) + 1,
      VariableSettings->Mtrr[Index].Mask
      );
  }
}


/**
  This function sets variable MTRRs

  @param  VariableSettings   A buffer to hold variable MTRRs content.

  @return The pointer of VariableSettings

**/
MTRR_VARIABLE_SETTINGS*
EFIAPI
MtrrSetVariableMtrr (
  IN MTRR_VARIABLE_SETTINGS         *VariableSettings
  )
{
  UINTN  Cr4;

  if (!IsMtrrSupported ()) {
    return VariableSettings;
  }

  Cr4 = PreMtrrChange ();
  MtrrSetVariableMtrrWorker (VariableSettings);
  PostMtrrChange (Cr4);
  return  VariableSettings;
}


/**
  This function gets the content in fixed MTRRs

  @param  FixedSettings  A buffer to hold fixed Mtrrs content.

  @retval The pointer of FixedSettings

**/
MTRR_FIXED_SETTINGS*
EFIAPI
MtrrGetFixedMtrr (
  OUT MTRR_FIXED_SETTINGS         *FixedSettings
  )
{
  UINT32  Index;

  if (!IsMtrrSupported ()) {
    return FixedSettings;
  }

  for (Index = 0; Index < MTRR_NUMBER_OF_FIXED_MTRR; Index++) {
      FixedSettings->Mtrr[Index] =
        AsmReadMsr64 (MtrrLibFixedMtrrTable[Index].Msr);
  };

  return FixedSettings;
}

/**
  Worker function setting fixed MTRRs

  @param  FixedSettings  A buffer to hold fixed Mtrrs content.

**/
VOID
MtrrSetFixedMtrrWorker (
  IN MTRR_FIXED_SETTINGS          *FixedSettings
  )
{
  UINT32  Index;

  for (Index = 0; Index < MTRR_NUMBER_OF_FIXED_MTRR; Index++) {
     AsmWriteMsr64 (
       MtrrLibFixedMtrrTable[Index].Msr,
       FixedSettings->Mtrr[Index]
       );
  }
}


/**
  This function sets fixed MTRRs

  @param  FixedSettings  A buffer to hold fixed Mtrrs content.

  @retval The pointer of FixedSettings

**/
MTRR_FIXED_SETTINGS*
EFIAPI
MtrrSetFixedMtrr (
  IN MTRR_FIXED_SETTINGS          *FixedSettings
  )
{
  UINTN  Cr4;

  if (!IsMtrrSupported ()) {
    return FixedSettings;
  }

  Cr4 = PreMtrrChange ();
  MtrrSetFixedMtrrWorker (FixedSettings);
  PostMtrrChange (Cr4);

  return FixedSettings;
}


/**
  This function gets the content in all MTRRs (variable and fixed)

  @param  MtrrSetting  A buffer to hold all Mtrrs content.

  @retval the pointer of MtrrSetting

**/
MTRR_SETTINGS *
EFIAPI
MtrrGetAllMtrrs (
  OUT MTRR_SETTINGS                *MtrrSetting
  )
{
  if (!IsMtrrSupported ()) {
    return MtrrSetting;
  }

  //
  // Get fixed MTRRs
  //
  MtrrGetFixedMtrr (&MtrrSetting->Fixed);

  //
  // Get variable MTRRs
  //
  MtrrGetVariableMtrr (&MtrrSetting->Variables);

  //
  // Get MTRR_DEF_TYPE value
  //
  MtrrSetting->MtrrDefType = AsmReadMsr64 (MTRR_LIB_IA32_MTRR_DEF_TYPE);

  return MtrrSetting;
}


/**
  This function sets all MTRRs (variable and fixed)

  @param  MtrrSetting  A buffer holding all MTRRs content.

  @retval The pointer of MtrrSetting

**/
MTRR_SETTINGS *
EFIAPI
MtrrSetAllMtrrs (
  IN MTRR_SETTINGS                *MtrrSetting
  )
{
  UINTN  Cr4;

  if (!IsMtrrSupported ()) {
    return MtrrSetting;
  }

  Cr4 = PreMtrrChange ();

  //
  // Set fixed MTRRs
  //
  MtrrSetFixedMtrrWorker (&MtrrSetting->Fixed);

  //
  // Set variable MTRRs
  //
  MtrrSetVariableMtrrWorker (&MtrrSetting->Variables);

  //
  // Set MTRR_DEF_TYPE value
  //
  AsmWriteMsr64 (MTRR_LIB_IA32_MTRR_DEF_TYPE, MtrrSetting->MtrrDefType);

  PostMtrrChange (Cr4);

  return MtrrSetting;
}


/**
  This function prints all MTRRs for debugging.
**/
VOID
MtrrDebugPrintAllMtrrs (
  )
{
  DEBUG_CODE (
    {
      MTRR_SETTINGS  MtrrSettings;
      UINTN          Index;
      UINTN          VariableMtrrCount;

      if (!IsMtrrSupported ()) {
        return;
      }

      MtrrGetAllMtrrs (&MtrrSettings);
      DEBUG((EFI_D_ERROR, "DefaultType = %016lx\n", MtrrSettings.MtrrDefType));
      for (Index = 0; Index < MTRR_NUMBER_OF_FIXED_MTRR; Index++) {
        DEBUG((
          EFI_D_ERROR, "Fixed[%02d] = %016lx\n",
          Index,
          MtrrSettings.Fixed.Mtrr[Index]
          ));
      }

      VariableMtrrCount = GetVariableMtrrCount ();
      for (Index = 0; Index < VariableMtrrCount; Index++) {
        DEBUG((
          EFI_D_ERROR, "Variable[%02d] = %016lx, %016lx\n",
          Index,
          MtrrSettings.Variables.Mtrr[Index].Base,
          MtrrSettings.Variables.Mtrr[Index].Mask
          ));
      }
    }
  );
}

/**
  Checks if MTRR is supported.

  @retval TRUE  MTRR is supported.
  @retval FALSE MTRR is not supported.

**/
BOOLEAN
EFIAPI
IsMtrrSupported (
  VOID
  )
{
  UINT32  RegEdx;
  UINT64  MtrrCap;

  //
  // Check CPUID(1).EDX[12] for MTRR capability
  //
  AsmCpuid (1, NULL, NULL, NULL, &RegEdx);
  if (BitFieldRead32 (RegEdx, 12, 12) == 0) {
    return FALSE;
  }

  //
  // Check IA32_MTRRCAP.[0..7] for number of variable MTRRs and IA32_MTRRCAP[8] for
  // fixed MTRRs existence. If number of variable MTRRs is zero, or fixed MTRRs do not
  // exist, return false.
  //
  MtrrCap = AsmReadMsr64 (MTRR_LIB_IA32_MTRR_CAP);
  if  ((BitFieldRead64 (MtrrCap, 0, 7) == 0) || (BitFieldRead64 (MtrrCap, 8, 8) == 0)) {
    return FALSE;
  }

  return TRUE;
}
