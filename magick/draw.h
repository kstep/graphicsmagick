/*
  Drawing methods.
*/
#ifndef _MAGICK_DRAW_H
#define _MAGICK_DRAW_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Typedef declarations.
*/
typedef struct _AffineMatrix
{
  double
    sx,
    rx,
    ry,
    sy,
    tx,
    ty;
} AffineMatrix;

typedef struct _DrawInfo
{
  char
    *primitive,
    *text,
    *geometry;

  AffineMatrix
    affine;

  GravityType
    gravity;

  Quantum
    opacity;

  Image
    *tile;

  PixelPacket
    fill,
    stroke;

  unsigned int
    stroke_antialias;

  double
    stroke_width;

  FillRule
    fill_rule;

  LineCap
    linecap;

  LineJoin
    linejoin;

  unsigned int
    miterlimit,
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

  CompositeOperator
    compose;

  char
    *server_name;

  unsigned int
    *dash_pattern;

  char
    *clip_path;

  ClipPathUnits
    clip_units;

  SegmentInfo
    bounds;

  unsigned int
    debug;

  unsigned long
    signature;
} DrawInfo;

typedef struct _TypeInfo
{ 
  char
    *filename,
    *name,
    *family,
    *alias,
    *description,
    *format,
    *weight,
    *glyphs,
    *metrics,
    *version;

  struct _TypeInfo
    *previous,
    *next;
} TypeInfo;

typedef struct _PrimitiveInfo
{
  PointInfo
    point;

  PrimitiveType
    primitive;

  unsigned long
    coordinates;

  PaintMethod
    method;

  char
    *text;
} PrimitiveInfo;

typedef struct _TypeMetric
{
  PointInfo
    pixels_per_em;
    
  long
    ascent,
    descent;
    
  unsigned long
    width, 
    height,
    max_advance;
} TypeMetric;

/*
  Method declarations.
*/
extern MagickExport DrawInfo
  *CloneDrawInfo(const ImageInfo *,const DrawInfo *);

extern MagickExport TypeInfo
  *GetTypeInfo(const char *,ExceptionInfo *);

extern MagickExport unsigned int
  AnnotateImage(Image *,const DrawInfo *),
  ColorFloodfillImage(Image *,const DrawInfo *,const PixelPacket,const int x,
    const int y,const PaintMethod),
  DrawImage(Image *,DrawInfo *),
  GetTypeMetrics(Image *,const DrawInfo *,TypeMetric *),
  ListTypeInfo(FILE *,ExceptionInfo *),
  MatteFloodfillImage(Image *,const PixelPacket,const unsigned int,const int x,
    const int y,const PaintMethod);

extern MagickExport void
  DestroyDrawInfo(DrawInfo *),
  DestroyTypeInfo(void),
  GetDrawInfo(const ImageInfo *,DrawInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
