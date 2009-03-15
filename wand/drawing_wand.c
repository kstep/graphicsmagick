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
#include "magick/enum_strings.h"
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

static const char *dummy_image_filename = "[MagickDrawingWandDummyImage]";


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
  buffer[sizeof(buffer)-1]=0;
  if (formatted_length < 0)
    {
    ThrowException(&drawing_wand->image->exception,DrawError,
      UnableToPrint,format);
    }
  else
    {
      if (((drawing_wand->mvg_width + formatted_length) > 78) &&
          (buffer[formatted_length-1] != '\n'))
        (void) MvgPrintf(drawing_wand, "\n");
      (void) MvgPrintf(drawing_wand,"%s",buffer);
    }
  return(formatted_length);
}

static void MvgAppendColor(DrawingWand *drawing_wand, const PixelPacket *color)
{
  if (color->red == 0U && color->green == 0U && color->blue == 0U &&
     color->opacity == TransparentOpacity)
    (void) MvgPrintf(drawing_wand,"none");
  else
    {
      char
        tuple[MaxTextExtent];

      GetColorTuple(color,drawing_wand->image->depth,drawing_wand->image->matte,
        True,tuple);
      (void) MvgPrintf(drawing_wand,"%.1024s",tuple);
    }
}

static void MvgAppendPointsCommand(DrawingWand *drawing_wand,
  char *command,const unsigned long number_coordinates,const PointInfo *coordinates)
{
  const PointInfo
    *coordinate;

  size_t
    i;

  (void) MvgPrintf(drawing_wand, "%s", command);
  for (i=number_coordinates, coordinate=coordinates; i; i--)
    {
      (void) MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g",coordinate->x,coordinate->y);
      coordinate++;
    }
  (void) MvgPrintf(drawing_wand, "\n");
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
%   M a g i c k D e s t r o y D r a w i n g W a n d                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   MagickDestroyDrawingWand() frees all resources associated with the drawing
%  wand. Once the drawing wand has been freed, it should not be used
%  any further unless it re-allocated.
%
%  The format of the  MagickDestroyDrawingWand method is:
%
%      void  MagickDestroyDrawingWand(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand. to destroy
%
*/
WandExport void MagickDestroyDrawingWand(DrawingWand *drawing_wand)
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
  /* A crummy way to determine if this image can be deleted. */
  if ((drawing_wand->image != (Image*) NULL) &&
      strcmp(drawing_wand->image->filename,dummy_image_filename) == 0)
    DestroyImage(drawing_wand->image);
  drawing_wand->image=(Image*) NULL;
  drawing_wand->signature=0;
  drawing_wand=(DrawingWand *) RelinquishMagickMemory(drawing_wand);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w A n n o t a t i o n                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawAnnotation() draws text on the image.
%
%  The format of the MagickDrawAnnotation method is:
%
%      void MagickDrawAnnotation(DrawingWand *drawing_wand,const double x,
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
WandExport void MagickDrawAnnotation(DrawingWand *drawing_wand,const double x,
  const double y,const unsigned char *text)
{
  char
    *escaped_text;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(text != (const unsigned char *) NULL);
  escaped_text=EscapeString((const char*)text,'\'');
  (void) MvgPrintf(drawing_wand,"text %.4g,%.4g '%.1024s'\n",x,y,escaped_text);
  escaped_text=(char *) RelinquishMagickMemory(escaped_text);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w A f f i n e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawAffine() adjusts the current affine transformation matrix with
%  the specified affine transformation matrix. Note that the current affine
%  transform is adjusted rather than replaced.
%
%  The format of the MagickDrawAffine method is:
%
%      void MagickDrawAffine(DrawingWand *drawing_wand,const AffineMatrix *affine)
%
%  A description of each parameter follows:
%
%    o drawing_wand: Drawing drawing_wand
%
%    o affine: Affine matrix parameters
%
*/
WandExport void MagickDrawAffine(DrawingWand *drawing_wand,
  const AffineMatrix *affine)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(affine != (const AffineMatrix *)NULL);
  AdjustAffine(drawing_wand,affine );
  (void) MvgPrintf(drawing_wand,"affine %.6g,%.6g,%.6g,%.6g,%.6g,%.6g\n",affine->sx,
    affine->rx,affine->ry,affine->sy,affine->tx,affine->ty);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w A l l o c a t e W a n d                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawAllocateWand() allocates an initial drawing wand which is an
%  opaque handle required by the remaining drawing methods.
%
%  The format of the MagickDrawAllocateWand method is:
%
%      DrawingWand MagickDrawAllocateWand(const DrawInfo *draw_info,Image *image)
%
%  A description of each parameter follows:
%
%    o draw_info: Initial drawing defaults. Set to NULL to use
%                 ImageMagick defaults.
%
%    o image: The image to draw on.
%
*/
WandExport DrawingWand *MagickDrawAllocateWand(const DrawInfo *draw_info,Image *image)
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
%   M a g i c k D r a w A r c                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawArc() draws an arc falling within a specified bounding rectangle on the
%  image.
%
%  The format of the MagickDrawArc method is:
%
%      void MagickDrawArc(DrawingWand *drawing_wand,const double sx,const double sy,
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
WandExport void MagickDrawArc(DrawingWand *drawing_wand,const double sx,
  const double sy,const double ex,const double ey,const double sd,
  const double ed)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  (void) MvgPrintf(drawing_wand,"arc %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",sx,sy,ex,ey,
    sd,ed);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w B e z i e r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawBezier() draws a bezier curve through a set of points on the image.
%
%  The format of the MagickDrawBezier method is:
%
%      void MagickDrawBezier(DrawingWand *drawing_wand,
%        const unsigned long number_coordinates,const PointInfo *coordinates)
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
WandExport void MagickDrawBezier(DrawingWand *drawing_wand,
  const unsigned long number_coordinates,const PointInfo *coordinates)
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
%   M a g i c k D r a w C i r c l e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawCircle() draws a circle on the image.
%
%  The format of the MagickDrawCircle method is:
%
%      void MagickDrawCircle(DrawingWand *drawing_wand,const double ox,
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
WandExport void MagickDrawCircle(DrawingWand *drawing_wand,const double ox,
  const double oy,const double px,const double py)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  (void) MvgPrintf(drawing_wand, "circle %.4g,%.4g %.4g,%.4g\n",ox,oy,px,py);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t C l i p P a t h                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetClipPath() obtains the current clipping path ID. The value returned
%  must be deallocated by the user when it is no longer needed.
%
%  The format of the MagickDrawGetClipPath method is:
%
%      char *MagickDrawGetClipPath(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport char *MagickDrawGetClipPath(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t C l i p P a t h                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetClipPath() associates a named clipping path with the image.  Only
%  the areas drawn on by the clipping path will be modified as long as it
%  remains in effect.
%
%  The format of the MagickDrawSetClipPath method is:
%
%      void MagickDrawSetClipPath(DrawingWand *drawing_wand,const char *clip_path)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o clip_path: name of clipping path to associate with image
%
*/
WandExport void MagickDrawSetClipPath(DrawingWand *drawing_wand,
  const char *clip_path)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(clip_path != (const char *) NULL);
  if ((CurrentContext->clip_path == NULL) || drawing_wand->filter_off ||
      LocaleCompare(CurrentContext->clip_path,clip_path) != 0)
    {
      (void) CloneString(&CurrentContext->clip_path,clip_path);
      if (CurrentContext->clip_path == (char*)NULL)
        ThrowException3(&drawing_wand->image->exception,ResourceLimitError,
          MemoryAllocationFailed,UnableToDrawOnImage);
#if DRAW_BINARY_IMPLEMENTATION
      (void) DrawClipPath(drawing_wand->image,CurrentContext,
        CurrentContext->clip_path);
#endif
      (void) MvgPrintf(drawing_wand,"clip-path url(#%s)\n",clip_path);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t C l i p R u l e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetClipRule() returns the current polygon fill rule to be used by the
%  clipping path.
%
%  The format of the MagickDrawGetClipRule method is:
%
%     FillRule MagickDrawGetClipRule(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport FillRule MagickDrawGetClipRule(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t C l i p R u l e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetClipRule() set the polygon fill rule to be used by the clipping path.
%
%  The format of the MagickDrawSetClipRule method is:
%
%      void MagickDrawSetClipRule(DrawingWand *drawing_wand,const FillRule fill_rule)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o fill_rule: fill rule (EvenOddRule or NonZeroRule)
%
*/
WandExport void MagickDrawSetClipRule(DrawingWand *drawing_wand,
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
        (void) MvgPrintf(drawing_wand, "clip-rule %s\n", p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t C l i p U n i t s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetClipUnits() returns the interpretation of clip path units.
%
%  The format of the MagickDrawGetClipUnits method is:
%
%      ClipPathUnits MagickDrawGetClipUnits(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport ClipPathUnits MagickDrawGetClipUnits(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t C l i p U n i t s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetClipUnits() sets the interpretation of clip path units.
%
%  The format of the MagickDrawSetClipUnits method is:
%
%      void MagickDrawSetClipUnits(DrawingWand *drawing_wand,
%        const ClipPathUnits clip_units)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o clip_units: units to use (UserSpace, UserSpaceOnUse, or ObjectBoundingBox)
%
*/
WandExport void MagickDrawSetClipUnits(DrawingWand *drawing_wand,
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
        (void) MvgPrintf(drawing_wand, "clip-units %s\n", p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w C o l o r                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawColor() draws color on image using the current fill color, starting at
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
%  The format of the MagickDrawColor method is:
%
%      void MagickDrawColor(DrawingWand *drawing_wand,const double x,const double y,
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
WandExport void MagickDrawColor(DrawingWand *drawing_wand,const double x,
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
    (void) MvgPrintf(drawing_wand, "color %.4g,%.4g %s\n", x, y, p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w C o m m e n t                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawComment() adds a comment to a vector output stream.
%
%  The format of the MagickDrawComment method is:
%
%      void MagickDrawComment(DrawingWand *drawing_wand,const char *comment)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o comment: comment text
%
*/
WandExport void MagickDrawComment(DrawingWand *drawing_wand,const char* comment)
{
  (void) MvgPrintf(drawing_wand,"#%s\n",comment);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w E l l i p s e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawEllipse() draws an ellipse on the image.
%
%  The format of the MagickDrawEllipse method is:
%
%       void MagickDrawEllipse(DrawingWand *drawing_wand,const double ox,
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
WandExport void MagickDrawEllipse(DrawingWand *drawing_wand,const double ox,
  const double oy,const double rx,const double ry,const double start,
  const double end)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  (void) MvgPrintf(drawing_wand,"ellipse %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",ox,oy,rx,ry,
    start,end);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t F i l l C o l o r                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetFillColor() returns the fill color used for drawing filled objects.
%
%  The format of the MagickDrawGetFillColor method is:
%
%      void MagickDrawGetFillColor(const DrawingWand *drawing_wand,
%        PixelWand *fill_color)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o fill_color: Return the fill color.
%
*/
WandExport void MagickDrawGetFillColor(const DrawingWand *drawing_wand,
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
%   M a g i c k D r a w S e t F i l l C o l o r                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetFillColor() sets the fill color to be used for drawing filled objects.
%
%  The format of the MagickDrawSetFillColor method is:
%
%      void MagickDrawSetFillColor(DrawingWand *drawing_wand,
%        const PixelWand *fill_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o fill_wand: fill wand.
%
*/
WandExport void MagickDrawSetFillColor(DrawingWand *drawing_wand,
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
      (void) MvgPrintf(drawing_wand,"fill '");
      MvgAppendColor(drawing_wand,&fill_color);
      (void) MvgPrintf(drawing_wand,"'\n");
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w S e t F i l l P a t t e r n U R L                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetFillPatternURL() sets the URL to use as a fill pattern for filling
%  objects. Only local URLs ("#identifier") are supported at this time. These
%  local URLs are normally created by defining a named fill pattern with
%  MagickDrawPushPattern/MagickDrawPopPattern.
%
%  The format of the MagickDrawSetFillPatternURL method is:
%
%      void MagickDrawSetFillPatternURL(DrawingWand *drawing_wand,
%        const char *fill_url)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o fill_url: URL to use to obtain fill pattern.
%
*/
WandExport void MagickDrawSetFillPatternURL(DrawingWand *drawing_wand,
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
      (void) MvgPrintf(drawing_wand,"fill %s\n",pattern_spec);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t F i l l O p a c i t y                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetFillOpacity() returns the opacity used when drawing using the fill
%  color or fill texture.  Fully opaque is 1.0.
%
%  The format of the MagickDrawGetFillOpacity method is:
%
%      double MagickDrawGetFillOpacity(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport double MagickDrawGetFillOpacity(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t F i l l O p a c i t y                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetFillOpacity() sets the opacity to use when drawing using the fill
%  color or fill texture.  Fully opaque is 1.0.
%
%  The format of the MagickDrawSetFillOpacity method is:
%
%      void MagickDrawSetFillOpacity(DrawingWand *drawing_wand,
%        const double fill_opacity)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o fill_opacity: fill opacity
%
*/
WandExport void MagickDrawSetFillOpacity(DrawingWand *drawing_wand,
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
      (void) MvgPrintf(drawing_wand,"fill-opacity %.4g\n",fill_opacity);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t F i l l R u l e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetFillRule() returns the fill rule used while drawing polygons.
%
%  The format of the MagickDrawGetFillRule method is:
%
%      FillRule MagickDrawGetFillRule(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport FillRule MagickDrawGetFillRule(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t F i l l R u l e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetFillRule() sets the fill rule to use while drawing polygons.
%
%  The format of the MagickDrawSetFillRule method is:
%
%      void MagickDrawSetFillRule(DrawingWand *drawing_wand,const FillRule fill_rule)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o fill_rule: fill rule (EvenOddRule or NonZeroRule)
%
*/
WandExport void MagickDrawSetFillRule(DrawingWand *drawing_wand,
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
        (void) MvgPrintf(drawing_wand,"fill-rule %s\n",p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t F o n t                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetFont() returns a null-terminaged string specifying the font used
%  when annotating with text. The value returned must be freed by the user
%  when no longer needed.
%
%  The format of the MagickDrawGetFont method is:
%
%      char *MagickDrawGetFont(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport char *MagickDrawGetFont(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t F o n t                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetFont() sets the fully-sepecified font to use when annotating with
%  text.
%
%  The format of the MagickDrawSetFont method is:
%
%      void MagickDrawSetFont(DrawingWand *drawing_wand,const char *font_name)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o font_name: font name
%
*/
WandExport void MagickDrawSetFont(DrawingWand *drawing_wand,const char *font_name)
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
      (void) MvgPrintf(drawing_wand,"font '%s'\n",font_name);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t F o n t F a m i l y                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetFontFamily() returns the font family to use when annotating with text.
%  The value returned must be freed by the user when it is no longer needed.
%
%  The format of the MagickDrawGetFontFamily method is:
%
%      char *MagickDrawGetFontFamily(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport char *MagickDrawGetFontFamily(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t F o n t F a m i l y                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetFontFamily() sets the font family to use when annotating with text.
%
%  The format of the MagickDrawSetFontFamily method is:
%
%      void MagickDrawSetFontFamily(DrawingWand *drawing_wand,const char *font_family)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o font_family: font family
%
*/
WandExport void MagickDrawSetFontFamily(DrawingWand *drawing_wand,
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
      (void) MvgPrintf(drawing_wand,"font-family '%s'\n",font_family);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t F o n t S i z e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetFontSize() returns the font pointsize used when annotating with text.
%
%  The format of the MagickDrawGetFontSize method is:
%
%      double MagickDrawGetFontSize(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport double MagickDrawGetFontSize(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t F o n t S i z e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetFontSize() sets the font pointsize to use when annotating with text.
%
%  The format of the MagickDrawSetFontSize method is:
%
%      void MagickDrawSetFontSize(DrawingWand *drawing_wand,const double pointsize)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o pointsize: text pointsize
%
*/
WandExport void MagickDrawSetFontSize(DrawingWand *drawing_wand,
  const double pointsize)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off ||
     (AbsoluteValue(CurrentContext->pointsize-pointsize) > MagickEpsilon))
    {
      CurrentContext->pointsize=pointsize;
      (void) MvgPrintf(drawing_wand,"font-size %.4g\n",pointsize);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t F o n t S t r e t c h                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetFontStretch() returns the font stretch used when annotating with text.
%
%  The format of the MagickDrawGetFontStretch method is:
%
%      StretchType MagickDrawGetFontStretch(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport StretchType MagickDrawGetFontStretch(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t F o n t S t r e t c h                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetFontStretch() sets the font stretch to use when annotating with text.
%  The AnyStretch enumeration acts as a wild-card "don't care" option.
%
%  The format of the MagickDrawSetFontStretch method is:
%
%      void MagickDrawSetFontStretch(DrawingWand *drawing_wand,
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
WandExport void MagickDrawSetFontStretch(DrawingWand *drawing_wand,
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
        (void) MvgPrintf(drawing_wand,"font-stretch '%s'\n",p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t F o n t S t y l e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetFontStyle() returns the font style used when annotating with text.
%
%  The format of the MagickDrawGetFontStyle method is:
%
%      StyleType MagickDrawGetFontStyle(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport StyleType MagickDrawGetFontStyle(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t F o n t S t y l e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetFontStyle() sets the font style to use when annotating with text.
%  The AnyStyle enumeration acts as a wild-card "don't care" option.
%
%  The format of the MagickDrawSetFontStyle method is:
%
%      void MagickDrawSetFontStyle(DrawingWand *drawing_wand,const StyleType style)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o style: font style (NormalStyle, ItalicStyle, ObliqueStyle, AnyStyle)
%
*/
WandExport void MagickDrawSetFontStyle(DrawingWand *drawing_wand,
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
        (void) MvgPrintf(drawing_wand, "font-style '%s'\n", p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t F o n t W e i g h t                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetFontWeight() returns the font weight used when annotating with text.
%
%  The format of the MagickDrawGetFontWeight method is:
%
%      unsigned long MagickDrawGetFontWeight(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport unsigned long MagickDrawGetFontWeight(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t F o n t W e i g h t                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetFontWeight() sets the font weight to use when annotating with text.
%
%  The format of the MagickDrawSetFontWeight method is:
%
%      void MagickDrawSetFontWeight(DrawingWand *drawing_wand,
%        const unsigned long font_weight)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o font_weight: font weight (valid range 100-900)
%
*/
WandExport void MagickDrawSetFontWeight(DrawingWand *drawing_wand,
  const unsigned long font_weight)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off || (CurrentContext->weight != font_weight))
    {
      CurrentContext->weight=font_weight;
      (void) MvgPrintf(drawing_wand,"font-weight %lu\n",font_weight);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t G r a v i t y                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetGravity() returns the text placement gravity used when annotating
%  with text.
%
%  The format of the MagickDrawGetGravity method is:
%
%      GravityType MagickDrawGetGravity(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport GravityType MagickDrawGetGravity(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t G r a v i t y                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetGravity() sets the text placement gravity to use when annotating
%  with text.
%
%  The format of the MagickDrawSetGravity method is:
%
%      void MagickDrawSetGravity(DrawingWand *drawing_wand,const GravityType gravity)
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
WandExport void MagickDrawSetGravity(DrawingWand *drawing_wand,
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
        (void) MvgPrintf(drawing_wand,"gravity %s\n",p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w C o m p o s i t e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawComposite() composites an image onto the current image, using the
%  specified composition operator, specified position, and at the specified
%  size.
%
%  The format of the MagickDrawComposite method is:
%
%      void MagickDrawComposite(DrawingWand *drawing_wand,
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
WandExport void MagickDrawComposite(DrawingWand *drawing_wand,
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
  mode=CompositeOperatorToString(composite_operator);
  media_type=MagickToMime(image->magick);
  if (media_type != NULL)
    {
      char
        *str;

      int
        remaining;

      (void) MvgPrintf(drawing_wand,"image %s %.4g,%.4g %.4g,%.4g 'data:%s;base64,\n",
        mode,x,y,width,height,media_type);
      remaining=(int) encoded_length;
      str=base64;
      while ( remaining > 0 )
      {
        (void) MvgPrintf(drawing_wand,"%.76s", str);
        remaining -= 76;
        str += 76;
        if (remaining > 0)
          (void) MvgPrintf(drawing_wand,"\n");
      }
      (void) MvgPrintf(drawing_wand,"'\n");
    }
  media_type=(char *) RelinquishMagickMemory(media_type);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w L i n e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawLine() draws a line on the image using the current stroke color,
%  stroke opacity, and stroke width.
%
%  The format of the MagickDrawLine method is:
%
%      void MagickDrawLine(DrawingWand *drawing_wand,const double sx,const double sy,
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
WandExport void MagickDrawLine(DrawingWand *drawing_wand,const double sx,
  const double sy,const double ex,const double ey)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  (void) MvgPrintf(drawing_wand,"line %.4g,%.4g %.4g,%.4g\n",sx,sy,ex,ey);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w M a t t e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawMatte() paints on the image's opacity channel in order to set effected
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
%  The format of the MagickDrawMatte method is:
%
%      void MagickDrawMatte(DrawingWand *drawing_wand,const double x,const double y,
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
WandExport void MagickDrawMatte(DrawingWand *drawing_wand,const double x,
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
    (void) MvgPrintf(drawing_wand,"matte %.4g,%.4g %s\n",x,y,p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w P a t h C l o s e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathClose() adds a path element to the current path which closes the
%  current subpath by drawing a straight line from the current point to the
%  current subpath's most recent starting point (usually, the most recent
%  moveto point).
%
%  The format of the MagickDrawPathClose method is:
%
%      void MagickDrawPathClose(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void MagickDrawPathClose(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  (void) MvgAutoWrapPrintf(drawing_wand,"%s",
    drawing_wand->path_mode == AbsolutePathMode ? "Z" : "z");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w P a t h C u r v e T o A b s o l u t e                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathCurveToAbsolute() draws a cubic Bezier curve from the current
%  point to (x,y) using (x1,y1) as the control point at the beginning of
%  the curve and (x2,y2) as the control point at the end of the curve using
%  absolute coordinates. At the end of the command, the new current point
%  becomes the final (x,y) coordinate pair used in the polybezier.
%
%  The format of the MagickDrawPathCurveToAbsolute method is:
%
%      void MagickDrawPathCurveToAbsolute(DrawingWand *drawing_wand,const double x1,
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
      (void) MvgAutoWrapPrintf(drawing_wand, "%c%.4g,%.4g %.4g,%.4g %.4g,%.4g",
        mode == AbsolutePathMode ? 'C' : 'c',x1,y1,x2,y2,x,y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g %.4g,%.4g %.4g,%.4g",
      x1,y1,x2,y2,x,y);
}

WandExport void MagickDrawPathCurveToAbsolute(DrawingWand *drawing_wand,
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
%   M a g i c k D r a w P a t h C u r v e T o R e l a t i v e                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathCurveToRelative() draws a cubic Bezier curve from the current
%  point to (x,y) using (x1,y1) as the control point at the beginning of
%  the curve and (x2,y2) as the control point at the end of the curve using
%  relative coordinates. At the end of the command, the new current point
%  becomes the final (x,y) coordinate pair used in the polybezier.
%
%  The format of the MagickDrawPathCurveToRelative method is:
%
%      void MagickDrawPathCurveToRelative(DrawingWand *drawing_wand,const double x1,
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
WandExport void MagickDrawPathCurveToRelative(DrawingWand *drawing_wand,
  const double x1,const double y1,const double x2,const double y2,
  const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathCurveTo(drawing_wand,RelativePathMode,x1,y1,x2,y2,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                                            %
%                                                                                            %
%                                                                                            %
%   M a g i c k D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r A b s o l u t e  %
%                                                                                            %
%                                                                                            %
%                                                                                            %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathCurveToQuadraticBezierAbsolute() draws a quadratic Bezier curve
%  from the current point to (x,y) using (x1,y1) as the control point using
%  absolute coordinates. At the end of the command, the new current point
%  becomes the final (x,y) coordinate pair used in the polybezier.
%
%  The format of the MagickDrawPathCurveToQuadraticBezierAbsolute method is:
%
%      void MagickDrawPathCurveToQuadraticBezierAbsolute(DrawingWand *drawing_wand,
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
      (void) MvgAutoWrapPrintf(drawing_wand, "%c%.4g,%.4g %.4g,%.4g",
        mode == AbsolutePathMode ? 'Q' : 'q',x1,y1,x,y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g %.4g,%.4g",x1,y1,x,y);
}

WandExport void MagickDrawPathCurveToQuadraticBezierAbsolute(
  DrawingWand *drawing_wand,const double x1,const double y1,const double x,
  const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathCurveToQuadraticBezier(drawing_wand,AbsolutePathMode,x1,y1,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                                             %
%                                                                                             %
%                                                                                             %
%   M a g i c k D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r R e l a t i v e   %
%                                                                                             %
%                                                                                             %
%                                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathCurveToQuadraticBezierRelative() draws a quadratic Bezier curve
%  from the current point to (x,y) using (x1,y1) as the control point using
%  relative coordinates. At the end of the command, the new current point
%  becomes the final (x,y) coordinate pair used in the polybezier.
%
%  The format of the MagickDrawPathCurveToQuadraticBezierRelative method is:
%
%      void MagickDrawPathCurveToQuadraticBezierRelative(DrawingWand *drawing_wand,
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
WandExport void MagickDrawPathCurveToQuadraticBezierRelative(
  DrawingWand *drawing_wand,const double x1,const double y1,const double x,
  const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathCurveToQuadraticBezier(drawing_wand,RelativePathMode,x1,y1,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                                                         %
%                                                                                                         %
%                                                                                                         %
%   M a g i c k D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r S m o o t h A b s o l u t e   %
%                                                                                                         %
%                                                                                                         %
%                                                                                                         %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathCurveToQuadraticBezierSmoothAbsolute() draws a quadratic
%  Bezier curve (using absolute coordinates) from the current point to
%  (x,y). The control point is assumed to be the reflection of the
%  control point on the previous command relative to the current
%  point. (If there is no previous command or if the previous command was
%  not a MagickDrawPathCurveToQuadraticBezierAbsolute,
%  MagickDrawPathCurveToQuadraticBezierRelative,
%  MagickDrawPathCurveToQuadraticBezierSmoothAbsolute or
%  MagickDrawPathCurveToQuadraticBezierSmoothRelative, assume the control point
%  is coincident with the current point.). At the end of the command, the
%  new current point becomes the final (x,y) coordinate pair used in the
%  polybezier.
%
%  The format of the MagickDrawPathCurveToQuadraticBezierSmoothAbsolute method is:
%
%      void MagickDrawPathCurveToQuadraticBezierSmoothAbsolute(
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
      (void) MvgAutoWrapPrintf(drawing_wand, "%c%.4g,%.4g",
        mode == AbsolutePathMode ? 'T' : 't',x,y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g",x,y);
}

WandExport void MagickDrawPathCurveToQuadraticBezierSmoothAbsolute(
  DrawingWand *drawing_wand,const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathCurveToQuadraticBezierSmooth(drawing_wand,AbsolutePathMode,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                                                         %
%                                                                                                         %
%                                                                                                         %
%   M a g i c k D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r S m o o t h R e l a t i v e   %
%                                                                                                         %
%                                                                                                         %
%                                                                                                         %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathCurveToQuadraticBezierSmoothRelative() draws a quadratic
%  Bezier curve (using relative coordinates) from the current point to
%  (x,y). The control point is assumed to be the reflection of the
%  control point on the previous command relative to the current
%  point. (If there is no previous command or if the previous command was
%  not a MagickDrawPathCurveToQuadraticBezierAbsolute,
%  MagickDrawPathCurveToQuadraticBezierRelative,
%  MagickDrawPathCurveToQuadraticBezierSmoothAbsolute or
%  MagickDrawPathCurveToQuadraticBezierSmoothRelative, assume the control point
%  is coincident with the current point.). At the end of the command, the
%  new current point becomes the final (x,y) coordinate pair used in the
%  polybezier.
%
%  The format of the MagickDrawPathCurveToQuadraticBezierSmoothRelative method is:
%
%      void MagickDrawPathCurveToQuadraticBezierSmoothRelative(
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
WandExport void MagickDrawPathCurveToQuadraticBezierSmoothRelative(
  DrawingWand *drawing_wand,const double x,const double y)
{
  DrawPathCurveToQuadraticBezierSmooth(drawing_wand,RelativePathMode,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w P a t h C u r v e T o S m o o t h A b s o l u t e     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathCurveToSmoothAbsolute() draws a cubic Bezier curve from the
%  current point to (x,y) using absolute coordinates. The first control
%  point is assumed to be the reflection of the second control point on
%  the previous command relative to the current point. (If there is no
%  previous command or if the previous command was not an
%  MagickDrawPathCurveToAbsolute, MagickDrawPathCurveToRelative,
%  MagickDrawPathCurveToSmoothAbsolute or MagickDrawPathCurveToSmoothRelative, assume
%  the first control point is coincident with the current point.) (x2,y2)
%  is the second control point (i.e., the control point at the end of the
%  curve). At the end of the command, the new current point becomes the
%  final (x,y) coordinate pair used in the polybezier.
%
%  The format of the MagickDrawPathCurveToSmoothAbsolute method is:
%
%      void MagickDrawPathCurveToSmoothAbsolute(DrawingWand *drawing_wand,
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
      (void) MvgAutoWrapPrintf(drawing_wand,"%c%.4g,%.4g %.4g,%.4g",
        mode == AbsolutePathMode ? 'S' : 's',x2,y2,x,y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g %.4g,%.4g",x2,y2,x,y);
}

WandExport void MagickDrawPathCurveToSmoothAbsolute(DrawingWand *drawing_wand,
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
%   M a g i c k D r a w P a t h C u r v e T o S m o o t h R e l a t i v e     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathCurveToSmoothRelative() draws a cubic Bezier curve from the
%  current point to (x,y) using relative coordinates. The first control
%  point is assumed to be the reflection of the second control point on
%  the previous command relative to the current point. (If there is no
%  previous command or if the previous command was not an
%  MagickDrawPathCurveToAbsolute, MagickDrawPathCurveToRelative,
%  MagickDrawPathCurveToSmoothAbsolute or MagickDrawPathCurveToSmoothRelative, assume
%  the first control point is coincident with the current point.) (x2,y2)
%  is the second control point (i.e., the control point at the end of the
%  curve). At the end of the command, the new current point becomes the
%  final (x,y) coordinate pair used in the polybezier.
%
%  The format of the MagickDrawPathCurveToSmoothRelative method is:
%
%      void MagickDrawPathCurveToSmoothRelative(DrawingWand *drawing_wand,
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
WandExport void MagickDrawPathCurveToSmoothRelative(DrawingWand *drawing_wand,
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
%   M a g i c k D r a w P a t h E l l i p t i c A r c A b s o l u t e         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathEllipticArcAbsolute() draws an elliptical arc from the current
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
%  The format of the MagickDrawPathEllipticArcAbsolute method is:
%
%      void MagickDrawPathEllipticArcAbsolute(DrawingWand *drawing_wand,
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
      (void) MvgAutoWrapPrintf(drawing_wand, "%c%.4g,%.4g %.4g %u %u %.4g,%.4g",
        mode == AbsolutePathMode ? 'A' : 'a',rx,ry,x_axis_rotation,
        large_arc_flag,sweep_flag,x,y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g %.4g %u %u %.4g,%.4g",rx,ry,
      x_axis_rotation,large_arc_flag,sweep_flag,x,y);
}

WandExport void MagickDrawPathEllipticArcAbsolute(DrawingWand *drawing_wand,
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
%   M a g i c k D r a w P a t h E l l i p t i c A r c R e l a t i v e         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathEllipticArcRelative() draws an elliptical arc from the current
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
%  The format of the MagickDrawPathEllipticArcRelative method is:
%
%      void MagickDrawPathEllipticArcRelative(DrawingWand *drawing_wand,
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
WandExport void MagickDrawPathEllipticArcRelative(DrawingWand *drawing_wand,
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
%   M a g i c k D r a w P a t h F i n i s h                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathFinish() terminates the current path.
%
%  The format of the MagickDrawPathFinish method is:
%
%      void MagickDrawPathFinish(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void MagickDrawPathFinish(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  (void) MvgPrintf(drawing_wand,"'\n");
  drawing_wand->path_operation=PathDefaultOperation;
  drawing_wand->path_mode=DefaultPathMode;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w P a t h L i n e T o A b s o l u t e                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathLineToAbsolute() draws a line path from the current point to the
%  given coordinate using absolute coordinates. The coordinate then becomes
%  the new current point.
%
%  The format of the MagickDrawPathLineToAbsolute method is:
%
%      void MagickDrawPathLineToAbsolute(DrawingWand *drawing_wand,const double x,const double y)
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
      (void) MvgAutoWrapPrintf(drawing_wand,"%c%.4g,%.4g",
        mode == AbsolutePathMode ? 'L' : 'l',x,y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g",x,y);
}

WandExport void MagickDrawPathLineToAbsolute(DrawingWand *drawing_wand,
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
%   M a g i c k D r a w P a t h L i n e T o R e l a t i v e                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathLineToRelative() draws a line path from the current point to the
%  given coordinate using relative coordinates. The coordinate then becomes
%  the new current point.
%
%  The format of the MagickDrawPathLineToRelative method is:
%
%      void MagickDrawPathLineToRelative(DrawingWand *drawing_wand,const double x,
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
WandExport void MagickDrawPathLineToRelative(DrawingWand *drawing_wand,
  const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathLineTo(drawing_wand,RelativePathMode,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                                %
%                                                                                %
%                                                                                %
%   M a g i c k D r a w P a t h L i n e T o H o r i z o n t a l A b s o l u t e  %
%                                                                                %
%                                                                                %
%                                                                                %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathLineToHorizontalAbsolute() draws a horizontal line path from the
%  current point to the target point using absolute coordinates.  The target
%  point then becomes the new current point.
%
%  The format of the MagickDrawPathLineToHorizontalAbsolute method is:
%
%      void MagickDrawPathLineToHorizontalAbsolute(DrawingWand *drawing_wand,
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
      (void) MvgAutoWrapPrintf(drawing_wand,"%c%.4g",
        mode == AbsolutePathMode ? 'H' : 'h',x);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %.4g",x);
}

WandExport void MagickDrawPathLineToHorizontalAbsolute(DrawingWand *drawing_wand,
  const double x)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathLineToHorizontal(drawing_wand,AbsolutePathMode,x);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                                %
%                                                                                %
%                                                                                %
%   M a g i c k D r a w P a t h L i n e T o H o r i z o n t a l R e l a t i v e  %
%                                                                                %
%                                                                                %
%                                                                                %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathLineToHorizontalRelative() draws a horizontal line path from the
%  current point to the target point using relative coordinates.  The target
%  point then becomes the new current point.
%
%  The format of the MagickDrawPathLineToHorizontalRelative method is:
%
%      void MagickDrawPathLineToHorizontalRelative(DrawingWand *drawing_wand,
%        const double x)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o x: target x ordinate
%
*/
WandExport void MagickDrawPathLineToHorizontalRelative(DrawingWand *drawing_wand,
  const double x)
{
  DrawPathLineToHorizontal(drawing_wand,RelativePathMode,x);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w P a t h L i n e T o V e r t i c a l A b s o l u t e   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathLineToVerticalAbsolute() draws a vertical line path from the
%  current point to the target point using absolute coordinates.  The target
%  point then becomes the new current point.
%
%  The format of the MagickDrawPathLineToVerticalAbsolute method is:
%
%      void MagickDrawPathLineToVerticalAbsolute(DrawingWand *drawing_wand,
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
      (void) MvgAutoWrapPrintf(drawing_wand,"%c%.4g",
        mode == AbsolutePathMode ? 'V' : 'v',y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %.4g",y);
}

WandExport void MagickDrawPathLineToVerticalAbsolute(DrawingWand *drawing_wand,
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
%   M a g i c k D r a w P a t h L i n e T o V e r t i c a l R e l a t i v e   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathLineToVerticalRelative() draws a vertical line path from the
%  current point to the target point using relative coordinates.  The target
%  point then becomes the new current point.
%
%  The format of the MagickDrawPathLineToVerticalRelative method is:
%
%      void MagickDrawPathLineToVerticalRelative(DrawingWand *drawing_wand,
%        const double y)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o y: target y ordinate
%
*/
WandExport void MagickDrawPathLineToVerticalRelative(DrawingWand *drawing_wand,
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
%   M a g i c k D r a w P a t h M o v e T o A b s o l u t e                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathMoveToAbsolute() starts a new sub-path at the given coordinate
%  using absolute coordinates. The current point then becomes the
%  specified coordinate.
%
%  The format of the MagickDrawPathMoveToAbsolute method is:
%
%      void MagickDrawPathMoveToAbsolute(DrawingWand *drawing_wand,const double x,
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
      (void) MvgAutoWrapPrintf(drawing_wand,"%c%.4g,%.4g",
        mode == AbsolutePathMode ? 'M' : 'm',x,y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %.4g,%.4g",x,y);
}

WandExport void MagickDrawPathMoveToAbsolute(DrawingWand *drawing_wand,
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
%   M a g i c k D r a w P a t h M o v e T o R e l a t i v e                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathMoveToRelative() starts a new sub-path at the given coordinate
%  using relative coordinates. The current point then becomes the
%  specified coordinate.
%
%  The format of the MagickDrawPathMoveToRelative method is:
%
%      void MagickDrawPathMoveToRelative(DrawingWand *drawing_wand,const double x,
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
WandExport void MagickDrawPathMoveToRelative(DrawingWand *drawing_wand,
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
%   M a g i c k D r a w P a t h S t a r t                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPathStart() declares the start of a path drawing list which is terminated
%  by a matching MagickDrawPathFinish() command. All other MagickDrawPath commands must
%  be enclosed between a MagickDrawPathStart() and a MagickDrawPathFinish() command. This
%  is because path drawing commands are subordinate commands and they do not
%  function by themselves.
%
%  The format of the MagickDrawPathStart method is:
%
%      void MagickDrawPathStart(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void MagickDrawPathStart(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  (void) MvgPrintf(drawing_wand,"path '");
  drawing_wand->path_operation=PathDefaultOperation;
  drawing_wand->path_mode=DefaultPathMode;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w P e e k G r a p h i c C o n t e x t                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPeekGraphicContext() returns the current graphic drawing_wand.
%
%  The format of the MagickDrawPeekGraphicContext method is:
%
%      DrawInfo *MagickDrawPeekGraphicContext(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport DrawInfo *MagickDrawPeekGraphicContext(const DrawingWand *drawing_wand)
{
  DrawInfo
    *draw_info;

  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  draw_info=CloneDrawInfo((ImageInfo *) NULL,CurrentContext);
  (void) CloneString(&draw_info->primitive,drawing_wand->mvg);
  return(draw_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w P o i n t                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPoint() draws a point using the current stroke color and stroke
%  thickness at the specified coordinates.
%
%  The format of the MagickDrawPoint method is:
%
%      void MagickDrawPoint(DrawingWand *drawing_wand,const double x,const double y)
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
WandExport void MagickDrawPoint(DrawingWand *drawing_wand,const double x,
  const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  (void) MvgPrintf(drawing_wand,"point %.4g,%.4g\n",x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w P o l y g o n                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPolygon() draws a polygon using the current stroke, stroke width, and
%  fill color or texture, using the specified array of coordinates.
%
%  The format of the MagickDrawPolygon method is:
%
%      void MagickDrawPolygon(DrawingWand *drawing_wand,
%        const unsigned long number_coordinates,const PointInfo *coordinates)
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
WandExport void MagickDrawPolygon(DrawingWand *drawing_wand,
  const unsigned long number_coordinates,const PointInfo *coordinates)
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
%   M a g i c k D r a w P o l y l i n e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPolyline() draws a polyline using the current stroke, stroke width, and
%  fill color or texture, using the specified array of coordinates.
%
%  The format of the MagickDrawPolyline method is:
%
%      void MagickDrawPolyline(DrawingWand *drawing_wand,
%        const unsigned long number_coordinates,const PointInfo *coordinates)
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
WandExport void MagickDrawPolyline(DrawingWand *drawing_wand,
  const unsigned long number_coordinates,const PointInfo *coordinates)
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
%   M a g i c k D r a w P o p C l i p P a t h                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPopClipPath() terminates a clip path definition.
%
%  The format of the MagickDrawPopClipPath method is:
%
%      void MagickDrawPopClipPath(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void MagickDrawPopClipPath(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->indent_depth > 0)
    drawing_wand->indent_depth--;
  (void) MvgPrintf(drawing_wand,"pop clip-path\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w P o p D e f s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPopDefs() terminates a definition list
%
%  The format of the MagickDrawPopDefs method is:
%
%      void MagickDrawPopDefs(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void MagickDrawPopDefs(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->indent_depth > 0)
    drawing_wand->indent_depth--;
  (void) MvgPrintf(drawing_wand,"pop defs\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w P o p G r a p h i c C o n t e x t                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPopGraphicContext() destroys the current drawing_wand returning to the
%  previously pushed drawing wand. Multiple drawing wand  may exist. It is an
%  error to attempt to pop more drawing_wands than have been pushed, and it is
%  proper form to pop all drawing_wands which have been pushed.
%
%  The format of the MagickDrawPopGraphicContext method is:
%
%      void MagickDrawPopGraphicContext(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void MagickDrawPopGraphicContext(DrawingWand *drawing_wand)
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
      (void) MvgPrintf(drawing_wand,"pop graphic-context\n");
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w P o p P a t t e r n                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPopPattern() terminates a pattern definition.
%
%  The format of the MagickDrawPopPattern method is:
%
%      void MagickDrawPopPattern(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void MagickDrawPopPattern(DrawingWand *drawing_wand)
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
  (void) MvgPrintf(drawing_wand,"pop pattern\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w P u s h C l i p P a t h                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPushClipPath() starts a clip path definition which is comprized of
%  any number of drawing commands and terminated by a MagickDrawPopClipPath()
%  command.
%
%  The format of the MagickDrawPushClipPath method is:
%
%      void MagickDrawPushClipPath(DrawingWand *drawing_wand,const char *clip_path_id)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o clip_path_id: string identifier to associate with the clip path for
%      later use.
%
*/
WandExport void MagickDrawPushClipPath(DrawingWand *drawing_wand,
  const char *clip_path_id)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(clip_path_id != (const char *) NULL);
  (void) MvgPrintf(drawing_wand,"push clip-path %s\n",clip_path_id);
  drawing_wand->indent_depth++;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w P u s h D e f s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPushDefs() indicates that commands up to a terminating MagickDrawPopDefs()
%  command create named elements (e.g. clip-paths, textures, etc.) which
%  may safely be processed earlier for the sake of efficiency.
%
%  The format of the MagickDrawPushDefs method is:
%
%      void MagickDrawPushDefs(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void MagickDrawPushDefs(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  (void) MvgPrintf(drawing_wand,"push defs\n");
  drawing_wand->indent_depth++;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w P u s h G r a p h i c C o n t e x t                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPushGraphicContext() clones the current drawing wand to create a
%  new drawing wand. The original drawing drawing_wand(s) may be returned to
%  by invoking MagickDrawPopGraphicContext().  The drawing wands are stored on a
%  drawing wand stack.  For every Pop there must have already been an
%  equivalent Push.
%
%  The format of the MagickDrawPushGraphicContext method is:
%
%      void MagickDrawPushGraphicContext(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport void MagickDrawPushGraphicContext(DrawingWand *drawing_wand)
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
  (void) MvgPrintf(drawing_wand,"push graphic-context\n");
  drawing_wand->indent_depth++;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w P u s h P a t t e r n                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawPushPattern() indicates that subsequent commands up to a
%  MagickDrawPopPattern() command comprise the definition of a named pattern.
%  The pattern space is assigned top left corner coordinates, a width
%  and height, and becomes its own drawing space.  Anything which can
%  be drawn may be used in a pattern definition.
%  Named patterns may be used as stroke or brush definitions.
%
%  The format of the MagickDrawPushPattern method is:
%
%      void MagickDrawPushPattern(DrawingWand *drawing_wand,const char *pattern_id,
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
WandExport void MagickDrawPushPattern(DrawingWand *drawing_wand,
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
  (void) MvgPrintf(drawing_wand,"push pattern %s %.4g,%.4g %.4g,%.4g\n",pattern_id,
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
%   M a g i c k D r a w R e c t a n g l e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawRectangle() draws a rectangle given two coordinates and using
%  the current stroke, stroke width, and fill settings.
%
%  The format of the MagickDrawRectangle method is:
%
%      void MagickDrawRectangle(DrawingWand *drawing_wand,const double x1,
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
WandExport void MagickDrawRectangle(DrawingWand *drawing_wand,const double x1,
  const double y1,const double x2,const double y2)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  (void) MvgPrintf(drawing_wand,"rectangle %.4g,%.4g %.4g,%.4g\n",x1,y1,x2,y2);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w R e n d e r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawRender() renders all preceding drawing commands onto the image.
%
%  The format of the MagickDrawRender method is:
%
%      unsigned int MagickDrawRender(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport unsigned int MagickDrawRender(const DrawingWand *drawing_wand)
{
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  CurrentContext->primitive=drawing_wand->mvg;
  (void) LogMagickEvent(DrawEvent,GetMagickModule(),"MVG:\n'%s'\n",
    drawing_wand->mvg);
  (void) DrawImage(drawing_wand->image, CurrentContext);
  CurrentContext->primitive=(char *) NULL;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w R o t a t e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawRotate() applies the specified rotation to the current coordinate space.
%
%  The format of the MagickDrawRotate method is:
%
%      void MagickDrawRotate(DrawingWand *drawing_wand,const double degrees)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o degrees: degrees of rotation
%
*/
WandExport void MagickDrawRotate(DrawingWand *drawing_wand,const double degrees)
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
  (void) MvgPrintf(drawing_wand,"rotate %.4g\n",degrees);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w R o u n d R e c t a n g l e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawRoundRectangle() draws a rounted rectangle given two coordinates,
%  x & y corner radiuses and using the current stroke, stroke width,
%  and fill settings.
%
%  The format of the MagickDrawRoundRectangle method is:
%
%      void MagickDrawRoundRectangle(DrawingWand *drawing_wand,double x1,double y1,
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
WandExport void MagickDrawRoundRectangle(DrawingWand *drawing_wand,double x1,
  double y1,double x2,double y2,double rx,double ry)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  (void) MvgPrintf(drawing_wand,"roundrectangle %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
    x1,y1,x2,y2,rx,ry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w S c a l e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawScale() adjusts the scaling factor to apply in the horizontal and
%  vertical directions to the current coordinate space.
%
%  The format of the MagickDrawScale method is:
%
%      void MagickDrawScale(DrawingWand *drawing_wand,const double x,const double y)
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
WandExport void MagickDrawScale(DrawingWand *drawing_wand,const double x,
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
  (void) MvgPrintf(drawing_wand,"scale %.4g,%.4g\n",x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w S k e w X                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSkewX() skews the current coordinate system in the horizontal
%  direction.
%
%  The format of the MagickDrawSkewX method is:
%
%      void MagickDrawSkewX(DrawingWand *drawing_wand,const double degrees)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o degrees: number of degrees to skew the coordinates
%
*/
WandExport void MagickDrawSkewX(DrawingWand *drawing_wand,const double degrees)
{
  AffineMatrix
    affine;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  GetAffineMatrix(&affine);
  affine.ry=tan(DegreesToRadians(fmod(degrees,360.0)));
  AdjustAffine(drawing_wand,&affine);
  (void) MvgPrintf(drawing_wand,"skewX %.4g\n",degrees);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w S k e w Y                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSkewY() skews the current coordinate system in the vertical
%  direction.
%
%  The format of the MagickDrawSkewY method is:
%
%      void MagickDrawSkewY(DrawingWand *drawing_wand,const double degrees)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o degrees: number of degrees to skew the coordinates
%
*/
WandExport void MagickDrawSkewY(DrawingWand *drawing_wand,const double degrees)
{
  AffineMatrix
    affine;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  GetAffineMatrix(&affine);
  affine.rx=tan(DegreesToRadians(fmod(degrees,360.0)));
  MagickDrawAffine(drawing_wand,&affine);
  (void) MvgPrintf(drawing_wand,"skewY %.4g\n",degrees);
}
#if 0

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w S e t S t o p C o l o r                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetStopColor() sets the stop color and offset for gradients
%
%  The format of the MagickDrawSetStopColor method is:
%
%      void MagickDrawSetStopColor(DrawingWand *drawing_wand,
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
WandExport void MagickDrawSetStopColor(DrawingWand *drawing_wand,
  const PixelPacket * stop_color,const double offset)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(stop_color != (const PixelPacket *) NULL);
  (void) MvgPrintf(drawing_wand,"stop-color ");
  MvgAppendColor(drawing_wand,stop_color);
  (void) MvgPrintf(drawing_wand,"\n");
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t S t r o k e C o l o r                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetStrokeColor() returns the color used for stroking object outlines.
%
%  The format of the MagickDrawGetStrokeColor method is:
%
%      void MagickDrawGetStrokeColor(const DrawingWand *drawing_wand,
$        PixelWand *stroke_color)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o stroke_color: Return the stroke color.
%
*/
WandExport void MagickDrawGetStrokeColor(const DrawingWand *drawing_wand,
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
%   M a g i c k D r a w S e t S t r o k e C o l o r                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetStrokeColor() sets the color used for stroking object outlines.
%
%  The format of the MagickDrawSetStrokeColor method is:
%
%      void MagickDrawSetStrokeColor(DrawingWand *drawing_wand,
%        const PixelWand *stroke_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o stroke_wand: stroke wand.
%
*/
WandExport void MagickDrawSetStrokeColor(DrawingWand *drawing_wand,
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
      (void) MvgPrintf(drawing_wand,"stroke '");
      MvgAppendColor(drawing_wand,&stroke_color);
      (void) MvgPrintf(drawing_wand,"'\n");
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w S e t S t r o k e P a t t e r n U R L                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetStrokePatternURL() sets the pattern used for stroking object outlines.
%
%  The format of the MagickDrawSetStrokePatternURL method is:
%
%      void MagickDrawSetStrokePatternURL(DrawingWand *drawing_wand,
%        const char *stroke_url)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o stroke_url: URL specifying pattern ID (e.g. "#pattern_id")
%
*/
WandExport void MagickDrawSetStrokePatternURL(DrawingWand *drawing_wand,
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
      (void) MvgPrintf(drawing_wand,"stroke %s\n",pattern_spec);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t S t r o k e A n t i a l i a s                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetStrokeAntialias() returns the current stroke antialias setting.
%  Stroked outlines are antialiased by default.  When antialiasing is disabled
%  stroked pixels are thresholded to determine if the stroke color or
%  underlying canvas color should be used.
%
%  The format of the MagickDrawGetStrokeAntialias method is:
%
%      unsigned int MagickDrawGetStrokeAntialias(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport unsigned int MagickDrawGetStrokeAntialias(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t S t r o k e A n t i a l i a s                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetStrokeAntialias() controls whether stroked outlines are antialiased.
%  Stroked outlines are antialiased by default.  When antialiasing is disabled
%  stroked pixels are thresholded to determine if the stroke color or
%  underlying canvas color should be used.
%
%  The format of the MagickDrawSetStrokeAntialias method is:
%
%      void MagickDrawSetStrokeAntialias(DrawingWand *drawing_wand,
%        const unsigned int stroke_antialias)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o stroke_antialias: set to false (zero) to disable antialiasing
%
*/
WandExport void MagickDrawSetStrokeAntialias(DrawingWand *drawing_wand,
  const unsigned int stroke_antialias)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off ||
     (CurrentContext->stroke_antialias != stroke_antialias))
    {
      CurrentContext->stroke_antialias=stroke_antialias;
      (void) MvgPrintf(drawing_wand,"stroke-antialias %i\n",stroke_antialias ? 1 : 0);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t S t r o k e D a s h A r r a y                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetStrokeDashArray() returns an array representing the pattern of
%  dashes and gaps used to stroke paths (see MagickDrawSetStrokeDashArray). The
%  array must be freed once it is no longer required by the user.
%
%  The format of the MagickDrawGetStrokeDashArray method is:
%
%      double *MagickDrawGetStrokeDashArray(const DrawingWand *drawing_wand,
%        unsigned long *number_elements)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o number_elements: address to place number of elements in dash array
%
% */
WandExport double *MagickDrawGetStrokeDashArray(const DrawingWand *drawing_wand,
  unsigned long *number_elements)
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
  assert(number_elements != (unsigned long *)NULL);
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
%   M a g i c k D r a w S e t S t r o k e D a s h A r r a y                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetStrokeDashArray() specifies the pattern of dashes and gaps used to
%  stroke paths. The stroke dash array represents an array of numbers that
%  specify the lengths of alternating dashes and gaps in pixels. If an odd
%  number of values is provided, then the list of values is repeated to yield
%  an even number of values. To remove an existing dash array, pass a zero
%  number_elements argument and null dash_array.
%  A typical stroke dash array might contain the members 5 3 2.
%
%  The format of the MagickDrawSetStrokeDashArray method is:
%
%      void MagickDrawSetStrokeDashArray(DrawingWand *drawing_wand,
%        const unsigned long number_elements,const double *dash_array)
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
WandExport void MagickDrawSetStrokeDashArray(DrawingWand *drawing_wand,
  const unsigned long number_elements,const double *dash_array)
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
      (void) MvgPrintf(drawing_wand,"stroke-dash_array ");
      if ( n_new == 0 )
        (void) MvgPrintf(drawing_wand, "none");
      else
        {
          p=dash_array;
          i=n_new;
          (void) MvgPrintf(drawing_wand,"%.4g",*p++);
          while (i--)
            (void) MvgPrintf(drawing_wand,",%.4g",*p++);
        }
      (void) MvgPrintf(drawing_wand,"0 \n");
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t S t r o k e D a s h O f f s e t                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetStrokeDashOffset() returns the offset into the dash pattern to
%  start the dash.
%
%  The format of the MagickDrawGetStrokeDashOffset method is:
%
%      double MagickDrawGetStrokeDashOffset(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport double MagickDrawGetStrokeDashOffset(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t S t r o k e D a s h O f f s e t                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetStrokeDashOffset() specifies the offset into the dash pattern to
%  start the dash.
%
%  The format of the MagickDrawSetStrokeDashOffset method is:
%
%      void MagickDrawSetStrokeDashOffset(DrawingWand *drawing_wand,
%        const double dash_offset)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o dash_offset: dash offset
%
*/
WandExport void MagickDrawSetStrokeDashOffset(DrawingWand *drawing_wand,
  const double dash_offset)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);

  if (drawing_wand->filter_off ||
     (AbsoluteValue(CurrentContext->dash_offset-dash_offset) > MagickEpsilon))
    {
      CurrentContext->dash_offset=dash_offset;
      (void) MvgPrintf(drawing_wand,"stroke-dashoffset %.4g\n",dash_offset);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t S t r o k e L i n e C a p                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetStrokeLineCap() returns the shape to be used at the end of
%  open subpaths when they are stroked. Values of LineCap are
%  UndefinedCap, ButtCap, RoundCap, and SquareCap.
%
%  The format of the MagickDrawGetStrokeLineCap method is:
%
%      LineCap MagickDrawGetStrokeLineCap(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
% */
WandExport LineCap MagickDrawGetStrokeLineCap(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t S t r o k e L i n e C a p                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetStrokeLineCap() specifies the shape to be used at the end of
%  open subpaths when they are stroked. Values of LineCap are
%  UndefinedCap, ButtCap, RoundCap, and SquareCap.
%
%  The format of the MagickDrawSetStrokeLineCap method is:
%
%      void MagickDrawSetStrokeLineCap(DrawingWand *drawing_wand,
%        const LineCap linecap)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o linecap: linecap style
%
% */
WandExport void MagickDrawSetStrokeLineCap(DrawingWand *drawing_wand,
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
        (void) MvgPrintf(drawing_wand,"stroke-linecap %s\n",p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t S t r o k e L i n e J o i n                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetStrokeLineJoin() returns the shape to be used at the
%  corners of paths (or other vector shapes) when they are
%  stroked. Values of LineJoin are UndefinedJoin, MiterJoin, RoundJoin,
%  and BevelJoin.
%
%  The format of the MagickDrawGetStrokeLineJoin method is:
%
%      LineJoin MagickDrawGetStrokeLineJoin(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
% */
WandExport LineJoin MagickDrawGetStrokeLineJoin(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t S t r o k e L i n e J o i n                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetStrokeLineJoin() specifies the shape to be used at the
%  corners of paths (or other vector shapes) when they are
%  stroked. Values of LineJoin are UndefinedJoin, MiterJoin, RoundJoin,
%  and BevelJoin.
%
%  The format of the MagickDrawSetStrokeLineJoin method is:
%
%      void MagickDrawSetStrokeLineJoin(DrawingWand *drawing_wand,
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
WandExport void MagickDrawSetStrokeLineJoin(DrawingWand *drawing_wand,
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
        (void) MvgPrintf(drawing_wand,"stroke-linejoin %s\n",p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t S t r o k e M i t e r L i m i t                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetStrokeMiterLimit() returns the miter limit. When two line
%  segments meet at a sharp angle and miter joins have been specified for
%  'lineJoin', it is possible for the miter to extend far beyond the
%  thickness of the line stroking the path. The miterLimit' imposes a
%  limit on the ratio of the miter length to the 'lineWidth'.
%
%  The format of the MagickDrawGetStrokeMiterLimit method is:
%
%      unsigned long MagickDrawGetStrokeMiterLimit(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
% */
WandExport unsigned long MagickDrawGetStrokeMiterLimit(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t S t r o k e M i t e r L i m i t                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetStrokeMiterLimit() specifies the miter limit. When two line
%  segments meet at a sharp angle and miter joins have been specified for
%  'lineJoin', it is possible for the miter to extend far beyond the
%  thickness of the line stroking the path. The miterLimit' imposes a
%  limit on the ratio of the miter length to the 'lineWidth'.
%
%  The format of the MagickDrawSetStrokeMiterLimit method is:
%
%      void MagickDrawSetStrokeMiterLimit(DrawingWand *drawing_wand,
%        const unsigned long miterlimit)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o miterlimit: miter limit
%
% */
WandExport void MagickDrawSetStrokeMiterLimit(DrawingWand *drawing_wand,
  const unsigned long miterlimit)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (CurrentContext->miterlimit != miterlimit)
    {
      CurrentContext->miterlimit=miterlimit;
      (void) MvgPrintf(drawing_wand,"stroke-miterlimit %lu\n",miterlimit);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t S t r o k e O p a c i t y                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetStrokeOpacity() returns the opacity of stroked object outlines.
%
%  The format of the MagickDrawGetStrokeOpacity method is:
%
%      double MagickDrawGetStrokeOpacity(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
*/
WandExport double MagickDrawGetStrokeOpacity(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t S t r o k e O p a c i t y                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetStrokeOpacity() specifies the opacity of stroked object outlines.
%
%  The format of the MagickDrawSetStrokeOpacity method is:
%
%      void MagickDrawSetStrokeOpacity(DrawingWand *drawing_wand,
%        const double stroke_opacity)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o stroke_opacity: stroke opacity.  The value 1.0 is opaque.
%
*/
WandExport void MagickDrawSetStrokeOpacity(DrawingWand *drawing_wand,
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
      (void) MvgPrintf(drawing_wand,"stroke-opacity %.4g\n",stroke_opacity);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t S t r o k e W i d t h                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetStrokeWidth() returns the width of the stroke used to draw object
%  outlines.
%
%  The format of the MagickDrawGetStrokeWidth method is:
%
%      double MagickDrawGetStrokeWidth(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport double MagickDrawGetStrokeWidth(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t S t r o k e W i d t h                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetStrokeWidth() sets the width of the stroke used to draw object
%  outlines.
%
%  The format of the MagickDrawSetStrokeWidth method is:
%
%      void MagickDrawSetStrokeWidth(DrawingWand *drawing_wand,
%        const double stroke_width)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o stroke_width: stroke width
%
*/
WandExport void MagickDrawSetStrokeWidth(DrawingWand *drawing_wand,
  const double stroke_width)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off ||
     (AbsoluteValue(CurrentContext->stroke_width-stroke_width) > MagickEpsilon))
    {
      CurrentContext->stroke_width=stroke_width;
      (void) MvgPrintf(drawing_wand,"stroke-width %.4g\n",stroke_width);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t T e x t A n t i a l i a s                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetTextAntialias() returns the current text antialias setting, which
%  determines whether text is antialiased.  Text is antialiased by default.
%
%  The format of the MagickDrawGetTextAntialias method is:
%
%      unsigned int MagickDrawGetTextAntialias(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport unsigned int MagickDrawGetTextAntialias(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t T e x t A n t i a l i a s                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetTextAntialias() controls whether text is antialiased.  Text is
%  antialiased by default.
%
%  The format of the MagickDrawSetTextAntialias method is:
%
%      void MagickDrawSetTextAntialias(DrawingWand *drawing_wand,
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
WandExport void MagickDrawSetTextAntialias(DrawingWand *drawing_wand,
  const unsigned int text_antialias)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (drawing_wand->filter_off ||
      (CurrentContext->text_antialias != text_antialias))
    {
      CurrentContext->text_antialias=text_antialias;
      (void) MvgPrintf(drawing_wand,"text-antialias %i\n",text_antialias ? 1 : 0);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t T e x t D e c o r a t i o n                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetTextDecoration() returns the decoration applied when annotating with
%  text.
%
%  The format of the MagickDrawGetTextDecoration method is:
%
%      DecorationType MagickDrawGetTextDecoration(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport DecorationType MagickDrawGetTextDecoration(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t T e x t D e c o r a t i o n                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetTextDecoration() specifies a decoration to be applied when
%  annotating with text.
%
%  The format of the MagickDrawSetTextDecoration method is:
%
%      void MagickDrawSetTextDecoration(DrawingWand *drawing_wand,
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
WandExport void MagickDrawSetTextDecoration(DrawingWand *drawing_wand,
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
        (void) MvgPrintf(drawing_wand,"decorate %s\n",p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t T e x t E n c o d i n g                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetTextEncoding() returns a null-terminated string which specifies the
%  code set used for text annotations. The string must be freed by the user
%  once it is no longer required.
%
%  The format of the MagickDrawGetTextEncoding method is:
%
%      char *MagickDrawGetTextEncoding(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
% */
WandExport char *MagickDrawGetTextEncoding(const DrawingWand *drawing_wand)
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
%   M a g i c k D r a w S e t T e x t E n c o d i n g                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetTextEncoding() specifies specifies the code set to use for
%  text annotations. The only character encoding which may be specified
%  at this time is "UTF-8" for representing Unicode as a sequence of
%  bytes. Specify an empty string to set text encoding to the system's
%  default. Successful text annotation using Unicode may require fonts
%  designed to support Unicode.
%
%  The format of the MagickDrawSetTextEncoding method is:
%
%      void MagickDrawSetTextEncoding(DrawingWand *drawing_wand,const char *encoding)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o encoding: character string specifying text encoding
%
% */
WandExport void MagickDrawSetTextEncoding(DrawingWand *drawing_wand,
  const char *encoding)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(encoding != (char *) NULL);
  if (drawing_wand->filter_off || (CurrentContext->encoding == (char *) NULL) ||
      (LocaleCompare(CurrentContext->encoding,encoding) != 0))
    {
      (void) CloneString(&CurrentContext->encoding,encoding);
      (void) MvgPrintf(drawing_wand,"encoding '%s'\n",encoding);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w G e t T e x t U n d e r C o l o r                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawGetTextUnderColor() returns the color of a background rectangle
%  to place under text annotations.
%
%  The format of the MagickDrawGetTextUnderColor method is:
%
%      void MagickDrawGetTextUnderColor(const DrawingWand *drawing_wand,
%        PixelWand *under_color)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o under_color: Return the under color.
%
*/
WandExport void MagickDrawGetTextUnderColor(const DrawingWand *drawing_wand,
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
%   M a g i c k D r a w S e t T e x t U n d e r C o l o r                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetTextUnderColor() specifies the color of a background rectangle
%  to place under text annotations.
%
%  The format of the MagickDrawSetTextUnderColor method is:
%
%      void MagickDrawSetTextUnderColor(DrawingWand *drawing_wand,
%        const PixelWand *under_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o under_wand.: text under wand.
%
*/
WandExport void MagickDrawSetTextUnderColor(DrawingWand *drawing_wand,
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
      (void) MvgPrintf(drawing_wand,"text-undercolor '");
      MvgAppendColor(drawing_wand,&under_color);
      (void) MvgPrintf(drawing_wand,"'\n");
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w T r a n s l a t e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawTranslate() applies a translation to the current coordinate
%  system which moves the coordinate system origin to the specified
%  coordinate.
%
%  The format of the MagickDrawTranslate method is:
%
%      void MagickDrawTranslate(DrawingWand *drawing_wand,const double x,
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
WandExport void MagickDrawTranslate(DrawingWand *drawing_wand,const double x,
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
  (void) MvgPrintf(drawing_wand,"translate %.4g,%.4g\n",x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w S e t V i e w b o x                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawSetViewbox() sets the overall canvas size to be recorded with the
%  drawing vector data.  Usually this will be specified using the same
%  size as the canvas image.  When the vector data is saved to SVG or MVG
%  formats, the viewbox is use to specify the size of the canvas image that
%  a viewer will render the vector data on.
%
%  The format of the MagickDrawSetViewbox method is:
%
%      void MagickDrawSetViewbox(DrawingWand *drawing_wand,unsigned long x1,
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
WandExport void MagickDrawSetViewbox(DrawingWand *drawing_wand,unsigned long x1,
  unsigned long y1,unsigned long x2,unsigned long y2)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  (void) MvgPrintf(drawing_wand,"viewbox %lu %lu %lu %lu\n",x1,y1,x2,y2);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k N e w D r a w i n g W a n d                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickNewDrawingWand() returns a drawing wand required for all other methods in
%  the API.
%
%  The format of the MagickNewDrawingWand method is:
%
%      DrawingWand *MagickNewDrawingWand(void)
%
%
*/
WandExport DrawingWand *MagickNewDrawingWand(void)
{
  Image
    *image;

  image=AllocateImage((const ImageInfo *) NULL);
  /* A crummy way to determine that this image can be deleted. */
  strlcpy(image->filename,dummy_image_filename,MaxTextExtent);
  return(MagickDrawAllocateWand((const DrawInfo *) NULL,image));
}
