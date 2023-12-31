/** @file
  This file defines performance-related definitions, including the format of:
  * performance GUID HOB.
  * performance protocol interfaces.
  * performance variables.  

Copyright (c) 2009 - 2010, Intel Corporation.  All rights reserved<BR>
This program and the accompanying materials are licensed and made available under 
the terms and conditions of the BSD License that accompanies this distribution.  
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.                                            

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PERFORMANCE_DATA_H__
#define __PERFORMANCE_DATA_H__

//
// PEI_PERFORMANCE_STRING_SIZE must be a multiple of 8.
//
#define PEI_PERFORMANCE_STRING_SIZE     8
#define PEI_PERFORMANCE_STRING_LENGTH   (PEI_PERFORMANCE_STRING_SIZE - 1)

typedef struct {
  EFI_PHYSICAL_ADDRESS  Handle;
  CHAR8                 Token[PEI_PERFORMANCE_STRING_SIZE];   ///< Measured token string name. 
  CHAR8                 Module[PEI_PERFORMANCE_STRING_SIZE];  ///< Module string name.
  UINT64                StartTimeStamp;                       ///< Start time point.
  UINT64                EndTimeStamp;                         ///< End time point.
} PEI_PERFORMANCE_LOG_ENTRY;

//
// The header must be aligned at 8 bytes.
// 
typedef struct {
  UINT32                NumberOfEntries;  ///< The number of all performance log entries.
  UINT32                Reserved;
} PEI_PERFORMANCE_LOG_HEADER;


//
// The data structure for performance data in ACPI memory.
//
#define PERFORMANCE_SIGNATURE   SIGNATURE_32 ('P', 'e', 'r', 'f')
#define PERF_TOKEN_SIZE         28
#define PERF_TOKEN_LENGTH       (PERF_TOKEN_SIZE - 1)
#define PERF_PEI_ENTRY_MAX_NUM  50

typedef struct {
  CHAR8   Token[PERF_TOKEN_SIZE];
  UINT32  Duration;
} PERF_DATA;

typedef struct {
  UINT64        BootToOs;
  UINT64        S3Resume;
  UINT32        S3EntryNum;
  PERF_DATA     S3Entry[PERF_PEI_ENTRY_MAX_NUM];
  UINT64        CpuFreq;
  UINT64        BDSRaw;
  UINT32        Count;
  UINT32        Signiture;
} PERF_HEADER;

#define PERFORMANCE_PROTOCOL_GUID \
  { 0x76b6bdfa, 0x2acd, 0x4462, {0x9E, 0x3F, 0xcb, 0x58, 0xC9, 0x69, 0xd9, 0x37 } }

//
// Forward reference for pure ANSI compatibility
//
typedef struct _PERFORMANCE_PROTOCOL PERFORMANCE_PROTOCOL;

//
// DXE_PERFORMANCE_STRING_SIZE must be a multiple of 8.
//
#define DXE_PERFORMANCE_STRING_SIZE     32
#define DXE_PERFORMANCE_STRING_LENGTH   (DXE_PERFORMANCE_STRING_SIZE - 1)

//
// The default guage entries number for DXE phase.
//
#define INIT_DXE_GAUGE_DATA_ENTRIES     800

typedef struct {
  EFI_PHYSICAL_ADDRESS  Handle;
  CHAR8                 Token[DXE_PERFORMANCE_STRING_SIZE];  ///< Measured token string name. 
  CHAR8                 Module[DXE_PERFORMANCE_STRING_SIZE]; ///< Module string name.
  UINT64                StartTimeStamp;                      ///< Start time point.
  UINT64                EndTimeStamp;                        ///< End time point.
} GAUGE_DATA_ENTRY;

//
// The header must be aligned at 8 bytes
//
typedef struct {
  UINT32                NumberOfEntries; ///< The number of all performance gauge entries.
  UINT32                Reserved;
} GAUGE_DATA_HEADER;

/**
  Adds a record at the end of the performance measurement log
  that records the start time of a performance measurement.

  The added record contains the Handle, Token, and Module.
  The end time of the new record is not recorded, so it is set to zero.
  If TimeStamp is not zero, then TimeStamp is used to fill in the start time in the record.
  If TimeStamp is zero, the start time in the record is filled in with the value
  read from the current time stamp.

  @param  Handle                  The pointer to environment specific context used
                                  to identify the component being measured.
  @param  Token                   The pointer to a Null-terminated ASCII string
                                  that identifies the component being measured.
  @param  Module                  The pointer to a Null-terminated ASCII string
                                  that identifies the module being measured.
  @param  TimeStamp               The 64-bit time stamp.

  @retval EFI_SUCCESS             The data was read correctly from the device.
  @retval EFI_OUT_OF_RESOURCES    There are not enough resources to record the measurement.

**/
typedef
EFI_STATUS
(EFIAPI * PERFORMANCE_START_GAUGE)(
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp
  );

/**
  Searches the performance measurement log from the beginning of the log
  for the first matching record that contains a zero end time and fills in a valid end time.

  Searches the performance measurement log from the beginning of the log
  for the first record that matches Handle, Token, and Module, and has an end time value of zero.
  If the record can not be found then return EFI_NOT_FOUND.
  If the record is found and TimeStamp is not zero,
  then the end time in the record is filled in with the value specified by TimeStamp.
  If the record is found and TimeStamp is zero, then the end time in the matching record
  is filled in with the current time stamp value.

  @param  Handle                  The pointer to environment specific context used
                                  to identify the component being measured.
  @param  Token                   The pointer to a Null-terminated ASCII string
                                  that identifies the component being measured.
  @param  Module                  The pointer to a Null-terminated ASCII string
                                  that identifies the module being measured.
  @param  TimeStamp               The 64-bit time stamp.

  @retval EFI_SUCCESS             The end of  the measurement was recorded.
  @retval EFI_NOT_FOUND           The specified measurement record could not be found.

**/
typedef
EFI_STATUS
(EFIAPI * PERFORMANCE_END_GAUGE)(
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp
  );

/**
  Retrieves a previously logged performance measurement.

  Retrieves the performance log entry from the performance log specified by LogEntryKey.
  If it stands for a valid entry, then EFI_SUCCESS is returned and
  GaugeDataEntry stores the pointer to that entry.

  @param  LogEntryKey             The key for the previous performance measurement log entry.
                                  If 0, then the first performance measurement log entry is retrieved.
  @param  GaugeDataEntry          Out parameter for the indirect pointer to the gauge data entry specified by LogEntryKey.
                                  
  @retval EFI_SUCCESS             The GuageDataEntry is successfully found based on LogEntryKey.
  @retval EFI_NOT_FOUND           There is no entry after the measurement referred to by LogEntryKey.
  @retval EFI_INVALID_PARAMETER   The LogEntryKey is not a valid entry, or GaugeDataEntry is NULL.

**/
typedef
EFI_STATUS
(EFIAPI * PERFORMANCE_GET_GAUGE)(
  IN  UINTN               LogEntryKey,
  OUT GAUGE_DATA_ENTRY    **GaugeDataEntry
  );

struct _PERFORMANCE_PROTOCOL {
  PERFORMANCE_START_GAUGE             StartGauge;
  PERFORMANCE_END_GAUGE               EndGauge;
  PERFORMANCE_GET_GAUGE               GetGauge;
};

extern EFI_GUID gPerformanceProtocolGuid;

#endif
