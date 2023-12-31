/*++

Copyright (c) 2004 - 2009, Intel Corporation                                                         
Portions copyright (c) 2008-2009 Apple Inc.<BR>
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  UnixThunk.h

Abstract:

  This protocol allows an EFI driver in the Unix emulation environment
  to make Posix calls.

  NEVER make an Unix call directly, always make the call via this protocol.

  There are no This pointers on the protocol member functions as they map
  exactly into Unix system calls.

--*/

#ifndef _UNIX_THUNK_H_
#define _UNIX_THUNK_H_

#include <sys/termios.h>
#include <stdio.h>
#include <sys/time.h>

#if __CYGWIN__
#include <sys/dirent.h>
#else
#include <sys/dir.h>
#endif

#include <unistd.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#ifdef __APPLE__
#include <sys/param.h>
#include <sys/mount.h>
#define _XOPEN_SOURCE
#else
#include <termio.h>
#include <sys/vfs.h>
#endif 

#include <utime.h>
#include <dlfcn.h>
#include <ucontext.h>

#include <Base.h>
#include <Library/PeCoffLib.h>


#if __APPLE__
//
// EFI packing is not compatible witht he default OS packing for struct stat.
// st_size is 64-bit but starts on a 32-bit offset in the structure. The compiler
// flags used to produce compatible EFI images, break struct stat
//
#pragma pack(4)

#if __DARWIN_64_BIT_INO_T

typedef struct stat_fix { \
	dev_t		st_dev;			/* [XSI] ID of device containing file */ 
	mode_t		st_mode;		/* [XSI] Mode of file (see below) */ 
	nlink_t		st_nlink;		/* [XSI] Number of hard links */ 
	__darwin_ino64_t st_ino;		/* [XSI] File serial number */ 
	uid_t		st_uid;			/* [XSI] User ID of the file */ 
	gid_t		st_gid;			/* [XSI] Group ID of the file */ 
	dev_t		st_rdev;		/* [XSI] Device ID */ 
	__DARWIN_STRUCT_STAT64_TIMES 
	off_t		st_size;		/* [XSI] file size, in bytes */ 
	blkcnt_t	st_blocks;		/* [XSI] blocks allocated for file */ 
	blksize_t	st_blksize;		/* [XSI] optimal blocksize for I/O */ 
	__uint32_t	st_flags;		/* user defined flags for file */ 
	__uint32_t	st_gen;			/* file generation number */ 
	__int32_t	st_lspare;		/* RESERVED: DO NOT USE! */ 
	__int64_t	st_qspare[2];		/* RESERVED: DO NOT USE! */ 
} STAT_FIX;

#else /* !__DARWIN_64_BIT_INO_T */

typedef struct stat_fix {
	dev_t	 	st_dev;		/* [XSI] ID of device containing file */
	ino_t	  	st_ino;		/* [XSI] File serial number */
	mode_t	 	st_mode;	/* [XSI] Mode of file (see below) */
	nlink_t		st_nlink;	/* [XSI] Number of hard links */
	uid_t		st_uid;		/* [XSI] User ID of the file */
	gid_t		st_gid;		/* [XSI] Group ID of the file */
	dev_t		st_rdev;	/* [XSI] Device ID */
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
	struct	timespec st_atimespec;	/* time of last access */
	struct	timespec st_mtimespec;	/* time of last data modification */
	struct	timespec st_ctimespec;	/* time of last status change */
#else
	time_t		st_atime;	/* [XSI] Time of last access */
	long		st_atimensec;	/* nsec of last access */
	time_t		st_mtime;	/* [XSI] Last data modification time */
	long		st_mtimensec;	/* last data modification nsec */
	time_t		st_ctime;	/* [XSI] Time of last status change */
	long		st_ctimensec;	/* nsec of last status change */
#endif
	off_t		st_size;	/* [XSI] file size, in bytes */
	blkcnt_t	st_blocks;	/* [XSI] blocks allocated for file */
	blksize_t	st_blksize;	/* [XSI] optimal blocksize for I/O */
	__uint32_t	st_flags;	/* user defined flags for file */
	__uint32_t	st_gen;		/* file generation number */
	__int32_t	st_lspare;	/* RESERVED: DO NOT USE! */
	__int64_t	st_qspare[2];	/* RESERVED: DO NOT USE! */
} STAT_FIX;

#pragma pack()
#endif

#else 

  typedef struct stat STAT_FIX;

#endif

#define EFI_UNIX_THUNK_PROTOCOL_GUID \
  { \
    0xf2e98868, 0x8985, 0x11db, {0x9a, 0x59, 0x00, 0x40, 0xd0, 0x2b, 0x18, 0x35 } \
  }

typedef
VOID
(*UnixSleep) (
  unsigned long Milliseconds
  );

typedef
VOID
(*UnixExit) (
  int status  // exit code for all threads
  );

typedef
VOID
(*UnixSetTimer) (UINT64 PeriodMs, VOID (*CallBack)(UINT64 DeltaMs));
typedef
VOID
(*UnixGetLocalTime) (EFI_TIME *Time);
typedef
struct tm *
(*UnixGmTime)(const time_t *timep);
typedef
long
(*UnixGetTimeZone)(void);
typedef
int
(*UnixGetDayLight)(void);
typedef
int
(*UnixPoll)(struct pollfd *pfd, int nfds, int timeout);
typedef
int
(*UnixRead) (int fd, void *buf, int count);
typedef
int
(*UnixWrite) (int fd, const void *buf, int count);
typedef
char *
(*UnixGetenv) (const char *var);
typedef
int
(*UnixOpen) (const char *name, int flags, int mode);
typedef
off_t
(*UnixSeek) (int fd, off_t off, int whence);
typedef
int
(*UnixFtruncate) (int fd, long int len);
typedef
int
(*UnixClose) (int fd);

typedef
int
(*UnixMkdir)(const char *pathname, mode_t mode);
typedef
int
(*UnixRmDir)(const char *pathname);
typedef
int
(*UnixUnLink)(const char *pathname);
typedef
int
(*UnixGetErrno)(VOID);
typedef
DIR *
(*UnixOpenDir)(const char *pathname);
typedef
void
(*UnixRewindDir)(DIR *dir);
typedef
struct dirent *
(*UnixReadDir)(DIR *dir);
typedef
int
(*UnixCloseDir)(DIR *dir);
typedef
int
(*UnixStat)(const char *path, STAT_FIX *buf);
typedef
int
(*UnixStatFs)(const char *path, struct statfs *buf);
typedef
int
(*UnixRename)(const char *oldpath, const char *newpath);
typedef
time_t
(*UnixMkTime)(struct tm *tm);
typedef
int
(*UnixFSync)(int fd);
typedef
int
(*UnixChmod)(const char *path, mode_t mode);
typedef
int
(*UnixUTime)(const char *filename, const struct utimbuf *buf);

struct _EFI_UNIX_UGA_IO_PROTOCOL;
typedef
EFI_STATUS
(*UnixUgaCreate)(struct _EFI_UNIX_UGA_IO_PROTOCOL **UgaIo,
		 CONST CHAR16 *Title);

typedef
int
(*UnixTcflush) (int fildes, int queue_selector);

typedef
void
(*UnixPerror) (__const char *__s);

typedef 
int 
#if __CYGWIN__
(*UnixIoCtl) (int fd, int __request, ...);
#else
(*UnixIoCtl) (int fd, unsigned long int __request, ...);
#endif

typedef 
int 
(*UnixFcntl) (int __fd, int __cmd, ...);

typedef
int 
(*UnixCfsetispeed) (struct termios *__termios_p, speed_t __speed);

typedef 
int 
(*UnixCfsetospeed) (struct termios *__termios_p, speed_t __speed);

typedef
int 
(*UnixTcgetattr) (int __fd, struct termios *__termios_p);

typedef 
int 
(*UnixTcsetattr) (int __fd, int __optional_actions,
		      __const struct termios *__termios_p);

typedef 
VOID *
(*UnixDlopen) (const char *FileName, int Flag);

typedef
char *
(*UnixDlerror) (VOID);

typedef 
VOID *
(*UnixDlsym) (VOID* Handle, const char* Symbol);


//
// Work functions to enable source level debug in the emulator
//

typedef 
RETURN_STATUS
(EFIAPI *UnixPeCoffGetEntryPoint) (
  IN     VOID  *Pe32Data,
  IN OUT VOID  **EntryPoint
  );

typedef 
VOID
(EFIAPI *UnixPeCoffRelocateImageExtraAction) (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  );

typedef 
VOID
(EFIAPI *UnixPeCoffLoaderUnloadImageExtraAction) (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  );



#define EFI_UNIX_THUNK_PROTOCOL_SIGNATURE SIGNATURE_32 ('L', 'N', 'X', 'T')

typedef struct _EFI_UNIX_THUNK_PROTOCOL {
  UINT64                              Signature;

  UnixSleep                           Sleep;
  UnixExit                    	      Exit;
  UnixSetTimer                        SetTimer;
  UnixGetLocalTime		                GetLocalTime;
  UnixGmTime                          GmTime;
  UnixGetTimeZone                     GetTimeZone;
  UnixGetDayLight                     GetDayLight;
  UnixPoll	                          Poll;
  UnixRead                           Read;
  UnixWrite                          Write;
  UnixGetenv                         Getenv;
  UnixOpen                           Open;
  UnixSeek                           Lseek;
  UnixFtruncate                      FTruncate;
  UnixClose                          Close;
  UnixMkdir                           MkDir;
  UnixRmDir                           RmDir;
  UnixUnLink                          UnLink;
  UnixGetErrno                        GetErrno;
  UnixOpenDir                         OpenDir;
  UnixRewindDir                       RewindDir;
  UnixReadDir                         ReadDir;
  UnixCloseDir                        CloseDir;
  UnixStat                            Stat;
  UnixStatFs                          StatFs;
  UnixRename                          Rename;
  UnixMkTime                          MkTime;
  UnixFSync                           FSync;
  UnixChmod                           Chmod;
  UnixUTime                           UTime;
  UnixTcflush                         Tcflush;
  UnixUgaCreate			                  UgaCreate;
  UnixPerror                          Perror;
  UnixIoCtl                           IoCtl;
  UnixFcntl                           Fcntl;
  UnixCfsetispeed                     Cfsetispeed;
  UnixCfsetospeed                     Cfsetospeed;
  UnixTcgetattr                       Tcgetattr;
  UnixTcsetattr                       Tcsetattr;
  UnixDlopen                          Dlopen;
  UnixDlerror                         Dlerror;
  UnixDlsym                           Dlsym;
  UnixPeCoffGetEntryPoint                 PeCoffGetEntryPoint;
  UnixPeCoffRelocateImageExtraAction      PeCoffRelocateImageExtraAction;
  UnixPeCoffLoaderUnloadImageExtraAction  PeCoffUnloadImageExtraAction;
	
	
} EFI_UNIX_THUNK_PROTOCOL;

extern EFI_GUID gEfiUnixThunkProtocolGuid;

#endif
