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
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
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
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

/*
  Define declarations.
*/
#define BezierQuantum  200
#define MatteMatch(color,target,delta) \
  (ColorMatch(color,target,delta) && ((color).opacity == (target).opacity))
#define MaxStacksize  (1 << 15)
#define Push(up,left,right,delta) \
  if ((s < (segment_stack+MaxStacksize)) && (((up)+(delta)) >= 0) && \
      (((up)+(delta)) < (int) image->rows)) \
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

  int
    number_points;

  PointInfo
    *points;

  int
    direction,
    visibility,
    highwater;

  double
    scanline;
} EdgeInfo;

typedef enum
{
  MoveToCode,
  OpenCode,
  HiddenCode,
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

typedef struct _PolygonInfo
{
  int
    number_edges;

  EdgeInfo
    *edges;
} PolygonInfo;

/*
  Forward declarations.
*/
static inline int
  GetWindingNumber(const PolygonInfo *,const double,const double);

static unsigned int
  TracePath(PrimitiveInfo *,const char *);

static void
  DrawPrimitive(const DrawInfo *,PrimitiveInfo *,Image *),
  ReversePoints(PointInfo *,const int),
  TraceArc(PrimitiveInfo *,const PointInfo,const PointInfo,const PointInfo,
    const double,const unsigned int,const unsigned int),
  TraceBezier(PrimitiveInfo *,const unsigned int),
  TraceCircle(PrimitiveInfo *,const PointInfo,const PointInfo),
  TraceEllipse(PrimitiveInfo *,const PointInfo,const PointInfo,const PointInfo),
  TraceLine(PrimitiveInfo *,const PointInfo,const PointInfo),
  TracePoint(PrimitiveInfo *,const PointInfo),
  TraceRectangle(PrimitiveInfo *,const PointInfo,const PointInfo),
  TraceRoundRectangle(PrimitiveInfo *,const PointInfo,const PointInfo,
    PointInfo);

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
%  Method CloneDrawInfo makes a duplicate of the given draw info, or if
%  draw info is NULL, a new one.
%
%  The format of the CloneDrawInfo method is:
%
%      DrawInfo *CloneDrawInfo(const ImageInfo *image_info,
%        const DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o cloned_info: Method CloneDrawInfo returns a duplicate of the given
%      annotate info, or if annotate info is NULL a new one.
%
%    o image_info: a structure of type ImageInfo.
%
%    o draw_info: a structure of type DrawInfo.
%
%
*/
MagickExport DrawInfo *CloneDrawInfo(const ImageInfo *image_info,
  const DrawInfo *draw_info)
{
  DrawInfo
    *cloned_info;

  cloned_info=(DrawInfo *) AcquireMemory(sizeof(DrawInfo));
  if (cloned_info == (DrawInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to clone draw info",
      "Memory allocation failed");
  if (draw_info == (DrawInfo *) NULL)
    {
      GetDrawInfo(image_info,cloned_info);
      return(cloned_info);
    }
  *cloned_info=(*draw_info);
  if (draw_info->primitive != (char *) NULL)
    cloned_info->primitive=AllocateString(draw_info->primitive);
  if (draw_info->font != (char *) NULL)
    cloned_info->font=AllocateString(draw_info->font);
  if (draw_info->tile != (Image *) NULL)
    cloned_info->tile=
      CloneImage(draw_info->tile,0,0,True,&draw_info->tile->exception);
  return(cloned_info);
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
%  Method ColorFloodfillImage floodfills the designated area with a color.
%  The floodfill algorithm is strongly based on a similar algorithm in
%  "Graphics Gems" by Paul Heckbert.
%
%  The format of the ColorFloodfillImage method is:
%
%      unsigned int ColorFloodfillImage(Image *image,const DrawInfo *draw_info,
%        const PixelPacket target,const int x_offset,const int y_offset,
%        const PaintMethod method)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o draw_info: a structure of type DrawInfo.
%
%    o target: A PixelPacket structure.  This is the RGB value of the target
%      color.
%
%    o x,y: Unsigned integers representing the current location of the pen.
%
%    o method: drawing method of type PrimitiveType: floodfill or fill to
%      border.
%
%
*/
MagickExport unsigned int ColorFloodfillImage(Image *image,
  const DrawInfo *draw_info,const PixelPacket target,const int x_offset,
  const int y_offset,const PaintMethod method)
{
  int
    offset,
    skip,
    start,
    x1,
    x2,
    y;

  PixelPacket
    color;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *p,
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
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  if ((x_offset < 0) || (x_offset >= (int) image->columns))
    return(False);
  if ((y_offset < 0) || (y_offset >= (int) image->rows))
    return(False);
  /*
    Set floodfill color.
  */
  if (ColorMatch(draw_info->fill,target,image->fuzz))
    return(False);
  segment_stack=(SegmentInfo *) AcquireMemory(MaxStacksize*sizeof(SegmentInfo));
  if (segment_stack == (SegmentInfo *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Unable to floodfill image",
      image->filename);
  /*
    Track "hot" pixels with the image index packets.
  */
  image->storage_class=PseudoClass;
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (int) image->columns; x++)
      indexes[x]=False;
    if (!SyncImagePixels(image))
      break;
  }
  /*
    Push initial segment on stack.
  */
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
    indexes=GetIndexes(image);
    for (x=x1; x >= 0 ; x--)
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
      indexes[x]=True;
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
          if (x < image->columns)
            {
              q=GetImagePixels(image,x,y,image->columns-x,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetIndexes(image);
              for (i=0; x < (int) image->columns; x++)
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
                indexes[i++]=True;
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
    for (y=0; y < (int) image->rows; y++)
    {
      /*
        Tile fill color onto floodplane.
      */
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=GetIndexes(image);
      for (x=0; x < (int) image->columns; x++)
      {
        if (indexes[x])
          *q=draw_info->fill;
        q++;
      }
      if (!SyncImagePixels(image))
        break;
    }
  else
    {
      double
        alpha;

      /*
        Tile image onto floodplane.
      */
      alpha=1.0/MaxRGB;
      for (y=0; y < (int) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (int) image->columns; x++)
        {
          if (indexes[x])
            {
              color=GetOnePixel(draw_info->tile,x % draw_info->tile->columns,
                y % draw_info->tile->rows);
              if (!draw_info->tile->matte)
                *q=color;
              else
                {
                  q->red=(Quantum) (alpha*(color.red*(MaxRGB-color.opacity)+
                    q->red*color.opacity));
                  q->green=(Quantum) (alpha*(color.green*(MaxRGB-color.opacity)+
                    q->green*color.opacity));
                  q->blue=(Quantum) (alpha*(color.blue*(MaxRGB-color.opacity)+
                    q->blue*color.opacity));
                  q->opacity=(Quantum) (alpha*(color.opacity*
                    (MaxRGB-color.opacity)+q->opacity*color.opacity));
                }
            }
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
    }
  image->storage_class=DirectClass;
  LiberateMemory((void **) &segment_stack);
  return(True);
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
%  Method DestroyDrawInfo deallocates memory associated with an
%  DrawInfo structure.
%
%  The format of the DestroyDrawInfo method is:
%
%      void DestroyDrawInfo(DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o draw_info: Specifies a pointer to an DrawInfo structure.
%
%
*/
MagickExport void DestroyDrawInfo(DrawInfo *draw_info)
{
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  if (draw_info->primitive != (char *) NULL)
    LiberateMemory((void **) &draw_info->primitive);
  if (draw_info->font != (char *) NULL)
    LiberateMemory((void **) &draw_info->font);
  if (draw_info->tile != (Image *) NULL)
    DestroyImage(draw_info->tile);
  if (draw_info->dash_pattern != (unsigned *) NULL)
    LiberateMemory((void **) &draw_info->dash_pattern);
  LiberateMemory((void **) &draw_info);
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
%  Method DrawImage draws a primitive (line, rectangle, ellipse) on the
%  image.
%
%  The format of the DrawImage method is:
%
%      unsigned int DrawImage(Image *image,const DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o draw_info: The address of a DrawInfo structure.
%
%
*/
MagickExport unsigned int DrawImage(Image *image,const DrawInfo *draw_info)
{
  AffineInfo
    affine,
    current;

  char
    keyword[MaxTextExtent],
    *primitive,
    *q,
    value[MaxTextExtent];

  double
    angle;

  DrawInfo
    **graphic_context;

  int
    j,
    k,
    n,
    number_points;

  PointInfo
    point;

  PrimitiveInfo
    *primitive_info;

  PrimitiveType
    primitive_type;

  register char
    *p;

  register int
    i,
    x;

  unsigned int
    indirection,
    length,
    status;

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
  n=0;
  graphic_context=(DrawInfo **) AcquireMemory(sizeof(DrawInfo *));
  if (graphic_context == (DrawInfo **) NULL)
    MagickError(ResourceLimitWarning,"Unable to draw image",
      "Memory allocation failed");
  graphic_context[n]=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  primitive=graphic_context[n]->primitive;
  indirection=(*primitive == '@');
  if (indirection)
    {
      FILE
        *file;

      int
        c;

      register char
        *q;

      /*
        Read text from a file.
      */
      file=(FILE *) fopen(primitive+1,"r");
      if (file == (FILE *) NULL)
        {
          for ( ; n >= 0; n--)
            DestroyDrawInfo(graphic_context[n]);
          LiberateMemory((void **) &graphic_context);
          ThrowBinaryException(FileOpenWarning,"Unable to read primitive file",
            primitive+1);
        }
      length=MaxTextExtent;
      primitive=AllocateString((char *) NULL);
      q=primitive;
      while (primitive != (char *) NULL)
      {
        c=fgetc(file);
        if (c == EOF)
          break;
        if ((q-primitive+1) >= (int) length)
          {
            *q='\0';
            length<<=1;
            ReacquireMemory((void **) &primitive,length);
            if (primitive == (char *) NULL)
              break;
            q=primitive+Extent(primitive);
          }
        *q++=c;
      }
      (void) fclose(file);
      if (primitive == (char *) NULL)
        {
          for ( ; n >= 0; n--)
            DestroyDrawInfo(graphic_context[n]);
          LiberateMemory((void **) &graphic_context);
          ThrowBinaryException(ResourceLimitWarning,"Unable to draw image",
            "Memory allocation failed");
        }
      *q='\0';
    }
  /*
    Allocate primitive info memory.
  */
  number_points=2047;
  primitive_info=(PrimitiveInfo *)
    AcquireMemory(number_points*sizeof(PrimitiveInfo));
  if (primitive_info == (PrimitiveInfo *) NULL)
    {
      if (indirection)
        LiberateMemory((void **) &primitive);
      for ( ; n >= 0; n--)
        DestroyDrawInfo(graphic_context[n]);
      LiberateMemory((void **) &graphic_context);
      ThrowBinaryException(ResourceLimitWarning,"Unable to draw image",
        "Memory allocation failed");
    }
  status=False;
  (void) SetImageAttribute(image,"[MVG]",primitive);
  if (graphic_context[n]->verbose)
    (void) fprintf(stdout,"begin vector-graphics\n");
  for (q=primitive; *q != '\0'; )
  {
    /*
      Define primitive.
    */
    while (isspace((int) (*q)) && (*q != '\0'))
      q++;
    p=q;
    for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
      keyword[x]=(*q++);
    keyword[x]='\0';
    if (*keyword == '\0')
      break;
    while (isspace((int) (*q)) && (*q != '\0'))
      q++;
    primitive_type=UndefinedPrimitive;
    current=graphic_context[n]->affine;
    affine.sx=1.0;
    affine.rx=0.0;
    affine.ry=0.0;
    affine.sy=1.0;
    affine.tx=0.0;
    affine.ty=0.0;
    switch (*keyword)
    {
      case ';':
        break;
      case 'a':
      case 'A':
      {
        if (LocaleCompare("affine",keyword) == 0)
          {
            affine.sx=strtod(q,&q);
            if (*q == ',')
              q++;
            affine.rx=strtod(q,&q);
            if (*q == ',')
              q++;
            affine.ry=strtod(q,&q);
            if (*q == ',')
              q++;
            affine.sy=strtod(q,&q);
            if (*q == ',')
              q++;
            affine.tx=strtod(q,&q);
            if (*q == ',')
              q++;
            affine.ty=strtod(q,&q);
            break;
          }
        if (LocaleCompare("angle",keyword) == 0)
          {
            graphic_context[n]->angle=strtod(q,&q);
            break;
          }
        if (LocaleCompare("arc",keyword) == 0)
          {
            primitive_type=ArcPrimitive;
            break;
          }
        status=True;
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
        status=True;
        break;
      }
      case 'c':
      case 'C':
      {
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
        status=True;
        break;
      }
      case 'd':
      case 'D':
      {
        if (LocaleCompare("decorate",keyword) == 0)
          {
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            if (LocaleCompare("none",value) == 0)
              graphic_context[n]->decorate=NoDecoration;
            if (LocaleCompare("underline",value) == 0)
              graphic_context[n]->decorate=UnderlineDecoration;
            if (LocaleCompare("overline",value) == 0)
              graphic_context[n]->decorate=OverlineDecoration;
            if (LocaleCompare("line-through",value) == 0)
              graphic_context[n]->decorate=LineThroughDecoration;
            break;
          }
        status=True;
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
        status=True;
        break;
      }
      case 'f':
      case 'F':
      {
        if (LocaleCompare("fill",keyword) == 0)
          {
            if ((LocaleNCompare(q,"rgb(",4) == 0))
              for (x=0; (*(q-1) != ')') && (*q != '\0'); x++)
                value[x]=(*q++);
            else
              for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
                value[x]=(*q++);
            value[x]='\0';
            (void) QueryColorDatabase(value,&graphic_context[n]->fill);
            break;
          }
        if (LocaleCompare("fill-opacity",keyword) == 0)
          {
            graphic_context[n]->fill.opacity=MaxRGB*strtod(q,&q);
            break;
          }
        if (LocaleCompare("font",keyword) == 0)
          {
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            CloneString(&graphic_context[n]->font,value);
            break;
          }
        status=True;
        break;
      }
      case 'g':
      case 'G':
      {
        if (LocaleCompare("gravity",keyword) == 0)
          {
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            if (LocaleCompare("NorthWest",value) == 0)
              graphic_context[n]->gravity=NorthWestGravity;
            if (LocaleCompare("North",value) == 0)
              graphic_context[n]->gravity=NorthGravity;
            if (LocaleCompare("NorthEast",value) == 0)
              graphic_context[n]->gravity=NorthEastGravity;
            if (LocaleCompare("West",value) == 0)
              graphic_context[n]->gravity=WestGravity;
            if (LocaleCompare("Center",value) == 0)
              graphic_context[n]->gravity=CenterGravity;
            if (LocaleCompare("East",value) == 0)
              graphic_context[n]->gravity=EastGravity;
            if (LocaleCompare("SouthWest",value) == 0)
              graphic_context[n]->gravity=SouthWestGravity;
            if (LocaleCompare("South",value) == 0)
              graphic_context[n]->gravity=SouthGravity;
            if (LocaleCompare("SouthEast",value) == 0)
              graphic_context[n]->gravity=SouthEastGravity;
            break;
          }
        status=True;
        break;
      }
      case 'i':
      case 'I':
      {
        if (LocaleCompare("image",keyword) == 0)
          {
            primitive_type=ImagePrimitive;
            break;
          }
        status=True;
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
        status=True;
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
        status=True;
        break;
      }
      case 'o':
      case 'O':
      {
        if (LocaleCompare("opacity",keyword) == 0)
          {
            graphic_context[n]->fill.opacity=MaxRGB*strtod(q,&q);
            graphic_context[n]->stroke.opacity=MaxRGB*strtod(q,&q);
            break;
          }
        status=True;
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
        if (LocaleCompare("pointsize",keyword) == 0)
          {
            graphic_context[n]->pointsize=strtod(q,&q);
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
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            DestroyDrawInfo(graphic_context[n]);
            n--;
            if (n < 0)
              ThrowBinaryException(CorruptImageWarning,
                "unbalanced graphic context push/pop",value);
            break;
          }
        if (LocaleCompare("push",keyword) == 0)
          {
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            n++;
            ReacquireMemory((void **) &graphic_context,
              (n+1)*sizeof(DrawInfo *));
            if (graphic_context == (DrawInfo **) NULL)
              MagickError(ResourceLimitWarning,"Unable to draw image",
                "Memory allocation failed");
            graphic_context[n]=
              CloneDrawInfo((ImageInfo *) NULL,graphic_context[n-1]);
            break;
          }
        status=True;
        break;
      }
      case 'r':
      case 'R':
      {
        if (LocaleNCompare("rect",keyword,4) == 0)
          {
            primitive_type=RectanglePrimitive;
            break;
          }
        if (LocaleCompare("roundRectangle",keyword) == 0)
          {
            primitive_type=RoundRectanglePrimitive;
            break;
          }
        if (LocaleCompare("rotate",keyword) == 0)
          {
            angle=strtod(q,&q);
            affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
            affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
            affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
            affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
            break;
          }
        status=True;
        break;
      }
      case 's':
      case 'S':
      {
        if (LocaleCompare("scale",keyword) == 0)
          {
            affine.sx=strtod(q,&q);
            if (*q == ',')
              q++;
            affine.sy=strtod(q,&q);
            break;
          }
        if (LocaleCompare("skewX",keyword) == 0)
          {
            angle=strtod(q,&q);
            affine.ry=tan(DegreesToRadians(fmod(angle,360.0)));
            break;
          }
        if (LocaleCompare("skewY",keyword) == 0)
          {
            angle=strtod(q,&q);
            affine.rx=tan(DegreesToRadians(fmod(angle,360.0)));
            break;
          }
        if (LocaleCompare("stroke",keyword) == 0)
          {
            if ((LocaleNCompare(q,"rgb(",4) == 0))
              for (x=0; (*(q-1) != ')') && (*q != '\0'); x++)
                value[x]=(*q++);
            else
              for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
                value[x]=(*q++);
            value[x]='\0';
            (void) QueryColorDatabase(value,&graphic_context[n]->stroke);
            break;
          }
        if (LocaleCompare("stroke-antialias",keyword) == 0)
          {
            graphic_context[n]->stroke_antialias=(unsigned int) strtod(q,&q);
            break;
          }
        if (LocaleCompare("stroke-dasharray",keyword) == 0)
          {
            if (IsGeometry(q))
              {
                graphic_context[n]->dash_pattern=(unsigned int *)
                  AcquireMemory(256*sizeof(unsigned int));
                if (graphic_context[n]->dash_pattern == (unsigned int *) NULL)
                  {
                    ThrowException(&image->exception,ResourceLimitWarning,
                      "Unable to draw image","Memory allocation failed");
                    break;
                  }
                for (x=0; IsGeometry(q) && (x < 255); x++)
                  graphic_context[n]->dash_pattern[x]=
                    (unsigned int) strtod(q,&q);
                graphic_context[n]->dash_pattern[x]=0;
                break;
              }
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            if (LocaleCompare(value,"none") != 0)
              break;
            if (graphic_context[n]->dash_pattern != (unsigned int *) NULL)
              LiberateMemory((void **) &graphic_context[n]->dash_pattern);
            graphic_context[n]->dash_pattern=(unsigned int *) NULL;
            break;
          }
        if (LocaleCompare("stroke-dashoffset",keyword) == 0)
          {
            graphic_context[n]->dash_offset=(unsigned int) strtod(q,&q);
            break;
          }
        if (LocaleCompare("stroke-linecap",keyword) == 0)
          {
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            if (LocaleCompare("butt",value) == 0)
              graphic_context[n]->linecap=ButtCap;
            if (LocaleCompare("round",value) == 0)
              graphic_context[n]->linecap=RoundCap;
            if (LocaleCompare("square",value) == 0)
              graphic_context[n]->linecap=SquareCap;
            break;
          }
        if (LocaleCompare("stroke-linejoin",keyword) == 0)
          {
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            if (LocaleCompare("butt",value) == 0)
              graphic_context[n]->linejoin=MiterJoin;
            if (LocaleCompare("round",value) == 0)
              graphic_context[n]->linejoin=RoundJoin;
            if (LocaleCompare("square",value) == 0)
              graphic_context[n]->linejoin=BevelJoin;
            break;
          }
        if (LocaleCompare("stroke-miterlimit",keyword) == 0)
          {
            graphic_context[n]->miterlimit=(unsigned int) strtod(q,&q);
            break;
          }
        if (LocaleCompare("stroke-opacity",keyword) == 0)
          {
            graphic_context[n]->stroke.opacity=MaxRGB*strtod(q,&q);
            break;
          }
        if (LocaleCompare("stroke-width",keyword) == 0)
          {
            graphic_context[n]->linewidth=strtod(q,&q);
            continue;
          }
        status=True;
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
            graphic_context[n]->text_antialias=(unsigned int) strtod(q,&q);
            break;
          }
        if (LocaleCompare("translate",keyword) == 0)
          {
            affine.tx=strtod(q,&q);
            if (*q == ',')
              q++;
            affine.ty=strtod(q,&q);
            break;
          }
        status=True;
        break;
      }
      default:
      {
        status=True;
        break;
      }
    }
    if (status)
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
        if (graphic_context[n]->verbose)
          (void) fprintf(stdout,"  %.*s\n",q-p,p);
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
      while (isspace((int) (*q)) && (*q != '\0'))
        q++;
      if (!IsGeometry(q))
        break;
      point.x=strtod(q,&q);
      if (*q == ',')
        q++;
      point.y=strtod(q,&q);
      if (*q == ',')
        q++;
      primitive_info[i].primitive=primitive_type;
      primitive_info[i].point=point;
      primitive_info[i].coordinates=0;
      primitive_info[i].method=FloodfillMethod;
      while ((isspace((int) (*q)) || (*q == ',')) && (*q != '\0'))
        q++;
      i++;
      if (i < (int) (number_points-6*BezierQuantum-360))
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
    while (isspace((int) (*q)) && (*q != '\0'))
      q++;
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
            status=True;
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
            status=True;
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
            status=True;
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
            status=True;
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
            status=True;
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
            status=True;
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
            status=True;
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
            status=True;
            break;
          }
        break;
      }
      case PolygonPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            status=True;
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
            status=True;
            break;
          }
        TraceBezier(primitive_info+j,primitive_info[j].coordinates);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case PathPrimitive:
      {
        char
          *path;

        int
          number_attributes;

        if (*q == '\0')
          break;
        number_attributes=1;
        p=q;
        if (*q == '"')
          {
            p++;
            for (q++; *q != '\0'; q++)
            {
              if (isalpha((int) *q))
                number_attributes++;
              if ((*q == '"') && (*(q-1) != '\\'))
                break;
            }
          }
        else
          if (*q == '\'')
            {
              p++;
              for (q++; *q != '\0'; q++)
              {
                if (isalpha((int) *q))
                  number_attributes++;
                if ((*q == '\'') && (*(q-1) != '\\'))
                  break;
              }
            }
          else
            for (q++;  *q != '\0'; q++)
            {
              if (isalpha((int) *q))
                number_attributes++;
              if (isspace((int) *q) && (*(q-1) != '\\') && (*q != '\0'))
                break;
            }
        path=(char *) AcquireMemory(q-p+1);
        if (i > (number_points-6*BezierQuantum*number_attributes-1))
          {
            number_points+=6*BezierQuantum*number_attributes;
            ReacquireMemory((void **) &primitive_info,
              number_points*sizeof(PrimitiveInfo));
          }
        if ((path == (char *) NULL) ||
            (primitive_info == (PrimitiveInfo *) NULL))
          {
            ThrowException(&image->exception,ResourceLimitWarning,
              "Unable to draw image","Memory allocation failed");
            break;
          }
        (void) strncpy(path,p,q-p+1);
        path[q-p]='\0';
        i=j+TracePath(primitive_info+j,path);
        LiberateMemory((void **) &path);
        break;
      }
      case ColorPrimitive:
      case MattePrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=True;
            break;
          }
        /*
          Define method.
        */
        for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
          keyword[x]=(*q++);
        keyword[x]='\0';
        if (*keyword == '\0')
          break;
        if (LocaleCompare("point",keyword) == 0)
          primitive_info[j].method=PointMethod;
        else
          if (LocaleCompare("replace",keyword) == 0)
            primitive_info[j].method=ReplaceMethod;
          else
            if (LocaleCompare("floodfill",keyword) == 0)
              primitive_info[j].method=FloodfillMethod;
            else
              if (LocaleCompare("filltoborder",keyword) == 0)
                primitive_info[j].method=FillToBorderMethod;
              else
                if (LocaleCompare("reset",keyword) == 0)
                  primitive_info[j].method=ResetMethod;
                else
                  status=True;
        break;
      }
      case TextPrimitive:
      {
        register char
          *p;

        if (primitive_info[j].coordinates != 1)
          {
            status=True;
            break;
          }
        if (*q == '\0')
          break;
        p=q;
        if (*q == '"')
          {
            p++;
            for (q++; *q != '\0'; q++)
              if ((*q == '"') && (*(q-1) != '\\'))
                break;
          }
        else
          if (*q == '\'')
            {
              p++;
              for (q++; *q != '\0'; q++)
                if ((*q == '\'') && (*(q-1) != '\\'))
                  break;
            }
          else
            for (q++;  *q != '\0'; q++)
              if (isspace((int) *q) && (*(q-1) != '\\') && (*q != '\0'))
                break;
        primitive_info[j].text=(char *) AcquireMemory(q-p+1);
        if (primitive_info[j].text != (char *) NULL)
          {
            (void) strncpy(primitive_info[j].text,p,q-p+1);
            primitive_info[j].text[q-p]='\0';
          }
        break;
      }
      case ImagePrimitive:
      {
        register char
          *p;

        if (primitive_info[j].coordinates != 2)
          {
            status=True;
            break;
          }
        if (*q == '\0')
          break;
        p=q;
        if (*p == '"')
          {
            p++;
            for (q++; *q != '\0'; q++)
              if ((*q == '"') && (*(q-1) != '\\'))
                break;
          }
        else
          if (*p == '\'')
            {
              p++;
              for (q++; *q != '\0'; q++)
                if ((*q == '\'') && (*(q-1) != '\\'))
                  break;
            }
          else
            for (q++;  *q != '\0'; q++)
              if (isspace((int) *q) && (*(q-1) != '\\') && (*q != '\0'))
                break;
        primitive_info[j].text=(char *) AcquireMemory(q-p+1);
        if (primitive_info[j].text != (char *) NULL)
          {
            (void) strncpy(primitive_info[j].text,p,q-p);
            primitive_info[j].text[q-p]='\0';
          }
        break;
      }
    }
    if (primitive_info == (PrimitiveInfo *) NULL)
      break;
    if (graphic_context[n]->verbose)
      (void) fprintf(stdout,"  %.*s\n",q-p,p);
    if (status)
      break;
    if ((*q == '"') || (*q == '\''))
      q++;
    while (isspace((int) (*q)) && (*q != '\0'))
      q++;
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
    }
    DrawPrimitive(graphic_context[n],primitive_info,image);
    if (primitive_info->text != (char *) NULL)
      LiberateMemory((void **) &primitive_info->text);
  }
  if (graphic_context[n]->verbose)
    (void) fprintf(stdout,"end vector-graphics\n");
  /*
    Free resources.
  */
  if (primitive_info != (PrimitiveInfo *) NULL)
    LiberateMemory((void **) &primitive_info);
  if (indirection)
    LiberateMemory((void **) &primitive);
  for ( ; n >= 0; n--)
    DestroyDrawInfo(graphic_context[n]);
  LiberateMemory((void **) &graphic_context);
  if (status)
    ThrowBinaryException(OptionWarning,
      "Non-conforming drawing primitive definition",keyword);
  image->storage_class=DirectClass;
  (void) IsMatteImage(image);
  return(True);
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
%  Bill Radcliffe contributed the polygon rendering code.  Parts of the fast
%  rendering algorithm was inspired by libart.
%
%  The format of the DrawPolygonPrimitive method is:
%
%      DrawPolygonPrimitive(const DrawInfo *draw_info,
%        const PrimitiveInfo *primitive_info,PolygonInfo *polygon_info,
%        Image *image)
%
%  A description of each parameter follows:
%
%    o draw_info: Specifies a pointer to a DrawInfo structure.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%    o polygon_info: Specifies a pointer to a PolygonInfo structure.
%
%    o image: The address of a structure of type Image.
%
%
*/

static int CompareEdges(const void *x,const void *y)
{
  const EdgeInfo
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

static PolygonInfo *ConvertPathToPolygon(const PathInfo *path_info)
{
  int
    direction,
    edge,
    visibility,
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
  polygon_info->edges=AcquireMemory(number_edges*sizeof(EdgeInfo));
  if (polygon_info->edges == (EdgeInfo *) NULL)
    return((PolygonInfo *) NULL);
  direction=0;
  edge=0;
  visibility=False;
  n=0;
  number_points=0;
  points=(PointInfo *) NULL;
  memset(&point,0,sizeof(PointInfo));
  memset(&bounds,0,sizeof(SegmentInfo));
  for (i=0; path_info[i].code != EndCode; i++)
  {
    if ((path_info[i].code == MoveToCode) || (path_info[i].code == OpenCode) ||
        (path_info[i].code == HiddenCode))
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
            polygon_info->edges[edge].visibility=visibility;
            polygon_info->edges[edge].direction=direction > 0;
            if (direction < 0)
              ReversePoints(points,n);
            polygon_info->edges[edge].points=points;
            polygon_info->edges[edge].bounds=bounds;
            polygon_info->edges[edge].bounds.y1=points[0].y;
            polygon_info->edges[edge].bounds.y2=points[n-1].y;
            points=(PointInfo *) NULL;
            visibility=False;
            edge++;
          }
        if (points == (PointInfo *) NULL)
          {
            number_points=16;
            points=(PointInfo *) AcquireMemory(number_points*sizeof(PointInfo));
            if (points == (PointInfo *) NULL)
              return((PolygonInfo *) NULL);
          }
        visibility=path_info[i].code == HiddenCode;
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
        polygon_info->edges[edge].visibility=visibility;
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
        visibility=False;
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
          polygon_info->edges[edge].visibility=visibility;
          polygon_info->edges[edge].direction=direction > 0;
          if (direction < 0)
            ReversePoints(points,n);
          polygon_info->edges[edge].points=points;
          polygon_info->edges[edge].bounds=bounds;
          polygon_info->edges[edge].bounds.y1=points[0].y;
          polygon_info->edges[edge].bounds.y2=points[n-1].y;
          visibility=False;
          edge++;
        }
    }
  polygon_info->number_edges=edge;
  qsort(polygon_info->edges,polygon_info->number_edges,sizeof(EdgeInfo),
    CompareEdges);
  return(polygon_info);
}

static PathInfo *ConvertPrimitiveToPath(const PrimitiveInfo *primitive_info)
{
  int
    code,
    coordinates,
    path_length,
    start;

  PathInfo
    *path_info;

  PointInfo
    first,
    last,
    point;

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
  coordinates=0;
  n=0;
  last.x=(-1.0);
  last.y=(-1.0);
  path_length=0;
  path_info=(PathInfo *) NULL;
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
  {
    point=primitive_info[i].point;
    code=LineToCode;
    if (coordinates <= 0)
      {
        coordinates=primitive_info[i].coordinates;
        path_length+=coordinates+3;
        if (path_info == (PathInfo *) NULL)
          path_info=(PathInfo *) AcquireMemory(path_length*sizeof(PathInfo));
        else
          ReacquireMemory((void **) &path_info,path_length*sizeof(PathInfo));
        if (path_info == (PathInfo *) NULL)
          return((PathInfo *) NULL);
        start=n;
        first=point;
        code=MoveToCode;
      }
    coordinates--;
    /*
      Reject duplicate points.
    */
    if ((fabs(last.x-point.x) > MagickEpsilon) ||
        (fabs(last.y-point.y) > MagickEpsilon))
      {
        path_info[n].code=code;
        path_info[n].point=point;
        last=point;
        n++;
      }
    if (coordinates > 0)
      continue;
    if ((fabs(first.x-point.x) <= MagickEpsilon) &&
        (fabs(first.y-point.y) <= MagickEpsilon))
      continue;
    /*
      Mark the first point as open if it does not match the last.
    */
    path_info[start].code=OpenCode;
    path_info[n].code=HiddenCode;
    path_info[n].point=point;
    n++;
    path_info[n].code=LineToCode;
    path_info[n].point=first;
    n++;
  }
  path_info[n].code=EndCode;
  path_info[n].point.x=0.0;
  path_info[n].point.y=0.0;
  return(path_info);
}

static int DestroyEdge(PolygonInfo *polygon_info,const int edge)
{
  register int
    i;

  if (edge >= polygon_info->number_edges)
    return(polygon_info->number_edges);
  LiberateMemory((void **) &polygon_info->edges[edge].points);
  for (i=edge; i < polygon_info->number_edges; i++)
    polygon_info->edges[i]=polygon_info->edges[i+1];
  polygon_info->number_edges--;
  return(polygon_info->number_edges);
}

static void DestroyPolygonInfo(PolygonInfo *polygon_info)
{
  register int
    i;

  for (i=0; i < polygon_info->number_edges; i++)
    LiberateMemory((void **) &polygon_info->edges[i].points);
  LiberateMemory((void **) &polygon_info->edges);
  LiberateMemory((void **) &polygon_info);
}

static inline double DistanceToLine(const PointInfo *p,const double x,
  const double y)
{
  double
    alpha,
    beta,
    dot_product;

  register const PointInfo
    *q;

  register double
    dx,
    dy;

  /*
    Determine distance between a point and a specific edge.
  */
  q=p+1;
  dx=q->x-p->x,
  dy=q->y-p->y;
  dot_product=dx*(x-p->x)+dy*(y-p->y);
  if (dot_product < 0.0)
    return((x-p->x)*(x-p->x)+(y-p->y)*(y-p->y));
  alpha=dx*dx+dy*dy;
  if (dot_product > alpha)
    return((x-q->x)*(x-q->x)+(y-q->y)*(y-q->y));
  beta=dx*(y-p->y)-dy*(x-p->x);
  return(beta*beta/alpha+MagickEpsilon);
}

static void DrawBoundingRectangles(Image *image,const PolygonInfo *polygon_info)
{
  double
    mid;

  DrawInfo
    *draw_info;

  int
    coordinates;

  PointInfo
    end,
    start;

  PrimitiveInfo
    primitive_info[6];

  register int
    i;

  SegmentInfo
    bounds;

  draw_info=CloneDrawInfo((const ImageInfo *) NULL,(const DrawInfo *) NULL);
  QueryColorDatabase("none",&draw_info->fill);
  mid=(image->x_resolution/72.0)*draw_info->linewidth/2.0;
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
      bounds.x1-=(mid-0.5);
      if (bounds.x1 < 0.0)
        bounds.x1=0.0;
      bounds.y1-=(mid-0.5);
      if (bounds.y1 < 0.0)
        bounds.y1=0.0;
      bounds.x2+=(mid-0.5);
      if (bounds.x2 >= image->columns)
        bounds.x2=image->columns-1.0;
      bounds.y2+=(mid-0.5);
      if (bounds.y2 >= image->rows)
        bounds.y2=image->rows-1.0;
      for (i=0; i < polygon_info->number_edges; i++)
      {
        if (polygon_info->edges[i].direction)
          QueryColorDatabase("red",&draw_info->stroke);
        else
          QueryColorDatabase("green",&draw_info->stroke);
        start.x=floor(polygon_info->edges[i].bounds.x1-mid);
        start.y=floor(polygon_info->edges[i].bounds.y1-mid);
        end.x=ceil(polygon_info->edges[i].bounds.x2+mid);
        end.y=ceil(polygon_info->edges[i].bounds.y2+mid);
        primitive_info[0].primitive=RectanglePrimitive;
        TraceRectangle(primitive_info,start,end);
        primitive_info[0].method=ReplaceMethod;
        coordinates=primitive_info[0].coordinates;
        primitive_info[coordinates].primitive=UndefinedPrimitive;
        DrawPrimitive(draw_info,primitive_info,image);
      }
    }
  QueryColorDatabase("blue",&draw_info->stroke);
  start.x=floor(bounds.x1-mid);
  start.y=floor(bounds.y1-mid);
  end.x=ceil(bounds.x2+mid);
  end.y=ceil(bounds.y2+mid);
  primitive_info[0].primitive=RectanglePrimitive;
  TraceRectangle(primitive_info,start,end);
  primitive_info[0].method=ReplaceMethod;
  coordinates=primitive_info[0].coordinates;
  primitive_info[coordinates].primitive=UndefinedPrimitive;
  DrawPrimitive(draw_info,primitive_info,image);
  DestroyDrawInfo(draw_info);
}

static inline int GetWindingNumber(const PolygonInfo *polygon_info,
  const double x,const double y)
{
  int
    j,
    winding_number;

  register double
    dx,
    dy;

  register int
    i;

  register PointInfo
    *p,
    *q;

  winding_number=0;
  for (i=0; i < polygon_info->number_edges; i++)
  {
    if (polygon_info->edges[i].bounds.y1 > y)
      break;
    if (polygon_info->edges[i].bounds.y2 <= y)
      continue;
    if (polygon_info->edges[i].bounds.x2 < x)
      {
        winding_number+=polygon_info->edges[i].direction ? 1 : -1;
        continue;
      }
    if (polygon_info->edges[i].bounds.x1 > x)
      continue;
    j=1;
    if (polygon_info->edges[i].highwater > 0)
      j=polygon_info->edges[i].highwater;
    for ( ; j < polygon_info->edges[i].number_points; j++)
      if (polygon_info->edges[i].points[j].y > y)
        break;
    p=polygon_info->edges[i].points+j-1;
    q=p+1;
    dx=q->x-p->x;
    dy=q->y-p->y;
    if ((dy*(x-p->x)) <= (dx*(y-p->y)))
      continue;
    winding_number+=polygon_info->edges[i].direction ? 1 : -1;
  }
  return(winding_number);
}

static void PrintPathInfo(const PathInfo *path_info)
{
  register const PathInfo
    *p;

  (void) fprintf(stdout,"  begin vector-path\n");
  for (p=path_info; p->code != EndCode; p++)
    (void) fprintf(stdout,"    %g,%g %s\n",p->point.x,p->point.y,
      p->code == HiddenCode ? "moveto visibility" :
      p->code == OpenCode ? "moveto open" : p->code == MoveToCode ? "moveto" :
      p->code == LineToCode ? "lineto" : "?");
  (void) fprintf(stdout,"  end vector-path\n");
}

static void PrintPolygonInfo(const PolygonInfo *polygon_info)
{
  register EdgeInfo
    *p;

  register int
    i,
    j;

  (void) fprintf(stdout,"  begin active-edge\n");
  p=polygon_info->edges;
  for (i=0; i < polygon_info->number_edges; i++)
  {
    (void) fprintf(stdout,"    edge %d:\n      direction: %s\n      "
      "visibility: %s\n      bounds: %g,%g - %g,%g\n",i,
      p->direction ? "down" : "up",p->visibility ? "transparent" : "opaque",
      p->bounds.x1,p->bounds.y1,p->bounds.x2,p->bounds.y2);
    for (j=0; j < p->number_points; j++)
      (void) fprintf(stdout,"        %g,%g\n",p->points[j].x,p->points[j].y);
    p++;
  }
  (void) fprintf(stdout,"  end active-edge\n");
}

static void PrintPrimitiveInfo(const PrimitiveInfo *primitive_info)
{
  char
    *methods[] =
    {
      "point",
      "replace",
      "floodfill",
      "filltoborder",
      "reset",
      "?"
    };

  int
    coordinates,
    y;

  PointInfo
    first,
    last,
    point;

  register int
    i,
    x;

  x=(int) ceil(primitive_info->point.x-0.5);
  y=(int) ceil(primitive_info->point.y-0.5);
  switch (primitive_info->primitive)
  {
    case PointPrimitive:
    {
      (void) fprintf(stdout,"PointPrimitive %d,%d %s\n",x,y,
        methods[primitive_info->method]);
      return;
    }
    case ColorPrimitive:
    {
      (void) fprintf(stdout,"ColorPrimitive %d,%d %s\n",x,y,
        methods[primitive_info->method]);
      return;
    }
    case MattePrimitive:
    {
      (void) fprintf(stdout,"MattePrimitive %d,%d %s\n",x,y,
        methods[primitive_info->method]);
      return;
    }
    case TextPrimitive:
    {
      (void) fprintf(stdout,"TextPrimitive %d,%d\n",x,y);
      return;
    }
    case ImagePrimitive:
    {
      (void) fprintf(stdout,"ImagePrimitive %d,%d\n",x,y);
      return;
    }
    default:
      break;
  }
  coordinates=0;
  last.x=(-1.0);
  last.y=(-1.0);
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
  {
    point=primitive_info[i].point;
    if (coordinates <= 0)
      {
        coordinates=primitive_info[i].coordinates;
        (void) fprintf(stdout,"  begin open (%d)\n",coordinates);
        first=point;
      }
    point=primitive_info[i].point;
    if ((fabs(last.x-point.x) <= MagickEpsilon) &&
        (fabs(last.y-point.y) <= MagickEpsilon))
      (void) fprintf(stdout,"    %d: %g,%g (duplicate)\n",coordinates,point.x,
        point.y);
    else
      (void) fprintf(stdout,"    %d: %g,%g\n",coordinates,point.x,point.y);
    last=point;
    coordinates--;
    if (coordinates > 0)
      continue;
    if ((fabs(first.x-point.x) <= MagickEpsilon) &&
        (fabs(first.y-point.y) <= MagickEpsilon))
      (void) fprintf(stdout,"  end open (%d)\n",coordinates);
    else
      (void) fprintf(stdout,"  end last (%d)\n",coordinates);
  }
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

static void DrawPolygonPrimitive(const DrawInfo *draw_info,
  const PrimitiveInfo *primitive_info,PolygonInfo *polygon_info,Image *image)
{
  double
    alpha,
    distance,
    fill_opacity,
    mid,
    stroke_opacity,
    subpath_opacity;

  int
    fill,
    j,
    number_edges,
    winding_number,
    y;

  PixelPacket
    color;

  register EdgeInfo
    *p;

  register int
    i,
    x;

  register PixelPacket
    *q;

  SegmentInfo
    bounds;

  /*
    Compute bounding box.
  */
  assert(primitive_info != (PrimitiveInfo *) NULL);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  bounds=polygon_info->edges[0].bounds;
  fill=False;
  if ((primitive_info->method == FillToBorderMethod) ||
      (primitive_info->method == FloodfillMethod))
    fill=True;
  mid=draw_info->linewidth/2.0;
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
  if (bounds.x1 < 0.0)
    bounds.x1=0.0;
  bounds.y1-=(mid+1.0);
  if (bounds.y1 < 0.0)
    bounds.y1=0.0;
  bounds.x2+=(mid+1.0);
  if (bounds.x2 >= image->columns)
    bounds.x2=image->columns-1.0;
  bounds.y2+=(mid+1.0);
  if (bounds.y2 >= image->rows)
    bounds.y2=image->rows-1.0;
  for (y=(int) ceil(bounds.y1-0.5); y <= (int) floor(bounds.y2-0.5); y++)
  {
    x=(int) ceil(bounds.x1-0.5);
    q=GetImagePixels(image,x,y,(int) floor(bounds.x2-0.5)-x+1,1);
    if (q == (PixelPacket *) NULL)
      break;
    for ( ; x <= (int) floor(bounds.x2-0.5); x++)
    {
      /*
        Compute the stroke and fill opacity values.
      */
      fill_opacity=0.0;
      stroke_opacity=0.0;
      subpath_opacity=0.0;
      switch (primitive_info->coordinates)
      {
        case 0:
          break;
        case 1:
        {
          if ((x == (int) ceil(primitive_info->point.x-0.5)) &&
              (y == (int) ceil(primitive_info->point.y-0.5)))
            stroke_opacity=1.0;
          break;
        }
        default:
        {
          winding_number=fill ? GetWindingNumber(polygon_info,x,y) : 0;
          number_edges=polygon_info->number_edges;
          for (i=0; i < number_edges; i++)
          {
            p=polygon_info->edges+i;
            if ((p->bounds.y1-mid-0.5) > y)
              break;
            if ((p->bounds.y2+mid+0.5) < y)
              {
                number_edges=DestroyEdge(polygon_info,i);
                continue;
              }
            if (x > (p->bounds.x2+mid+0.5))
              continue;
            if (x < (p->bounds.x1-mid-0.5))
              continue;
            j=p->highwater > 0 ? p->highwater : 1;
            for ( ; j < p->number_points; j++)
            {
              if ((p->points[j-1].y-mid-0.5) > y)
                break;
              if ((p->points[j].y+mid+0.5) <= y)
                continue;
              if (p->scanline != y)
                {
                  p->scanline=y;
                  p->highwater=j;
                }
              distance=DistanceToLine(p->points+j-1,x,y);
              if (!p->visibility)
                {
                  if ((distance <= ((mid+0.5)*(mid+0.5))) &&
                      (stroke_opacity < 1.0))
                    {
                      if (distance <= ((mid-0.5)*(mid-0.5)))
                        stroke_opacity=1.0;
                      else
                        {
                          alpha=sqrt(distance)-(mid+0.5);
                          stroke_opacity=Max(stroke_opacity,alpha*alpha);
                        }
                    }
                }
              if (!fill)
                continue;
              if ((distance > 1.0) || (subpath_opacity >= 1.0))
                continue;
              if (distance <= 0.0)
                {
                  subpath_opacity=1.0;
                  continue;
                }
              if (distance > 1.0)
                continue;
              alpha=sqrt(distance)-1.0;
              subpath_opacity=Max(subpath_opacity,alpha*alpha);
            }
          }
          if (!fill)
            break;
          if (subpath_opacity > 0.0)
            fill_opacity=subpath_opacity;
          if (winding_number < 0)
            if ((-winding_number) & 0x01)
              fill_opacity=1.0;
          if (winding_number > 0)
            if (winding_number & 0x01)
              fill_opacity=1.0;
          break;
        }
      }
      alpha=1.0/MaxRGB;
      color=draw_info->fill;
      if (draw_info->tile != (Image *) NULL)
        color=GetOnePixel(draw_info->tile,x % draw_info->tile->columns,
          y % draw_info->tile->rows);
      if ((fill_opacity != 0.0) && (color.opacity != TransparentOpacity))
        {
          /*
            Fill.
          */
          fill_opacity=MaxRGB-fill_opacity*(MaxRGB-color.opacity);
          q->red=(Quantum) (alpha*(color.red*(MaxRGB-fill_opacity)+
            q->red*fill_opacity));
          q->green=(Quantum) (alpha*(color.green*(MaxRGB-fill_opacity)+
            q->green*fill_opacity));
          q->blue=(Quantum) (alpha*(color.blue*(MaxRGB-fill_opacity)+
            q->blue*fill_opacity));
          q->opacity=(Quantum) (alpha*(color.opacity*(MaxRGB-fill_opacity)+
            q->opacity*fill_opacity));
        }
      color=draw_info->stroke;
      if ((stroke_opacity != 0.0) && (color.opacity != TransparentOpacity))
        {
          /*
            Stroke.
          */
          stroke_opacity=MaxRGB-stroke_opacity*(MaxRGB-color.opacity);
          q->red=(Quantum) (alpha*(color.red*(MaxRGB-stroke_opacity)+
            q->red*stroke_opacity));
          q->green=(Quantum) (alpha*(color.green*(MaxRGB-stroke_opacity)+
            q->green*stroke_opacity));
          q->blue=(Quantum) (alpha*(color.blue*(MaxRGB-stroke_opacity)+
            q->blue*stroke_opacity));
          q->opacity=(Quantum) (alpha*(stroke_opacity*(MaxRGB-stroke_opacity)+
            q->opacity*stroke_opacity));
        }
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P r i m i t i v e                                                 %
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
%      void DrawPrimitive(const DrawInfo *draw_info,
%        PrimitiveInfo *primitive_info,Image *image)
%
%  A description of each parameter follows:
%
%    o draw_info: The address of a DrawInfo structure.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%    o image: The address of a structure of type Image.
%
%
*/
static void DrawPrimitive(const DrawInfo *draw_info,
  PrimitiveInfo *primitive_info,Image *image)
{
  int
    y;

  register int
    i,
    x;

  register PixelPacket
    *q;

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
            target;

          target=GetOnePixel(image,x,y);
          (void) OpaqueImage(image,target,draw_info->fill);
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
          for (y=0; y < (int) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (int) image->columns; x++)
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
        MatteImage(image,OpaqueOpacity);
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
          (void) TransparentImage(image,target);
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
          for (y=0; y < (int) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (int) image->columns; x++)
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
      AnnotateInfo
        *annotate;

      ImageInfo
        *clone_info;

      if (primitive_info->text == (char *) NULL)
        break;
      clone_info=CloneImageInfo((ImageInfo *) NULL);
      clone_info->font=AllocateString(draw_info->font);
      clone_info->antialias=draw_info->text_antialias;
      clone_info->pointsize=draw_info->pointsize;
      clone_info->affine=draw_info->affine;
      annotate=CloneAnnotateInfo(clone_info,(AnnotateInfo *) NULL);
      DestroyImageInfo(clone_info);
      annotate->degrees=draw_info->angle;
      annotate->gravity=draw_info->gravity;
      annotate->decorate=draw_info->decorate;
      annotate->geometry=AllocateString((char *) NULL);
      annotate->fill=draw_info->fill;
      annotate->stroke=draw_info->stroke;
      annotate->box=draw_info->box;
      annotate->text=AllocateString(primitive_info->text);
      FormatString(annotate->geometry,"%+d%+d",x,y);
      AnnotateImage(image,annotate);
      DestroyAnnotateInfo(annotate);
      break;
    }
    case ImagePrimitive:
    {
      Image
        *composite_image;

      ImageInfo
        *clone_info;

      if (primitive_info->text == (char *) NULL)
        break;
      clone_info=CloneImageInfo((ImageInfo *) NULL);
      (void) strcpy(clone_info->filename,primitive_info->text);
      composite_image=ReadImage(clone_info,&image->exception);
      DestroyImageInfo(clone_info);
      if (composite_image == (Image *) NULL)
        break;
      if ((primitive_info[1].point.x != 0) && (primitive_info[1].point.y != 0))
        {
          char
            geometry[MaxTextExtent];

          /*
            Resize image.
          */
          FormatString(geometry,"%gx%g",primitive_info[1].point.x,
            primitive_info[1].point.y);
          TransformImage(&composite_image,(char *) NULL,geometry);
        }
      if ((draw_info->affine.rx == 0.0) && (draw_info->affine.ry == 0.0))
        {
          if ((draw_info->affine.sx != 1.0) || (draw_info->affine.sx != 1.0))
            {
              Image
                *scale_image;

              unsigned int
                height,
                width;

              width=(unsigned int)
                (draw_info->affine.sx*composite_image->columns);
              height=(unsigned int)
                (draw_info->affine.sy*composite_image->rows);
              scale_image=ZoomImage(composite_image,width,height,
                &image->exception);
              if (scale_image != (Image *) NULL)
                {
                  DestroyImage(composite_image);
                  composite_image=scale_image;
                }
            }
        }
      else
        {
          if (((draw_info->affine.sx-draw_info->affine.sy) == 0.0) &&
              ((draw_info->affine.rx+draw_info->affine.ry) == 0.0))
            {
              double
                theta;

              Image
                *rotate_image;

              theta=(180.0/M_PI)*
                atan2(draw_info->affine.rx,draw_info->affine.sx);
              rotate_image=
                RotateImage(composite_image,theta,&image->exception);
              if (rotate_image != (Image *) NULL)
                {
                  DestroyImage(composite_image);
                  composite_image=rotate_image;
                }
            }
        }
      CompositeImage(image,image->matte ? OverCompositeOp : ReplaceCompositeOp,
        composite_image,x,y);
      DestroyImage(composite_image);
      break;
    }
    default:
    {
      PathInfo
        *path_info;

      PolygonInfo
        *polygon_info;

      if (draw_info->verbose)
        PrintPrimitiveInfo(primitive_info);
      path_info=ConvertPrimitiveToPath(primitive_info);
      if (path_info == (PathInfo *) NULL)
        return;
      if (draw_info->verbose)
        PrintPathInfo(path_info);
      polygon_info=ConvertPathToPolygon(path_info);
      LiberateMemory((void **) &path_info);
      if (polygon_info == (PolygonInfo *) NULL)
        return;
      if (draw_info->verbose)
        PrintPolygonInfo(polygon_info);
#ifdef DEBUG_BOUND_BOXES
      DrawBoundingRectangles(image,polygon_info);
#endif
      DrawPolygonPrimitive(draw_info,primitive_info,polygon_info,image);
      DestroyPolygonInfo(polygon_info);
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t D r a w I n f o                                                     %
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
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o draw_info: Specifies a pointer to a DrawInfo structure.
%
%
*/
MagickExport void GetDrawInfo(const ImageInfo *image_info,DrawInfo *draw_info)
{
  register int
    i;

  /*
    Initialize draw attributes.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  memset(draw_info,0,sizeof(DrawInfo));
  draw_info->affine=image_info->affine;
  draw_info->gravity=NorthWestGravity;
  draw_info->fill=image_info->fill;
  draw_info->stroke=image_info->stroke;
  draw_info->stroke_antialias=image_info->antialias;
  draw_info->linewidth=1.0;
  draw_info->linecap=ButtCap;
  draw_info->linejoin=MiterJoin;
  draw_info->miterlimit=4;
  draw_info->dash_pattern=(unsigned int *) NULL;
  draw_info->dash_offset=0;
  draw_info->decorate=NoDecoration;
  draw_info->font=AllocateString(image_info->font);
  draw_info->text_antialias=image_info->antialias;
  draw_info->pointsize=image_info->pointsize;
  (void) QueryColorDatabase("none",&draw_info->box);
  draw_info->border_color=image_info->border_color;
  draw_info->verbose=image_info->verbose;
  draw_info->signature=MagickSignature;
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
%  Method MatteFloodfillImage floodfills the designated area with a matte
%  value.  The floodfill algorithm is strongly based on a similar algorithm in
%  "Graphics Gems" by Paul Heckbert.
%
%  The format of the MatteFloodfillImage method is:
%
%      unsigned int MatteFloodfillImage(Image *image,const PixelPacket target,
%        const unsigned int matte,const int x_offset,const int y_offset,
%        const PaintMethod method)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o target: A PixelPacket structure.  This is the RGB value of the target
%      color.
%
%    o matte: A integer value representing the amount of transparency.
%
%    o x,y: Unsigned integers representing the current location of the pen.
%
%    o method: drawing method of type PrimitiveType: floodfill or fill to
%      border.
%
%
*/
MagickExport unsigned int MatteFloodfillImage(Image *image,
  const PixelPacket target,const unsigned int matte,const int x_offset,
  const int y_offset,const PaintMethod method)
{
  int
    offset,
    skip,
    start,
    x1,
    x2,
    y;

  register int
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
  if ((x_offset < 0) || (x_offset >= (int) image->columns))
    return(False);
  if ((y_offset < 0) || (y_offset >= (int) image->rows))
    return(False);
  if (target.opacity == matte)
    return(False);
  q=GetImagePixels(image,x_offset,y_offset,1,1);
  if (q == (PixelPacket *) NULL)
    return(False);
  if (q->opacity == matte)
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
  image->storage_class=DirectClass;
  if (!image->matte)
    MatteImage(image,OpaqueOpacity);
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
    for (x=x1; x >= 0 ; x--)
    {
      if (method == FloodfillMethod)
        {
          if (!MatteMatch(*q,target,image->fuzz))
            break;
        }
      else
        if (MatteMatch(*q,target,image->fuzz) || (q->opacity == matte))
          break;
      q->opacity=matte;
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
          for ( ; x < (int) image->columns; x++)
          {
            if (method == FloodfillMethod)
              {
                if (!MatteMatch(*q,target,image->fuzz))
                  break;
              }
            else
              if (MatteMatch(*q,target,image->fuzz) || (q->opacity == matte))
                break;
            q->opacity=matte;
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
      for (x++; x <= x2 ; x++)
      {
        q++;
        if (method == FloodfillMethod)
          {
            if (MatteMatch(*q,target,image->fuzz))
              break;
          }
        else
          if (!MatteMatch(*q,target,image->fuzz) && (q->opacity != matte))
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
%  Method OpaqueImage changes the color of an opaque point to the pen color.
%
%  The format of the OpaqueImage method is:
%
%      unsigned int OpaqueImage(Image *image,const PixelPacket target,
%        const PixelPacket pen_color)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o target: the color to search for in the image.
%
%    o pen_color: the color to replace it with.
%
%
*/
MagickExport unsigned int OpaqueImage(Image *image,const PixelPacket target,
  const PixelPacket pen_color)
{
#define OpaqueImageText  "  Setting opaque color in the image...  "

  int
    y;

  register int
    i,
    x;

  register PixelPacket
    *q;

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
      for (y=0; y < (int) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          if (ColorMatch(*q,target,image->fuzz))
            *q=pen_color;
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
      for (i=0; i < (int) image->colors; i++)
      {
        if (ColorMatch(image->colormap[i],target,image->fuzz))
          image->colormap[i]=pen_color;
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
    arc_angle,
    beta,
    factor,
    gamma;

  int
    number_segments;

  PointInfo
    center,
    points[3];

  register double
    cosine,
    sine;

  register int
    i;

  register PrimitiveInfo
    *p;

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
  arc_angle=atan2(points[1].y-center.y,points[1].x-center.x)-alpha;
  if ((arc_angle < 0.0) && sweep)
    arc_angle+=2.0*M_PI;
  else
    if ((arc_angle > 0.0) && !sweep)
      arc_angle-=2.0*M_PI;
  number_segments=ceil(fabs(arc_angle/(0.5*M_PI+MagickEpsilon)));
  p=primitive_info;
  for (i=0; i < number_segments; i++)
  {
    beta=0.5*((alpha+(i+1)*arc_angle/number_segments)-
      (alpha+i*arc_angle/number_segments));
    gamma=(8.0/3.0)*sin(0.5*beta)*sin(0.5*beta)/sin(beta);
    points[0].x=center.x+cos((alpha+i*arc_angle/number_segments))-
      gamma*sin((alpha+i*arc_angle/number_segments));
    points[0].y=center.y+sin((alpha+i*arc_angle/number_segments))+
      gamma*cos((alpha+i*arc_angle/number_segments));
    points[2].x=center.x+cos((alpha+(i+1)*arc_angle/number_segments));
    points[2].y=center.y+sin((alpha+(i+1)*arc_angle/number_segments));
    points[1].x=points[2].x+gamma*sin((alpha+(i+1)*arc_angle/number_segments));
    points[1].y=points[2].y-gamma*cos((alpha+(i+1)*arc_angle/number_segments));
    p->point.x=(p == primitive_info) ? start.x : (p-1)->point.x;
    p->point.y=(p == primitive_info) ? start.y : (p-1)->point.y;
    (p+1)->point.x=cosine*arc.x*points[0].x-sine*arc.y*points[0].y;
    (p+1)->point.y=sine*arc.x*points[0].x+cosine*arc.y*points[0].y;
    (p+2)->point.x=cosine*arc.x*points[1].x-sine*arc.y*points[1].y;
    (p+2)->point.y=sine*arc.x*points[1].x+cosine*arc.y*points[1].y;
    (p+3)->point.x=cosine*arc.x*points[2].x-sine*arc.y*points[2].y;
    (p+3)->point.y=sine*arc.x*points[2].x+cosine*arc.y*points[2].y;
    if (i == (number_segments-1))
      (p+3)->point=end;
    TraceBezier(p,4);
    p+=p->coordinates;
  }
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void TraceBezier(PrimitiveInfo *primitive_info,
  const unsigned int number_coordinates)
{
  double
    alpha,
    *coefficients,
    weight;

  PointInfo
    end,
    point,
    *points;

  register int
    i,
    j;

  register PrimitiveInfo
    *p;

  unsigned int
    control_points,
    quantum;

  /*
    Allocate coeficients.
  */
  quantum=number_coordinates;
  for (i=0; i < (int) number_coordinates; i++)
  {
    for (j=i+1; j < (int) number_coordinates; j++)
    {
      alpha=fabs(primitive_info[j].point.x-primitive_info[i].point.x);
      if (alpha > quantum)
        quantum=alpha;
      alpha=fabs(primitive_info[j].point.y-primitive_info[i].point.y);
      if (alpha > quantum)
        quantum=alpha;
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
  for (i=0; i < (int) number_coordinates; i++)
    coefficients[i]=Permutate(number_coordinates-1,i);
  for (i=0; i < (int) control_points; i++)
  {
    p=primitive_info;
    point.x=0;
    point.y=0;
    alpha=pow(1.0-weight,number_coordinates-1);
    for (j=0; j < (int) number_coordinates; j++)
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
  for (i=0; i < (int) control_points; i++)
  {
    TracePoint(p,points[i]);
    p++;
  }
  TracePoint(p,end);
  primitive_info->coordinates=p-primitive_info+1;
  for (i=0; i < primitive_info->coordinates; i++)
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

  register int
    i;

  register PrimitiveInfo
    *p;

  /*
    Ellipses are just short segmented polys.
  */
  p=primitive_info;
  for (angle=(degrees.x+1.0); angle <= degrees.y; angle+=1.0)
  {
    point.x=cos(DegreesToRadians(fmod(angle,360.0)))*end.x+start.x;
    point.y=sin(DegreesToRadians(fmod(angle,360.0)))*end.y+start.y;
    TracePoint(p,point);
    p++;
  }
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < primitive_info->coordinates; i++)
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
  (primitive_info+1)->primitive=primitive_info->primitive;
  primitive_info->coordinates=2;
}

static unsigned int TracePath(PrimitiveInfo *primitive_info,const char *path)
{
  char
    *p;

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

  register int
    i;

  register PrimitiveInfo
    *q;

  unsigned int
    number_coordinates,
    subpath,
    z_count;

  point.x=0;
  point.y=0;
  number_coordinates=0;
  subpath=False;
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
        subpath=True;
        arc.x=strtod(p,&p);
        if (*p == ',')
          p++;
        arc.y=strtod(p,&p);
        if (*p == ',')
          p++;
        angle=strtod(p,&p);
        if (*p == ',')
          p++;
        large_arc=strtod(p,&p);
        if (*p == ',')
          p++;
        sweep=strtod(p,&p);
        if (*p == ',')
          p++;
        x=strtod(p,&p);
        if (*p == ',')
          p++;
        y=strtod(p,&p);
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
        subpath=True;
        do
        {
          points[0]=point;
          for (i=1; i <= 3; i++)
          {
            x=strtod(p,&p);
            if (*p == ',')
              p++;
            y=strtod(p,&p);
            if (*p == ',')
              p++;
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
          x=strtod(p,&p);
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
          x=strtod(p,&p);
          if (*p == ',')
            p++;
          y=strtod(p,&p);
          if (*p == ',')
            p++;
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
        if (subpath)
          {
            primitive_info->coordinates=q-primitive_info;
            number_coordinates+=primitive_info->coordinates;
            primitive_info=q;
          }
        do
        {
          x=strtod(p,&p);
          if (*p == ',')
            p++;
          y=strtod(p,&p);
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
        subpath=True;
        do
        {
          points[0]=point;
          for (i=1; i < 3; i++)
          {
            x=strtod(p,&p);
            if (*p == ',')
              p++;
            y=strtod(p,&p);
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
        subpath=True;
        do
        {
          points[0]=points[3];
          points[1].x=2.0*points[3].x-points[2].x;
          points[1].y=2.0*points[3].y-points[2].y;
          for (i=2; i <= 3; i++)
          {
            x=strtod(p,&p);
            if (*p == ',')
              p++;
            y=strtod(p,&p);
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
        subpath=True;
        do
        {
          points[0]=points[2];
          points[1].x=2.0*points[2].x-points[1].x;
          points[1].y=2.0*points[2].y-points[1].y;
          for (i=2; i < 3; i++)
          {
            x=strtod(p,&p);
            if (*p == ',')
              p++;
            y=strtod(p,&p);
            if (*p == ',')
              p++;
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
          y=strtod(p,&p);
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
  if (z_count == 0)
    {
      TracePoint(q,point);
      q+=q->coordinates;
    }
  primitive_info->coordinates=q-primitive_info;
  number_coordinates+=primitive_info->coordinates;
  for (i=0; i < number_coordinates; i++)
  {
    q--;
    q->primitive=primitive_type;
    if (z_count > 1)
      q->method=FillToBorderMethod;
  }
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

  register int
    i;

  register PrimitiveInfo
    *p;

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
  for (i=0; i < primitive_info->coordinates; i++)
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

  register int
    i;

  register PrimitiveInfo
    *p;

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
  for (i=0; i < primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
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
%  Method TransparentImage creates a matte image associated with the
%  image.  All pixel locations are initially set to opaque.  Any pixel
%  that matches the specified color are set to transparent.
%
%  The format of the TransparentImage method is:
%
%      unsigned int TransparentImage(Image *image,const PixelPacket target)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o target: The color to search for in the image.
%
%
*/
MagickExport unsigned int TransparentImage(Image *image,
  const PixelPacket target)
{
#define TransparentImageText  "  Setting transparent color in the image...  "

  int
    y;

  register int
    x;

  register PixelPacket
    *q;

  /*
    Make image color transparent.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (!image->matte)
    MatteImage(image,OpaqueOpacity);
  for (y=0; y < (int) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) image->columns; x++)
    {
      if (ColorMatch(*q,target,image->fuzz))
        q->opacity=TransparentOpacity;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(TransparentImageText,y,image->rows);
  }
  return(True);
}
