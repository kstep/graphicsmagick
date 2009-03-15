/*
  ImageMagick Drawing Wand API.
*/
#ifndef _MAGICK_DRAWING_WAND_H
#define _MAGICK_DRAWING_WAND_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include "wand/pixel_wand.h"

typedef struct _DrawingWand
  DrawingWand;

extern WandExport char
  *MagickDrawGetClipPath(const DrawingWand *),
  *MagickDrawGetFont(const DrawingWand *),
  *MagickDrawGetFontFamily(const DrawingWand *),
  *MagickDrawGetTextEncoding(const DrawingWand *);

extern WandExport ClipPathUnits
  MagickDrawGetClipUnits(const DrawingWand *);

extern WandExport DecorationType
  MagickDrawGetTextDecoration(const DrawingWand *);

extern WandExport double
  MagickDrawGetFillOpacity(const DrawingWand *),
  MagickDrawGetFontSize(const DrawingWand *),
  *MagickDrawGetStrokeDashArray(const DrawingWand *,unsigned long *),
  MagickDrawGetStrokeDashOffset(const DrawingWand *),
  MagickDrawGetStrokeOpacity(const DrawingWand *),
  MagickDrawGetStrokeWidth(const DrawingWand *);

extern WandExport DrawInfo
  *MagickDrawPeekGraphicContext(const DrawingWand *);

extern WandExport DrawingWand
  *MagickDrawAllocateWand(const DrawInfo *,Image *),
  *MagickNewDrawingWand(void);

extern WandExport FillRule
  MagickDrawGetClipRule(const DrawingWand *),
  MagickDrawGetFillRule(const DrawingWand *);

extern WandExport GravityType
  MagickDrawGetGravity(const DrawingWand *);

extern WandExport LineCap
  MagickDrawGetStrokeLineCap(const DrawingWand *);

extern WandExport LineJoin
  MagickDrawGetStrokeLineJoin(const DrawingWand *);

extern WandExport StretchType
  MagickDrawGetFontStretch(const DrawingWand *);

extern WandExport StyleType
  MagickDrawGetFontStyle(const DrawingWand *);

extern WandExport unsigned int
  MagickDrawGetStrokeAntialias(const DrawingWand *),
  MagickDrawGetTextAntialias(const DrawingWand *),
  MagickDrawRender(const DrawingWand *);

extern WandExport unsigned long
  MagickDrawGetFontWeight(const DrawingWand *),
  MagickDrawGetStrokeMiterLimit(const DrawingWand *);

extern WandExport void
  MagickDrawAffine(DrawingWand *,const AffineMatrix *),
  MagickDrawAnnotation(DrawingWand *,const double,const double,const unsigned char *),
  MagickDrawArc(DrawingWand *,const double,const double,const double,const double,
    const double,const double),
  MagickDrawBezier(DrawingWand *,const unsigned long,const PointInfo *),
  MagickDrawCircle(DrawingWand *,const double,const double,const double,const double),
  MagickDrawColor(DrawingWand *,const double,const double,const PaintMethod),
  MagickDrawComment(DrawingWand *,const char *),
  MagickDestroyDrawingWand(DrawingWand *),
  MagickDrawEllipse(DrawingWand *,const double,const double,const double,const double,
    const double,const double),
  MagickDrawComposite(DrawingWand *,const CompositeOperator,const double,const double,
    const double,const double,const Image *),
  MagickDrawGetFillColor(const DrawingWand *,PixelWand *),
  MagickDrawGetStrokeColor(const DrawingWand *,PixelWand *),
  MagickDrawGetTextUnderColor(const DrawingWand *,PixelWand *),
  MagickDrawLine(DrawingWand *,const double, const double,const double,const double),
  MagickDrawMatte(DrawingWand *,const double,const double,const PaintMethod),
  MagickDrawPathClose(DrawingWand *),
  MagickDrawPathCurveToAbsolute(DrawingWand *,const double,const double,const double,
    const double,const double,const double),
  MagickDrawPathCurveToRelative(DrawingWand *,const double,const double,const double,
    const double,const double, const double),
  MagickDrawPathCurveToQuadraticBezierAbsolute(DrawingWand *,const double,
    const double,const double,const double),
  MagickDrawPathCurveToQuadraticBezierRelative(DrawingWand *,const double,
    const double,const double,const double),
  MagickDrawPathCurveToQuadraticBezierSmoothAbsolute(DrawingWand *,const double,
    const double),
  MagickDrawPathCurveToQuadraticBezierSmoothRelative(DrawingWand *,const double,
    const double),
  MagickDrawPathCurveToSmoothAbsolute(DrawingWand *,const double,const double,
    const double,const double),
  MagickDrawPathCurveToSmoothRelative(DrawingWand *,const double,const double,
    const double,const double),
  MagickDrawPathEllipticArcAbsolute(DrawingWand *,const double,const double,
    const double,unsigned int,unsigned int,const double,const double),
  MagickDrawPathEllipticArcRelative(DrawingWand *,const double,const double,
    const double,unsigned int,unsigned int,const double,const double),
  MagickDrawPathFinish(DrawingWand *),
  MagickDrawPathLineToAbsolute(DrawingWand *,const double,const double),
  MagickDrawPathLineToRelative(DrawingWand *,const double,const double),
  MagickDrawPathLineToHorizontalAbsolute(DrawingWand *,const double),
  MagickDrawPathLineToHorizontalRelative(DrawingWand *,const double),
  MagickDrawPathLineToVerticalAbsolute(DrawingWand *,const double),
  MagickDrawPathLineToVerticalRelative(DrawingWand *,const double),
  MagickDrawPathMoveToAbsolute(DrawingWand *,const double,const double),
  MagickDrawPathMoveToRelative(DrawingWand *,const double,const double),
  MagickDrawPathStart(DrawingWand *),
  MagickDrawPoint(DrawingWand *,const double,const double),
  MagickDrawPolygon(DrawingWand *,const unsigned long,const PointInfo *),
  MagickDrawPolyline(DrawingWand *,const unsigned long,const PointInfo *),
  MagickDrawPopClipPath(DrawingWand *),
  MagickDrawPopDefs(DrawingWand *),
  MagickDrawPopGraphicContext(DrawingWand *),
  MagickDrawPopPattern(DrawingWand *),
  MagickDrawPushClipPath(DrawingWand *,const char *),
  MagickDrawPushDefs(DrawingWand *),
  MagickDrawPushGraphicContext(DrawingWand *),
  MagickDrawPushPattern(DrawingWand *,const char *,const double,const double,
    const double,const double),
  MagickDrawRectangle(DrawingWand *,const double,const double,const double,
    const double),
  MagickDrawRotate(DrawingWand *,const double),
  MagickDrawRoundRectangle(DrawingWand *,double,double,double,double,double,double),
  MagickDrawScale(DrawingWand *,const double,const double),
  MagickDrawSetClipPath(DrawingWand *,const char *),
  MagickDrawSetClipRule(DrawingWand *,const FillRule),
  MagickDrawSetClipUnits(DrawingWand *,const ClipPathUnits),
  MagickDrawSetFillColor(DrawingWand *,const PixelWand *),
  MagickDrawSetFillOpacity(DrawingWand *,const double),
  MagickDrawSetFillRule(DrawingWand *,const FillRule),
  MagickDrawSetFillPatternURL(DrawingWand *,const char *),
  MagickDrawSetFont(DrawingWand *,const char *),
  MagickDrawSetFontFamily(DrawingWand *,const char *),
  MagickDrawSetFontSize(DrawingWand *,const double),
  MagickDrawSetFontStretch(DrawingWand *,const StretchType),
  MagickDrawSetFontStyle(DrawingWand *,const StyleType),
  MagickDrawSetFontWeight(DrawingWand *,const unsigned long),
  MagickDrawSetGravity(DrawingWand *,const GravityType),
  MagickDrawSkewX(DrawingWand *,const double),
  MagickDrawSkewY(DrawingWand *,const double),
  MagickDrawSetStrokeAntialias(DrawingWand *,const unsigned int),
  MagickDrawSetStrokeColor(DrawingWand *,const PixelWand *),
  MagickDrawSetStrokeDashArray(DrawingWand *,const unsigned long,const double *),
  MagickDrawSetStrokeDashOffset(DrawingWand *,const double dashoffset),
  MagickDrawSetStrokeLineCap(DrawingWand *,const LineCap),
  MagickDrawSetStrokeLineJoin(DrawingWand *,const LineJoin),
  MagickDrawSetStrokeMiterLimit(DrawingWand *,const unsigned long),
  MagickDrawSetStrokeOpacity(DrawingWand *, const double),
  MagickDrawSetStrokePatternURL(DrawingWand *,const char *),
  MagickDrawSetStrokeWidth(DrawingWand *,const double),
  MagickDrawSetTextAntialias(DrawingWand *,const unsigned int),
  MagickDrawSetTextDecoration(DrawingWand *,const DecorationType),
  MagickDrawSetTextEncoding(DrawingWand *,const char *),
  MagickDrawSetTextUnderColor(DrawingWand *,const PixelWand *),
  MagickDrawSetViewbox(DrawingWand *,unsigned long,unsigned long,unsigned long,
    unsigned long),
  MagickDrawTranslate(DrawingWand *,const double,const double);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
