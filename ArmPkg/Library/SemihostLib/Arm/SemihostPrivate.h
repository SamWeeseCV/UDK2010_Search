/** @file

  Copyright (c) 2008-2009 Apple Inc. All rights reserved.<BR>

  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __SEMIHOST_PRIVATE_H__
#define __SEMIHOST_PRIVATE_H__

typedef struct {
  CHAR8   *FileName;
  UINT32  Mode;
  UINT32  NameLength;
} SEMIHOST_FILE_OPEN_BLOCK;

typedef struct {
  UINT32  Handle;
  VOID    *Buffer;
  UINT32  Length;
} SEMIHOST_FILE_READ_WRITE_BLOCK;

typedef struct {
  UINT32  Handle;
  UINT32  Location;
} SEMIHOST_FILE_SEEK_BLOCK;

typedef struct {
  CHAR8   *FileName;
  UINT32  NameLength;
} SEMIHOST_FILE_REMOVE_BLOCK;

typedef struct {
  CHAR8   *CommandLine;
  UINT32  CommandLength;
} SEMIHOST_SYSTEM_BLOCK;

#ifdef __CC_ARM 

#if defined(__thumb__)
#define SWI 0xAB
#else
#define SWI 0x123456
#endif

#define SEMIHOST_SUPPORTED  TRUE

__swi(SWI)
INT32
_Semihost_SYS_OPEN(
  IN UINTN                    SWI_0x01,
  IN SEMIHOST_FILE_OPEN_BLOCK *OpenBlock
  );

__swi(SWI)
INT32
_Semihost_SYS_CLOSE(
  IN UINTN  SWI_0x02,
  IN UINT32 *Handle
  );

__swi(SWI)
VOID
_Semihost_SYS_WRITEC(
  IN UINTN    SWI_0x03,
  IN CHAR8    *Character
  );

__swi(SWI)
VOID
_Semihost_SYS_WRITE0(
  IN UINTN SWI_0x04,
  IN CHAR8 *String
  );

__swi(SWI)
UINT32
_Semihost_SYS_WRITE(
  IN     UINTN                          SWI_0x05,
  IN OUT SEMIHOST_FILE_READ_WRITE_BLOCK *WriteBlock
  );

__swi(SWI)
UINT32
_Semihost_SYS_READ(
  IN     UINTN                          SWI_0x06,
  IN OUT SEMIHOST_FILE_READ_WRITE_BLOCK *ReadBlock
  );

__swi(SWI)
CHAR8
_Semihost_SYS_READC(
  IN     UINTN SWI_0x07,
  IN     UINTN Zero
  );

__swi(SWI)
INT32
_Semihost_SYS_SEEK(
  IN UINTN                    SWI_0x0A,
  IN SEMIHOST_FILE_SEEK_BLOCK *SeekBlock
  );

__swi(SWI)
INT32
_Semihost_SYS_FLEN(
  IN UINTN  SWI_0x0C,
  IN UINT32 *Handle
  );

__swi(SWI)
UINT32
_Semihost_SYS_REMOVE(
  IN UINTN                      SWI_0x0E,
  IN SEMIHOST_FILE_REMOVE_BLOCK *RemoveBlock
  );

__swi(SWI)
UINT32
_Semihost_SYS_SYSTEM(
  IN UINTN                 SWI_0x12,
  IN SEMIHOST_SYSTEM_BLOCK *SystemBlock
  );

#define Semihost_SYS_OPEN(OpenBlock)        _Semihost_SYS_OPEN(0x01, OpenBlock)
#define Semihost_SYS_CLOSE(Handle)          _Semihost_SYS_CLOSE(0x02, Handle)
#define Semihost_SYS_WRITE0(String)         _Semihost_SYS_WRITE0(0x04, String)
#define Semihost_SYS_WRITEC(Character)      _Semihost_SYS_WRITEC(0x03, Character)
#define Semihost_SYS_WRITE(WriteBlock)      _Semihost_SYS_WRITE(0x05, WriteBlock)
#define Semihost_SYS_READ(ReadBlock)        _Semihost_SYS_READ(0x06, ReadBlock)
#define Semihost_SYS_READC()                _Semihost_SYS_READC(0x07, 0)
#define Semihost_SYS_SEEK(SeekBlock)        _Semihost_SYS_SEEK(0x0A, SeekBlock)
#define Semihost_SYS_FLEN(Handle)           _Semihost_SYS_FLEN(0x0C, Handle)
#define Semihost_SYS_REMOVE(RemoveBlock)    _Semihost_SYS_REMOVE(0x0E, RemoveBlock)
#define Semihost_SYS_SYSTEM(SystemBlock)    _Semihost_SYS_SYSTEM(0x12, SystemBlock)

#else // __CC_ARM

#define SEMIHOST_SUPPORTED  FALSE

#define Semihost_SYS_OPEN(OpenBlock)        (-1)
#define Semihost_SYS_CLOSE(Handle)          (-1)
#define Semihost_SYS_WRITE0(String)
#define Semihost_SYS_WRITEC(Character)
#define Semihost_SYS_WRITE(WriteBlock)      (0)
#define Semihost_SYS_READ(ReadBlock)        ((ReadBlock)->Length)
#define Semihost_SYS_READC()                ('x')
#define Semihost_SYS_SEEK(SeekBlock)        (-1)
#define Semihost_SYS_FLEN(Handle)           (-1)
#define Semihost_SYS_REMOVE(RemoveBlock)    (-1)
#define Semihost_SYS_SYSTEM(SystemBlock)    (-1)

#endif // __CC_ARM

#endif //__SEMIHOST_PRIVATE_H__
