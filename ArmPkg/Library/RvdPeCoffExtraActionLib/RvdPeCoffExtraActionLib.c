/**@file

Copyright (c) 2006 - 2009, Intel Corporation
Portions copyright (c) 2008-2010 Apple Inc. All rights reserved.
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Library/PeCoffLib.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeCoffExtraActionLib.h>
#include <Library/SemihostLib.h>
#include <Library/PrintLib.h>

/**
  Append string to debugger script file, create file if needed. 

  This library can show up in mulitple places so we need to append the file every time we write to it.
  For example Sec can use this to load the DXE core, and the DXE core would use this to load all the
  other modules. So we have two instances of the library in the system.

  @param  Buffer  Buffer to write to file.
  @param  Length  Length of Buffer in bytes.
**/
VOID
WriteStringToFile (
  IN  VOID    *Buffer,
  IN  UINT32  Length
  )
{
  // Working around and issue with the code that is commented out. For now send it to the console.
  // You can copy the console into a file and source the file as a script and you get symbols. 
  // This gets you all the symbols except for SEC. To get SEC symbols you need to copy the 
  // debug print in the SEC into the debugger manually
  SemihostWriteString (Buffer);
/*
  I'm currently having issues with this code crashing the debugger. Seems like it should work.

  UINT32        SemihostHandle;
  UINT32        SemihostMode = SEMIHOST_FILE_MODE_WRITE | SEMIHOST_FILE_MODE_BINARY | SEMIHOST_FILE_MODE_CREATE;

  SemihostFileOpen ("c:\rvi_symbols.inc", SemihostMode, &SemihostHandle);
  SemihostFileWrite (SemihostHandle, &Length, Buffer);
  SemihostFileClose (SemihostHandle);
 */
}


/**
  If the build is done on cygwin the paths are cygpaths. 
  /cygdrive/c/tmp.txt vs c:\tmp.txt so we need to convert
  them to work with RVD commands

  @param  Name  Path to convert if needed

**/
CHAR8 *
DeCygwinPathIfNeeded (
  IN  CHAR8   *Name
  )
{
  CHAR8   *Ptr;
  UINTN   Index;
  UINTN   Len;
  
  Ptr = AsciiStrStr (Name, "/cygdrive/");
  if (Ptr == NULL) {
    return Name;
  }
  
  Len = AsciiStrLen (Ptr);
  
  // convert "/cygdrive" to spaces
  for (Index = 0; Index < 9; Index++) {
    Ptr[Index] = ' ';
  }

  // convert /c to c:
  Ptr[9]  = Ptr[10];
  Ptr[10] = ':';
  
  // switch path seperators
  for (Index = 11; Index < Len; Index++) {
    if (Ptr[Index] == '/') {
      Ptr[Index] = '\\' ;
    }
  }

  return Name;
}


/**
  Performs additional actions after a PE/COFF image has been loaded and relocated.

  If ImageContext is NULL, then ASSERT().

  @param  ImageContext  Pointer to the image context structure that describes the
                        PE/COFF image that has already been loaded and relocated.

**/
VOID
EFIAPI
PeCoffLoaderRelocateImageExtraAction (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  )
{
  CHAR8 Buffer[256];
  
  AsciiSPrint (Buffer, sizeof(Buffer), "load /a /ni /np %a &0x%08x\n", ImageContext->PdbPointer, (UINTN)(ImageContext->ImageAddress + ImageContext->SizeOfHeaders));
  DeCygwinPathIfNeeded (&Buffer[16]);
 
  WriteStringToFile (Buffer, AsciiStrSize (Buffer));
}



/**
  Performs additional actions just before a PE/COFF image is unloaded.  Any resources
  that were allocated by PeCoffLoaderRelocateImageExtraAction() must be freed.
  
  If ImageContext is NULL, then ASSERT().
  
  @param  ImageContext  Pointer to the image context structure that describes the
                        PE/COFF image that is being unloaded.

**/
VOID
EFIAPI
PeCoffLoaderUnloadImageExtraAction (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  )
{
  CHAR8 Buffer[256];
  
  AsciiSPrint (Buffer, sizeof(Buffer), "unload symbols_only %a", ImageContext->PdbPointer);
  DeCygwinPathIfNeeded (Buffer);
 
  WriteStringToFile (Buffer, AsciiStrSize (Buffer));
}
