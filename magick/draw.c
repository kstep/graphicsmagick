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
%                   ImageMagick Image Vector Drawing Methods                  %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Bob Friesenhahn                                %
%                                March 2002                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
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
#include "studio.h"

/*
  Define declarations.
*/
#define ThrowDrawException(code,reason,description) \
{ \
  if (context->image->exception.severity > (long)code) \
    ThrowException(&context->image->exception,code,reason,description); \
  return; \
}
#define CurrentContext (context->graphic_context[context->index])
#define PixelPacketMatch(p,q) (((p)->red == (q)->red) && \
  ((p)->green == (q)->green) && ((p)->blue == (q)->blue) && \
  ((p)->opacity == (q)->opacity))

/*
  Typedef declarations.
*/
typedef enum
{
  PathDefaultOperation,
  PathCloseOperation,                           /* Z|z (none) */
  PathCurveToOperation,                         /* C|c (x1 y1 x2 y2 x y)+ */
  PathCurveToQuadraticBezierOperation,          /* Q|q (x1 y1 x y)+ */
  PathCurveToQuadraticBezierSmoothOperation,    /* T|t (x y)+ */
  PathCurveToSmoothOperation,                   /* S|s (x2 y2 x y)+ */
  PathEllipticArcOperation,                     /* A|a (rx ry x-axis-rotation large-arc-flag sweep-flag x y)+ */
  PathLineToHorizontalOperation,                /* H|h x+ */
  PathLineToOperation,                          /* L|l (x y)+ */
  PathLineToVerticalOperation,                  /* V|v y+ */
  PathMoveToOperation                           /* M|m (x y)+ */
}
PathOperation;

typedef enum
{
  DefaultPathMode,
  AbsolutePathMode,
  RelativePathMode
}
PathMode;

struct _DrawContext
{
  /* Support structures */
  ImageInfo
    *image_info;

  Image
    *image;

  /* MVG output string and housekeeping */
  char
    *mvg;

  size_t
    mvg_alloc,
    mvg_length;

  /* Graphic context */
  long
    index;

  DrawInfo
    **graphic_context;

  int
    filter_off;

  /* Pretty-printing depth */
  long
    indent_depth;

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
  MvgPrintf(DrawContext context, const char *format, ...);
static void
  MvgAppendColor(DrawContext context, const PixelPacket *color);


static int MvgPrintf(DrawContext context, const char *format, ...)
{
  const size_t
    alloc_size = MaxTextExtent * 20; /* 40K */

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  /* Allocate initial memory */
  if (context->mvg == 0)
    {
      context->mvg = (char *) AcquireMemory(alloc_size);
      context->mvg_alloc = alloc_size;
      context->mvg_length = 0;
      if (context->mvg == 0)
        return -1;
    }

  /* Re-allocate additional memory if necessary (ensure 20K unused) */
  if (context->mvg_alloc < (context->mvg_length + MaxTextExtent * 10))
    {
      size_t realloc_size = context->mvg_alloc + alloc_size;

      ReacquireMemory((void **) &context->mvg, realloc_size);
      if (context->mvg == NULL)
        return -1;
      context->mvg_alloc = realloc_size;
    }

  /* Write to end of existing MVG string */
  {
    long
      str_length;

    va_list
      argp;

    /* Pretty-print indentation */
    if (*(context->mvg + context->mvg_length - 1) == '\n')
      {
        long i;

        for (i = context->indent_depth; i; i--)
          {
            *(context->mvg + context->mvg_length) = ' ';
            ++context->mvg_length;
          }
        *(context->mvg + context->mvg_length) = 0;
      }

    va_start(argp, format);
#if !defined(HAVE_VSNPRINTF)
    str_length = vsprintf(context->mvg + context->mvg_length, format, argp);
#else
    str_length =
      vsnprintf(context->mvg + context->mvg_length,
                context->mvg_alloc - context->mvg_length - 1, format, argp);
#endif
    va_end(argp);

    context->mvg_length += str_length;
    *(context->mvg + context->mvg_length) = 0;

    assert(context->mvg_length + 1 < context->mvg_alloc);

    return str_length;
  }
}

static void MvgAppendColor(DrawContext context, const PixelPacket *color)
{
  if(color->red == 0 && color->green == 0 && color->blue == 0 &&
     color->opacity == TransparentOpacity)
    {
      MvgPrintf(context,"none");
    }
  else
    {
      MvgPrintf(context,
#if QuantumDepth == 8
                "#%02x%02x%02x",
#elif QuantumDepth == 16
                "#%04x%04x%04x",
#endif
                color->red, color->green, color->blue);

      if (color->opacity != OpaqueOpacity)
        MvgPrintf(context,
#if QuantumDepth == 8
                  "%02x",
#elif QuantumDepth == 16
                  "%04x",
#endif
                  color->opacity);
    }
}

MagickExport void DrawAnnotation(DrawContext context,
                                 const double x, const double y,
                                 const unsigned char *text)
{
  unsigned char
    escaped_string[MaxTextExtent];

  unsigned const char
    *p;

  unsigned char
    *q;

  unsigned int
    string_length;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(text != (const unsigned char *) NULL);

  for (p = text, q = escaped_string, string_length = 0;
       (*p != 0) && (string_length < (sizeof(escaped_string) - 3));
       ++p)
    {
      if (*p == '\'')
        {
          *q++ = '\\';
          *q++ = '\\';
          string_length += 2;
        }
      else
        {
          *q++ = (*p);
          ++string_length;
        }
    }
  *q = 0;

  MvgPrintf(context, "text %.4g,%.4g '%.1024s'\n", x, y, escaped_string);
}

MagickExport void DrawSetAffine(DrawContext context, const AffineMatrix *affine)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

#if 0
  if ((affine->sx != 1.0) || (affine->rx != 0.0) || (affine->ry != 0.0) ||
      (affine->sy != 1.0) || (affine->tx != 0.0) || (affine->ty != 0.0))
    {
      AffineMatrix
        current;

      current = CurrentContext->affine;
      CurrentContext->affine.sx=current.sx*affine->sx+current.ry*affine->rx;
      CurrentContext->affine.rx=current.rx*affine->sx+current.sy*affine->rx;
      CurrentContext->affine.ry=current.sx*affine->ry+current.ry*affine->sy;
      CurrentContext->affine.sy=current.rx*affine->ry+current.sy*affine->sy;
      CurrentContext->affine.tx=current.sx*affine->tx+current.ry*affine->ty+current.tx;
      CurrentContext->affine.ty=current.rx*affine->tx+current.sy*affine->ty+current.ty;
    }
#endif

  MvgPrintf(context, "affine %.4g,%.4g,%.4g,%.4g,%.4g,%.4g\n",
             affine->sy, affine->rx, affine->ry, affine->sy,
             affine->tx, affine->ty);
}

MagickExport DrawContext DrawAllocateContext(const ImageInfo *image_info,
                                             Image *image)
{
  DrawContext
    context;

  /* Allocate initial drawing context */
  context = (DrawContext) AcquireMemory(sizeof(struct _DrawContext));
  if(context == (DrawContext) NULL)
    MagickFatalError(ResourceLimitFatalError,
      "Unable to allocate initial drawing context","Memory allocation failed");

  /* Support structures */
  context->image_info = CloneImageInfo(image_info);
  context->image = image;

  /* MVG output string and housekeeping */
  context->mvg = NULL;
  context->mvg_alloc = 0;
  context->mvg_length = 0;

  /* Graphic context */
  context->index = 0;
  context->graphic_context=(DrawInfo **) AcquireMemory(sizeof(DrawInfo *));
  if(context->graphic_context == (DrawInfo **) NULL)
    MagickFatalError(ResourceLimitFatalError,"Unable to allocate drawing context",
      "Memory allocation failed");
  CurrentContext=CloneDrawInfo(image_info,(DrawInfo*)NULL);

  context->filter_off = False;

  /* Pretty-printing depth */
  context->indent_depth = 0;

  /* Path operation support */
  context->path_operation = PathDefaultOperation;
  context->path_mode = DefaultPathMode;

  /* Structure unique signature */
  context->signature = MagickSignature;

  return context;
}

MagickExport void DrawArc(DrawContext context,
                          const double sx, const double sy,
                          const double ex, const double ey,
                          const double sd, const double ed)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "arc %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
             sx, sy, ex, ey, sd, ed);
}

MagickExport void DrawBezier(DrawContext context, const size_t num_coords,
                             const PointInfo * coordinates)
{
  const PointInfo
    *coordinate;

  size_t i;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(coordinates != (const PointInfo *) NULL);

  MvgPrintf(context, "bezier");
  for (i = num_coords, coordinate = coordinates; i; i--)
    {
      MvgPrintf(context, " %.4g,%.4g", coordinate->x, coordinate->y);
      ++coordinate;
    }
  MvgPrintf(context, "\n");
}

MagickExport void DrawCircle(DrawContext context, const double ox,
                             const double oy, const double px, const double py)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "circle %.4g,%.4g %.4g,%.4g\n", ox, oy, px, py);
}

MagickExport void DrawSetClipPath(DrawContext context, const char *clip_path)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(clip_path != (const char *) NULL);

  if( CurrentContext->clip_path == NULL || context->filter_off ||
      LocaleCompare(CurrentContext->clip_path,clip_path) != 0)
    {
      CloneString(&CurrentContext->clip_path,clip_path);
#if 0
      (void) DrawClipPath(context->image,context->graphic_context[context->index,
                          CurrentContext->clip_path));
#endif
      MvgPrintf(context, "clip-path url(#%s)\n", clip_path);
    }
}

MagickExport void DrawSetClipRule(DrawContext context,
                                  const FillRule fill_rule)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->fill_rule != fill_rule))
    {
      CurrentContext->fill_rule = fill_rule;
      
      switch (fill_rule)
        {
        case EvenOddRule:
          p = "evenodd";
          break;
        case NonZeroRule:
          p = "nonzero";
          break;
        default:
          break;
        }
      
      if (p != NULL)
        MvgPrintf(context, "clip-rule %s\n", p);
    }
}

MagickExport void DrawSetClipUnits(DrawContext context,
                                   const ClipPathUnits clip_units)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->clip_units != clip_units))
    {
      CurrentContext->clip_units = clip_units;

      switch (clip_units)
        {
        case UserSpace:
          p = "userSpace";
          break;
        case UserSpaceOnUse:
          p = "userSpaceOnUse";
          break;
        case ObjectBoundingBox:
          p = "objectBoundingBox";
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "clip-units %s\n", p);
    }
}

MagickExport void DrawColor(DrawContext context,
                            const double x, const double y,
                            const PaintMethod paintMethod)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  switch (paintMethod)
    {
    case PointMethod:
      p = "point";
      break;
    case ReplaceMethod:
      p = "replace";
      break;
    case FloodfillMethod:
      p = "floodfill";
      break;
    case FillToBorderMethod:
      p = "filltoborder";
      break;
    case ResetMethod:
      p = "reset";
      break;
    }

  if (p != NULL)
    MvgPrintf(context, "color %s\n", p);
}

MagickExport void DrawComment(DrawContext context,const char* comment)
{
  MvgPrintf(context, "#%s\n", comment);
}

MagickExport void DrawDestroyContext(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  /* Path operation support */
  context->path_operation = PathDefaultOperation;
  context->path_mode = DefaultPathMode;

  /* Pretty-printing depth */
  context->indent_depth = 0;

  /* Graphic context */
  for ( ; context->index >= 0; context->index--)
    DestroyDrawInfo(CurrentContext);
  LiberateMemory((void **) &context->graphic_context);

  /* MVG output string and housekeeping */
  LiberateMemory((void **) &context->mvg);
  context->mvg_alloc = 0;
  context->mvg_length = 0;

  /* Support structures */
  DestroyImageInfo(context->image_info);
  context->image = (Image*)NULL;

  /* Context itself */
  context->signature = 0;
  LiberateMemory((void **) &context);
}

MagickExport void DrawEllipse(DrawContext context,
                              const double ox, const double oy,
                              const double rx, const double ry,
                              const double start, const double end)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "ellipse %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
             ox, oy, rx, ry, start, end);
}

MagickExport void DrawSetFillColor(DrawContext context,
                                   const PixelPacket * fill_color)
{
  PixelPacket
    *current_fill,
    new_fill;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(fill_color != (const PixelPacket *) NULL);

  new_fill = *fill_color;
  if(new_fill.opacity != TransparentOpacity)
    new_fill.opacity = CurrentContext->opacity;

  current_fill = &CurrentContext->fill;
  if( context->filter_off || !(PixelPacketMatch(current_fill,&new_fill)) )
    {
      CurrentContext->fill = new_fill;

      MvgPrintf(context, "fill ");
      MvgAppendColor(context, fill_color);
      MvgPrintf(context, "\n");
    }
}
MagickExport void DrawSetFillColorString(DrawContext context, const char* fill_color)
{
  PixelPacket
    pixel_packet;

  if(QueryColorDatabase(fill_color,&pixel_packet,&context->image->exception))
    DrawSetFillColor(context,&pixel_packet);
}
MagickExport void DrawSetFillPatternURL(DrawContext context, const char* fill_url)
{
  MvgPrintf(context, "fill url(%s)\n",fill_url);
}

MagickExport void DrawSetFillOpacity(DrawContext context,
                                     const double fill_opacity)
{
  Quantum
    opacity;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  opacity = (Quantum)(MaxRGB*(1.0-(fill_opacity <= 1.0 ? fill_opacity : 1.0 )));

  if (context->filter_off || (CurrentContext->opacity != opacity))
    {
      CurrentContext->opacity = opacity;
      MvgPrintf(context, "fill-opacity %.4g\n", fill_opacity);
    }
}

MagickExport void DrawSetFillRule(DrawContext context,
                                  const FillRule fill_rule)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->fill_rule != fill_rule))
    {
      CurrentContext->fill_rule = fill_rule;

      switch (fill_rule)
        {
        case EvenOddRule:
          p = "evenodd";
          break;
        case NonZeroRule:
          p = "nonzero";
          break;
        default:
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "fill-rule %s\n", p);
    }
}

MagickExport void DrawSetFont(DrawContext context, const char *font_name)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(font_name != (const char *) NULL);

  if(context->filter_off || (CurrentContext->font == NULL) ||
     LocaleCompare(CurrentContext->font,font_name) != 0)
    {
      (void) CloneString(&CurrentContext->font,font_name);
      MvgPrintf(context, "font '%s'\n", font_name);
    }
}

MagickExport void DrawSetFontFamily(DrawContext context,
                                    const char *font_family)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(font_family != (const char *) NULL);

  if(context->filter_off || (CurrentContext->family == NULL) ||
     LocaleCompare(CurrentContext->family,font_family) != 0)
    {
      (void) CloneString(&CurrentContext->family,font_family);

      MvgPrintf(context, "font-family %s\n", font_family);
    }
}

MagickExport void DrawSetFontSize(DrawContext context,
                                  const double font_pointsize)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->pointsize != font_pointsize))
    {
      CurrentContext->pointsize=font_pointsize;

      MvgPrintf(context, "font-size %.4g\n", font_pointsize);
    }
}

MagickExport void DrawSetFontStretch(DrawContext context,
                                     const StretchType font_stretch)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->stretch != font_stretch))
    {
      CurrentContext->stretch=font_stretch;

      switch (font_stretch)
        {
        case NormalStretch:
          p = "normal";
          break;
        case UltraCondensedStretch:
          p = "ultra-condensed";
          break;
        case ExtraCondensedStretch:
          p = "extra-condensed";
          break;
        case CondensedStretch:
          p = "condensed";
          break;
        case SemiCondensedStretch:
          p = "semi-condensed";
          break;
        case SemiExpandedStretch:
          p = "semi-expanded";
          break;
        case ExpandedStretch:
          p = "expanded";
          break;
        case ExtraExpandedStretch:
          p = "extra-expanded";
          break;
        case UltraExpandedStretch:
          p = "ultra-expanded";
          break;
        case AnyStretch:
          p = "all";
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "font-stretch %s\n", p);
    }
}

MagickExport void DrawSetFontStyle(DrawContext context,
                                   const StyleType font_style)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->style != font_style))
    {
      CurrentContext->style=font_style;

      switch (font_style)
        {
        case NormalStyle:
          p = "normal";
          break;
        case ItalicStyle:
          p = "italic";
          break;
        case ObliqueStyle:
          p = "oblique";
          break;
        case AnyStyle:
          p = "all";
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "font-style %s\n", p);
    }
}

MagickExport void DrawSetFontWeight(DrawContext context,
                                    const double font_weight)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->weight != font_weight))
    {
      CurrentContext->weight=font_weight;
      MvgPrintf(context, "font-weight %.4g\n", font_weight);
    }
}

MagickExport void DrawSetGravity(DrawContext context,
                                 const GravityType gravity)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->gravity != gravity))
    {
      CurrentContext->gravity=gravity;

      switch (gravity)
        {
        case NorthWestGravity:
          p = "NorthWest";
          break;
        case NorthGravity:
          p = "North";
          break;
        case NorthEastGravity:
          p = "NorthEast";
          break;
        case WestGravity:
          p = "West";
          break;
        case CenterGravity:
          p = "Center";
          break;
        case EastGravity:
          p = "East";
          break;
        case SouthWestGravity:
          p = "SouthWest";
          break;
        case SouthGravity:
          p = "South";
          break;
        case SouthEastGravity:
          p = "SouthEast";
          break;
        case StaticGravity:
        case ForgetGravity:
          {
          }
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "gravity %s\n", p);
    }
}

MagickExport void DrawComposite(DrawContext context,
                                const CompositeOperator composite_operator,
                                const double x, const double y,
                                const double width, const double height,
                                const Image * image )

{
  Image
    *clone_image;

  char
    *media_type = NULL,
    *base64 = NULL;

  const char
    *mode = NULL;

  unsigned char
    *blob = (unsigned char*)NULL;

  size_t
    blob_length = 2048;

  assert(context != (DrawContext)NULL);
  assert(image != (Image *) NULL);
  assert(width != 0);
  assert(height != 0);
  assert(*image->magick != '\0');

  clone_image = CloneImage(image,0,0,True,&context->image->exception);
  if(!clone_image)
    return;

  blob = (unsigned char*)ImageToBlob( context->image_info, clone_image, &blob_length,
                                      &context->image->exception );
  DestroyImageList(clone_image);
  if(!blob)
    return;

  base64 = Base64Encode(blob,blob_length);
  LiberateMemory((void**)&blob);
  if(!base64)
    {
      char
        buffer[MaxTextExtent];

      FormatString(buffer,"%d bytes", (4*blob_length/3+4));
      ThrowDrawException(ResourceLimitWarning, "allocating Base64 memory",buffer);
    }

  mode = "copy";
  switch (composite_operator)
    {
    case AddCompositeOp:
      mode = "add";
      break;
    case AtopCompositeOp:
      mode = "atop";
      break;
    case BumpmapCompositeOp:
      mode = "bumpmap";
      break;
    case ClearCompositeOp:
      mode = "clear";
      break;
    case ColorizeCompositeOp:
      mode = "colorize_not_supported";
      break;
    case CopyBlueCompositeOp:
      mode = "copyblue";
      break;
    case CopyCompositeOp:
      mode = "copy";
      break;
    case CopyGreenCompositeOp:
      mode = "copygreen";
      break;
    case CopyOpacityCompositeOp:
      mode = "copyopacity";
      break;
    case CopyRedCompositeOp:
      mode = "copyred";
      break;
    case DarkenCompositeOp:
      mode = "darken_not_supported";
      break;
    case DifferenceCompositeOp:
      mode = "difference";
      break;
    case DisplaceCompositeOp:
      mode = "displace_not_supported";
      break;
    case DissolveCompositeOp:
      mode = "dissolve_not_supported";
      break;
    case HueCompositeOp:
      mode = "hue_not_supported";
      break;
    case InCompositeOp:
      mode = "in";
      break;
    case LightenCompositeOp:
      mode = "lighten_not_supported";
      break;
    case LuminizeCompositeOp:
      mode = "luminize_not_supported";
      break;
    case MinusCompositeOp:
      mode = "minus";
      break;
    case ModulateCompositeOp:
      mode = "modulate_not_supported";
      break;
    case MultiplyCompositeOp:
      mode = "multiply";
      break;
    case NoCompositeOp:
      mode = "no_not_supported";
      break;
    case OutCompositeOp:
      mode = "out";
      break;
    case OverCompositeOp:
      mode = "over";
      break;
    case OverlayCompositeOp:
      mode = "overlay_not_supported";
      break;
    case PlusCompositeOp:
      mode = "plus";
      break;
    case SaturateCompositeOp:
      mode = "saturate_not_supported";
      break;
    case ScreenCompositeOp:
      mode = "screen_not_supported";
      break;
    case SubtractCompositeOp:
      mode = "subtract";
      break;
    case ThresholdCompositeOp:
      mode = "threshold";
      break;
    case XorCompositeOp:
      mode = "xor";
      break;
    default:
      break;
    }

  media_type = MagickToMime( clone_image->magick );

  if( media_type != NULL )
    {
      long
        remaining;

      char
        *str;

      MvgPrintf(context, "image %s %.4g,%.4g %.4g,%.4g 'data:%s;base64,\n",
                      mode, x, y, width, height, media_type);

      remaining = strlen(base64);
      for( str = base64; remaining > 0; )
        {
          MvgPrintf(context,"%.76s", str);
          remaining -= 76;
          str += 76;
          if(remaining > 0)
            MvgPrintf(context,"\n");
        }

      MvgPrintf(context,"'\n");
    }

  LiberateMemory((void**)&media_type);
}

MagickExport void DrawLine(DrawContext context,
                           const double sx, const double sy,
                           const double ex, const double ey)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "line %.4g,%.4g %.4g,%.4g\n", sx, sy, ex, ey);
}

MagickExport void DrawMatte(DrawContext context,
                            const double x, const double y,
                            const PaintMethod paint_method)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  switch (paint_method)
    {
    case PointMethod:
      p = "point";
      break;
    case ReplaceMethod:
      p = "replace";
      break;
    case FloodfillMethod:
      p = "floodfill";
      break;
    case FillToBorderMethod:
      p = "filltoborder";
      break;
    case ResetMethod:
      p = "reset";
      break;
    }

  if (p != NULL)
    MvgPrintf(context, "matte %.4g,%.4g %s\n", x, y, p);
}

MagickExport void DrawPathClose(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "%s", context->path_mode == AbsolutePathMode ? "Z" : "z");
}

static void DrawPathCurveTo(DrawContext context,
                            const PathMode mode,
                            const double x1, const double y1,
                            const double x2, const double y2,
                            const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathCurveToOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathCurveToOperation;
      context->path_mode = mode;
      MvgPrintf(context, "%c%.4g,%.4g %.4g,%.4g %.4g,%.4g",
                 mode == AbsolutePathMode ? 'C' : 'c', x1, y1, x2, y2, x, y);
    }
  else
    MvgPrintf(context, " %.4g,%.4g %.4g,%.4g %.4g,%.4g", x1, y1, x2, y2, x, y);
}

MagickExport void DrawPathCurveToAbsolute(DrawContext context,
                                          const double x1, const double y1,
                                          const double x2, const double y2,
                                          const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveTo(context, AbsolutePathMode, x1, y1, x2, y2, x, y);
}

MagickExport void DrawPathCurveToRelative(DrawContext context,
                                          const double x1, const double y1,
                                          const double x2, const double y2,
                                          const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveTo(context, RelativePathMode, x1, y1, x2, y2, x, y);
}

static void DrawPathCurveToQuadraticBezier(DrawContext context,
                                           const PathMode mode,
                                           const double x1, double y1,
                                           const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathCurveToQuadraticBezierOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathCurveToQuadraticBezierOperation;
      context->path_mode = mode;
      MvgPrintf(context, "%c%.4g,%.4g %.4g,%.4g",
                 mode == AbsolutePathMode ? 'Q' : 'q', x1, y1, x, y);
    }
  else
    MvgPrintf(context, " %.4g,%.4g %.4g,%.4g", x1, y1, x, y);
}

MagickExport void DrawPathCurveToQuadraticBezierAbsolute(DrawContext context,
                                                         const double x1,
                                                         const double y1,
                                                         const double x,
                                                         const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveToQuadraticBezier(context, AbsolutePathMode, x1, y1, x, y);
}

MagickExport void DrawPathCurveToQuadraticBezierRelative(DrawContext context,
                                                         const double x1,
                                                         const double y1,
                                                         const double x,
                                                         const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveToQuadraticBezier(context, RelativePathMode, x1, y1, x, y);
}

static void DrawPathCurveToQuadraticBezierSmooth(DrawContext context,
                                                 const PathMode mode,
                                                 const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathCurveToQuadraticBezierSmoothOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathCurveToQuadraticBezierSmoothOperation;
      context->path_mode = mode;
      MvgPrintf(context, "%c%.4g,%.4g", mode == AbsolutePathMode ? 'T' : 't', x,
                    y);
    }
  else
    MvgPrintf(context, " %.4g,%.4g", x, y);
}

MagickExport void DrawPathCurveToQuadraticBezierSmoothAbsolute(DrawContext
                                                               context,
                                                               const double x,
                                                               const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveToQuadraticBezierSmooth(context, AbsolutePathMode, x, y);
}

MagickExport void DrawPathCurveToQuadraticBezierSmoothRelative(DrawContext
                                                               context,
                                                               const double x,
                                                               const double y)
{
  DrawPathCurveToQuadraticBezierSmooth(context, RelativePathMode, x, y);
}

static void DrawPathCurveToSmooth(DrawContext context, const PathMode mode,
                                  const double x2, const double y2,
                                  const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathCurveToSmoothOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathCurveToSmoothOperation;
      context->path_mode = mode;
      MvgPrintf(context, "%c%.4g,%.4g %.4g,%.4g",
                    mode == AbsolutePathMode ? 'S' : 's', x2, y2, x, y);
    }
  else
    MvgPrintf(context, " %.4g,%.4g %.4g,%.4g", x2, y2, x, y);
}

MagickExport void DrawPathCurveToSmoothAbsolute(DrawContext context,
                                                const double x2, const double y2,
                                                const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveToSmooth(context, AbsolutePathMode, x2, y2, x, y);
}

MagickExport void DrawPathCurveToSmoothRelative(DrawContext context,
                                                const double x2, const double y2,
                                                const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveToSmooth(context, RelativePathMode, x2, y2, x, y);
}

static void DrawPathEllipticArc(DrawContext context, const PathMode mode,
                                const double rx, const double ry,
                                const double x_axis_rotation,
                                unsigned int large_arc_flag,
                                unsigned int sweep_flag,
                                const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathEllipticArcOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathEllipticArcOperation;
      context->path_mode = mode;
      MvgPrintf(context, "%c%.4g,%.4g %.4g %u %u %.4g,%.4g",
                 mode == AbsolutePathMode ? 'A' : 'a', rx, ry, x_axis_rotation,
                 large_arc_flag, sweep_flag, x, y);
    }
  else
    MvgPrintf(context, " %.4g,%.4g %.4g %u %u %.4g,%.4g", rx, ry,
               x_axis_rotation, large_arc_flag, sweep_flag, x, y);
}

MagickExport void DrawPathEllipticArcAbsolute(DrawContext context,
                                              const double rx, const double ry,
                                              const double x_axis_rotation,
                                              unsigned int large_arc_flag,
                                              unsigned int sweep_flag,
                                              const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathEllipticArc(context, AbsolutePathMode, rx, ry, x_axis_rotation,
                         large_arc_flag, sweep_flag, x, y);
}

MagickExport void DrawPathEllipticArcRelative(DrawContext context,
                                              const double rx, const double ry,
                                              const double x_axis_rotation,
                                              unsigned int large_arc_flag,
                                              unsigned int sweep_flag,
                                              const double x, const double y)
{
  DrawPathEllipticArc(context, RelativePathMode, rx, ry, x_axis_rotation,
                         large_arc_flag, sweep_flag, x, y);
}

MagickExport void DrawPathFinish(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "\"\n");
  context->path_operation = PathDefaultOperation;
  context->path_mode = DefaultPathMode;
}

static void DrawPathLineTo(DrawContext context,
                           const PathMode mode,
                           const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathLineToOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathLineToOperation;
      context->path_mode = mode;
      MvgPrintf(context, "%c%.4g,%.4g", mode == AbsolutePathMode ? 'L' : 'l', x,
                 y);
    }
  else
    MvgPrintf(context, " %.4g,%.4g", x, y);
}

MagickExport void DrawPathLineToAbsolute(DrawContext context,
                                         const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathLineTo(context, AbsolutePathMode, x, y);
}

MagickExport void DrawPathLineToRelative(DrawContext context,
                                         const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathLineTo(context, RelativePathMode, x, y);
}

static void DrawPathLineToHorizontal(DrawContext context,
                                     const PathMode mode, const double x)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathLineToHorizontalOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathLineToHorizontalOperation;
      context->path_mode = mode;
      MvgPrintf(context, "%c%.4g", mode == AbsolutePathMode ? 'H' : 'h', x);
    }
  else
    MvgPrintf(context, " %.4g", x);
}

MagickExport void DrawPathLineToHorizontalAbsolute(DrawContext context,
                                                   const double x)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathLineToHorizontal(context, AbsolutePathMode, x);
}

MagickExport void DrawPathLineToHorizontalRelative(DrawContext context,
                                                   const double x)
{
  DrawPathLineToHorizontal(context, RelativePathMode, x);
}

static void DrawPathLineToVertical(DrawContext context, const PathMode mode,
                                   const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathLineToVerticalOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathLineToVerticalOperation;
      context->path_mode = mode;
      MvgPrintf(context, "%c%.4g", mode == AbsolutePathMode ? 'V' : 'v', y);
    }
  else
    MvgPrintf(context, " %.4g", y);
}

MagickExport void DrawPathLineToVerticalAbsolute(DrawContext context,
                                                 const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathLineToVertical(context, AbsolutePathMode, y);
}

MagickExport void DrawPathLineToVerticalRelative(DrawContext context,
                                                 const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathLineToVertical(context, RelativePathMode, y);
}

static void DrawPathMoveTo(DrawContext context, const PathMode mode,
                           const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathMoveToOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathMoveToOperation;
      context->path_mode = mode;
      MvgPrintf(context, "%c%.4g,%.4g", mode == AbsolutePathMode ? 'M' : 'm', x,
                 y);
    }
  else
    MvgPrintf(context, " %.4g,%.4g", x, y);
}

MagickExport void DrawPathMoveToAbsolute(DrawContext context, const double x,
                                         const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathMoveTo(context, AbsolutePathMode, x, y);
}
MagickExport void DrawPathMoveToRelative(DrawContext context,
                                         const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathMoveTo(context, RelativePathMode, x, y);
}

MagickExport void DrawPathStart(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "path d=\"");
  context->path_operation = PathDefaultOperation;
  context->path_mode = DefaultPathMode;
}

MagickExport void DrawPoint(DrawContext context,
                            const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "point %.4g,%.4g\n", x, y);
}

MagickExport void DrawPolygon(DrawContext context,
                              const size_t num_coords,
                              const PointInfo * coordinates)
{
  const PointInfo
    *coordinate;

  size_t
    i;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "polygon");
  for (i = num_coords, coordinate = coordinates; i; i--)
    {
      MvgPrintf(context, " %.4g,%.4g", coordinate->x, coordinate->y);
      ++coordinate;
    }
  MvgPrintf(context, "\n");
}

MagickExport void DrawPolyline(DrawContext context,
                               const size_t num_coords,
                               const PointInfo * coordinates)
{
  const PointInfo
    *coordinate;

  size_t
    i;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "polyline");
  for (i = num_coords, coordinate = coordinates; i; i--)
    {
      MvgPrintf(context, " %.4g,%.4g", coordinate->x, coordinate->y);
      ++coordinate;
    }
  MvgPrintf(context, "\n");
}

MagickExport void DrawPopClipPath(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  context->indent_depth--;
  MvgPrintf(context, "pop clip-path\n");
}

MagickExport void DrawPopDefs(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  context->indent_depth--;
  MvgPrintf(context, "pop defs\n");
}

MagickExport void DrawPopGraphicContext(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->index > 0)
    {

#if 0
      /* Destroy clip path if not same in preceding context */
      if (CurrentContext->clip_path != (char *) NULL)
        if (LocaleCompare(CurrentContext->clip_path,
                          context->graphic_context[context->index-1]->clip_path) != 0)
          (void) SetImageClipMask(image,(Image *) NULL);
#endif
      DestroyDrawInfo(CurrentContext);
      context->index--;
      
      context->indent_depth--;
      MvgPrintf(context, "pop graphic-context\n");
    }
}

MagickExport void DrawPopPattern(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  context->filter_off = False;

  context->indent_depth--;
  MvgPrintf(context, "pop pattern\n");
}

MagickExport void DrawPushClipPath(DrawContext context,
                                   const char *clip_path_id)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(clip_path_id != (const char *) NULL);

  MvgPrintf(context, "push clip-path %s\n", clip_path_id);
  context->indent_depth++;
}

MagickExport void DrawPushDefs(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "push defs\n");
  context->indent_depth++;
}

MagickExport void DrawPushGraphicContext(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  context->index++;
  ReacquireMemory((void **) &context->graphic_context,
                  (context->index+1)*sizeof(DrawInfo *));
  if (context->graphic_context == (DrawInfo **) NULL)
    {
      /* FIXME error */
    }
  CurrentContext=
    CloneDrawInfo((ImageInfo *) NULL,context->graphic_context[context->index-1]);

  MvgPrintf(context, "push graphic-context\n");
  context->indent_depth++;
}

MagickExport void DrawPushPattern(DrawContext context,
                                  const char *pattern_id,
                                  const double x, const double y,
                                  const double width, const double height)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(pattern_id != (const char *) NULL);

  context->filter_off = True;

  MvgPrintf(context, "push pattern %s %.4g,%.4g %.4g,%.4g\n",
    pattern_id, x, y, width, height);
  context->indent_depth++;
}

MagickExport void DrawRectangle(DrawContext context,
                                const double x1, const double y1,
                                const double x2, const double y2)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "rectangle %.4g,%.4g %.4g,%.4g\n", x1, y1, x2, y2);
}

MagickExport int DrawRender(const DrawContext context)
{
  DrawInfo
    *draw_info;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  draw_info = (DrawInfo *) AcquireMemory(sizeof(DrawInfo));
  GetDrawInfo(context->image_info, draw_info);
  draw_info->primitive = context->mvg;
  /* puts(draw_info->primitive); */
  DrawImage(context->image, draw_info);
  draw_info->primitive = (char *) NULL;
  DestroyDrawInfo(draw_info);
  return True;
}

MagickExport void DrawSetRotate(DrawContext context, const double degrees)
{
#if 0
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.sx=cos(DegreesToRadians(fmod(degrees,360.0)));
  affine.rx=sin(DegreesToRadians(fmod(degrees,360.0)));
  affine.ry=(-sin(DegreesToRadians(fmod(degrees,360.0))));
  affine.sy=cos(DegreesToRadians(fmod(degrees,360.0)));
  DrawSetAffine(context,&affine);
#else
  MvgPrintf(context, "rotate %.4g\n", degrees);
#endif
}

MagickExport void DrawRoundRectangle(DrawContext context,
                                     double x1, double y1,
                                     double x2, double y2,
                                     double rx, double ry)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "roundrectangle %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
                x1, y1, x2, y2, rx, ry);
}

MagickExport void DrawSetScale(DrawContext context,
                               const double x, const double y)
{
#if 0
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.sx=x;
  affine.sy=y;
  DrawSetAffine(context,&affine);
#else
  MvgPrintf(context, "scale %.4g,%.4g\n", x, y);
#endif
}

MagickExport void DrawSetSkewX(DrawContext context, const double degrees)
{
#if 0
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.ry=tan(DegreesToRadians(fmod(degrees,360.0)));
  DrawSetAffine(context,&affine);
#else
  MvgPrintf(context, "skewX %.4g\n", degrees);
#endif
}

MagickExport void DrawSetSkewY(DrawContext context, const double degrees)
{
#if 0
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.rx=tan(DegreesToRadians(fmod(degrees,360.0)));
  DrawSetAffine(context,&affine);
#else
  MvgPrintf(context, "skewY %.4g\n", degrees);
#endif
}

MagickExport void DrawSetStopColor(DrawContext context,
                                   const PixelPacket * stop_color,
                                   const double offset)
{

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(stop_color != (const PixelPacket *) NULL);

  MvgPrintf(context, "stop-color ");
  MvgAppendColor(context, stop_color);
  MvgPrintf(context, "\n");
}

MagickExport void DrawSetStrokeColor(DrawContext context,
                                     const PixelPacket * stroke_color)
{
  PixelPacket
    *current_stroke,
    new_stroke;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(stroke_color != (const PixelPacket *) NULL);

  new_stroke = *stroke_color;
  if(new_stroke.opacity != TransparentOpacity)
    new_stroke.opacity = CurrentContext->opacity;

  current_stroke = &CurrentContext->stroke;
  if( context->filter_off || !(PixelPacketMatch(current_stroke,&new_stroke)) )
    {
      CurrentContext->stroke = new_stroke;

      MvgPrintf(context, "stroke ");
      MvgAppendColor(context, stroke_color);
      MvgPrintf(context, "\n");
    }
}
MagickExport void DrawSetStrokeColorString(DrawContext context, const char* stroke_color)
{
  PixelPacket
    pixel_packet;

  if(QueryColorDatabase(stroke_color,&pixel_packet,&context->image->exception))
    DrawSetStrokeColor(context,&pixel_packet);
}
MagickExport void DrawSetStrokePatternURL(DrawContext context, const char* stroke_url)
{
  MvgPrintf(context, "stroke url(%s)\n",stroke_url);
}

MagickExport void DrawSetStrokeAntialias(DrawContext context,
                                         const int stroke_antialias)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->stroke_antialias != stroke_antialias))
    {
      CurrentContext->stroke_antialias = stroke_antialias;

      MvgPrintf(context, "stroke-antialias %i\n", stroke_antialias ? 1 : 0);
    }
}

MagickExport void DrawSetStrokeDashArray(DrawContext context,
                                         const double *dasharray)
{
  register const double
    *p;

  register double
    *q;

  unsigned int
    updated = False,
    n_new = 0,
    n_old = 0;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  p = dasharray;
  if( p != (const double *) NULL )
    while( *p++ != 0)
      n_new++;

  q = CurrentContext->dash_pattern;
  if( q != (const double *) NULL )
    while( *q++ != 0)
      n_old++;

  if( (n_old == 0) && (n_new == 0) )
    {
      updated = False;
    }
  else if( n_old != n_new )
    {
      updated = True;
    }
  else if((CurrentContext->dash_pattern != (double*)NULL)
          && (dasharray != (double*)NULL))
    {
      p = dasharray;
      q = CurrentContext->dash_pattern;
      while( *p && *q )
        if( *p != *q )
          {
            updated = True;
            break;
          }
    }

  if( context->filter_off || updated )
    {
      if(CurrentContext->dash_pattern != (double*)NULL)
        LiberateMemory((void **) &CurrentContext->dash_pattern);

      if( n_new != 0)
        {
          CurrentContext->dash_pattern = (double *)
            AcquireMemory((n_new+1)*sizeof(double));
          if(CurrentContext->dash_pattern == (double*)NULL)
            {
              ThrowDrawException(ResourceLimitError, "Unable to draw image",
                                 "Memory allocation failed");
            }
          else
            {
              q=CurrentContext->dash_pattern;
              p=dasharray;
              while( *p )
                *q++=*p++;
              *q=0;
            }
        }

      MvgPrintf(context, "stroke-dasharray ");
      if ( n_new == 0 )
        MvgPrintf(context, "none");
      else
        {
          p = dasharray;
          MvgPrintf(context, "%.4g", *p++);
          while (*p)
            MvgPrintf(context, ",%.4g", *p++);
        }
      MvgPrintf(context, "\n");
    }
}

MagickExport void DrawSetStrokeDashOffset(DrawContext context,
                                          const unsigned int dash_offset)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->dash_offset != dash_offset))
    {
      CurrentContext->dash_offset = dash_offset;

      MvgPrintf(context, "stroke-dashoffset %u\n", dash_offset);
    }
}

MagickExport void DrawSetStrokeLineCap(DrawContext context,
                                       const LineCap linecap)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->linecap != linecap))
    {
      const char
        *p = NULL;

      CurrentContext->linecap = linecap;

      switch (linecap)
        {
        case ButtCap:
          p = "butt";
          break;
        case RoundCap:
          p = "round";
          break;
        case SquareCap:
          p = "square";
          break;
        default:
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "stroke-linecap %s\n", p);
    }
}

MagickExport void DrawSetStrokeLineJoin(DrawContext context,
                                        const LineJoin linejoin)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->linejoin != linejoin))
    {
      const char
        *p = NULL;

      CurrentContext->linejoin = linejoin;

      switch (linejoin)
        {
        case MiterJoin:
          p = "miter";
          break;
        case RoundJoin:
          p = "round";
          break;
        case BevelJoin:
          p = "square";
          break;
        default:
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "stroke-linejoin %s\n", p);
    }
}

MagickExport void DrawSetStrokeMiterLimit(DrawContext context,
                                          const unsigned long miterlimit)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(CurrentContext->miterlimit != miterlimit)
    {
      CurrentContext->miterlimit = miterlimit;

      MvgPrintf(context, "stroke-miterlimit %u\n", miterlimit);
    }
}

MagickExport void DrawSetStrokeOpacity(DrawContext context,
                                       const double stroke_opacity)
{
  double
    opacity;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  opacity = (Quantum)(MaxRGB*(1.0-(stroke_opacity <= 1.0 ? stroke_opacity : 1.0 )));

  if (context->filter_off || (CurrentContext->stroke.opacity != opacity))
    {
      CurrentContext->stroke.opacity = opacity;
      MvgPrintf(context, "stroke-opacity %.4g\n", stroke_opacity);
    }
}

MagickExport void DrawSetStrokeWidth(DrawContext context, const double stroke_width)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (context->filter_off || (CurrentContext->stroke_width != stroke_width))
    {
      CurrentContext->stroke_width = stroke_width;

      MvgPrintf(context, "stroke-width %.4g\n", stroke_width);
    }
}

MagickExport void DrawSetTextAntialias(DrawContext context,
                                       const int text_antialias)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (context->filter_off || (CurrentContext->text_antialias != text_antialias))
    {
      CurrentContext->text_antialias = text_antialias;

      MvgPrintf(context, "text-antialias %i\n", text_antialias ? 1 : 0);
    }
}

MagickExport void DrawSetTextDecoration(DrawContext context,
                                        const DecorationType decoration)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (context->filter_off || (CurrentContext->decorate != decoration))
    {
      CurrentContext->decorate = decoration;

      switch (decoration)
        {
        case NoDecoration:
          p = "none";
          break;
        case UnderlineDecoration:
          p = "underline";
          break;
        case OverlineDecoration:
          p = "overline";
          break;
        case LineThroughDecoration:
          p = "line-through";
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "decorate %s\n", p);
    }
}

MagickExport void DrawSetTextUnderColor(DrawContext context,
                                        const PixelPacket * under_color)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(under_color != (const PixelPacket *)NULL);

  if (context->filter_off || !(PixelPacketMatch(&CurrentContext->box, under_color)))
    {
      CurrentContext->box = *under_color;
#if 0
      MvgPrintf(context, "decorate ");
      MvgAppendColor(context, under_color);
      MvgPrintf(context, "\n");
#endif
    }
}

MagickExport void DrawSetTextUnderColorString(DrawContext context, const char* under_color)
{
  PixelPacket
    pixel_packet;

  if(QueryColorDatabase(under_color,&pixel_packet,&context->image->exception))
    DrawSetTextUnderColor(context,&pixel_packet);  
}

MagickExport void DrawSetTranslate(DrawContext context,
                                   const double x, const double y)
{
#if 0
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.tx=x;
  affine.ty=y;
  DrawSetAffine(context,&affine);
#else
  MvgPrintf(context, "translate %.4g,%.4g\n", x, y);
#endif
}

MagickExport void DrawSetViewbox(DrawContext context,
                                 unsigned long x1, unsigned long y1,
                                 unsigned long x2, unsigned long y2)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "viewbox %lu %lu %lu %lu\n", x1, y1, x2, y2);
}
