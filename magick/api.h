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
extern "C" {
#endif

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

/*
  Assign typedefs based on the sizes of native types
  gm_int16_t
  gm_uint16_t
  gm_int32_t
  gm_uint32_t
  gm_int64_t
  gm_uint64_t  -- unsigned 64 bit -- may not port
*/

#if !defined(SIZEOF_SHORT)
# define SIZEOF_SHORT 2
#endif

#if !defined(SIZEOF_UNSIGNED_SHORT)
# define SIZEOF_UNSIGNED_SHORT 2
#endif

#if !defined(SIZEOF_INT)
# define SIZEOF_INT 4
#endif

#if !defined(SIZEOF_UNSIGNED_INT)
# define SIZEOF_UNSIGNED_INT 4
#endif

#if !defined(SIZEOF_LONG)
# define SIZEOF_LONG 4
#endif

#if !defined(SIZEOF_UNSIGNED_LONG)
# define SIZEOF_UNSIGNED_LONG 4
#endif

#if !defined(SIZEOF_LONG_LONG)
# define SIZEOF_LONG_LONG 0
#endif

#if !defined(SIZEOF_UNSIGNED_LONG_LONG)
# define SIZEOF_UNSIGNED_LONG_LONG 0
#endif

#if (SIZEOF_SHORT == 2)
  typedef short gm_int16_t;
#else
#  error sizeof(short) != 2
#endif

#if (SIZEOF_UNSIGNED_SHORT == 2)
  typedef unsigned short gm_uint16_t;
#else
#  error sizeof(unsigned short) != 2
#endif

#if (SIZEOF_INT == 4)
  typedef int gm_int32_t;
#else
#  error sizeof(int) != 4
#endif

#if (SIZEOF_UNSIGNED_INT == 4)
  typedef unsigned int gm_uint32_t;
#else
#  error sizeof(unsigned int) != 4)
#endif

#if (SIZEOF_LONG == 8)
  typedef long gm_int64_t;
#elif (SIZEOF_LONG_LONG == 8)
  typedef long long gm_int64_t;
#elif defined(_VISUALC_)
  typedef __int64 gm_int64_t;
#else
  typedef double gm_int64_t;
#endif

#if (SIZEOF_UNSIGNED_LONG == 8)
  typedef unsigned long gm_uint64_t;
#elif (SIZEOF_UNSIGNED_LONG_LONG == 8)
  typedef unsigned long long gm_uint64_t;
#elif defined(_VISUALC_)
  /* Does Visual C++ support an unsigned 64-bit type? */
  typedef __int64 gm_uint64_t;
#else
  typedef double gm_uint64_t;
#endif

#if defined(__cplusplus) || defined(c_plusplus)
# define storage_class  c_class
#else
# define storage_class  class
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

#define MaxTextExtent  2053
#define MagickSignature  0xabacadabUL

#if !defined(vms) && !defined(macintosh)
# include "magick/semaphore.h"
# include "magick/error.h"
# include "magick/timer.h"
# include "magick/image.h"
# include "magick/list.h"
# include "magick/paint.h"
# include "magick/render.h"
# include "magick/draw.h"
# include "magick/gem.h"
# include "magick/quantize.h"
# include "magick/compress.h"
# include "magick/attribute.h"
# include "magick/command.h"
# include "magick/utility.h"
# include "magick/signature.h"
# include "magick/random.h"
# include "magick/blob.h"
# include "magick/cache.h"
# include "magick/cache_view.h"
# include "magick/color.h"
# include "magick/command.h"
# include "magick/constitute.h"
# include "magick/decorate.h"
# include "magick/enhance.h"
# include "magick/fx.h"
# include "magick/magick.h"
# include "magick/montage.h"
# include "magick/effect.h"
# include "magick/resize.h"
# include "magick/shear.h"
# include "magick/stream.h"
# include "magick/transform.h"
# include "magick/composite.h"
# include "magick/registry.h"
# include "magick/magick.h"
# include "magick/magic.h"
# include "magick/delegate.h"
# include "magick/module.h"
# include "magick/monitor.h"
# include "magick/resource.h"
# include "magick/log.h"
# include "magick/version.h"
# include "magick/deprecate.h"
#else
# include "semaphore.h"
# include "timer.h"
# include "error.h"
# include "image.h"
# include "list.h"
# include "paint.h"
# include "render.h"
# include "draw.h"
# include "gem.h"
# include "quantize.h"
# include "compress.h"
# include "attribute.h"
# include "command.h"
# include "utility.h"
# include "signature.h"
# include "random.h"
# include "blob.h"
# include "cache.h"
# include "cache_view.h"
# include "color.h"
# include "command.h"
# include "constitute.h"
# include "decorate.h"
# include "enhance.h"
# include "fx.h"
# include "magick.h"
# include "montage.h"
# include "effect.h"
# include "resize.h"
# include "shear.h"
# include "stream.h"
# include "transform.h"
# include "composite.h"
# include "registry.h"
# include "magick.h"
# include "magic.h"
# include "delegate.h"
# include "module.h"
# include "monitor.h"
# include "resource.h"
# include "log.h"
# include "version.h"
# include "deprecate.h"
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
