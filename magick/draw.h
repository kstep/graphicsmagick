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
  unsigned long
    signature;

  char
    *primitive,
    *text,
    *geometry;

  Image
    *tile;

  AffineMatrix
    affine;

  GravityType
    gravity;

  PixelPacket
    fill,
    stroke;

  unsigned int
    stroke_antialias,
    text_antialias;

  FillRule
    fill_rule;

  LineCap
    linecap;

  LineJoin
    linejoin;

  unsigned long
    miterlimit,
    dash_offset;

  DecorationType
    decorate;

  CompositeOperator
    compose;

  double
    pointsize;

  double
    stroke_width;

  char
    *font,
    *density;

  PixelPacket
    box,
    border_color;

  char
    *server_name;

  double
    *dash_pattern;

  char
    *clip_path;

  SegmentInfo
    bounds;

  ClipPathUnits
    clip_units;

  unsigned int
    debug;

  Quantum
    opacity;
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

  unsigned long
    coordinates;

  PrimitiveType
    primitive;

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
extern MagickExport const TypeInfo
  *GetTypeInfo(const char *,ExceptionInfo *);

extern MagickExport DrawInfo
  *CloneDrawInfo(const ImageInfo *,const DrawInfo *);

extern MagickExport unsigned int
  AnnotateImage(Image *,const DrawInfo *),
  ColorFloodfillImage(Image *,const DrawInfo *,const PixelPacket,const long,
    const long,const PaintMethod),
  DrawImage(Image *,DrawInfo *),
  GetTypeMetrics(Image *,const DrawInfo *,TypeMetric *),
  ListTypeInfo(FILE *,ExceptionInfo *),
  MatteFloodfillImage(Image *,const PixelPacket,const unsigned int,const long,
    const long,const PaintMethod);

extern MagickExport void
  DestroyDrawInfo(DrawInfo *),
  DestroyTypeInfo(void),
  GetDrawInfo(const ImageInfo *,DrawInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
