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
#include "magick.h"
#include "define.h"

/*
  Define declarations.
*/
#define AccessContext(c) ((struct _DrawContext *)c)

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
  PathMoveToOperation,                          /* M|m (x y)+ */
}
PathOperation;

typedef enum
{
  DefaultPathMode,
  AbsolutePathMode,
  RelativePathMode
}
PathMode;

#define AccessContext(c) ((struct _DrawContext *)c)

struct _DrawContext
{
  /* MVG string and housekeeping */
  char *mvg;

  size_t
    mvg_alloc,
    mvg_length;

  /* Pretty-printing depth */
  long
    indention_depth;

  /* Path operation support */
  PathOperation
    path_operation;

  PathMode
    path_mode;
};

/*
  Forward declarations.
*/
static int
  DrawPrintf(DrawContext context, const char *format, ...);

MagickExport void DrawSetAffine(DrawContext context, const double sx,
                                   const double rx, const double ry, const double sy,
                                   const double tx, const double ty)
{
  DrawPrintf(context, "affine %.10g,%.10g,%.10g,%.10g,%.10g,%.10g\n",
                sy, rx, ry, sy, tx, ty);
}
MagickExport DrawContext DrawAllocateContext(ExceptionInfo * exception)
{
  DrawContext
    context;

  context = (DrawContext) AcquireMemory(sizeof(struct _DrawContext));
  AccessContext(context)->mvg = NULL;
  AccessContext(context)->mvg_alloc = 0;
  AccessContext(context)->mvg_length = 0;

  AccessContext(context)->indention_depth = 0;

  AccessContext(context)->path_operation = PathDefaultOperation;
  AccessContext(context)->path_mode = DefaultPathMode;

  return context;
}
MagickExport void DrawSetAngle(DrawContext context, const double degrees)
{
  DrawPrintf(context, "angle %.10g\n", degrees);
}
MagickExport void DrawArc(DrawContext context, const double sx,
                             const double sy, const double ex, const double ey,
                             const double sd, const double ed)
{
  DrawPrintf(context, "arc %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
                sx, sy, ex, ey, sd, ed);
}
MagickExport void DrawBezier(DrawContext context, const size_t num_coords,
                                const PointInfo * coordinates)
{
  const PointInfo * coordinate;

  size_t i;

  DrawPrintf(context, "bezier");
  for (i = num_coords, coordinate = coordinates; i; i--)
    {
      DrawPrintf(context, " %.10g,%.10g", coordinate->x, coordinate->y);
      ++coordinate;
    }
  DrawPrintf(context, "\n");
}
MagickExport void DrawCircle(DrawContext context, const double ox,
                                const double oy, const double px, const double py)
{
  DrawPrintf(context, "circle %.10g,%.10g %.10g,%.10g\n", ox, oy, px, py);
}
MagickExport void DrawSetClipPath(DrawContext context, const char *clip_url)
{
  DrawPrintf(context, "clip-path url(%s)\n", clip_url);
}
MagickExport void DrawSetClipRule(DrawContext context,
                                     const FillRule fill_rule)
{
  const char *p = NULL;

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
MagickExport void DrawSetClipUnits(DrawContext context,
                                      const ClipPathUnits clip_units)
{
  const char *p = NULL;

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
MagickExport void DrawColor(DrawContext context, const double x,
                               const double y, const PaintMethod paintMethod)
{
  const char *p = NULL;

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
  LiberateMemory((void **) &AccessContext(context)->mvg);
  AccessContext(context)->mvg_alloc = 0;
  AccessContext(context)->mvg_length = 0;
  LiberateMemory((void **) &context);
}
MagickExport void DrawEllipse(DrawContext context,
                                 const double ox, const double oy,
                                 const double rx, const double ry,
                                 const double start, const double end)
{
  DrawPrintf(context, "ellipse %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
                ox, oy, rx, ry, start, end);

}
MagickExport void DrawSetFill(DrawContext context,
                                 const PixelPacket * fill_color)
{
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
MagickExport void DrawSetFillOpacity(DrawContext context,
                                        const double fill_opacity)
{
  DrawPrintf(context, "fill-opacity %.10g\n", fill_opacity);
}
MagickExport void DrawSetFillRule(DrawContext context,
                                     const FillRule fill_rule)
{
  const char *p = NULL;

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
MagickExport void DrawSetFont(DrawContext context, const char *font_name)
{
  DrawPrintf(context, "font %s\n", font_name);
}
MagickExport void DrawSetFontFamily(DrawContext context,
                                       const char *font_family)
{
  DrawPrintf(context, "font-family %s\n", font_family);
}
MagickExport void DrawSetFontSize(DrawContext context,
                                     const double font_pointsize)
{
  DrawPrintf(context, "font-size %.10g\n", font_pointsize);
}
MagickExport void DrawSetFontStretch(DrawContext context,
                                        const StretchType font_stretch)
{
  const char *p = NULL;

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
MagickExport void DrawSetFontStyle(DrawContext context,
                                      const StyleType font_style)
{
  const char *p = NULL;

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
MagickExport void DrawSetFontWeight(DrawContext context,
                                       const double font_weight)
{
  DrawPrintf(context, "font-weight %.10g\n", font_weight);
}
MagickExport void DrawSetGravity(DrawContext context,
                                    const GravityType gravity)
{
  const char *p = NULL;

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
MagickExport void DrawComposite(DrawContext context,
                                const CompositeOperator composite_operator,
                                const double x, const double y,
                                const double width, const double height,
                                const Image * image )
{

}
MagickExport void DrawCompositeFile(DrawContext context,
                                    const CompositeOperator composite_operator,
                                    const double x, const double y,
                                    const double width, const double height,
                                    const char * filespec )
{
  const char *p = NULL;

  switch (composite_operator)
    {
    case AddCompositeOp:
      p = "Add";
      break;
    case AtopCompositeOp:
      p = "Atop";
      break;
    case BumpmapCompositeOp:
      p = "Bumpmap";
      break;
    case ClearCompositeOp:
      p = "Clear";
      break;
    case ColorizeCompositeOp:
      p = "Colorize_not_supported";
      break;
    case CopyBlueCompositeOp:
      p = "CopyBlue";
      break;
    case CopyCompositeOp:
      p = "Copy";
      break;
    case CopyGreenCompositeOp:
      p = "CopyGreen";
      break;
    case CopyOpacityCompositeOp:
      p = "CopyOpacity";
      break;
    case CopyRedCompositeOp:
      p = "CopyRed";
      break;
    case DarkenCompositeOp:
      p = "Darken_not_supported";
      break;
    case DifferenceCompositeOp:
      p = "Difference";
      break;
    case DisplaceCompositeOp:
      p = "Displace_not_supported";
      break;
    case DissolveCompositeOp:
      p = "Dissolve_not_supported";
      break;
    case HueCompositeOp:
      p = "Hue_not_supported";
      break;
    case InCompositeOp:
      p = "In";
      break;
    case LightenCompositeOp:
      p = "Lighten_not_supported";
      break;
    case LuminizeCompositeOp:
      p = "Luminize_not_supported";
      break;
    case MinusCompositeOp:
      p = "Minus";
      break;
    case ModulateCompositeOp:
      p = "Modulate_not_supported";
      break;
    case MultiplyCompositeOp:
      p = "Multiply";
      break;
    case NoCompositeOp:
      p = "No_not_supported";
      break;
    case OutCompositeOp:
      p = "Out";
      break;
    case OverCompositeOp:
      p = "Over";
      break;
    case OverlayCompositeOp:
      p = "Overlay_not_supported";
      break;
    case PlusCompositeOp:
      p = "Plus";
      break;
    case SaturateCompositeOp:
      p = "Saturate_not_supported";
      break;
    case ScreenCompositeOp:
      p = "Screen_not_supported";
      break;
    case SubtractCompositeOp:
      p = "Subtract";
      break;
    case ThresholdCompositeOp:
      p = "Threshold";
      break;
    case XorCompositeOp:
      p = "Xor";
      break;
    default:
      break;
    }

  if (p != NULL)
    DrawPrintf(context, "image %s %.10g,%.10g %.10g,%.10g %s\n",
                  p, x, y, width, height, filespec);
}
MagickExport void DrawLine(DrawContext context, const double sx,
                              const double sy, const double ex, const double ey)
{
  DrawPrintf(context, "line %.10g,%.10g %.10g,%.10g\n", sx, sy, ex, ey);
}
MagickExport void DrawMatte(DrawContext context, const double x,
                               const double y, const PaintMethod paint_method)
{
  const char *p = NULL;

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
    DrawPrintf(context, "matte %.10g,%.10g %s\n", x, y, p);
}

MagickExport void DrawPathClose(DrawContext context)
{
  DrawPrintf(context, "%s", AccessContext(context)->path_mode == AbsolutePathMode ? "Z" : "z");
}
static void DrawPathCurveTo(DrawContext context, const PathMode mode,
                               const double x1, const double y1, const double x2,
                               const double y2, const double x, const double y)
{
  if ((AccessContext(context)->path_operation != PathCurveToOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathCurveToOperation;
      AccessContext(context)->path_mode = mode;
      DrawPrintf(context, "%c%.10g,%.10g %.10g,%.10g %.10g,%.10g",
                    mode == AbsolutePathMode ? 'C' : 'c', x1, y1, x2, y2, x, y);
    }
  else
    DrawPrintf(context, " %.10g,%.10g %.10g,%.10g %.10g,%.10g", x1, y1, x2, y2, x,
                  y);
}
MagickExport void DrawPathCurveToAbsolute(DrawContext context,
                                             const double x1, const double y1,
                                             const double x2, const double y2,
                                             const double x, const double y)
{
  DrawPathCurveTo(context, AbsolutePathMode, x1, y1, x2, y2, x, y);
}
MagickExport void DrawPathCurveToRelative(DrawContext context,
                                             const double x1, const double y1,
                                             const double x2, const double y2,
                                             const double x, const double y)
{
  DrawPathCurveTo(context, RelativePathMode, x1, y1, x2, y2, x, y);
}
static void DrawPathCurveToQuadraticBezier(DrawContext context,
                                              const PathMode mode, const double x1,
                                              const double y1, const double x,
                                              const double y)
{
  if ((AccessContext(context)->path_operation != PathCurveToQuadraticBezierOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathCurveToQuadraticBezierOperation;
      AccessContext(context)->path_mode = mode;
      DrawPrintf(context, "%c%.10g,%.10g %.10g,%.10g",
                    mode == AbsolutePathMode ? 'Q' : 'q', x1, y1, x, y);
    }
  else
    DrawPrintf(context, " %.10g,%.10g %.10g,%.10g", x1, y1, x, y);
}
MagickExport void DrawPathCurveToQuadraticBezierAbsolute(DrawContext context,
                                                            const double x1,
                                                            const double y1,
                                                            const double x,
                                                            const double y)
{
  DrawPathCurveToQuadraticBezier(context, AbsolutePathMode, x1, y1, x, y);
}
MagickExport void DrawPathCurveToQuadraticBezierRelative(DrawContext context,
                                                            const double x1,
                                                            const double y1,
                                                            const double x,
                                                            const double y)
{
  DrawPathCurveToQuadraticBezier(context, RelativePathMode, x1, y1, x, y);
}
static void DrawPathCurveToQuadraticBezierSmooth(DrawContext context,
                                                    const PathMode mode,
                                                    const double x, const double y)
{
  if ((AccessContext(context)->path_operation != PathCurveToQuadraticBezierSmoothOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathCurveToQuadraticBezierSmoothOperation;
      AccessContext(context)->path_mode = mode;
      DrawPrintf(context, "%c%.10g,%.10g", mode == AbsolutePathMode ? 'T' : 't', x,
                    y);
    }
  else
    DrawPrintf(context, " %.10g,%.10g", x, y);
}
MagickExport void DrawPathCurveToQuadraticBezierSmoothAbsolute(DrawContext
                                                                  context,
                                                                  const double x,
                                                                  const double y)
{
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
  if ((AccessContext(context)->path_operation != PathCurveToSmoothOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathCurveToSmoothOperation;
      AccessContext(context)->path_mode = mode;
      DrawPrintf(context, "%c%.10g,%.10g %.10g,%.10g",
                    mode == AbsolutePathMode ? 'S' : 's', x2, y2, x, y);
    }
  else
    DrawPrintf(context, " %.10g,%.10g %.10g,%.10g", x2, y2, x, y);
}
MagickExport void DrawPathCurveToSmoothAbsolute(DrawContext context,
                                                   const double x2, const double y2,
                                                   const double x, const double y)
{
  DrawPathCurveToSmooth(context, AbsolutePathMode, x2, y2, x, y);
}
MagickExport void DrawPathCurveToSmoothRelative(DrawContext context,
                                                   const double x2, const double y2,
                                                   const double x, const double y)
{
  DrawPathCurveToSmooth(context, RelativePathMode, x2, y2, x, y);
}
static void DrawPathEllipticArc(DrawContext context, const PathMode mode,
                                   const double rx, const double ry,
                                   const double x_axis_rotation,
                                   unsigned int large_arc_flag,
                                   unsigned int sweep_flag, const double x,
                                   const double y)
{
  if ((AccessContext(context)->path_operation != PathEllipticArcOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathEllipticArcOperation;
      AccessContext(context)->path_mode = mode;
      DrawPrintf(context, "%c%.10g,%.10g %.10g %u %u %.10g,%.10g",
                    mode == AbsolutePathMode ? 'A' : 'a', rx, ry, x_axis_rotation,
                    large_arc_flag, sweep_flag, x, y);
    }
  else
    DrawPrintf(context, " %.10g,%.10g %.10g %u %u %.10g,%.10g", rx, ry,
                  x_axis_rotation, large_arc_flag, sweep_flag, x, y);
}
MagickExport void DrawPathEllipticArcAbsolute(DrawContext context,
                                                 const double rx, const double ry,
                                                 const double x_axis_rotation,
                                                 unsigned int large_arc_flag,
                                                 unsigned int sweep_flag,
                                                 const double x, const double y)
{
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
  DrawPrintf(context, "\"\n");
  AccessContext(context)->path_operation = PathDefaultOperation;
  AccessContext(context)->path_mode = DefaultPathMode;
}
static void DrawPathLineTo(DrawContext context, const PathMode mode,
                              const double x, const double y)
{
  if ((AccessContext(context)->path_operation != PathLineToOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathLineToOperation;
      AccessContext(context)->path_mode = mode;
      DrawPrintf(context, "%c%.10g,%.10g", mode == AbsolutePathMode ? 'L' : 'l', x,
                    y);
    }
  else
    DrawPrintf(context, " %.10g,%.10g", x, y);
}
MagickExport void DrawPathLineToAbsolute(DrawContext context, const double x,
                                            const double y)
{
  DrawPathLineTo(context, AbsolutePathMode, x, y);
}
MagickExport void DrawPathLineToRelative(DrawContext context, const double x,
                                            const double y)
{
  DrawPathLineTo(context, RelativePathMode, x, y);
}
static void DrawPathLineToHorizontal(DrawContext context,
                                        const PathMode mode, const double x)
{
  if ((AccessContext(context)->path_operation != PathLineToHorizontalOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathLineToHorizontalOperation;
      AccessContext(context)->path_mode = mode;
      DrawPrintf(context, "%c%.10g", mode == AbsolutePathMode ? 'H' : 'h', x);
    }
  else
    DrawPrintf(context, " %.10g", x);
}
MagickExport void DrawPathLineToHorizontalAbsolute(DrawContext context,
                                                      const double x)
{
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
  if ((AccessContext(context)->path_operation != PathLineToVerticalOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathLineToVerticalOperation;
      AccessContext(context)->path_mode = mode;
      DrawPrintf(context, "%c%.10g", mode == AbsolutePathMode ? 'V' : 'v', y);
    }
  else
    DrawPrintf(context, " %.10g", y);
}
MagickExport void DrawPathLineToVerticalAbsolute(DrawContext context,
                                                    const double y)
{
  DrawPathLineToVertical(context, AbsolutePathMode, y);
}
MagickExport void DrawPathLineToVerticalRelative(DrawContext context,
                                                    const double y)
{
  DrawPathLineToVertical(context, RelativePathMode, y);
}
static void DrawPathMoveTo(DrawContext context, const PathMode mode,
                              const double x, const double y)
{
  if ((AccessContext(context)->path_operation != PathMoveToOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathMoveToOperation;
      AccessContext(context)->path_mode = mode;
      DrawPrintf(context, "%c%.10g,%.10g", mode == AbsolutePathMode ? 'M' : 'm', x,
                    y);
    }
  else
    DrawPrintf(context, " %.10g,%.10g", x, y);
}
MagickExport void DrawPathMoveToAbsolute(DrawContext context, const double x,
                                            const double y)
{
  DrawPathMoveTo(context, AbsolutePathMode, x, y);
}
MagickExport void DrawPathMoveToRelative(DrawContext context, const double x,
                                            const double y)
{
  DrawPathMoveTo(context, RelativePathMode, x, y);
}
MagickExport void DrawPathStart(DrawContext context)
{
  DrawPrintf(context, "path d=\"");
  AccessContext(context)->path_operation = PathDefaultOperation;
  AccessContext(context)->path_mode = DefaultPathMode;
}
MagickExport void DrawPoint(DrawContext context, const double x,
             const double y)
{
  DrawPrintf(context, "point %.10g,%.10g\n", x, y);
}
MagickExport void DrawPolygon(DrawContext context, const size_t num_coords,
                                 const PointInfo * coordinates)
{
  const PointInfo * coordinate;

  size_t i;

  DrawPrintf(context, "polygon");
  for (i = num_coords, coordinate = coordinates; i; i--)
    {
      DrawPrintf(context, " %.10g,%.10g", coordinate->x, coordinate->y);
      ++coordinate;
    }
  DrawPrintf(context, "\n");
}
MagickExport void DrawPolyline(DrawContext context, const size_t num_coords,
                                  const PointInfo * coordinates)
{
  const PointInfo * coordinate;

  size_t i;

  DrawPrintf(context, "polyline");
  for (i = num_coords, coordinate = coordinates; i; i--)
    {
      DrawPrintf(context, " %.10g,%.10g", coordinate->x, coordinate->y);
      ++coordinate;
    }
  DrawPrintf(context, "\n");
}
MagickExport void DrawPopClipPath(DrawContext context)
{
  AccessContext(context)->indention_depth--;
  DrawPrintf(context, "pop clip-path\n");
}
MagickExport void DrawPopDefs(DrawContext context)
{
  AccessContext(context)->indention_depth--;
  DrawPrintf(context, "pop defs\n");
}
MagickExport void DrawPopGraphicContext(DrawContext context)
{
  AccessContext(context)->indention_depth--;
  DrawPrintf(context, "pop graphic-context\n");
}
MagickExport void DrawPopPattern(DrawContext context)
{
  AccessContext(context)->indention_depth--;
  DrawPrintf(context, "pop pattern\n");
}
static int DrawPrintf(DrawContext context, const char *format, ...)
{
  const size_t alloc_size = MaxTextExtent * 20; /* 40K */

  /* Allocate initial memory */
  if (AccessContext(context)->mvg == 0)
    {
      AccessContext(context)->mvg = (char *) AcquireMemory(alloc_size);
      AccessContext(context)->mvg_alloc = alloc_size;
      AccessContext(context)->mvg_length = 0;
      if (AccessContext(context)->mvg == 0)
        return -1;
    }

  /* Re-allocate additional memory if necessary (ensure 20K unused) */
  if (AccessContext(context)->mvg_alloc < (AccessContext(context)->mvg_length + MaxTextExtent * 10))
    {
      size_t realloc_size = AccessContext(context)->mvg_alloc + alloc_size;

      ReacquireMemory((void **) &AccessContext(context)->mvg, realloc_size);
      if (AccessContext(context)->mvg == NULL)
        return -1;
      AccessContext(context)->mvg_alloc = realloc_size;
    }

  /* Write to end of existing MVG string */
  {
    long str_length;

    va_list argp;

    /* Pretty-print indentation */
    if (*(AccessContext(context)->mvg + AccessContext(context)->mvg_length - 1) == '\n')
      {
        long i;

        for (i = AccessContext(context)->indention_depth; i; i--)
          {
            *(AccessContext(context)->mvg + AccessContext(context)->mvg_length) = ' ';
            ++AccessContext(context)->mvg_length;
          }
        *(AccessContext(context)->mvg + AccessContext(context)->mvg_length) = 0;
      }

    va_start(argp, format);
#if !defined(HAVE_VSNPRINTF)
    str_length = vsprintf(AccessContext(context)->mvg + AccessContext(context)->mvg_length, format, argp);
#else
    str_length =
      vsnprintf(AccessContext(context)->mvg + AccessContext(context)->mvg_length,
                AccessContext(context)->mvg_alloc - AccessContext(context)->mvg_length - 1, format, argp);
#endif
    va_end(argp);

    AccessContext(context)->mvg_length += str_length;
    *(AccessContext(context)->mvg + AccessContext(context)->mvg_length) = 0;

    assert(AccessContext(context)->mvg_length + 1 < AccessContext(context)->mvg_alloc);

    return str_length;
  }
}
MagickExport void DrawPushClipPath(DrawContext context,
                                      const char *clip_path_id)
{
  DrawPrintf(context, "push clip-path %s\n", clip_path_id);
  AccessContext(context)->indention_depth++;
}
MagickExport void DrawPushDefs(DrawContext context)
{
  DrawPrintf(context, "push defs\n");
  AccessContext(context)->indention_depth++;
}
MagickExport void DrawPushGraphicContext(DrawContext context)
{
  DrawPrintf(context, "push graphic-context\n");
  AccessContext(context)->indention_depth++;
}
MagickExport void DrawPushPattern(DrawContext context,
                                     const char *pattern_id, const double x,
                                     const double y, const double width,
                                     const double height)
{
  DrawPrintf(context, "push pattern %s %.10g,%.10g %.10g,%.10g\n",
    pattern_id, x, y, width, height);
  AccessContext(context)->indention_depth++;
}
MagickExport void DrawRectangle(DrawContext context, const double x1,
                                   const double y1, const double x2, const double y2)
{
  DrawPrintf(context, "rectangle %.10g,%.10g %.10g,%.10g\n", x1, y1, x2, y2);
}
MagickExport int DrawRender(Image * image, const ImageInfo * image_info,
                               const DrawContext context)
{
  DrawInfo * draw_info;

  draw_info = (DrawInfo *) AcquireMemory(sizeof(DrawInfo));
  GetDrawInfo(image_info, draw_info);
  draw_info->primitive = AccessContext(context)->mvg;
  /* puts(draw_info->primitive); */
  DrawImage(image, draw_info);
  draw_info->primitive = (char *) NULL;
  DestroyDrawInfo(draw_info);
  return True;
}
MagickExport void DrawSetRotate(DrawContext context, const double degrees)
{
  DrawPrintf(context, "rotate %.10g\n", degrees);
}
MagickExport void DrawRoundRectangle(DrawContext context,
                                        double x1, double y1,
                                        double x2, double y2, double rx, double ry)
{
  DrawPrintf(context, "roundrectangle %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
                x1, y1, x2, y2, rx, ry);
}
MagickExport void DrawSetScale(DrawContext context, const double x,
                                  const double y)
{
  DrawPrintf(context, "scale %.10g,%.10g\n", x, y);
}
MagickExport void DrawSetSkewX(DrawContext context, const double degrees)
{
  DrawPrintf(context, "skewX %.10g\n", degrees);
}
MagickExport void DrawSetSkewY(DrawContext context, const double degrees)
{
  DrawPrintf(context, "skewY %.10g\n", degrees);
}
MagickExport void DrawSetStopColor(DrawContext context,
                                      const PixelPacket * stop_color,
                                      const double offset)
{
  if (stop_color->opacity == OpaqueOpacity)
    DrawPrintf(context,
#if QuantumDepth == 8
                  "stop-color #%02x%02x%02x %.10g\n",
#elif QuantumDepth == 16
                  "stop-color #%04x%04x%04x %.10g\n",
#endif
                  stop_color->red, stop_color->green, stop_color->blue, offset);
  else
    DrawPrintf(context,
#if QuantumDepth == 8
                  "stop-color #%02x%02x%02x%02x %.10g\n",
#elif QuantumDepth == 16
                  "stop-color #%04x%04x%04x%04x %.10g\n",
#endif
                  stop_color->red, stop_color->green, stop_color->blue,
                  stop_color->opacity, offset);
}
MagickExport void DrawSetStroke(DrawContext context,
                                   const PixelPacket * stroke_color)
{
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
MagickExport void DrawSetStrokeAntialias(DrawContext context,
                                            const int true_false)
{
  DrawPrintf(context, "stroke-antialias %i\n", true_false ? 1 : 0);
}
MagickExport void DrawSetStrokeDashArray(DrawContext context,
                                            const unsigned int *dasharray)
{
  register const unsigned int *p = dasharray;

  DrawPrintf(context, "stroke-dasharray ");
  if ((p == (const unsigned int *) NULL) || (*p == 0))
    DrawPrintf(context, "none");
  else
    {
      DrawPrintf(context, "%u", *p++);
      while (*p)
        DrawPrintf(context, ",%u", *p++);
    }
  DrawPrintf(context, "\n");
}
MagickExport void DrawSetStrokeDashOffset(DrawContext context,
                                             const unsigned int dashoffset)
{
  DrawPrintf(context, "stroke-dashoffset %u\n", dashoffset);
}
MagickExport void DrawSetStrokeLineCap(DrawContext context,
                                          const LineCap linecap)
{
  const char *p = NULL;

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
MagickExport void DrawSetStrokeLineJoin(DrawContext context,
                                           const LineJoin linejoin)
{
  const char *p = NULL;

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
MagickExport void DrawSetStrokeMiterLimit(DrawContext context,
                                             const unsigned int miterlimit)
{
  DrawPrintf(context, "stroke-miterlimit %u\n", miterlimit);
}
MagickExport void DrawSetStrokeOpacity(DrawContext context,
                                          const double opacity)
{
  DrawPrintf(context, "stroke-opacity %.10g\n", opacity);
}
MagickExport void DrawSetStrokeWidth(DrawContext context, const double width)
{
  DrawPrintf(context, "stroke-width %.10g\n", width);
}
MagickExport void DrawText(DrawContext context, const double x,
                              const double y, const char *text)
{
  char escaped_string[MaxTextExtent];

  unsigned char *p,
    *q;

  int string_length;

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

  DrawPrintf(context, "text %.10g,%.10g '%.1024s'\n", escaped_string);
}
MagickExport void DrawSetTextAntialias(DrawContext context,
                                          const int true_false)
{
  DrawPrintf(context, "text-antialias %i\n", true_false ? 1 : 0);
}
MagickExport void DrawSetTextDecoration(DrawContext context,
                                           const DecorationType decoration)
{
  const char *p = NULL;

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
MagickExport void DrawSetTextUnderColor(DrawContext context,
                                           const PixelPacket * under_color)
{
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

MagickExport void DrawSetTranslate(DrawContext context, const double x,
                                      const double y)
{
  DrawPrintf(context, "translate %.10g,%.10g\n", x, y);
}
MagickExport void DrawSetViewbox(DrawContext context,
                                    unsigned long x1, unsigned long y1,
                                    unsigned long x2, unsigned long y2)
{
  DrawPrintf(context, "viewbox %lu %lu %lu %lu\n", x1, y1, x2, y2);
}
