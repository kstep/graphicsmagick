/*
  ImageMagick Application Programming Interface declarations.

  Api.h depends on a number of ANSI C headers as follows:

      #include <stdio.h>
      #include <time.h>
      #include <sys/type.h>
      #include <magick/api.h>

*/

#ifndef _MAGICK_API_H
#define _MAGICK_API_H

#if defined(__cplusplus) || defined(c_plusplus)
#define class  c_class
#endif

#if defined(WIN32) || defined(__CYGWIN__)
#if defined(_VISUALC_)
/**
 * Under VISUALC we have single threaded static libraries, or
 * mutli-threaded DLLs using the multithreaded runtime DLLs.
 **/
#	if defined(_MT) && defined(_DLL) && !defined(_MAGICKDLL_)
#		define _MAGICKDLL_
#	endif

#	if defined(_MT) && !defined(_MAGICKDLL_)
#		error To use the Visualmagick libraries from a multithreaded project, you must use the DLLs
#		error Use the Mutlithreaded DLL runtime libraries (prefered), or define the macro "_MAGICKDLL_"
#	endif

#	if !defined(_MT) && defined(_MAGICKDLL_)
#		error To use the Visualmagick libraries from a single-threaded project, you must use static libraries
#		error Undefine the macro "_MAGICKDLL_"
#	endif

#	if defined(_MAGICKDLL_)
#		pragma warning( disable: 4273 )	/* Disable the stupid dll linkage warnings */
#		if !defined(_MAGICKLIB_)
#			define Export __declspec(dllimport)
#		else
#			define Export __declspec(dllexport)
#		endif
#	else
#		define Export
#	endif

#pragma warning(disable : 4018)
#pragma warning(disable : 4244)
#pragma warning(disable : 4142)

#else
#define Export __declspec(dllexport)
#endif
#else
# define Export
#endif

#define MaxTextExtent  1664

#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#if defined(HAVE_CONFIG_H)
#include "magick/config.h"
#endif
#include "magick/classify.h"
#include "magick/image.h"
#include "magick/gems.h"
#include "magick/quantize.h"
#include "magick/compress.h"
#include "magick/attributes.h"
#include "magick/utility.h"
#include "magick/blob.h"
#include "magick/stash.h"
#include "magick/cache_view.h"
#include "magick/memory.h"
#include "magick/delegates.h"
#include "magick/timer.h"
#include "magick/monitor.h"
#include "magick/error.h"
#include "magick/version.h"
#else
#include "classify.h"
#include "image.h"
#include "gems.h"
#include "quantize.h"
#include "compress.h"
#include "attributes.h"
#include "utility.h"
#include "blob.h"
#include "stash.h"
#include "cache_view.h"
#include "memory.h"
#include "delegates.h"
#include "timer.h"
#include "monitor.h"
#include "error.h"
#include "version.h"
#endif

#endif
