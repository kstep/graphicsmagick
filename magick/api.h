/*
  GraphicsMagick Application Programming Interface declarations.

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
# include "magick/integral_types.h"
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
# include "integral_types.h"
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
