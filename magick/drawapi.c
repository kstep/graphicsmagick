/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%               DDDD   RRRR    AAA   W   W    AAA   PPPP   IIIII              %
%               D   D  R   R  A   A  W   W   A   A  P   P    I                %
%               D   D  RRRR   AAAAA  W   W   AAAAA  PPPP     I                %
%               D   D  R R    A   A  W W W   A   A  P        I                %
%               DDDD   R  R   A   A   W W    A   A  P      IIIII              %
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

#include "drawapi.h"

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

#define AccessContext(c) ((struct _MgkDrawContext *)c)

struct _MgkDrawContext
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

MagickExport void MgkDrawSetAffine(MgkDrawContext context, const double sx,
                                   const double rx, const double ry, const double sy,
                                   const double tx, const double ty)
{
  MgkDrawPrintf(context, "affine %.10g,%.10g,%.10g,%.10g,%.10g,%.10g\n",
                sy, rx, ry, sy, tx, ty);
}
MagickExport MgkDrawContext MgkDrawAllocateContext(ExceptionInfo * exception)
{
  MgkDrawContext
    context;

  context = (MgkDrawContext) AcquireMemory(sizeof(struct _MgkDrawContext));
  AccessContext(context)->mvg = NULL;
  AccessContext(context)->mvg_alloc = 0;
  AccessContext(context)->mvg_length = 0;

  AccessContext(context)->indention_depth = 0;

  AccessContext(context)->path_operation = PathDefaultOperation;
  AccessContext(context)->path_mode = DefaultPathMode;

  return context;
}
MagickExport void MgkDrawSetAngle(MgkDrawContext context, const double degrees)
{
  MgkDrawPrintf(context, "angle %.10g\n", degrees);
}
MagickExport void MgkDrawArc(MgkDrawContext context, const double sx,
                             const double sy, const double ex, const double ey,
                             const double sd, const double ed)
{
  MgkDrawPrintf(context, "arc %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
                sx, sy, ex, ey, sd, ed);
}
MagickExport void MgkDrawBezier(MgkDrawContext context, const size_t num_coords,
                                const PointInfo * coordinates)
{
  const PointInfo * coordinate;

  size_t i;

  MgkDrawPrintf(context, "bezier");
  for (i = num_coords, coordinate = coordinates; i; i--)
    {
      MgkDrawPrintf(context, " %.10g,%.10g", coordinate->x, coordinate->y);
      ++coordinate;
    }
  MgkDrawPrintf(context, "\n");
}
MagickExport void MgkDrawCircle(MgkDrawContext context, const double ox,
                                const double oy, const double px, const double py)
{
  MgkDrawPrintf(context, "circle %.10g,%.10g %.10g,%.10g\n", ox, oy, px, py);
}
MagickExport void MgkDrawSetClipPath(MgkDrawContext context, const char *clip_url)
{
  MgkDrawPrintf(context, "clip-path url(%s)\n", clip_url);
}
MagickExport void MgkDrawSetClipRule(MgkDrawContext context,
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
    }

  if (p != NULL)
    MgkDrawPrintf(context, "clip-rule %s\n", p);
}
MagickExport void MgkDrawSetClipUnits(MgkDrawContext context,
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
    MgkDrawPrintf(context, "clip-units %s\n", p);
}
MagickExport void MgkDrawColor(MgkDrawContext context, const double x,
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
    MgkDrawPrintf(context, "color %s\n", p);
}
MagickExport void MgkDrawDestroyContext(MgkDrawContext context)
{
  LiberateMemory((void **) &AccessContext(context)->mvg);
  AccessContext(context)->mvg_alloc = 0;
  AccessContext(context)->mvg_length = 0;
  LiberateMemory((void **) &context);
}
MagickExport void MgkDrawEllipse(MgkDrawContext context,
                                 const double ox, const double oy,
                                 const double rx, const double ry,
                                 const double start, const double end)
{
  MgkDrawPrintf(context, "ellipse %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
                ox, oy, rx, ry, start, end);

}
MagickExport void MgkDrawSetFill(MgkDrawContext context,
                                 const PixelPacket * fill_color)
{
  if (fill_color->opacity == OpaqueOpacity)
    MgkDrawPrintf(context,
#if QuantumDepth == 8
                  "fill #%02x%02x%02x\n",
#elif QuantumDepth == 16
                  "fill #%04x%04x%04x\n",
#endif
                  fill_color->red, fill_color->green, fill_color->blue);
  else
    MgkDrawPrintf(context,
#if QuantumDepth == 8
                  "fill #%02x%02x%02x%02x\n",
#elif QuantumDepth == 16
                  "fill #%04x%04x%04x%04x\n",
#endif
                  fill_color->red, fill_color->green, fill_color->blue,
                  fill_color->opacity);
}
MagickExport void MgkDrawSetFillOpacity(MgkDrawContext context,
                                        const double fill_opacity)
{
  MgkDrawPrintf(context, "fill-opacity %.10g\n", fill_opacity);
}
MagickExport void MgkDrawSetFillRule(MgkDrawContext context,
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
    }

  if (p != NULL)
    MgkDrawPrintf(context, "fill-rule %s\n", p);
}
MagickExport void MgkDrawSetFont(MgkDrawContext context, const char *font_name)
{
  MgkDrawPrintf(context, "font %s\n", font_name);
}
MagickExport void MgkDrawSetFontFamily(MgkDrawContext context,
                                       const char *font_family)
{
  MgkDrawPrintf(context, "font-family %s\n", font_family);
}
MagickExport void MgkDrawSetFontSize(MgkDrawContext context,
                                     const double font_pointsize)
{
  MgkDrawPrintf(context, "font-size %.10g\n", font_pointsize);
}
MagickExport void MgkDrawSetFontStretch(MgkDrawContext context,
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
    MgkDrawPrintf(context, "font-stretch %s\n", p);
}
MagickExport void MgkDrawSetFontStyle(MgkDrawContext context,
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
    MgkDrawPrintf(context, "font-style %s\n", p);
}
MagickExport void MgkDrawSetFontWeight(MgkDrawContext context,
                                       const double font_weight)
{
  MgkDrawPrintf(context, "font-weight %.10g\n", font_weight);
}
MagickExport void MgkDrawSetGravity(MgkDrawContext context,
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
    MgkDrawPrintf(context, "gravity %s\n", p);
}
MagickExport void MgkDrawImage(MgkDrawContext context, const double x,
                               const double y, const double width,
                               const double height, const Image * image,
                               const CompositeOperator composition)
{
}
MagickExport void MgkDrawLine(MgkDrawContext context, const double sx,
                              const double sy, const double ex, const double ey)
{
  MgkDrawPrintf(context, "line %.10g,%.10g %.10g,%.10g\n", sx, sy, ex, ey);
}
MagickExport void MgkDrawMatte(MgkDrawContext context, const double x,
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
    MgkDrawPrintf(context, "matte %.10g,%.10g %s\n", x, y, p);
}

MagickExport void MgkDrawPathClose(MgkDrawContext context)
{
  MgkDrawPrintf(context, "%s", AccessContext(context)->path_mode == AbsolutePathMode ? "Z" : "z");
}
static void MgkDrawPathCurveTo(MgkDrawContext context, const PathMode mode,
                               const double x1, const double y1, const double x2,
                               const double y2, const double x, const double y)
{
  if ((AccessContext(context)->path_operation != PathCurveToOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathCurveToOperation;
      AccessContext(context)->path_mode = mode;
      MgkDrawPrintf(context, "%c%.10g,%.10g %.10g,%.10g %.10g,%.10g",
                    mode == AbsolutePathMode ? 'C' : 'c', x1, y1, x2, y2, x, y);
    }
  else
    MgkDrawPrintf(context, " %.10g,%.10g %.10g,%.10g %.10g,%.10g", x1, y1, x2, y2, x,
                  y);
}
MagickExport void MgkDrawPathCurveToAbsolute(MgkDrawContext context,
                                             const double x1, const double y1,
                                             const double x2, const double y2,
                                             const double x, const double y)
{
  MgkDrawPathCurveTo(context, AbsolutePathMode, x1, y1, x2, y2, x, y);
}
MagickExport void MgkDrawPathCurveToRelative(MgkDrawContext context,
                                             const double x1, const double y1,
                                             const double x2, const double y2,
                                             const double x, const double y)
{
  MgkDrawPathCurveTo(context, RelativePathMode, x1, y1, x2, y2, x, y);
}
static void MgkDrawPathCurveToQuadraticBezier(MgkDrawContext context,
                                              const PathMode mode, const double x1,
                                              const double y1, const double x,
                                              const double y)
{
  if ((AccessContext(context)->path_operation != PathCurveToQuadraticBezierOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathCurveToQuadraticBezierOperation;
      AccessContext(context)->path_mode = mode;
      MgkDrawPrintf(context, "%c%.10g,%.10g %.10g,%.10g",
                    mode == AbsolutePathMode ? 'Q' : 'q', x1, y1, x, y);
    }
  else
    MgkDrawPrintf(context, " %.10g,%.10g %.10g,%.10g", x1, y1, x, y);
}
MagickExport void MgkDrawPathCurveToQuadraticBezierAbsolute(MgkDrawContext context,
                                                            const double x1,
                                                            const double y1,
                                                            const double x,
                                                            const double y)
{
  MgkDrawPathCurveToQuadraticBezier(context, AbsolutePathMode, x1, y1, x, y);
}
MagickExport void MgkDrawPathCurveToQuadraticBezierRelative(MgkDrawContext context,
                                                            const double x1,
                                                            const double y1,
                                                            const double x,
                                                            const double y)
{
  MgkDrawPathCurveToQuadraticBezier(context, RelativePathMode, x1, y1, x, y);
}
static void MgkDrawPathCurveToQuadraticBezierSmooth(MgkDrawContext context,
                                                    const PathMode mode,
                                                    const double x, const double y)
{
  if ((AccessContext(context)->path_operation != PathCurveToQuadraticBezierSmoothOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathCurveToQuadraticBezierSmoothOperation;
      AccessContext(context)->path_mode = mode;
      MgkDrawPrintf(context, "%c%.10g,%.10g", mode == AbsolutePathMode ? 'T' : 't', x,
                    y);
    }
  else
    MgkDrawPrintf(context, " %.10g,%.10g", x, y);
}
MagickExport void MgkDrawPathCurveToQuadraticBezierSmoothAbsolute(MgkDrawContext
                                                                  context,
                                                                  const double x,
                                                                  const double y)
{
  MgkDrawPathCurveToQuadraticBezierSmooth(context, AbsolutePathMode, x, y);
}
MagickExport void MgkDrawPathCurveToQuadraticBezierSmoothRelative(MgkDrawContext
                                                                  context,
                                                                  const double x,
                                                                  const double y)
{
  MgkDrawPathCurveToQuadraticBezierSmooth(context, RelativePathMode, x, y);
}
static void MgkDrawPathCurveToSmooth(MgkDrawContext context, const PathMode mode,
                                     const double x2, const double y2,
                                     const double x, const double y)
{
  if ((AccessContext(context)->path_operation != PathCurveToSmoothOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathCurveToSmoothOperation;
      AccessContext(context)->path_mode = mode;
      MgkDrawPrintf(context, "%c%.10g,%.10g %.10g,%.10g",
                    mode == AbsolutePathMode ? 'S' : 's', x2, y2, x, y);
    }
  else
    MgkDrawPrintf(context, " %.10g,%.10g %.10g,%.10g", x2, y2, x, y);
}
MagickExport void MgkDrawPathCurveToSmoothAbsolute(MgkDrawContext context,
                                                   const double x2, const double y2,
                                                   const double x, const double y)
{
  MgkDrawPathCurveToSmooth(context, AbsolutePathMode, x2, y2, x, y);
}
MagickExport void MgkDrawPathCurveToSmoothRelative(MgkDrawContext context,
                                                   const double x2, const double y2,
                                                   const double x, const double y)
{
  MgkDrawPathCurveToSmooth(context, RelativePathMode, x2, y2, x, y);
}
static void MgkDrawPathEllipticArc(MgkDrawContext context, const PathMode mode,
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
      MgkDrawPrintf(context, "%c%.10g,%.10g %.10g %u %u %.10g,%.10g",
                    mode == AbsolutePathMode ? 'A' : 'a', rx, ry, x_axis_rotation,
                    large_arc_flag, sweep_flag, x, y);
    }
  else
    MgkDrawPrintf(context, " %.10g,%.10g %.10g %u %u %.10g,%.10g", rx, ry,
                  x_axis_rotation, large_arc_flag, sweep_flag, x, y);
}
MagickExport void MgkDrawPathEllipticArcAbsolute(MgkDrawContext context,
                                                 const double rx, const double ry,
                                                 const double x_axis_rotation,
                                                 unsigned int large_arc_flag,
                                                 unsigned int sweep_flag,
                                                 const double x, const double y)
{
  MgkDrawPathEllipticArc(context, AbsolutePathMode, rx, ry, x_axis_rotation,
                         large_arc_flag, sweep_flag, x, y);
}
MagickExport void MgkDrawPathEllipticArcRelative(MgkDrawContext context,
                                                 const double rx, const double ry,
                                                 const double x_axis_rotation,
                                                 unsigned int large_arc_flag,
                                                 unsigned int sweep_flag,
                                                 const double x, const double y)
{
  MgkDrawPathEllipticArc(context, RelativePathMode, rx, ry, x_axis_rotation,
                         large_arc_flag, sweep_flag, x, y);
}
MagickExport void MgkDrawPathFinish(MgkDrawContext context)
{
  MgkDrawPrintf(context, "\"\n");
  AccessContext(context)->path_operation = PathDefaultOperation;
  AccessContext(context)->path_mode = DefaultPathMode;
}
static void MgkDrawPathLineTo(MgkDrawContext context, const PathMode mode,
                              const double x, const double y)
{
  if ((AccessContext(context)->path_operation != PathLineToOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathLineToOperation;
      AccessContext(context)->path_mode = mode;
      MgkDrawPrintf(context, "%c%.10g,%.10g", mode == AbsolutePathMode ? 'L' : 'l', x,
                    y);
    }
  else
    MgkDrawPrintf(context, " %.10g,%.10g", x, y);
}
MagickExport void MgkDrawPathLineToAbsolute(MgkDrawContext context, const double x,
                                            const double y)
{
  MgkDrawPathLineTo(context, AbsolutePathMode, x, y);
}
MagickExport void MgkDrawPathLineToRelative(MgkDrawContext context, const double x,
                                            const double y)
{
  MgkDrawPathLineTo(context, RelativePathMode, x, y);
}
static void MgkDrawPathLineToHorizontal(MgkDrawContext context,
                                        const PathMode mode, const double x)
{
  if ((AccessContext(context)->path_operation != PathLineToHorizontalOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathLineToHorizontalOperation;
      AccessContext(context)->path_mode = mode;
      MgkDrawPrintf(context, "%c%.10g", mode == AbsolutePathMode ? 'H' : 'h', x);
    }
  else
    MgkDrawPrintf(context, " %.10g", x);
}
MagickExport void MgkDrawPathLineToHorizontalAbsolute(MgkDrawContext context,
                                                      const double x)
{
  MgkDrawPathLineToHorizontal(context, AbsolutePathMode, x);
}
MagickExport void MgkDrawPathLineToHorizontalRelative(MgkDrawContext context,
                                                      const double x)
{
  MgkDrawPathLineToHorizontal(context, RelativePathMode, x);
}
static void MgkDrawPathLineToVertical(MgkDrawContext context, const PathMode mode,
                                      const double y)
{
  if ((AccessContext(context)->path_operation != PathLineToVerticalOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathLineToVerticalOperation;
      AccessContext(context)->path_mode = mode;
      MgkDrawPrintf(context, "%c%.10g", mode == AbsolutePathMode ? 'V' : 'v', y);
    }
  else
    MgkDrawPrintf(context, " %.10g", y);
}
MagickExport void MgkDrawPathLineToVerticalAbsolute(MgkDrawContext context,
                                                    const double y)
{
  MgkDrawPathLineToVertical(context, AbsolutePathMode, y);
}
MagickExport void MgkDrawPathLineToVerticalRelative(MgkDrawContext context,
                                                    const double y)
{
  MgkDrawPathLineToVertical(context, RelativePathMode, y);
}
static void MgkDrawPathMoveTo(MgkDrawContext context, const PathMode mode,
                              const double x, const double y)
{
  if ((AccessContext(context)->path_operation != PathMoveToOperation)
      || (AccessContext(context)->path_mode != mode))
    {
      AccessContext(context)->path_operation = PathMoveToOperation;
      AccessContext(context)->path_mode = mode;
      MgkDrawPrintf(context, "%c%.10g,%.10g", mode == AbsolutePathMode ? 'M' : 'm', x,
                    y);
    }
  else
    MgkDrawPrintf(context, " %.10g,%.10g", x, y);
}
MagickExport void MgkDrawPathMoveToAbsolute(MgkDrawContext context, const double x,
                                            const double y)
{
  MgkDrawPathMoveTo(context, AbsolutePathMode, x, y);
}
MagickExport void MgkDrawPathMoveToRelative(MgkDrawContext context, const double x,
                                            const double y)
{
  MgkDrawPathMoveTo(context, RelativePathMode, x, y);
}
MagickExport void MgkDrawPathStart(MgkDrawContext context)
{
  MgkDrawPrintf(context, "path d=\"");
  AccessContext(context)->path_operation = PathDefaultOperation;
  AccessContext(context)->path_mode = DefaultPathMode;
}
MagickExport void MgkDrawPoint(MgkDrawContext context, const double x,
             const double y)
{
  MgkDrawPrintf(context, "point %.10g,%.10g\n", x, y);
}
MagickExport void MgkDrawPolygon(MgkDrawContext context, const size_t num_coords,
                                 const PointInfo * coordinates)
{
  const PointInfo * coordinate;

  size_t i;

  MgkDrawPrintf(context, "polygon");
  for (i = num_coords, coordinate = coordinates; i; i--)
    {
      MgkDrawPrintf(context, " %.10g,%.10g", coordinate->x, coordinate->y);
      ++coordinate;
    }
  MgkDrawPrintf(context, "\n");
}
MagickExport void MgkDrawPolyline(MgkDrawContext context, const size_t num_coords,
                                  const PointInfo * coordinates)
{
  const PointInfo * coordinate;

  size_t i;

  MgkDrawPrintf(context, "polyline");
  for (i = num_coords, coordinate = coordinates; i; i--)
    {
      MgkDrawPrintf(context, " %.10g,%.10g", coordinate->x, coordinate->y);
      ++coordinate;
    }
  MgkDrawPrintf(context, "\n");
}
MagickExport void MgkDrawPopClipPath(MgkDrawContext context)
{
  AccessContext(context)->indention_depth--;
  MgkDrawPrintf(context, "pop clip-path\n");
}
MagickExport void MgkDrawPopDefs(MgkDrawContext context)
{
  AccessContext(context)->indention_depth--;
  MgkDrawPrintf(context, "pop defs\n");
}
MagickExport void MgkDrawPopGraphicContext(MgkDrawContext context)
{
  AccessContext(context)->indention_depth--;
  MgkDrawPrintf(context, "pop graphic-context\n");
}
MagickExport void MgkDrawPopPattern(MgkDrawContext context)
{
  AccessContext(context)->indention_depth--;
  MgkDrawPrintf(context, "pop pattern\n");
}
MagickExport int MgkDrawPrintf(MgkDrawContext context, const char *format, ...)
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
MagickExport void MgkDrawPushClipPath(MgkDrawContext context,
                                      const char *clip_path_id)
{
  MgkDrawPrintf(context, "push clip-path %s\n", clip_path_id);
  AccessContext(context)->indention_depth++;
}
MagickExport void MgkDrawPushDefs(MgkDrawContext context)
{
  MgkDrawPrintf(context, "push defs\n");
  AccessContext(context)->indention_depth++;
}
MagickExport void MgkDrawPushGraphicContext(MgkDrawContext context)
{
  MgkDrawPrintf(context, "push graphic-context\n");
  AccessContext(context)->indention_depth++;
}
MagickExport void MgkDrawPushPattern(MgkDrawContext context,
                                     const char *pattern_id, const double x,
                                     const double y, const double width,
                                     const double height)
{
  MgkDrawPrintf(context, "push pattern %s %.10g,%.10g %.10g,%.10g\n",
    pattern_id, x, y, width, height);
  AccessContext(context)->indention_depth++;
}
MagickExport void MgkDrawRectangle(MgkDrawContext context, const double x1,
                                   const double y1, const double x2, const double y2)
{
  MgkDrawPrintf(context, "rectangle %.10g,%.10g %.10g,%.10g\n", x1, y1, x2, y2);
}
MagickExport int MgkDrawRender(Image * image, const ImageInfo * image_info,
                               const MgkDrawContext context)
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
MagickExport void MgkDrawSetRotate(MgkDrawContext context, const double degrees)
{
  MgkDrawPrintf(context, "rotate %.10g\n", degrees);
}
MagickExport void MgkDrawRoundRectangle(MgkDrawContext context,
                                        double x1, double y1,
                                        double x2, double y2, double rx, double ry)
{
  MgkDrawPrintf(context, "roundrectangle %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
                x1, y1, x2, y2, rx, ry);
}
MagickExport void MgkDrawSetScale(MgkDrawContext context, const double x,
                                  const double y)
{
  MgkDrawPrintf(context, "scale %.10g,%.10g\n", x, y);
}
MagickExport void MgkDrawSetSkewX(MgkDrawContext context, const double degrees)
{
  MgkDrawPrintf(context, "skewX %.10g\n", degrees);
}
MagickExport void MgkDrawSetSkewY(MgkDrawContext context, const double degrees)
{
  MgkDrawPrintf(context, "skewY %.10g\n", degrees);
}
MagickExport void MgkDrawSetStopColor(MgkDrawContext context,
                                      const PixelPacket * stop_color,
                                      const double offset)
{
  if (stop_color->opacity == OpaqueOpacity)
    MgkDrawPrintf(context,
#if QuantumDepth == 8
                  "stop-color #%02x%02x%02x %.10g\n",
#elif QuantumDepth == 16
                  "stop-color #%04x%04x%04x %.10g\n",
#endif
                  stop_color->red, stop_color->green, stop_color->blue, offset);
  else
    MgkDrawPrintf(context,
#if QuantumDepth == 8
                  "stop-color #%02x%02x%02x%02x %.10g\n",
#elif QuantumDepth == 16
                  "stop-color #%04x%04x%04x%04x %.10g\n",
#endif
                  stop_color->red, stop_color->green, stop_color->blue,
                  stop_color->opacity, offset);
}
MagickExport void MgkDrawSetStroke(MgkDrawContext context,
                                   const PixelPacket * stroke_color)
{
  if (stroke_color->opacity == OpaqueOpacity)
    MgkDrawPrintf(context,
#if QuantumDepth == 8
                  "stroke #%02x%02x%02x\n",
#elif QuantumDepth == 16
                  "stroke #%04x%04x%04x\n",
#endif
                  stroke_color->red, stroke_color->green, stroke_color->blue);
  else
    MgkDrawPrintf(context,
#if QuantumDepth == 8
                  "stroke #%02x%02x%02x%02x\n",
#elif QuantumDepth == 16
                  "stroke #%04x%04x%04x%04x\n",
#endif
                  stroke_color->red, stroke_color->green, stroke_color->blue,
                  stroke_color->opacity);

}
MagickExport void MgkDrawSetStrokeAntialias(MgkDrawContext context,
                                            const int true_false)
{
  MgkDrawPrintf(context, "stroke-antialias %i\n", true_false ? 1 : 0);
}
MagickExport void MgkDrawSetStrokeDashArray(MgkDrawContext context,
                                            const unsigned int *dasharray)
{
  register const unsigned int *p = dasharray;

  MgkDrawPrintf(context, "stroke-dasharray ");
  if ((p == (const unsigned int *) NULL) || (*p == 0))
    MgkDrawPrintf(context, "none");
  else
    {
      MgkDrawPrintf(context, "%u", *p++);
      while (*p)
        MgkDrawPrintf(context, ",%u", *p++);
    }
  MgkDrawPrintf(context, "\n");
}
MagickExport void MgkDrawSetStrokeDashOffset(MgkDrawContext context,
                                             const unsigned int dashoffset)
{
  MgkDrawPrintf(context, "stroke-dashoffset %u\n", dashoffset);
}
MagickExport void MgkDrawSetStrokeLineCap(MgkDrawContext context,
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
    }

  if (p != NULL)
    MgkDrawPrintf(context, "stroke-linecap %s\n", p);
}
MagickExport void MgkDrawSetStrokeLineJoin(MgkDrawContext context,
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
    }

  if (p != NULL)
    MgkDrawPrintf(context, "stroke-linejoin %s\n", p);
}
MagickExport void MgkDrawSetStrokeMiterLimit(MgkDrawContext context,
                                             const unsigned int miterlimit)
{
  MgkDrawPrintf(context, "stroke-miterlimit %u\n", miterlimit);
}
MagickExport void MgkDrawSetStrokeOpacity(MgkDrawContext context,
                                          const double opacity)
{
  MgkDrawPrintf(context, "stroke-opacity %.10g\n", opacity);
}
MagickExport void MgkDrawSetStrokeWidth(MgkDrawContext context, const double width)
{
  MgkDrawPrintf(context, "stroke-width %.10g\n", width);
}
MagickExport void MgkDrawText(MgkDrawContext context, const double x,
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

  MgkDrawPrintf(context, "text %.10g,%.10g '%.1024s'\n", escaped_string);
}
MagickExport void MgkDrawSetTextAntialias(MgkDrawContext context,
                                          const int true_false)
{
  MgkDrawPrintf(context, "text-antialias %i\n", true_false ? 1 : 0);
}
MagickExport void MgkDrawSetTextDecoration(MgkDrawContext context,
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
    MgkDrawPrintf(context, "decorate %s\n", p);
}
MagickExport void MgkDrawSetTextUnderColor(MgkDrawContext context,
                                           const PixelPacket * under_color)
{
  if (under_color->opacity == OpaqueOpacity)
    MgkDrawPrintf(context,
#if QuantumDepth == 8
                  "decorate #%02x%02x%02x\n",
#elif QuantumDepth == 16
                  "decorate #%04x%04x%04x\n",
#endif
                  under_color->red, under_color->green, under_color->blue);
  else
    MgkDrawPrintf(context,
#if QuantumDepth == 8
                  "decorate #%02x%02x%02x%02x\n",
#elif QuantumDepth == 16
                  "decorate #%04x%04x%04x%04x\n",
#endif
                  under_color->red, under_color->green, under_color->blue,
                  under_color->opacity);

}

MagickExport void MgkDrawSetTranslate(MgkDrawContext context, const double x,
                                      const double y)
{
  MgkDrawPrintf(context, "translate %.10g,%.10g\n", x, y);
}
MagickExport void MgkDrawSetViewbox(MgkDrawContext context,
                                    unsigned long x1, unsigned long y1,
                                    unsigned long x2, unsigned long y2)
{
  MgkDrawPrintf(context, "viewbox %lu %lu %lu %lu\n", x1, y1, x2, y2);
}
