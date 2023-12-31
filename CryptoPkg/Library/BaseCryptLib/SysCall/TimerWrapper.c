/** @file
  C Run-Time Libraries (CRT) Time Management Routines Wrapper Implementation
  for OpenSSL-based Cryptographic Library (used in DXE & RUNTIME).

Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <OpenSslSupport.h>
#include <Library/UefiRuntimeServicesTableLib.h>

//
// -- Time Management Routines --
//

#define IsLeap(y)   (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))
#define SECSPERHOUR (60 * 60)
#define SECSPERDAY  (24 * SECSPERHOUR)

//
//  The arrays give the cumulative number of days up to the first of the
//  month number used as the index (1 -> 12) for regular and leap years.
//  The value at index 13 is for the whole year.
//
UINTN CumulativeDays[2][14] = {
  {
    0,
    0,
    31,
    31 + 28,
    31 + 28 + 31,
    31 + 28 + 31 + 30,
    31 + 28 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31
  },
  {
    0,
    0,
    31,
    31 + 29,
    31 + 29 + 31,
    31 + 29 + 31 + 30,
    31 + 29 + 31 + 30 + 31,
    31 + 29 + 31 + 30 + 31 + 30,
    31 + 29 + 31 + 30 + 31 + 30 + 31,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31 
  }
};

/* Get the system time as seconds elapsed since midnight, January 1, 1970. */
//INTN time(
//  INTN *timer
//  )
time_t time (time_t *timer)
{
  EFI_TIME  Time;
  UINTN     Year;

  //
  // Get the current time and date information
  //
  gRT->GetTime (&Time, NULL);

  //
  // Years Handling
  // UTime should now be set to 00:00:00 on Jan 1 of the current year.
  //
  for (Year = 1970, *timer = 0; Year != Time.Year; Year++) {
    *timer = *timer + (time_t)(CumulativeDays[IsLeap(Year)][13] * SECSPERDAY);
  }

  //
  // Add in number of seconds for current Month, Day, Hour, Minute, Seconds, and TimeZone adjustment
  //
  *timer = *timer + 
           (time_t)((Time.TimeZone != EFI_UNSPECIFIED_TIMEZONE) ? (Time.TimeZone * 60) : 0) +
           (time_t)(CumulativeDays[IsLeap(Time.Year)][Time.Month] * SECSPERDAY) + 
           (time_t)(((Time.Day > 0) ? Time.Day - 1 : 0) * SECSPERDAY) + 
           (time_t)(Time.Hour * SECSPERHOUR) + 
           (time_t)(Time.Minute * 60) + 
           (time_t)Time.Second;

  return *timer;
}
