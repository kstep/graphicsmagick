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

  ExceptionInfo
    exception;

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

  /* Pretty-printing depth */
  long
    indention_depth;

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
  DrawPrintf(DrawContext context, const char *format, ...);

MagickExport void DrawAnnotation(DrawContext context, const double x,
				 const double y, const char *text)
{
  char escaped_string[MaxTextExtent];

  unsigned char *p,
    *q;

  int string_length;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(text != (const char *) NULL);

  for (p = (unsigned char *) text, q =
         (unsigned char *) escaped_string, string_length = 0;
       *p != 0 && string_length < ((int) sizeof(escaped_string) - 3); ++p)
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

  DrawPrintf(context, "text %.4g,%.4g '%.1024s'\n", escaped_string);
}

MagickExport void DrawSetAffine(DrawContext context, const AffineMatrix *affine)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((affine->sx != 1.0) || (affine->rx != 0.0) || (affine->ry != 0.0) ||
      (affine->sy != 1.0) || (affine->tx != 0.0) || (affine->ty != 0.0))
    {
      AffineMatrix
        current;

      current = context->graphic_context[context->index]->affine;

      context->graphic_context[context->index]->affine.sx=
        current.sx*affine->sx+current.ry*affine->rx;
      context->graphic_context[context->index]->affine.rx=
        current.rx*affine->sx+current.sy*affine->rx;
      context->graphic_context[context->index]->affine.ry=
        current.sx*affine->ry+current.ry*affine->sy;
      context->graphic_context[context->index]->affine.sy=
        current.rx*affine->ry+current.sy*affine->sy;
      context->graphic_context[context->index]->affine.tx=
        current.sx*affine->tx+current.ry*affine->ty+current.tx;
      context->graphic_context[context->index]->affine.ty=
        current.rx*affine->tx+current.sy*affine->ty+current.ty;
    }

  DrawPrintf(context, "affine %.4g,%.4g,%.4g,%.4g,%.4g,%.4g\n",
             affine->sy, affine->rx, affine->ry, affine->sy,
             affine->tx, affine->ty);
}

MagickExport DrawContext DrawAllocateContext(void)
{
  DrawContext
    context;

  /* Allocate initial drawing context */
  context = (DrawContext) AcquireMemory(sizeof(struct _DrawContext));
  if(context == (DrawContext) NULL)
    MagickFatalError(ResourceLimitFatalError,
      "Unable to allocate initial drawing context","Memory allocation failed");

  /* Support structures */
  GetExceptionInfo( &context->exception );
  context->image_info = CloneImageInfo((const ImageInfo *)NULL);

  /* MVG output string and housekeeping */
  context->mvg = NULL;
  context->mvg_alloc = 0;
  context->mvg_length = 0;

  /* Graphic context */
  context->index = 0;
  context->graphic_context=(DrawInfo **) AcquireMemory(sizeof(DrawInfo *));
  if(context->graphic_context == (DrawInfo **) NULL)
    MagickFatalError(ResourceLimitFatalError,"Unable to allocate draw info",
      "Memory allocation failed");
  context->graphic_context[context->index]=(DrawInfo *) AcquireMemory(sizeof(DrawInfo));
  if(context->graphic_context[context->index]==(DrawInfo *) NULL)
    MagickFatalError(ResourceLimitFatalError,"Unable to allocatedraw info",
      "Memory allocation failed");
  GetDrawInfo((const ImageInfo *)NULL,context->graphic_context[context->index]);

  /* Pretty-printing depth */
  context->indention_depth = 0;

  /* Path operation support */
  context->path_operation = PathDefaultOperation;
  context->path_mode = DefaultPathMode;

  /* Structure unique signature */
  context->signature = MagickSignature;

  return context;
}

MagickExport void DrawArc(DrawContext context, const double sx,
                          const double sy, const double ex, const double ey,
                          const double sd, const double ed)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPrintf(context, "arc %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
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

  DrawPrintf(context, "bezier");
  for (i = num_coords, coordinate = coordinates; i; i--)
    {
      DrawPrintf(context, " %.4g,%.4g", coordinate->x, coordinate->y);
      ++coordinate;
    }
  DrawPrintf(context, "\n");
}

MagickExport void DrawCircle(DrawContext context, const double ox,
                             const double oy, const double px, const double py)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPrintf(context, "circle %.4g,%.4g %.4g,%.4g\n", ox, oy, px, py);
}

MagickExport void DrawSetClipPath(DrawContext context, const char *clip_path)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(clip_path != (const char *) NULL);

  if( context->graphic_context[context->index]->clip_path == NULL ||
      LocaleCompare(context->graphic_context[context->index]->clip_path,
                    clip_path) != 0)
    {
      CloneString(&context->graphic_context[context->index]->clip_path,clip_path);
#if 0
      (void) DrawClipPath(context->image,context->graphic_context[context->index,
                          context->graphic_context[context->index]->clip_path));
#endif
      DrawPrintf(context, "clip-path url(%s)\n", clip_path);
    }
}

MagickExport void DrawSetClipRule(DrawContext context,
                                  const FillRule fill_rule)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->graphic_context[context->index]->fill_rule != fill_rule)
    {
      context->graphic_context[context->index]->fill_rule = fill_rule;
      
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
        DrawPrintf(context, "clip-rule %s\n", p);
    }
}

MagickExport void DrawSetClipUnits(DrawContext context,
                                   const ClipPathUnits clip_units)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->graphic_context[context->index]->clip_units != clip_units)
    {
      context->graphic_context[context->index]->clip_units = clip_units;

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
        DrawPrintf(context, "clip-units %s\n", p);
    }
}

MagickExport void DrawColor(DrawContext context, const double x,
                            const double y, const PaintMethod paintMethod)
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
    DrawPrintf(context, "color %s\n", p);
}

MagickExport void DrawDestroyContext(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  /* Path operation support */
  context->path_operation = PathDefaultOperation;
  context->path_mode = DefaultPathMode;

  /* Pretty-printing depth */
  context->indention_depth = 0;

  /* Graphic context */
  for ( ; context->index >= 0; context->index--)
    DestroyDrawInfo(context->graphic_context[context->index]);
  LiberateMemory((void **) &context->graphic_context);

  /* MVG output string and housekeeping */
  LiberateMemory((void **) &context->mvg);
  context->mvg_alloc = 0;
  context->mvg_length = 0;

  /* Support structures */
  DestroyImageInfo(context->image_info);
  DestroyExceptionInfo(&context->exception);

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

  DrawPrintf(context, "ellipse %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
             ox, oy, rx, ry, start, end);
}

MagickExport void DrawSetFill(DrawContext context,
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
    new_fill.opacity = context->graphic_context[context->index]->opacity;

  current_fill = &context->graphic_context[context->index]->fill;
  if( !(PixelPacketMatch(current_fill,&new_fill)) )
    {
      context->graphic_context[context->index]->fill = new_fill;

      if (fill_color->opacity == OpaqueOpacity)
        DrawPrintf(context,
#if QuantumDepth == 8
                   "fill #%02x%02x%02x\n",
#elif QuantumDepth == 16
                   "fill #%04x%04x%04x\n",
#endif
                   fill_color->red, fill_color->green, fill_color->blue);
      else
        DrawPrintf(context,
#if QuantumDepth == 8
                   "fill #%02x%02x%02x%02x\n",
#elif QuantumDepth == 16
                   "fill #%04x%04x%04x%04x\n",
#endif
                   fill_color->red, fill_color->green, fill_color->blue,
                   fill_color->opacity);
    }
}
MagickExport void DrawSetFillString(DrawContext context, const char* fill_color)
{
  PixelPacket
    pixel_packet;

  if(QueryColorDatabase(fill_color,&pixel_packet,&context->exception))
    DrawSetFill(context,&pixel_packet);
}


MagickExport void DrawSetFillOpacity(DrawContext context,
                                     const double fill_opacity)
{
  Quantum
    opacity;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  opacity = (Quantum)(MaxRGB*(1.0-(fill_opacity <= 1.0 ? fill_opacity : 1.0 )));

  if (context->graphic_context[context->index]->opacity != opacity)
    {
      context->graphic_context[context->index]->opacity = opacity;
      DrawPrintf(context, "fill-opacity %.4g\n", fill_opacity);
    }
}

MagickExport void DrawSetFillRule(DrawContext context,
                                  const FillRule fill_rule)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->graphic_context[context->index]->fill_rule != fill_rule)
    {
      context->graphic_context[context->index]->fill_rule = fill_rule;

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
        DrawPrintf(context, "fill-rule %s\n", p);
    }
}

MagickExport void DrawSetFont(DrawContext context, const char *font_name)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(font_name != (const char *) NULL);

  if(context->graphic_context[context->index]->font == NULL ||
     LocaleCompare(context->graphic_context[context->index]->font,font_name) != 0)
    {
      (void) CloneString(&context->graphic_context[context->index]->font,font_name);
      DrawPrintf(context, "font %s\n", font_name);
    }
}

MagickExport void DrawSetFontFamily(DrawContext context,
                                    const char *font_family)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(font_family != (const char *) NULL);

  if(context->graphic_context[context->index]->family == NULL ||
     LocaleCompare(context->graphic_context[context->index]->family,font_family) != 0)
    {
      (void) CloneString(&context->graphic_context[context->index]->family,font_family);

      DrawPrintf(context, "font-family %s\n", font_family);
    }
}

MagickExport void DrawSetFontSize(DrawContext context,
                                  const double font_pointsize)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->graphic_context[context->index]->pointsize != font_pointsize)
    {
      context->graphic_context[context->index]->pointsize=font_pointsize;

      DrawPrintf(context, "font-size %.4g\n", font_pointsize);
    }
}

MagickExport void DrawSetFontStretch(DrawContext context,
                                     const StretchType font_stretch)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->graphic_context[context->index]->stretch != font_stretch)
    {
      context->graphic_context[context->index]->stretch=font_stretch;

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
        DrawPrintf(context, "font-stretch %s\n", p);
    }
}

MagickExport void DrawSetFontStyle(DrawContext context,
                                   const StyleType font_style)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->graphic_context[context->index]->style != font_style)
    {
      context->graphic_context[context->index]->style=font_style;

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
        DrawPrintf(context, "font-style %s\n", p);
    }
}

MagickExport void DrawSetFontWeight(DrawContext context,
                                    const double font_weight)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->graphic_context[context->index]->weight != font_weight)
    {
      context->graphic_context[context->index]->weight=font_weight;
      DrawPrintf(context, "font-weight %.4g\n", font_weight);
    }
}

MagickExport void DrawSetGravity(DrawContext context,
                                 const GravityType gravity)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->graphic_context[context->index]->gravity != gravity)
    {
      context->graphic_context[context->index]->gravity=gravity;

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
        DrawPrintf(context, "gravity %s\n", p);
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

  clone_image = CloneImage(image,0,0,True,&context->exception);
  if(!clone_image)
    return;

  blob = (unsigned char*)ImageToBlob( context->image_info, clone_image, &blob_length,
                                      &context->exception );
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
      ThrowException(&context->exception,ResourceLimitWarning,
                     "allocating Base64 memory",buffer);
      return;
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

      DrawPrintf(context, "image %s %.4g,%.4g %.4g,%.4g 'data:%s;base64,\n",
                      mode, x, y, width, height, media_type);

      remaining = strlen(base64);
      for( str = base64; remaining > 0; )
        {
          DrawPrintf(context,"%.76s", str);
          remaining -= 76;
          str += 76;
          if(remaining > 0)
            DrawPrintf(context,"\n");
        }

      DrawPrintf(context,"'\n");
    }

  LiberateMemory((void**)&media_type);
}

MagickExport void DrawLine(DrawContext context,
                           const double sx, const double sy,
                           const double ex, const double ey)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPrintf(context, "line %.4g,%.4g %.4g,%.4g\n", sx, sy, ex, ey);
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
    DrawPrintf(context, "matte %.4g,%.4g %s\n", x, y, p);
}

MagickExport void DrawPathClose(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPrintf(context, "%s", context->path_mode == AbsolutePathMode ? "Z" : "z");
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
      DrawPrintf(context, "%c%.4g,%.4g %.4g,%.4g %.4g,%.4g",
                 mode == AbsolutePathMode ? 'C' : 'c', x1, y1, x2, y2, x, y);
    }
  else
    DrawPrintf(context, " %.4g,%.4g %.4g,%.4g %.4g,%.4g", x1, y1, x2, y2, x,
               y);
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
      DrawPrintf(context, "%c%.4g,%.4g %.4g,%.4g",
                 mode == AbsolutePathMode ? 'Q' : 'q', x1, y1, x, y);
    }
  else
    DrawPrintf(context, " %.4g,%.4g %.4g,%.4g", x1, y1, x, y);
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
      DrawPrintf(context, "%c%.4g,%.4g", mode == AbsolutePathMode ? 'T' : 't', x,
                    y);
    }
  else
    DrawPrintf(context, " %.4g,%.4g", x, y);
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
      DrawPrintf(context, "%c%.4g,%.4g %.4g,%.4g",
                    mode == AbsolutePathMode ? 'S' : 's', x2, y2, x, y);
    }
  else
    DrawPrintf(context, " %.4g,%.4g %.4g,%.4g", x2, y2, x, y);
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
      DrawPrintf(context, "%c%.4g,%.4g %.4g %u %u %.4g,%.4g",
                 mode == AbsolutePathMode ? 'A' : 'a', rx, ry, x_axis_rotation,
                 large_arc_flag, sweep_flag, x, y);
    }
  else
    DrawPrintf(context, " %.4g,%.4g %.4g %u %u %.4g,%.4g", rx, ry,
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

  DrawPrintf(context, "\"\n");
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
      DrawPrintf(context, "%c%.4g,%.4g", mode == AbsolutePathMode ? 'L' : 'l', x,
                 y);
    }
  else
    DrawPrintf(context, " %.4g,%.4g", x, y);
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
      DrawPrintf(context, "%c%.4g", mode == AbsolutePathMode ? 'H' : 'h', x);
    }
  else
    DrawPrintf(context, " %.4g", x);
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
      DrawPrintf(context, "%c%.4g", mode == AbsolutePathMode ? 'V' : 'v', y);
    }
  else
    DrawPrintf(context, " %.4g", y);
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
      DrawPrintf(context, "%c%.4g,%.4g", mode == AbsolutePathMode ? 'M' : 'm', x,
                 y);
    }
  else
    DrawPrintf(context, " %.4g,%.4g", x, y);
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

  DrawPrintf(context, "path d=\"");
  context->path_operation = PathDefaultOperation;
  context->path_mode = DefaultPathMode;
}

MagickExport void DrawPoint(DrawContext context,
                            const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPrintf(context, "point %.4g,%.4g\n", x, y);
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

  DrawPrintf(context, "polygon");
  for (i = num_coords, coordinate = coordinates; i; i--)
    {
      DrawPrintf(context, " %.4g,%.4g", coordinate->x, coordinate->y);
      ++coordinate;
    }
  DrawPrintf(context, "\n");
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

  DrawPrintf(context, "polyline");
  for (i = num_coords, coordinate = coordinates; i; i--)
    {
      DrawPrintf(context, " %.4g,%.4g", coordinate->x, coordinate->y);
      ++coordinate;
    }
  DrawPrintf(context, "\n");
}

MagickExport void DrawPopClipPath(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  context->indention_depth--;
  DrawPrintf(context, "pop clip-path\n");
}

MagickExport void DrawPopDefs(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  context->indention_depth--;
  DrawPrintf(context, "pop defs\n");
}

MagickExport void DrawPopGraphicContext(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->index > 0)
    {

#if 0
      /* Destroy clip path if not same in preceding context */
      if (context->graphic_context[context->index]->clip_path != (char *) NULL)
        if (LocaleCompare(context->graphic_context[context->index]->clip_path,
                          context->graphic_context[context->index-1]->clip_path) != 0)
          (void) SetImageClipMask(image,(Image *) NULL);
#endif
      DestroyDrawInfo(context->graphic_context[context->index]);
      context->index--;
      
      context->indention_depth--;
      DrawPrintf(context, "pop graphic-context\n");
    }
}

MagickExport void DrawPopPattern(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  context->indention_depth--;
  DrawPrintf(context, "pop pattern\n");
}

static int DrawPrintf(DrawContext context, const char *format, ...)
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

        for (i = context->indention_depth; i; i--)
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

MagickExport void DrawPushClipPath(DrawContext context,
                                   const char *clip_path_id)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(clip_path_id != (const char *) NULL);

  DrawPrintf(context, "push clip-path %s\n", clip_path_id);
  context->indention_depth++;
}

MagickExport void DrawPushDefs(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPrintf(context, "push defs\n");
  context->indention_depth++;
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
  context->graphic_context[context->index]=
    CloneDrawInfo((ImageInfo *) NULL,context->graphic_context[context->index-1]);

  DrawPrintf(context, "push graphic-context\n");
  context->indention_depth++;
}

MagickExport void DrawPushPattern(DrawContext context,
                                  const char *pattern_id,
                                  const double x, const double y,
                                  const double width, const double height)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(pattern_id != (const char *) NULL);

  DrawPrintf(context, "push pattern %s %.4g,%.4g %.4g,%.4g\n",
    pattern_id, x, y, width, height);
  context->indention_depth++;
}

MagickExport void DrawRectangle(DrawContext context,
                                const double x1, const double y1,
                                const double x2, const double y2)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPrintf(context, "rectangle %.4g,%.4g %.4g,%.4g\n", x1, y1, x2, y2);
}

MagickExport int DrawRender(const DrawContext context,
                            const ImageInfo *image_info,
                            Image *image)
{
  DrawInfo
    *draw_info;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  draw_info = (DrawInfo *) AcquireMemory(sizeof(DrawInfo));
  GetDrawInfo(image_info, draw_info);
  draw_info->primitive = context->mvg;
  /* puts(draw_info->primitive); */
  DrawImage(image, draw_info);
  draw_info->primitive = (char *) NULL;
  DestroyDrawInfo(draw_info);
  return True;
}

MagickExport void DrawSetRotate(DrawContext context, const double degrees)
{
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

/*   DrawPrintf(context, "rotate %.4g\n", degrees); */
}

MagickExport void DrawRoundRectangle(DrawContext context,
                                     double x1, double y1,
                                     double x2, double y2,
                                     double rx, double ry)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPrintf(context, "roundrectangle %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
                x1, y1, x2, y2, rx, ry);
}

MagickExport void DrawSetScale(DrawContext context,
                               const double x, const double y)
{
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.sx=x;
  affine.sy=y;
  DrawSetAffine(context,&affine);

/*   DrawPrintf(context, "scale %.4g,%.4g\n", x, y); */
}

MagickExport void DrawSetSkewX(DrawContext context, const double degrees)
{
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.ry=tan(DegreesToRadians(fmod(degrees,360.0)));
  DrawSetAffine(context,&affine);

/*   DrawPrintf(context, "skewX %.4g\n", degrees); */
}

MagickExport void DrawSetSkewY(DrawContext context, const double degrees)
{
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.rx=tan(DegreesToRadians(fmod(degrees,360.0)));
  DrawSetAffine(context,&affine);

/*   DrawPrintf(context, "skewY %.4g\n", degrees); */
}

MagickExport void DrawSetStopColor(DrawContext context,
                                   const PixelPacket * stop_color,
                                   const double offset)
{

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(stop_color != (const PixelPacket *) NULL);

  if (stop_color->opacity == OpaqueOpacity)
    DrawPrintf(context,
#if QuantumDepth == 8
                  "stop-color #%02x%02x%02x %.4g\n",
#elif QuantumDepth == 16
                  "stop-color #%04x%04x%04x %.4g\n",
#endif
                  stop_color->red, stop_color->green, stop_color->blue, offset);
  else
    DrawPrintf(context,
#if QuantumDepth == 8
                  "stop-color #%02x%02x%02x%02x %.4g\n",
#elif QuantumDepth == 16
                  "stop-color #%04x%04x%04x%04x %.4g\n",
#endif
                  stop_color->red, stop_color->green, stop_color->blue,
                  stop_color->opacity, offset);
}

MagickExport void DrawSetStroke(DrawContext context,
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
    new_stroke.opacity = context->graphic_context[context->index]->opacity;

  current_stroke = &context->graphic_context[context->index]->stroke;
  if( !(PixelPacketMatch(current_stroke,&new_stroke)) )
    {
      context->graphic_context[context->index]->stroke = new_stroke;

      if (stroke_color->opacity == OpaqueOpacity)
        DrawPrintf(context,
#if QuantumDepth == 8
                   "stroke #%02x%02x%02x\n",
#elif QuantumDepth == 16
                   "stroke #%04x%04x%04x\n",
#endif
                   stroke_color->red, stroke_color->green, stroke_color->blue);
      else
        DrawPrintf(context,
#if QuantumDepth == 8
                   "stroke #%02x%02x%02x%02x\n",
#elif QuantumDepth == 16
                   "stroke #%04x%04x%04x%04x\n",
#endif
                   stroke_color->red, stroke_color->green, stroke_color->blue,
                   stroke_color->opacity);
    }
}
MagickExport void DrawSetStrokeString(DrawContext context, const char* stroke_color)
{
  PixelPacket
    pixel_packet;

  if(QueryColorDatabase(stroke_color,&pixel_packet,&context->exception))
    DrawSetStroke(context,&pixel_packet);
}

MagickExport void DrawSetStrokeAntialias(DrawContext context,
                                         const int stroke_antialias)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->graphic_context[context->index]->stroke_antialias != stroke_antialias)
    {
      context->graphic_context[context->index]->stroke_antialias = stroke_antialias;

      DrawPrintf(context, "stroke-antialias %i\n", stroke_antialias ? 1 : 0);
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

  q = context->graphic_context[context->index]->dash_pattern;
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
  else if((context->graphic_context[context->index]->dash_pattern != (double*)NULL)
          && (dasharray != (double*)NULL))
    {
      p = dasharray;
      q = context->graphic_context[context->index]->dash_pattern;
      while( *p && *q )
        if( *p != *q )
          {
            updated = True;
            break;
          }
    }

  if( updated )
    {
      if(context->graphic_context[context->index]->dash_pattern != (double*)NULL)
        LiberateMemory((void **) &context->graphic_context[context->index]->dash_pattern);

      if( n_new != 0)
        {
          context->graphic_context[context->index]->dash_pattern = (double *)
            AcquireMemory((n_new+1)*sizeof(double));
          if(context->graphic_context[context->index]->dash_pattern == (double*)NULL)
            {
              ThrowException(&context->exception,ResourceLimitError,
                             "Unable to draw image","Memory allocation failed");
            }
          else
            {
              q=context->graphic_context[context->index]->dash_pattern;
              p=dasharray;
              while( *p )
                *q++=*p++;
              *q=0;
            }
        }

      DrawPrintf(context, "stroke-dasharray ");
      if ( n_new == 0 )
        DrawPrintf(context, "none");
      else
        {
          p = dasharray;
          DrawPrintf(context, "%.4g", *p++);
          while (*p)
            DrawPrintf(context, ",%.4g", *p++);
        }
      DrawPrintf(context, "\n");
    }
}

MagickExport void DrawSetStrokeDashOffset(DrawContext context,
                                          const unsigned int dash_offset)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->graphic_context[context->index]->dash_offset != dash_offset)
    {
      context->graphic_context[context->index]->dash_offset = dash_offset;

      DrawPrintf(context, "stroke-dashoffset %u\n", dash_offset);
    }
}

MagickExport void DrawSetStrokeLineCap(DrawContext context,
                                       const LineCap linecap)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->graphic_context[context->index]->linecap != linecap)
    {
      const char
        *p = NULL;

      context->graphic_context[context->index]->linecap = linecap;

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
        DrawPrintf(context, "stroke-linecap %s\n", p);
    }
}

MagickExport void DrawSetStrokeLineJoin(DrawContext context,
                                        const LineJoin linejoin)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->graphic_context[context->index]->linejoin != linejoin)
    {
      const char
        *p = NULL;

      context->graphic_context[context->index]->linejoin = linejoin;

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
        DrawPrintf(context, "stroke-linejoin %s\n", p);
    }
}

MagickExport void DrawSetStrokeMiterLimit(DrawContext context,
                                          const unsigned long miterlimit)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->graphic_context[context->index]->miterlimit != miterlimit)
    {
      context->graphic_context[context->index]->miterlimit = miterlimit;

      DrawPrintf(context, "stroke-miterlimit %u\n", miterlimit);
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

  if (context->graphic_context[context->index]->stroke.opacity != opacity)
    {
      context->graphic_context[context->index]->stroke.opacity = opacity;
      DrawPrintf(context, "stroke-opacity %.4g\n", stroke_opacity);
    }
}

MagickExport void DrawSetStrokeWidth(DrawContext context, const double stroke_width)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (context->graphic_context[context->index]->stroke_width != stroke_width)
    {
      context->graphic_context[context->index]->stroke_width = stroke_width;

      DrawPrintf(context, "stroke-width %.4g\n", stroke_width);
    }
}

MagickExport void DrawSetTextAntialias(DrawContext context,
                                       const int text_antialias)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (context->graphic_context[context->index]->text_antialias != text_antialias)
    {
      context->graphic_context[context->index]->text_antialias = text_antialias;

      DrawPrintf(context, "text-antialias %i\n", text_antialias ? 1 : 0);
    }
}

MagickExport void DrawSetTextDecoration(DrawContext context,
                                        const DecorationType decoration)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (context->graphic_context[context->index]->decorate != decoration)
    {
      context->graphic_context[context->index]->decorate = decoration;

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
        DrawPrintf(context, "decorate %s\n", p);
    }
}

MagickExport void DrawSetTextUnderColor(DrawContext context,
                                        const PixelPacket * under_color)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(under_color != (const PixelPacket *)NULL);

  if (!(PixelPacketMatch(&context->graphic_context[context->index]->box, under_color)))
    {
      context->graphic_context[context->index]->box = *under_color;

      if (under_color->opacity == OpaqueOpacity)
        DrawPrintf(context,
#if QuantumDepth == 8
                   "decorate #%02x%02x%02x\n",
#elif QuantumDepth == 16
                   "decorate #%04x%04x%04x\n",
#endif
                   under_color->red, under_color->green, under_color->blue);
      else
        DrawPrintf(context,
#if QuantumDepth == 8
                   "decorate #%02x%02x%02x%02x\n",
#elif QuantumDepth == 16
                   "decorate #%04x%04x%04x%04x\n",
#endif
                   under_color->red, under_color->green, under_color->blue,
                   under_color->opacity);
    }
}

MagickExport void DrawSetTranslate(DrawContext context,
                                   const double x, const double y)
{
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.tx=x;
  affine.ty=y;
  DrawSetAffine(context,&affine);

/*   DrawPrintf(context, "translate %.4g,%.4g\n", x, y); */
}

MagickExport void DrawSetViewbox(DrawContext context,
                                 unsigned long x1, unsigned long y1,
                                 unsigned long x2, unsigned long y2)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPrintf(context, "viewbox %lu %lu %lu %lu\n", x1, y1, x2, y2);
}
