/*
  Drawing methods.
*/
#ifndef _DRAW_H
#define _DRAW_H

/*
  Typedef declarations.
*/
typedef struct _AnnotateInfo
{
  char
    *geometry,
    *text,
    *font,
    *density;

  unsigned int
    antialias;

  GravityType
    gravity;

  double
    pointsize,
    degrees;

  AffineInfo
    affine;

  PixelPacket
    fill,
    stroke,
    box;

  DecorationType
    decorate;

  char
    *server_name;

  unsigned long
    signature;
} AnnotateInfo;

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
    *font,
    *density;

  unsigned int
    text_antialias;

  double
    pointsize;

  PixelPacket
    box,
    border_color;

  unsigned int
    verbose;

  char
    *server_name;

  unsigned long
    signature;
} DrawInfo;

typedef struct _PrimitiveInfo
{
  PointInfo
    point;

  PrimitiveType
    primitive;

  unsigned int
    coordinates;

  PaintMethod
    method;

  char
    *text;
} PrimitiveInfo;

/*
  Method declarations.
*/
extern MagickExport AnnotateInfo
  *CloneAnnotateInfo(const ImageInfo *,const AnnotateInfo *);

extern MagickExport DrawInfo
  *CloneDrawInfo(const ImageInfo *,const DrawInfo *);

extern MagickExport unsigned int
  AnnotateImage(Image *,const AnnotateInfo *),
  ColorFloodfillImage(Image *,const DrawInfo *,const PixelPacket,const int x,
    const int y,const PaintMethod),
  DrawImage(Image *,const DrawInfo *),
  GetFontMetrics(const AnnotateInfo *,SegmentInfo *),
  MatteFloodfillImage(Image *,const PixelPacket,const unsigned int,const int x,
    const int y,const PaintMethod);

extern MagickExport void
  DestroyAnnotateInfo(AnnotateInfo *),
  DestroyDrawInfo(DrawInfo *),
  GetAnnotateInfo(const ImageInfo *,AnnotateInfo *),
  GetDrawInfo(const ImageInfo *,DrawInfo *);

#endif
