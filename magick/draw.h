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
typedef struct _EdgeInfo
{
  SegmentInfo
    bounds;

  double
    scanline;

  PointInfo
    *points;

  int
    number_points,
    direction,
    ghostline,
    highwater;
} EdgeInfo;

typedef struct _PolygonInfo
{
  EdgeInfo
    *edges;

  int
    number_edges;
} PolygonInfo;

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

  PolygonInfo
    clip_path;

  unsigned long
    signature;
} DrawInfo;

typedef struct _FontMetrics
{
  PointInfo
    pixels_per_em;

  int
    ascent,
    descent;

  unsigned int
    width,
    height,
    max_advance;
} FontMetrics;

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
extern MagickExport DrawInfo
  *CloneDrawInfo(const ImageInfo *,const DrawInfo *);

extern MagickExport DrawInfo
  *CloneDrawInfo(const ImageInfo *,const DrawInfo *);

extern MagickExport unsigned int
  AnnotateImage(Image *,const DrawInfo *),
  ColorFloodfillImage(Image *,const DrawInfo *,const PixelPacket,const int x,
    const int y,const PaintMethod),
  DrawImage(Image *,const DrawInfo *),
  GetFontMetrics(Image *,const DrawInfo *,FontMetrics *),
  MatteFloodfillImage(Image *,const PixelPacket,const unsigned int,const int x,
    const int y,const PaintMethod);

extern MagickExport void
  DestroyDrawInfo(DrawInfo *),
  GetDrawInfo(const ImageInfo *,DrawInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
