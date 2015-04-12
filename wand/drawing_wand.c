/* Copyright (C) 2003-2015 GraphicsMagick Group */
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
#include "wand/wand_private.h"

/*
  Define declarations.
*/
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
  ExceptionInfo
    exception;

  Image
    *image;

  MagickBool
    own_image;          /* If true, then we own the image. */

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
  int
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
MAGICK_ATTRIBUTE ((format (printf, 2, 3)))
#endif
,
  MvgAutoWrapPrintf(DrawingWand *drawing_wand, const char *format, ...)
#if defined(__GNUC__)
MAGICK_ATTRIBUTE ((format (printf, 2, 3)))
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
      drawing_wand->mvg=MagickAllocateMemory(char *,alloc_size);
      if ( drawing_wand->mvg == (char*) NULL )
        {
          ThrowException3(&drawing_wand->exception,
            ResourceLimitError,MemoryAllocationFailed,UnableToDrawOnImage);
          return(-1);
        }

      drawing_wand->mvg_alloc=alloc_size;
      drawing_wand->mvg_length=0;
      if (drawing_wand->mvg == 0)
        {
          ThrowException3(&drawing_wand->exception,
            ResourceLimitError,MemoryAllocationFailed,UnableToDrawOnImage);
          return(-1);
        }
    }
  if (drawing_wand->mvg_alloc < (drawing_wand->mvg_length + MaxTextExtent * 10))
    {
      size_t realloc_size=drawing_wand->mvg_alloc + alloc_size;

      MagickReallocMemory(char *,drawing_wand->mvg,realloc_size);
      if (drawing_wand->mvg == NULL)
        {
          ThrowException3(&drawing_wand->exception,
            ResourceLimitError,MemoryAllocationFailed,UnableToDrawOnImage);
          return -1;
        }
      drawing_wand->mvg_alloc=realloc_size;
    }
  {
    ssize_t
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

    /*
      According to C99, vsnprintf() returns the number of formatted
      bytes which WOULD have been written (not including trailing
      '\0') if there was sufficient space available.  If an output
      error occurs, then a negative value is returned.

      Under Linux glibc 2.0.6 and earlier, -1 is returned when the
      output has been truncated.
    */
    {
      size_t
		space_available;
      
      space_available=drawing_wand->mvg_alloc-drawing_wand->mvg_length-1;
      formatted_length = -1;
      if (space_available > 0)
	{
	  
	  va_start(argp, format);
#if defined(HAVE_VSNPRINTF)
	  formatted_length=vsnprintf(drawing_wand->mvg+drawing_wand->mvg_length,
				     (size_t) space_available,format,argp);
#else
	  formatted_length=vsprintf(drawing_wand->mvg+drawing_wand->mvg_length,
				    format,argp);
#endif
	  va_end(argp);
	}
      if ((formatted_length < 0) || ((size_t) formatted_length > space_available))
	{
	  ThrowException(&drawing_wand->exception,DrawError,
			 UnableToPrint,format);
	}
      else
	{
	  drawing_wand->mvg_length+=formatted_length;
	  drawing_wand->mvg_width+=formatted_length;
	}
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
      ThrowException(&drawing_wand->exception,DrawError,
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

      GetColorTuple(color,QuantumDepth,MagickTrue,MagickTrue,tuple);
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
      (void) MvgAutoWrapPrintf(drawing_wand," %g,%g",coordinate->x,coordinate->y);
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
%   C l o n e D r a w i n g W a n d                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneDrawingWand() returns a new drawing wand which is a full (deep) copy
%  of an existing drawing wand.
%
%  The format of the  CloneDrawingWand method is:
%
%      DrawingWand *CloneDrawingWand(const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand to copy
%
*/
WandExport DrawingWand *CloneDrawingWand(const DrawingWand *drawing_wand)
{  
  DrawingWand
    *clone_wand;

  ExceptionInfo
    exeption_info;
  
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  
  clone_wand=MagickAllocateMemory(DrawingWand *,sizeof(*clone_wand));
  if (clone_wand == (DrawingWand *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
		      UnableToAllocateDrawingWand);

  GetExceptionInfo(&exeption_info);
  (void) memcpy(clone_wand,drawing_wand,sizeof(*clone_wand));
  GetExceptionInfo(&clone_wand->exception);
  clone_wand->image=(Image *) NULL;
  clone_wand->mvg=(char *) NULL;
  clone_wand->pattern_id=(char *) NULL;
  clone_wand->graphic_context=(DrawInfo **) NULL;
  
  clone_wand->own_image=MagickTrue;
  if (drawing_wand->image != (Image *) NULL)
    {
      clone_wand->image=CloneImage(drawing_wand->image,0,0,MagickTrue,
				   &exeption_info);
      if (clone_wand->image == (Image *) NULL)
	goto clone_drawing_wand_fail;
    }
  
  if (drawing_wand->mvg)
    {
      clone_wand->mvg=MagickAllocateMemory(char *,drawing_wand->mvg_alloc);
      if (clone_wand->mvg == (char *) NULL)
	{
	  ThrowException3(&exeption_info,ResourceLimitError,
			  MemoryAllocationFailed,UnableToCloneDrawingWand);
	  goto clone_drawing_wand_fail;
	}
      (void) memcpy(clone_wand->mvg,drawing_wand->mvg,drawing_wand->mvg_length+1);
    }

  if (drawing_wand->pattern_id != (const char *) NULL)
    clone_wand->pattern_id=AllocateString(drawing_wand->pattern_id);

  if (drawing_wand->graphic_context != (DrawInfo **) NULL)
    {
      clone_wand->graphic_context=MagickAllocateArray(DrawInfo **,
						      drawing_wand->index+1,
						      sizeof(DrawInfo *));
      if (clone_wand->graphic_context == (DrawInfo **) NULL)
	{
	  ThrowException3(&exeption_info,ResourceLimitError,
			  MemoryAllocationFailed,UnableToCloneDrawingWand);
	  goto clone_drawing_wand_fail;
	}
      (void) memset(clone_wand->graphic_context,0,
		    (drawing_wand->index+1)*sizeof(DrawInfo *));

      for (clone_wand->index=0; clone_wand->index <= drawing_wand->index; clone_wand->index++)
	{
	  clone_wand->graphic_context[clone_wand->index]=
	    CloneDrawInfo((ImageInfo*)NULL,drawing_wand->graphic_context[clone_wand->index]);
	  if (clone_wand->graphic_context[clone_wand->index] == (DrawInfo*) NULL)
	    {
	      ThrowException3(&exeption_info,ResourceLimitError,
			      MemoryAllocationFailed,UnableToCloneDrawingWand);
	      goto clone_drawing_wand_fail;
	    }
	}
      clone_wand->index=drawing_wand->index;
    }

  return(clone_wand);

clone_drawing_wand_fail:

  if (clone_wand->image != (Image *) NULL)
    DestroyImage(clone_wand->image);
  MagickFreeMemory(clone_wand->mvg);
  MagickFreeMemory(clone_wand->pattern_id);
  if (clone_wand->graphic_context != (DrawInfo **) NULL)
    {
      for ( ; clone_wand->index >= 0; clone_wand->index--)
	{
	  if (clone_wand->graphic_context[clone_wand->index] != (DrawInfo*) NULL)
	    DestroyDrawInfo(clone_wand->graphic_context[clone_wand->index]);
	  clone_wand->graphic_context[clone_wand->index]=(DrawInfo*) NULL;
	}
      MagickFreeMemory(clone_wand->graphic_context);
    }
  (void) memset(clone_wand,0,sizeof(*clone_wand));
  MagickFreeMemory(clone_wand);
  /* No place to report exception, but it may be logged. */
  DestroyExceptionInfo(&exeption_info);
  return(clone_wand);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y D r a w i n g W a n d                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyDrawingWand() frees all resources associated with the drawing
%  wand. Once the drawing wand has been freed, it should not be used
%  any further unless it re-allocated.
%
%  The format of the  DestroyDrawingWand method is:
%
%      void  DestroyDrawingWand(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand to destroy.
%
*/
WandExport void DestroyDrawingWand(DrawingWand *drawing_wand)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);

  DestroyExceptionInfo(&drawing_wand->exception);

  if ((drawing_wand->image != (Image*) NULL) &&
      (drawing_wand->own_image == MagickTrue))
    DestroyImage(drawing_wand->image);

  MagickFreeMemory(drawing_wand->mvg);

  MagickFreeMemory(drawing_wand->pattern_id);

  if (drawing_wand->graphic_context != (DrawInfo **) NULL)
    {
      for ( ; drawing_wand->index >= 0; drawing_wand->index--)
	{
	  if (drawing_wand->graphic_context[drawing_wand->index] != (DrawInfo*) NULL)
	    DestroyDrawInfo(drawing_wand->graphic_context[drawing_wand->index]);
	  drawing_wand->graphic_context[drawing_wand->index]=(DrawInfo*) NULL;
	}
      MagickFreeMemory(drawing_wand->graphic_context);
    }

  (void) memset(drawing_wand,0,sizeof(*drawing_wand));
  MagickFreeMemory(drawing_wand);
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
  (void) MvgPrintf(drawing_wand,"text %g,%g '%s'\n",x,y,escaped_text);
  MagickFreeMemory(escaped_text);
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
  (void) MvgPrintf(drawing_wand,"affine %.6g,%.6g,%.6g,%.6g,%.6g,%.6g\n",affine->sx,
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

  drawing_wand=NewDrawingWand();
  if (draw_info != (const DrawInfo *) NULL)
    {
      DestroyDrawInfo(CurrentContext);
      CurrentContext=CloneDrawInfo((ImageInfo *) NULL,draw_info);
    }
  if (image != (Image *) NULL)
    {
      DestroyImage(drawing_wand->image);
      drawing_wand->own_image=MagickFalse;
    }
  drawing_wand->image=image;
  return drawing_wand;
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
  (void) MvgPrintf(drawing_wand,"arc %g,%g %g,%g %g,%g\n",sx,sy,ex,ey,
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
WandExport void DrawBezier(DrawingWand *drawing_wand,
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
  (void) MvgPrintf(drawing_wand, "circle %g,%g %g,%g\n",ox,oy,px,py);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w C l e a r E x c e p t i o n                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawClearException() clears any existing exception from the drawing wand.
%
%  The format of the DrawGetException method is:
%
%      MagickPassFail DrawClearException(DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
*/
WandExport MagickBool DrawClearException(DrawingWand *drawing_wand)
{
  DestroyExceptionInfo(&drawing_wand->exception);
  GetExceptionInfo(&drawing_wand->exception);
  return MagickPass;
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
      (void) CloneString(&CurrentContext->clip_path,clip_path);
      if (CurrentContext->clip_path == (char*)NULL)
        ThrowException3(&drawing_wand->exception,ResourceLimitError,
          MemoryAllocationFailed,UnableToDrawOnImage);
      (void) MvgPrintf(drawing_wand,"clip-path url(#%s)\n",clip_path);
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
        (void) MvgPrintf(drawing_wand, "clip-rule %s\n", p);
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
%   D r a w G e t E x c e p t i o n                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetException() obtains error information associated with the last
%  exception (if any).  If an exception did occur, an allocated text string
%  is returned which contains a detailed description of the exception.  This
%  string must be deallocated by the user once it is no longer needed.
%
%  The format of the DrawGetException method is:
%
%      char *DrawGetException(const DrawingWand *drawing_wand,
%                             ExceptionType *severity)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o severity: Enumeration corresponding to last thrown exception.
%
*/
WandExport char *DrawGetException(const DrawingWand *drawing_wand,
				  ExceptionType *severity)
{
  char
    buffer[MaxTextExtent],
    *description;
  
  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  assert(drawing_wand->exception.signature == MagickSignature);

  description=(char *) NULL;
  buffer[0]='\0';
  *severity=drawing_wand->exception.severity;
  if (drawing_wand->exception.severity != UndefinedException)
    {
      if (drawing_wand->exception.description)
	FormatString(buffer,"%.1024s (%.1024s)",
		     drawing_wand->exception.reason,
		     drawing_wand->exception.description);
      else
	FormatString(buffer,"%.1024s",
		     drawing_wand->exception.reason);
      CloneString(&description,buffer);
    }

  return(description);
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

          IdentityAffine(&affine);
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
    (void) MvgPrintf(drawing_wand, "color %g,%g %s\n", x, y, p);
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
  (void) MvgPrintf(drawing_wand,"#%s\n",comment);
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
  (void) MvgPrintf(drawing_wand,"ellipse %g,%g %g,%g %g,%g\n",ox,oy,rx,ry,
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
    ThrowException(&drawing_wand->exception,DrawWarning,
      NotARelativeURL,fill_url);
  (void) MagickFormatString(pattern,MaxTextExtent,"[%.1024s]",fill_url+1);
  if (GetImageAttribute(drawing_wand->image,pattern) == (ImageAttribute *) NULL)
    {
      ThrowException(&drawing_wand->exception,DrawWarning,
        URLNotFound,fill_url);
    }
  else
    {
      char
        pattern_spec[MaxTextExtent];

      (void) MagickFormatString(pattern_spec,MaxTextExtent,"url(%.1024s)",
        fill_url);
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
      (void) MvgPrintf(drawing_wand,"fill-opacity %g\n",fill_opacity);
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
        (void) MvgPrintf(drawing_wand,"fill-rule %s\n",p);
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
        ThrowException3(&drawing_wand->exception,ResourceLimitError,
          MemoryAllocationFailed,UnableToDrawOnImage);
      (void) MvgPrintf(drawing_wand,"font '%s'\n",font_name);
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
        ThrowException3(&drawing_wand->exception,ResourceLimitError,
          MemoryAllocationFailed,UnableToDrawOnImage);
      (void) MvgPrintf(drawing_wand,"font-family '%s'\n",font_family);
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
      (void) MvgPrintf(drawing_wand,"font-size %g\n",pointsize);
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
        (void) MvgPrintf(drawing_wand,"font-stretch '%s'\n",p);
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
        (void) MvgPrintf(drawing_wand, "font-style '%s'\n", p);
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
      (void) MvgPrintf(drawing_wand,"font-weight %lu\n",font_weight);
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
        (void) MvgPrintf(drawing_wand,"gravity %s\n",p);
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
  clone_image=CloneImage(image,0,0,MagickTrue,&drawing_wand->exception);
  if (!clone_image)
    return;
  image_info=CloneImageInfo((ImageInfo*)NULL);
  if (!image_info)
    ThrowException3(&drawing_wand->exception,ResourceLimitError,
      MemoryAllocationFailed,UnableToDrawOnImage);
  handler=SetMonitorHandler((MonitorHandler) NULL);
  blob=(unsigned char*) ImageToBlob( image_info,clone_image,&blob_length,
    &drawing_wand->exception);
  (void) SetMonitorHandler(handler);
  DestroyImageInfo(image_info);
  DestroyImageList(clone_image);
  if (!blob)
    return;
  base64=Base64Encode(blob,blob_length,&encoded_length);
  MagickFreeMemory(blob);
  if (!base64)
    {
      char
        buffer[MaxTextExtent];

      (void) MagickFormatString(buffer,MaxTextExtent,"%"
                                MAGICK_SIZE_T_F "d bytes",
        (4L*blob_length/3L+4L));
      ThrowException(&drawing_wand->exception,ResourceLimitWarning,
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

      (void) MvgPrintf(drawing_wand,"image %s %g,%g %g,%g 'data:%s;base64,\n",
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
  MagickFreeMemory(base64);
  MagickFreeMemory(media_type);
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
  (void) MvgPrintf(drawing_wand,"line %g,%g %g,%g\n",sx,sy,ex,ey);
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
    (void) MvgPrintf(drawing_wand,"matte %g,%g %s\n",x,y,p);
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
  (void) MvgAutoWrapPrintf(drawing_wand,"%s",
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
      (void) MvgAutoWrapPrintf(drawing_wand, "%c%g,%g %g,%g %g,%g",
        mode == AbsolutePathMode ? 'C' : 'c',x1,y1,x2,y2,x,y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %g,%g %g,%g %g,%g",
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                                %
%                                                                                %
%                                                                                %
%   D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r A b s o l u t e  %
%                                                                                %
%                                                                                %
%                                                                                %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
      (void) MvgAutoWrapPrintf(drawing_wand, "%c%g,%g %g,%g",
        mode == AbsolutePathMode ? 'Q' : 'q',x1,y1,x,y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %g,%g %g,%g",x1,y1,x,y);
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                                 %
%                                                                                 %
%                                                                                 %
%   D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r R e l a t i v e   %
%                                                                                 %
%                                                                                 %
%                                                                                 %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                                             %
%                                                                                             %
%                                                                                             %
%   D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r S m o o t h A b s o l u t e   %
%                                                                                             %
%                                                                                             %
%                                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToQuadraticBezierSmoothAbsolute() draws a quadratic
%  Bezier curve (using absolute coordinates) from the current point to
%  (x,y). The control point is assumed to be the reflection of the
%  control point on the previous command relative to the current
%  point. (If there is no previous command or if the previous command was
%  not a DrawPathCurveToQuadraticBezierAbsolute,
%  DrawPathCurveToQuadraticBezierRelative,
%  DrawPathCurveToQuadraticBezierSmoothAbsolute or
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
      (void) MvgAutoWrapPrintf(drawing_wand, "%c%g,%g",
        mode == AbsolutePathMode ? 'T' : 't',x,y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %g,%g",x,y);
}

WandExport void DrawPathCurveToQuadraticBezierSmoothAbsolute(
  DrawingWand *drawing_wand,const double x,const double y)
{
  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  DrawPathCurveToQuadraticBezierSmooth(drawing_wand,AbsolutePathMode,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                                             %
%                                                                                             %
%                                                                                             %
%   D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r S m o o t h R e l a t i v e   %
%                                                                                             %
%                                                                                             %
%                                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToQuadraticBezierSmoothRelative() draws a quadratic
%  Bezier curve (using relative coordinates) from the current point to
%  (x,y). The control point is assumed to be the reflection of the
%  control point on the previous command relative to the current
%  point. (If there is no previous command or if the previous command was
%  not a DrawPathCurveToQuadraticBezierAbsolute,
%  DrawPathCurveToQuadraticBezierRelative,
%  DrawPathCurveToQuadraticBezierSmoothAbsolute or
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
      (void) MvgAutoWrapPrintf(drawing_wand,"%c%g,%g %g,%g",
        mode == AbsolutePathMode ? 'S' : 's',x2,y2,x,y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %g,%g %g,%g",x2,y2,x,y);
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
      (void) MvgAutoWrapPrintf(drawing_wand, "%c%g,%g %g %u %u %g,%g",
        mode == AbsolutePathMode ? 'A' : 'a',rx,ry,x_axis_rotation,
        large_arc_flag,sweep_flag,x,y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %g,%g %g %u %u %g,%g",rx,ry,
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
  (void) MvgPrintf(drawing_wand,"'\n");
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
      (void) MvgAutoWrapPrintf(drawing_wand,"%c%g,%g",
        mode == AbsolutePathMode ? 'L' : 'l',x,y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %g,%g",x,y);
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                                %
%                                                                                %
%                                                                                %
%   D r a w P a t h L i n e T o H o r i z o n t a l A b s o l u t e              %
%                                                                                %
%                                                                                %
%                                                                                %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
      (void) MvgAutoWrapPrintf(drawing_wand,"%c%g",
        mode == AbsolutePathMode ? 'H' : 'h',x);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %g",x);
}

WandExport void DrawPathLineToHorizontalAbsolute(DrawingWand *drawing_wand,
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
%   D r a w P a t h L i n e T o H o r i z o n t a l R e l a t i v e              %
%                                                                                %
%                                                                                %
%                                                                                %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
      (void) MvgAutoWrapPrintf(drawing_wand,"%c%g",
        mode == AbsolutePathMode ? 'V' : 'v',y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %g",y);
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
      (void) MvgAutoWrapPrintf(drawing_wand,"%c%g,%g",
        mode == AbsolutePathMode ? 'M' : 'm',x,y);
    }
  else
    (void) MvgAutoWrapPrintf(drawing_wand," %g,%g",x,y);
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
  (void) MvgPrintf(drawing_wand,"path '");
  drawing_wand->path_operation=PathDefaultOperation;
  drawing_wand->path_mode=DefaultPathMode;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P e e k G r a p h i c C o n t e x t                               %
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
  (void) CloneString(&draw_info->primitive,drawing_wand->mvg);
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
  (void) MvgPrintf(drawing_wand,"point %g,%g\n",x,y);
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
WandExport void DrawPolygon(DrawingWand *drawing_wand,
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
WandExport void DrawPolyline(DrawingWand *drawing_wand,
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
  (void) MvgPrintf(drawing_wand,"pop clip-path\n");
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
  (void) MvgPrintf(drawing_wand,"pop defs\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P o p G r a p h i c C o n t e x t                                 %
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
      ThrowException(&drawing_wand->exception,DrawError,
        UnbalancedGraphicContextPushPop,NULL);
    }
  else
    {
      /* Destroy clip path if not same in preceding drawing_wand */
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
    ThrowException(&drawing_wand->exception,DrawWarning,
      NotCurrentlyPushingPatternDefinition,NULL);
  (void) MagickFormatString(key,MaxTextExtent,"[%.1024s]",
    drawing_wand->pattern_id);
  (void) SetImageAttribute(drawing_wand->image,key,
    drawing_wand->mvg+drawing_wand->pattern_offset);
  (void) MagickFormatString(geometry,MaxTextExtent,"%lux%lu%+ld%+ld",
    drawing_wand->pattern_bounds.width,drawing_wand->pattern_bounds.height,
    drawing_wand->pattern_bounds.x,drawing_wand->pattern_bounds.y);
  (void) SetImageAttribute(drawing_wand->image,key,geometry);
  MagickFreeMemory(drawing_wand->pattern_id);
  drawing_wand->pattern_id=NULL;
  drawing_wand->pattern_offset=0;
  drawing_wand->pattern_bounds.x=0;
  drawing_wand->pattern_bounds.y=0;
  drawing_wand->pattern_bounds.width=0;
  drawing_wand->pattern_bounds.height=0;
  drawing_wand->filter_off=MagickFalse;
  if (drawing_wand->indent_depth > 0)
    drawing_wand->indent_depth--;
  (void) MvgPrintf(drawing_wand,"pop pattern\n");
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
  (void) MvgPrintf(drawing_wand,"push clip-path %s\n",clip_path_id);
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
  (void) MvgPrintf(drawing_wand,"push defs\n");
  drawing_wand->indent_depth++;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P u s h G r a p h i c C o n t e x t                               %
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
  MagickReallocMemory(DrawInfo **,drawing_wand->graphic_context,
                      (drawing_wand->index+1)*sizeof(DrawInfo *));
  if (drawing_wand->graphic_context == (DrawInfo **) NULL)
    {
      drawing_wand->index--;
      ThrowException3(&drawing_wand->exception,ResourceLimitError,
                      MemoryAllocationFailed,UnableToDrawOnImage);
      return;
    }
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
    ThrowException(&drawing_wand->exception,DrawError,
      AlreadyPushingPatternDefinition,drawing_wand->pattern_id);
  drawing_wand->filter_off=MagickTrue;
  (void) MvgPrintf(drawing_wand,"push pattern %s %g,%g %g,%g\n",pattern_id,
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
  (void) MvgPrintf(drawing_wand,"rectangle %g,%g %g,%g\n",x1,y1,x2,y2);
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
%  This function is deprecated.  Use MagickDrawImage() instead.
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
  MagickPassFail
    status;

  assert(drawing_wand != (const DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  CurrentContext->primitive=drawing_wand->mvg;
  (void) LogMagickEvent(DrawEvent,GetMagickModule(),"MVG:\n'%s'\n",
    drawing_wand->mvg);
  status=DrawImage(drawing_wand->image, CurrentContext);
  CurrentContext->primitive=(char *) NULL;
  return status;
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
  IdentityAffine(&affine);
  affine.sx=cos(DegreesToRadians(fmod(degrees,360.0)));
  affine.rx=sin(DegreesToRadians(fmod(degrees,360.0)));
  affine.ry=(-sin(DegreesToRadians(fmod(degrees,360.0))));
  affine.sy=cos(DegreesToRadians(fmod(degrees,360.0)));
  AdjustAffine(drawing_wand,&affine);
  (void) MvgPrintf(drawing_wand,"rotate %g\n",degrees);
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
  (void) MvgPrintf(drawing_wand,"roundrectangle %g,%g %g,%g %g,%g\n",
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
  IdentityAffine(&affine);
  affine.sx=x;
  affine.sy=y;
  AdjustAffine( drawing_wand, &affine );
  (void) MvgPrintf(drawing_wand,"scale %g,%g\n",x,y);
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
  IdentityAffine(&affine);
  affine.ry=tan(DegreesToRadians(fmod(degrees,360.0)));
  AdjustAffine(drawing_wand,&affine);
  (void) MvgPrintf(drawing_wand,"skewX %g\n",degrees);
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
  IdentityAffine(&affine);
  affine.rx=tan(DegreesToRadians(fmod(degrees,360.0)));
  DrawAffine(drawing_wand,&affine);
  (void) MvgPrintf(drawing_wand,"skewY %g\n",degrees);
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
    ThrowException(&drawing_wand->exception,DrawWarning,
      NotARelativeURL,stroke_url);
  (void) MagickFormatString(pattern,MaxTextExtent,"[%.1024s]",stroke_url+1);
  if (GetImageAttribute(drawing_wand->image,pattern) == (ImageAttribute *) NULL)
    {
      ThrowException(&drawing_wand->exception,DrawWarning,
        URLNotFound,stroke_url);
    }
  else
    {
      char
        pattern_spec[MaxTextExtent];

      (void) MagickFormatString(pattern_spec,MaxTextExtent,"url(%.1024s)",
        stroke_url);
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
      (void) MvgPrintf(drawing_wand,"stroke-antialias %i\n",stroke_antialias ? 1 : 0);
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
%        unsigned long *number_elements)
%
%  A description of each parameter follows:
%
%    o drawing_wand: The drawing wand.
%
%    o number_elements: address to place number of elements in dash array
%
% */
WandExport double *DrawGetStrokeDashArray(const DrawingWand *drawing_wand,
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
    while (*p++ != 0.0)
      n++;
  *number_elements=n;
  dash_array=(double *)NULL;
  if (n != 0)
    {
      dash_array=MagickAllocateArray(double *, n+1, sizeof(double));
      p=CurrentContext->dash_pattern;
      q=dash_array;
      i=n;
      while (i--)
        *q++=(*p++);
      *q=0.0;
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
%  stroke paths. The stroke dash array represents an array of numbers that
%  specify the lengths of alternating dashes and gaps in pixels. If an odd
%  number of values is provided, then the list of values is repeated to yield
%  an even number of values. To remove an existing dash array, pass a zero
%  number_elements argument and null dash_array.
%  A typical stroke dash array might contain the members 5 3 2.
%
%  The format of the DrawSetStrokeDashArray method is:
%
%      void DrawSetStrokeDashArray(DrawingWand *drawing_wand,
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
WandExport void DrawSetStrokeDashArray(DrawingWand *drawing_wand,
  const unsigned long number_elements,const double *dash_array)
{
  register const double
    *p;

  register double
    *q;

  unsigned long
    i,
    n_new = number_elements,
    n_old = 0;

  MagickBool
    updated = MagickFalse;

  assert(drawing_wand != (DrawingWand *) NULL);
  assert(drawing_wand->signature == MagickSignature);
  if (dash_array == (const double *) NULL)
    n_new = 0;
  q=CurrentContext->dash_pattern;
  if (q != (const double *) NULL)
    while (*q++ != 0.0)
      n_old++;

  if ((n_old == 0) && (n_new == 0))
    {
      updated=MagickFalse;
    }
  else if ( n_old != n_new )
    {
      updated=MagickTrue;
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
            updated=MagickTrue;
            break;
          }
        p++;
        q++;
      }
    }
  if (drawing_wand->filter_off || updated)
    {
      if (CurrentContext->dash_pattern != (double *) NULL)
        MagickFreeMemory(CurrentContext->dash_pattern);
      if (n_new != 0)
        {
          CurrentContext->dash_pattern=MagickAllocateArray(double *,
                                                           (n_new+1),
                                                           sizeof(double));
          if (!CurrentContext->dash_pattern)
            {
              ThrowException3(&drawing_wand->exception,
                ResourceLimitError,MemoryAllocationFailed,UnableToDrawOnImage);
            }
          else
            {
              for (i=0; i < n_new; i++)
                CurrentContext->dash_pattern[i]=dash_array[i];
              CurrentContext->dash_pattern[n_new]=0.0;
            }
        }
      (void) MvgPrintf(drawing_wand,"stroke-dasharray ");
      if ( n_new == 0 )
        (void) MvgPrintf(drawing_wand, "none");
      else
        {
          for (i=0; i < n_new; i++)
            {
              if (i != 0)
                (void) MvgPrintf(drawing_wand, ",");
              (void) MvgPrintf(drawing_wand, "%g", dash_array[i]);
            }
        }
      (void) MvgPrintf(drawing_wand,"\n");
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
      (void) MvgPrintf(drawing_wand,"stroke-dashoffset %g\n",dash_offset);
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
        (void) MvgPrintf(drawing_wand,"stroke-linecap %s\n",p);
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
          p="bevel";
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
      (void) MvgPrintf(drawing_wand,"stroke-miterlimit %lu\n",miterlimit);
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
      (void) MvgPrintf(drawing_wand,"stroke-opacity %g\n",stroke_opacity);
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
      (void) MvgPrintf(drawing_wand,"stroke-width %g\n",stroke_width);
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
      (void) MvgPrintf(drawing_wand,"text-antialias %i\n",text_antialias ? 1 : 0);
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
        (void) MvgPrintf(drawing_wand,"decorate %s\n",p);
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
      (void) CloneString(&CurrentContext->encoding,encoding);
      (void) MvgPrintf(drawing_wand,"encoding '%s'\n",encoding);
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
  IdentityAffine(&affine);
  affine.tx=x;
  affine.ty=y;
  AdjustAffine(drawing_wand,&affine );
  (void) MvgPrintf(drawing_wand,"translate %g,%g\n",x,y);
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
  (void) MvgPrintf(drawing_wand,"viewbox %lu %lu %lu %lu\n",x1,y1,x2,y2);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N e w D r a w i n g W a n d                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  NewDrawingWand() returns a drawing wand required for all other methods in
%  the API.
%
%  The format of the NewDrawingWand method is:
%
%      DrawingWand *NewDrawingWand(void)
%
%
*/
WandExport DrawingWand *NewDrawingWand(void)
{
  DrawingWand
    *drawing_wand;

  /*
    Initialize GraphicsMagick in case it is not already initialized.
  */
  InitializeMagick(NULL);

  drawing_wand=MagickAllocateMemory(DrawingWand *,sizeof(struct _DrawingWand));
  if (drawing_wand == (DrawingWand *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateDrawingWand);
  GetExceptionInfo(&drawing_wand->exception);
  drawing_wand->image=AllocateImage((const ImageInfo *) NULL);
  drawing_wand->own_image=MagickTrue;
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
  drawing_wand->graphic_context=MagickAllocateMemory(DrawInfo **,
                                                     sizeof(DrawInfo *));
  if (drawing_wand->graphic_context == (DrawInfo **) NULL)
    {
      ThrowException3(&drawing_wand->exception,ResourceLimitError,
        MemoryAllocationFailed,UnableToDrawOnImage);
      return (DrawingWand *) NULL;
    }
  CurrentContext=CloneDrawInfo((ImageInfo*)NULL,(DrawInfo *) NULL);
  if (CurrentContext == (DrawInfo*) NULL)
    {
      ThrowException3(&drawing_wand->exception,ResourceLimitError,
        MemoryAllocationFailed,UnableToDrawOnImage);
      return (DrawingWand *) NULL;
    }
  drawing_wand->filter_off=MagickFalse;
  drawing_wand->indent_depth=0;
  drawing_wand->path_operation=PathDefaultOperation;
  drawing_wand->path_mode=DefaultPathMode;
  drawing_wand->signature=MagickSignature;
  return(drawing_wand);
}
