/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        DDDD   RRRR    AAA   W   W                           %
%                        D   D  R   R  A   A  W   W                           %
%                        D   D  RRRR   AAAAA  W   W                           %
%                        D   D  R R    A   A  W W W                           %
%                        DDDD   R  R   A   A   W W                            %
%                                                                             %
%                                                                             %
%                     ImageMagick Image Drawing Methods                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Bill Radcliffe of Corbis (www.corbis.com) contributed the polygon
% rendering code based on Paul Heckbert's "Concave Polygon Scan Conversion",
% Graphics Gems, 1990.  Leonard Rosenthal and David Harr of Appligent
% (www.appligent.com) contributed the dash pattern, linecap stroking
% algorithm, and minor rendering improvements.
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

/*
  Define declarations.
*/
#define BezierQuantum  200
#define MatteMatch(color,target,delta) \
  (ColorMatch(color,target,delta) && ((color).opacity == (target).opacity))
#define MaxStacksize  (1 << 15)
#define Push(up,left,right,delta) \
  if ((s < (segment_stack+MaxStacksize)) && (((up)+(delta)) >= 0) && \
      (((up)+(delta)) < (long) image->rows)) \
    { \
      s->y1=(up); \
      s->x1=(left); \
      s->x2=(right); \
      s->y2=(delta); \
      s++; \
    }

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

  long
    number_points,
    direction,
    ghostline,
    highwater;
} EdgeInfo;

typedef struct _PolygonInfo
{
  EdgeInfo
    *edges;

  long
    number_edges;
} PolygonInfo;

typedef enum
{
  MoveToCode,
  OpenCode,
  GhostlineCode,
  LineToCode,
  EndCode
} PathInfoCode;

typedef struct _PathInfo
{
  PointInfo
    point;

  PathInfoCode
    code;
} PathInfo;

/*
  Forward declarations.
*/
static PrimitiveInfo
  *TraceStrokePolygon(const DrawInfo *,const PrimitiveInfo *);

static unsigned int
  DrawPrimitive(Image *,const DrawInfo *,const PrimitiveInfo *),
  DrawStrokePolygon(Image *,const DrawInfo *,const PrimitiveInfo *);

static unsigned long
  TracePath(PrimitiveInfo *,const char *);

static void
  DrawBoundingRectangles(Image *,const DrawInfo *,const PolygonInfo *),
  TraceArc(PrimitiveInfo *,const PointInfo,const PointInfo,const PointInfo,
    const double,const unsigned int,const unsigned int),
  TraceBezier(PrimitiveInfo *,const unsigned long),
  TraceCircle(PrimitiveInfo *,const PointInfo,const PointInfo),
  TraceEllipse(PrimitiveInfo *,const PointInfo,const PointInfo,const PointInfo),
  TraceLine(PrimitiveInfo *,const PointInfo,const PointInfo),
  TracePoint(PrimitiveInfo *,const PointInfo),
  TraceRectangle(PrimitiveInfo *,const PointInfo,const PointInfo),
  TraceRoundRectangle(PrimitiveInfo *,const PointInfo,const PointInfo,
    PointInfo),
  TraceSquareLinecap(PrimitiveInfo *,const unsigned long,const double);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e D r a w I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneDrawInfo() makes a copy of the given draw info structure.  If NULL
%  is specified, a new image info structure is created initialized to
%  default values.
%
%  The format of the CloneDrawInfo method is:
%
%      DrawInfo *CloneDrawInfo(const ImageInfo *image_info,
%        const DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o draw_info: The draw info.
%
%
*/
MagickExport DrawInfo *CloneDrawInfo(const ImageInfo *image_info,
  const DrawInfo *draw_info)
{
  DrawInfo
    *clone_info;

  clone_info=(DrawInfo *) AcquireMemory(sizeof(DrawInfo));
  if (clone_info == (DrawInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to clone draw info",
      "Memory allocation failed");
  if (draw_info == (DrawInfo *) NULL)
    {
      GetDrawInfo(image_info,clone_info);
      return(clone_info);
    }
  *clone_info=(*draw_info);
  if (draw_info->primitive != (char *) NULL)
    clone_info->primitive=AllocateString(draw_info->primitive);
  if (draw_info->text != (char *) NULL)
    clone_info->text=AllocateString(draw_info->text);
  if (draw_info->geometry != (char *) NULL)
    clone_info->geometry=AllocateString(draw_info->geometry);
  if (draw_info->font != (char *) NULL)
    clone_info->font=AllocateString(draw_info->font);
  if (draw_info->density != (char *) NULL)
    clone_info->density=AllocateString(draw_info->density);
  if (draw_info->tile != (Image *) NULL)
    clone_info->tile=ReferenceImage(draw_info->tile);
  if (draw_info->server_name != (char *) NULL)
    clone_info->server_name=AllocateString(draw_info->server_name);
  if (draw_info->dash_pattern != (unsigned int *) NULL)
    {
      register long
        x;

      for (x=0; draw_info->dash_pattern[x]; x++);
      clone_info->dash_pattern=(unsigned int *)
        AcquireMemory((x+1)*sizeof(unsigned int));
      if (clone_info->dash_pattern == (unsigned int *) NULL)
        MagickError(ResourceLimitError,"Unable to clone dash pattern",
          "Memory allocation failed");
      (void) memcpy(clone_info->dash_pattern,draw_info->dash_pattern,
        (x+1)*sizeof(unsigned int));
    }
  if (draw_info->clip_path != (char *) NULL)
    clone_info->clip_path=AllocateString(draw_info->clip_path);
  return(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o l o r F l o o d f i l l I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ColorFloodfill() changes the color value of any pixel that matches
%  target and is an immediate neighbor.  If the method is
%  FillToBorderMethod is specified, the color value is changed for any
%  neighbor pixel that does not match the bordercolor member of image.
%
%  By default target must match a particular pixel color exactly.
%  However, in many cases two colors may differ by a small amount.  The
%  fuzz member of image defines how much tolerance is acceptable to
%  consider two colors as the same.  For example, set fuzz to 10 and the
%  color red at intensities of 100 and 102 respectively are now
%  interpreted as the same color for the purposes of the floodfill.
%
%  The format of the ColorFloodfillImage method is:
%
%      unsigned int ColorFloodfillImage(Image *image,const DrawInfo *draw_info,
%        const PixelPacket target,const long x_offset,const long y_offset,
%        const PaintMethod method)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o target: The RGB value of the target color.
%
%    o x,y: The starting location of the operation.
%
%    o method: Choose either FloodfillMethod or FillToBorderMethod.
%
%
*/
MagickExport unsigned int ColorFloodfillImage(Image *image,
  const DrawInfo *draw_info,const PixelPacket target,const long x_offset,
  const long y_offset,const PaintMethod method)
{
  int
    offset,
    skip;

  long
    start,
    x1,
    x2,
    y;

  PixelPacket
    color;

  register long
    x;

  register PixelPacket
    *q;

  register SegmentInfo
    *s;

  SegmentInfo
    *segment_stack;

  unsigned char
    *floodplane;

  /*
    Check boundary conditions.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  if ((x_offset < 0) || (x_offset >= (long) image->columns))
    return(False);
  if ((y_offset < 0) || (y_offset >= (long) image->rows))
    return(False);
  /*
    Set floodfill color.
  */
  if (ColorMatch(draw_info->fill,target,image->fuzz))
    return(False);
  floodplane=(unsigned char *) AcquireMemory(image->columns*image->rows);
  segment_stack=(SegmentInfo *) AcquireMemory(MaxStacksize*sizeof(SegmentInfo));
  if ((floodplane== (unsigned char *) NULL) ||
      (segment_stack == (SegmentInfo *) NULL))
    ThrowBinaryException(ResourceLimitWarning,"Unable to floodfill image",
      image->filename);
  (void) memset(floodplane,0,image->columns*image->rows);
  /*
    Push initial segment on stack.
  */
  SetImageType(image,TrueColorType);
  x=x_offset;
  y=y_offset;
  start=0;
  s=segment_stack;
  Push(y,x,x,1);
  Push(y+1,x,x,-1);
  while (s > segment_stack)
  {
    /*
      Pop segment off stack.
    */
    s--;
    x1=(int) s->x1;
    x2=(int) s->x2;
    offset=(int) s->y2;
    y=(int) s->y1+offset;
    /*
      Recolor neighboring pixels.
    */
    q=GetImagePixels(image,0,y,x1+1,1);
    if (q == (PixelPacket *) NULL)
      break;
    q+=x1;
    for (x=x1; x >= 0; x--)
    {
      if (method == FloodfillMethod)
        {
          if (!ColorMatch(*q,target,image->fuzz))
            break;
        }
      else
        if (ColorMatch(*q,target,image->fuzz) ||
            ColorMatch(*q,draw_info->fill,image->fuzz))
          break;
      floodplane[y*image->columns+x]=True;
      *q=draw_info->fill;
      q--;
    }
    if (!SyncImagePixels(image))
      break;
    skip=x >= x1;
    if (!skip)
      {
        start=x+1;
        if (start < x1)
          Push(y,start,x1-1,-offset);
        x=x1+1;
      }
    do
    {
      if (!skip)
        {
          if (x < (long) image->columns)
            {
              q=GetImagePixels(image,x,y,image->columns-x+1,1);
              if (q == (PixelPacket *) NULL)
                break;
              for ( ; x < (long) image->columns; x++)
              {
                if (method == FloodfillMethod)
                  {
                    if (!ColorMatch(*q,target,image->fuzz))
                      break;
                  }
                else
                  if (ColorMatch(*q,target,image->fuzz) ||
                      ColorMatch(*q,draw_info->fill,image->fuzz))
                    break;
                floodplane[y*image->columns+x]=True;
                *q=draw_info->fill;
                q++;
              }
              if (!SyncImagePixels(image))
                break;
            }
          Push(y,start,x-1,offset);
          if (x > (x2+1))
            Push(y,x2+1,x-1,-offset);
        }
      skip=False;
      x++;
      if (x <= x2)
        {
          q=GetImagePixels(image,x,y,x2-x+1,1);
          if (q == (PixelPacket *) NULL)
            break;
          for ( ; x <= x2; x++)
          {
            if (method == FloodfillMethod)
              {
                if (ColorMatch(*q,target,image->fuzz))
                  break;
              }
            else
              if (!ColorMatch(*q,target,image->fuzz) &&
                  !ColorMatch(*q,draw_info->fill,image->fuzz))
                break;
            q++;
          }
        }
      start=x;
    } while (x <= x2);
  }
  if (draw_info->tile == (Image *) NULL)
    for (y=0; y < (long) image->rows; y++)
    {
      /*
        Tile fill color onto floodplane.
      */
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=0; x < (long) image->columns; x++)
      {
        if (floodplane[y*image->columns+x])
          *q=draw_info->fill;
        q++;
      }
      if (!SyncImagePixels(image))
        break;
    }
  else
    {
      /*
        Tile image onto floodplane.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          if (floodplane[y*image->columns+x])
            {
              color=GetOnePixel(draw_info->tile,x % draw_info->tile->columns,
                y % draw_info->tile->rows);
              if (!draw_info->tile->matte)
                color.opacity=OpaqueOpacity;
              AlphaComposite(&color,color.opacity,q,q->opacity);
            }
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
    }
  LiberateMemory((void **) &segment_stack);
  LiberateMemory((void **) &floodplane);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C o n v e r t P a t h T o P o l y g o n                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ConvertPathToPolygon converts a path to the more efficient sorted
%  rendering form.
%
%  The format of the ConvertPathToPolygon method is:
%
%      PolygonInfo *ConvertPathToPolygon(const DrawInfo *draw_info,
%        const PathInfo *path_info)
%
%  A description of each parameter follows:
%
%    o Method ConvertPathToPolygon returns the path in a more efficient sorted
%      rendering form of type PolygonInfo.
%
%    o draw_info: Specifies a pointer to an DrawInfo structure.
%
%    o path_info: Specifies a pointer to an PathInfo structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int CompareEdges(const void *x,const void *y)
{
  register const EdgeInfo
    *p,
    *q;

  /*
    Compare two edges.
  */
  p=(EdgeInfo *) x;
  q=(EdgeInfo *) y;
  if ((p->points[0].y-MagickEpsilon) > q->points[0].y)
    return(1);
  if ((p->points[0].y+MagickEpsilon) < q->points[0].y)
    return(-1);
  if ((p->points[0].x-MagickEpsilon) > q->points[0].x)
    return(1);
  if ((p->points[0].x+MagickEpsilon) < q->points[0].x)
    return(-1);
  if (((p->points[1].x-p->points[0].x)*(q->points[1].y-q->points[0].y)-
       (p->points[1].y-p->points[0].y)*(q->points[1].x-q->points[0].x)) > 0.0)
    return(1);
  return(-1);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

static void PrintPolygonInfo(const PolygonInfo *polygon_info)
{
  register EdgeInfo
    *p;

  register long
    i,
    j;

  (void) fprintf(stdout,"    begin active-edge\n");
  p=polygon_info->edges;
  for (i=0; i < polygon_info->number_edges; i++)
  {
    (void) fprintf(stdout,"      edge %lu:\n      direction: %s\n      "
      "ghostline: %s\n      bounds: %g,%g - %g,%g\n",i,
      p->direction ? "down" : "up",p->ghostline ? "transparent" : "opaque",
      p->bounds.x1,p->bounds.y1,p->bounds.x2,p->bounds.y2);
    for (j=0; j < p->number_points; j++)
      (void) fprintf(stdout,"        %g,%g\n",p->points[j].x,p->points[j].y);
    p++;
  }
  (void) fprintf(stdout,"    end active-edge\n");
}

static void ReversePoints(PointInfo *points,const int number_points)
{
  PointInfo
    point;

  register int
    i;

  for (i=0; i < (number_points >> 1); i++)
  {
    point=points[i];
    points[i]=points[number_points-(i+1)];
    points[number_points-(i+1)]=point;
  }
}

static PolygonInfo *ConvertPathToPolygon(const DrawInfo *draw_info,
  const PathInfo *path_info)
{
  int
    direction,
    edge,
    ghostline,
    next_direction,
    number_edges,
    number_points;

  PointInfo
    point,
    *points;

  PolygonInfo
    *polygon_info;

  SegmentInfo
    bounds;

  register int
    i,
    n;

  /*
    Convert a path to the more efficient sorted rendering form.
  */
  polygon_info=(PolygonInfo *) AcquireMemory(sizeof(PolygonInfo));
  if (polygon_info == (PolygonInfo *) NULL)
    return((PolygonInfo *) NULL);
  number_edges=16;
  polygon_info->edges=(EdgeInfo *) AcquireMemory(number_edges*sizeof(EdgeInfo));
  if (polygon_info->edges == (EdgeInfo *) NULL)
    return((PolygonInfo *) NULL);
  direction=0;
  edge=0;
  ghostline=False;
  n=0;
  number_points=0;
  points=(PointInfo *) NULL;
  (void) memset(&point,0,sizeof(PointInfo));
  (void) memset(&bounds,0,sizeof(SegmentInfo));
  for (i=0; path_info[i].code != EndCode; i++)
  {
    if ((path_info[i].code == MoveToCode) || (path_info[i].code == OpenCode) ||
        (path_info[i].code == GhostlineCode))
      {
        /*
          Move to.
        */
        if ((points != (PointInfo *) NULL) && (n >= 2))
          {
            if (edge == number_edges)
              {
                number_edges<<=1;
                ReacquireMemory((void **) &polygon_info->edges,
                  number_edges*sizeof(EdgeInfo));
                if (polygon_info->edges == (EdgeInfo *) NULL)
                  return((PolygonInfo *) NULL);
              }
            polygon_info->edges[edge].number_points=n;
            polygon_info->edges[edge].scanline=(-1.0);
            polygon_info->edges[edge].highwater=0;
            polygon_info->edges[edge].ghostline=ghostline;
            polygon_info->edges[edge].direction=direction > 0;
            if (direction < 0)
              ReversePoints(points,n);
            polygon_info->edges[edge].points=points;
            polygon_info->edges[edge].bounds=bounds;
            polygon_info->edges[edge].bounds.y1=points[0].y;
            polygon_info->edges[edge].bounds.y2=points[n-1].y;
            points=(PointInfo *) NULL;
            ghostline=False;
            edge++;
          }
        if (points == (PointInfo *) NULL)
          {
            number_points=16;
            points=(PointInfo *) AcquireMemory(number_points*sizeof(PointInfo));
            if (points == (PointInfo *) NULL)
              return((PolygonInfo *) NULL);
          }
        ghostline=path_info[i].code == GhostlineCode;
        point=path_info[i].point;
        points[0]=point;
        bounds.x1=point.x;
        bounds.x2=point.x;
        direction=0;
        n=1;
        continue;
      }
    /*
      Line to.
    */
    next_direction=((path_info[i].point.y > point.y) ||
      ((path_info[i].point.y == point.y) &&
       (path_info[i].point.x > point.x))) ? 1 : -1;
    if (direction && (direction != next_direction))
      {
        /*
          New edge.
        */
        point=points[n-1];
        if (edge == number_edges)
          {
            number_edges<<=1;
            ReacquireMemory((void **) &polygon_info->edges,
              number_edges*sizeof(EdgeInfo));
            if (polygon_info->edges == (EdgeInfo *) NULL)
              return((PolygonInfo *) NULL);
          }
        polygon_info->edges[edge].number_points=n;
        polygon_info->edges[edge].scanline=(-1.0);
        polygon_info->edges[edge].highwater=0;
        polygon_info->edges[edge].ghostline=ghostline;
        polygon_info->edges[edge].direction=direction > 0;
        if (direction < 0)
          ReversePoints(points,n);
        polygon_info->edges[edge].points=points;
        polygon_info->edges[edge].bounds=bounds;
        polygon_info->edges[edge].bounds.y1=points[0].y;
        polygon_info->edges[edge].bounds.y2=points[n-1].y;
        number_points=16;
        points=(PointInfo *) AcquireMemory(number_points*sizeof(PointInfo));
        if (points == (PointInfo *) NULL)
          return((PolygonInfo *) NULL);
        n=1;
        ghostline=False;
        points[0]=point;
        bounds.x1=point.x;
        bounds.x2=point.x;
        edge++;
      }
    direction=next_direction;
    if (points == (PointInfo *) NULL)
      continue;
    if (n == number_points)
      {
        number_points<<=1;
        ReacquireMemory((void **) &points,number_points*sizeof(PointInfo));
        if (points == (PointInfo *) NULL)
          return((PolygonInfo *) NULL);
      }
    point=path_info[i].point;
    points[n]=point;
    if (point.x < bounds.x1)
      bounds.x1=point.x;
    if (point.x > bounds.x2)
      bounds.x2=point.x;
    n++;
  }
  if (points != (PointInfo *) NULL)
    {
      if (n < 2)
        LiberateMemory((void **) &points);
      else
        {
          if (edge == number_edges)
            {
              number_edges<<=1;
              ReacquireMemory((void **) &polygon_info->edges,
                number_edges*sizeof(EdgeInfo));
              if (polygon_info->edges == (EdgeInfo *) NULL)
                return((PolygonInfo *) NULL);
            }
          polygon_info->edges[edge].number_points=n;
          polygon_info->edges[edge].scanline=(-1.0);
          polygon_info->edges[edge].highwater=0;
          polygon_info->edges[edge].ghostline=ghostline;
          polygon_info->edges[edge].direction=direction > 0;
          if (direction < 0)
            ReversePoints(points,n);
          polygon_info->edges[edge].points=points;
          polygon_info->edges[edge].bounds=bounds;
          polygon_info->edges[edge].bounds.y1=points[0].y;
          polygon_info->edges[edge].bounds.y2=points[n-1].y;
          ghostline=False;
          edge++;
        }
    }
  polygon_info->number_edges=edge;
  qsort(polygon_info->edges,polygon_info->number_edges,sizeof(EdgeInfo),
    CompareEdges);
  if (draw_info->debug)
    PrintPolygonInfo(polygon_info);
  return(polygon_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C o n v e r t P r i m i t i v e T o P a t h                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ConvertPrimitiveToPath converts a PrimitiveInfo structure into a
%  vector path structure.
%
%  The format of the ConvertPrimitiveToPath method is:
%
%      PathInfo *ConvertPrimitiveToPath(const DrawInfo *draw_info,
%        const PrimitiveInfo *primitive_info)
%
%  A description of each parameter follows:
%
%    o Method ConvertPrimitiveToPath returns a vector path structure of type
%      PathInfo.
%
%    o draw_info: a structure of type DrawInfo.
%
%    o primitive_info: Specifies a pointer to an PrimitiveInfo structure.
%
%
*/

static void PrintPathInfo(const PathInfo *path_info)
{
  register const PathInfo
    *p;

  (void) fprintf(stdout,"    begin vector-path\n");
  for (p=path_info; p->code != EndCode; p++)
    (void) fprintf(stdout,"      %g,%g %s\n",p->point.x,p->point.y,
      p->code == GhostlineCode ? "moveto ghostline" :
      p->code == OpenCode ? "moveto open" : p->code == MoveToCode ? "moveto" :
      p->code == LineToCode ? "lineto" : "?");
  (void) fprintf(stdout,"    end vector-path\n");
}

static PathInfo *ConvertPrimitiveToPath(const DrawInfo *draw_info,
  const PrimitiveInfo *primitive_info)
{
  long
    coordinates,
    start;

  PathInfo
    *path_info;

  PathInfoCode
    code;

  PointInfo
    p,
    q;

  register int
    i,
    n;

  /*
    Converts a PrimitiveInfo structure into a vector path structure.
  */
  switch (primitive_info->primitive)
  {
    case PointPrimitive:
    case ColorPrimitive:
    case MattePrimitive:
    case TextPrimitive:
    case ImagePrimitive:
      return((PathInfo *) NULL);
    default:
      break;
  }
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++);
  path_info=(PathInfo *) AcquireMemory((2*i+3)*sizeof(PathInfo));
  if (path_info == (PathInfo *) NULL)
    return((PathInfo *) NULL);
  coordinates=0;
  n=0;
  q.x=(-1);
  q.y=(-1);
  start=0;
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
  {
    code=LineToCode;
    if (coordinates <= 0)
      {
        coordinates=primitive_info[i].coordinates;
        p=primitive_info[i].point;
        start=n;
        code=MoveToCode;
      }
    coordinates--;
    /*
      Eliminate duplicate points.
    */
    if ((fabs(q.x-primitive_info[i].point.x) > MagickEpsilon) ||
        (fabs(q.y-primitive_info[i].point.y) > MagickEpsilon))
      {
        path_info[n].code=code;
        path_info[n].point=primitive_info[i].point;
        q=primitive_info[i].point;
        n++;
      }
    if (coordinates > 0)
      continue;
    if ((fabs(p.x-primitive_info[i].point.x) <= MagickEpsilon) &&
        (fabs(p.y-primitive_info[i].point.y) <= MagickEpsilon))
      continue;
    /*
      Mark the p point as open if it does not match the q.
    */
    path_info[start].code=OpenCode;
    path_info[n].code=GhostlineCode;
    path_info[n].point=primitive_info[i].point;
    n++;
    path_info[n].code=LineToCode;
    path_info[n].point=p;
    n++;
  }
  path_info[n].code=EndCode;
  path_info[n].point.x=0.0;
  path_info[n].point.y=0.0;
  if (draw_info->debug)
    PrintPathInfo(path_info);
  return(path_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y D r a w I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyDrawInfo() deallocates memory associated with an DrawInfo
%  structure.
%
%  The format of the DestroyDrawInfo method is:
%
%      void DestroyDrawInfo(DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o draw_info: The draw info.
%
%
*/
MagickExport void DestroyDrawInfo(DrawInfo *draw_info)
{
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  if (draw_info->primitive != (char *) NULL)
    LiberateMemory((void **) &draw_info->primitive);
  if (draw_info->text != (char *) NULL)
    LiberateMemory((void **) &draw_info->text);
  if (draw_info->geometry != (char *) NULL)
    LiberateMemory((void **) &draw_info->geometry);
  if (draw_info->font != (char *) NULL)
    LiberateMemory((void **) &draw_info->font);
  if (draw_info->density != (char *) NULL)
    LiberateMemory((void **) &draw_info->density);
  if (draw_info->tile != (Image *) NULL)
    DestroyImage(draw_info->tile);
  if (draw_info->server_name != (char *) NULL)
    LiberateMemory((void **) &draw_info->server_name);
  if (draw_info->dash_pattern != (unsigned int *) NULL)
    LiberateMemory((void **) &draw_info->dash_pattern);
  if (draw_info->clip_path != (char *) NULL)
    LiberateMemory((void **) &draw_info->clip_path);
  LiberateMemory((void **) &draw_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y E d g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyEdge destroys the specified polygon edge.
%
%  The format of the DestroyEdge method is:
%
%      long DestroyEdge(PolygonInfo *polygon_info,const int edge)
%
%  A description of each parameter follows:
%
%    o polygon_info: Specifies a pointer to an PolygonInfo structure.
%
%    o edge: the polygon edge number to destroy.
%
%
*/
static long DestroyEdge(PolygonInfo *polygon_info,const long edge)
{
  assert(edge >= 0);
  assert(edge < polygon_info->number_edges);
  LiberateMemory((void **) &polygon_info->edges[edge].points);
  polygon_info->number_edges--;
  if (edge < polygon_info->number_edges)
    (void) memcpy(polygon_info->edges+edge,polygon_info->edges+edge+1,
      (polygon_info->number_edges-edge)*sizeof(EdgeInfo));
  return(polygon_info->number_edges);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y P o l y g o n I n f o                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyPolygonInfo destroys the PolygonInfo data structure.
%
%  The format of the DestroyPolygonInfo method is:
%
%      void DestroyPolygonInfo(PolygonInfo *polygon_info)
%
%  A description of each parameter follows:
%
%    o polygon_info: Specifies a pointer to an PolygonInfo structure.
%
%
*/
static void DestroyPolygonInfo(PolygonInfo *polygon_info)
{
  register long
    i;

  for (i=0; i < polygon_info->number_edges; i++)
    LiberateMemory((void **) &polygon_info->edges[i].points);
  LiberateMemory((void **) &polygon_info->edges);
  LiberateMemory((void **) &polygon_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D r a w B o u n d i n g R e c t a n g l e s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawBoundingRectangles draws the bounding rectangles on the image.
%  This is only useful for developers debugging the rendering algorithm.
%
%  The format of the DrawBoundingRectangles method is:
%
%      void DrawBoundingRectangles(Image *image,const DrawInfo *draw_info,
%        PolygonInfo *polygon_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o polygon_info: Specifies a pointer to a PolygonInfo structure.
%
%
*/
static void DrawBoundingRectangles(Image *image,const DrawInfo *draw_info,
  const PolygonInfo *polygon_info)
{
  double
    mid;

  DrawInfo
    *clone_info;

  long
    coordinates;

  PointInfo
    end,
    resolution,
    start;

  PrimitiveInfo
    primitive_info[6];

  register long
    i;

  SegmentInfo
    bounds;

  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  clone_info->debug=False;
  (void) QueryColorDatabase("#000000ff",&clone_info->fill);
  resolution.x=72.0;
  resolution.y=72.0;
  if (clone_info->density != (char *) NULL)
    {
      int
        count;

      count=sscanf(clone_info->density,"%lfx%lf",&resolution.x,&resolution.y);
      if (count != 2)
        resolution.y=resolution.x;
    }
  mid=(resolution.x/72.0)*ExpandAffine(&clone_info->affine)*
    clone_info->stroke_width/2.0;
  if (polygon_info != (PolygonInfo *) NULL)
    {
      bounds=polygon_info->edges[0].bounds;
      for (i=1; i < polygon_info->number_edges; i++)
      {
        if (polygon_info->edges[i].bounds.x1 < bounds.x1)
          bounds.x1=polygon_info->edges[i].bounds.x1;
        if (polygon_info->edges[i].bounds.y1 < bounds.y1)
          bounds.y1=polygon_info->edges[i].bounds.y1;
        if (polygon_info->edges[i].bounds.x2 > bounds.x2)
          bounds.x2=polygon_info->edges[i].bounds.x2;
        if (polygon_info->edges[i].bounds.y2 > bounds.y2)
          bounds.y2=polygon_info->edges[i].bounds.y2;
      }
      bounds.x1-=mid;
      bounds.x1=bounds.x1 < 0.0 ? 0.0 :
        bounds.x1 >= image->columns ? image->columns-1 : bounds.x1;
      bounds.y1-=mid;
      bounds.y1=bounds.y1 < 0.0 ? 0.0 :
        bounds.y1 >= image->rows ? image->rows-1 : bounds.y1;
      bounds.x2+=mid;
      bounds.x2=bounds.x2 < 0.0 ? 0.0 :
        bounds.x2 >= image->columns ? image->columns-1 : bounds.x2;
      bounds.y2+=mid;
      bounds.y2=bounds.y2 < 0.0 ? 0.0 :
        bounds.y2 >= image->rows ? image->rows-1 : bounds.y2;
      for (i=0; i < polygon_info->number_edges; i++)
      {
        if (polygon_info->edges[i].direction)
          (void) QueryColorDatabase("red",&clone_info->stroke);
        else
          (void) QueryColorDatabase("green",&clone_info->stroke);
        start.x=floor(polygon_info->edges[i].bounds.x1-mid-0.5);
        start.y=floor(polygon_info->edges[i].bounds.y1-mid-0.5);
        end.x=ceil(polygon_info->edges[i].bounds.x2+mid-0.5);
        end.y=ceil(polygon_info->edges[i].bounds.y2+mid-0.5);
        primitive_info[0].primitive=RectanglePrimitive;
        TraceRectangle(primitive_info,start,end);
        primitive_info[0].method=ReplaceMethod;
        coordinates=primitive_info[0].coordinates;
        primitive_info[coordinates].primitive=UndefinedPrimitive;
        (void) DrawPrimitive(image,clone_info,primitive_info);
      }
    }
  (void) QueryColorDatabase("blue",&clone_info->stroke);
  start.x=floor(bounds.x1-mid-0.5);
  start.y=floor(bounds.y1-mid-0.5);
  end.x=ceil(bounds.x2+mid-0.5);
  end.y=ceil(bounds.y2+mid-0.5);
  primitive_info[0].primitive=RectanglePrimitive;
  TraceRectangle(primitive_info,start,end);
  primitive_info[0].method=ReplaceMethod;
  coordinates=primitive_info[0].coordinates;
  primitive_info[coordinates].primitive=UndefinedPrimitive;
  (void) DrawPrimitive(image,clone_info,primitive_info);
  DestroyDrawInfo(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w C l i p P a t h                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawClipPath() draws the clip path on the image mask.
%
%  The format of the DrawClipPath method is:
%
%      unsigned int DrawClipPath(Image *image,DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%
*/
static unsigned int DrawClipPath(Image *image,DrawInfo *draw_info)
{
  char
    clip_path[MaxTextExtent];

  DrawInfo
    *clone_info;

  ImageAttribute
    *attribute;

  unsigned int
    status;

  assert(draw_info != (const DrawInfo *) NULL);
  FormatString(clip_path,"[%.1024s]",draw_info->clip_path);
  attribute=GetImageAttribute(image,clip_path);
  if (attribute == (ImageAttribute *) NULL)
    return(False);
  if (image->clip_mask != (Image *) NULL)
    ModifyImage(&image->clip_mask,&image->exception);
  else
    {
      Image
        *clip_mask;

      clip_mask=CloneImage(image,image->columns,image->rows,True,
        &image->exception);
      if (clip_mask == (Image *) NULL)
        return(False);
      (void) SetImageClipMask(image,clip_mask);
      DestroyImage(clip_mask);
    }
  SetImage(image->clip_mask,TransparentOpacity);
  if (draw_info->debug)
    (void) fprintf(stdout,"\nbegin clip-path %.1024s\n",draw_info->clip_path);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  (void) CloneString(&clone_info->primitive,attribute->value);
  (void) QueryColorDatabase("black",&clone_info->fill);
  clone_info->clip_path=(char *) NULL;
  status=DrawImage(image->clip_mask,clone_info);
  draw_info->clip_units=clone_info->clip_units;
  DestroyDrawInfo(clone_info);
  if (draw_info->debug)
    (void) fprintf(stdout,"end clip-path\n\n");
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D r a w D a s h P o l y g o n                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawDashPolygon draws a dashed polygon (line, rectangle, ellipse)
%  on the image while respecting the dash offset and dash pattern attributes.
%
%  The format of the DrawDashPolygon method is:
%
%      unsigned int DrawDashPolygon(const DrawInfo *draw_info,
%        const PrimitiveInfo *primitive_info,Image *image)
%
%  A description of each parameter follows:
%
%    o draw_info: The draw info.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%    o image: The image.
%
%
*/
static unsigned int DrawDashPolygon(const DrawInfo *draw_info,
  const PrimitiveInfo *primitive_info,Image *image)
{
  double
    dash_offset,
    distance,
    maximum_distance,
    scale,
    this_distance;

  DrawInfo
    *clone_info;

  int
    j,
    n;

  PrimitiveInfo
    *dash_polygon;

  register double
    dx,
    dy;

  register int
    i;

  TimerInfo
    timer;

  unsigned int
    number_vertices,
    status;

  assert(draw_info != (const DrawInfo *) NULL);
  if (draw_info->debug)
    {
      GetTimerInfo(&timer);
      (void) fprintf(stdout,"    begin draw-dash\n");
    }
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  clone_info->miterlimit=0;
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++);
  number_vertices=i;
  dash_polygon=(PrimitiveInfo *)
    AcquireMemory((2*number_vertices+1)*sizeof(PrimitiveInfo));
  if (dash_polygon == (PrimitiveInfo *) NULL)
    return(False);
  scale=ExpandAffine(&draw_info->affine);
  dash_offset=draw_info->dash_offset != 0 ? scale*draw_info->dash_offset : 0.0;
  distance=0.0;
  for (n=(-1); dash_offset > 0.0; )
  {
    if (n == -1)
      n=0;
    if (draw_info->dash_pattern[n] == 0)
      break;
    if (dash_offset > (scale*draw_info->dash_pattern[n]))
      {
        dash_offset-=scale*draw_info->dash_pattern[n];
        n++;
        continue;
      }
    if (dash_offset < (scale*draw_info->dash_pattern[n]))
      {
        distance=scale*draw_info->dash_pattern[n]-dash_offset;
        dash_offset=0.0;
        break;
      }
    dash_offset=0.0;
    distance=scale*draw_info->dash_pattern[n];
    n++;
  }
  j=0;
  if (n == -1)
    {
      distance=scale*draw_info->dash_pattern[0];
      dash_polygon[0]=primitive_info[0];
      n=0;
      j=1;
    }
  status=True;
  for (i=1; i < (int) number_vertices; i++)
  {
    dx=primitive_info[i].point.x-primitive_info[i-1].point.x;
    dy=primitive_info[i].point.y-primitive_info[i-1].point.y;
    maximum_distance=sqrt(dx*dx+dy*dy+MagickEpsilon);
    if (distance == 0)
      {
        n++;
        if (draw_info->dash_pattern[n] == 0)
          n=0;
        distance=scale*draw_info->dash_pattern[n];
      }
    for (this_distance=0.0; maximum_distance >= (distance+this_distance); )
    {
      this_distance+=distance;
      if (n & 0x01)
        {
          dash_polygon[0]=primitive_info[0];
          dash_polygon[0].point.x=primitive_info[i-1].point.x+
            dx*this_distance/maximum_distance;
          dash_polygon[0].point.y=primitive_info[i-1].point.y+
            dy*this_distance/maximum_distance;
          j=1;
        }
      else
        {
          dash_polygon[j]=primitive_info[i-1];
          dash_polygon[j].point.x=primitive_info[i-1].point.x+
            dx*this_distance/maximum_distance;
          dash_polygon[j].point.y=primitive_info[i-1].point.y+
            dy*this_distance/maximum_distance;
          dash_polygon[j].coordinates=1;
          j++;
          dash_polygon[0].coordinates=j;
          dash_polygon[j].primitive=UndefinedPrimitive;
          status|=DrawStrokePolygon(image,clone_info,dash_polygon);
        }
      n++;
      if (draw_info->dash_pattern[n] == 0)
        n=0;
      distance=scale*draw_info->dash_pattern[n];
    }
    distance-=(maximum_distance-this_distance);
    if (n & 0x01)
      continue;
    dash_polygon[j]=primitive_info[i];
    dash_polygon[j].coordinates=1;
    j++;
  }
  LiberateMemory((void **) &dash_polygon);
  DestroyDrawInfo(clone_info);
  if (draw_info->debug)
    (void) fprintf(stdout,"    end draw-dash (%.2fu)\n",GetUserTime(&timer));
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawImage() allows you to draw a graphic primitive on your image.  The
%  primitive may be represented as a string or filename.  Precede the
%  filename with an ampersand (@) and the contents of the file are drawn
%  on the image.  image.   You can affect how text is drawn by setting one
%  or more members of the draw info structure.
%
%  The format of the DrawImage method is:
%
%      unsigned int DrawImage(Image *image,DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%
*/
MagickExport unsigned int DrawImage(Image *image,DrawInfo *draw_info)
{
  AffineMatrix
    affine,
    current;

  char
    keyword[MaxTextExtent],
    *primitive,
    *q,
    *token;

  double
    angle,
    factor;

  DrawInfo
    **graphic_context;

  long
    j,
    n;

  PointInfo
    point;

  PrimitiveInfo
    *primitive_info;

  PrimitiveType
    primitive_type;

  register char
    *p;

  register long
    i,
    x;

  size_t
    length;

  TimerInfo
    timer;

  unsigned int
    status;

  unsigned long
    number_points;

  /*
    Ensure the annotation info is valid.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  assert(draw_info->primitive != (char *) NULL);
  if (*draw_info->primitive == '\0')
    return(False);
  if (draw_info->debug)
    {
      GetTimerInfo(&timer);
      (void) fprintf(stdout,"begin draw-image\n");
    }
  if (*draw_info->primitive != '@')
    primitive=TranslateText((ImageInfo *) NULL,image,draw_info->primitive);
  else
    {
      char
        *text;

      /*
        Read text from a file.
      */
      text=(char *)
        FileToBlob(draw_info->primitive+1,&length,&image->exception);
      if (text == (char *) NULL)
        return(False);
      primitive=TranslateText((ImageInfo *) NULL,image,text);
      LiberateMemory((void **) &text);
    }
  (void) SetImageAttribute(image,"[MVG]",primitive);
  n=0;
  /*
    Allocate primitive info memory.
  */
  graphic_context=(DrawInfo **) AcquireMemory(sizeof(DrawInfo *));
  if (graphic_context == (DrawInfo **) NULL)
    {
      LiberateMemory((void **) &primitive);
      ThrowBinaryException(ResourceLimitWarning,"Unable to draw image",
        "Memory allocation failed")
    }
  number_points=2047;
  primitive_info=(PrimitiveInfo *)
    AcquireMemory(number_points*sizeof(PrimitiveInfo));
  if (primitive_info == (PrimitiveInfo *) NULL)
    {
      LiberateMemory((void **) &primitive);
      for ( ; n >= 0; n--)
        DestroyDrawInfo(graphic_context[n]);
      LiberateMemory((void **) &graphic_context);
      ThrowBinaryException(ResourceLimitWarning,"Unable to draw image",
        "Memory allocation failed")
    }
  graphic_context[n]=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  token=AllocateString(primitive);
  status=True;
  for (q=primitive; *q != '\0'; )
  {
    /*
      Interpret graphic primitive.
    */
    GetToken(q,&q,keyword);
    if (*keyword == '\0')
      break;
    if (*keyword == '#')
      {
        /*
          Comment.
        */
        while ((*q != '\n') && (*q != '\0'))
          q++;
        continue;
      }
    p=q-strlen(keyword);
    primitive_type=UndefinedPrimitive;
    current=graphic_context[n]->affine;
    IdentityAffine(&affine);
    switch (*keyword)
    {
      case ';':
        break;
      case 'a':
      case 'A':
      {
        if (LocaleCompare("affine",keyword) == 0)
          {
            GetToken(q,&q,token);
            affine.sx=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.rx=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.ry=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.sy=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.tx=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.ty=atof(token);
            break;
          }
        if (LocaleCompare("angle",keyword) == 0)
          {
            GetToken(q,&q,token);
            break;
          }
        if (LocaleCompare("arc",keyword) == 0)
          {
            primitive_type=ArcPrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'b':
      case 'B':
      {
        if (LocaleCompare("bezier",keyword) == 0)
          {
            primitive_type=BezierPrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'c':
      case 'C':
      {
        if (LocaleCompare("clip-path",keyword) == 0)
          {
            /*
              Create clip mask.
            */
            GetToken(q,&q,token);
            graphic_context[n]->clip_path=AllocateString(token);
            (void) DrawClipPath(image,graphic_context[n]);
            break;
          }
        if (LocaleCompare("clip-rule",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("evenodd",token) == 0)
              {
                graphic_context[n]->fill_rule=EvenOddRule;
                break;
              }
            if (LocaleCompare("nonzero",token) == 0)
              {
                graphic_context[n]->fill_rule=NonZeroRule;
                break;
              }
            status=False;
            break;
          }
        if (LocaleCompare("clip-units",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("userSpace",token) == 0)
              {
                draw_info->clip_units=UserSpace;
                break;
              }
            if (LocaleCompare("userSpaceOnUse",token) == 0)
              {
                draw_info->clip_units=UserSpaceOnUse;
                break;
              }
            if (LocaleCompare("objectBoundingBox",token) == 0)
              {
                draw_info->clip_units=ObjectBoundingBox;
                IdentityAffine(&current);
                affine.sx=draw_info->bounds.x2;
                affine.sy=draw_info->bounds.y2;
                affine.tx=draw_info->bounds.x1;
                affine.ty=draw_info->bounds.y1;
                break;
              }
            status=False;
            break;
          }
        if (LocaleCompare("circle",keyword) == 0)
          {
            primitive_type=CirclePrimitive;
            break;
          }
        if (LocaleCompare("color",keyword) == 0)
          {
            primitive_type=ColorPrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'd':
      case 'D':
      {
        if (LocaleCompare("decorate",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("#000000ff",token) == 0)
              {
                graphic_context[n]->decorate=NoDecoration;
                break;
              }
            if (LocaleCompare("underline",token) == 0)
              {
                graphic_context[n]->decorate=UnderlineDecoration;
                break;
              }
            if (LocaleCompare("overline",token) == 0)
              {
                graphic_context[n]->decorate=OverlineDecoration;
                break;
              }
            if (LocaleCompare("line-through",token) == 0)
              {
                graphic_context[n]->decorate=LineThroughDecoration;
                break;
              }
            status=False;
            break;
          }
        status=False;
        break;
      }
      case 'e':
      case 'E':
      {
        if (LocaleCompare("ellipse",keyword) == 0)
          {
            primitive_type=EllipsePrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'f':
      case 'F':
      {
        if (LocaleCompare("fill",keyword) == 0)
          {
            GetToken(q,&q,token);
            (void) QueryColorDatabase(token,&graphic_context[n]->fill);
            if (graphic_context[n]->fill.opacity != TransparentOpacity)
              graphic_context[n]->fill.opacity=graphic_context[n]->opacity;
            break;
          }
        if (LocaleCompare("fill-rule",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("evenodd",token) == 0)
              {
                graphic_context[n]->fill_rule=EvenOddRule;
                break;
              }
            if (LocaleCompare("nonzero",token) == 0)
              {
                graphic_context[n]->fill_rule=NonZeroRule;
                break;
              }
            status=False;
            break;
          }
        if (LocaleCompare("fill-opacity",keyword) == 0)
          {
            GetToken(q,&q,token);
            factor=strchr(token,'%') != (char *) NULL ? 0.01 : 1.0;
            graphic_context[n]->fill.opacity=(Quantum)
              (MaxRGB*(1.0-factor*atof(token)));
            break;
          }
        if (LocaleCompare("font",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (draw_info->font != (char *) NULL)
              (void) strncpy(token,draw_info->font,MaxTextExtent-1);
            (void) CloneString(&graphic_context[n]->font,token);
            break;
          }
        if (LocaleCompare("font-size",keyword) == 0)
          {
            GetToken(q,&q,token);
            graphic_context[n]->pointsize=atof(token);
            break;
          }
        status=False;
        break;
      }
      case 'g':
      case 'G':
      {
        if (LocaleCompare("gradient",keyword) == 0)
          {
            GetToken(q,&q,token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            break;
          }
        if (LocaleCompare("gradient-units",keyword) == 0)
          {
            GetToken(q,&q,token);
            break;
          }
        if (LocaleCompare("gravity",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("NorthWest",token) == 0)
              {
                graphic_context[n]->gravity=NorthWestGravity;
                break;
              }
            if (LocaleCompare("North",token) == 0)
              {
                graphic_context[n]->gravity=NorthGravity;
                break;
              }
            if (LocaleCompare("NorthEast",token) == 0)
              {
                graphic_context[n]->gravity=NorthEastGravity;
                break;
              }
            if (LocaleCompare("West",token) == 0)
              {
                graphic_context[n]->gravity=WestGravity;
                break;
              }
            if (LocaleCompare("Center",token) == 0)
              {
                graphic_context[n]->gravity=CenterGravity;
                break;
              }
            if (LocaleCompare("East",token) == 0)
              {
                graphic_context[n]->gravity=EastGravity;
                break;
              }
            if (LocaleCompare("SouthWest",token) == 0)
              {
                graphic_context[n]->gravity=SouthWestGravity;
                break;
              }
            if (LocaleCompare("South",token) == 0)
              {
                graphic_context[n]->gravity=SouthGravity;
                break;
              }
            if (LocaleCompare("SouthEast",token) == 0)
              {
                graphic_context[n]->gravity=SouthEastGravity;
                break;
              }
            status=False;
            break;
          }
        status=False;
        break;
      }
      case 'i':
      case 'I':
      {
        if (LocaleCompare("image",keyword) == 0)
          {
            primitive_type=ImagePrimitive;
            GetToken(q,&q,token);
            if (LocaleCompare("Over",token) == 0)
              {
                graphic_context[n]->compose=OverCompositeOp;
                break;
              }
            if (LocaleCompare("In",token) == 0)
              {
                graphic_context[n]->compose=InCompositeOp;
                break;
              }
            if (LocaleCompare("Out",token) == 0)
              {
                graphic_context[n]->compose=OutCompositeOp;
                break;
              }
            if (LocaleCompare("Atop",token) == 0)
              {
                graphic_context[n]->compose=AtopCompositeOp;
                break;
              }
            if (LocaleCompare("Xor",token) == 0)
              {
                graphic_context[n]->compose=XorCompositeOp;
                break;
              }
            if (LocaleCompare("Plus",token) == 0)
              {
                graphic_context[n]->compose=PlusCompositeOp;
                break;
              }
            if (LocaleCompare("Minus",token) == 0)
              {
                graphic_context[n]->compose=MinusCompositeOp;
                break;
              }
            if (LocaleCompare("Add",token) == 0)
              {
                graphic_context[n]->compose=AddCompositeOp;
                break;
              }
            if (LocaleCompare("Subtract",token) == 0)
              {
                graphic_context[n]->compose=SubtractCompositeOp;
                break;
              }
            if (LocaleCompare("Difference",token) == 0)
              {
                graphic_context[n]->compose=DifferenceCompositeOp;
                break;
              }
            if (LocaleCompare("Multiply",token) == 0)
              {
                graphic_context[n]->compose=MultiplyCompositeOp;
                break;
              }
            if (LocaleCompare("Bumpmap",token) == 0)
              {
                graphic_context[n]->compose=BumpmapCompositeOp;
                break;
              }
            if (LocaleCompare("Copy",token) == 0)
              {
                graphic_context[n]->compose=CopyCompositeOp;
                break;
              }
            if (LocaleCompare("CopyRed",token) == 0)
              {
                graphic_context[n]->compose=CopyRedCompositeOp;
                break;
              }
            if (LocaleCompare("CopyGreen",token) == 0)
              {
                graphic_context[n]->compose=CopyGreenCompositeOp;
                break;
              }
            if (LocaleCompare("CopyBlue",token) == 0)
              {
                graphic_context[n]->compose=CopyBlueCompositeOp;
                break;
              }
            if (LocaleCompare("CopyOpacity",token) == 0)
              {
                graphic_context[n]->compose=CopyOpacityCompositeOp;
                break;
              }
            if (LocaleCompare("Clear",token) == 0)
              {
                graphic_context[n]->compose=ClearCompositeOp;
                break;
              }
            status=False;
            break;
          }
        status=False;
        break;
      }
      case 'l':
      case 'L':
      {
        if (LocaleCompare("line",keyword) == 0)
          {
            primitive_type=LinePrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'm':
      case 'M':
      {
        if (LocaleCompare("matte",keyword) == 0)
          {
            primitive_type=MattePrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'o':
      case 'O':
      {
        if (LocaleCompare("offset",keyword) == 0)
          {
            GetToken(q,&q,token);
            break;
          }
        if (LocaleCompare("opacity",keyword) == 0)
          {
            GetToken(q,&q,token);
            factor=strchr(token,'%') != (char *) NULL ? 0.01 : 1.0;
            graphic_context[n]->opacity=(Quantum) (MaxRGB*(1.0-((1.0-(double)
              graphic_context[n]->opacity/MaxRGB)*factor*atof(token))));
            graphic_context[n]->fill.opacity=graphic_context[n]->opacity;
            graphic_context[n]->stroke.opacity=graphic_context[n]->opacity;
            break;
          }
        status=False;
        break;
      }
      case 'p':
      case 'P':
      {
        if (LocaleCompare("path",keyword) == 0)
          {
            primitive_type=PathPrimitive;
            break;
          }
        if (LocaleCompare("point",keyword) == 0)
          {
            primitive_type=PointPrimitive;
            break;
          }
        if (LocaleCompare("polyline",keyword) == 0)
          {
            primitive_type=PolylinePrimitive;
            break;
          }
        if (LocaleCompare("polygon",keyword) == 0)
          {
            primitive_type=PolygonPrimitive;
            break;
          }
        if (LocaleCompare("pop",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("clip-path",token) == 0)
              break;
            if (LocaleCompare("defs",token) == 0)
              break;
            if (LocaleCompare("graphic-context",token) == 0)
              {
                if (graphic_context[n]->clip_path != (char *) NULL)
                  if (LocaleCompare(graphic_context[n]->clip_path,
                      graphic_context[n-1]->clip_path) != 0)
                    (void) SetImageClipMask(image,(Image *) NULL);
                DestroyDrawInfo(graphic_context[n]);
                n--;
                if (n < 0)
                  ThrowException(&image->exception,CorruptImageWarning,
                    "unbalanced graphic context push/pop",token);
                break;
              }
            if (LocaleCompare("linear-gradient",token) == 0)
              break;
            status=False;
            break;
          }
        if (LocaleCompare("push",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("clip-path",token) == 0)
              {
                char
                  name[MaxTextExtent];

                GetToken(q,&q,token);
                FormatString(name,"[%.1024s]",token);
                for (p=q; *q != '\0'; )
                {
                  GetToken(q,&q,token);
                  if (LocaleCompare(token,"pop") != 0)
                    continue;
                  GetToken(q,(char **) NULL,token);
                  if (LocaleCompare(token,"clip-path") != 0)
                    continue;
                  break;
                }
                (void) strncpy(token,p,q-p-4);
                token[q-p-4]='\0';
                (void) SetImageAttribute(image,name,token);
                GetToken(q,&q,token);
                break;
              }
            if (LocaleCompare("graphic-context",token) == 0)
              {
                n++;
                ReacquireMemory((void **) &graphic_context,
                  (n+1)*sizeof(DrawInfo *));
                if (graphic_context == (DrawInfo **) NULL)
                  {
                    ThrowException(&image->exception,ResourceLimitWarning,
                      "Unable to draw image","Memory allocation failed");
                    break;
                  }
                graphic_context[n]=
                  CloneDrawInfo((ImageInfo *) NULL,graphic_context[n-1]);
                break;
              }
            if (LocaleCompare("defs",token) == 0)
              break;
            if (LocaleCompare("linear-gradient",token) == 0)
              {
                GetToken(q,&q,token);
                break;
              }
            status=False;
            break;
          }
        status=False;
        break;
      }
      case 'r':
      case 'R':
      {
        if (LocaleCompare("rectangle",keyword) == 0)
          {
            primitive_type=RectanglePrimitive;
            break;
          }
        if (LocaleCompare("rotate",keyword) == 0)
          {
            GetToken(q,&q,token);
            angle=atof(token);
            affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
            affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
            affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
            affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
            break;
          }
        if (LocaleCompare("roundRectangle",keyword) == 0)
          {
            primitive_type=RoundRectanglePrimitive;
            break;
          }
        status=False;
        break;
      }
      case 's':
      case 'S':
      {
        if (LocaleCompare("scale",keyword) == 0)
          {
            GetToken(q,&q,token);
            affine.sx=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.sy=atof(token);
            break;
          }
        if (LocaleCompare("skewX",keyword) == 0)
          {
            GetToken(q,&q,token);
            angle=atof(token);
            affine.ry=tan(DegreesToRadians(fmod(angle,360.0)));
            break;
          }
        if (LocaleCompare("skewY",keyword) == 0)
          {
            GetToken(q,&q,token);
            angle=atof(token);
            affine.rx=tan(DegreesToRadians(fmod(angle,360.0)));
            break;
          }
        if (LocaleCompare("stop-color",keyword) == 0)
          {
            GetToken(q,&q,token);
            break;
          }
        if (LocaleCompare("stroke",keyword) == 0)
          {
            GetToken(q,&q,token);
            (void) QueryColorDatabase(token,&graphic_context[n]->stroke);
            if (graphic_context[n]->stroke.opacity != TransparentOpacity)
              graphic_context[n]->stroke.opacity=graphic_context[n]->opacity;
            break;
          }
        if (LocaleCompare("stroke-antialias",keyword) == 0)
          {
            GetToken(q,&q,token);
            graphic_context[n]->stroke_antialias=atoi(token);
            break;
          }
        if (LocaleCompare("stroke-dasharray",keyword) == 0)
          {
            if (IsGeometry(q))
              {
                char
                  *p;

                p=q;
                GetToken(p,&p,token);
                for (x=0; IsGeometry(token); x++)
                  GetToken(p,&p,token);
                graphic_context[n]->dash_pattern=(unsigned int *)
                  AcquireMemory((x+1)*sizeof(unsigned int));
                if (graphic_context[n]->dash_pattern == (unsigned int *) NULL)
                  {
                    ThrowException(&image->exception,ResourceLimitWarning,
                      "Unable to draw image","Memory allocation failed");
                    break;
                  }
                for (j=0; j < x; j++)
                {
                  GetToken(q,&q,token);
                  graphic_context[n]->dash_pattern[j]=atoi(token);
                }
                graphic_context[n]->dash_pattern[j]=0;
                break;
              }
            GetToken(q,&q,token);
            if (LocaleCompare(token,"#000000ff") != 0)
              break;
            if (graphic_context[n]->dash_pattern != (unsigned int *) NULL)
              LiberateMemory((void **) &graphic_context[n]->dash_pattern);
            graphic_context[n]->dash_pattern=(unsigned int *) NULL;
            break;
          }
        if (LocaleCompare("stroke-dashoffset",keyword) == 0)
          {
            GetToken(q,&q,token);
            graphic_context[n]->dash_offset=atol(token);
            break;
          }
        if (LocaleCompare("stroke-linecap",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("butt",token) == 0)
              {
                graphic_context[n]->linecap=ButtCap;
                break;
              }
            if (LocaleCompare("round",token) == 0)
              {
                graphic_context[n]->linecap=RoundCap;
                break;
              }
            if (LocaleCompare("square",token) == 0)
              {
                graphic_context[n]->linecap=SquareCap;
                break;
              }
            status=False;
            break;
          }
        if (LocaleCompare("stroke-linejoin",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("butt",token) == 0)
              {
                graphic_context[n]->linejoin=MiterJoin;
                break;
              }
            if (LocaleCompare("miter",token) == 0)
              {
                graphic_context[n]->linejoin=MiterJoin;
                break;
              }
            if (LocaleCompare("round",token) == 0)
              {
                graphic_context[n]->linejoin=RoundJoin;
                break;
              }
            if (LocaleCompare("square",token) == 0)
              {
                graphic_context[n]->linejoin=BevelJoin;
                break;
              }
            status=False;
            break;
          }
        if (LocaleCompare("stroke-miterlimit",keyword) == 0)
          {
            GetToken(q,&q,token);
            graphic_context[n]->miterlimit=atol(token);
            break;
          }
        if (LocaleCompare("stroke-opacity",keyword) == 0)
          {
            GetToken(q,&q,token);
            factor=strchr(token,'%') != (char *) NULL ? 0.01 : 1.0;
            graphic_context[n]->stroke.opacity=(Quantum)
              (MaxRGB*(1.0*factor*atof(token)));
            break;
          }
        if (LocaleCompare("stroke-width",keyword) == 0)
          {
            GetToken(q,&q,token);
            graphic_context[n]->stroke_width=atof(token);
            break;
          }
        status=False;
        break;
      }
      case 't':
      case 'T':
      {
        if (LocaleCompare("text",keyword) == 0)
          {
            primitive_type=TextPrimitive;
            break;
          }
        if (LocaleCompare("text-antialias",keyword) == 0)
          {
            GetToken(q,&q,token);
            graphic_context[n]->text_antialias=atoi(token);
            break;
          }
        if (LocaleCompare("translate",keyword) == 0)
          {
            GetToken(q,&q,token);
            affine.tx=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.ty=atof(token);
            break;
          }
        status=False;
        break;
      }
      case 'v':
      case 'V':
      {
        if (LocaleCompare("viewbox",keyword) == 0)
          {
            GetToken(q,&q,token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            break;
          }
        status=False;
        break;
      }
      default:
      {
        status=False;
        break;
      }
    }
    if (status == False)
      break;
    if ((affine.sx != 1.0) || (affine.rx != 0.0) || (affine.ry != 0.0) ||
        (affine.sy != 1.0) || (affine.tx != 0.0) || (affine.ty != 0.0))
      {
        graphic_context[n]->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
        graphic_context[n]->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
        graphic_context[n]->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
        graphic_context[n]->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
        graphic_context[n]->affine.tx=
          current.sx*affine.tx+current.ry*affine.ty+current.tx;
        graphic_context[n]->affine.ty=
          current.rx*affine.tx+current.sy*affine.ty+current.ty;
      }
    if (primitive_type == UndefinedPrimitive)
      {
        if (graphic_context[n]->debug)
          (void) fprintf(stdout,"  %.*s\n",(int) (q-p),p);
        continue;
      }
    /*
      Parse the primitive attributes.
    */
    i=0;
    j=0;
    for (x=0; *q != '\0'; x++)
    {
      /*
        Define points.
      */
      if (!IsGeometry(q))
        break;
      GetToken(q,&q,token);
      point.x=atof(token);
      GetToken(q,&q,token);
      if (*token == ',')
        GetToken(q,&q,token);
      point.y=atof(token);
      GetToken(q,(char **) NULL,token);
      if (*token == ',')
        GetToken(q,&q,token);
      primitive_info[i].primitive=primitive_type;
      primitive_info[i].point=point;
      primitive_info[i].coordinates=0;
      primitive_info[i].method=FloodfillMethod;
      i++;
      if (i < (long) (number_points-6*BezierQuantum-360))
        continue;
      number_points+=6*BezierQuantum+360;
      ReacquireMemory((void **) &primitive_info,
        number_points*sizeof(PrimitiveInfo));
      if (primitive_info == (PrimitiveInfo *) NULL)
        {
          ThrowException(&image->exception,ResourceLimitWarning,
            "Unable to draw image","Memory allocation failed");
          break;
        }
    }
    primitive_info[j].primitive=primitive_type;
    primitive_info[j].coordinates=x;
    primitive_info[j].method=FloodfillMethod;
    primitive_info[j].text=(char *) NULL;
    switch (primitive_type)
    {
      case PointPrimitive:
      default:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=False;
            break;
          }
        TracePoint(primitive_info+j,primitive_info[j].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case LinePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        TraceLine(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case RectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        TraceRectangle(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case RoundRectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            status=False;
            break;
          }
        TraceRoundRectangle(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point,primitive_info[j+2].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case ArcPrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            status=False;
            break;
          }
        TraceArc(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point,primitive_info[j+2].point,0,True,False);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case EllipsePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            status=False;
            break;
          }
        TraceEllipse(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point,primitive_info[j+2].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case CirclePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        TraceCircle(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case PolylinePrimitive:
      {
        if (primitive_info[j].coordinates < 2)
          {
            status=False;
            break;
          }
        break;
      }
      case PolygonPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            status=False;
            break;
          }
        primitive_info[i]=primitive_info[j];
        primitive_info[i].coordinates=0;
        primitive_info[j].coordinates++;
        i++;
        break;
      }
      case BezierPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            status=False;
            break;
          }
        TraceBezier(primitive_info+j,primitive_info[j].coordinates);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case PathPrimitive:
      {
        register char
          *p;

        unsigned long
          number_attributes;

        GetToken(q,&q,token);
        number_attributes=1;
        for (p=token; *p != '\0'; p++)
          if (isalpha((int) *p))
            number_attributes++;
        if (i > (long) (number_points-6*BezierQuantum*number_attributes-1))
          {
            number_points+=6*BezierQuantum*number_attributes;
            ReacquireMemory((void **) &primitive_info,
              number_points*sizeof(PrimitiveInfo));
            if (primitive_info == (PrimitiveInfo *) NULL)
              {
                ThrowException(&image->exception,ResourceLimitWarning,
                  "Unable to draw path","Memory allocation failed");
                break;
              }
          }
        i=j+TracePath(primitive_info+j,token);
        break;
      }
      case ColorPrimitive:
      case MattePrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=False;
            break;
          }
        GetToken(q,&q,token);
        if (LocaleCompare("point",token) == 0)
          primitive_info[j].method=PointMethod;
        if (LocaleCompare("replace",token) == 0)
          primitive_info[j].method=ReplaceMethod;
        if (LocaleCompare("floodfill",token) == 0)
          primitive_info[j].method=FloodfillMethod;
        if (LocaleCompare("filltoborder",token) == 0)
          primitive_info[j].method=FillToBorderMethod;
        if (LocaleCompare("reset",token) == 0)
          primitive_info[j].method=ResetMethod;
        break;
      }
      case TextPrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=False;
            break;
          }
        GetToken(q,&q,token);
        primitive_info[j].text=AllocateString(token);
        break;
      }
      case ImagePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        GetToken(q,&q,token);
        primitive_info[j].text=AllocateString(token);
        break;
      }
    }
    if (primitive_info == (PrimitiveInfo *) NULL)
      break;
    if (graphic_context[n]->debug)
      (void) fprintf(stdout,"  %.*s\n",(int) (q-p),p);
    if (status == False)
      break;
    primitive_info[i].primitive=UndefinedPrimitive;
    /*
      Transform points.
    */
    for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
    {
      point=primitive_info[i].point;
      primitive_info[i].point.x=graphic_context[n]->affine.sx*point.x+
        graphic_context[n]->affine.ry*point.y+graphic_context[n]->affine.tx;
      primitive_info[i].point.y=graphic_context[n]->affine.rx*point.x+
        graphic_context[n]->affine.sy*point.y+graphic_context[n]->affine.ty;
      point=primitive_info[i].point;
      if (point.x < graphic_context[n]->bounds.x1)
        graphic_context[n]->bounds.x1=point.x;
      if (point.y < graphic_context[n]->bounds.y1)
        graphic_context[n]->bounds.y1=point.y;
      if (point.x > graphic_context[n]->bounds.x2)
        graphic_context[n]->bounds.x2=point.x;
      if (point.y > graphic_context[n]->bounds.y2)
        graphic_context[n]->bounds.y2=point.y;
    }
    if (graphic_context[n]->clip_path != (char *) NULL)
      if (graphic_context[n]->clip_units == ObjectBoundingBox)
        (void) DrawClipPath(image,graphic_context[n]);
    (void) DrawPrimitive(image,graphic_context[n],primitive_info);
    if (primitive_info->text != (char *) NULL)
      LiberateMemory((void **) &primitive_info->text);
  }
  if (graphic_context[n]->debug)
    (void) fprintf(stdout,"end draw-image (%.2fu)\n",GetUserTime(&timer));
  /*
    Free resources.
  */
  LiberateMemory((void **) &token);
  if (primitive_info != (PrimitiveInfo *) NULL)
    LiberateMemory((void **) &primitive_info);
  LiberateMemory((void **) &primitive);
  for ( ; n >= 0; n--)
    DestroyDrawInfo(graphic_context[n]);
  LiberateMemory((void **) &graphic_context);
  if (status == False)
    ThrowBinaryException(OptionWarning,
      "Non-conforming drawing primitive definition",keyword);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D r a w P o l y g o n P r i m i t i v e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawPolygonPrimitive draws a polygon on the image.
%
%  The format of the DrawPolygonPrimitive method is:
%
%      unsigned int DrawPolygonPrimitive(Image *image,const DrawInfo *draw_info,
%        const PrimitiveInfo *primitive_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%
*/

static inline double GetPixelOpacity(PolygonInfo *polygon_info,const double mid,
  const unsigned int fill,const FillRule fill_rule,const long x,const long y,
  double *stroke_opacity)
{
  double
    alpha,
    beta,
    distance,
    subpath_opacity;

  int
    winding_number;

  register double
    dx,
    dy;

  register EdgeInfo
    *p;

  register const PointInfo
    *q;

  register long
    i;

  long
    j;

  /*
    Compute fill & stroke opacity for this (x,y) point.
  */
  *stroke_opacity=0.0;
  subpath_opacity=0.0;
  p=polygon_info->edges;
  for (j=0; j < polygon_info->number_edges; j++)
  {
    if (y <= (p->bounds.y1-mid-0.5))
      break;
    if (y > (p->bounds.y2+mid+0.5))
      {
        (void) DestroyEdge(polygon_info,j);
        p++;
        continue;
      }
    if (x <= (p->bounds.x1-mid-0.5))
      {
        p++;
        continue;
      }
    if (x > (p->bounds.x2+mid+0.5))
      {
        p++;
        continue;
      }
    for (i=Max(p->highwater,1); i < p->number_points; i++)
    {
      if (y <= (p->points[i-1].y-mid-0.5))
        break;
      if (y > (p->points[i].y+mid+0.5))
        continue;
      if (p->scanline != y)
        {
          p->scanline=y;
          p->highwater=i;
        }
      /*
        Compute distance between a point and an edge.
      */
      q=p->points+i-1;
      dx=(q+1)->x-q->x,
      dy=(q+1)->y-q->y;
      beta=dx*(x-q->x)+dy*(y-q->y);
      if (beta < 0.0)
        {
          dx=x-q->x;
          dy=y-q->y;
          distance=dx*dx+dy*dy;
        }
      else
        {
          alpha=dx*dx+dy*dy;
          if (beta > alpha)
            {
              dx=x-(q+1)->x;
              dy=y-(q+1)->y;
              distance=dx*dx+dy*dy;
            }
          else
            {
              alpha=1.0/alpha;
              beta=dx*(y-q->y)-dy*(x-q->x);
              distance=alpha*beta*beta;
            }
        }
      /*
        Compute stroke & subpath opacity.
      */
      beta=0.0;
      if (!p->ghostline)
        {
          alpha=mid+0.5;
          if ((*stroke_opacity < 1.0) && (distance <= (alpha*alpha)))
            {
              alpha=mid-0.5;
              if (distance <= (alpha*alpha))
                *stroke_opacity=1.0;
              else
                {
                  beta=1.0;
                  if (distance != 1.0)
                    beta=sqrt(distance);
                  alpha=beta-mid-0.5;
                  if (*stroke_opacity < (alpha*alpha))
                    *stroke_opacity=alpha*alpha;
                }
            }
        }
      if (!fill || (distance > 1.0) || (subpath_opacity >= 1.0))
        continue;
      if (distance <= 0.0)
        {
          subpath_opacity=1.0;
          continue;
        }
      if (distance > 1.0)
        continue;
      if (beta == 0.0)
        {
          beta=1.0;
          if (distance != 1.0)
            beta=sqrt(distance);
        }
      alpha=beta-1.0;
      if (subpath_opacity < (alpha*alpha))
        subpath_opacity=alpha*alpha;
    }
    p++;
  }
  /*
    Compute fill opacity.
  */
  if (!fill)
    return(0.0);
  if (subpath_opacity >= 1.0)
    return(1.0);
  /*
    Determine winding number.
  */
  winding_number=0;
  p=polygon_info->edges;
  for (j=0; j < polygon_info->number_edges; j++)
  {
    if (y <= p->bounds.y1)
      break;
    if (y > p->bounds.y2)
      {
        p++;
        continue;
      }
    if (x <= p->bounds.x1)
      {
        p++;
        continue;
      }
    if (x > p->bounds.x2)
      {
        winding_number+=p->direction ? 1 : -1;
        p++;
        continue;
      }
    for (i=Max(p->highwater,1); i < p->number_points; i++)
      if (y <= p->points[i].y)
        break;
    q=p->points+i-1;
    dx=(q+1)->x-q->x;
    dy=(q+1)->y-q->y;
    if ((dx*(y-q->y)) <= (dy*(x-q->x)))
      winding_number+=p->direction ? 1 : -1;
    p++;
  }
  if (fill_rule != NonZeroRule)
    {
      if (AbsoluteValue(winding_number) & 0x01)
        return(1.0);
    }
  else
    if (AbsoluteValue(winding_number) > 0)
      return(1.0);
  return(subpath_opacity);
}

static unsigned int DrawPolygonPrimitive(Image *image,const DrawInfo *draw_info,
  const PrimitiveInfo *primitive_info)
{
  double
    fill_opacity,
    mid,
    stroke_opacity;

  long
    y;

  PathInfo
    *path_info;

  PixelPacket
    fill_color,
    stroke_color;

  PolygonInfo
    *polygon_info;

  register EdgeInfo
    *p;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  SegmentInfo
    bounds;

  TimerInfo
    timer;

  unsigned int
    fill;

  /*
    Compute bounding box.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  assert(primitive_info != (PrimitiveInfo *) NULL);
  if (primitive_info->coordinates == 0)
    return(True);
  path_info=ConvertPrimitiveToPath(draw_info,primitive_info);
  if (path_info == (PathInfo *) NULL)
    return(False);
  polygon_info=ConvertPathToPolygon(draw_info,path_info);
  LiberateMemory((void **) &path_info);
  if (polygon_info == (PolygonInfo *) NULL)
    return(False);
  if (draw_info->debug > 1)
    DrawBoundingRectangles(image,draw_info,polygon_info);
  if (draw_info->debug)
    {
      GetTimerInfo(&timer);
      (void) fprintf(stdout,"    begin draw-polygon\n");
    }
  fill=(primitive_info->method == FillToBorderMethod) ||
    (primitive_info->method == FloodfillMethod);
  fill_color=draw_info->fill;
  mid=ExpandAffine(&draw_info->affine)*draw_info->stroke_width/2.0;
  stroke_color=draw_info->stroke;
  bounds=polygon_info->edges[0].bounds;
  for (i=1; i < polygon_info->number_edges; i++)
  {
    p=polygon_info->edges+i;
    if (p->bounds.x1 < bounds.x1)
      bounds.x1=p->bounds.x1;
    if (p->bounds.y1 < bounds.y1)
      bounds.y1=p->bounds.y1;
    if (p->bounds.x2 > bounds.x2)
      bounds.x2=p->bounds.x2;
    if (p->bounds.y2 > bounds.y2)
      bounds.y2=p->bounds.y2;
  }
  bounds.x1-=(mid+1.0);
  bounds.x1=bounds.x1 < 0.0 ? 0.0 : bounds.x1 >= image->columns ?
    image->columns-1 : bounds.x1;
  bounds.y1-=(mid+1.0);
  bounds.y1=bounds.y1 < 0.0 ? 0.0 : bounds.y1 >= image->rows ?
    image->rows-1 : bounds.y1;
  bounds.x2+=(mid+1.0);
  bounds.x2=bounds.x2 < 0.0 ? 0.0 : bounds.x2 >= image->columns ?
    image->columns-1 : bounds.x2;
  bounds.y2+=(mid+1.0);
  bounds.y2=bounds.y2 < 0.0 ? 0.0 : bounds.y2 >= image->rows ?
    image->rows-1 : bounds.y2;
  if (primitive_info->coordinates == 1)
    {
      /*
        Draw point.
      */
      for (y=(int) ceil(bounds.y1-0.5); y <= (int) floor(bounds.y2-0.5); y++)
      {
        x=(int) ceil(bounds.x1-0.5);
        q=GetImagePixels(image,x,y,(int) floor(bounds.x2-0.5)-x+1,1);
        if (q == (PixelPacket *) NULL)
          break;
        for ( ; x <= (int) floor(bounds.x2-0.5); x++)
        {
          if ((x == (int) ceil(primitive_info->point.x-0.5)) &&
              (y == (int) ceil(primitive_info->point.y-0.5)))
            *q=stroke_color;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      if (draw_info->debug)
        (void) fprintf(stdout,"    end draw-polygon (%.2fu)\n",
          GetUserTime(&timer));
      return(True);
    }
  /*
    Draw polygon or line.
  */
  for (y=(int) ceil(bounds.y1-0.5); y <= (int) floor(bounds.y2-0.5); y++)
  {
    x=(int) ceil(bounds.x1-0.5);
    q=GetImagePixels(image,x,y,(int) floor(bounds.x2-0.5)-x+1,1);
    if (q == (PixelPacket *) NULL)
      break;
    for ( ; x <= (int) floor(bounds.x2-0.5); x++)
    {
      /*
        Fill and/or stroke.
      */
      fill_opacity=GetPixelOpacity(polygon_info,mid,fill,draw_info->fill_rule,
        x,y,&stroke_opacity);
      if (draw_info->tile != (Image *) NULL)
        fill_color=GetOnePixel(draw_info->tile,x % draw_info->tile->columns,
          y % draw_info->tile->rows);
      if (!draw_info->stroke_antialias)
        {
          fill_opacity=fill_opacity > 0.50 ? 1.0 : 0.0;
          stroke_opacity=stroke_opacity > 0.50 ? 1.0 : 0.0;
        }
      fill_opacity=MaxRGB-fill_opacity*(MaxRGB-fill_color.opacity);
      AlphaComposite(&fill_color,fill_opacity,q,q->opacity);
      stroke_opacity=MaxRGB-stroke_opacity*(MaxRGB-stroke_color.opacity);
      AlphaComposite(&stroke_color,stroke_opacity,q,q->opacity);
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
  if (draw_info->debug)
    (void) fprintf(stdout,"    end draw-polygon (%.2fu)\n",GetUserTime(&timer));
  DestroyPolygonInfo(polygon_info);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D r a w P r i m i t i v e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawPrimitive draws a primitive (line, rectangle, ellipse) on the
%  image.
%
%  The format of the DrawPrimitive method is:
%
%      void DrawPrimitive(Image *image,const DrawInfo *draw_info,
%        PrimitiveInfo *primitive_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%
*/

static void PrintPrimitiveInfo(const PrimitiveInfo *primitive_info)
{
  char
    *methods[] =
    {
      (char *) "point",
      (char *) "replace",
      (char *) "floodfill",
      (char *) "filltoborder",
      (char *) "reset",
      (char *) "?"
    };

  long
    coordinates,
    y;

  PointInfo
    p,
    q,
    point;

  register long
    i,
    x;

  x=(int) ceil(primitive_info->point.x-0.5);
  y=(int) ceil(primitive_info->point.y-0.5);
  switch (primitive_info->primitive)
  {
    case PointPrimitive:
    {
      (void) fprintf(stdout,"PointPrimitive %ld,%ld %s\n",x,y,
        methods[primitive_info->method]);
      return;
    }
    case ColorPrimitive:
    {
      (void) fprintf(stdout,"ColorPrimitive %ld,%ld %s\n",x,y,
        methods[primitive_info->method]);
      return;
    }
    case MattePrimitive:
    {
      (void) fprintf(stdout,"MattePrimitive %ld,%ld %s\n",x,y,
        methods[primitive_info->method]);
      return;
    }
    case TextPrimitive:
    {
      (void) fprintf(stdout,"TextPrimitive %ld,%ld\n",x,y);
      return;
    }
    case ImagePrimitive:
    {
      (void) fprintf(stdout,"ImagePrimitive %ld,%ld\n",x,y);
      return;
    }
    default:
      break;
  }
  coordinates=0;
  q.x=(-1.0);
  q.y=(-1.0);
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
  {
    point=primitive_info[i].point;
    if (coordinates <= 0)
      {
        coordinates=primitive_info[i].coordinates;
        (void) fprintf(stdout,"    begin open (%ld)\n",coordinates);
        p=point;
      }
    point=primitive_info[i].point;
    if ((fabs(q.x-point.x) > MagickEpsilon) ||
        (fabs(q.y-point.y) > MagickEpsilon))
      (void) fprintf(stdout,"      %ld: %g,%g\n",coordinates,point.x,point.y);
    else
      (void) fprintf(stdout,"      %ld: %g,%g (duplicate)\n",coordinates,
        point.x,point.y);
    q=point;
    coordinates--;
    if (coordinates > 0)
      continue;
    if ((fabs(p.x-point.x) > MagickEpsilon) ||
        (fabs(p.y-point.y) > MagickEpsilon))
      (void) fprintf(stdout,"    end last (%ld)\n",coordinates);
    else
      (void) fprintf(stdout,"    end open (%ld)\n",coordinates);
  }
}

static unsigned int DrawPrimitive(Image *image,const DrawInfo *draw_info,
  const PrimitiveInfo *primitive_info)
{
  long
    y;

  register long
    i,
    x;

  register PixelPacket
    *q;

  TimerInfo
    timer;

  unsigned int
    status;

  if (draw_info->debug)
    {
      GetTimerInfo(&timer);
      (void) fprintf(stdout,"  begin draw-primitive\n");
    }
  status=True;
  x=(int) ceil(primitive_info->point.x-0.5);
  y=(int) ceil(primitive_info->point.y-0.5);
  switch (primitive_info->primitive)
  {
    case PointPrimitive:
    {
      q=GetImagePixels(image,x,y,1,1);
      if (q == (PixelPacket *) NULL)
        break;
      *q=draw_info->fill;
      (void) SyncImagePixels(image);
      break;
    }
    case ColorPrimitive:
    {
      switch (primitive_info->method)
      {
        case PointMethod:
        default:
        {
          q=GetImagePixels(image,x,y,1,1);
          if (q == (PixelPacket *) NULL)
            break;
          *q=draw_info->fill;
          (void) SyncImagePixels(image);
          break;
        }
        case ReplaceMethod:
        {
          PixelPacket
            color,
            target;

          color=draw_info->fill;
          target=GetOnePixel(image,x,y);
          for (y=0; y < (long) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
            {
              if (!ColorMatch(*q,target,image->fuzz))
                {
                  q++;
                  continue;
                }
              if (draw_info->tile != (Image *) NULL)
                {
                  color=GetOnePixel(draw_info->tile,
                    x % draw_info->tile->columns,y % draw_info->tile->rows);
                  if (!draw_info->tile->matte)
                    color.opacity=OpaqueOpacity;
                }
              AlphaComposite(&color,color.opacity,q,q->opacity);
              q++;
            }
            if (!SyncImagePixels(image))
              break;
          }
          break;
        }
        case FloodfillMethod:
        case FillToBorderMethod:
        {
          PixelPacket
            border_color,
            target;

          target=GetOnePixel(image,x,y);
          if (primitive_info->method == FillToBorderMethod)
            {
              border_color=draw_info->border_color;
              target=border_color;
            }
          (void) ColorFloodfillImage(image,draw_info,target,x,y,
            primitive_info->method);
          break;
        }
        case ResetMethod:
        {
          for (y=0; y < (long) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
            {
              *q=draw_info->fill;
              q++;
            }
            if (!SyncImagePixels(image))
              break;
          }
          break;
        }
      }
      break;
    }
    case MattePrimitive:
    {
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
      switch (primitive_info->method)
      {
        case PointMethod:
        default:
        {
          q=GetImagePixels(image,x,y,1,1);
          if (q == (PixelPacket *) NULL)
            break;
          q->opacity=TransparentOpacity;
          (void) SyncImagePixels(image);
          break;
        }
        case ReplaceMethod:
        {
          PixelPacket
            target;

          target=GetOnePixel(image,x,y);
          (void) TransparentImage(image,target,TransparentOpacity);
          break;
        }
        case FloodfillMethod:
        case FillToBorderMethod:
        {
          PixelPacket
            border_color,
            target;

          target=GetOnePixel(image,x,y);
          if (primitive_info->method == FillToBorderMethod)
            {
              border_color=draw_info->border_color;
              target=border_color;
            }
          (void) MatteFloodfillImage(image,target,TransparentOpacity,x,y,
            primitive_info->method);
          break;
        }
        case ResetMethod:
        {
          for (y=0; y < (long) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
            {
              q->opacity=draw_info->fill.opacity;
              q++;
            }
            if (!SyncImagePixels(image))
              break;
          }
          break;
        }
      }
      break;
    }
    case TextPrimitive:
    {
      char
        geometry[MaxTextExtent];

      DrawInfo
        *clone_info;

      if (primitive_info->text == (char *) NULL)
        break;
      clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
      (void) CloneString(&clone_info->text,primitive_info->text);
      FormatString(geometry,"%+g%+g",primitive_info->point.x,
        primitive_info->point.y);
      (void) CloneString(&clone_info->geometry,geometry);
      status=AnnotateImage(image,clone_info);
      DestroyDrawInfo(clone_info);
      break;
    }
    case ImagePrimitive:
    {
      ExceptionInfo
        exception;

      Image
        *composite_image;

      ImageInfo
        *clone_info;

      unsigned int
        matte;

      if (primitive_info->text == (char *) NULL)
        break;
      clone_info=CloneImageInfo((ImageInfo *) NULL);
      (void) strncpy(clone_info->filename,primitive_info->text,MaxTextExtent-1);
      composite_image=ReadImage(clone_info,&exception);
      if (exception.severity != UndefinedException)
        MagickWarning(exception.severity,exception.reason,
          exception.description);
      DestroyImageInfo(clone_info);
      if (composite_image == (Image *) NULL)
        break;
      if (draw_info->opacity != OpaqueOpacity)
        SetImageOpacity(composite_image,draw_info->opacity);
      if ((primitive_info[1].point.x != 0) && (primitive_info[1].point.y != 0))
        {
          char
            geometry[MaxTextExtent];

          /*
            Resize image.
          */
          FormatString(geometry,"%gx%g!",primitive_info[1].point.x,
            primitive_info[1].point.y);
          TransformImage(&composite_image,(char *) NULL,geometry);
        }
      if ((draw_info->affine.rx != 0.0) || (draw_info->affine.ry != 0.0))
        {
          if (((draw_info->affine.sx-draw_info->affine.sy) == 0.0) &&
              ((draw_info->affine.rx+draw_info->affine.ry) == 0.0))
            {
              double
                theta;

              Image
                *rotate_image;

              theta=(180.0/MagickPI)*
                atan2(draw_info->affine.rx,draw_info->affine.sx);
              rotate_image=RotateImage(composite_image,theta,&image->exception);
              if (rotate_image != (Image *) NULL)
                {
                  DestroyImage(composite_image);
                  composite_image=rotate_image;
                }
            }
        }
      matte=image->matte;
      (void) CompositeImage(image,composite_image->matte ? OverCompositeOp :
        draw_info->compose,composite_image,x,y);
      DestroyImage(composite_image);
      image->matte=matte;
      break;
    }
    default:
    {
      double
        mid,
        scale;

      DrawInfo
        *clone_info;

      SetImageType(image,TrueColorType);
      if (draw_info->debug)
        PrintPrimitiveInfo(primitive_info);
      scale=ExpandAffine(&draw_info->affine);
      if ((draw_info->dash_pattern != (unsigned *) NULL) &&
          (scale*draw_info->stroke_width > MagickEpsilon) &&
          (draw_info->stroke.opacity != TransparentOpacity))
        {
          /*
            Draw dash polygon.
          */
          clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
          clone_info->stroke_width=0.0;
          clone_info->stroke.opacity=TransparentOpacity;
          status=DrawPolygonPrimitive(image,clone_info,primitive_info);
          DestroyDrawInfo(clone_info);
          (void) DrawDashPolygon(draw_info,primitive_info,image);
          break;
        }
      mid=ExpandAffine(&draw_info->affine)*draw_info->stroke_width/2.0;
      if ((mid > 1.0) && (draw_info->stroke.opacity != TransparentOpacity))
        {
          unsigned int
            closed_path;

          /*
            Draw strokes while respecting line cap/join attributes.
          */
          for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++);
          closed_path=
            (primitive_info[i-1].point.x == primitive_info[0].point.x) &&
            (primitive_info[i-1].point.y == primitive_info[0].point.y);
          i=primitive_info[0].coordinates;
          if ((((draw_info->linecap == RoundCap) || closed_path) &&
               (draw_info->linejoin == RoundJoin)) ||
               (primitive_info[i].primitive != UndefinedPrimitive))
            {
              (void) DrawPolygonPrimitive(image,draw_info,primitive_info);
              break;
            }
          clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
          clone_info->stroke_width=0.0;
          clone_info->stroke.opacity=TransparentOpacity;
          status=DrawPolygonPrimitive(image,clone_info,primitive_info);
          DestroyDrawInfo(clone_info);
          status|=DrawStrokePolygon(image,draw_info,primitive_info);
          break;
        }
      (void) DrawPolygonPrimitive(image,draw_info,primitive_info);
      break;
    }
  }
  if (draw_info->debug)
    (void) fprintf(stdout,"  end draw-primitive (%.2fu)\n",GetUserTime(&timer));
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D r a w S t r o k e P o l y g o n                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawStrokePolygon draws a stroked polygon (line, rectangle, ellipse)
%  on the image while respecting the line cap and join attributes.
%
%  The format of the DrawStrokePolygon method is:
%
%      unsigned int DrawStrokePolygon(Image *image,const DrawInfo *draw_info,
%        const PrimitiveInfo *primitive_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%
*/

static void DrawRoundLinecap(Image *image,const DrawInfo *draw_info,
  const PrimitiveInfo *primitive_info)
{
  PrimitiveInfo
    linecap[5];

  register int
    i;

  for (i=0; i < 4; i++)
    linecap[i]=(*primitive_info);
  linecap[0].coordinates=4;
  linecap[1].point.x+=10.0*MagickEpsilon;
  linecap[2].point.x+=10.0*MagickEpsilon;
  linecap[2].point.y+=10.0*MagickEpsilon;
  linecap[3].point.y+=10.0*MagickEpsilon;
  linecap[4].primitive=UndefinedPrimitive;
  (void) DrawPolygonPrimitive(image,draw_info,linecap);
}

static unsigned int DrawStrokePolygon(Image *image,const DrawInfo *draw_info,
  const PrimitiveInfo *primitive_info)
{
  DrawInfo
    *clone_info;

  PrimitiveInfo
    *stroke_polygon;

  register const PrimitiveInfo
    *p,
    *q;

  TimerInfo
    timer;

  unsigned int
    closed_path,
    status;

  /*
    Draw stroked polygon.
  */
  if (draw_info->debug)
    {
      GetTimerInfo(&timer);
      (void) fprintf(stdout,"    begin draw-stroke-polygon\n");
    }
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  clone_info->fill=draw_info->stroke;
  clone_info->stroke.opacity=TransparentOpacity;
  clone_info->stroke_width=0.0;
  clone_info->fill_rule=NonZeroRule;
  status=True;
  for (p=primitive_info; p->primitive != UndefinedPrimitive; p+=p->coordinates)
  {
    stroke_polygon=TraceStrokePolygon(draw_info,p);
    status=DrawPolygonPrimitive(image,clone_info,stroke_polygon);
    LiberateMemory((void **) &stroke_polygon);
    q=p+p->coordinates-1;
    closed_path=(q->point.x == p->point.x) && (q->point.y == p->point.y);
    if ((draw_info->linecap == RoundCap) && !closed_path)
      {
        DrawRoundLinecap(image,clone_info,p);
        DrawRoundLinecap(image,clone_info,q);
      }
  }
  DestroyDrawInfo(clone_info);
  if (draw_info->debug)
    (void) fprintf(stdout,"    end draw-stroke-polygon (%.2fu)\n",
      GetUserTime(&timer));
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t D r a w I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetDrawInfo initializes the DrawInfo structure.
%
%  The format of the GetDrawInfo method is:
%
%      void GetDrawInfo(const ImageInfo *image_info,DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o draw_info: The draw info.
%
%
*/
MagickExport void GetDrawInfo(const ImageInfo *image_info,DrawInfo *draw_info)
{
  ImageInfo
    *clone_info;

  /*
    Initialize draw attributes.
  */
  assert(draw_info != (DrawInfo *) NULL);
  (void) memset(draw_info,0,sizeof(DrawInfo));
  clone_info=CloneImageInfo(image_info);
  IdentityAffine(&draw_info->affine);
  draw_info->gravity=NorthWestGravity;
  draw_info->opacity=OpaqueOpacity;
  draw_info->fill=clone_info->pen;
  (void) QueryColorDatabase("#000000ff",&draw_info->stroke);
  draw_info->stroke_antialias=clone_info->antialias;
  draw_info->stroke_width=1.0;
  draw_info->fill_rule=EvenOddRule;
  draw_info->linecap=ButtCap;
  draw_info->linejoin=MiterJoin;
  draw_info->miterlimit=10;
  draw_info->decorate=NoDecoration;
  if (clone_info->font != (char *) NULL)
    draw_info->font=AllocateString(clone_info->font);
  if (clone_info->density != (char *) NULL)
    draw_info->density=AllocateString(clone_info->density);
  draw_info->text_antialias=clone_info->antialias;
  draw_info->pointsize=clone_info->pointsize;
  (void) QueryColorDatabase("#000000ff",&draw_info->box);
  draw_info->border_color=clone_info->border_color;
  draw_info->compose=CopyCompositeOp;
  if (clone_info->server_name != (char *) NULL)
    draw_info->server_name=AllocateString(clone_info->server_name);
  draw_info->debug=clone_info->debug;
  draw_info->signature=MagickSignature;
  DestroyImageInfo(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a t t e F l o o d f i l l I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MatteFloodfill() changes the transparency value of any pixel that
%  matches target and is an immediate neighbor.  If the method is
%  FillToBorderMethod is specified, the transparency value is changed for
%  any neighbor pixel that does not match the bordercolor member of
%  image.
%
%  By default target must match a particular pixel transparency exactly.
%  However, in many cases two transparency values may differ by a small
%  amount.  The fuzz member of image defines how much tolerance is
%  acceptable to consider two transparency values as the same.  For
%  example, set fuzz to 10 and the opacity values of 100 and 102
%  respectively are now interpreted as the same value for the purposes of
%  the floodfill.
%
%  The format of the MatteFloodfillImage method is:
%
%      unsigned int MatteFloodfillImage(Image *image,const PixelPacket target,
%        const unsigned int opacity,const long x_offset,const long y_offset,
%        const PaintMethod method)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o target: The RGB value of the target color.
%
%    o opacity: The level of transparency: 0 is fully opaque and MaxRGB is
%      fully transparent.
%
%    o x,y: The starting location of the operation.
%
%    o method:  Choose either FloodfillMethod or FillToBorderMethod.
%
%
*/
MagickExport unsigned int MatteFloodfillImage(Image *image,
  const PixelPacket target,const unsigned int opacity,const long x_offset,
  const long y_offset,const PaintMethod method)
{
  int
    offset,
    skip;

  long
    start,
    x1,
    x2,
    y;

  register long
    x;

  register PixelPacket
    *q;

  register SegmentInfo
    *s;

  SegmentInfo
    *segment_stack;

  /*
    Check boundary conditions.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((x_offset < 0) || (x_offset >= (long) image->columns))
    return(False);
  if ((y_offset < 0) || (y_offset >= (long) image->rows))
    return(False);
  if (target.opacity == opacity)
    return(False);
  q=GetImagePixels(image,x_offset,y_offset,1,1);
  if (q == (PixelPacket *) NULL)
    return(False);
  if (q->opacity == opacity)
    return(False);
  /*
    Allocate segment stack.
  */
  segment_stack=(SegmentInfo *) AcquireMemory(MaxStacksize*sizeof(SegmentInfo));
  if (segment_stack == (SegmentInfo *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Unable to floodfill image",
      image->filename);
  /*
    Push initial segment on stack.
  */
  SetImageType(image,TrueColorMatteType);
  x=x_offset;
  y=y_offset;
  start=0;
  s=segment_stack;
  Push(y,x,x,1);
  Push(y+1,x,x,-1);
  while (s > segment_stack)
  {
    /*
      Pop segment off stack.
    */
    s--;
    x1=(int) s->x1;
    x2=(int) s->x2;
    offset=(int) s->y2;
    y=(int) s->y1+offset;
    /*
      Recolor neighboring points.
    */
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    q+=x1;
    for (x=x1; x >= 0; x--)
    {
      if (method == FloodfillMethod)
        {
          if (!MatteMatch(*q,target,image->fuzz))
            break;
        }
      else
        if (MatteMatch(*q,target,image->fuzz) || (q->opacity == opacity))
          break;
      q->opacity=opacity;
      q--;
    }
    if (!SyncImagePixels(image))
      break;
    skip=x >= x1;
    if (!skip)
      {
        start=x+1;
        if (start < x1)
          Push(y,start,x1-1,-offset);
        x=x1+1;
      }
    do
    {
      if (!skip)
        {
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          q+=x;
          for ( ; x < (long) image->columns; x++)
          {
            if (method == FloodfillMethod)
              {
                if (!MatteMatch(*q,target,image->fuzz))
                  break;
              }
            else
              if (MatteMatch(*q,target,image->fuzz) || (q->opacity == opacity))
                break;
            q->opacity=opacity;
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          Push(y,start,x-1,offset);
          if (x > (x2+1))
            Push(y,x2+1,x-1,-offset);
        }
      skip=False;
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      q+=x;
      for (x++; x <= x2; x++)
      {
        q++;
        if (method == FloodfillMethod)
          {
            if (MatteMatch(*q,target,image->fuzz))
              break;
          }
        else
          if (!MatteMatch(*q,target,image->fuzz) && (q->opacity != opacity))
            break;
      }
      start=x;
    } while (x <= x2);
  }
  LiberateMemory((void **) &segment_stack);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     O p a q u e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  OpaqueImage() changes any pixel that matches color with the color
%  defined by fill.
%
%  By default color must match a particular pixel color exactly.  However,
%  in many cases two colors may differ by a small amount.  Fuzz defines
%  how much tolerance is acceptable to consider two colors as the same.
%  For example, set fuzz to 10 and the color red at intensities of 100 and
%  102 respectively are now interpreted as the same color.
%
%  The format of the OpaqueImage method is:
%
%      unsigned int OpaqueImage(Image *image,const PixelPacket target,
%        const PixelPacket fill)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o target: The RGB value of the target color.
%
%    o fill: The replacement color.
%
%
*/
MagickExport unsigned int OpaqueImage(Image *image,const PixelPacket target,
  const PixelPacket fill)
{
#define OpaqueImageText  "  Setting opaque color in the image...  "

  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  /*
    Make image color opaque.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      /*
        Make DirectClass image opaque.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          if (ColorMatch(*q,target,image->fuzz))
            *q=fill;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          MagickMonitor(OpaqueImageText,y,image->rows);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Make PseudoClass image opaque.
      */
      for (i=0; i < (long) image->colors; i++)
      {
        if (ColorMatch(image->colormap[i],target,image->fuzz))
          image->colormap[i]=fill;
        if (QuantumTick(i,image->colors))
          MagickMonitor(OpaqueImageText,i,image->colors);
      }
      SyncImage(image);
      break;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   T r a c e P r i m i t i v e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TracePrimitive is a collection of methods for generating graphic
%  primitives such as arcs, ellipses, paths, etc.
%
%
*/

static void TraceArc(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end,const PointInfo arc,const double angle,
  const unsigned int large_arc,const unsigned int sweep)
{
  double
    alpha,
    beta,
    factor,
    gamma,
    theta;

  PointInfo
    center,
    points[3];

  register double
    cosine,
    sine;

  register PrimitiveInfo
    *p;

  register long
    i;

  unsigned long
    arc_segments;

  primitive_info->coordinates=0;
  if ((arc.x == 0.0) || (arc.y == 0.0))
    return;
  cosine=cos(DegreesToRadians(angle));
  sine=sin(DegreesToRadians(angle));
  points[0].x=cosine*start.x/arc.x+sine*start.y/arc.x;
  points[0].y=cosine*start.y/arc.y-sine*start.x/arc.y;
  points[1].x=cosine*end.x/arc.x+sine*end.y/arc.x;
  points[1].y=cosine*end.y/arc.y-sine*end.x/arc.y;
  alpha=points[1].x-points[0].x;
  beta=points[1].y-points[0].y;
  factor=1.0/(alpha*alpha+beta*beta)-0.25;
  if (factor <= 0.0)
    factor=0.0;
  else
    {
      factor=sqrt(factor);
      if (sweep == large_arc)
        factor=(-factor);
    }
  center.x=0.5*(points[0].x+points[1].x)-factor*beta;
  center.y=0.5*(points[0].y+points[1].y)+factor*alpha;
  alpha=atan2(points[0].y-center.y,points[0].x-center.x);
  theta=atan2(points[1].y-center.y,points[1].x-center.x)-alpha;
  if ((theta < 0.0) && sweep)
    theta+=2.0*MagickPI;
  else
    if ((theta > 0.0) && !sweep)
      theta-=2.0*MagickPI;
  arc_segments=(int) ceil(fabs(theta/(0.5*MagickPI+MagickEpsilon)));
  p=primitive_info;
  for (i=0; i < (long) arc_segments; i++)
  {
    beta=0.5*((alpha+(i+1)*theta/arc_segments)-
      (alpha+i*theta/arc_segments));
    gamma=(8.0/3.0)*sin(0.5*beta)*sin(0.5*beta)/sin(beta);
    points[0].x=center.x+cos((alpha+i*theta/arc_segments))-
      gamma*sin((alpha+i*theta/arc_segments));
    points[0].y=center.y+sin((alpha+i*theta/arc_segments))+
      gamma*cos((alpha+i*theta/arc_segments));
    points[2].x=center.x+cos((alpha+(i+1)*theta/arc_segments));
    points[2].y=center.y+sin((alpha+(i+1)*theta/arc_segments));
    points[1].x=points[2].x+gamma*sin((alpha+(i+1)*theta/arc_segments));
    points[1].y=points[2].y-gamma*cos((alpha+(i+1)*theta/arc_segments));
    p->point.x=(p == primitive_info) ? start.x : (p-1)->point.x;
    p->point.y=(p == primitive_info) ? start.y : (p-1)->point.y;
    (p+1)->point.x=cosine*arc.x*points[0].x-sine*arc.y*points[0].y;
    (p+1)->point.y=sine*arc.x*points[0].x+cosine*arc.y*points[0].y;
    (p+2)->point.x=cosine*arc.x*points[1].x-sine*arc.y*points[1].y;
    (p+2)->point.y=sine*arc.x*points[1].x+cosine*arc.y*points[1].y;
    (p+3)->point.x=cosine*arc.x*points[2].x-sine*arc.y*points[2].y;
    (p+3)->point.y=sine*arc.x*points[2].x+cosine*arc.y*points[2].y;
    if (i == (long) (arc_segments-1))
      (p+3)->point=end;
    TraceBezier(p,4);
    p+=p->coordinates;
  }
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < (long) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void TraceBezier(PrimitiveInfo *primitive_info,
  const unsigned long number_coordinates)
{
  double
    alpha,
    *coefficients,
    weight;

  PointInfo
    end,
    point,
    *points;

  register PrimitiveInfo
    *p;

  register long
    i,
    j;

  unsigned long
    control_points,
    quantum;

  /*
    Allocate coeficients.
  */
  quantum=number_coordinates;
  for (i=0; i < (long) number_coordinates; i++)
  {
    for (j=i+1; j < (long) number_coordinates; j++)
    {
      alpha=fabs(primitive_info[j].point.x-primitive_info[i].point.x);
      if (alpha > quantum)
        quantum=(unsigned int) alpha;
      alpha=fabs(primitive_info[j].point.y-primitive_info[i].point.y);
      if (alpha > quantum)
        quantum=(unsigned int) alpha;
    }
  }
  quantum=Min(quantum/number_coordinates,BezierQuantum);
  control_points=quantum*number_coordinates;
  coefficients=(double *) AcquireMemory(number_coordinates*sizeof(double));
  points=(PointInfo *) AcquireMemory(control_points*sizeof(PointInfo));
  if ((coefficients == (double *) NULL) || (points == (PointInfo *) NULL))
    MagickError(ResourceLimitWarning,"Unable to draw image",
      "Memory allocation failed");
  /*
    Compute bezier points.
  */
  end=primitive_info[number_coordinates-1].point;
  weight=0.0;
  for (i=0; i < (long) number_coordinates; i++)
    coefficients[i]=Permutate(number_coordinates-1,i);
  for (i=0; i < (long) control_points; i++)
  {
    p=primitive_info;
    point.x=0;
    point.y=0;
    alpha=pow(1.0-weight,number_coordinates-1);
    for (j=0; j < (long) number_coordinates; j++)
    {
      point.x+=alpha*coefficients[j]*p->point.x;
      point.y+=alpha*coefficients[j]*p->point.y;
      alpha*=weight/(1.0-weight);
      p++;
    }
    points[i]=point;
    weight+=1.0/quantum/number_coordinates;
  }
  /*
    Bezier curves are just short segmented polys.
  */
  p=primitive_info;
  for (i=0; i < (long) control_points; i++)
  {
    TracePoint(p,points[i]);
    p++;
  }
  TracePoint(p,end);
  primitive_info->coordinates=p-primitive_info+1;
  for (i=0; i < (long) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
  LiberateMemory((void **) &points);
  LiberateMemory((void **) &coefficients);
}

static void TraceCircle(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end)
{
  double
    alpha,
    beta,
    radius;

  PointInfo
    offset,
    degrees;

  alpha=end.x-start.x;
  beta=end.y-start.y;
  radius=sqrt(alpha*alpha+beta*beta);
  offset.x=radius;
  offset.y=radius;
  degrees.x=0.0;
  degrees.y=360.0;
  TraceEllipse(primitive_info,start,offset,degrees);
}

static void TraceEllipse(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end,const PointInfo degrees)
{
  double
    angle;

  PointInfo
    point;

  register PrimitiveInfo
    *p;

  register long
    i;

  /*
    Ellipses are just short segmented polys.
  */
  p=primitive_info;
  for (angle=degrees.x; angle <= degrees.y; angle+=1.0)
  {
    point.x=cos(DegreesToRadians(fmod(angle,360.0)))*end.x+start.x;
    point.y=sin(DegreesToRadians(fmod(angle,360.0)))*end.y+start.y;
    TracePoint(p,point);
    p++;
  }
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < (long) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void TraceLine(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end)
{
  TracePoint(primitive_info,start);
  TracePoint(primitive_info+1,end);
  if ((start.x == end.x) && (start.y == end.y))
    {
      PointInfo
        colinear;

      colinear.x=end.x+MagickEpsilon;
      colinear.y=end.y+MagickEpsilon;
      (void) TracePoint(primitive_info+1,colinear);
    }
  (primitive_info+1)->primitive=primitive_info->primitive;
  primitive_info->coordinates=2;
}

static unsigned long TracePath(PrimitiveInfo *primitive_info,const char *path)
{
  char
    *p,
    token[MaxTextExtent];

  double
    x,
    y;

  int
    attribute;

  PointInfo
    end,
    points[4],
    point,
    start;

  PrimitiveType
    primitive_type;

  register PrimitiveInfo
    *q;

  register long
    i;

  unsigned long
    number_coordinates,
    z_count;

  point.x=0;
  point.y=0;
  number_coordinates=0;
  z_count=0;
  primitive_type=primitive_info->primitive;
  q=primitive_info;
  for (p=(char *) path; *p != '\0'; )
  {
    while (isspace((int) *p))
      p++;
    if (*p == '\0')
      break;
    attribute=(*p++);
    switch (attribute)
    {
      case 'a':
      case 'A':
      {
        double
          angle;

        PointInfo
          arc;

        unsigned int
          large_arc,
          sweep;

        /*
          Compute arc points.
        */
        GetToken(p,&p,token);
        if (*token == ',')
          GetToken(p,&p,token);
        arc.x=atof(token);
        GetToken(p,&p,token);
        if (*token == ',')
          GetToken(p,&p,token);
        arc.y=atof(token);
        GetToken(p,&p,token);
        if (*token == ',')
          GetToken(p,&p,token);
        angle=atof(token);
        GetToken(p,&p,token);
        if (*token == ',')
          GetToken(p,&p,token);
        large_arc=atoi(token);
        GetToken(p,&p,token);
        if (*token == ',')
          GetToken(p,&p,token);
        sweep=atoi(token);
        GetToken(p,&p,token);
        if (*token == ',')
          GetToken(p,&p,token);
        x=atof(token);
        GetToken(p,&p,token);
        if (*token == ',')
          GetToken(p,&p,token);
        y=atof(token);
        end.x=attribute == 'A' ? x : point.x+x;
        end.y=attribute == 'A' ? y : point.y+y;
        TraceArc(q,point,end,arc,angle,large_arc,sweep);
        q+=q->coordinates;
        point=end;
        break;
      }
      case 'c':
      case 'C':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=point;
          for (i=1; i <= 3; i++)
          {
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            x=atof(token);
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            y=atof(token);
            end.x=attribute == 'C' ? x : point.x+x;
            end.y=attribute == 'C' ? y : point.y+y;
            points[i]=end;
          }
          for (i=0; i <= 3; i++)
            (q+i)->point=points[i];
          TraceBezier(q,4);
          q+=q->coordinates;
          point=end;
        } while (IsGeometry(p));
        break;
      }
      case 'H':
      case 'h':
      {
        do
        {
          GetToken(p,&p,token);
          if (*token == ',')
            GetToken(p,&p,token);
          x=atof(token);
          point.x=attribute == 'H' ? x: point.x+x;
          TracePoint(q,point);
          q+=q->coordinates;
        } while (IsGeometry(p));
        break;
      }
      case 'l':
      case 'L':
      {
        do
        {
          GetToken(p,&p,token);
          if (*token == ',')
            GetToken(p,&p,token);
          x=atof(token);
          GetToken(p,&p,token);
          if (*token == ',')
            GetToken(p,&p,token);
          y=atof(token);
          point.x=attribute == 'L' ? x : point.x+x;
          point.y=attribute == 'L' ? y : point.y+y;
          TracePoint(q,point);
          q+=q->coordinates;
        } while (IsGeometry(p));
        break;
      }
      case 'M':
      case 'm':
      {
        if (q != primitive_info)
          {
            primitive_info->coordinates=q-primitive_info;
            number_coordinates+=primitive_info->coordinates;
            primitive_info=q;
          }
        do
        {
          GetToken(p,&p,token);
          if (*token == ',')
            GetToken(p,&p,token);
          x=atof(token);
          GetToken(p,&p,token);
          if (*token == ',')
            GetToken(p,&p,token);
          y=atof(token);
          point.x=attribute == 'M' ? x : point.x+x;
          point.y=attribute == 'M' ? y : point.y+y;
          TracePoint(q,point);
          q+=q->coordinates;
        } while (IsGeometry(p));
        start=point;
        break;
      }
      case 'q':
      case 'Q':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=point;
          for (i=1; i < 3; i++)
          {
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            x=atof(token);
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            y=atof(token);
            if (*p == ',')
              p++;
            end.x=attribute == 'Q' ? x : point.x+x;
            end.y=attribute == 'Q' ? y : point.y+y;
            points[i]=end;
          }
          for (i=0; i < 3; i++)
            (q+i)->point=points[i];
          TraceBezier(q,3);
          q+=q->coordinates;
          point=end;
        } while (IsGeometry(p));
        break;
      }
      case 's':
      case 'S':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=points[3];
          points[1].x=2.0*points[3].x-points[2].x;
          points[1].y=2.0*points[3].y-points[2].y;
          for (i=2; i <= 3; i++)
          {
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            x=atof(token);
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            y=atof(token);
            if (*p == ',')
              p++;
            end.x=attribute == 'S' ? x : point.x+x;
            end.y=attribute == 'S' ? y : point.y+y;
            points[i]=end;
          }
          for (i=0; i <= 4; i++)
            (q+i)->point=points[i];
          TraceBezier(q,4);
          q+=q->coordinates;
          point=end;
        } while (IsGeometry(p));
        break;
      }
      case 't':
      case 'T':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=points[2];
          points[1].x=2.0*points[2].x-points[1].x;
          points[1].y=2.0*points[2].y-points[1].y;
          for (i=2; i < 3; i++)
          {
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            x=atof(token);
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            y=atof(token);
            end.x=attribute == 'T' ? x : point.x+x;
            end.y=attribute == 'T' ? y : point.y+y;
            points[i]=end;
          }
          for (i=0; i < 3; i++)
            (q+i)->point=points[i];
          TraceBezier(q,3);
          q+=q->coordinates;
          point=end;
        } while (IsGeometry(p));
        break;
      }
      case 'v':
      case 'V':
      {
        do
        {
          GetToken(p,&p,token);
          if (*token == ',')
            GetToken(p,&p,token);
          y=atof(token);
          point.y=attribute == 'V' ? y : point.y+y;
          TracePoint(q,point);
          q+=q->coordinates;
        } while (IsGeometry(p));
        break;
      }
      case 'z':
      case 'Z':
      {
        point=start;
        TracePoint(q,point);
        q+=q->coordinates;
        primitive_info->coordinates=q-primitive_info;
        number_coordinates+=primitive_info->coordinates;
        primitive_info=q;
        z_count++;
        break;
      }
      default:
      {
        if (isalpha((int) attribute))
          (void) fprintf(stderr,"attribute not recognized: %c\n",attribute);
        break;
      }
    }
  }
  primitive_info->coordinates=q-primitive_info;
  number_coordinates+=primitive_info->coordinates;
  for (i=0; i < (long) number_coordinates; i++)
  {
    q--;
    q->primitive=primitive_type;
    if (z_count > 1)
      q->method=FillToBorderMethod;
  }
  q=primitive_info;
  return(number_coordinates);
}

static void TracePoint(PrimitiveInfo *primitive_info,const PointInfo point)
{
  primitive_info->coordinates=1;
  primitive_info->point=point;
}

static void TraceRectangle(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end)
{
  PointInfo
    point;

  register PrimitiveInfo
    *p;

  register long
    i;

  p=primitive_info;
  TracePoint(p,start);
  p+=p->coordinates;
  point.x=start.x;
  point.y=end.y;
  TracePoint(p,point);
  p+=p->coordinates;
  TracePoint(p,end);
  p+=p->coordinates;
  point.x=end.x;
  point.y=start.y;
  TracePoint(p,point);
  p+=p->coordinates;
  TracePoint(p,start);
  p+=p->coordinates;
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < (long) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void TraceRoundRectangle(PrimitiveInfo *primitive_info,
  const PointInfo start,const PointInfo end,PointInfo arc)
{
  PointInfo
    degrees,
    offset,
    point;

  register PrimitiveInfo
    *p;

  register long
    i;

  p=primitive_info;
  offset.x=end.x-start.x;
  offset.y=end.y-start.y;
  if (arc.x > (0.5*offset.x))
    arc.x=0.5*offset.x;
  if (arc.y > (0.5*offset.y))
    arc.y=0.5*offset.y;
  point.x=start.x+offset.x-arc.x;
  point.y=start.y+arc.y;
  degrees.x=270.0;
  degrees.y=360.0;
  TraceEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  point.x=start.x+offset.x-arc.x;
  point.y=start.y+offset.y-arc.y;
  degrees.x=0.0;
  degrees.y=90.0;
  TraceEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  point.x=start.x+arc.x;
  point.y=start.y+offset.y-arc.y;
  degrees.x=90.0;
  degrees.y=180.0;
  TraceEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  point.x=start.x+arc.x;
  point.y=start.y+arc.y;
  degrees.x=180.0;
  degrees.y=270.0;
  TraceEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  TracePoint(p,primitive_info->point);
  p+=p->coordinates;
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < (long) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void TraceSquareLinecap(PrimitiveInfo *primitive_info,
  const unsigned long number_vertices,const double offset)
{
  double
    distance;

  long
    j;

  register double
    dx,
    dy;

  register long
    i;

  dx=0.0;
  dy=0.0;
  for (i=1; i < (long) number_vertices; i++)
  {
    dx=primitive_info[0].point.x-primitive_info[i].point.x;
    dy=primitive_info[0].point.y-primitive_info[i].point.y;
    if ((fabs(dx) >= MagickEpsilon) || (fabs(dy) >= MagickEpsilon))
      break;
  }
  distance=sqrt(dx*dx+dy*dy+MagickEpsilon);
  primitive_info[0].point.x=primitive_info[i].point.x+
    dx*(distance+offset)/distance;
  primitive_info[0].point.y=primitive_info[i].point.y+
    dy*(distance+offset)/distance;
  for (j=number_vertices-2; j >= 0;  j--)
  {
    dx=primitive_info[number_vertices-1].point.x-primitive_info[j].point.x;
    dy=primitive_info[number_vertices-1].point.y-primitive_info[j].point.y;
    if ((fabs(dx) >= MagickEpsilon) || (fabs(dy) >= MagickEpsilon))
      break;
  }
  distance=sqrt(dx*dx+dy*dy+MagickEpsilon);
  primitive_info[number_vertices-1].point.x=primitive_info[j].point.x+
    dx*(distance+offset)/distance;
  primitive_info[number_vertices-1].point.y=primitive_info[j].point.y+
    dy*(distance+offset)/distance;
}

static PrimitiveInfo *TraceStrokePolygon(const DrawInfo *draw_info,
  const PrimitiveInfo *primitive_info)
{
  typedef struct _LineSegment
  {
    double
      p,
      q;
  } LineSegment;

  double
    delta_theta,
    dot_product,
    mid,
    miterlimit;
  LineSegment
    dx,
    dy,
    inverse_slope,
    slope,
    theta;

  PointInfo
    box_p[5],
    box_q[5],
    center,
    offset,
    *path_p,
    *path_q;

  PrimitiveInfo
    *polygon_primitive,
    *stroke_polygon;

  register long
    i;

  unsigned int
    closed_path;

  unsigned long
    arc_segments,
    j,
    n,
    max_strokes,
    number_vertices,
    p,
    q;

  /*
    Allocate paths.
  */
  number_vertices=primitive_info->coordinates;
  max_strokes=2*number_vertices+6*BezierQuantum+360;
  path_p=(PointInfo *) AcquireMemory(max_strokes*sizeof(PointInfo));
  path_q=(PointInfo *) AcquireMemory(max_strokes*sizeof(PointInfo));
  polygon_primitive=(PrimitiveInfo *)
    AcquireMemory((number_vertices+2)*sizeof(PrimitiveInfo));
  if ((path_p == (PointInfo *) NULL) || (path_q == (PointInfo *) NULL) ||
      (polygon_primitive == (PrimitiveInfo *) NULL))
    return((PrimitiveInfo *) NULL);
  (void) memcpy(polygon_primitive,primitive_info,number_vertices*
    sizeof(PrimitiveInfo));
  closed_path=
    (primitive_info[number_vertices-1].point.x == primitive_info[0].point.x) &&
    (primitive_info[number_vertices-1].point.y == primitive_info[0].point.y);
  if ((draw_info->linejoin == RoundJoin) ||
      ((draw_info->linejoin == MiterJoin) && closed_path))
    {
      polygon_primitive[number_vertices]=primitive_info[1];
      number_vertices++;
    }
  polygon_primitive[number_vertices].primitive=UndefinedPrimitive;
  /*
    Compute the slope for the first line segment, p.
  */
  for (n=1; n < number_vertices; n++)
  {
    dx.p=polygon_primitive[n].point.x-polygon_primitive[0].point.x;
    dy.p=polygon_primitive[n].point.y-polygon_primitive[0].point.y;
    if ((fabs(dx.p) >= MagickEpsilon) || (fabs(dy.p) >= MagickEpsilon))
      break;
  }
  slope.p=0.0;
  inverse_slope.p=0.0;
  if (fabs(dx.p) <= MagickEpsilon)
    {
      if (dx.p >= 0.0)
        slope.p=dy.p < 0.0 ? -1.0/MagickEpsilon : 1.0/MagickEpsilon;
      else
        slope.p=dy.p < 0.0 ? 1.0/MagickEpsilon : -1.0/MagickEpsilon;
    }
  else
    if (fabs(dy.p) <= MagickEpsilon)
      {
        if (dy.p >= 0.0)
          inverse_slope.p=dx.p < 0.0 ? -1.0/MagickEpsilon : 1.0/MagickEpsilon;
        else
          inverse_slope.p=dx.p < 0.0 ? 1.0/MagickEpsilon : -1.0/MagickEpsilon;
      }
    else
      {
        slope.p=dy.p/dx.p;
        inverse_slope.p=(-1.0/slope.p);
      }
  mid=ExpandAffine(&draw_info->affine)*draw_info->stroke_width/2.0;
  miterlimit=draw_info->miterlimit*draw_info->miterlimit*mid*mid;
  if ((draw_info->linecap == SquareCap) && !closed_path)
    TraceSquareLinecap(polygon_primitive,number_vertices,mid);
  offset.x=sqrt(mid*mid/(inverse_slope.p*inverse_slope.p+1.0));
  offset.y=offset.x*inverse_slope.p;
  if ((dy.p*offset.x-dx.p*offset.y) > 0.0)
    {
      box_p[0].x=polygon_primitive[0].point.x-offset.x;
      box_p[0].y=polygon_primitive[0].point.y-offset.x*inverse_slope.p;
      box_p[1].x=polygon_primitive[n].point.x-offset.x;
      box_p[1].y=polygon_primitive[n].point.y-offset.x*inverse_slope.p;
      box_q[0].x=polygon_primitive[0].point.x+offset.x;
      box_q[0].y=polygon_primitive[0].point.y+offset.x*inverse_slope.p;
      box_q[1].x=polygon_primitive[n].point.x+offset.x;
      box_q[1].y=polygon_primitive[n].point.y+offset.x*inverse_slope.p;
    }
  else
    {
      box_p[0].x=polygon_primitive[0].point.x+offset.x;
      box_p[0].y=polygon_primitive[0].point.y+offset.y;
      box_p[1].x=polygon_primitive[n].point.x+offset.x;
      box_p[1].y=polygon_primitive[n].point.y+offset.y;
      box_q[0].x=polygon_primitive[0].point.x-offset.x;
      box_q[0].y=polygon_primitive[0].point.y-offset.y;
      box_q[1].x=polygon_primitive[n].point.x-offset.x;
      box_q[1].y=polygon_primitive[n].point.y-offset.y;
    }
  /*
    Create strokes for the line join attribute: bevel, miter, round.
  */
  p=0;
  q=0;
  path_q[p++]=box_q[0];
  path_p[q++]=box_p[0];
  for (i=n+1; i < (long) number_vertices; i++)
  {
    /*
      Compute the slope for this line segment, q.
    */
    dx.q=polygon_primitive[i].point.x-polygon_primitive[n].point.x;
    dy.q=polygon_primitive[i].point.y-polygon_primitive[n].point.y;
    dot_product=dx.q*dx.q+dy.q*dy.q;
    if (dot_product < 0.25)
      continue;
    slope.q=0.0;
    inverse_slope.q=0.0;
    if (fabs(dx.q) < MagickEpsilon)
      {
        if (dx.q >= 0.0)
          slope.q=dy.q < 0.0 ? -1.0/MagickEpsilon : 1.0/MagickEpsilon;
        else
          slope.q=dy.q < 0.0 ? 1.0/MagickEpsilon : -1.0/MagickEpsilon;
      }
    else
      if (fabs(dy.q) <= MagickEpsilon)
        {
          if (dy.q >= 0.0)
            inverse_slope.q=dx.q < 0.0 ? -1.0/MagickEpsilon : 1.0/MagickEpsilon;
          else
            inverse_slope.q=dx.q < 0.0 ? 1.0/MagickEpsilon : -1.0/MagickEpsilon;
        }
      else
        {
          slope.q=dy.q/dx.q;
          inverse_slope.q=(-1.0/slope.q);
        }
    offset.x=sqrt(mid*mid/(inverse_slope.q*inverse_slope.q+1.0));
    offset.y=offset.x*inverse_slope.q;
    dot_product=dy.q*offset.x-dx.q*offset.y;
    if (dot_product > 0.0)
      {
        box_p[2].x=polygon_primitive[n].point.x-offset.x;
        box_p[2].y=polygon_primitive[n].point.y-offset.y;
        box_p[3].x=polygon_primitive[i].point.x-offset.x;
        box_p[3].y=polygon_primitive[i].point.y-offset.y;
        box_q[2].x=polygon_primitive[n].point.x+offset.x;
        box_q[2].y=polygon_primitive[n].point.y+offset.y;
        box_q[3].x=polygon_primitive[i].point.x+offset.x;
        box_q[3].y=polygon_primitive[i].point.y+offset.y;
      }
    else
      {
        box_p[2].x=polygon_primitive[n].point.x+offset.x;
        box_p[2].y=polygon_primitive[n].point.y+offset.y;
        box_p[3].x=polygon_primitive[i].point.x+offset.x;
        box_p[3].y=polygon_primitive[i].point.y+offset.y;
        box_q[2].x=polygon_primitive[n].point.x-offset.x;
        box_q[2].y=polygon_primitive[n].point.y-offset.y;
        box_q[3].x=polygon_primitive[i].point.x-offset.x;
        box_q[3].y=polygon_primitive[i].point.y-offset.y;
      }
    if (fabs(slope.p-slope.q) <= MagickEpsilon)
      {
        box_p[4]=box_p[1];
        box_q[4]=box_q[1];
      }
    else
      {
        box_p[4].x=(slope.p*box_p[0].x-box_p[0].y-slope.q*box_p[3].x+
          box_p[3].y)/(slope.p-slope.q);
        box_p[4].y=slope.p*(box_p[4].x-box_p[0].x)+box_p[0].y;
        box_q[4].x=(slope.p*box_q[0].x-box_q[0].y-slope.q*box_q[3].x+
          box_q[3].y)/(slope.p-slope.q);
        box_q[4].y=slope.p*(box_q[4].x-box_q[0].x)+box_q[0].y;
      }
    if (q >= (max_strokes-6*BezierQuantum-360))
      {
         max_strokes+=6*BezierQuantum+360;
         ReacquireMemory((void **) &path_p,max_strokes*sizeof(PointInfo));
         ReacquireMemory((void **) &path_q,max_strokes*sizeof(PointInfo));
         if ((path_p == (PointInfo *) NULL) || (path_q == (PointInfo *) NULL))
           {
             LiberateMemory((void **) &polygon_primitive);
             return((PrimitiveInfo *) NULL);
           }
      }
    dot_product=dx.q*dy.p-dx.p*dy.q;
    if (dot_product <= 0.0)
      switch (draw_info->linejoin)
      {
        case BevelJoin:
        {
          path_q[q++]=box_q[1];
          path_q[q++]=box_q[2];
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product <= miterlimit)
            path_p[p++]=box_p[4];
          else
            {
              path_p[p++]=box_p[1];
              path_p[p++]=box_p[2];
            }
          break;
        }
        case MiterJoin:
        {
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product > miterlimit)
            {
              path_q[q++]=box_q[1];
              path_q[q++]=box_q[2];
              path_p[p++]=box_p[1];
              path_p[p++]=box_p[2];
           }
         else
           {
             path_q[q++]=box_q[4];
             path_p[p++]=box_p[4];
           }
          break;
        }
        case RoundJoin:
        {
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product <= miterlimit)
            path_p[p++]=box_p[4];
          else
            {
              path_p[p++]=box_p[1];
              path_p[p++]=box_p[2];
            }
          center=polygon_primitive[n].point;
          theta.p=atan2(box_q[1].y-center.y,box_q[1].x-center.x);
          theta.q=atan2(box_q[2].y-center.y,box_q[2].x-center.x);
          if (theta.q < theta.p)
            theta.q+=2.0*MagickPI;
          arc_segments=(int) ceil((theta.q-theta.p)/(2.0*sqrt(1.0/mid)));
          path_q[q].x=box_q[1].x;
          path_q[q].y=box_q[1].y;
          q++;
          for (j=1; j < arc_segments; j++)
          {
            delta_theta=j*(theta.q-theta.p)/arc_segments;
            path_q[q].x=center.x+mid*cos(theta.p+delta_theta);
            path_q[q].y=center.y+mid*sin(theta.p+delta_theta);
            q++;
          }
          path_q[q++]=box_q[2];
          break;
        }
        default:
          break;
      }
    else
      switch (draw_info->linejoin)
      {
        case BevelJoin:
        {
          path_p[p++]=box_p[1];
          path_p[p++]=box_p[2];
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product <= miterlimit)
            path_q[q++]=box_q[4];
          else
            {
              path_q[q++]=box_q[1];
              path_q[q++]=box_q[2];
            }
          break;
        }
        case MiterJoin:
        {
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product <= miterlimit)
            {
              path_q[q++]=box_q[4];
              path_p[p++]=box_p[4];
            }
          else
            {
              path_q[q++]=box_q[1];
              path_q[q++]=box_q[2];
              path_p[p++]=box_p[1];
              path_p[p++]=box_p[2];
            }
          break;
        }
        case RoundJoin:
        {
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product <= miterlimit)
            path_q[q++]=box_q[4];
          else
            {
              path_q[q++]=box_q[1];
              path_q[q++]=box_q[2];
            }
          center=polygon_primitive[n].point;
          theta.p=atan2(box_p[1].y-center.y,box_p[1].x-center.x);
          theta.q=atan2(box_p[2].y-center.y,box_p[2].x-center.x);
          if (theta.p < theta.q)
            theta.p+=2.0*MagickPI;
          arc_segments=(int) ceil((theta.p-theta.q)/(2.0*sqrt(1.0/mid)));
          path_p[p++]=box_p[1];
          for (j=1; j < arc_segments; j++)
          {
            delta_theta=j*(theta.q-theta.p)/arc_segments;
            path_p[p].x=center.x+mid*cos(theta.p+delta_theta);
            path_p[p].y=center.y+mid*sin(theta.p+delta_theta);
            p++;
          }
          path_p[p++]=box_p[2];
          break;
        }
        default:
          break;
      }
    slope.p=slope.q;
    inverse_slope.p=inverse_slope.q;
    box_p[0]=box_p[2];
    box_p[1]=box_p[3];
    box_q[0]=box_q[2];
    box_q[1]=box_q[3];
    dx.p=dx.q;
    dy.p=dy.q;
    n=i;
  }
  path_p[p++]=box_p[1];
  path_q[q++]=box_q[1];
  /*
    Trace stroked polygon.
  */
  stroke_polygon=(PrimitiveInfo *)
    AcquireMemory((p+q+2*closed_path+2)*sizeof(PrimitiveInfo));
  if (stroke_polygon != (PrimitiveInfo *) NULL)
    {
      for (i=0; i < (long) p; i++)
      {
        stroke_polygon[i]=polygon_primitive[0];
        stroke_polygon[i].point=path_p[i];
      }
      if (closed_path)
        {
          stroke_polygon[i]=polygon_primitive[0];
          stroke_polygon[i].point=stroke_polygon[0].point;
          i++;
        }
      for ( ; i < (long) (p+q+closed_path); i++)
      {
        stroke_polygon[i]=polygon_primitive[0];
        stroke_polygon[i].point=path_q[p+q+closed_path-(i+1)];
      }
      if (closed_path)
        {
          stroke_polygon[i]=polygon_primitive[0];
          stroke_polygon[i].point=stroke_polygon[p+closed_path].point;
          i++;
        }
      stroke_polygon[i]=polygon_primitive[0];
      stroke_polygon[i].point=stroke_polygon[0].point;
      i++;
      stroke_polygon[i].primitive=UndefinedPrimitive;
      stroke_polygon[0].coordinates=p+q+2*closed_path+1;
    }
  LiberateMemory((void **) &path_p);
  LiberateMemory((void **) &path_q);
  LiberateMemory((void **) &polygon_primitive);
  return(stroke_polygon);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T r a n s p a r e n t I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TransparentImage() changes the opacity value associated with any pixel
%  that matches color to the value defined by opacity.
%
%  By default color must match a particular pixel color exactly.  However,
%  in many cases two colors may differ by a small amount.  Fuzz defines
%  how much tolerance is acceptable to consider two colors as the same.
%  For example, set fuzz to 10 and the color red at intensities of 100 and
%  102 respectively are now interpreted as the same color.
%
%  The format of the TransparentImage method is:
%
%      unsigned int TransparentImage(Image *image,const PixelPacket target,
%        const unsigned int opacity)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o target: The RGB value of the target color.
%
%
*/
MagickExport unsigned int TransparentImage(Image *image,
  const PixelPacket target,const unsigned int opacity)
{
#define TransparentImageText  "  Setting transparent color in the image...  "

  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Make image color transparent.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (!image->matte)
    SetImageOpacity(image,OpaqueOpacity);
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      if (ColorMatch(*q,target,image->fuzz))
        q->opacity=opacity;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(TransparentImageText,y,image->rows);
  }
  return(True);
}
