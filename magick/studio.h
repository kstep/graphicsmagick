/*
  ImageMagick Application Programming Interface declarations.
*/
#ifndef _MAGICK_STUDIO_H
#define _MAGICK_STUDIO_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  System include declarations.
*/
#define _XOPEN_SOURCE  500
#define _POSIX_C_SOURCE  199506L

#define _MAGICK_CONFIG_H
#if !defined(vms) && !defined(macintosh)
#include "magick/magick_config.h"
#else
#include "magick_config.h"
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#if !defined(WIN32)
#  include <unistd.h>
#else
#  include <direct.h>
#  define HAVE_STRERROR
#endif

#if defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64)
#define fseek  fseeko
#define ftell  ftello
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

#if !defined(vms) && !defined(macintosh)
#include <sys/types.h>
#include <sys/stat.h>
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#if defined(HAVE_SYS_NDIR_H) || defined(HAVE_SYS_DIR_H) || defined(HAVE_NDIR_H)
#define dirent direct
#define NAMLEN(dirent) (dirent)->d_namlen
#if defined(HAVE_SYS_NDIR_H)
#include <sys/ndir.h>
#endif
#if defined(HAVE_SYS_DIR_H)
#include <sys/dir.h>
#endif
#if defined(HAVE_NDIR_H)
#include <ndir.h>
#endif
#else
#include <dirent.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)
#endif
#include <pwd.h>
#endif
#if !defined(S_ISDIR)
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif
#include "magick/api.h"
#if !defined(WIN32)
#include <sys/time.h>
#include <sys/times.h>
#endif
#else
#include <types.h>
#include <stat.h>
#if defined(macintosh)
#include <SIOUX.h>
#include <console.h>
#include <unix.h>
#endif
#include "api.h"
#endif

#if defined(WIN32)
#include "nt.h"
#endif
#if defined(macintosh)
#include "mac.h"
#endif
#if defined(vms)
#include "vms.h"
#endif
#if defined(HAVE_MMAP) && !defined(WIN32)
#include <sys/mman.h>
#endif
#if defined(HasPTHREADS)
#include <pthread.h>
#endif
#if defined(HAVE_POLL)
#include <sys/poll.h>
#endif

#undef index
#undef pipe

/*
  Review these platform specific definitions.
*/
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#if !defined(ApplicationDefaults)
#define ApplicationDefaults  "/usr/X11R6/lib/X11/app-defaults/"
#endif
#define DirectorySeparator  "/"
#define DirectoryListSeparator  ':'
#define EditorOptions  " -title \"Edit Image Comment\" -e vi"
#define Exit  exit
#define IsBasenameSeparator(c)  ((c) == '/')
#define IsGlob(text) \
  ((strchr(text,'*') != (char *) NULL) || \
   (strchr(text,'?') != (char *) NULL) || \
   (strchr(text,'{') != (char *) NULL) || \
   (strchr(text,'}') != (char *) NULL) || \
   (strchr(text,'[') != (char *) NULL) || \
   (strchr(text,']') != (char *) NULL))
#if !defined(MagickLibPath)
#define MagickLibPath  "/usr/local/lib/ImageMagick/"
#endif
#if !defined(MagickModulesPath)
#define MagickModulesPath  "/usr/local/lib/ImageMagick/modules/"
#endif
#if !defined(MagickSharePath)
#define MagickSharePath  "/usr/local/share/ImageMagick/"
#endif
#define PreferencesDefaults  "~/."
#define ProcessPendingEvents(text)
#define ReadCommandlLine(argc,argv)
#define SetNotifyHandlers
#else

#if defined(vms)
#define ApplicationDefaults  "decw$system_defaults:"
#define DirectorySeparator  ""
#define DirectoryListSeparator  ';'
#define EditorOptions  ""
#define Exit  exit
#define IsBasenameSeparator(c)  (((c) == ']') || ((c) == ':') || ((c) == '/'))
#define IsGlob(text) \
  ((strchr(text,'*') != (char *) NULL) || \
   (strchr(text,'?') != (char *) NULL) || \
   (strchr(text,'{') != (char *) NULL) || \
   (strchr(text,'}') != (char *) NULL))
#define j1(x)  x
#define MagickLibPath  "sys$login:"
#define MagickModulesPath  "sys$login:"
#define MagickSharePath  "sys$login:"
#define PreferencesDefaults  "decw$user_defaults:"
#define ProcessPendingEvents(text)
#define ReadCommandlLine(argc,argv)
#define SetNotifyHandlers
#endif

#if defined(macintosh)
#define ApplicationDefaults  "/usr/lib/X11/app-defaults/"
#define DirectorySeparator  ":"
#define DirectoryListSeparator  ';'
#define EditorOptions ""
#define IsBasenameSeparator(c)  ((c) == ':')
#define IsGlob(text) \
  ((strchr(text,'*') != (char *) NULL) || \
   (strchr(text,'?') != (char *) NULL) || \
   (strchr(text,'{') != (char *) NULL) || \
   (strchr(text,'}') != (char *) NULL) || \
   (strchr(text,'[') != (char *) NULL) || \
   (strchr(text,']') != (char *) NULL))
#define j1(x)  x
#define MagickLibPath  ""
#define MagickModulesPath  ""
#define MagickSharePath  ""
#define PreferencesDefaults  "~/."
#define ReadCommandlLine(argc,argv)  argc=ccommand(argv); puts(MagickVersion);
#define SetNotifyHandlers \
  SetErrorHandler(MACErrorHandler); \
  SetWarningHandler(MACWarningHandler)
#endif

#if defined(WIN32)
#define ApplicationDefaults  "c:\\ImageMagick\\"
#define DirectorySeparator  "\\"
#define DirectoryListSeparator  ';'
#define EditorOptions ""
#undef isatty
#define isatty(filedes)  1
#define IsBasenameSeparator(c)  (((c) == '/') || ((c) == '\\'))
#define IsGlob(text) \
  ((strchr(text,'*') != (char *) NULL) || \
   (strchr(text,'?') != (char *) NULL) || \
   (strchr(text,'{') != (char *) NULL) || \
   (strchr(text,'}') != (char *) NULL) || \
   (strchr(text,'[') != (char *) NULL) || \
   (strchr(text,']') != (char *) NULL))
#define j1(x)  x
#define MagickLibPath  "c:\\ImageMagick\\"
#define MagickModulesPath  "c:\\ImageMagick\\"
#define MagickSharePath  "c:\\ImageMagick\\"
#define PreferencesDefaults  "~/."
#define ProcessPendingEvents(text)
#define ReadCommandlLine(argc,argv)
#define SetNotifyHandlers \
  SetErrorHandler(NTErrorHandler); \
  SetWarningHandler(NTWarningHandler)
#undef sleep
#define sleep(seconds)  Sleep(seconds*1000)
#define HAVE_TIFFCONF_H
#endif

#endif

/*
  Define declarations.
*/
#define AbsoluteValue(x)  ((x) < 0 ? -(x) : (x))
#define False  0
#define DegreesToRadians(x) (MagickPI*(x)/180.0)
#define IsFaxImage(color) \
  (IsMonochromeImage(image) && ((image)->columns <= 2560))
#define IsGray(color)  \
  (((color).red == (color).green) && ((color).green == (color).blue))
#define IsMono(color)  \
  (IsGray(color) && ((Intensity(color) == 0) || (Intensity(color) == MaxRGB)))
#define MagickIncarnate(x)  InitializeMagick(x)
#define MagickEpsilon  1.0e-12
#define MagickPI  3.14159265358979323846264338327950288419716939937510
#define Max(x,y)  (((x) > (y)) ? (x) : (y))
#define Min(x,y)  (((x) < (y)) ? (x) : (y))
#define QuantumTick(i,span) \
  (((~((span)-i-1) & ((span)-i-2))+1) == ((span)-i-1))
#define RadiansToDegrees(x) (180.0*(x)/MagickPI)
#define ScaleColor5to8(x)  ((x) << 3)
#define ScaleColor6to8(x)  ((x) << 2)
#define Swap(x,y) ((x)^=(y), (y)^=(x), (x)^=(y))
#define True  1
#define TypeFilename  "type.mgk"

/*
  3D effects.
*/
#define AccentuateModulate  Upscale(80)
#define HighlightModulate  Upscale(125)
#define ShadowModulate  Upscale(135)
#define DepthModulate  Upscale(185)
#define TroughModulate  Upscale(110)

/*
  Define system symbols if not already defined.
*/
#if !defined(STDIN_FILENO)
#define STDIN_FILENO  0x00
#endif

#if !defined(O_BINARY)
#define O_BINARY  0x00
#endif

#if !defined(MAP_FAILED)
#define MAP_FAILED      ((void *) -1)
#endif

#if defined(HasLTDL) || defined(_MAGICKMOD_)
#define HasMODULES
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
