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
#define BezierQuantum  4
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
  Typedef declaractions.
*/
typedef struct _PrimitiveInfo
{
  PrimitiveType
    primitive;

  unsigned int
    coordinates;

  PointInfo
    pixel;

  PaintMethod
    method;

  char
    *text;
} PrimitiveInfo;

/*
  Forward declarations
*/
static double
  GenerateCircle(PrimitiveInfo *,PointInfo,PointInfo),
  InsidePrimitive(PrimitiveInfo *,const DrawInfo *,const PointInfo *,const int,
    Image *);

static void
  GenerateArc(PrimitiveInfo *,PointInfo,PointInfo,PointInfo),
  GenerateBezier(PrimitiveInfo *),
  GenerateEllipse(PrimitiveInfo *,PointInfo,PointInfo,PointInfo),
  GenerateLine(PrimitiveInfo *,PointInfo,PointInfo),
  GeneratePath(PrimitiveInfo *,const char *),
  GeneratePoint(PrimitiveInfo *,PointInfo),
  GenerateRectangle(PrimitiveInfo *,PointInfo,PointInfo),
  GenerateRoundRectangle(PrimitiveInfo *,PointInfo,PointInfo,PointInfo);

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

  cloned_info=(DrawInfo *) AllocateMemory(sizeof(DrawInfo));
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
  double
    alpha;

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
  assert(draw_info != (DrawInfo *) NULL);
  if ((x_offset < 0) || (x_offset >= (int) image->columns))
    return(False);
  if ((y_offset < 0) || (y_offset >= (int) image->rows))
    return(False);
  /*
    Set floodfill color.
  */
  if (ColorMatch(draw_info->fill,target,image->fuzz))
    return(False);
  segment_stack=(SegmentInfo *)
    AllocateMemory(MaxStacksize*sizeof(SegmentInfo));
  if (segment_stack == (SegmentInfo *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Unable to floodfill image",
      image->filename);
  /*
    Track "hot" pixels with the image index packets.
  */
  alpha=1.0/OpaqueOpacity;
  image->class=PseudoClass;
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
                indexes[i]=True;
                *q=draw_info->fill;
                i++;
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
    for (y=0; y < (int) image->rows; y++)
    {
      /*
        Tile image onto floodplane.
      */
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
                q->red=alpha*(color.red*color.opacity+
                  q->red*(OpaqueOpacity-color.opacity));
                q->green=alpha*(color.green*color.opacity+
                  q->green*(OpaqueOpacity-color.opacity));
                q->blue=alpha*(color.blue*color.opacity+
                  q->blue*(OpaqueOpacity-color.opacity));
                q->opacity=alpha*(color.opacity*
                  color.opacity+q->opacity*(OpaqueOpacity-color.opacity));
              }
          }
        q++;
      }
      if (!SyncImagePixels(image))
        break;
    }
  image->class=DirectClass;
  FreeMemory((void **) &segment_stack);
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
  if (draw_info->primitive != (char *) NULL)
    FreeMemory((void **) &draw_info->primitive);
  if (draw_info->font != (char *) NULL)
    FreeMemory((void **) &draw_info->font);
  if (draw_info->tile != (Image *) NULL)
    DestroyImage(draw_info->tile);
  FreeMemory((void **) &draw_info);
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
#define DrawImageText  "  Drawing on image...  "

  char
    keyword[MaxTextExtent],
    *p,
    *primitive;

  double
    alpha,
    mid,
    opacity,
    radius;

  DrawInfo
    *clone_info;

  int
    j,
    n,
    y;

  PixelPacket
    color;

  PointInfo
    pixel,
    target;

  PrimitiveInfo
    *primitive_info;

  PrimitiveType
    primitive_type;

  register int
    i,
    x;

  register PixelPacket
    *q;

  SegmentInfo
    bounds;

  unsigned int
    indirection,
    length,
    number_points;

  /*
    Ensure the annotation info is valid.
  */
  assert(image != (Image *) NULL);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->primitive != (char *) NULL);
  if (*draw_info->primitive == '\0')
    return(False);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  primitive=clone_info->primitive;
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
          DestroyDrawInfo(clone_info);
          ThrowBinaryException(FileOpenWarning,"Unable to read primitive file",
            primitive+1);
        }
      length=MaxTextExtent;
      primitive=(char *) AllocateMemory(length);
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
            ReallocateMemory((void **) &primitive,length);
            if (primitive == (char *) NULL)
              break;
            q=primitive+Extent(primitive);
          }
        *q++=c;
      }
      (void) fclose(file);
      if (primitive == (char *) NULL)
        {
          DestroyDrawInfo(clone_info);
          ThrowBinaryException(ResourceLimitWarning,"Unable to draw image",
            "Memory allocation failed");
        }
      *q='\0';
    }
  /*
    Allocate primitive info memory.
  */
  number_points=2048;
  primitive_info=(PrimitiveInfo *)
    AllocateMemory(number_points*sizeof(PrimitiveInfo));
  if (primitive_info == (PrimitiveInfo *) NULL)
    {
      if (indirection)
        FreeMemory((void **) &primitive);
      DestroyDrawInfo(clone_info);
      ThrowBinaryException(ResourceLimitWarning,"Unable to draw image",
        "Memory allocation failed");
    }
  for (p=primitive; *p != '\0'; )
  {
    /*
      Define primitive.
    */
    while (isspace((int) (*p)) && (*p != '\0'))
      p++;
    for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
      keyword[x]=(*p++);
    keyword[x]='\0';
    if (*keyword == '\0')
      break;
    while (isspace((int) (*p)) && (*p != '\0'))
      p++;
    if (LocaleCompare("affine",keyword) == 0)
      {
        for (n=0; n < 6; n++)
        {
          clone_info->affine[n]=strtod(p,&p);
          if (*p == ',')
            break;
        }
        continue;
      }
    if (LocaleCompare("angle",keyword) == 0)
      {
        clone_info->angle=strtod(p,&p);
        continue;
      }
    if (LocaleCompare("antialias",keyword) == 0)
      {
        clone_info->antialias=strtod(p,&p);
        continue;
      }
    if (LocaleCompare("decorate",keyword) == 0)
      {
        for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
          keyword[x]=(*p++);
        keyword[x]='\0';
        if (LocaleCompare("none",keyword) == 0)
          clone_info->decorate=NoDecoration;
        if (LocaleCompare("underline",keyword) == 0)
          clone_info->decorate=UnderlineDecoration;
        if (LocaleCompare("overline",keyword) == 0)
          clone_info->decorate=OverlineDecoration;
        if (LocaleCompare("line-through",keyword) == 0)
          clone_info->decorate=LineThroughDecoration;
        continue;
      }
    if (LocaleCompare("fill",keyword) == 0)
      {
        for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
          keyword[x]=(*p++);
        keyword[x]='\0';
        (void) QueryColorDatabase(keyword,&clone_info->fill);
        continue;
      }
    if (LocaleCompare("font",keyword) == 0)
      {
        for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
          keyword[x]=(*p++);
        keyword[x]='\0';
        CloneString(&clone_info->font,keyword);
        continue;
      }
    if (LocaleCompare("gravity",keyword) == 0)
      {
        for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
          keyword[x]=(*p++);
        keyword[x]='\0';
        if (LocaleCompare("NorthWest",keyword) == 0)
          clone_info->gravity=NorthWestGravity;
        if (LocaleCompare("North",keyword) == 0)
          clone_info->gravity=NorthGravity;
        if (LocaleCompare("NorthEast",keyword) == 0)
          clone_info->gravity=NorthEastGravity;
        if (LocaleCompare("West",keyword) == 0)
          clone_info->gravity=WestGravity;
        if (LocaleCompare("Center",keyword) == 0)
          clone_info->gravity=CenterGravity;
        if (LocaleCompare("East",keyword) == 0)
          clone_info->gravity=EastGravity;
        if (LocaleCompare("SouthWest",keyword) == 0)
          clone_info->gravity=SouthWestGravity;
        if (LocaleCompare("South",keyword) == 0)
          clone_info->gravity=SouthGravity;
        if (LocaleCompare("SouthEast",keyword) == 0)
          clone_info->gravity=SouthEastGravity;
        continue;
      }
    if (LocaleCompare("linewidth",keyword) == 0)
      {
        clone_info->linewidth=strtod(p,&p);
        continue;
      }
    if (LocaleCompare("opacity",keyword) == 0)
      {
        clone_info->opacity=strtod(p,&p);
        continue;
      }
    if (LocaleCompare("pointsize",keyword) == 0)
      {
        clone_info->pointsize=strtod(p,&p);
        continue;
      }
    if (LocaleCompare("stroke",keyword) == 0)
      {
        for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
          keyword[x]=(*p++);
        keyword[x]='\0';
        (void) QueryColorDatabase(keyword,&clone_info->stroke);
        continue;
      }
    primitive_type=UndefinedPrimitive;
    if (LocaleNCompare("fill",keyword,4) == 0)
      (void) strcpy(keyword,keyword+4);
    if (LocaleCompare("Point",keyword) == 0)
      primitive_type=PointPrimitive;
    if (LocaleCompare("Line",keyword) == 0)
      primitive_type=LinePrimitive;
    if (LocaleNCompare("Rect",keyword,4) == 0)
      primitive_type=RectanglePrimitive;
    if (LocaleCompare("RoundRectangle",keyword) == 0)
      primitive_type=RoundRectanglePrimitive;
    if (LocaleCompare("Arc",keyword) == 0)
      primitive_type=ArcPrimitive;
    if (LocaleCompare("Ellipse",keyword) == 0)
      primitive_type=EllipsePrimitive;
    if (LocaleCompare("Circle",keyword) == 0)
      primitive_type=CirclePrimitive;
    if (LocaleCompare("Polyline",keyword) == 0)
      primitive_type=PolylinePrimitive;
    if (LocaleCompare("Polygon",keyword) == 0)
      primitive_type=PolygonPrimitive;
    if (LocaleCompare("Bezier",keyword) == 0)
      primitive_type=BezierPrimitive;
    if (LocaleCompare("Path",keyword) == 0)
      primitive_type=PathPrimitive;
    if (LocaleCompare("Color",keyword) == 0)
      primitive_type=ColorPrimitive;
    if (LocaleCompare("Matte",keyword) == 0)
      primitive_type=MattePrimitive;
    if (LocaleCompare("Text",keyword) == 0)
      primitive_type=TextPrimitive;
    if (LocaleCompare("Image",keyword) == 0)
      primitive_type=ImagePrimitive;
    if (primitive_type == UndefinedPrimitive)
      break;
    /*
      Parse the primitive attributes.
    */
    bounds.x1=image->columns-1.0;
    bounds.y1=image->rows-1.0;
    bounds.x2=0.0;
    bounds.y2=0.0;
    radius=0.0;
    i=0;
    j=0;
    for (x=0; *p != '\0'; x++)
    {
      /*
        Define points.
      */
      while (isspace((int) (*p)) && (*p != '\0'))
        p++;
      if (!IsGeometry(p))
        break;
      pixel.x=strtod(p,&p);
      if (*p == ',')
        p++;
      pixel.y=strtod(p,&p);
      if (*p == ',')
        p++;
      primitive_info[i].primitive=primitive_type;
      primitive_info[i].coordinates=0;
      primitive_info[i].pixel=pixel;
      primitive_info[i].method=FloodfillMethod;
      while ((isspace((int) (*p)) || (*p == ',')) && (*p != '\0'))
        p++;
      i++;
      if (i < (int) (number_points-Max(BezierQuantum*BezierQuantum*x,360)-1))
        continue;
      number_points<<=1;
      ReallocateMemory((void **) &primitive_info,
        number_points*sizeof(PrimitiveInfo));
      if (primitive_info != (PrimitiveInfo *) NULL)
        continue;
      if (indirection)
        FreeMemory((void **) &primitive);
      DestroyDrawInfo(clone_info);
      ThrowBinaryException(ResourceLimitWarning,"Unable to draw image",
        "Memory allocation failed");
    }
    while (isspace((int) (*p)) && (*p != '\0'))
      p++;
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
            primitive_type=UndefinedPrimitive;
            break;
          }
        GeneratePoint(primitive_info+j,primitive_info[j].pixel);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case LinePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateLine(primitive_info+j,primitive_info[j].pixel,
          primitive_info[j+1].pixel);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case RectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateRectangle(primitive_info+j,primitive_info[j].pixel,
          primitive_info[j+1].pixel);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case RoundRectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateRoundRectangle(primitive_info+j,primitive_info[j].pixel,
          primitive_info[j+1].pixel,primitive_info[j+2].pixel);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case ArcPrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateArc(primitive_info+j,primitive_info[j].pixel,
          primitive_info[j+1].pixel,primitive_info[j+2].pixel);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case EllipsePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateEllipse(primitive_info+j,primitive_info[j].pixel,
          primitive_info[j+1].pixel,primitive_info[j+2].pixel);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case CirclePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        radius=GenerateCircle(primitive_info+j,primitive_info[j].pixel,
          primitive_info[j+1].pixel);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case PolylinePrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        break;
      }
      case PolygonPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            primitive_type=UndefinedPrimitive;
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
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateBezier(primitive_info+j);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case PathPrimitive:
      {
        char
          *path;

        path=p;
        if (*p == '"')
          {
            for (p++; *p != '\0'; p++)
              if ((*p == '"') && (*(p-1) != '\\'))
                break;
          }
        else
          if (*p == '\'')
            {
              for (p++; *p != '\0'; p++)
                if ((*p == '\'') && (*(p-1) != '\\'))
                  break;
            }
          else
            for (p++;  *p != '\0'; p++)
              if (isspace((int) *p) && (*(p-1) != '\\') && (*p != '\0'))
                break;
        if (*p != '\0')
          p++;
        *p++='\0';
        if (i < (number_points-BezierQuantum*BezierQuantum*Extent(path)-1))
          {
            number_points+=BezierQuantum*BezierQuantum*Extent(path);
            ReallocateMemory((void **) &primitive_info,
              number_points*sizeof(PrimitiveInfo));
            if (primitive_info == (PrimitiveInfo *) NULL)
              {
                if (indirection)
                  FreeMemory((void **) &primitive);
                DestroyDrawInfo(clone_info);
                ThrowBinaryException(ResourceLimitWarning,
                  "Unable to draw image","Memory allocation failed");
              }
          }
        GeneratePath(primitive_info+j,path);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case ColorPrimitive:
      case MattePrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        /*
          Define method.
        */
        for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
          keyword[x]=(*p++);
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
                  primitive_type=UndefinedPrimitive;
        if ((primitive_info[j].method == ReplaceMethod) ||
            (primitive_info[j].method == ResetMethod))
          {
            bounds.x1=0.0;
            bounds.y1=0.0;
            bounds.x2=image->columns-1.0;
            bounds.y2=image->rows-1.0;
          }
        while (isspace((int) (*p)) && (*p != '\0'))
          p++;
        break;
      }
      case TextPrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        if (*p == '\0')
          break;
        primitive_info[j].text=p;
        if (*p == '"')
          {
            for (p++; *p != '\0'; p++)
              if ((*p == '"') && (*(p-1) != '\\'))
                break;
          }
        else
          if (*p == '\'')
            {
              for (p++; *p != '\0'; p++)
                if ((*p == '\'') && (*(p-1) != '\\'))
                  break;
            }
          else
            for (p++;  *p != '\0'; p++)
              if (isspace((int) *p) && (*(p-1) != '\\') && (*p != '\0'))
                break;
        if (*p != '\0')
          p++;
        break;
      }
      case ImagePrimitive:
      {
        Image
          *composite_image;

        ImageInfo
          *composite_info;

        register char
          *q;

        if (primitive_info[j].coordinates != 1)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        composite_info=CloneImageInfo((ImageInfo *) NULL);
        q=composite_info->filename;
        if (*p != '\0')
          {
            primitive_info[j].text=p;
            if (*p == '"')
              {
                for (p++; *p != '\0'; p++)
                {
                  if ((*p == '"') && (*(p-1) != '\\'))
                    break;
                  *q++=(*p);
                }
              }
            else
              if (*p == '\'')
                {
                  for (p++; *p != '\0'; p++)
                  {
                    if ((*p == '\'') && (*(p-1) != '\\'))
                      break;
                    *q++=(*p);
                  }
                }
              else
                for ( ;  *p != '\0'; p++)
                {
                  if (isspace((int) *p) && (*(p-1) != '\\') && (*p != '\0'))
                    break;
                  *q++=(*p);
                }
            if (*p != '\0')
              p++;
          }
        *q='\0';
        composite_image=ReadImage(composite_info,&image->exception);
        if (composite_image == (Image *) NULL)
          break;
        if ((clone_info->affine[1] == 0.0) && (clone_info->affine[2] == 0.0))
          {
            if ((clone_info->affine[0] != 1.0) ||
                (clone_info->affine[0] != 1.0))
              {
                Image
                  *scale_image;

                unsigned int
                  height,
                  width;

                width=clone_info->affine[0]*composite_image->columns;
                height=clone_info->affine[3]*composite_image->rows;
                scale_image=
                  ZoomImage(composite_image,width,height,&image->exception);
                if (scale_image != (Image *) NULL)
                  {
                    DestroyImage(composite_image);
                    composite_image=scale_image;
                  }
              }
          }
        else
          {
            if (((clone_info->affine[0]-clone_info->affine[3]) == 0.0) &&
                ((clone_info->affine[1]+clone_info->affine[2]) == 0.0))
              {
                double
                  theta;

                Image
                  *rotate_image;

                theta=(180.0/M_PI)*
                  atan2(clone_info->affine[1],clone_info->affine[0]);
                rotate_image=
                  RotateImage(composite_image,theta,&image->exception);
                if (rotate_image != (Image *) NULL)
                  {
                    DestroyImage(composite_image);
                    composite_image=rotate_image;
                  }
              }
          }
        CompositeImage(image,image->matte ? OverCompositeOp :
          ReplaceCompositeOp,composite_image,(int) pixel.x,(int) pixel.y);
        DestroyImage(composite_image);
        DestroyImageInfo(composite_info);
        break;
      }
    }
    if (primitive_type == UndefinedPrimitive)
      break;
    primitive_info[i].primitive=UndefinedPrimitive;
    /*
      Transform points.
    */
    for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
    {
      pixel=primitive_info[i].pixel;
      primitive_info[i].pixel.x=clone_info->affine[0]*pixel.x+
        clone_info->affine[2]*pixel.y+clone_info->affine[4];
      primitive_info[i].pixel.y=clone_info->affine[1]*pixel.x+
        clone_info->affine[3]*pixel.y+clone_info->affine[5];
    }
    /*
      Compute bounding box.
    */
    for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
    {
      pixel=primitive_info[i].pixel;
      if (pixel.x < bounds.x1)
        bounds.x1=pixel.x;
      if (pixel.y < bounds.y1)
        bounds.y1=pixel.y;
      if (pixel.x > bounds.x2)
        bounds.x2=pixel.x;
      if (pixel.y > bounds.y2)
        bounds.y2=pixel.y;
    }
    mid=ceil(radius+clone_info->linewidth/2.0);
    bounds.x1-=mid;
    if (bounds.x1 < 0.0)
      bounds.x1=0.0;
    bounds.y1-=mid;
    if (bounds.y1 < 0.0)
      bounds.y1=0.0;
    bounds.x2+=mid;
    if (bounds.x2 >= image->columns)
      bounds.x2=image->columns-1.0;
    bounds.y2+=mid;
    if (bounds.y2 >= image->rows)
      bounds.y2=image->rows-1.0;
    alpha=1.0/OpaqueOpacity;
    image->class=DirectClass;
    for (y=(int) bounds.y1; y <= (int) bounds.y2; y++)
    {
      /*
        Fill the primitive on the image.
      */
      n=bounds.x2-bounds.x1+1.0;
      target.y=y;
      q=GetImagePixels(image,(int) bounds.x1,y,n,1);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=(int) bounds.x1; x <= (int) bounds.x2; x++)
      {
        target.x=x;
        switch (primitive_info->primitive)
        {
          case ArcPrimitive:
          case BezierPrimitive:
          case CirclePrimitive:
          case EllipsePrimitive:
          case PathPrimitive:
          case PolylinePrimitive:
          case PolygonPrimitive:
          case RectanglePrimitive:
          case RoundRectanglePrimitive:
          {
            opacity=
              InsidePrimitive(primitive_info,clone_info,&target,True,image);
            color=clone_info->fill;
            if (clone_info->tile != (Image *) NULL)
              color=GetOnePixel(clone_info->tile,x % clone_info->tile->columns,
                y % clone_info->tile->rows);
            color.opacity*=clone_info->opacity/100.0;
            if ((opacity == TransparentOpacity) ||
                (color.opacity == TransparentOpacity))
              break;
            if (!clone_info->antialias)
              opacity=OpaqueOpacity;
            if (clone_info->opacity == 100.0)
              {
                q->red=alpha*(color.red*opacity+
                  q->red*(OpaqueOpacity-opacity));
                q->green=alpha*(color.green*opacity+
                  q->green*(OpaqueOpacity-opacity));
                q->blue=alpha*(color.blue*opacity+
                  q->blue*(OpaqueOpacity-opacity));
                break;
              }
            q->red=alpha*(color.red*color.opacity+
              q->red*(OpaqueOpacity-color.opacity));
            q->green=alpha*(color.green*color.opacity+
              q->green*(OpaqueOpacity-color.opacity));
            q->blue=alpha*(color.blue*color.opacity+
              q->blue*(OpaqueOpacity-color.opacity));
            q->opacity=alpha*(color.opacity*color.opacity+
              q->opacity*(OpaqueOpacity-color.opacity));
            break;
          }
          default:
            break;
        }
        opacity=InsidePrimitive(primitive_info,clone_info,&target,False,image);
        color=clone_info->stroke;
        color.opacity*=clone_info->opacity/100.0;
        if ((opacity == TransparentOpacity) ||
            (color.opacity == TransparentOpacity))
          {
            q++;
            continue;
          }
        if (!clone_info->antialias)
          opacity=OpaqueOpacity;
        if (clone_info->opacity == 100.0)
          {
            q->red=alpha*(color.red*opacity+
              q->red*(OpaqueOpacity-opacity));
            q->green=alpha*(color.green*opacity+
              q->green*(OpaqueOpacity-opacity));
            q->blue=alpha*(color.blue*opacity+
              q->blue*(OpaqueOpacity-opacity));
            q++;
            continue;
          }
        q->red=alpha*(color.red*color.opacity+
          q->red*(OpaqueOpacity-color.opacity));
        q->green=alpha*(color.green*color.opacity+
          q->green*(OpaqueOpacity-color.opacity));
        q->blue=alpha*(color.blue*color.opacity+
          q->blue*(OpaqueOpacity-color.opacity));
        q->opacity=alpha*(color.opacity*color.opacity+
          q->opacity*(OpaqueOpacity-color.opacity));
        q++;
      }
      if (!SyncImagePixels(image))
        break;
      if (QuantumTick(y,image->rows))
        ProgressMonitor(DrawImageText,y,image->rows);
    }
  }
  /*
    Free resources.
  */
  FreeMemory((void **) &primitive_info);
  if (indirection)
    FreeMemory((void **) &primitive);
  DestroyDrawInfo(clone_info);
  if (primitive_type == UndefinedPrimitive)
    ThrowBinaryException(OptionWarning,
      "Non-conforming drawing primitive definition",keyword);
  (void) IsMatteImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e n e r a t e P r i m i t i v e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GeneratePrimitive is a collection of methods for generating graphic
%  primitives such as arcs, ellipses, paths, etc.
%
%
*/
static void GenerateArc(PrimitiveInfo *primitive_info,PointInfo start,
  PointInfo end,PointInfo arc)
{
  double
    angle,
    x,
    y;

  PointInfo
    pixel,
    scale;

  register PrimitiveInfo
    *p;

  p=primitive_info;
  scale.x=fabs((end.x-start.x)/2.0);
  scale.y=fabs((end.y-start.y)/2.0);
  x=Min(start.x,end.x)+scale.x;
  y=Min(start.y,end.y)+scale.y;
  for (angle=(arc.x+1.0); angle <= arc.y; angle+=1.0)
  {
    pixel.x=x+scale.x*cos(DegreesToRadians(fmod(angle,360.0)));
    pixel.y=y-scale.y*sin(DegreesToRadians(fmod(angle,360.0)));
    p->primitive=primitive_info->primitive;
    p->coordinates=0;
    p->pixel=pixel;
    p++;
    primitive_info->coordinates++;
  }
  pixel.x=(start.x+end.x)/2.0;
  pixel.y=(start.y+end.y)/2.0;
  p->primitive=primitive_info->primitive;
  p->coordinates=0;
  p->pixel=pixel;
  p++;
  primitive_info->coordinates++;
  p->primitive=primitive_info->primitive;
  p->coordinates=0;
  p->pixel=primitive_info->pixel;
  primitive_info->coordinates++;
}

static void GenerateBezier(PrimitiveInfo *primitive_info)
{
  double
    alpha,
    *coefficients,
    weight;

  PointInfo
    last,
    pixel,
    *points;

  register int
    i,
    j;

  register PrimitiveInfo
    *p;

  unsigned
    control_points;

  /*
    Allocate coeficients.
  */
  p=primitive_info;
  control_points=BezierQuantum*p->coordinates;
  coefficients=(double *)
    AllocateMemory(p->coordinates*sizeof(double));
  points=(PointInfo *) AllocateMemory(control_points*sizeof(PointInfo));
  if ((coefficients == (double *) NULL) || (points == (PointInfo *) NULL))
    MagickError(ResourceLimitWarning,"Unable to draw image",
      "Memory allocation failed");
  /*
    Compute bezier points.
  */
  last=primitive_info[primitive_info->coordinates-1].pixel;
  weight=0.0;
  for (i=0; i < p->coordinates; i++)
    coefficients[i]=Permutate(p->coordinates-1,i);
  for (i=0; i < (int) control_points; i++)
  {
    p=primitive_info;
    pixel.x=0;
    pixel.y=0;
    alpha=pow(1.0-weight,primitive_info->coordinates-1);
    for (j=0; j < primitive_info->coordinates; j++)
    {
      pixel.x+=alpha*coefficients[j]*p->pixel.x;
      pixel.y+=alpha*coefficients[j]*p->pixel.y;
      alpha*=weight/(1.0-weight);
      p++;
    }
    points[i]=pixel;
    weight+=1.0/BezierQuantum/primitive_info->coordinates;
  }
  /*
    Bezier curves are just short segmented polys.
  */
  p=primitive_info;
  for (i=0; i < (int) control_points; i++)
  {
    pixel=points[i];
    p->primitive=primitive_info->primitive;
    p->coordinates=0;
    p->pixel=pixel;
    p++;
  }
  p->primitive=primitive_info->primitive;
  p->coordinates=0;
  p->pixel=last;
  p++;
  primitive_info->coordinates=p-primitive_info;
  FreeMemory((void **) &points);
  FreeMemory((void **) &coefficients);
}

static double GenerateCircle(PrimitiveInfo *primitive_info,PointInfo start,
  PointInfo end)
{
  double
    alpha,
    beta;

  register PrimitiveInfo
    *p,
    *q;

  p=primitive_info;
  p->coordinates=2;
  p->pixel=start;
  q=p+1;
  q->primitive=p->primitive;
  q->pixel=end;
  alpha=q->pixel.x-p->pixel.x;
  beta=q->pixel.y-p->pixel.y;
  return(sqrt(alpha*alpha+beta*beta));
}

static void GenerateEllipse(PrimitiveInfo *primitive_info,PointInfo start,
  PointInfo end,PointInfo degrees)
{
  double
    angle;

  PointInfo
    pixel;

  register PrimitiveInfo
    *p;

  /*
    Arc's are just short segmented polys.
  */
  p=primitive_info;
  end.x/=2.0;
  end.y/=2.0;
  while (degrees.y < degrees.x)
    degrees.y+=360.0;
  for (angle=(degrees.x+1.0); angle <= degrees.y; angle+=1.0)
  {
    pixel.x=cos(DegreesToRadians(fmod(angle,360.0)))*end.x+start.x;
    pixel.y=sin(DegreesToRadians(fmod(angle,360.0)))*end.y+start.y;
    p->primitive=primitive_info->primitive;
    p->coordinates=0;
    p->pixel=pixel;
    p++;
    primitive_info->coordinates++;
  }
}

static void GenerateLine(PrimitiveInfo *primitive_info,PointInfo start,
  PointInfo end)
{
  register PrimitiveInfo
    *p,
    *q;

  p=primitive_info;
  p->coordinates=2;
  p->pixel=start;
  q=p+1;
  q->primitive=p->primitive;
  q->pixel=end;
}

static void GeneratePath(PrimitiveInfo *primitive_info,const char *path)
{
  char
    *p;

  double
    reflect,
    x,
    y;

  int
    attribute;

  PointInfo
    last,
    pixels[4],
    point,
    reflected_pixels[4],
    start;

  register int
    i;

  register PrimitiveInfo
    *q;

  point.x=0;
  point.y=0;
  p=(char *) path;
  q=primitive_info;
  q->primitive=PolygonPrimitive;
  while (*p != '\0')
  {
    while (isspace((int) *p))
      p++;
    attribute=(*p++);
    q->primitive=primitive_info->primitive;
    switch (attribute)
    {
      case 'a':
      case 'A':
      {
        double
          large_arc,
          rotate,
          sweep;

        PointInfo
          arc;

        /*
          Compute arc points.
        */
        arc.x=strtod(p,&p);
        if (*p == ',')
          p++;
        arc.y=strtod(p,&p);
        if (*p == ',')
          p++;
        rotate=strtod(p,&p);
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
        last.x=attribute == 'A' ? x : point.x+x;
        last.y=attribute == 'A' ? y : point.y+y;
        GenerateArc(q,point,last,arc);
        q+=q->coordinates;
        break;
      }
      case 'c':
      case 'C':
      {
        /*
          Compute bezier points.
        */
        pixels[0]=point;
        for (i=1; i < 4; i++)
        {
          x=strtod(p,&p);
          if (*p == ',')
            p++;
          y=strtod(p,&p);
          if (*p == ',')
            p++;
          last.x=attribute == 'C' ? x : point.x+x;
          last.y=attribute == 'C' ? y : point.y+y;
          pixels[i]=last;
        }
        point=last;
        for (i=0; i < 4; i++)
          (q+i)->pixel=pixels[i];
        q->coordinates=4;
        GenerateBezier(q);
        q+=q->coordinates;
        break;
      }
      case 'H':
      case 'h':
      {
        x=strtod(p,&p);
        point.x=attribute == 'H' ? x: point.x+x;
        GeneratePoint(q,point);
        q+=q->coordinates;
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
          GeneratePoint(q,point);
          q+=q->coordinates;
        } while (IsGeometry(p));
        break;
      }
      case 'M':
      case 'm':
      {
        point.x=strtod(p,&p);
        if (*p == ',')
          p++;
        point.y=strtod(p,&p);
        GeneratePoint(q,point);
        q+=q->coordinates;
        start=point;
        break;
      }
      case 'q':
      case 'Q':
      {
        /*
          Compute bezier points.
        */
        pixels[0]=point;
        for (i=1; i < 3; i++)
        {
          x=strtod(p,&p);
          if (*p == ',')
            p++;
          y=strtod(p,&p);
          if (*p == ',')
            p++;
          last.x=attribute == 'Q' ? x : point.x+x;
          last.y=attribute == 'Q' ? y : point.y+y;
          pixels[i]=last;
        }
        point=last;
        for (i=0; i < 3; i++)
          (q+i)->pixel=pixels[i];
        q->coordinates=3;
        GenerateBezier(q);
        q+=q->coordinates;
        break;
      }
      case 's':
      case 'S':
      {
        /*
          Compute bezier points.
        */
        reflected_pixels[0]=pixels[3];
        for (i=2; i < 4; i++)
        {
          x=strtod(p,&p);
          if (*p == ',')
            p++;
          y=strtod(p,&p);
          if (*p == ',')
            p++;
          last.x=attribute == 'S' ? x : point.x+x;
          last.y=attribute == 'S' ? y : point.y+y;
          reflected_pixels[i]=last;
        }
        point=last;
        /*
          Reflect the x control pixel.
        */
        reflect=pixels[3].x-pixels[0].x;
        if (!reflect)
          reflect=1.0;
        reflect=(pixels[3].x-pixels[2].x)/reflect;
        reflect=reflect*(reflected_pixels[3].x-pixels[3].x);
        reflected_pixels[1].x=reflect+pixels[3].x;
        /*
          Reflect the y control pixel.
        */
        reflect=pixels[3].y-pixels[0].y;
        if (!reflect)
          reflect=1.0;
        reflect=(pixels[3].y-pixels[2].y)/reflect;
        reflect=reflect*(reflected_pixels[3].y-pixels[3].y);
        reflected_pixels[1].y=reflect+pixels[3].y;
        for (i=0; i < 4; i++)
        {
          pixels[i]=reflected_pixels[i];
          (q+i)->pixel=pixels[i];
        }
        q->coordinates=4;
        GenerateBezier(q);
        q+=q->coordinates;
        break;
      }
      case 't':
      case 'T':
      {
        /*
          Compute bezier points.
        */
        reflected_pixels[0]=pixels[2];
        for (i=2; i < 3; i++)
        {
          x=strtod(p,&p);
          if (*p == ',')
            p++;
          y=strtod(p,&p);
          if (*p == ',')
            p++;
          last.x=attribute == 'T' ? x : point.x+x;
          last.y=attribute == 'T' ? y : point.y+y;
          reflected_pixels[i]=last;
        }
        point=last;
        /*
          Reflect the x control pixel.
        */
        reflect=pixels[2].x-pixels[0].x;
        if (!reflect)
          reflect=1.0;
        reflect=(pixels[2].x-pixels[1].x)/reflect;
        reflect=reflect*(reflected_pixels[2].x-pixels[2].x);
        reflected_pixels[1].x=reflect+pixels[2].x;
        /*
          Reflect the y control pixel.
        */
        reflect=pixels[2].y-pixels[0].y;
        if (!reflect)
          reflect=1.0;
        reflect=(pixels[2].y-pixels[1].y)/reflect;
        reflect=reflect*(reflected_pixels[2].y-pixels[2].y);
        reflected_pixels[1].y=reflect+pixels[2].y;
        for (i=0; i < 3; i++)
        {
          pixels[i]=reflected_pixels[i];
          (q+i)->pixel=pixels[i];
        }
        q->coordinates=3;
        GenerateBezier(q);
        q+=q->coordinates;
        break;
      }
      case 'v':
      case 'V':
      {
        y=strtod(p,&p);
        point.y=attribute == 'V' ? y : point.y+y;
        GeneratePoint(q,point);
        q+=q->coordinates;
        break;
      }
      case 'z':
      case 'Z':
      {
        point=start;
        GeneratePoint(q,point);
        q+=q->coordinates;
        break;
      }
      default:
      {
        if (isalpha((int) attribute))
          (void) fprintf(stderr,"attribute not implemented: %c\n",attribute);
        break;
      }
    }
  }
  primitive_info->primitive=PolygonPrimitive;
  primitive_info->coordinates=q-primitive_info;
}

static void GeneratePoint(PrimitiveInfo *primitive_info,PointInfo start)
{
  primitive_info->primitive=PointPrimitive;
  primitive_info->coordinates=1;
  primitive_info->pixel=start;
}

static void GenerateRectangle(PrimitiveInfo *primitive_info,PointInfo start,
  PointInfo end)
{
  PointInfo
    point;

  register PrimitiveInfo
    *q;

  q=primitive_info;
  GeneratePoint(q,start);
  q+=q->coordinates;
  point.x=start.x;
  point.y=end.y;
  GeneratePoint(q,point);
  q+=q->coordinates;
  GeneratePoint(q,end);
  q+=q->coordinates;
  point.x=end.x;
  point.y=start.y;
  GeneratePoint(q,point);
  q+=q->coordinates;
  GeneratePoint(q,start);
  q+=q->coordinates;
  primitive_info->primitive=PolygonPrimitive;
  primitive_info->coordinates=q-primitive_info;
}

static void GenerateRoundRectangle(PrimitiveInfo *primitive_info,
  PointInfo start,PointInfo end,PointInfo arc)
{
  PointInfo
    degrees,
    u,
    v;

  register PrimitiveInfo
    *p;

  p=primitive_info;
  start.x-=end.x/2;
  start.y-=end.y/2;
  end.x+=start.x-1;
  end.y+=start.y-1;
  u.x=start.x+arc.x;
  u.y=start.y;
  v.x=end.x-arc.x;
  v.y=start.y;
  GenerateLine(p,u,v);
  p+=p->coordinates;
  p->primitive=primitive_info->primitive;
  u.x= end.x-arc.x;
  u.y=start.y+arc.y;
  v.x=2.0*arc.x;
  v.y=2.0*arc.y;
  degrees.x=270.0;
  degrees.y=360.0;
  GenerateEllipse(p,u,v,degrees);
  p+=p->coordinates;
  p->primitive=primitive_info->primitive;
  u.x=end.x;
  u.y=start.y+arc.y;
  v.x=end.x;
  v.y=end.y-arc.y;
  GenerateLine(p,u,v);
  p+=p->coordinates;
  p->primitive=primitive_info->primitive;
  u.x=end.x-arc.x;
  u.y=end.y-arc.y;
  v.x=2.0*arc.x;
  v.y=2.0*arc.y;
  degrees.x=0.0;
  degrees.y=90.0;
  GenerateEllipse(p,u,v,degrees);
  p+=p->coordinates;
  p->primitive=primitive_info->primitive;
  u.x=end.x-arc.x;
  u.y=end.y;
  v.x=start.x+arc.x;
  v.y=end.y;
  GenerateLine(p,u,v);
  u.x=start.x+arc.x;
  u.y=end.y-arc.y;
  v.x=2.0*arc.x;
  v.y=2.0*arc.y;
  degrees.x=90.0;
  degrees.y=180.0;
  GenerateEllipse(p,u,v,degrees);
  p+=p->coordinates;
  p->primitive=primitive_info->primitive;
  u.x=start.x;
  u.y=end.y-arc.y;
  v.x=start.x;
  v.y=start.y+arc.y;
  GenerateLine(p,u,v);
  p+=p->coordinates;
  p->primitive=primitive_info->primitive;
  u.x=start.x+arc.x;
  u.y=start.y+arc.y;
  v.x=2.0*arc.x;
  v.y=2.0*arc.y;
  degrees.x=180.0;
  degrees.y=270.0;
  GenerateEllipse(p,u,v,degrees);
  p+=p->coordinates;
  p->primitive=primitive_info->primitive;
  u.x=start.x+arc.x;
  u.y=start.y;
  GeneratePoint(p,u);
  p+=p->coordinates;
  p->primitive=primitive_info->primitive;
  primitive_info->primitive=PolygonPrimitive;
  primitive_info->coordinates=p-primitive_info;
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
  assert(draw_info != (DrawInfo *) NULL);
  draw_info->primitive=(char *) NULL;
  draw_info->font=AllocateString(image_info->font);
  draw_info->antialias=image_info->antialias;
  draw_info->gravity=NorthWestGravity;
  draw_info->opacity=100.0;
  draw_info->linewidth=1.0;
  draw_info->pointsize=image_info->pointsize;
  draw_info->angle=0.0;
  for (i=0; i < 6; i++)
    draw_info->affine[i]=(i == 0) || (i == 3) ? 1.0 : 0.0;
  draw_info->fill=image_info->fill;
  draw_info->stroke=image_info->stroke;
  (void) QueryColorDatabase("none",&draw_info->box);
  draw_info->border_color=image_info->border_color;
  draw_info->decorate=NoDecoration;
  draw_info->tile=(Image *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n s i d e P r i m i t i v e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InsidePrimitive returns the opacity of the pen at the (x,y) position
%  of the image.  The opacity is OpaqueOpacity if the (x,y) position is within
%  the primitive as defined in primitive_info.  A value less than fully opaque
%  and greater than fully transparent is returned for a primitive edge pixel
%  to allow for anti-aliasing.  Otherwise fully transparent is returned.
%
%  Rick Mabry provided the algorithms for anti-aliased primitives.
%
%  The format of the InsidePrimitive method is:
%
%      double InsidePrimitive(PrimitiveInfo *primitive_info,
%        const DrawInfo *draw_info,const PointInfo *pixel,const int fill,
%        Image *image)
%
%  A description of each parameter follows:
%
%    o opacity:  Method InsidePrimitive returns a pen opacity associated with
%      the (x,y) position of the image.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%    o draw_info: Specifies a pointer to a DrawInfo structure.
%
%    o target: PointInfo representing the (x,y) location in the image.
%
%    o fill: only return True if the pixel is in the interior of the
%      specified graphics primitive.
%
%    o image: The address of a structure of type Image.
%
%
%
*/

static double DistanceToLine(const PointInfo *pixel,const PointInfo *p,
  const PointInfo *q)
{
  double
    dot_product,
    gamma,
    phi;

  register double
    alpha,
    beta;

  alpha=pixel->x-p->x;
  beta=pixel->y-p->y;
  dot_product=alpha*(q->x-p->x)+beta*(q->y-p->y);
  if (dot_product <= 0)
    return(alpha*alpha+beta*beta);
  phi=(q->x-p->x)*(q->x-p->x)+(q->y-p->y)*(q->y-p->y);
  gamma=dot_product*dot_product/phi;
  if (gamma <= phi)
    return(alpha*alpha+beta*beta-gamma);
  alpha=pixel->x-q->x;
  beta=pixel->y-q->y;
  return(alpha*alpha+beta*beta);
}

static double PixelOnLine(const PointInfo *pixel,const PointInfo *p,
  const PointInfo *q,const double mid,const double opacity)
{
  register double
    alpha,
    distance;

  if ((mid == 0) || (opacity == OpaqueOpacity))
    return(opacity);
  if ((p->x == q->x) && (p->y == q->y))
    return((pixel->x == p->x) && (pixel->y == p->y) ? OpaqueOpacity : opacity);
  distance=DistanceToLine(pixel,p,q);
  alpha=mid-0.5;
  if (distance <= (alpha*alpha))
    return((double) OpaqueOpacity);
  alpha=mid+0.5;
  if (distance <= (alpha*alpha))
    {
      alpha=sqrt(distance)-mid-0.5;
      return(Max(opacity,OpaqueOpacity*alpha*alpha));
    }
  return(opacity);
}

static double InsidePrimitive(PrimitiveInfo *primitive_info,
  const DrawInfo *draw_info,const PointInfo *pixel,const int fill,Image *image)
{
  PixelPacket
    border_color;

  double
    alpha,
    beta,
    distance,
    mid,
    opacity,
    radius;

  register int
    i;

  register PrimitiveInfo
    *p,
    *q;

  PixelPacket
    target;

  assert(primitive_info != (PrimitiveInfo *) NULL);
  assert(draw_info != (DrawInfo *) NULL);
  assert(image != (Image *) NULL);
  opacity=TransparentOpacity;
  mid=draw_info->linewidth/2.0;
  p=primitive_info;
  while (p->primitive != UndefinedPrimitive)
  {
    q=p+p->coordinates-1;
    switch (p->primitive)
    {
      case PointPrimitive:
      default:
      {
        if ((pixel->x == (int) (p->pixel.x+0.5)) &&
            (pixel->y == (int) (p->pixel.y+0.5)))
          opacity=OpaqueOpacity;
        break;
      }
      case LinePrimitive:
      {
        opacity=PixelOnLine(pixel,&p->pixel,&q->pixel,mid,opacity);
        break;
      }
      case CirclePrimitive:
      {
        alpha=p->pixel.x-pixel->x;
        beta=p->pixel.y-pixel->y;
        distance=sqrt(alpha*alpha+beta*beta);
        alpha=p->pixel.x-q->pixel.x;
        beta=p->pixel.y-q->pixel.y;
        radius=sqrt(alpha*alpha+beta*beta);
        if (fill)
          {
            if (distance <= (radius-1.0))
              opacity=OpaqueOpacity;
            else
              if (distance < (radius+1.0))
                {
                  alpha=(radius-distance+1.0)/2.0;
                  opacity=Max(opacity,OpaqueOpacity*alpha*alpha);
                }
            break;
          }
        if (fabs(distance-radius) < (mid+0.5))
          {
            if (fabs(distance-radius) <= (mid-0.5))
              opacity=OpaqueOpacity;
            else
              {
                alpha=mid-fabs(distance-radius)+0.5;
                opacity=Max(opacity,OpaqueOpacity*alpha*alpha);
              }
          }
        break;
      }
      case ArcPrimitive:
      case BezierPrimitive:
      case EllipsePrimitive:
      case PathPrimitive:
      case PolylinePrimitive:
      case PolygonPrimitive:
      case RectanglePrimitive:
      case RoundRectanglePrimitive:
      {
        double
          minimum_distance;

        int
          crossing,
          crossings;

        PrimitiveInfo
          *primitive_info;

        unsigned int
          poly_opacity;

        if (!fill)
          {
            poly_opacity=TransparentOpacity;
            for ( ; (p < q) && (poly_opacity != OpaqueOpacity); p++)
              poly_opacity=PixelOnLine(pixel,&p->pixel,&(p+1)->pixel,mid,
                Max(opacity,poly_opacity));
            opacity=Max(opacity,poly_opacity);
            break;
          }
        primitive_info=p;
        crossings=0;
        if ((pixel->y < q->pixel.y) != (pixel->y < p->pixel.y))
          {
            crossing=pixel->x < q->pixel.x;
            if (crossing != (pixel->x < p->pixel.x))
              crossings+=pixel->x < (q->pixel.x-(q->pixel.y-pixel->y)*
                (p->pixel.x-q->pixel.x)/(p->pixel.y-q->pixel.y));
            else
              if (crossing)
                crossings++;
          }
        for (p++; p <= q; p++)
        {
          if (pixel->y < (p-1)->pixel.y)
            {
              while ((p <= q) && (pixel->y < p->pixel.y))
                p++;
              if (p > q)
                break;
              crossing=pixel->x < (p-1)->pixel.x;
              if (crossing != (pixel->x < p->pixel.x))
                crossings+=pixel->x < ((p-1)->pixel.x-((p-1)->pixel.y-pixel->y)*
                  (p->pixel.x-(p-1)->pixel.x)/(p->pixel.y-(p-1)->pixel.y));
              else
                if (crossing)
                  crossings++;
              continue;
            }
          while ((p <= q) && (pixel->y >= p->pixel.y))
            p++;
          if (p > q)
            break;
          crossing=pixel->x < (p-1)->pixel.x;
          if (crossing != (pixel->x < p->pixel.x))
            crossings+=pixel->x < ((p-1)->pixel.x-((p-1)->pixel.y-pixel->y)*
              (p->pixel.x-(p-1)->pixel.x)/(p->pixel.y-(p-1)->pixel.y));
          else
            if (crossing)
              crossings++;
        }
        poly_opacity=TransparentOpacity;
        if (crossings & 0x01)
          poly_opacity=OpaqueOpacity;
        p=primitive_info;
        minimum_distance=DistanceToLine(pixel,&q->pixel,&p->pixel);
        for (p++ ; p <= q; p++)
        {
          distance=DistanceToLine(pixel,&(p-1)->pixel,&p->pixel);
          if (distance < minimum_distance)
            minimum_distance=distance;
        }
        if (minimum_distance < (0.5*0.5))
          {
            if (crossings & 0x01)
              alpha=0.5+sqrt(minimum_distance);
            else
              alpha=0.5-sqrt(minimum_distance);
            poly_opacity=alpha*alpha*OpaqueOpacity;
          }
        opacity=Max(opacity,poly_opacity);
        break;
      }
      case ColorPrimitive:
      {
        switch (p->method)
        {
          case PointMethod:
          default:
          {
            if ((pixel->x != (int) (p->pixel.x+0.5)) ||
                (pixel->y != (int) (p->pixel.y+0.5)))
              break;
            opacity=OpaqueOpacity;
            break;
          }
          case ReplaceMethod:
          {
            PixelPacket
              color;

            PixelPacket
              target;

            target=GetOnePixel(image,(int) p->pixel.x,(int) p->pixel.y);
            color=GetOnePixel(image,(int) pixel->x,(int) pixel->y);
            if (ColorMatch(color,target,(int) image->fuzz))
              opacity=OpaqueOpacity;
            break;
          }
          case FloodfillMethod:
          case FillToBorderMethod:
          {
            if ((pixel->x != (int) (p->pixel.x+0.5)) ||
                (pixel->y != (int) (p->pixel.y+0.5)))
              break;
            target=GetOnePixel(image,(int) p->pixel.x,(int) p->pixel.y);
            if (p->method == FillToBorderMethod)
              {
                border_color=draw_info->border_color;
                target=border_color;
              }
            (void) ColorFloodfillImage(image,draw_info,target,
              (int) pixel->x,(int) pixel->y,p->method);
            break;
          }
          case ResetMethod:
          {
            opacity=OpaqueOpacity;
            break;
          }
        }
        break;
      }
      case MattePrimitive:
      {
        register PixelPacket
          *q;

        if (!image->matte)
          MatteImage(image,OpaqueOpacity);
        switch (p->method)
        {
          case PointMethod:
          default:
          {
            if ((pixel->x != (int) (p->pixel.x+0.5)) ||
                (pixel->y != (int) (p->pixel.y+0.5)))
              break;
            q=GetImagePixels(image,(int) pixel->x,(int) pixel->y,1,1);
            if (q != (PixelPacket *) NULL)
              {
                q->opacity=TransparentOpacity;
                (void) SyncImagePixels(image);
              }
            break;
          }
          case ReplaceMethod:
          {
            PixelPacket
              color;

            PixelPacket
              target;

            color=GetOnePixel(image,(int) p->pixel.x,(int) p->pixel.y);
            target=GetOnePixel(image,(int) pixel->x,(int) pixel->y);
            if (!ColorMatch(color,target,image->fuzz))
              break;
            q=GetImagePixels(image,(int) pixel->x,(int) pixel->y,1,1);
            if (q != (PixelPacket *) NULL)
              q->opacity=TransparentOpacity;
            (void) SyncImagePixels(image);
            break;
          }
          case FloodfillMethod:
          case FillToBorderMethod:
          {
            if ((pixel->x != (int) (p->pixel.x+0.5)) ||
                (pixel->y != (int) (p->pixel.y+0.5)))
              break;
            target=GetOnePixel(image,(int) p->pixel.x,(int) p->pixel.y);
            if (p->method == FillToBorderMethod)
              {
                border_color=draw_info->border_color;
                target=border_color;
              }
            (void) MatteFloodfillImage(image,target,TransparentOpacity,
              (int) pixel->x,(int) pixel->y,p->method);
            break;
          }
          case ResetMethod:
          {
            q=GetImagePixels(image,(int) pixel->x,(int) pixel->y,1,1);
            if (q != (PixelPacket *) NULL)
              {
                q->opacity=OpaqueOpacity;
                (void) SyncImagePixels(image);
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

        register char
          *r;

        if ((pixel->x != (int) (p->pixel.x+0.5)) ||
            (pixel->y != (int) (p->pixel.y+0.5)))
          break;
        if (p->text == (char *) NULL)
          break;
        r=p->text;
        if (*r == '"')
          {
            p->text++;
            for (r++; *r != '\0'; r++)
              if ((*r == '"') && (*(r-1) != '\\'))
                break;
          }
        else
          if (*r == '\'')
            {
              p->text++;
              for (r++; *r != '\0'; r++)
                if ((*r == '\'') && (*(r-1) != '\\'))
                  break;
            }
          else
            for (r++;  *r != '\0'; r++)
              if (isspace((int) *r) && (*(r-1) != '\\') && (*r != '\0'))
                break;
        clone_info=CloneImageInfo((ImageInfo *) NULL);
        annotate=CloneAnnotateInfo(clone_info,(AnnotateInfo *) NULL);
        DestroyImageInfo(clone_info);
        annotate->font=AllocateString(draw_info->font);
        annotate->antialias=draw_info->antialias;
        annotate->pointsize=draw_info->pointsize;
        annotate->degrees=draw_info->angle;
        for (i=0; i < 6; i++)
          annotate->affine[i]=draw_info->affine[i];
        annotate->gravity=draw_info->gravity;
        annotate->decorate=draw_info->decorate;
        annotate->geometry=AllocateString("                                  ");
        annotate->fill=draw_info->fill;
        annotate->stroke=draw_info->stroke;
        annotate->box=draw_info->box;
        annotate->text=(char *) AllocateMemory(r-p->text+1);
        if (annotate->text == (char *) NULL)
          MagickError(ResourceLimitError,"Unable to annotate image",
            "Memory allocation failed");
        (void) strncpy(annotate->text,p->text,r-p->text);
        annotate->text[r-p->text]='\0';
        FormatString(annotate->geometry,"%+f%+f",p->pixel.x,p->pixel.y);
        AnnotateImage(image,annotate);
        DestroyAnnotateInfo(annotate);
        break;
      }
      case ImagePrimitive:
        break;
    }
    if (opacity == OpaqueOpacity)
      return(opacity);
    p=q+1;
  }
  return(opacity);
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
  segment_stack=(SegmentInfo *)
    AllocateMemory(MaxStacksize*sizeof(SegmentInfo));
  if (segment_stack == (SegmentInfo *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Unable to floodfill image",
      image->filename);
  /*
    Push initial segment on stack.
  */
  image->class=DirectClass;
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
      Recolor neighboring pixels.
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
  FreeMemory((void **) &segment_stack);
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
%  Method OpaqueImage changes the color of an opaque pixel to the pen color.
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
  switch (image->class)
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
          ProgressMonitor(OpaqueImageText,y,image->rows);
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
          ProgressMonitor(OpaqueImageText,i,image->colors);
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
      ProgressMonitor(TransparentImageText,y,image->rows);
  }
  return(True);
}
