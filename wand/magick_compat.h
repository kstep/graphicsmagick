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
#include "magick/transform.h"

#define AcquireMagickMemory(memory) malloc(memory)
#define GetAffineMatrix(affine_matrix) IdentityAffine(affine_matrix)
#define RelinquishMagickMemory(memory) MagickFreeMemory(memory)

#define PsiNegative YNegative
#define PsiValue XValue
#define RhoValue WidthValue
#define SigmaValue HeightValue
#define XiNegative XNegative
#define XiValue YValue

#define GeometryInfo GMPrivateGeometryInfo
#define ImportImagePixels GMPrivateImportImagePixels
#define ParseAbsoluteGeometry GMPrivateParseAbsoluteGeometry
#define ParseGeometry GMPrivateParseGeometry
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

extern MagickExport void
  *ResizeMagickMemory(void *memory,const size_t size),
   SetGeometryInfo(GeometryInfo *geometry_info);

extern MagickExport unsigned int
  ImportImagePixels(Image *image,const long x_offset,
    const long y_offset,const unsigned long columns,const unsigned long rows,
    const char *map,const StorageType type,const void *pixels),
  ParseAbsoluteGeometry(const char *geometry,RectangleInfo *region_info),
  ParseGeometry(const char *geometry,GeometryInfo *geometry_info);

#endif /* defined(MAGICK_IMPLEMENTATION) */

#endif /* _MAGICK_COMPAT_H */
