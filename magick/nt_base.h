/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  Windows NT Utility Methods for ImageMagick.
*/
#ifndef _MAGICK_NTBASE_H
#define _MAGICK_NTBASE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include "magick/delegate.h"

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>
#include <winuser.h>
#include <wingdi.h>
#include <io.h>
#include <process.h>
#include <errno.h>
#if defined(_DEBUG)
#include <crtdbg.h>
#endif

/*
  Define declarations.
*/

/* Memory mapping support */
#define PROT_NONE       0x0  // pages can not be accessed
#define PROT_READ       0x1  // pages can be read
#define PROT_WRITE      0x2  // pages can be written
#define MAP_SHARED      0x1  // share changes
#define MAP_PRIVATE     0x2  // changes are private
#define MAP_NORESERVE   0x4  // do not reserve paging space
#define MAP_ANON        0x8  // anonymous mapping
#if !defined(MAP_FAILED)
#  define MAP_FAILED      ((void *) -1) // returned on error by mmap
#endif
#define MS_ASYNC        0x0  // asynchronous page sync
#define MS_SYNC         0x1  // synchronous page sync


#define F_OK 0
#define R_OK 4
#define W_OK 2
#define RW_OK 6
#define HAVE_VSNPRINTF 1
#define HAVE_TEMPNAM 1
#define HAVE_RAISE 1

// Define to support memory mapping files for improved performance
#define HAVE_MMAP_FILEIO 1

// Use Visual C++ C inline method extension to improve performance
#define inline __inline

#if !defined(chsize)
# if defined(__BORLANDC__)
#   define chsize(file,length) chsize(file,length)
# else
#   define chsize(file,length) _chsize(file,length)
# endif
#endif

#if !defined(tempnam)
# define tempnam _tempnam
#endif
#define vsnprintf _vsnprintf 
#if defined(_MT) && defined(WIN32)
#define SAFE_GLOBAL __declspec(thread)
#else
#define SAFE_GLOBAL
#endif

/*
  With Visual C++, Popen and pclose are available via _popen and _pclose.
  These are documented to work for console applications only.
*/
#define HAVE_POPEN 1
#define HAVE__POPEN 1
#define popen(command,mode)  _popen(command,mode)
#define HAVE_PCLOSE 1
#define HAVE__PCLOSE 1
#define pclose(stream)  _pclose(stream)

/*
  Windows provides Unix-style access() via _access()
 */
#define HAVE_ACCESS 1
#define access(path,mode) _access(path,mode)
/*
  Typedef declarations.
*/
typedef UINT (CALLBACK *LPFNDLLFUNC1)(DWORD,UINT);

#if !defined(XS_VERSION)
struct dirent
{
  char
    d_name[2048];

  int
    d_namlen;
};

/*
  ssize_t is the type returned by _read and _write.
  Recent MinGW compilers include this typedef by default.
 */
#if !defined(ssize_t) && !defined(__MINGW32__)
typedef long ssize_t;
#endif

typedef struct _DIR
{
  HANDLE
    hSearch;

  WIN32_FIND_DATA
    Win32FindData;

  BOOL
    firsttime;

  struct dirent
    file_info;
} DIR;

#endif


/*
  NT utilities routines.
*/
extern MagickExport char
  *NTGetLastError(void);

extern MagickExport int
  Exit(int),
  IsWindows95(),
  NTGhostscriptDLL(char *path, int path_length),
  NTGhostscriptEXE(char *path, int path_length),
  NTGhostscriptFonts(char *path, int path_length),
  NTGhostscriptLoadDLL(void),
  NTGhostscriptUnLoadDLL(void),
  NTSystemComman(const char *);

#if defined(MAGICK_IMPLEMENTATION)
extern MagickExport const GhostscriptVectors
  *NTGhostscriptDLLVectors( void );
#endif

#if !defined(XS_VERSION)
extern MagickExport DIR
  *opendir(char *);

extern MagickExport double
  NTElapsedTime(void),
  NTUserTime(void);

extern MagickExport int
#if !defined(HasLTDL)
  lt_dlclose(void *),
  lt_dlexit(void),
  lt_dlinit(void),
  lt_dlsetsearchpath(const char *),
#endif /* !HasLTDL */
  msync(void *addr, size_t len, int flags),
  munmap(void *addr, size_t len);

extern MagickExport long
  telldir(DIR *);

extern MagickExport struct dirent
  *readdir(DIR *);

#if !defined(HasLTDL)
extern MagickExport const char
  *lt_dlerror(void);
#endif
  
extern MagickExport unsigned char
  *NTResourceToBlob(const char *);

extern MagickExport unsigned int
  NTGetExecutionPath(char *);

extern MagickExport void
  closedir(DIR *),
  *mmap(char *address,size_t length,int protection,int access,int file,
    magick_off_t offset),
  NTErrorHandler(const ExceptionType,const char *,const char *),
  NTWarningHandler(const ExceptionType,const char *,const char *),
  seekdir(DIR *,long)
#if !defined(HasLTDL)
  ,
  *lt_dlopen(const char *),
  *lt_dlsym(void *, const char *)
#endif /* !HasLTDL */
  ;

#endif /* !XS_VERSION */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* !C++ */

#endif /* !_MAGICK_NTBASE_H */
