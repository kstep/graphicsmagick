/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Application Programming Interface declarations.
*/
#ifndef _MAGICK_STUDIO_H
#define _MAGICK_STUDIO_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
# define POSIX
#endif

/*
  System include declarations.
*/
#define __EXTENSIONS__  1
#define _GNU_SOURCE  1
#if !defined(__FreeBSD__)
# define _ISOC99_SOURCE  1
# define _POSIX_C_SOURCE  199506L
# define _XOPEN_SOURCE  500
#endif
#define _LARGEFILE64_SOURCE  1

#if !defined(_MAGICK_CONFIG_H)
# define _MAGICK_CONFIG_H
# if !defined(vms) && !defined(macintosh)
#  include "magick/magick_config.h"
# else
#  include "magick_config.h"
# endif
# if defined(__cplusplus) || defined(c_plusplus)
#  undef inline
# endif
#endif

#if defined(WIN32) && !defined(__CYGWIN__)
# if defined(_MT) && defined(_DLL) && !defined(_MAGICKDLL_) && !defined(_LIB)
#  define _MAGICKDLL_
# endif
# if defined(_MAGICKDLL_)
#  if defined(_VISUALC_)
#   pragma warning( disable: 4273 )  /* Disable the dll linkage warnings */
#  endif
#  if !defined(_MAGICKLIB_)
#   define MagickExport  __declspec(dllimport)
#   if defined(_VISUALC_)
#    pragma message( "Magick lib DLL import interface" )
#   endif
#  else
#   define MagickExport  __declspec(dllexport)
#   if defined(_VISUALC_)
#    pragma message( "Magick lib DLL export interface" )
#   endif
#  endif
# else
#  define MagickExport
#  if defined(_VISUALC_)
#   pragma message( "Magick lib static interface" )
#  endif
# endif

# if defined(_DLL) && !defined(_LIB)
#  define ModuleExport  __declspec(dllexport)
#  if defined(_VISUALC_)
#   pragma message( "Magick module DLL export interface" ) 
#  endif
# else
#  define ModuleExport
#  if defined(_VISUALC_)
#   pragma message( "Magick module static interface" ) 
#  endif

# endif
# define MagickGlobal __declspec(thread)
# if defined(_VISUALC_)
#  pragma warning(disable : 4018)
#  pragma warning(disable : 4244)
#  pragma warning(disable : 4142)
#  pragma warning(disable : 4800)
#  pragma warning(disable : 4786)
# endif
#else
# define MagickExport
# define ModuleExport
# define MagickGlobal
#endif

#if defined(__cplusplus) || defined(c_plusplus)
# define storage_class  c_class
#else
# define storage_class  class
#endif

#define MagickSignature  0xabacadabUL
#define MaxTextExtent  2053

#include <stdarg.h>
#include <stdio.h>
#if defined(WIN32) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#endif
#include <stdlib.h>
#if !defined(WIN32)
# include <unistd.h>
#else
# include <direct.h>
# if !defined(HAVE_STRERROR)
#  define HAVE_STRERROR
# endif
#endif

#if defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64)
# define fseek  fseeko
# define ftell  ftello
#endif

#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <signal.h>
#include <assert.h>

#if defined(WIN32) || defined(POSIX)
# include <sys/types.h>
# include <sys/stat.h>
# if defined(HAVE_FTIME)
# include <sys/timeb.h>
# endif
# if defined(POSIX)
#  if defined(HAVE_SYS_NDIR_H) || defined(HAVE_SYS_DIR_H) || defined(HAVE_NDIR_H)
#   define dirent direct
#   define NAMLEN(dirent) (dirent)->d_namlen
#   if defined(HAVE_SYS_NDIR_H)
#    include <sys/ndir.h>
#   endif
#   if defined(HAVE_SYS_DIR_H)
#    include <sys/dir.h>
#   endif
#   if defined(HAVE_NDIR_H)
#    include <ndir.h>
#   endif
#  else
#   include <dirent.h>
#   define NAMLEN(dirent) strlen((dirent)->d_name)
#  endif
#  include <pwd.h>
# endif
# if !defined(S_ISDIR)
#  define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
# endif
# if !defined(S_ISREG)
#  define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)
# endif
# include "magick/integral_types.h"
# include "magick/image.h"
# include "magick/list.h"
# if !defined(WIN32)
#  include <sys/time.h>
#if defined(HAVE_SYS_TIMES_H)
#  include <sys/times.h>
#endif
# endif
#else
# include <types.h>
# include <stat.h>
# if defined(macintosh)
#  include <SIOUX.h>
#  include <console.h>
#  include <unix.h>
# endif
# include "magick/integral_types.h"
# include "magick/image.h"
# include "magick/list.h"
#endif

#if defined(WIN32)
# include "magick/nt_base.h"
#endif
#if defined(macintosh)
# include "magick/mac.h"
#endif
#if defined(vms)
# include "magick/vms.h"
#endif
#if defined(HAVE_MMAP) && !defined(WIN32)
# include <sys/mman.h>
#endif
#if defined(HasPTHREADS)
# include <pthread.h>
#endif
#if defined(HAVE_POLL)
# include <sys/poll.h>
#endif

#undef index
#undef pipe

/*
  Review these platform specific definitions.
*/
#if defined(POSIX)
# define DirectorySeparator  "/"
# define DirectoryListSeparator  ':'
# define EditorOptions  " -title \"Edit Image Comment\" -e vi"
# define Exit  exit
# define IsBasenameSeparator(c)  ((c) == '/')
# define PreferencesDefaults  "~/."
# define ProcessPendingEvents(text)
# define ReadCommandlLine(argc,argv)
# define SetNotifyHandlers
#else

# if defined(vms)
#  define ApplicationDefaults  "decw$system_defaults:"
#  define DirectorySeparator  ""
#  define DirectoryListSeparator  ';'
#  define EditorOptions  ""
#  define Exit  exit
#  define IsBasenameSeparator(c)  (((c) == ']') || ((c) == ':') || ((c) == '/'))
#  define MagickLibPath  "sys$login:"
#  define MagickCoderModulesPath  "sys$login:"
#  define MagickFilterModulesPath  "sys$login:"
#  define MagickSharePath  "sys$login:"
#  define PreferencesDefaults  "decw$user_defaults:"
#  define ProcessPendingEvents(text)
#  define ReadCommandlLine(argc,argv)
#  define SetNotifyHandlers
# endif

# if defined(macintosh)
#  define ApplicationDefaults  "/usr/lib/X11/app-defaults/"
#  define DirectorySeparator  ":"
#  define DirectoryListSeparator  ';'
#  define EditorOptions ""
#  define IsBasenameSeparator(c)  ((c) == ':')
#  define MagickLibPath  ""
#  define MagickCoderModulesPath  ""
#  define MagickFilterModulesPath  ""
#  define MagickSharePath  ""
#  define PreferencesDefaults  "~/."
#  define ReadCommandlLine(argc,argv)  argc=ccommand(argv); puts(MagickVersion);
#  define SetNotifyHandlers \
    SetErrorHandler(MACErrorHandler); \
    SetWarningHandler(MACWarningHandler)
# endif

# if defined(WIN32)
#  define DirectorySeparator  "\\"
#  define DirectoryListSeparator  ';'
#  define EditorOptions ""
#  define IsBasenameSeparator(c)  (((c) == '/') || ((c) == '\\'))
#  define ProcessPendingEvents(text)
#if !defined(PreferencesDefaults)
#  define PreferencesDefaults  "~\\."
#endif /* PreferencesDefaults */
#  define ReadCommandlLine(argc,argv)
#  define SetNotifyHandlers \
    SetErrorHandler(NTErrorHandler); \
    SetWarningHandler(NTWarningHandler)
#  undef sleep
#  define sleep(seconds)  Sleep(seconds*1000)
#  if !defined(HAVE_TIFFCONF_H)
#    define HAVE_TIFFCONF_H
#  endif
# endif

#endif

/*
  Define declarations.
*/
#define AbsoluteValue(x)  ((x) < 0 ? -(x) : (x))
#define False  0
#define DegreesToRadians(x) (MagickPI*(x)/180.0)
#define IsGray(color)  \
  (((color).red == (color).green) && ((color).green == (color).blue))
#define MagickIncarnate(x)  InitializeMagick(x)
#define MagickEpsilon  1.0e-12
#define MagickPI  3.14159265358979323846264338327950288419716939937510
#define MagickSQ2PI 2.50662827463100024161235523934010416269302368164062
#define Max(x,y)  (((x) > (y)) ? (x) : (y))
#define Min(x,y)  (((x) < (y)) ? (x) : (y))
#define QuantumTick(i,span) \
  ((((i) & 0xff) == 0) || (i == ((ExtendedSignedIntegralType) (span)-1)))
#define RadiansToDegrees(x) (180.0*(x)/MagickPI)
#define ScaleColor5to8(x)  ((x) << 3)
#define ScaleColor6to8(x)  ((x) << 2)
#define Swap(x,y) ((x)^=(y), (y)^=(x), (x)^=(y))
#define True  1

/*
  3D effects.
*/
#define AccentuateModulate  ScaleCharToQuantum(80)
#define HighlightModulate  ScaleCharToQuantum(125)
#define ShadowModulate  ScaleCharToQuantum(135)
#define DepthModulate  ScaleCharToQuantum(185)
#define TroughModulate  ScaleCharToQuantum(110)

/*
  Define system symbols if not already defined.
*/
#if !defined(STDIN_FILENO)
#  define STDIN_FILENO  0x00
#endif

#if !defined(O_BINARY)
#  define O_BINARY  0x00
#endif

#if !defined(MAP_FAILED)
#  define MAP_FAILED      ((void *) -1)
#endif

#if defined(HasLTDL) || ( defined(WIN32) && defined(_DLL) )
#  define SupportMagickModules
#endif

#if defined(_MAGICKMOD_)
#  undef BuildMagickModules
#  define BuildMagickModules
#endif


/*
  I/O defines.
*/
#if defined(WIN32) && !defined(Windows95)
#  define MagickSeek(file,offset,whence)  _lseeki64(file,offset,whence)
#else
#  define MagickSeek(file,offset,whence)  lseek(file,offset,whence)
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
