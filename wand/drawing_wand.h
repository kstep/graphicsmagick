/*
  ImageMagick Drawing Wand API.
*/
#ifndef _MAGICK_DRAWING_WAND_H
#define _MAGICK_DRAWING_WAND_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include "wand/pixel_wand.h"

#define DestroyDrawingWand MagickDestroyDrawingWand
#define DrawAffine MagickDrawAffine
#define DrawAllocateWand MagickDrawAllocateWand
#define DrawAnnotation MagickDrawAnnotation
#define DrawArc MagickDrawArc
#define DrawBezier MagickDrawBezier
#define DrawCircle MagickDrawCircle
#define DrawColor MagickDrawColor
#define DrawComment MagickDrawComment
#define DrawComposite MagickDrawComposite
#define DrawEllipse MagickDrawEllipse
#define DrawGetClipPath MagickDrawGetClipPath
#define DrawGetClipRule MagickDrawGetClipRule
#define DrawGetClipUnits MagickDrawGetClipUnits
#define DrawGetFillColor MagickDrawGetFillColor
#define DrawGetFillOpacity MagickDrawGetFillOpacity
#define DrawGetFillRule MagickDrawGetFillRule
#define DrawGetFont MagickDrawGetFont
#define DrawGetFontFamily MagickDrawGetFontFamily
#define DrawGetFontSize MagickDrawGetFontSize
#define DrawGetFontStretch MagickDrawGetFontStretch
#define DrawGetFontStyle MagickDrawGetFontStyle
#define DrawGetFontWeight MagickDrawGetFontWeight
#define DrawGetGravity MagickDrawGetGravity
#define DrawGetStrokeAntialias MagickDrawGetStrokeAntialias
#define DrawGetStrokeColor MagickDrawGetStrokeColor
#define DrawGetStrokeDashArray MagickDrawGetStrokeDashArray
#define DrawGetStrokeDashOffset MagickDrawGetStrokeDashOffset
#define DrawGetStrokeLineCap MagickDrawGetStrokeLineCap
#define DrawGetStrokeLineJoin MagickDrawGetStrokeLineJoin
#define DrawGetStrokeMiterLimit MagickDrawGetStrokeMiterLimit
#define DrawGetStrokeOpacity MagickDrawGetStrokeOpacity
#define DrawGetStrokeWidth MagickDrawGetStrokeWidth
#define DrawGetTextAntialias MagickDrawGetTextAntialias
#define DrawGetTextDecoration MagickDrawGetTextDecoration
#define DrawGetTextEncoding MagickDrawGetTextEncoding
#define DrawGetTextUnderColor MagickDrawGetTextUnderColor
#define DrawLine MagickDrawLine
#define DrawMatte MagickDrawMatte
#define DrawPathClose MagickDrawPathClose
#define DrawPathCurveToAbsolute MagickDrawPathCurveToAbsolute
#define DrawPathCurveToQuadraticBezierAbsolute MagickDrawPathCurveToQuadraticBezierAbsolute
#define DrawPathCurveToQuadraticBezierRelative MagickDrawPathCurveToQuadraticBezierRelative
#define DrawPathCurveToQuadraticBezierSmoothAbsolute MagickDrawPathCurveToQuadraticBezierSmoothAbsolute
#define DrawPathCurveToQuadraticBezierSmoothRelative MagickDrawPathCurveToQuadraticBezierSmoothRelative
#define DrawPathCurveToRelative MagickDrawPathCurveToRelative
#define DrawPathCurveToSmoothAbsolute MagickDrawPathCurveToSmoothAbsolute
#define DrawPathCurveToSmoothRelative MagickDrawPathCurveToSmoothRelative
#define DrawPathEllipticArcAbsolute MagickDrawPathEllipticArcAbsolute
#define DrawPathEllipticArcRelative MagickDrawPathEllipticArcRelative
#define DrawPathFinish MagickDrawPathFinish
#define DrawPathLineToAbsolute MagickDrawPathLineToAbsolute
#define DrawPathLineToHorizontalAbsolute MagickDrawPathLineToHorizontalAbsolute
#define DrawPathLineToHorizontalRelative MagickDrawPathLineToHorizontalRelative
#define DrawPathLineToRelative MagickDrawPathLineToRelative
#define DrawPathLineToVerticalAbsolute MagickDrawPathLineToVerticalAbsolute
#define DrawPathLineToVerticalRelative MagickDrawPathLineToVerticalRelative
#define DrawPathMoveToAbsolute MagickDrawPathMoveToAbsolute
#define DrawPathMoveToRelative MagickDrawPathMoveToRelative
#define DrawPathStart MagickDrawPathStart
#define DrawPeekGraphicContext MagickDrawPeekGraphicContext
#define DrawPoint MagickDrawPoint
#define DrawPolygon MagickDrawPolygon
#define DrawPolyline MagickDrawPolyline
#define DrawPopClipPath MagickDrawPopClipPath
#define DrawPopDefs MagickDrawPopDefs
#define DrawPopGraphicContext MagickDrawPopGraphicContext
#define DrawPopPattern MagickDrawPopPattern
#define DrawPushClipPath MagickDrawPushClipPath
#define DrawPushDefs MagickDrawPushDefs
#define DrawPushGraphicContext MagickDrawPushGraphicContext
#define DrawPushPattern MagickDrawPushPattern
#define DrawRectangle MagickDrawRectangle
#define DrawRender MagickDrawRender
#define DrawRotate MagickDrawRotate
#define DrawRoundRectangle MagickDrawRoundRectangle
#define DrawScale MagickDrawScale
#define DrawSetClipPath MagickDrawSetClipPath
#define DrawSetClipRule MagickDrawSetClipRule
#define DrawSetClipUnits MagickDrawSetClipUnits
#define DrawSetFillColor MagickDrawSetFillColor
#define DrawSetFillOpacity MagickDrawSetFillOpacity
#define DrawSetFillPatternURL MagickDrawSetFillPatternURL
#define DrawSetFillRule MagickDrawSetFillRule
#define DrawSetFont MagickDrawSetFont
#define DrawSetFontFamily MagickDrawSetFontFamily
#define DrawSetFontSize MagickDrawSetFontSize
#define DrawSetFontStretch MagickDrawSetFontStretch
#define DrawSetFontStyle MagickDrawSetFontStyle
#define DrawSetFontWeight MagickDrawSetFontWeight
#define DrawSetGravity MagickDrawSetGravity
#define DrawSetStrokeAntialias MagickDrawSetStrokeAntialias
#define DrawSetStrokeColor MagickDrawSetStrokeColor
#define DrawSetStrokeDashArray MagickDrawSetStrokeDashArray
#define DrawSetStrokeDashOffset MagickDrawSetStrokeDashOffset
#define DrawSetStrokeLineCap MagickDrawSetStrokeLineCap
#define DrawSetStrokeLineJoin MagickDrawSetStrokeLineJoin
#define DrawSetStrokeMiterLimit MagickDrawSetStrokeMiterLimit
#define DrawSetStrokeOpacity MagickDrawSetStrokeOpacity
#define DrawSetStrokePatternURL MagickDrawSetStrokePatternURL
#define DrawSetStrokeWidth MagickDrawSetStrokeWidth
#define DrawSetTextAntialias MagickDrawSetTextAntialias
#define DrawSetTextDecoration MagickDrawSetTextDecoration
#define DrawSetTextEncoding MagickDrawSetTextEncoding
#define DrawSetTextUnderColor MagickDrawSetTextUnderColor
#define DrawSetViewbox MagickDrawSetViewbox
#define DrawSkewX MagickDrawSkewX
#define DrawSkewY MagickDrawSkewY
#define DrawTranslate MagickDrawTranslate
#define NewDrawingWand MagickNewDrawingWand

typedef struct _DrawingWand
/*  *DrawContext, */
  DrawingWand;

extern WandExport char
  *DrawGetClipPath(const DrawingWand *),
  *DrawGetFont(const DrawingWand *),
  *DrawGetFontFamily(const DrawingWand *),
  *DrawGetTextEncoding(const DrawingWand *);

extern WandExport ClipPathUnits
  DrawGetClipUnits(const DrawingWand *);

extern WandExport DecorationType
  DrawGetTextDecoration(const DrawingWand *);

extern WandExport double
  DrawGetFillOpacity(const DrawingWand *),
  DrawGetFontSize(const DrawingWand *),
  *DrawGetStrokeDashArray(const DrawingWand *,size_t *),
  DrawGetStrokeDashOffset(const DrawingWand *),
  DrawGetStrokeOpacity(const DrawingWand *),
  DrawGetStrokeWidth(const DrawingWand *);

extern WandExport DrawInfo
  *DrawPeekGraphicContext(const DrawingWand *);

extern WandExport DrawingWand
  *DrawAllocateWand(const DrawInfo *,Image *),
  *NewDrawingWand(void);

extern WandExport FillRule
  DrawGetClipRule(const DrawingWand *),
  DrawGetFillRule(const DrawingWand *);

extern WandExport GravityType
  DrawGetGravity(const DrawingWand *);

extern WandExport LineCap
  DrawGetStrokeLineCap(const DrawingWand *);

extern WandExport LineJoin
  DrawGetStrokeLineJoin(const DrawingWand *);

extern WandExport StretchType
  DrawGetFontStretch(const DrawingWand *);

extern WandExport StyleType
  DrawGetFontStyle(const DrawingWand *);

extern WandExport unsigned int
  DrawGetStrokeAntialias(const DrawingWand *),
  DrawGetTextAntialias(const DrawingWand *),
  DrawRender(const DrawingWand *);

extern WandExport unsigned long
  DrawGetFontWeight(const DrawingWand *),
  DrawGetStrokeMiterLimit(const DrawingWand *);

extern WandExport void
  DrawAffine(DrawingWand *,const AffineMatrix *),
  DrawAnnotation(DrawingWand *,const double,const double,const unsigned char *),
  DrawArc(DrawingWand *,const double,const double,const double,const double,
    const double,const double),
  DrawBezier(DrawingWand *,const size_t,const PointInfo *),
  DrawCircle(DrawingWand *,const double,const double,const double,const double),
  DrawColor(DrawingWand *,const double,const double,const PaintMethod),
  DrawComment(DrawingWand *,const char *),
  DestroyDrawingWand(DrawingWand *),
  DrawEllipse(DrawingWand *,const double,const double,const double,const double,
    const double,const double),
  DrawComposite(DrawingWand *,const CompositeOperator,const double,const double,
    const double,const double,const Image *),
  DrawGetFillColor(const DrawingWand *,PixelWand *),
  DrawGetStrokeColor(const DrawingWand *,PixelWand *),
  DrawGetTextUnderColor(const DrawingWand *,PixelWand *),
  DrawLine(DrawingWand *,const double, const double,const double,const double),
  DrawMatte(DrawingWand *,const double,const double,const PaintMethod),
  DrawPathClose(DrawingWand *),
  DrawPathCurveToAbsolute(DrawingWand *,const double,const double,const double,
    const double,const double,const double),
  DrawPathCurveToRelative(DrawingWand *,const double,const double,const double,
    const double,const double, const double),
  DrawPathCurveToQuadraticBezierAbsolute(DrawingWand *,const double,
    const double,const double,const double),
  DrawPathCurveToQuadraticBezierRelative(DrawingWand *,const double,
    const double,const double,const double),
  DrawPathCurveToQuadraticBezierSmoothAbsolute(DrawingWand *,const double,
    const double),
  DrawPathCurveToQuadraticBezierSmoothRelative(DrawingWand *,const double,
    const double),
  DrawPathCurveToSmoothAbsolute(DrawingWand *,const double,const double,
    const double,const double),
  DrawPathCurveToSmoothRelative(DrawingWand *,const double,const double,
    const double,const double),
  DrawPathEllipticArcAbsolute(DrawingWand *,const double,const double,
    const double,unsigned int,unsigned int,const double,const double),
  DrawPathEllipticArcRelative(DrawingWand *,const double,const double,
    const double,unsigned int,unsigned int,const double,const double),
  DrawPathFinish(DrawingWand *),
  DrawPathLineToAbsolute(DrawingWand *,const double,const double),
  DrawPathLineToRelative(DrawingWand *,const double,const double),
  DrawPathLineToHorizontalAbsolute(DrawingWand *,const double),
  DrawPathLineToHorizontalRelative(DrawingWand *,const double),
  DrawPathLineToVerticalAbsolute(DrawingWand *,const double),
  DrawPathLineToVerticalRelative(DrawingWand *,const double),
  DrawPathMoveToAbsolute(DrawingWand *,const double,const double),
  DrawPathMoveToRelative(DrawingWand *,const double,const double),
  DrawPathStart(DrawingWand *),
  DrawPoint(DrawingWand *,const double,const double),
  DrawPolygon(DrawingWand *,const size_t,const PointInfo *),
  DrawPolyline(DrawingWand *,const size_t,const PointInfo *),
  DrawPopClipPath(DrawingWand *),
  DrawPopDefs(DrawingWand *),
  DrawPopGraphicContext(DrawingWand *),
  DrawPopPattern(DrawingWand *),
  DrawPushClipPath(DrawingWand *,const char *),
  DrawPushDefs(DrawingWand *),
  DrawPushGraphicContext(DrawingWand *),
  DrawPushPattern(DrawingWand *,const char *,const double,const double,
    const double,const double),
  DrawRectangle(DrawingWand *,const double,const double,const double,
    const double),
  DrawRotate(DrawingWand *,const double),
  DrawRoundRectangle(DrawingWand *,double,double,double,double,double,double),
  DrawScale(DrawingWand *,const double,const double),
  DrawSetClipPath(DrawingWand *,const char *),
  DrawSetClipRule(DrawingWand *,const FillRule),
  DrawSetClipUnits(DrawingWand *,const ClipPathUnits),
  DrawSetFillColor(DrawingWand *,const PixelWand *),
  DrawSetFillOpacity(DrawingWand *,const double),
  DrawSetFillRule(DrawingWand *,const FillRule),
  DrawSetFillPatternURL(DrawingWand *,const char *),
  DrawSetFont(DrawingWand *,const char *),
  DrawSetFontFamily(DrawingWand *,const char *),
  DrawSetFontSize(DrawingWand *,const double),
  DrawSetFontStretch(DrawingWand *,const StretchType),
  DrawSetFontStyle(DrawingWand *,const StyleType),
  DrawSetFontWeight(DrawingWand *,const unsigned long),
  DrawSetGravity(DrawingWand *,const GravityType),
  DrawSkewX(DrawingWand *,const double),
  DrawSkewY(DrawingWand *,const double),
  DrawSetStrokeAntialias(DrawingWand *,const unsigned int),
  DrawSetStrokeColor(DrawingWand *,const PixelWand *),
  DrawSetStrokeDashArray(DrawingWand *,const size_t,const double *),
  DrawSetStrokeDashOffset(DrawingWand *,const double dashoffset),
  DrawSetStrokeLineCap(DrawingWand *,const LineCap),
  DrawSetStrokeLineJoin(DrawingWand *,const LineJoin),
  DrawSetStrokeMiterLimit(DrawingWand *,const unsigned long),
  DrawSetStrokeOpacity(DrawingWand *, const double),
  DrawSetStrokePatternURL(DrawingWand *,const char *),
  DrawSetStrokeWidth(DrawingWand *,const double),
  DrawSetTextAntialias(DrawingWand *,const unsigned int),
  DrawSetTextDecoration(DrawingWand *,const DecorationType),
  DrawSetTextEncoding(DrawingWand *,const char *),
  DrawSetTextUnderColor(DrawingWand *,const PixelWand *),
  DrawSetViewbox(DrawingWand *,unsigned long,unsigned long,unsigned long,
    unsigned long),
  DrawTranslate(DrawingWand *,const double,const double);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
