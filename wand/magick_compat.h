/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2003 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  ImageMagick compatability methods and definitions for Wand implementation.
*/

#ifndef _MAGICK_COMPAT_H
#define _MAGICK_COMPAT_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


#if defined(MAGICK_IMPLEMENTATION)

#include "magick/color.h"
#include "magick/composite.h"
#include "magick/constitute.h"
#include "magick/cache.h"
#include "magick/decorate.h"
#include "magick/effect.h"
#include "magick/enhance.h"
#include "magick/fx.h"
#include "magick/shear.h"
#include "magick/tempfile.h"
#include "magick/transform.h"

#define AcquireMagickMemory(memory) malloc(memory)
#define AcquireUniqueFileResource(char_pointer) AcquireTemporaryFileName(char_pointer)
  /* #define CopyMagickString GMPrivateCopyMagickString */
  /* #define FormatMagickString GMPrivateFormatMagickString */
#define GetAffineMatrix(affine_matrix) IdentityAffine(affine_matrix)
#define InheritException(copy,original) CopyException(copy,original)
#define RelinquishUniqueFileResource LiberateTemporaryFile
#define ThrowMagickException ThrowException

#define PsiNegative YNegative
#define PsiValue XValue
#define RhoValue WidthValue
#define SigmaValue HeightValue
#define XiNegative XNegative
#define XiValue YValue

#define ConcatenateMagickString GMPrivateConcatenateMagickString
#define GeometryInfo GMPrivateGeometryInfo
#define ImportImagePixels GMPrivateImportImagePixels
#define ParseAbsoluteGeometry GMPrivateParseAbsoluteGeometry
#define ParseGeometry GMPrivateParseGeometry
#define RelinquishMagickMemory GMPrivateRelinquishMagickMemory
#define ResizeMagickMemory GMPrivateResizeMagickMemory
#define SetGeometryInfo GMPrivateSetGeometryInfo
#define _GeometryInfo _GMPrivateGeometryInfo

#define ExportImagePixels DispatchImage

typedef struct _GeometryInfo
{
  double
    rho,
    sigma,
    xi,
    psi;
} GeometryInfo;

#if !defined(__GNUC__) && !defined(__attribute__)
#  define __attribute__(x) /*nothing*/
#endif

extern WandExport void
  *RelinquishMagickMemory(void *),
  *ResizeMagickMemory(void *memory,const size_t size),
   SetGeometryInfo(GeometryInfo *geometry_info);

extern MagickExport int
  FormatMagickString(char *,const size_t,const char *,...)
    __attribute__((format (printf,3,4)));

extern WandExport unsigned int
  ImportImagePixels(Image *image,const long x_offset,
    const long y_offset,const unsigned long columns,const unsigned long rows,
    const char *map,const StorageType type,const void *pixels),
  ParseAbsoluteGeometry(const char *geometry,RectangleInfo *region_info),
  ParseGeometry(const char *geometry,GeometryInfo *geometry_info);

extern MagickExport size_t
  ConcatenateMagickString(char *,const char *,const size_t),
  CopyMagickString(char *,const char *,const size_t);

#endif /* defined(MAGICK_IMPLEMENTATION) */

#endif /* _MAGICK_COMPAT_H */
