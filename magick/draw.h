/*
  Drawing methods.
*/
#ifndef _DRAW_H
#define _DRAW_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Typedef declarations.
*/
typedef struct _DrawInfo
{
  char
    *primitive;

  AffineInfo
    affine;

  double
    angle;

  GravityType
    gravity;

  PixelPacket
    fill;

  FillRule
    fill_rule;

  Image
    *tile;

  PixelPacket
    stroke;

  unsigned int
    stroke_antialias;

  double
    linewidth;

  LineCap
    linecap;

  LineJoin
    linejoin;

  unsigned int
    miterlimit,
    *dash_pattern,
    dash_offset;

  DecorationType
    decorate;

  char
    *font;

  unsigned int
    text_antialias;

  double
    pointsize;

  PixelPacket
    box,
    border_color;

  unsigned int
    verbose;

  unsigned long
    signature;
} DrawInfo;

/*
  Method declarations.
*/
extern MagickExport DrawInfo
  *CloneDrawInfo(const ImageInfo *,const DrawInfo *);

extern MagickExport unsigned int
  ColorFloodfillImage(Image *,const DrawInfo *,const PixelPacket,const int x,
    const int y,const PaintMethod),
  DrawImage(Image *,const DrawInfo *),
  MatteFloodfillImage(Image *,const PixelPacket,const unsigned int,const int x,
    const int y,const PaintMethod);

extern MagickExport void
  DestroyDrawInfo(DrawInfo *),
  GetDrawInfo(const ImageInfo *,DrawInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
