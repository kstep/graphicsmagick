/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%               DDDD   RRRR    AAA   W   W  IIIII  N   N    GGGG              %
%               D   D  R   R  A   A  W   W    I    NN  N   G                  %
%               D   D  RRRR   AAAAA  W   W    I    N N N   G  GG              %
%               D   D  R R    A   A  W W W    I    N  NN   G   G              %
%               DDDD   R  R   A   A   W W   IIIII  N   N    GGG               %
%                                                                             %
%                         W   W   AAA   N   N  DDDD                           %
%                         W   W  A   A  NN  N  D   D                          %
%                         W W W  AAAAA  N N N  D   D                          %
%                         WW WW  A   A  N  NN  D   D                          %
%                         W   W  A   A  N   N  DDDD                           %
%                                                                             %
%                                                                             %
%                   ImageMagick Image Vector Drawing Methods                  %
%                                                                             %
%                              Software Design                                %
%                              Bob Friesenhahn                                %
%                                March 2002                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2003 ImageMagick Studio, a non-profit organization dedicated %
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
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/error.h"
#include "magick/gem.h"
#include "magick/list.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "wand/magick_wand.h"
#include "wand/magick_compat.h"

/*
  Define declarations.
*/
#define DRAW_BINARY_IMPLEMENTATION 0

#define CurrentContext  (drawing_wand->graphic_context[drawing_wand->index])
#define WandColorMatch(p,q) (((p)->red == (q)->red) && \
  ((p)->green == (q)->green) && ((p)->blue == (q)->blue) && \
  ((p)->opacity == (q)->opacity))

/*
  Typedef declarations.
*/
typedef enum
{
  PathDefaultOperation,
  PathCloseOperation,                        /* Z|z (none) */
  PathCurveToOperation,                      /* C|c (x1 y1 x2 y2 x y)+ */
  PathCurveToQuadraticBezierOperation,       /* Q|q (x1 y1 x y)+ */
  PathCurveToQuadraticBezierSmoothOperation, /* T|t (x y)+ */
  PathCurveToSmoothOperation,                /* S|s (x2 y2 x y)+ */
  PathEllipticArcOperation,                  /* A|a (rx ry x-axis-rotation large-arc-flag sweep-flag x y)+ */
  PathLineToHorizontalOperation,             /* H|h x+ */
  PathLineToOperation,                       /* L|l (x y)+ */
  PathLineToVerticalOperation,               /* V|v y+ */
  PathMoveToOperation                        /* M|m (x y)+ */
} PathOperation;

typedef enum
{
  DefaultPathMode,
  AbsolutePathMode,
  RelativePathMode
} PathMode;

struct _DrawingWand
{
  /* Support structures */
  Image
    *image;

  /* MVG output string and housekeeping */
  char
    *mvg;               /* MVG data */

  size_t
    mvg_alloc,          /* total allocated memory */
    mvg_length;         /* total MVG length */

  unsigned int
    mvg_width;          /* current line length */

  /* Pattern support */
  char
    *pattern_id;

  RectangleInfo
    pattern_bounds;

  size_t
    pattern_offset;

  /* Graphic drawing_wand */
  unsigned int
    index;              /* array index */

  DrawInfo
    **graphic_context;

  int
    filter_off;         /* true if not filtering attributes */

  /* Pretty-printing depth */
  unsigned int
    indent_depth;       /* number of left-hand pad characters */

  /* Path operation support */
  PathOperation
    path_operation;

  PathMode
    path_mode;

  /* Structure unique signature */
  unsigned long
    signature;
};

/* Vector table for invoking subordinate renderers */
struct _DrawVTable
{
  void (*DestroyDrawingWand) (DrawingWand *drawing_wand);
  void (*DrawAnnotation)(DrawingWand *,const double,const double,
    const unsigned char *);
  void (*DrawArc)(DrawingWand *,const double,const double,const double,
    const double,const double,const double);
  void (*DrawBezier)(DrawingWand *,const size_t,const PointInfo *);
  void (*DrawCircle)(DrawingWand *,const double,const double,const double,
    const double);
  void (*DrawColor)(DrawingWand *,const double,const double,const PaintMethod);
  void (*DrawComment)(DrawingWand *,const char *);
  void (*DrawDestroyContext) (DrawContext context);
  void (*DrawEllipse)(DrawingWand *,const double,const double,const double,
    const double,const double,const double);
  void (*DrawComposite)(DrawingWand *,const CompositeOperator,const double,
    const double,const double,const double,const Image *);
  void (*DrawLine)(DrawingWand *,const double,const double,const double,
    const double);
  void (*DrawMatte)(DrawingWand *,const double,const double,const PaintMethod);
  void (*DrawPathClose)(DrawingWand *);
  void (*DrawPathCurveToAbsolute)(DrawingWand *,const double,const double,
    const double,const double,const double,const double);
  void (*DrawPathCurveToRelative)(DrawingWand *,const double,const double,
    const double,const double,const double,const double);
  void (*DrawPathCurveToQuadraticBezierAbsolute)(DrawingWand *,const double,
    const double,const double,const double);
  void (*DrawPathCurveToQuadraticBezierRelative)(DrawingWand *,const double,
    const double,const double,const double);
  void (*DrawPathCurveToQuadraticBezierSmoothAbsolute)(DrawingWand *,
    const double,const double);
  void (*DrawPathCurveToQuadraticBezierSmoothRelative)(DrawingWand *,
    const double,const double);
  void (*DrawPathCurveToSmoothAbsolute)(DrawingWand *,const double,
    const double,const double,const double);
  void (*DrawPathCurveToSmoothRelative)(DrawingWand *,const double,
    const double,const double,const double);
  void (*DrawPathEllipticArcAbsolute)(DrawingWand *,const double,const double,
    const double,unsigned int,unsigned int,const double,const double);
  void (*DrawPathEllipticArcRelative)(DrawingWand *,const double,const double,
    const double,unsigned int,unsigned int,const double,const double);
  void (*DrawPathFinish)(DrawingWand *);
  void (*DrawPathLineToAbsolute)(DrawingWand *,const double,const double);
  void (*DrawPathLineToRelative)(DrawingWand *,const double,const double);
  void (*DrawPathLineToHorizontalAbsolute)(DrawingWand *,const double);
  void (*DrawPathLineToHorizontalRelative)(DrawingWand *,const double);
  void (*DrawPathLineToVerticalAbsolute)(DrawingWand *,const double);
  void (*DrawPathLineToVerticalRelative)(DrawingWand *,const double);
  void (*DrawPathMoveToAbsolute)(DrawingWand *,const double,const double);
  void (*DrawPathMoveToRelative)(DrawingWand *,const double,const double);
  void (*DrawPathStart)(DrawingWand *);
  void (*DrawPoint)(DrawingWand *,const double,const double);
  void (*DrawPolygon)(DrawingWand *,const size_t,const PointInfo *);
  void (*DrawPolyline)(DrawingWand *,const size_t,const PointInfo *);
  void (*DrawPopClipPath)(DrawingWand *);
  void (*DrawPopDefs)(DrawingWand *);
  void (*DrawPopGraphicContext)(DrawingWand *);
  void (*DrawPopPattern)(DrawingWand *);
  void (*DrawPushClipPath)(DrawingWand *,const char *);
  void (*DrawPushDefs)(DrawingWand *);
  void (*DrawPushGraphicContext)(DrawingWand *);
  void (*DrawPushPattern)(DrawingWand *,const char *,const double,const double,
    const double,const double);
  void (*DrawRectangle)(DrawingWand *,const double,const double,const double,
    const double);
  void (*DrawRoundRectangle)(DrawingWand *,double,double,double,double,
    double,double);
  void (*DrawAffine)(DrawingWand *,const AffineMatrix *);
  void (*DrawSetClipPath)(DrawingWand *,const char *);
  void (*DrawSetClipRule)(DrawingWand *,const FillRule);
  void (*DrawSetClipUnits)(DrawingWand *,const ClipPathUnits);
  void (*DrawSetFillColor)(DrawingWand *,const PixelWand *);
  void (*DrawSetFillOpacity)(DrawingWand *,const double);
  void (*DrawSetFillRule)(DrawingWand *,const FillRule);
  void (*DrawSetFillPatternURL)(DrawingWand *,const char *);
  void (*DrawSetFont)(DrawingWand *,const char *);
  void (*DrawSetFontFamily)(DrawingWand *,const char *);
  void (*DrawSetFontSize)(DrawingWand *,const double);
  void (*DrawSetFontStretch)(DrawingWand *,const StretchType);
  void (*DrawSetFontStyle)(DrawingWand *,const StyleType);
  void (*DrawSetFontWeight)(DrawingWand *,const unsigned long);
  void (*DrawSetGravity)(DrawingWand *,const GravityType);
  void (*DrawRotate)(DrawingWand *,const double);
  void (*DrawScale)(DrawingWand *,const double,const double);
  void (*DrawSkewX)(DrawingWand *,const double);
  void (*DrawSkewY)(DrawingWand *,const double);
  void (*DrawSetStrokeAntialias)(DrawingWand *,const unsigned int);
  void (*DrawSetStrokeColor)(DrawingWand *,const PixelWand *);
  void (*DrawSetStrokeDashArray)(DrawingWand *,const double *);
  void (*DrawSetStrokeDashOffset)(DrawingWand *,const double);
  void (*DrawSetStrokeLineCap)(DrawingWand *,const LineCap);
  void (*DrawSetStrokeLineJoin)(DrawingWand *,const LineJoin);
  void (*DrawSetStrokeMiterLimit)(DrawingWand *,const unsigned long);
  void (*DrawSetStrokeOpacity)(DrawingWand *,const double);
  void (*DrawSetStrokePatternURL)(DrawingWand *,const char *);
  void (*DrawSetStrokeWidth)(DrawingWand *,const double);
  void (*DrawSetTextAntialias)(DrawingWand *,const unsigned int);
  void (*DrawSetTextDecoration)(DrawingWand *,const DecorationType);
  void (*DrawSetTextUnderColor)(DrawingWand *,const PixelWand *);
  void (*DrawTranslate)(DrawingWand *,const double,const double);
  void (*DrawSetViewbox)(DrawingWand *,unsigned long,unsigned long,
    unsigned long,unsigned long);
};

/*
  Forward declarations.
*/
static int
  MvgPrintf(DrawingWand *drawing_wand, const char *format, ...)
#if defined(__GNUC__)
__attribute__ ((format (printf, 2, 3)))
#endif
,
  MvgAutoWrapPrintf(DrawingWand *drawing_wand, const char *format, ...)
#if defined(__GNUC__)
__attribute__ ((format (printf, 2, 3)))
#endif
;
static void
  MvgAppendColor(DrawingWand *drawing_wand, const PixelPacket *color);


/* "Printf" for MVG commands */
static int MvgPrintf(DrawingWand *drawing_wand,const char *format,...)
{
  const size_t
    alloc_size=20*MaxTextExtent; /* 40K */

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->mvg == (char*) NULL)
    {
      drawing_wand->mvg=(char *) AcquireMagickMemory(alloc_size);
      if ( drawing_wand->mvg == (char*) NULL )
        {
          ThrowException3(&drawing_wand->image->exception,
            ResourceLimitError,MemoryAllocationFailed,UnableToDrawOnImage);
          return(-1);
        }

      drawing_wand->mvg_alloc=alloc_size;
      drawing_wand->mvg_length=0;
      if (drawing_wand->mvg == 0)
        {
          ThrowException3(&drawing_wand->image->exception,
            ResourceLimitError,MemoryAllocationFailed,UnableToDrawOnImage);
          return(-1);
        }
    }
  if (drawing_wand->mvg_alloc < (drawing_wand->mvg_length + MaxTextExtent * 10))
    {
      size_t realloc_size=drawing_wand->mvg_alloc + alloc_size;

      drawing_wand->mvg=(char *)
        ResizeMagickMemory(drawing_wand->mvg,realloc_size);
      if (drawing_wand->mvg == NULL)
        {
          ThrowException3(&drawing_wand->image->exception,
            ResourceLimitError,MemoryAllocationFailed,UnableToDrawOnImage);
          return -1;
        }
      drawing_wand->mvg_alloc=realloc_size;
    }
  {
    int
      formatted_length;

    va_list
      argp;

    while (drawing_wand->mvg_width < drawing_wand->indent_depth)
    {
      drawing_wand->mvg[drawing_wand->mvg_length]=' ';
      drawing_wand->mvg_length++;
      drawing_wand->mvg_width++;
    }
    drawing_wand->mvg[drawing_wand->mvg_length]=0;
    va_start(argp, format);
#if defined(HAVE_VSNPRINTF)
    formatted_length=vsnprintf(drawing_wand->mvg+drawing_wand->mvg_length,
      drawing_wand->mvg_alloc-drawing_wand->mvg_length-1,format,argp);
#else
    formatted_length=vsprintf(drawing_wand->mvg+drawing_wand->mvg_length,
      format,argp);
#endif
    va_end(argp);
    if (formatted_length < 0)
      {
        ThrowException(&drawing_wand->image->exception,DrawError,
          UnableToPrint,format);
      }
    else
      {
        drawing_wand->mvg_length+=formatted_length;
        drawing_wand->mvg_width+=formatted_length;
      }
    drawing_wand->mvg[drawing_wand->mvg_length]=0;
    if ((drawing_wand->mvg_length > 1) &&
        (drawing_wand->mvg[drawing_wand->mvg_length-1] == '\n'))
      drawing_wand->mvg_width=0;
    assert((drawing_wand->mvg_length+1) < drawing_wand->mvg_alloc);
    return formatted_length;
  }
}

static int MvgAutoWrapPrintf(DrawingWand *drawing_wand,const char *format,...)
{
  char
    buffer[MaxTextExtent];

  int
    formatted_length;

  va_list
    argp;

  va_start(argp,format);
#if defined(HAVE_VSNPRINTF)
  formatted_length=vsnprintf(buffer,sizeof(buffer)-1,format,argp);
#else
  formatted_length=vsprintf(buffer,format,argp);
#endif
  va_end(argp);
  *(buffer+sizeof(buffer)-1)=0;
  if (formatted_length < 0)
    {
    ThrowException(&drawing_wand->image->exception,DrawError,
      UnableToPrint,format);
    }
  else
    {
      if (((drawing_wand->mvg_width + formatted_length) > 78) &&
          (buffer[formatted_length-1] != '\n'))
        MvgPrintf(drawing_wand, "\n");
      MvgPrintf(drawing_wand,"%s",buffer);
    }
  return(formatted_length);
}

static void MvgAppendColor(DrawingWand *drawing_wand, const PixelPacket *color)
{
  if (color->red == 0 && color->green == 0 && color->blue == 0 &&
     color->opacity == TransparentOpacity)
    MvgPrintf(drawing_wand,"none");
  else
    {
      char
        tuple[MaxTextExtent];

      GetColorTuple(color,drawing_wand->image->depth,drawing_wand->image->matte,
        True,tuple);
      MvgPrintf(drawing_wand,"%.1024s",tuple);
    }
}

static void MvgAppendPointsCommand(DrawingWand *drawing_wand,
  char *command,const size_t number_coordinates,const PointInfo *coordinates)
{
  const PointInfo
    *coordinate;

  size_t
    i;

  MvgPrintf(drawing_wand, command);
  for (i=number_coordinates, coordinate=coordinates; i; i--)
    {
      MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g",coordinate->x,coordinate->y);
      coordinate++;
    }
  MvgPrintf(drawing_wand, "\n");
}

static void AdjustAffine(DrawingWand *drawing_wand,const AffineMatrix *affine)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if ((affine->sx != 1.0) || (affine->rx != 0.0) || (affine->ry != 0.0) ||
      (affine->sy != 1.0) || (affine->tx != 0.0) || (affine->ty != 0.0))
    {
      AffineMatrix
        current;

      current=CurrentContext->affine;
      CurrentContext->affine.sx=current.sx*affine->sx+current.ry*affine->rx;
      CurrentContext->affine.rx=current.rx*affine->sx+current.sy*affine->rx;
      CurrentContext->affine.ry=current.sx*affine->ry+current.ry*affine->sy;
      CurrentContext->affine.sy=current.rx*affine->ry+current.sy*affine->sy;
      CurrentContext->affine.tx=current.sx*affine->tx+current.ry*affine->ty+
        current.tx;
      CurrentContext->affine.ty=current.rx*affine->tx+current.sy*affine->ty+
        current.ty;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y D r a w W a n d                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyDrawingWand() frees all resources associated with the drawing
%  wand. Once the drawing wand has been freed, it should not be used
%  any further unless it re-allocated.
%
%  The format of the DestroyDrawingWand method is:
%
%      void DestroyDrawingWand(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand. to destroy
%
*/
WandExport void DestroyDrawingWand(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  drawing_wand->path_operation=PathDefaultOperation;
  drawing_wand->path_mode=DefaultPathMode;
  drawing_wand->indent_depth=0;
  for ( ; drawing_wand->index > 0; drawing_wand->index--)
  {
    DestroyDrawInfo(CurrentContext);
    CurrentContext=(DrawInfo*) NULL;
  }
  DestroyDrawInfo(CurrentContext);
  CurrentContext=(DrawInfo*) NULL;
  drawing_wand->graphic_context=(DrawInfo **)
    RelinquishMagickMemory(drawing_wand->graphic_context);
  if (drawing_wand->pattern_id != (char *) NULL)
    drawing_wand->pattern_id=(char *)
      RelinquishMagickMemory(drawing_wand->pattern_id);
  drawing_wand->pattern_offset=0;
  drawing_wand->pattern_bounds.x=0;
  drawing_wand->pattern_bounds.y=0;
  drawing_wand->pattern_bounds.width=0;
  drawing_wand->pattern_bounds.height=0;
  drawing_wand->mvg=(char *) RelinquishMagickMemory(drawing_wand->mvg);
  drawing_wand->mvg_alloc=0;
  drawing_wand->mvg_length=0;
  drawing_wand->image=(Image*)NULL;
  drawing_wand->signature=0;
  drawing_wand=(DrawingWand *) RelinquishMagickMemory(drawing_wand);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w A n n o t a t i o n                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawAnnotation() draws text on the image.
%
%  The format of the DrawAnnotation method is:
%
%      void DrawAnnotation(DrawingWand *drawing_wand,const double x,
%        const double y,const unsigned char *text)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x: x ordinate to left of text
%
%    o y: y ordinate to text baseline
%
%    o text: text to draw
%
*/
WandExport void DrawAnnotation(DrawingWand *drawing_wand,const double x,
  const double y,const unsigned char *text)
{
  char
    *escaped_text;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(text != (const unsigned char *) NULL);
  escaped_text=EscapeString((const char*)text,'\'');
  MvgPrintf(drawing_wand,"text %.4g,%.4g '%.1024s'\n",x,y,escaped_text);
  escaped_text=(char *) RelinquishMagickMemory(escaped_text);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w A f f i n e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawAffine() adjusts the current affine transformation matrix with
%  the specified affine transformation matrix. Note that the current affine
%  transform is adjusted rather than replaced.
%
%  The format of the DrawAffine method is:
%
%      void DrawAffine(DrawingWand *drawing_wand,const AffineMatrix *affine)
%
%  A description of each parameter follows:
%
%    o drawing_wand: Drawing drawing_wand
%
%    o affine: Affine matrix parameters
%
*/
WandExport void DrawAffine(DrawingWand *drawing_wand,
  const AffineMatrix *affine)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(affine != (const AffineMatrix *)NULL);
  AdjustAffine(drawing_wand,affine );
  MvgPrintf(drawing_wand,"affine %.6g,%.6g,%.6g,%.6g,%.6g,%.6g\n",affine->sx,
    affine->rx,affine->ry,affine->sy,affine->tx,affine->ty);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w A l l o c a t e W a n d                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawAllocateWand() allocates an initial drawing wand which is an
%  opaque handle required by the remaining drawing methods.
%
%  The format of the DrawAllocateWand method is:
%
%      DrawingWand DrawAllocateWand(const DrawInfo *draw_info,Image *image)
%
%  A description of each parameter follows:
%
%    o draw_info: Initial drawing defaults. Set to NULL to use
%                 ImageMagick defaults.
%
%    o image: The image to draw on.
%
*/
WandExport DrawingWand *DrawAllocateWand(const DrawInfo *draw_info,Image *image)
{
  DrawingWand
    *drawing_wand;

  drawing_wand=(DrawingWand *) AcquireMagickMemory(sizeof(struct _DrawingWand));
  if (drawing_wand == (DrawingWand *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateDrawingWand);
  drawing_wand->image=image;
  drawing_wand->mvg=NULL;
  drawing_wand->mvg_alloc=0;
  drawing_wand->mvg_length=0;
  drawing_wand->mvg_width=0;
  drawing_wand->pattern_id=NULL;
  drawing_wand->pattern_offset=0;
  drawing_wand->pattern_bounds.x=0;
  drawing_wand->pattern_bounds.y=0;
  drawing_wand->pattern_bounds.width=0;
  drawing_wand->pattern_bounds.height=0;
  drawing_wand->index=0;
  drawing_wand->graphic_context=(DrawInfo **)
    AcquireMagickMemory(sizeof(DrawInfo *));
  if (drawing_wand->graphic_context == (DrawInfo **) NULL)
    {
      ThrowException3(&drawing_wand->image->exception,ResourceLimitError,
        MemoryAllocationFailed,UnableToDrawOnImage);
      return (DrawingWand *) NULL;
    }
  CurrentContext=CloneDrawInfo((ImageInfo*)NULL,draw_info);
  if (CurrentContext == (DrawInfo*) NULL)
    {
      ThrowException3(&drawing_wand->image->exception,ResourceLimitError,
        MemoryAllocationFailed,UnableToDrawOnImage);
      return (DrawingWand *) NULL;
    }
  drawing_wand->filter_off=False;
  drawing_wand->indent_depth=0;
  drawing_wand->path_operation=PathDefaultOperation;
  drawing_wand->path_mode=DefaultPathMode;
  drawing_wand->signature=MagickSignature;
  return(drawing_wand);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w A r c                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawArc() draws an arc falling within a specified bounding rectangle on the
%  image.
%
%  The format of the DrawArc method is:
%
%      void DrawArc(DrawingWand *drawing_wand,const double sx,const double sy,
%        const double ex,const double ey,const double sd,const double ed)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o sx: starting x ordinate of bounding rectangle
%
%    o sy: starting y ordinate of bounding rectangle
%
%    o ex: ending x ordinate of bounding rectangle
%
%    o ey: ending y ordinate of bounding rectangle
%
%    o sd: starting degrees of rotation
%
%    o ed: ending degrees of rotation
%
*/
WandExport void DrawArc(DrawingWand *drawing_wand,const double sx,
  const double sy,const double ex,const double ey,const double sd,
  const double ed)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  MvgPrintf(drawing_wand,"arc %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",sx,sy,ex,ey,
    sd,ed);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w B e z i e r                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawBezier() draws a bezier curve through a set of points on the image.
%
%  The format of the DrawBezier method is:
%
%      void DrawBezier(DrawingWand *drawing_wand,
%        const size_t number_coordinates,const PointInfo *coordinates)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o number_coordinates: number of coordinates
%
%    o coordinates: coordinates
%
*/
WandExport void DrawBezier(DrawingWand *drawing_wand,
  const size_t number_coordinates,const PointInfo *coordinates)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(coordinates != (const PointInfo *) NULL);
  MvgAppendPointsCommand(drawing_wand,"bezier",number_coordinates,coordinates);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w C i r c l e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawCircle() draws a circle on the image.
%
%  The format of the DrawCircle method is:
%
%      void DrawCircle(DrawingWand *drawing_wand,const double ox,
%        const double oy,const double px, const double py)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o ox: origin x ordinate
%
%    o oy: origin y ordinate
%
%    o px: perimeter x ordinate
%
%    o py: perimeter y ordinate
%
*/
WandExport void DrawCircle(DrawingWand *drawing_wand,const double ox,
  const double oy,const double px,const double py)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  MvgPrintf(drawing_wand, "circle %.4g,%.4g %.4g,%.4g\n",ox,oy,px,py);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t C l i p P a t h                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetClipPath() obtains the current clipping path ID. The value returned
%  must be deallocated by the user when it is no longer needed.
%
%  The format of the DrawGetClipPath method is:
%
%      char *DrawGetClipPath(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport char *DrawGetClipPath(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (CurrentContext->clip_path != (char *) NULL)
    return((char *) AcquireString(CurrentContext->clip_path));
  return((char *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t C l i p P a t h                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetClipPath() associates a named clipping path with the image.  Only
%  the areas drawn on by the clipping path will be modified as long as it
%  remains in effect.
%
%  The format of the DrawSetClipPath method is:
%
%      void DrawSetClipPath(DrawingWand *drawing_wand,const char *clip_path)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o clip_path: name of clipping path to associate with image
%
*/
WandExport void DrawSetClipPath(DrawingWand *drawing_wand,
  const char *clip_path)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(clip_path != (const char *) NULL);
  if ((CurrentContext->clip_path == NULL) || drawing_wand->filter_off ||
      LocaleCompare(CurrentContext->clip_path,clip_path) != 0)
    {
      CloneString(&CurrentContext->clip_path,clip_path);
      if (CurrentContext->clip_path == (char*)NULL)
        ThrowException3(&drawing_wand->image->exception,ResourceLimitError,
          MemoryAllocationFailed,UnableToDrawOnImage);
#if DRAW_BINARY_IMPLEMENTATION
      (void) DrawClipPath(drawing_wand->image,CurrentContext,
        CurrentContext->clip_path);
#endif
      MvgPrintf(drawing_wand,"clip-path url(#%s)\n",clip_path);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t C l i p R u l e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetClipRule() returns the current polygon fill rule to be used by the
%  clipping path.
%
%  The format of the DrawGetClipRule method is:
%
%     FillRule DrawGetClipRule(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport FillRule DrawGetClipRule(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(CurrentContext->fill_rule);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t C l i p R u l e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetClipRule() set the polygon fill rule to be used by the clipping path.
%
%  The format of the DrawSetClipRule method is:
%
%      void DrawSetClipRule(DrawingWand *drawing_wand,const FillRule fill_rule)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o fill_rule: fill rule (EvenOddRule or NonZeroRule)
%
*/
WandExport void DrawSetClipRule(DrawingWand *drawing_wand,
  const FillRule fill_rule)
{
  const char
    *p=NULL;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off || (CurrentContext->fill_rule != fill_rule))
    {
      CurrentContext->fill_rule=fill_rule;
      switch (fill_rule)
      {
        case EvenOddRule:
          p="evenodd";
          break;
        case NonZeroRule:
          p="nonzero";
          break;
        default:
          break;
      }
      if (p != NULL)
        MvgPrintf(drawing_wand, "clip-rule %s\n", p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t C l i p U n i t s                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetClipUnits() returns the interpretation of clip path units.
%
%  The format of the DrawGetClipUnits method is:
%
%      ClipPathUnits DrawGetClipUnits(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport ClipPathUnits DrawGetClipUnits(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(CurrentContext->clip_units);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t C l i p U n i t s                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetClipUnits() sets the interpretation of clip path units.
%
%  The format of the DrawSetClipUnits method is:
%
%      void DrawSetClipUnits(DrawingWand *drawing_wand,
%        const ClipPathUnits clip_units)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o clip_units: units to use (UserSpace, UserSpaceOnUse, or ObjectBoundingBox)
%
*/
WandExport void DrawSetClipUnits(DrawingWand *drawing_wand,
  const ClipPathUnits clip_units)
{
  const char
    *p=NULL;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off || (CurrentContext->clip_units != clip_units))
    {
      CurrentContext->clip_units=clip_units;
      if ( clip_units == ObjectBoundingBox )
        {
          AffineMatrix
            affine;

          GetAffineMatrix(&affine);
          affine.sx=CurrentContext->bounds.x2;
          affine.sy=CurrentContext->bounds.y2;
          affine.tx=CurrentContext->bounds.x1;
          affine.ty=CurrentContext->bounds.y1;
          AdjustAffine( drawing_wand, &affine );
        }

      switch (clip_units)
      {
        case UserSpace:
          p="userSpace";
          break;
        case UserSpaceOnUse:
          p="userSpaceOnUse";
          break;
        case ObjectBoundingBox:
          p="objectBoundingBox";
          break;
      }
      if (p != NULL)
        MvgPrintf(drawing_wand, "clip-units %s\n", p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w C o l o r                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawColor() draws color on image using the current fill color, starting at
%  specified position, and using specified paint method. The available paint
%  methods are:
%
%    PointMethod: Recolors the target pixel
%    ReplaceMethod: Recolor any pixel that matches the target pixel.
%    FloodfillMethod: Recolors target pixels and matching neighbors.
%    FillToBorderMethod: Recolor target pixels and neighbors not matching
$      border color.
%    ResetMethod: Recolor all pixels.
%
%  The format of the DrawColor method is:
%
%      void DrawColor(DrawingWand *drawing_wand,const double x,const double y,
%        const PaintMethod paintMethod)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x: x ordinate
%
%    o y: y ordinate
%
%    o paintMethod: paint method
%
*/
WandExport void DrawColor(DrawingWand *drawing_wand,const double x,
  const double y,const PaintMethod paintMethod)
{
  const char
    *p=NULL;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  switch (paintMethod)
  {
    case PointMethod:
      p="point";
      break;
    case ReplaceMethod:
      p="replace";
      break;
    case FloodfillMethod:
      p="floodfill";
      break;
    case FillToBorderMethod:
      p="filltoborder";
      break;
    case ResetMethod:
      p="reset";
      break;
  }
  if (p != NULL)
    MvgPrintf(drawing_wand, "color %.4g,%.4g %s\n", x, y, p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w C o m m e n t                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawComment() adds a comment to a vector output stream.
%
%  The format of the DrawComment method is:
%
%      void DrawComment(DrawingWand *drawing_wand,const char *comment)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o comment: comment text
%
*/
WandExport void DrawComment(DrawingWand *drawing_wand,const char* comment)
{
  MvgPrintf(drawing_wand,"#%s\n",comment);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w E l l i p s e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawEllipse() draws an ellipse on the image.
%
%  The format of the DrawEllipse method is:
%
%       void DrawEllipse(DrawingWand *drawing_wand,const double ox,
%         const double oy,const double rx,const double ry,const double start,
%         const double end)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o ox: origin x ordinate
%
%    o oy: origin y ordinate
%
%    o rx: radius in x
%
%    o ry: radius in y
%
%    o start: starting rotation in degrees
%
%    o end: ending rotation in degrees
%
*/
WandExport void DrawEllipse(DrawingWand *drawing_wand,const double ox,
  const double oy,const double rx,const double ry,const double start,
  const double end)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  MvgPrintf(drawing_wand,"ellipse %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",ox,oy,rx,ry,
    start,end);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F i l l C o l o r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFillColor() returns the fill color used for drawing filled objects.
%
%  The format of the DrawGetFillColor method is:
%
%      void DrawGetFillColor(const DrawingWand *drawing_wand,
%        PixelWand *fill_color)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o fill_color: Return the fill color.
%
*/
WandExport void DrawGetFillColor(const DrawingWand *drawing_wand,
  PixelWand *fill_color)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  PixelSetQuantumColor(fill_color,&CurrentContext->fill);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F i l l C o l o r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFillColor() sets the fill color to be used for drawing filled objects.
%
%  The format of the DrawSetFillColor method is:
%
%      void DrawSetFillColor(DrawingWand *drawing_wand,
%        const PixelWand *fill_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o fill_wand: fill wand.
%
*/
WandExport void DrawSetFillColor(DrawingWand *drawing_wand,
  const PixelWand *fill_wand)
{
  PixelPacket
    *current_fill,
    fill_color,
    new_fill;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(fill_wand != (const PixelWand *) NULL);
  PixelGetQuantumColor(fill_wand,&fill_color);
  new_fill=fill_color;
  if (new_fill.opacity != TransparentOpacity)
    new_fill.opacity=CurrentContext->opacity;
  current_fill=&CurrentContext->fill;
  if (drawing_wand->filter_off || !WandColorMatch(current_fill,&new_fill))
    {
      CurrentContext->fill=new_fill;
      MvgPrintf(drawing_wand,"fill '");
      MvgAppendColor(drawing_wand,&fill_color);
      MvgPrintf(drawing_wand,"'\n");
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F i l l P a t t e r n U R L                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFillPatternURL() sets the URL to use as a fill pattern for filling
%  objects. Only local URLs ("#identifier") are supported at this time. These
%  local URLs are normally created by defining a named fill pattern with
%  DrawPushPattern/DrawPopPattern.
%
%  The format of the DrawSetFillPatternURL method is:
%
%      void DrawSetFillPatternURL(DrawingWand *drawing_wand,
%        const char *fill_url)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o fill_url: URL to use to obtain fill pattern.
%
*/
WandExport void DrawSetFillPatternURL(DrawingWand *drawing_wand,
  const char* fill_url)
{
  char
    pattern[MaxTextExtent];

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(fill_url != NULL);
  if (fill_url[0] != '#')
    ThrowException(&drawing_wand->image->exception,DrawWarning,
      NotARelativeURL,fill_url);
  (void) FormatMagickString(pattern,MaxTextExtent,"[%.1024s]",fill_url+1);
  if (GetImageAttribute(drawing_wand->image,pattern) == (ImageAttribute *) NULL)
    {
      ThrowException(&drawing_wand->image->exception,DrawWarning,
        URLNotFound,fill_url);
    }
  else
    {
      char
        pattern_spec[MaxTextExtent];

      (void) FormatMagickString(pattern_spec,MaxTextExtent,"url(%.1024s)",
        fill_url);
#if DRAW_BINARY_IMPLEMENTATION
      DrawPatternPath(drawing_wand->image,CurrentContext,pattern_spec,&CurrentContext->fill_pattern);
#endif
      if (CurrentContext->fill.opacity != TransparentOpacity)
        CurrentContext->fill.opacity=CurrentContext->opacity;
      MvgPrintf(drawing_wand,"fill %s\n",pattern_spec);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F i l l O p a c i t y                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFillOpacity() returns the opacity used when drawing using the fill
%  color or fill texture.  Fully opaque is 1.0.
%
%  The format of the DrawGetFillOpacity method is:
%
%      double DrawGetFillOpacity(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport double DrawGetFillOpacity(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return((double) CurrentContext->opacity/MaxRGB);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F i l l O p a c i t y                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFillOpacity() sets the opacity to use when drawing using the fill
%  color or fill texture.  Fully opaque is 1.0.
%
%  The format of the DrawSetFillOpacity method is:
%
%      void DrawSetFillOpacity(DrawingWand *drawing_wand,
%        const double fill_opacity)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o fill_opacity: fill opacity
%
*/
WandExport void DrawSetFillOpacity(DrawingWand *drawing_wand,
  const double fill_opacity)
{
  Quantum
    opacity;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  opacity=(Quantum)
    ((double) MaxRGB*(1.0-(fill_opacity <= 1.0 ? fill_opacity : 1.0 ))+0.5);
  if (drawing_wand->filter_off || (CurrentContext->opacity != opacity))
    {
      CurrentContext->opacity=opacity;
      MvgPrintf(drawing_wand,"fill-opacity %.4g\n",fill_opacity);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F i l l R u l e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFillRule() returns the fill rule used while drawing polygons.
%
%  The format of the DrawGetFillRule method is:
%
%      FillRule DrawGetFillRule(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport FillRule DrawGetFillRule(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(CurrentContext->fill_rule);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F i l l R u l e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFillRule() sets the fill rule to use while drawing polygons.
%
%  The format of the DrawSetFillRule method is:
%
%      void DrawSetFillRule(DrawingWand *drawing_wand,const FillRule fill_rule)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o fill_rule: fill rule (EvenOddRule or NonZeroRule)
%
*/
WandExport void DrawSetFillRule(DrawingWand *drawing_wand,
  const FillRule fill_rule)
{
  const char
    *p=NULL;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off || (CurrentContext->fill_rule != fill_rule))
    {
      CurrentContext->fill_rule=fill_rule;
      switch (fill_rule)
      {
        case EvenOddRule:
          p="evenodd";
          break;
        case NonZeroRule:
          p="nonzero";
          break;
        default:
          break;
      }
      if (p != NULL)
        MvgPrintf(drawing_wand,"fill-rule %s\n",p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F o n t                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFont() returns a null-terminaged string specifying the font used
%  when annotating with text. The value returned must be freed by the user
%  when no longer needed.
%
%  The format of the DrawGetFont method is:
%
%      char *DrawGetFont(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport char *DrawGetFont(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (CurrentContext->font != (char *) NULL)
    return(AcquireString(CurrentContext->font));
  return((char *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F o n t                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFont() sets the fully-sepecified font to use when annotating with
%  text.
%
%  The format of the DrawSetFont method is:
%
%      void DrawSetFont(DrawingWand *drawing_wand,const char *font_name)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o font_name: font name
%
*/
WandExport void DrawSetFont(DrawingWand *drawing_wand,const char *font_name)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(font_name != (const char *) NULL);
  if (drawing_wand->filter_off || (CurrentContext->font == NULL) ||
     LocaleCompare(CurrentContext->font,font_name) != 0)
    {
      (void) CloneString(&CurrentContext->font,font_name);
      if (CurrentContext->font == (char*)NULL)
        ThrowException3(&drawing_wand->image->exception,ResourceLimitError,
          MemoryAllocationFailed,UnableToDrawOnImage);
      MvgPrintf(drawing_wand,"font '%s'\n",font_name);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F o n t F a m i l y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFontFamily() returns the font family to use when annotating with text.
%  The value returned must be freed by the user when it is no longer needed.
%
%  The format of the DrawGetFontFamily method is:
%
%      char *DrawGetFontFamily(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport char *DrawGetFontFamily(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (CurrentContext->family != NULL)
    return(AcquireString(CurrentContext->family));
  return((char *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F o n t F a m i l y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFontFamily() sets the font family to use when annotating with text.
%
%  The format of the DrawSetFontFamily method is:
%
%      void DrawSetFontFamily(DrawingWand *drawing_wand,const char *font_family)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o font_family: font family
%
*/
WandExport void DrawSetFontFamily(DrawingWand *drawing_wand,
  const char *font_family)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(font_family != (const char *) NULL);
  if (drawing_wand->filter_off || (CurrentContext->family == NULL) ||
     LocaleCompare(CurrentContext->family,font_family) != 0)
    {
      (void) CloneString(&CurrentContext->family,font_family);
      if (CurrentContext->family == (char *) NULL)
        ThrowException3(&drawing_wand->image->exception,ResourceLimitError,
          MemoryAllocationFailed,UnableToDrawOnImage);
      MvgPrintf(drawing_wand,"font-family '%s'\n",font_family);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F o n t S i z e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFontSize() returns the font pointsize used when annotating with text.
%
%  The format of the DrawGetFontSize method is:
%
%      double DrawGetFontSize(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport double DrawGetFontSize(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(CurrentContext->pointsize);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F o n t S i z e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFontSize() sets the font pointsize to use when annotating with text.
%
%  The format of the DrawSetFontSize method is:
%
%      void DrawSetFontSize(DrawingWand *drawing_wand,const double pointsize)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o pointsize: text pointsize
%
*/
WandExport void DrawSetFontSize(DrawingWand *drawing_wand,
  const double pointsize)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off ||
     (AbsoluteValue(CurrentContext->pointsize-pointsize) > MagickEpsilon))
    {
      CurrentContext->pointsize=pointsize;
      MvgPrintf(drawing_wand,"font-size %.4g\n",pointsize);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F o n t S t r e t c h                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFontStretch() returns the font stretch used when annotating with text.
%
%  The format of the DrawGetFontStretch method is:
%
%      StretchType DrawGetFontStretch(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport StretchType DrawGetFontStretch(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(CurrentContext->stretch);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F o n t S t r e t c h                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFontStretch() sets the font stretch to use when annotating with text.
%  The AnyStretch enumeration acts as a wild-card "don't care" option.
%
%  The format of the DrawSetFontStretch method is:
%
%      void DrawSetFontStretch(DrawingWand *drawing_wand,
%        const StretchType font_stretch)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o font_stretch: font stretch (NormalStretch, UltraCondensedStretch,
%                    CondensedStretch, SemiCondensedStretch,
%                    SemiExpandedStretch, ExpandedStretch,
%                    ExtraExpandedStretch, UltraExpandedStretch, AnyStretch)
%
*/
WandExport void DrawSetFontStretch(DrawingWand *drawing_wand,
  const StretchType font_stretch)
{
  const char
    *p=NULL;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off || (CurrentContext->stretch != font_stretch))
    {
      CurrentContext->stretch=font_stretch;
      switch (font_stretch)
      {
        case NormalStretch:
          p="normal";
          break;
        case UltraCondensedStretch:
          p="ultra-condensed";
          break;
        case ExtraCondensedStretch:
          p="extra-condensed";
          break;
        case CondensedStretch:
          p="condensed";
          break;
        case SemiCondensedStretch:
          p="semi-condensed";
          break;
        case SemiExpandedStretch:
          p="semi-expanded";
          break;
        case ExpandedStretch:
          p="expanded";
          break;
        case ExtraExpandedStretch:
          p="extra-expanded";
          break;
        case UltraExpandedStretch:
          p="ultra-expanded";
          break;
        case AnyStretch:
          p="all";
          break;
      }
      if (p != NULL)
        MvgPrintf(drawing_wand,"font-stretch '%s'\n",p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F o n t S t y l e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFontStyle() returns the font style used when annotating with text.
%
%  The format of the DrawGetFontStyle method is:
%
%      StyleType DrawGetFontStyle(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport StyleType DrawGetFontStyle(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(CurrentContext->style);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F o n t S t y l e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFontStyle() sets the font style to use when annotating with text.
%  The AnyStyle enumeration acts as a wild-card "don't care" option.
%
%  The format of the DrawSetFontStyle method is:
%
%      void DrawSetFontStyle(DrawingWand *drawing_wand,const StyleType style)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o style: font style (NormalStyle, ItalicStyle, ObliqueStyle, AnyStyle)
%
*/
WandExport void DrawSetFontStyle(DrawingWand *drawing_wand,
  const StyleType style)
{
  const char
    *p=NULL;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off || (CurrentContext->style != style))
    {
      CurrentContext->style=style;
      switch (style)
      {
        case NormalStyle:
          p="normal";
          break;
        case ItalicStyle:
          p="italic";
          break;
        case ObliqueStyle:
          p="oblique";
          break;
        case AnyStyle:
          p="all";
          break;
      }
      if (p != NULL)
        MvgPrintf(drawing_wand, "font-style '%s'\n", p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F o n t W e i g h t                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFontWeight() returns the font weight used when annotating with text.
%
%  The format of the DrawGetFontWeight method is:
%
%      unsigned long DrawGetFontWeight(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport unsigned long DrawGetFontWeight(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(CurrentContext->weight);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F o n t W e i g h t                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFontWeight() sets the font weight to use when annotating with text.
%
%  The format of the DrawSetFontWeight method is:
%
%      void DrawSetFontWeight(DrawingWand *drawing_wand,
%        const unsigned long font_weight)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o font_weight: font weight (valid range 100-900)
%
*/
WandExport void DrawSetFontWeight(DrawingWand *drawing_wand,
  const unsigned long font_weight)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off || (CurrentContext->weight != font_weight))
    {
      CurrentContext->weight=font_weight;
      MvgPrintf(drawing_wand,"font-weight %lu\n",font_weight);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t G r a v i t y                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetGravity() returns the text placement gravity used when annotating
%  with text.
%
%  The format of the DrawGetGravity method is:
%
%      GravityType DrawGetGravity(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport GravityType DrawGetGravity(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(CurrentContext->gravity);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t G r a v i t y                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetGravity() sets the text placement gravity to use when annotating
%  with text.
%
%  The format of the DrawSetGravity method is:
%
%      void DrawSetGravity(DrawingWand *drawing_wand,const GravityType gravity)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o gravity: positioning gravity (NorthWestGravity, NorthGravity,
%               NorthEastGravity, WestGravity, CenterGravity,
%               EastGravity, SouthWestGravity, SouthGravity,
%               SouthEastGravity)
%
*/
WandExport void DrawSetGravity(DrawingWand *drawing_wand,
  const GravityType gravity)
{
  const char
    *p=NULL;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off || (CurrentContext->gravity != gravity))
    {
      CurrentContext->gravity=gravity;
      switch (gravity)
      {
        case NorthWestGravity:
          p="NorthWest";
          break;
        case NorthGravity:
          p="North";
          break;
        case NorthEastGravity:
          p="NorthEast";
          break;
        case WestGravity:
          p="West";
          break;
        case CenterGravity:
          p="Center";
          break;
        case EastGravity:
          p="East";
          break;
        case SouthWestGravity:
          p="SouthWest";
          break;
        case SouthGravity:
          p="South";
          break;
        case SouthEastGravity:
          p="SouthEast";
          break;
        case StaticGravity:
        case ForgetGravity:
          {
          }
          break;
      }
      if (p != NULL)
        MvgPrintf(drawing_wand,"gravity %s\n",p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w C o m p o s i t e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawComposite() composites an image onto the current image, using the
%  specified composition operator, specified position, and at the specified
%  size.
%
%  The format of the DrawComposite method is:
%
%      void DrawComposite(DrawingWand *drawing_wand,
%        const CompositeOperator composite_operator,const double x,
%        const double y,const double width,const double height,
%        const Image *image)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o composite_operator: composition operator
%
%    o x: x ordinate of top left corner
%
%    o y: y ordinate of top left corner
%
%    o width: Width to resize image to prior to compositing.  Specify zero to
%             use existing width.
%
%    o height: Height to resize image to prior to compositing.  Specify zero
%             to use existing height.
%
%    o image: Image to composite
%
*/
WandExport void DrawComposite(DrawingWand *drawing_wand,
  const CompositeOperator composite_operator,const double x,const double y,
  const double width,const double height,const Image *image)

{
  ImageInfo
    *image_info;

  Image
    *clone_image;

  char
    *media_type=NULL,
    *base64=NULL;

  const char
    *mode=NULL;

  unsigned char
    *blob=(unsigned char *) NULL;

  size_t
    blob_length=2048,
    encoded_length=0;

  MonitorHandler
    handler;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(image != (Image *) NULL);
  assert(width != 0);
  assert(height != 0);
  assert(*image->magick != '\0');
  clone_image=CloneImage(image,0,0,True,&drawing_wand->image->exception);
  if (!clone_image)
    return;
  image_info=CloneImageInfo((ImageInfo*)NULL);
  if (!image_info)
    ThrowException3(&drawing_wand->image->exception,ResourceLimitError,
      MemoryAllocationFailed,UnableToDrawOnImage);
  handler=SetMonitorHandler((MonitorHandler) NULL);
  blob=(unsigned char*) ImageToBlob( image_info,clone_image,&blob_length,
    &drawing_wand->image->exception);
  (void) SetMonitorHandler(handler);
  DestroyImageInfo(image_info);
  DestroyImageList(clone_image);
  if (!blob)
    return;
  base64=Base64Encode(blob,blob_length,&encoded_length);
  blob=(unsigned char *) RelinquishMagickMemory(blob);
  if (!base64)
    {
      char
        buffer[MaxTextExtent];

      (void) FormatMagickString(buffer,MaxTextExtent,"%ld bytes",
        (4L*blob_length/3L+4L));
      ThrowException(&drawing_wand->image->exception,ResourceLimitWarning,
        MemoryAllocationFailed,buffer);
    }
  mode="copy";
  switch (composite_operator)
  {
    case AddCompositeOp:
      mode="add";
      break;
    case AtopCompositeOp:
      mode="atop";
      break;
    case BumpmapCompositeOp:
      mode="bumpmap";
      break;
    case ClearCompositeOp:
      mode="clear";
      break;
    case ColorizeCompositeOp:
      mode="colorize_not_supported";
      break;
    case CopyBlueCompositeOp:
      mode="copyblue";
      break;
    case CopyCompositeOp:
      mode="copy";
      break;
    case CopyGreenCompositeOp:
      mode="copygreen";
      break;
    case CopyOpacityCompositeOp:
      mode="copyopacity";
      break;
    case CopyRedCompositeOp:
      mode="copyred";
      break;
    case DarkenCompositeOp:
      mode="darken_not_supported";
      break;
    case DifferenceCompositeOp:
      mode="difference";
      break;
    case DisplaceCompositeOp:
      mode="displace_not_supported";
      break;
    case DissolveCompositeOp:
      mode="dissolve_not_supported";
      break;
    case HueCompositeOp:
      mode="hue_not_supported";
      break;
    case InCompositeOp:
      mode="in";
      break;
    case LightenCompositeOp:
      mode="lighten_not_supported";
      break;
    case LuminizeCompositeOp:
      mode="luminize_not_supported";
      break;
    case MinusCompositeOp:
      mode="minus";
      break;
    case ModulateCompositeOp:
      mode="modulate_not_supported";
      break;
    case MultiplyCompositeOp:
      mode="multiply";
      break;
    case NoCompositeOp:
      mode="no_not_supported";
      break;
    case OutCompositeOp:
      mode="out";
      break;
    case OverCompositeOp:
      mode="over";
      break;
    case OverlayCompositeOp:
      mode="overlay_not_supported";
      break;
    case PlusCompositeOp:
      mode="plus";
      break;
    case SaturateCompositeOp:
      mode="saturate_not_supported";
      break;
    case ScreenCompositeOp:
      mode="screen_not_supported";
      break;
    case SubtractCompositeOp:
      mode="subtract";
      break;
    case ThresholdCompositeOp:
      mode="threshold";
      break;
    case XorCompositeOp:
      mode="xor";
      break;
    default:
      break;
  }
  media_type=MagickToMime(image->magick);
  if (media_type != NULL)
    {
      char
        *str;

      int
        remaining;

      MvgPrintf(drawing_wand,"image %s %.4g,%.4g %.4g,%.4g 'data:%s;base64,\n",
        mode,x,y,width,height,media_type);
      remaining=(int) encoded_length;
      str=base64;
      while ( remaining > 0 )
      {
        MvgPrintf(drawing_wand,"%.76s", str);
        remaining -= 76;
        str += 76;
        if (remaining > 0)
          MvgPrintf(drawing_wand,"\n");
      }
      MvgPrintf(drawing_wand,"'\n");
    }
  media_type=(char *) RelinquishMagickMemory(media_type);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w L i n e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawLine() draws a line on the image using the current stroke color,
%  stroke opacity, and stroke width.
%
%  The format of the DrawLine method is:
%
%      void DrawLine(DrawingWand *drawing_wand,const double sx,const double sy,
%        const double ex,const double ey)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o sx: starting x ordinate
%
%    o sy: starting y ordinate
%
%    o ex: ending x ordinate
%
%    o ey: ending y ordinate
%
*/
WandExport void DrawLine(DrawingWand *drawing_wand,const double sx,
  const double sy,const double ex,const double ey)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  MvgPrintf(drawing_wand,"line %.4g,%.4g %.4g,%.4g\n",sx,sy,ex,ey);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w M a t t e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawMatte() paints on the image's opacity channel in order to set effected
%  pixels to transparent.
%  to influence the opacity of pixels. The available paint
%  methods are:
%
%    PointMethod: Select the target pixel
%    ReplaceMethod: Select any pixel that matches the target pixel.
%    FloodfillMethod: Select the target pixel and matching neighbors.
%    FillToBorderMethod: Select the target pixel and neighbors not matching
%                        border color.
%    ResetMethod: Select all pixels.
%
%  The format of the DrawMatte method is:
%
%      void DrawMatte(DrawingWand *drawing_wand,const double x,const double y,
%        const PaintMethod paint_method)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x: x ordinate
%
%    o y: y ordinate
%
%    o paint_method:
%
*/
WandExport void DrawMatte(DrawingWand *drawing_wand,const double x,
  const double y,const PaintMethod paint_method)
{
  const char
    *p=NULL;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  switch (paint_method)
  {
    case PointMethod:
      p="point";
      break;
    case ReplaceMethod:
      p="replace";
      break;
    case FloodfillMethod:
      p="floodfill";
      break;
    case FillToBorderMethod:
      p="filltoborder";
      break;
    case ResetMethod:
      p="reset";
      break;
  }
  if (p != NULL)
    MvgPrintf(drawing_wand,"matte %.4g,%.4g %s\n",x,y,p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C l o s e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathClose() adds a path element to the current path which closes the
%  current subpath by drawing a straight line from the current point to the
%  current subpath's most recent starting point (usually, the most recent
%  moveto point).
%
%  The format of the DrawPathClose method is:
%
%      void DrawPathClose(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void DrawPathClose(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  MvgAutoWrapPrintf(drawing_wand,"%s",
    drawing_wand->path_mode == AbsolutePathMode ? "Z" : "z");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o A b s o l u t e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToAbsolute() draws a cubic Bezier curve from the current
%  point to (x,y) using (x1,y1) as the control point at the beginning of
%  the curve and (x2,y2) as the control point at the end of the curve using
%  absolute coordinates. At the end of the command, the new current point
%  becomes the final (x,y) coordinate pair used in the polybezier.
%
%  The format of the DrawPathCurveToAbsolute method is:
%
%      void DrawPathCurveToAbsolute(DrawingWand *drawing_wand,const double x1,
%        const double y1,const double x2,const double y2,const double x,
%        const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x1: x ordinate of control point for curve beginning
%
%    o y1: y ordinate of control point for curve beginning
%
%    o x2: x ordinate of control point for curve ending
%
%    o y2: y ordinate of control point for curve ending
%
%    o x: x ordinate of the end of the curve
%
%    o y: y ordinate of the end of the curve
%
*/

static void DrawPathCurveTo(DrawingWand *drawing_wand,const PathMode mode,
  const double x1,const double y1,const double x2,const double y2,
  const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if ((drawing_wand->path_operation != PathCurveToOperation) ||
      (drawing_wand->path_mode != mode))
    {
      drawing_wand->path_operation=PathCurveToOperation;
      drawing_wand->path_mode=mode;
      MvgAutoWrapPrintf(drawing_wand, "%c%.4g,%.4g %.4g,%.4g %.4g,%.4g",
        mode == AbsolutePathMode ? 'C' : 'c',x1,y1,x2,y2,x,y);
    }
  else
    MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g %.4g,%.4g %.4g,%.4g",
      x1,y1,x2,y2,x,y);
}

WandExport void DrawPathCurveToAbsolute(DrawingWand *drawing_wand,
  const double x1,const double y1,const double x2,const double y2,
  const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathCurveTo(drawing_wand,AbsolutePathMode,x1,y1,x2,y2,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o R e l a t i v e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToRelative() draws a cubic Bezier curve from the current
%  point to (x,y) using (x1,y1) as the control point at the beginning of
%  the curve and (x2,y2) as the control point at the end of the curve using
%  relative coordinates. At the end of the command, the new current point
%  becomes the final (x,y) coordinate pair used in the polybezier.
%
%  The format of the DrawPathCurveToRelative method is:
%
%      void DrawPathCurveToRelative(DrawingWand *drawing_wand,const double x1,
%        const double y1,const double x2,const double y2,const double x,
%        const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x1: x ordinate of control point for curve beginning
%
%    o y1: y ordinate of control point for curve beginning
%
%    o x2: x ordinate of control point for curve ending
%
%    o y2: y ordinate of control point for curve ending
%
%    o x: x ordinate of the end of the curve
%
%    o y: y ordinate of the end of the curve
%
*/
WandExport void DrawPathCurveToRelative(DrawingWand *drawing_wand,
  const double x1,const double y1,const double x2,const double y2,
  const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathCurveTo(drawing_wand,RelativePathMode,x1,y1,x2,y2,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r A b s o l u t e %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToQuadraticBezierAbsolute() draws a quadratic Bezier curve
%  from the current point to (x,y) using (x1,y1) as the control point using
%  absolute coordinates. At the end of the command, the new current point
%  becomes the final (x,y) coordinate pair used in the polybezier.
%
%  The format of the DrawPathCurveToQuadraticBezierAbsolute method is:
%
%      void DrawPathCurveToQuadraticBezierAbsolute(DrawingWand *drawing_wand,
%        const double x1,const double y1,onst double x,const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x1: x ordinate of the control point
%
%    o y1: y ordinate of the control point
%
%    o x: x ordinate of final point
%
%    o y: y ordinate of final point
%
*/

static void DrawPathCurveToQuadraticBezier(DrawingWand *drawing_wand,
  const PathMode mode,const double x1,double y1,const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if ((drawing_wand->path_operation != PathCurveToQuadraticBezierOperation) ||
      (drawing_wand->path_mode != mode))
    {
      drawing_wand->path_operation=PathCurveToQuadraticBezierOperation;
      drawing_wand->path_mode=mode;
      MvgAutoWrapPrintf(drawing_wand, "%c%.4g,%.4g %.4g,%.4g",
        mode == AbsolutePathMode ? 'Q' : 'q',x1,y1,x,y);
    }
  else
    MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g %.4g,%.4g",x1,y1,x,y);
}

WandExport void DrawPathCurveToQuadraticBezierAbsolute(
  DrawingWand *drawing_wand,const double x1,const double y1,const double x,
  const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathCurveToQuadraticBezier(drawing_wand,AbsolutePathMode,x1,y1,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r R e l a t i v %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToQuadraticBezierRelative() draws a quadratic Bezier curve
%  from the current point to (x,y) using (x1,y1) as the control point using
%  relative coordinates. At the end of the command, the new current point
%  becomes the final (x,y) coordinate pair used in the polybezier.
%
%  The format of the DrawPathCurveToQuadraticBezierRelative method is:
%
%      void DrawPathCurveToQuadraticBezierRelative(DrawingWand *drawing_wand,
%        const double x1,const double y1,const double x,const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x1: x ordinate of the control point
%
%    o y1: y ordinate of the control point
%
%    o x: x ordinate of final point
%
%    o y: y ordinate of final point
%
*/
WandExport void DrawPathCurveToQuadraticBezierRelative(
  DrawingWand *drawing_wand,const double x1,const double y1,const double x,
  const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathCurveToQuadraticBezier(drawing_wand,RelativePathMode,x1,y1,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r S m o o t h   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToQuadraticBezierSmoothAbsolute() draws a quadratic
%  Bezier curve (using absolute coordinates) from the current point to
%  (x,y). The control point is assumed to be the reflection of the
%  control point on the previous command relative to the current
%  point. (If there is no previous command or if the previous command was
%  not a DrawPathCurveToQuadraticBezierAbsolute,
%  DrawPathCurveToQuadraticBezierRelative,
%  DrawPathCurveToQuadraticBezierSmoothAbsolut or
%  DrawPathCurveToQuadraticBezierSmoothRelative, assume the control point
%  is coincident with the current point.). At the end of the command, the
%  new current point becomes the final (x,y) coordinate pair used in the
%  polybezier.
%
%  The format of the DrawPathCurveToQuadraticBezierSmoothAbsolute method is:
%
%      void DrawPathCurveToQuadraticBezierSmoothAbsolute(
%        DrawingWand *drawing_wand,const double x,const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x: x ordinate of final point
%
%    o y: y ordinate of final point
%
*/

static void DrawPathCurveToQuadraticBezierSmooth(DrawingWand *drawing_wand,
  const PathMode mode,const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if ((drawing_wand->path_operation != PathCurveToQuadraticBezierSmoothOperation) ||
      (drawing_wand->path_mode != mode))
    {
      drawing_wand->path_operation=PathCurveToQuadraticBezierSmoothOperation;
      drawing_wand->path_mode=mode;
      MvgAutoWrapPrintf(drawing_wand, "%c%.4g,%.4g",
        mode == AbsolutePathMode ? 'T' : 't',x,y);
    }
  else
    MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g",x,y);
}

WandExport void DrawPathCurveToQuadraticBezierSmoothAbsolute(
  DrawingWand *drawing_wand,const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathCurveToQuadraticBezierSmooth(drawing_wand,AbsolutePathMode,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r S m o o t h   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToQuadraticBezierSmoothAbsolute() draws a quadratic
%  Bezier curve (using relative coordinates) from the current point to
%  (x,y). The control point is assumed to be the reflection of the
%  control point on the previous command relative to the current
%  point. (If there is no previous command or if the previous command was
%  not a DrawPathCurveToQuadraticBezierAbsolute,
%  DrawPathCurveToQuadraticBezierRelative,
%  DrawPathCurveToQuadraticBezierSmoothAbsolut or
%  DrawPathCurveToQuadraticBezierSmoothRelative, assume the control point
%  is coincident with the current point.). At the end of the command, the
%  new current point becomes the final (x,y) coordinate pair used in the
%  polybezier.
%
%  The format of the DrawPathCurveToQuadraticBezierSmoothRelative method is:
%
%      void DrawPathCurveToQuadraticBezierSmoothRelative(
%        DrawingWand *drawing_wand,const double x,const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x: x ordinate of final point
%
%    o y: y ordinate of final point
%
%
*/
WandExport void DrawPathCurveToQuadraticBezierSmoothRelative(
  DrawingWand *drawing_wand,const double x,const double y)
{
  DrawPathCurveToQuadraticBezierSmooth(drawing_wand,RelativePathMode,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o S m o o t h A b s o l u t e                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToSmoothAbsolute() draws a cubic Bezier curve from the
%  current point to (x,y) using absolute coordinates. The first control
%  point is assumed to be the reflection of the second control point on
%  the previous command relative to the current point. (If there is no
%  previous command or if the previous command was not an
%  DrawPathCurveToAbsolute, DrawPathCurveToRelative,
%  DrawPathCurveToSmoothAbsolute or DrawPathCurveToSmoothRelative, assume
%  the first control point is coincident with the current point.) (x2,y2)
%  is the second control point (i.e., the control point at the end of the
%  curve). At the end of the command, the new current point becomes the
%  final (x,y) coordinate pair used in the polybezier.
%
%  The format of the DrawPathCurveToSmoothAbsolute method is:
%
%      void DrawPathCurveToSmoothAbsolute(DrawingWand *drawing_wand,
%        const double x2const double y2,const double x,const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x2: x ordinate of second control point
%
%    o y2: y ordinate of second control point
%
%    o x: x ordinate of termination point
%
%    o y: y ordinate of termination point
%
%
*/
static void DrawPathCurveToSmooth(DrawingWand *drawing_wand,const PathMode mode,
  const double x2,const double y2,const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if ((drawing_wand->path_operation != PathCurveToSmoothOperation) ||
      (drawing_wand->path_mode != mode))
    {
      drawing_wand->path_operation=PathCurveToSmoothOperation;
      drawing_wand->path_mode=mode;
      MvgAutoWrapPrintf(drawing_wand,"%c%.4g,%.4g %.4g,%.4g",
        mode == AbsolutePathMode ? 'S' : 's',x2,y2,x,y);
    }
  else
    MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g %.4g,%.4g",x2,y2,x,y);
}

WandExport void DrawPathCurveToSmoothAbsolute(DrawingWand *drawing_wand,
  const double x2,const double y2,const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathCurveToSmooth(drawing_wand,AbsolutePathMode,x2,y2,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o S m o o t h R e l a t i v e                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToSmoothRelative() draws a cubic Bezier curve from the
%  current point to (x,y) using relative coordinates. The first control
%  point is assumed to be the reflection of the second control point on
%  the previous command relative to the current point. (If there is no
%  previous command or if the previous command was not an
%  DrawPathCurveToAbsolute, DrawPathCurveToRelative,
%  DrawPathCurveToSmoothAbsolute or DrawPathCurveToSmoothRelative, assume
%  the first control point is coincident with the current point.) (x2,y2)
%  is the second control point (i.e., the control point at the end of the
%  curve). At the end of the command, the new current point becomes the
%  final (x,y) coordinate pair used in the polybezier.
%
%  The format of the DrawPathCurveToSmoothRelative method is:
%
%      void DrawPathCurveToSmoothRelative(DrawingWand *drawing_wand,
%        const double x2,const double y2,const double x,const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x2: x ordinate of second control point
%
%    o y2: y ordinate of second control point
%
%    o x: x ordinate of termination point
%
%    o y: y ordinate of termination point
%
%
*/
WandExport void DrawPathCurveToSmoothRelative(DrawingWand *drawing_wand,
  const double x2,const double y2,const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathCurveToSmooth(drawing_wand,RelativePathMode,x2,y2,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h E l l i p t i c A r c A b s o l u t e                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathEllipticArcAbsolute() draws an elliptical arc from the current
%  point to (x, y) using absolute coordinates. The size and orientation
%  of the ellipse are defined by two radii (rx, ry) and an
%  xAxisRotation, which indicates how the ellipse as a whole is rotated
%  relative to the current coordinate system. The center (cx, cy) of the
%  ellipse is calculated automatically to satisfy the constraints imposed
%  by the other parameters. largeArcFlag and sweepFlag contribute to the
%  automatic calculations and help determine how the arc is drawn. If
%  largeArcFlag is true then draw the larger of the available arcs. If
%  sweepFlag is true, then draw the arc matching a clock-wise rotation.
%
%  The format of the DrawPathEllipticArcAbsolute method is:
%
%      void DrawPathEllipticArcAbsolute(DrawingWand *drawing_wand,
%        const double rx,const double ry,const double x_axis_rotation,
%        unsigned int large_arc_flag,unsigned int sweep_flag,const double x,
%        const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o rx: x radius
%
%    o ry: y radius
%
%    o x_axis_rotation: indicates how the ellipse as a whole is rotated
%                       relative to the current coordinate system
%
%    o large_arc_flag: If non-zero (true) then draw the larger of the
%                      available arcs
%
%    o sweep_flag: If non-zero (true) then draw the arc matching a
%                  clock-wise rotation
%
%
*/

static void DrawPathEllipticArc(DrawingWand *drawing_wand, const PathMode mode,
  const double rx,const double ry,const double x_axis_rotation,
  unsigned int large_arc_flag,unsigned int sweep_flag,const double x,
  const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if ((drawing_wand->path_operation != PathEllipticArcOperation) ||
      (drawing_wand->path_mode != mode))
    {
      drawing_wand->path_operation=PathEllipticArcOperation;
      drawing_wand->path_mode=mode;
      MvgAutoWrapPrintf(drawing_wand, "%c%.4g,%.4g %.4g %u %u %.4g,%.4g",
        mode == AbsolutePathMode ? 'A' : 'a',rx,ry,x_axis_rotation,
        large_arc_flag,sweep_flag,x,y);
    }
  else
    MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g %.4g %u %u %.4g,%.4g",rx,ry,
      x_axis_rotation,large_arc_flag,sweep_flag,x,y);
}

WandExport void DrawPathEllipticArcAbsolute(DrawingWand *drawing_wand,
  const double rx,const double ry,const double x_axis_rotation,
  unsigned int large_arc_flag,unsigned int sweep_flag,const double x,
  const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathEllipticArc(drawing_wand,AbsolutePathMode,rx,ry,x_axis_rotation,
    large_arc_flag,sweep_flag,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h E l l i p t i c A r c R e l a t i v e                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathEllipticArcRelative() draws an elliptical arc from the current
%  point to (x, y) using relative coordinates. The size and orientation
%  of the ellipse are defined by two radii (rx, ry) and an
%  xAxisRotation, which indicates how the ellipse as a whole is rotated
%  relative to the current coordinate system. The center (cx, cy) of the
%  ellipse is calculated automatically to satisfy the constraints imposed
%  by the other parameters. largeArcFlag and sweepFlag contribute to the
%  automatic calculations and help determine how the arc is drawn. If
%  largeArcFlag is true then draw the larger of the available arcs. If
%  sweepFlag is true, then draw the arc matching a clock-wise rotation.
%
%  The format of the DrawPathEllipticArcRelative method is:
%
%      void DrawPathEllipticArcRelative(DrawingWand *drawing_wand,
%        const double rx,const double ry,const double x_axis_rotation,
%        unsigned int large_arc_flag,unsigned int sweep_flag,const double x,
%        const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o rx: x radius
%
%    o ry: y radius
%
%    o x_axis_rotation: indicates how the ellipse as a whole is rotated
%                       relative to the current coordinate system
%
%    o large_arc_flag: If non-zero (true) then draw the larger of the
%                      available arcs
%
%    o sweep_flag: If non-zero (true) then draw the arc matching a
%                  clock-wise rotation
%
*/
WandExport void DrawPathEllipticArcRelative(DrawingWand *drawing_wand,
  const double rx,const double ry,const double x_axis_rotation,
  unsigned int large_arc_flag,unsigned int sweep_flag,const double x,
  const double y)
{
  DrawPathEllipticArc(drawing_wand,RelativePathMode,rx,ry,x_axis_rotation,
    large_arc_flag,sweep_flag,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h F i n i s h                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathFinish() terminates the current path.
%
%  The format of the DrawPathFinish method is:
%
%      void DrawPathFinish(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void DrawPathFinish(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  MvgPrintf(drawing_wand,"'\n");
  drawing_wand->path_operation=PathDefaultOperation;
  drawing_wand->path_mode=DefaultPathMode;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h L i n e T o A b s o l u t e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathLineToAbsolute() draws a line path from the current point to the
%  given coordinate using absolute coordinates. The coordinate then becomes
%  the new current point.
%
%  The format of the DrawPathLineToAbsolute method is:
%
%      void DrawPathLineToAbsolute(DrawingWand *drawing_wand,const double x,const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x: target x ordinate
%
%    o y: target y ordinate
%
*/
static void DrawPathLineTo(DrawingWand *drawing_wand,const PathMode mode,
  const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);

  if ((drawing_wand->path_operation != PathLineToOperation) ||
      (drawing_wand->path_mode != mode))
    {
      drawing_wand->path_operation=PathLineToOperation;
      drawing_wand->path_mode=mode;
      MvgAutoWrapPrintf(drawing_wand,"%c%.4g,%.4g",
        mode == AbsolutePathMode ? 'L' : 'l',x,y);
    }
  else
    MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g",x,y);
}

WandExport void DrawPathLineToAbsolute(DrawingWand *drawing_wand,
  const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathLineTo(drawing_wand,AbsolutePathMode,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h L i n e T o R e l a t i v e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathLineToRelative() draws a line path from the current point to the
%  given coordinate using relative coordinates. The coordinate then becomes
%  the new current point.
%
%  The format of the DrawPathLineToRelative method is:
%
%      void DrawPathLineToRelative(DrawingWand *drawing_wand,const double x,
%        const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x: target x ordinate
%
%    o y: target y ordinate
%
*/
WandExport void DrawPathLineToRelative(DrawingWand *drawing_wand,
  const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathLineTo(drawing_wand,RelativePathMode,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h L i n e T o H o r i z o n t a l A b s o l u t e           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathLineToHorizontalAbsolute() draws a horizontal line path from the
%  current point to the target point using absolute coordinates.  The target
%  point then becomes the new current point.
%
%  The format of the DrawPathLineToHorizontalAbsolute method is:
%
%      void DrawPathLineToHorizontalAbsolute(DrawingWand *drawing_wand,
%        const PathMode mode,const double x)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x: target x ordinate
%
*/

static void DrawPathLineToHorizontal(DrawingWand *drawing_wand,
  const PathMode mode,const double x)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if ((drawing_wand->path_operation != PathLineToHorizontalOperation) ||
      (drawing_wand->path_mode != mode))
    {
      drawing_wand->path_operation=PathLineToHorizontalOperation;
      drawing_wand->path_mode=mode;
      MvgAutoWrapPrintf(drawing_wand,"%c%.4g",
        mode == AbsolutePathMode ? 'H' : 'h',x);
    }
  else
    MvgAutoWrapPrintf(drawing_wand," %.4g",x);
}

WandExport void DrawPathLineToHorizontalAbsolute(DrawingWand *drawing_wand,
  const double x)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathLineToHorizontal(drawing_wand,AbsolutePathMode,x);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h L i n e T o H o r i z o n t a l R e l a t i v e           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathLineToHorizontalRelative() draws a horizontal line path from the
%  current point to the target point using relative coordinates.  The target
%  point then becomes the new current point.
%
%  The format of the DrawPathLineToHorizontalRelative method is:
%
%      void DrawPathLineToHorizontalRelative(DrawingWand *drawing_wand,
%        const double x)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x: target x ordinate
%
*/
WandExport void DrawPathLineToHorizontalRelative(DrawingWand *drawing_wand,
  const double x)
{
  DrawPathLineToHorizontal(drawing_wand,RelativePathMode,x);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h L i n e T o V e r t i c a l A b s o l u t e               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathLineToVerticalAbsolute() draws a vertical line path from the
%  current point to the target point using absolute coordinates.  The target
%  point then becomes the new current point.
%
%  The format of the DrawPathLineToVerticalAbsolute method is:
%
%      void DrawPathLineToVerticalAbsolute(DrawingWand *drawing_wand,
%        const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o y: target y ordinate
%
*/

static void DrawPathLineToVertical(DrawingWand *drawing_wand,
  const PathMode mode,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if ((drawing_wand->path_operation != PathLineToVerticalOperation) ||
      (drawing_wand->path_mode != mode))
    {
      drawing_wand->path_operation=PathLineToVerticalOperation;
      drawing_wand->path_mode=mode;
      MvgAutoWrapPrintf(drawing_wand,"%c%.4g",
        mode == AbsolutePathMode ? 'V' : 'v',y);
    }
  else
    MvgAutoWrapPrintf(drawing_wand," %.4g",y);
}

WandExport void DrawPathLineToVerticalAbsolute(DrawingWand *drawing_wand,
  const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathLineToVertical(drawing_wand,AbsolutePathMode,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h L i n e T o V e r t i c a l R e l a t i v e               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathLineToVerticalRelative() draws a vertical line path from the
%  current point to the target point using relative coordinates.  The target
%  point then becomes the new current point.
%
%  The format of the DrawPathLineToVerticalRelative method is:
%
%      void DrawPathLineToVerticalRelative(DrawingWand *drawing_wand,
%        const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o y: target y ordinate
%
*/
WandExport void DrawPathLineToVerticalRelative(DrawingWand *drawing_wand,
  const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathLineToVertical(drawing_wand,RelativePathMode,y);
}
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h M o v e T o A b s o l u t e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathMoveToAbsolute() starts a new sub-path at the given coordinate
%  using absolute coordinates. The current point then becomes the
%  specified coordinate.
%
%  The format of the DrawPathMoveToAbsolute method is:
%
%      void DrawPathMoveToAbsolute(DrawingWand *drawing_wand,const double x,
%        const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x: target x ordinate
%
%    o y: target y ordinate
%
*/
static void DrawPathMoveTo(DrawingWand *drawing_wand,const PathMode mode,
  const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if ((drawing_wand->path_operation != PathMoveToOperation) ||
      (drawing_wand->path_mode != mode))
    {
      drawing_wand->path_operation=PathMoveToOperation;
      drawing_wand->path_mode=mode;
      MvgAutoWrapPrintf(drawing_wand,"%c%.4g,%.4g",
        mode == AbsolutePathMode ? 'M' : 'm',x,y);
    }
  else
    MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g",x,y);
}

WandExport void DrawPathMoveToAbsolute(DrawingWand *drawing_wand,
  const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathMoveTo(drawing_wand,AbsolutePathMode,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h M o v e T o R e l a t i v e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathMoveToRelative() starts a new sub-path at the given coordinate
%  using relative coordinates. The current point then becomes the
%  specified coordinate.
%
%  The format of the DrawPathMoveToRelative method is:
%
%      void DrawPathMoveToRelative(DrawingWand *drawing_wand,const double x,
%        const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x: target x ordinate
%
%    o y: target y ordinate
%
*/
WandExport void DrawPathMoveToRelative(DrawingWand *drawing_wand,
  const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathMoveTo(drawing_wand,RelativePathMode,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h S t a r t                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathStart() declares the start of a path drawing list which is terminated
%  by a matching DrawPathFinish() command. All other DrawPath commands must
%  be enclosed between a DrawPathStart() and a DrawPathFinish() command. This
%  is because path drawing commands are subordinate commands and they do not
%  function by themselves.
%
%  The format of the DrawPathStart method is:
%
%      void DrawPathStart(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void DrawPathStart(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  MvgPrintf(drawing_wand,"path '");
  drawing_wand->path_operation=PathDefaultOperation;
  drawing_wand->path_mode=DefaultPathMode;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P e e k G r a p h i c W a n d                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPeekGraphicContext() returns the current graphic drawing_wand.
%
%  The format of the DrawPeekGraphicContext method is:
%
%      DrawInfo *DrawPeekGraphicContext(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport DrawInfo *DrawPeekGraphicContext(const DrawingWand *drawing_wand)
{
  DrawInfo
    *draw_info;

  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  draw_info=CloneDrawInfo((ImageInfo *) NULL,CurrentContext);
  CloneString(&draw_info->primitive,drawing_wand->mvg);
  return(draw_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P o i n t                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPoint() draws a point using the current stroke color and stroke
%  thickness at the specified coordinates.
%
%  The format of the DrawPoint method is:
%
%      void DrawPoint(DrawingWand *drawing_wand,const double x,const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x: target x coordinate
%
%    o y: target y coordinate
%
*/
WandExport void DrawPoint(DrawingWand *drawing_wand,const double x,
  const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  MvgPrintf(drawing_wand,"point %.4g,%.4g\n",x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P o l y g o n                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPolygon() draws a polygon using the current stroke, stroke width, and
%  fill color or texture, using the specified array of coordinates.
%
%  The format of the DrawPolygon method is:
%
%      void DrawPolygon(DrawingWand *drawing_wand,
%        const size_t number_coordinates,const PointInfo *coordinates)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o number_coordinates: number of coordinates
%
%    o coordinates: coordinate array
%
*/
WandExport void DrawPolygon(DrawingWand *drawing_wand,
  const size_t number_coordinates,const PointInfo *coordinates)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  MvgAppendPointsCommand(drawing_wand,"polygon",number_coordinates,coordinates);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P o l y l i n e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPolyline() draws a polyline using the current stroke, stroke width, and
%  fill color or texture, using the specified array of coordinates.
%
%  The format of the DrawPolyline method is:
%
%      void DrawPolyline(DrawingWand *drawing_wand,
%        const size_t number_coordinates,const PointInfo *coordinates)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o number_coordinates: number of coordinates
%
%    o coordinates: coordinate array
%
*/
WandExport void DrawPolyline(DrawingWand *drawing_wand,
  const size_t number_coordinates,const PointInfo *coordinates)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  MvgAppendPointsCommand(drawing_wand,"polyline",number_coordinates,
    coordinates);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P o p C l i p P a t h                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPopClipPath() terminates a clip path definition.
%
%  The format of the DrawPopClipPath method is:
%
%      void DrawPopClipPath(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void DrawPopClipPath(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->indent_depth > 0)
    drawing_wand->indent_depth--;
  MvgPrintf(drawing_wand,"pop clip-path\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P o p D e f s                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPopDefs() terminates a definition list
%
%  The format of the DrawPopDefs method is:
%
%      void DrawPopDefs(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void DrawPopDefs(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->indent_depth > 0)
    drawing_wand->indent_depth--;
  MvgPrintf(drawing_wand,"pop defs\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P o p G r a p h i c W a n d                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPopGraphicContext() destroys the current drawing_wand returning to the
%  previously pushed drawing wand. Multiple drawing wand  may exist. It is an
%  error to attempt to pop more drawing_wands than have been pushed, and it is
%  proper form to pop all drawing_wands which have been pushed.
%
%  The format of the DrawPopGraphicContext method is:
%
%      void DrawPopGraphicContext(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void DrawPopGraphicContext(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->index <= 0)
    {
      ThrowException(&drawing_wand->image->exception,DrawError,
        UnbalancedGraphicContextPushPop,NULL);
    }
  else
    {
      /* Destroy clip path if not same in preceding drawing_wand */
#if DRAW_BINARY_IMPLEMENTATION
      if (CurrentContext->clip_path != (char *) NULL)
        if (LocaleCompare(CurrentContext->clip_path,
            drawing_wand->graphic_context[drawing_wand->index-1]->clip_path) != 0)
          (void) SetImageClipMask(drawing_wand->image,(Image *) NULL);
#endif

      DestroyDrawInfo(CurrentContext);
      CurrentContext=(DrawInfo*)NULL;
      drawing_wand->index--;
      if (drawing_wand->indent_depth > 0)
        drawing_wand->indent_depth--;
      MvgPrintf(drawing_wand,"pop graphic-context\n");
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P o p P a t t e r n                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPopPattern() terminates a pattern definition.
%
%  The format of the DrawPopPattern method is:
%
%      void DrawPopPattern(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void DrawPopPattern(DrawingWand *drawing_wand)
{
  char
    geometry[MaxTextExtent],
    key[MaxTextExtent];

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->pattern_id == NULL)
    ThrowException(&drawing_wand->image->exception,DrawWarning,
      NotCurrentlyPushingPatternDefinition,NULL);
  (void) FormatMagickString(key,MaxTextExtent,"[%.1024s]",
    drawing_wand->pattern_id);
  (void) SetImageAttribute(drawing_wand->image,key,
    drawing_wand->mvg+drawing_wand->pattern_offset);
  (void) FormatMagickString(geometry,MaxTextExtent,"%lux%lu%+ld%+ld",
    drawing_wand->pattern_bounds.width,drawing_wand->pattern_bounds.height,
    drawing_wand->pattern_bounds.x,drawing_wand->pattern_bounds.y);
  (void) SetImageAttribute(drawing_wand->image,key,geometry);
  drawing_wand->pattern_id=(char *) RelinquishMagickMemory(drawing_wand->pattern_id);
  drawing_wand->pattern_id=NULL;
  drawing_wand->pattern_offset=0;
  drawing_wand->pattern_bounds.x=0;
  drawing_wand->pattern_bounds.y=0;
  drawing_wand->pattern_bounds.width=0;
  drawing_wand->pattern_bounds.height=0;
  drawing_wand->filter_off=False;
  if (drawing_wand->indent_depth > 0)
    drawing_wand->indent_depth--;
  MvgPrintf(drawing_wand,"pop pattern\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P u s h C l i p P a t h                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPushClipPath() starts a clip path definition which is comprized of
%  any number of drawing commands and terminated by a DrawPopClipPath()
%  command.
%
%  The format of the DrawPushClipPath method is:
%
%      void DrawPushClipPath(DrawingWand *drawing_wand,const char *clip_path_id)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o clip_path_id: string identifier to associate with the clip path for
%      later use.
%
*/
WandExport void DrawPushClipPath(DrawingWand *drawing_wand,
  const char *clip_path_id)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(clip_path_id != (const char *) NULL);
  MvgPrintf(drawing_wand,"push clip-path %s\n",clip_path_id);
  drawing_wand->indent_depth++;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P u s h D e f s                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPushDefs() indicates that commands up to a terminating DrawPopDefs()
%  command create named elements (e.g. clip-paths, textures, etc.) which
%  may safely be processed earlier for the sake of efficiency.
%
%  The format of the DrawPushDefs method is:
%
%      void DrawPushDefs(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void DrawPushDefs(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  MvgPrintf(drawing_wand,"push defs\n");
  drawing_wand->indent_depth++;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P u s h G r a p h i c W a n d                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPushGraphicContext() clones the current drawing wand to create a
%  new drawing wand. The original drawing drawing_wand(s) may be returned to
%  by invoking DrawPopGraphicContext().  The drawing wands are stored on a
%  drawing wand stack.  For every Pop there must have already been an
%  equivalent Push.
%
%  The format of the DrawPushGraphicContext method is:
%
%      void DrawPushGraphicContext(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void DrawPushGraphicContext(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  drawing_wand->index++;
  drawing_wand->graphic_context=(DrawInfo **) ResizeMagickMemory(
    drawing_wand->graphic_context,(drawing_wand->index+1)*sizeof(DrawInfo *));
  if (drawing_wand->graphic_context == (DrawInfo **) NULL)
    ThrowException3(&drawing_wand->image->exception,ResourceLimitError,
      MemoryAllocationFailed,UnableToDrawOnImage);
  CurrentContext=CloneDrawInfo((ImageInfo *) NULL,
    drawing_wand->graphic_context[drawing_wand->index-1]);
  MvgPrintf(drawing_wand,"push graphic-context\n");
  drawing_wand->indent_depth++;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P u s h P a t t e r n                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPushPattern() indicates that subsequent commands up to a
%  DrawPopPattern() command comprise the definition of a named pattern.
%  The pattern space is assigned top left corner coordinates, a width
%  and height, and becomes its own drawing space.  Anything which can
%  be drawn may be used in a pattern definition.
%  Named patterns may be used as stroke or brush definitions.
%
%  The format of the DrawPushPattern method is:
%
%      void DrawPushPattern(DrawingWand *drawing_wand,const char *pattern_id,
%        const double x,const double y,const double width,const double height)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o pattern_id: pattern identification for later reference
%
%    o x: x ordinate of top left corner
%
%    o y: y ordinate of top left corner
%
%    o width: width of pattern space
%
%    o height: height of pattern space
%
*/
WandExport void DrawPushPattern(DrawingWand *drawing_wand,
  const char *pattern_id,const double x,const double y,const double width,
  const double height)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(pattern_id != (const char *) NULL);
  if (drawing_wand->pattern_id != NULL)
    ThrowException(&drawing_wand->image->exception,DrawError,
      AlreadyPushingPatternDefinition,drawing_wand->pattern_id);
  drawing_wand->filter_off=True;
  MvgPrintf(drawing_wand,"push pattern %s %.4g,%.4g %.4g,%.4g\n",pattern_id,
    x,y,width,height);
  drawing_wand->indent_depth++;
  drawing_wand->pattern_id=AcquireString(pattern_id);
  drawing_wand->pattern_bounds.x=(long) ceil(x-0.5);
  drawing_wand->pattern_bounds.y=(long) ceil(y-0.5);
  drawing_wand->pattern_bounds.width=(unsigned long) (width+0.5);
  drawing_wand->pattern_bounds.height=(unsigned long) (height+0.5);
  drawing_wand->pattern_offset=drawing_wand->mvg_length;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w R e c t a n g l e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawRectangle() draws a rectangle given two coordinates and using
%  the current stroke, stroke width, and fill settings.
%
%  The format of the DrawRectangle method is:
%
%      void DrawRectangle(DrawingWand *drawing_wand,const double x1,
%        const double y1,const double x2,const double y2)
%
%  A description of each parameter follows:
%
%    o x1: x ordinate of first coordinate
%
%    o y1: y ordinate of first coordinate
%
%    o x2: x ordinate of second coordinate
%
%    o y2: y ordinate of second coordinate
%
*/
WandExport void DrawRectangle(DrawingWand *drawing_wand,const double x1,
  const double y1,const double x2,const double y2)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  MvgPrintf(drawing_wand,"rectangle %.4g,%.4g %.4g,%.4g\n",x1,y1,x2,y2);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w R e n d e r                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawRender() renders all preceding drawing commands onto the image.
%
%  The format of the DrawRender method is:
%
%      unsigned int DrawRender(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport unsigned int DrawRender(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  CurrentContext->primitive=drawing_wand->mvg;
  (void) LogMagickEvent(DrawEvent,GetMagickModule(),"MVG:\n'%s'\n",
    drawing_wand->mvg);
  DrawImage(drawing_wand->image, CurrentContext);
  CurrentContext->primitive=(char *) NULL;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w R o t a t e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawRotate() applies the specified rotation to the current coordinate space.
%
%  The format of the DrawRotate method is:
%
%      void DrawRotate(DrawingWand *drawing_wand,const double degrees)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o degrees: degrees of rotation
%
*/
WandExport void DrawRotate(DrawingWand *drawing_wand,const double degrees)
{
  AffineMatrix
    affine;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  GetAffineMatrix(&affine);
  affine.sx=cos(DegreesToRadians(fmod(degrees,360.0)));
  affine.rx=sin(DegreesToRadians(fmod(degrees,360.0)));
  affine.ry=(-sin(DegreesToRadians(fmod(degrees,360.0))));
  affine.sy=cos(DegreesToRadians(fmod(degrees,360.0)));
  AdjustAffine(drawing_wand,&affine);
  MvgPrintf(drawing_wand,"rotate %.4g\n",degrees);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w R o u n d R e c t a n g l e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawRoundRectangle() draws a rounted rectangle given two coordinates,
%  x & y corner radiuses and using the current stroke, stroke width,
%  and fill settings.
%
%  The format of the DrawRoundRectangle method is:
%
%      void DrawRoundRectangle(DrawingWand *drawing_wand,double x1,double y1,
%        double x2,double y2,double rx,double ry)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x1: x ordinate of first coordinate
%
%    o y1: y ordinate of first coordinate
%
%    o x2: x ordinate of second coordinate
%
%    o y2: y ordinate of second coordinate
%
%    o rx: radius of corner in horizontal direction
%
%    o ry: radius of corner in vertical direction
%
*/
WandExport void DrawRoundRectangle(DrawingWand *drawing_wand,double x1,
  double y1,double x2,double y2,double rx,double ry)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  MvgPrintf(drawing_wand,"roundrectangle %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
    x1,y1,x2,y2,rx,ry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S c a l e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawScale() adjusts the scaling factor to apply in the horizontal and
%  vertical directions to the current coordinate space.
%
%  The format of the DrawScale method is:
%
%      void DrawScale(DrawingWand *drawing_wand,const double x,const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x: horizontal scale factor
%
%    o y: vertical scale factor
%
*/
WandExport void DrawScale(DrawingWand *drawing_wand,const double x,
  const double y)
{
  AffineMatrix
    affine;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  GetAffineMatrix(&affine);
  affine.sx=x;
  affine.sy=y;
  AdjustAffine( drawing_wand, &affine );
  MvgPrintf(drawing_wand,"scale %.4g,%.4g\n",x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S k e w X                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSkewX() skews the current coordinate system in the horizontal
%  direction.
%
%  The format of the DrawSkewX method is:
%
%      void DrawSkewX(DrawingWand *drawing_wand,const double degrees)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o degrees: number of degrees to skew the coordinates
%
*/
WandExport void DrawSkewX(DrawingWand *drawing_wand,const double degrees)
{
  AffineMatrix
    affine;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  GetAffineMatrix(&affine);
  affine.ry=tan(DegreesToRadians(fmod(degrees,360.0)));
  AdjustAffine(drawing_wand,&affine);
  MvgPrintf(drawing_wand,"skewX %.4g\n",degrees);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S k e w Y                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSkewY() skews the current coordinate system in the vertical
%  direction.
%
%  The format of the DrawSkewY method is:
%
%      void DrawSkewY(DrawingWand *drawing_wand,const double degrees)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o degrees: number of degrees to skew the coordinates
%
*/
WandExport void DrawSkewY(DrawingWand *drawing_wand,const double degrees)
{
  AffineMatrix
    affine;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  GetAffineMatrix(&affine);
  affine.rx=tan(DegreesToRadians(fmod(degrees,360.0)));
  DrawAffine(drawing_wand,&affine);
  MvgPrintf(drawing_wand,"skewY %.4g\n",degrees);
}
#if 0

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t o p C o l o r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStopColor() sets the stop color and offset for gradients
%
%  The format of the DrawSetStopColor method is:
%
%      void DrawSetStopColor(DrawingWand *drawing_wand,
%        const PixelPacket *stop_color,const double offset)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o stop_color:
%
%    o offset:
%
*/
/* This is gradient stuff so it shouldn't be supported yet */
WandExport void DrawSetStopColor(DrawingWand *drawing_wand,
  const PixelPacket * stop_color,const double offset)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(stop_color != (const PixelPacket *) NULL);
  MvgPrintf(drawing_wand,"stop-color ");
  MvgAppendColor(drawing_wand,stop_color);
  MvgPrintf(drawing_wand,"\n");
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e C o l o r                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeColor() returns the color used for stroking object outlines.
%
%  The format of the DrawGetStrokeColor method is:
%
%      void DrawGetStrokeColor(const DrawingWand *drawing_wand,
$        PixelWand *stroke_color)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o stroke_color: Return the stroke color.
%
*/
WandExport void DrawGetStrokeColor(const DrawingWand *drawing_wand,
  PixelWand *stroke_color)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  PixelSetQuantumColor(stroke_color,&CurrentContext->stroke);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e C o l o r                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeColor() sets the color used for stroking object outlines.
%
%  The format of the DrawSetStrokeColor method is:
%
%      void DrawSetStrokeColor(DrawingWand *drawing_wand,
%        const PixelWand *stroke_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o stroke_wand: stroke wand.
%
*/
WandExport void DrawSetStrokeColor(DrawingWand *drawing_wand,
  const PixelWand *stroke_wand)
{
  PixelPacket
    *current_stroke,
    new_stroke,
    stroke_color;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(stroke_wand != (const PixelWand *) NULL);
  PixelGetQuantumColor(stroke_wand,&stroke_color);
  new_stroke=stroke_color;
  if (new_stroke.opacity != TransparentOpacity)
    new_stroke.opacity=CurrentContext->opacity;
  current_stroke=&CurrentContext->stroke;
  if (drawing_wand->filter_off ||
      !WandColorMatch(current_stroke,&new_stroke))
    {
      CurrentContext->stroke=new_stroke;
      MvgPrintf(drawing_wand,"stroke '");
      MvgAppendColor(drawing_wand,&stroke_color);
      MvgPrintf(drawing_wand,"'\n");
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e P a t t e r n U R L                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokePatternURL() sets the pattern used for stroking object outlines.
%
%  The format of the DrawSetStrokePatternURL method is:
%
%      void DrawSetStrokePatternURL(DrawingWand *drawing_wand,
%        const char *stroke_url)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o stroke_url: URL specifying pattern ID (e.g. "#pattern_id")
%
*/
WandExport void DrawSetStrokePatternURL(DrawingWand *drawing_wand,
  const char *stroke_url)
{
  char
    pattern[MaxTextExtent];

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(stroke_url != NULL);
  if (stroke_url[0] != '#')
    ThrowException(&drawing_wand->image->exception,DrawWarning,
      NotARelativeURL,stroke_url);
  (void) FormatMagickString(pattern,MaxTextExtent,"[%.1024s]",stroke_url+1);
  if (GetImageAttribute(drawing_wand->image,pattern) == (ImageAttribute *) NULL)
    {
      ThrowException(&drawing_wand->image->exception,DrawWarning,
        URLNotFound,stroke_url);
    }
  else
    {
      char
        pattern_spec[MaxTextExtent];

      (void) FormatMagickString(pattern_spec,MaxTextExtent,"url(%.1024s)",
        stroke_url);
#if DRAW_BINARY_IMPLEMENTATION
      DrawPatternPath(drawing_wand->image,CurrentContext,pattern_spec,&CurrentContext->stroke_pattern);
#endif
      if (CurrentContext->stroke.opacity != TransparentOpacity)
        CurrentContext->stroke.opacity=CurrentContext->opacity;
      MvgPrintf(drawing_wand,"stroke %s\n",pattern_spec);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e A n t i a l i a s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeAntialias() returns the current stroke antialias setting.
%  Stroked outlines are antialiased by default.  When antialiasing is disabled
%  stroked pixels are thresholded to determine if the stroke color or
%  underlying canvas color should be used.
%
%  The format of the DrawGetStrokeAntialias method is:
%
%      unsigned int DrawGetStrokeAntialias(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport unsigned int DrawGetStrokeAntialias(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(CurrentContext->stroke_antialias);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e A n t i a l i a s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeAntialias() controls whether stroked outlines are antialiased.
%  Stroked outlines are antialiased by default.  When antialiasing is disabled
%  stroked pixels are thresholded to determine if the stroke color or
%  underlying canvas color should be used.
%
%  The format of the DrawSetStrokeAntialias method is:
%
%      void DrawSetStrokeAntialias(DrawingWand *drawing_wand,
%        const unsigned int stroke_antialias)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o stroke_antialias: set to false (zero) to disable antialiasing
%
*/
WandExport void DrawSetStrokeAntialias(DrawingWand *drawing_wand,
  const unsigned int stroke_antialias)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off ||
     (CurrentContext->stroke_antialias != stroke_antialias))
    {
      CurrentContext->stroke_antialias=stroke_antialias;
      MvgPrintf(drawing_wand,"stroke-antialias %i\n",stroke_antialias ? 1 : 0);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e D a s h A r r a y                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeDashArray() returns an array representing the pattern of
%  dashes and gaps used to stroke paths (see DrawSetStrokeDashArray). The
%  array must be freed once it is no longer required by the user.
%
%  The format of the DrawGetStrokeDashArray method is:
%
%      double *DrawGetStrokeDashArray(const DrawingWand *drawing_wand,
%        size_t *number_elements)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o number_elements: address to place number of elements in dash array
%
% */
WandExport double *DrawGetStrokeDashArray(const DrawingWand *drawing_wand,
  size_t *number_elements)
{
  register const double
    *p;

  register double
    *q;

  double
    *dash_array;

  unsigned int
    i,
    n=0;

  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(number_elements != (size_t *)NULL);
  p=CurrentContext->dash_pattern;
  if ( p != (const double *) NULL )
    while (*p++ != 0)
      n++;
  *number_elements=n;
  dash_array=(double *)NULL;
  if (n != 0)
    {
      dash_array=(double *) AcquireMagickMemory(n*sizeof(double));
      p=CurrentContext->dash_pattern;
      q=dash_array;
      i=n;
      while (i--)
        *q++=(*p++);
    }
  return(dash_array);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e D a s h A r r a y                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeDashArray() specifies the pattern of dashes and gaps used to
%  stroke paths. The strokeDashArray represents an array of numbers that
%  specify the lengths of alternating dashes and gaps in pixels. If an odd
%  number of values is provided, then the list of values is repeated to yield
%  an even number of values. To remove an existing dash array, pass a zero
%  number_elements argument and null dash_array.
%  A typical strokeDashArray_ array might contain the members 5 3 2.
%
%  The format of the DrawSetStrokeDashArray method is:
%
%      void DrawSetStrokeDashArray(DrawingWand *drawing_wand,
%        const size_t number_elements,const double *dash_array)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o number_elements: number of elements in dash array
%
%    o dash_array: dash array values
%
% */
WandExport void DrawSetStrokeDashArray(DrawingWand *drawing_wand,
  const size_t number_elements,const double *dash_array)
{
  register const double
    *p;

  register double
    *q;

  unsigned int
    i,
    updated=False,
    n_new=number_elements,
    n_old=0;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  q=CurrentContext->dash_pattern;
  if (q != (const double *) NULL)
    while (*q++ != 0)
      n_old++;
  if ((n_old == 0) && (n_new == 0))
    {
      updated=False;
    }
  else if ( n_old != n_new )
    {
      updated=True;
    }
  else if ((CurrentContext->dash_pattern != (double *) NULL) &&
           (dash_array != (double *) NULL))
    {
      p=dash_array;
      q=CurrentContext->dash_pattern;
      i=n_new;
      while ( i-- )
      {
        if (AbsoluteValue(*p - *q) > MagickEpsilon)
          {
            updated=True;
            break;
          }
        p++;
        q++;
      }
    }
  if (drawing_wand->filter_off || updated)
    {
      if (CurrentContext->dash_pattern != (double *) NULL)
        CurrentContext->dash_pattern=(double *)
          RelinquishMagickMemory(CurrentContext->dash_pattern);
      if (n_new != 0)
        {
          CurrentContext->dash_pattern=(double *)
            AcquireMagickMemory((n_new+1)*sizeof(double));
          if (!CurrentContext->dash_pattern)
            {
              ThrowException3(&drawing_wand->image->exception,
                ResourceLimitError,MemoryAllocationFailed,UnableToDrawOnImage);
            }
          else
            {
              q=CurrentContext->dash_pattern;
              p=dash_array;
              while (*p)
                *q++=*p++;
              *q=0;
            }
        }
      MvgPrintf(drawing_wand,"stroke-dash_array ");
      if ( n_new == 0 )
        MvgPrintf(drawing_wand, "none");
      else
        {
          p=dash_array;
          i=n_new;
          MvgPrintf(drawing_wand,"%.4g",*p++);
          while (i--)
            MvgPrintf(drawing_wand,",%.4g",*p++);
        }
      MvgPrintf(drawing_wand,"0 \n");
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e D a s h O f f s e t                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeDashOffset() returns the offset into the dash pattern to
%  start the dash.
%
%  The format of the DrawGetStrokeDashOffset method is:
%
%      double DrawGetStrokeDashOffset(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport double DrawGetStrokeDashOffset(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(CurrentContext->dash_offset);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e D a s h O f f s e t                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeDashOffset() specifies the offset into the dash pattern to
%  start the dash.
%
%  The format of the DrawSetStrokeDashOffset method is:
%
%      void DrawSetStrokeDashOffset(DrawingWand *drawing_wand,
%        const double dash_offset)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o dash_offset: dash offset
%
*/
WandExport void DrawSetStrokeDashOffset(DrawingWand *drawing_wand,
  const double dash_offset)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);

  if (drawing_wand->filter_off ||
     (AbsoluteValue(CurrentContext->dash_offset-dash_offset) > MagickEpsilon))
    {
      CurrentContext->dash_offset=dash_offset;
      MvgPrintf(drawing_wand,"stroke-dashoffset %.4g\n",dash_offset);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e L i n e C a p                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeLineCap() returns the shape to be used at the end of
%  open subpaths when they are stroked. Values of LineCap are
%  UndefinedCap, ButtCap, RoundCap, and SquareCap.
%
%  The format of the DrawGetStrokeLineCap method is:
%
%      LineCap DrawGetStrokeLineCap(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
% */
WandExport LineCap DrawGetStrokeLineCap(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(CurrentContext->linecap);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e L i n e C a p                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeLineCap() specifies the shape to be used at the end of
%  open subpaths when they are stroked. Values of LineCap are
%  UndefinedCap, ButtCap, RoundCap, and SquareCap.
%
%  The format of the DrawSetStrokeLineCap method is:
%
%      void DrawSetStrokeLineCap(DrawingWand *drawing_wand,
%        const LineCap linecap)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o linecap: linecap style
%
% */
WandExport void DrawSetStrokeLineCap(DrawingWand *drawing_wand,
  const LineCap linecap)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);

  if (drawing_wand->filter_off || (CurrentContext->linecap != linecap))
    {
      const char
        *p=NULL;

      CurrentContext->linecap=linecap;
      switch (linecap)
      {
        case ButtCap:
          p="butt";
          break;
        case RoundCap:
          p="round";
          break;
        case SquareCap:
          p="square";
          break;
        default:
          break;
      }
      if (p != NULL)
        MvgPrintf(drawing_wand,"stroke-linecap %s\n",p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e L i n e J o i n                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeLineJoin() returns the shape to be used at the
%  corners of paths (or other vector shapes) when they are
%  stroked. Values of LineJoin are UndefinedJoin, MiterJoin, RoundJoin,
%  and BevelJoin.
%
%  The format of the DrawGetStrokeLineJoin method is:
%
%      LineJoin DrawGetStrokeLineJoin(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
% */
WandExport LineJoin DrawGetStrokeLineJoin(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(CurrentContext->linejoin);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e L i n e J o i n                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeLineJoin() specifies the shape to be used at the
%  corners of paths (or other vector shapes) when they are
%  stroked. Values of LineJoin are UndefinedJoin, MiterJoin, RoundJoin,
%  and BevelJoin.
%
%  The format of the DrawSetStrokeLineJoin method is:
%
%      void DrawSetStrokeLineJoin(DrawingWand *drawing_wand,
%        const LineJoin linejoin)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o linejoin: line join style
%
%
*/
WandExport void DrawSetStrokeLineJoin(DrawingWand *drawing_wand,
  const LineJoin linejoin)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off || (CurrentContext->linejoin != linejoin))
    {
      const char
        *p=NULL;

      CurrentContext->linejoin=linejoin;

      switch (linejoin)
      {
        case MiterJoin:
          p="miter";
          break;
        case RoundJoin:
          p="round";
          break;
        case BevelJoin:
          p="square";
          break;
        default:
          break;
      }
      if (p != NULL)
        MvgPrintf(drawing_wand,"stroke-linejoin %s\n",p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e M i t e r L i m i t                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeMiterLimit() returns the miter limit. When two line
%  segments meet at a sharp angle and miter joins have been specified for
%  'lineJoin', it is possible for the miter to extend far beyond the
%  thickness of the line stroking the path. The miterLimit' imposes a
%  limit on the ratio of the miter length to the 'lineWidth'.
%
%  The format of the DrawGetStrokeMiterLimit method is:
%
%      unsigned long DrawGetStrokeMiterLimit(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
% */
WandExport unsigned long DrawGetStrokeMiterLimit(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return CurrentContext->miterlimit;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e M i t e r L i m i t                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeMiterLimit() specifies the miter limit. When two line
%  segments meet at a sharp angle and miter joins have been specified for
%  'lineJoin', it is possible for the miter to extend far beyond the
%  thickness of the line stroking the path. The miterLimit' imposes a
%  limit on the ratio of the miter length to the 'lineWidth'.
%
%  The format of the DrawSetStrokeMiterLimit method is:
%
%      void DrawSetStrokeMiterLimit(DrawingWand *drawing_wand,
%        const unsigned long miterlimit)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o miterlimit: miter limit
%
% */
WandExport void DrawSetStrokeMiterLimit(DrawingWand *drawing_wand,
  const unsigned long miterlimit)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (CurrentContext->miterlimit != miterlimit)
    {
      CurrentContext->miterlimit=miterlimit;
      MvgPrintf(drawing_wand,"stroke-miterlimit %lu\n",miterlimit);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e O p a c i t y                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeOpacity() returns the opacity of stroked object outlines.
%
%  The format of the DrawGetStrokeOpacity method is:
%
%      double DrawGetStrokeOpacity(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport double DrawGetStrokeOpacity(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(1.0-(((double)CurrentContext->stroke.opacity)/MaxRGB));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e O p a c i t y                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeOpacity() specifies the opacity of stroked object outlines.
%
%  The format of the DrawSetStrokeOpacity method is:
%
%      void DrawSetStrokeOpacity(DrawingWand *drawing_wand,
%        const double stroke_opacity)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o stroke_opacity: stroke opacity.  The value 1.0 is opaque.
%
*/
WandExport void DrawSetStrokeOpacity(DrawingWand *drawing_wand,
  const double stroke_opacity)
{
  double
    opacity;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  opacity=(Quantum) ((double) MaxRGB*
    (1.0-(stroke_opacity <= 1.0 ? stroke_opacity : 1.0 ))+0.5);
  if (drawing_wand->filter_off || (CurrentContext->stroke.opacity != opacity))
    {
      CurrentContext->stroke.opacity=(Quantum) (opacity+0.5);
      MvgPrintf(drawing_wand,"stroke-opacity %.4g\n",stroke_opacity);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e W i d t h                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeWidth() returns the width of the stroke used to draw object
%  outlines.
%
%  The format of the DrawGetStrokeWidth method is:
%
%      double DrawGetStrokeWidth(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport double DrawGetStrokeWidth(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(CurrentContext->stroke_width);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e W i d t h                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeWidth() sets the width of the stroke used to draw object
%  outlines.
%
%  The format of the DrawSetStrokeWidth method is:
%
%      void DrawSetStrokeWidth(DrawingWand *drawing_wand,
%        const double stroke_width)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o stroke_width: stroke width
%
*/
WandExport void DrawSetStrokeWidth(DrawingWand *drawing_wand,
  const double stroke_width)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off ||
     (AbsoluteValue(CurrentContext->stroke_width-stroke_width) > MagickEpsilon))
    {
      CurrentContext->stroke_width=stroke_width;
      MvgPrintf(drawing_wand,"stroke-width %.4g\n",stroke_width);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t T e x t A n t i a l i a s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetTextAntialias() returns the current text antialias setting, which
%  determines whether text is antialiased.  Text is antialiased by default.
%
%  The format of the DrawGetTextAntialias method is:
%
%      unsigned int DrawGetTextAntialias(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport unsigned int DrawGetTextAntialias(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(CurrentContext->text_antialias);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t T e x t A n t i a l i a s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetTextAntialias() controls whether text is antialiased.  Text is
%  antialiased by default.
%
%  The format of the DrawSetTextAntialias method is:
%
%      void DrawSetTextAntialias(DrawingWand *drawing_wand,
%        const unsigned int text_antialias)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o text_antialias: antialias boolean. Set to false (0) to disable
%      antialiasing.
%
*/
WandExport void DrawSetTextAntialias(DrawingWand *drawing_wand,
  const unsigned int text_antialias)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off ||
      (CurrentContext->text_antialias != text_antialias))
    {
      CurrentContext->text_antialias=text_antialias;
      MvgPrintf(drawing_wand,"text-antialias %i\n",text_antialias ? 1 : 0);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t T e x t D e c o r a t i o n                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetTextDecoration() returns the decoration applied when annotating with
%  text.
%
%  The format of the DrawGetTextDecoration method is:
%
%      DecorationType DrawGetTextDecoration(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport DecorationType DrawGetTextDecoration(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  return(CurrentContext->decorate);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t T e x t D e c o r a t i o n                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetTextDecoration() specifies a decoration to be applied when
%  annotating with text.
%
%  The format of the DrawSetTextDecoration method is:
%
%      void DrawSetTextDecoration(DrawingWand *drawing_wand,
%        const DecorationType decoration)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o decoration: text decoration.  One of NoDecoration, UnderlineDecoration,
%      OverlineDecoration, or LineThroughDecoration
%
*/
WandExport void DrawSetTextDecoration(DrawingWand *drawing_wand,
  const DecorationType decoration)
{
  const char
    *p=NULL;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off || (CurrentContext->decorate != decoration))
    {
      CurrentContext->decorate=decoration;
      switch (decoration)
      {
        case NoDecoration:
          p="none";
          break;
        case UnderlineDecoration:
          p="underline";
          break;
        case OverlineDecoration:
          p="overline";
          break;
        case LineThroughDecoration:
          p="line-through";
          break;
      }
      if (p != NULL)
        MvgPrintf(drawing_wand,"decorate %s\n",p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t T e x t E n c o d i n g                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetTextEncoding() returns a null-terminated string which specifies the
%  code set used for text annotations. The string must be freed by the user
%  once it is no longer required.
%
%  The format of the DrawGetTextEncoding method is:
%
%      char *DrawGetTextEncoding(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
% */
WandExport char *DrawGetTextEncoding(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (CurrentContext->encoding != (char *)NULL)
    return((char *) AcquireString(CurrentContext->encoding));
  return((char *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t T e x t E n c o d i n g                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetTextEncoding() specifies specifies the code set to use for
%  text annotations. The only character encoding which may be specified
%  at this time is "UTF-8" for representing Unicode as a sequence of
%  bytes. Specify an empty string to set text encoding to the system's
%  default. Successful text annotation using Unicode may require fonts
%  designed to support Unicode.
%
%  The format of the DrawSetTextEncoding method is:
%
%      void DrawSetTextEncoding(DrawingWand *drawing_wand,const char *encoding)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o encoding: character string specifying text encoding
%
% */
WandExport void DrawSetTextEncoding(DrawingWand *drawing_wand,
  const char *encoding)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(encoding != (char *) NULL);
  if (drawing_wand->filter_off || (CurrentContext->encoding == (char *) NULL) ||
      (LocaleCompare(CurrentContext->encoding,encoding) != 0))
    {
      CloneString(&CurrentContext->encoding,encoding);
      MvgPrintf(drawing_wand,"encoding '%s'\n",encoding);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t T e x t U n d e r C o l o r                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetTextUnderColor() returns the color of a background rectangle
%  to place under text annotations.
%
%  The format of the DrawGetTextUnderColor method is:
%
%      void DrawGetTextUnderColor(const DrawingWand *drawing_wand,
%        PixelWand *under_color)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o under_color: Return the under color.
%
*/
WandExport void DrawGetTextUnderColor(const DrawingWand *drawing_wand,
  PixelWand *under_color)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  PixelSetQuantumColor(under_color,&CurrentContext->undercolor);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t T e x t U n d e r C o l o r                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetTextUnderColor() specifies the color of a background rectangle
%  to place under text annotations.
%
%  The format of the DrawSetTextUnderColor method is:
%
%      void DrawSetTextUnderColor(DrawingWand *drawing_wand,
%        const PixelWand *under_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o under_wand.: text under wand.
%
*/
WandExport void DrawSetTextUnderColor(DrawingWand *drawing_wand,
  const PixelWand *under_wand)
{
  PixelPacket
    under_color;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(under_wand != (const PixelWand *) NULL);
  PixelGetQuantumColor(under_wand,&under_color);
  if (drawing_wand->filter_off ||
      !WandColorMatch(&CurrentContext->undercolor,&under_color))
    {
      CurrentContext->undercolor=under_color;
      MvgPrintf(drawing_wand,"text-undercolor '");
      MvgAppendColor(drawing_wand,&under_color);
      MvgPrintf(drawing_wand,"'\n");
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w T r a n s l a t e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawTranslate() applies a translation to the current coordinate
%  system which moves the coordinate system origin to the specified
%  coordinate.
%
%  The format of the DrawTranslate method is:
%
%      void DrawTranslate(DrawingWand *drawing_wand,const double x,
%        const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x: new x ordinate for coordinate system origin
%
%    o y: new y ordinate for coordinate system origin
%
*/
WandExport void DrawTranslate(DrawingWand *drawing_wand,const double x,
  const double y)
{
  AffineMatrix
    affine;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  GetAffineMatrix(&affine);
  affine.tx=x;
  affine.ty=y;
  AdjustAffine(drawing_wand,&affine );
  MvgPrintf(drawing_wand,"translate %.4g,%.4g\n",x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t V i e w b o x                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetViewbox() sets the overall canvas size to be recorded with the
%  drawing vector data.  Usually this will be specified using the same
%  size as the canvas image.  When the vector data is saved to SVG or MVG
%  formats, the viewbox is use to specify the size of the canvas image that
%  a viewer will render the vector data on.
%
%  The format of the DrawSetViewbox method is:
%
%      void DrawSetViewbox(DrawingWand *drawing_wand,unsigned long x1,
%        unsigned long y1,unsigned long x2,unsigned long y2)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x1: left x ordinate
%
%    o y1: top y ordinate
%
%    o x2: right x ordinate
%
%    o y2: bottom y ordinate
%
*/
WandExport void DrawSetViewbox(DrawingWand *drawing_wand,unsigned long x1,
  unsigned long y1,unsigned long x2,unsigned long y2)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  MvgPrintf(drawing_wand,"viewbox %lu %lu %lu %lu\n",x1,y1,x2,y2);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N e w D r a w W a n d                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  NewDrawingWand() returns a draw wand required for all other methods in
%  the API.
%
%  The format of the NewDrawingWand method is:
%
%      DrawingWand NewDrawingWand(void)
%
%
*/
WandExport DrawingWand *NewDrawingWand(void)
{
  Image
    *image;

  image=AllocateImage((const ImageInfo *) NULL);
  return(DrawAllocateWand((const DrawInfo *) NULL,image));
}
