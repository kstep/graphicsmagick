/*
  ImageMagick Image Paint Methods.
*/
#ifndef _MAGICK_PAINT_H
#define _MAGICK_PAINT_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include <render.h>

extern MagickExport unsigned int
  ColorFloodfillImage(Image *,const DrawInfo *,const PixelPacket,const long,
    const long,const PaintMethod),
  MatteFloodfillImage(Image *,const PixelPacket,const unsigned int,const long,
    const long,const PaintMethod);

extern MagickExport unsigned int
  OpaqueImage(Image *,const PixelPacket,const PixelPacket),
  TransparentImage(Image *,const PixelPacket,const unsigned int);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
