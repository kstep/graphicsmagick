/*
  ImageMagick Application Programming Interface declarations.

  Api.h depends on a number of ANSI C headers as follows:

      #include <stdio.h>
      #include <time.h>
      #include <sys/types.h>
      #include <magick/api.h>

*/

#ifndef _MAGICK_API_H
#define _MAGICK_API_H

#if defined(__cplusplus) || defined(c_plusplus)
#define storage_class  c_class
#else
#define storage_class  class
#endif

#if defined(WIN32) || defined(__CYGWIN__)
#if defined(_VISUALC_)
#  if defined(_MT) && defined(_DLL) && !defined(_MAGICKDLL_) && !defined(_LIB)
#    define _MAGICKDLL_
#  endif
#  if defined(_MAGICKDLL_)
#    pragma warning( disable: 4273 )  /* Disable the dll linkage warnings */
#    if !defined(_MAGICKLIB_)
#      define MagickExport  __declspec(dllimport)
#      pragma message( "Compiling as dll import" ) 
#    else
#      define MagickExport  __declspec(dllexport)
#      pragma message( "Compiling as dll export" ) 
#    endif
#  else
#    define MagickExport
#    pragma message( "Compiling as simple library" ) 
#  endif

#if defined(_DLL) && !defined(_LIB)
#  define ModuleExport  __declspec(dllexport)
#  pragma message( "Module compiled as DLL" ) 
#else
#  define ModuleExport
#  pragma message( "Module compiled as library" ) 
#endif

#define MagickGlobal __declspec(thread)

#pragma warning(disable : 4018)
#pragma warning(disable : 4244)
#pragma warning(disable : 4142)

#else
# define MagickExport __declspec(dllexport)
# define ModuleExport __declspec(dllexport)
# define MagickGlobal
#endif
#else
# define MagickExport
# define ModuleExport
# define MagickGlobal
#endif

#define MaxTextExtent  1664

#if !defined(vms) && !defined(WIN32)
#if defined(HAVE_CONFIG_H)
#include "magick/config.h"
#endif
#include "magick/classify.h"
#include "magick/image.h"
#include "magick/draw.h"
#include "magick/gems.h"
#include "magick/quantize.h"
#include "magick/compress.h"
#include "magick/attributes.h"
#include "magick/utility.h"
#include "magick/blob.h"
#include "magick/cache.h"
#include "magick/cache_view.h"
#include "magick/delegates.h"
#include "magick/timer.h"
#include "magick/semaphore.h"
#include "magick/monitor.h"
#include "magick/error.h"
#include "magick/version.h"
#else
#include "classify.h"
#include "image.h"
#include "draw.h"
#include "gems.h"
#include "quantize.h"
#include "compress.h"
#include "attributes.h"
#include "utility.h"
#include "blob.h"
#include "cache.h"
#include "cache_view.h"
#include "delegates.h"
#include "timer.h"
#include "semaphore.h"
#include "monitor.h"
#include "error.h"
#include "version.h"
#endif

#endif
