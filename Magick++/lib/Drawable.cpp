// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001, 2002
//
// Implementation of Drawable (Graphic objects)
//

#define MAGICK_IMPLEMENTATION

#include <math.h>
#include <string>

#include "Magick++/Drawable.h"
#include "Magick++/Image.h"

using namespace std;

//
// Explicit template instantiations
//
#if defined(EXPLICIT_TEMPLATE_INSTANTIATION)
template class std::list <Magick::Coordinate>;
// template class std::list<const Magick::Drawable>;
// template class std::list<const Magick::PathArcArgs>;
// template class std::list<const Magick::PathCurvetoArgs>;
// template class std::list<const Magick::PathQuadraticCurvetoArgs>;
// template class std::list<const Magick::VPath>;
#endif

int Magick::operator == ( const Magick::Coordinate& left_, const Magick::Coordinate& right_ )
{
  return ( ( left_.x() == right_.x() ) && ( left_.y() == right_.y() ) );
}
int Magick::operator != ( const Magick::Coordinate& left_, const Magick::Coordinate& right_ )
{
  return ( ! (left_ == right_) );
}
int Magick::operator >  ( const Magick::Coordinate& left_, const Magick::Coordinate& right_ )
{
  return ( !( left_ < right_ ) && ( left_ != right_ ) );
}
int Magick::operator <  ( const Magick::Coordinate& left_, const Magick::Coordinate& right_ )
{
  // Based on distance from origin
  return  ( (sqrt(left_.x()*left_.x() + left_.y()*left_.y())) <
            (sqrt(right_.x()*right_.x() + right_.y()*right_.y())) );
}
int Magick::operator >= ( const Magick::Coordinate& left_, const Magick::Coordinate& right_ )
{
  return ( ( left_ > right_ ) || ( left_ == right_ ) );
}
int Magick::operator <= ( const Magick::Coordinate& left_, const Magick::Coordinate& right_ )
{
  return ( ( left_ < right_ ) || ( left_ == right_ ) );
}

//
// Drawable Objects
//

// Affine (scaling, rotation, and translation)
void Magick::DrawableAffine::operator()( MagickLib::DrawContext context_ ) const
{
  AffineMatrix
    affine;

  affine.sx = _sx;
  affine.rx = _rx;
  affine.ry = _ry;
  affine.sy = _sy;
  affine.rx = _tx;
  affine.ty = _ty;
  DrawSetAffine( context_, &affine );
}

// Arc
void Magick::DrawableArc::operator()( MagickLib::DrawContext context_ ) const
{
  DrawArc( context_, _startX, _startY, _endX, _endY, _startDegrees, _endDegrees );
}

// Bezier curve (Coordinate list must contain at least three members)
Magick::DrawableBezier::DrawableBezier ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
void Magick::DrawableBezier::operator()( MagickLib::DrawContext context_ ) const
{
  size_t num_coords = _coordinates.size();
  PointInfo *coordinates = new PointInfo[num_coords];

  PointInfo *q = coordinates;
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  while( p != _coordinates.end() )
    {
      q->x = p->x();
      q->y = p->y();
      q++;
      p++;
    }

  DrawBezier( context_, num_coords, coordinates );
  delete [] coordinates;
}

// Circle
void Magick::DrawableCircle::operator()( MagickLib::DrawContext context_ ) const
{
  DrawCircle( context_, _originX, _originY, _perimX, _perimY );
}

// Colorize at point using PaintMethod
void Magick::DrawableColor::operator()( MagickLib::DrawContext context_ ) const
{
  DrawColor( context_, _x, _y, _paintMethod );
}

// Drawable Path
void Magick::DrawablePath::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPathStart( context_ );

  // FIXME, improve access syntax
  for( std::list<Magick::VPath>::const_iterator p = _path.begin();
       p != _path.end(); p++ )
    p->dp->operator()( context_ );

  DrawPathFinish( context_ );
}

// Decoration (text decoration)
void Magick::DrawableTextDecoration::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetTextDecoration( context_, _decoration );
}

// Ellipse
void Magick::DrawableEllipse::operator()( MagickLib::DrawContext context_ ) const
{
  DrawEllipse( context_, _originX, _originY, _radiusX, _radiusY, _arcStart, _arcEnd );
}

// Specify drawing fill color
void Magick::DrawableFillColor::operator()( MagickLib::DrawContext context_ ) const
{
  PixelPacket color = static_cast<PixelPacket>(_color);
  DrawSetFillColor( context_, &color );
}

// Specify drawing fill fule
void Magick::DrawableFillRule::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetFillRule( context_, _fillRule );
}

// Specify drawing fill opacity
void Magick::DrawableFillOpacity::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetFillOpacity( context_, _opacity );
}

// Specify text font
void Magick::DrawableFont::operator()( MagickLib::DrawContext context_ ) const
{
  // font
  if(_font.length())
    {
      DrawSetFont( context_, _font.c_str() );
    }

  if(_family.length())
    {
      // font-family
      DrawSetFontFamily( context_, _family.c_str() );

      // font-style
      DrawSetFontStyle( context_, _style );

      // font-weight
      DrawSetFontWeight( context_, _weight );

      // font-stretch
      DrawSetFontStretch( context_, _stretch );
    }
}

// Specify text positioning gravity
void Magick::DrawableGravity::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetGravity( context_, _gravity );
}

// Draw image at point
Magick::DrawableCompositeImage::DrawableCompositeImage ( double x_, double y_,
                                                         double width_, double height_,
                                                         const std::string &filename_,
                                                         Magick::CompositeOperator composition_ )
  : _composition(composition_),
    _x(x_),
    _y(y_),
    _width(width_),
    _height(height_),
    _image(new Image(filename_))
{
}
Magick::DrawableCompositeImage::DrawableCompositeImage ( double x_, double y_,
                                                         double width_, double height_,
                                                         const Image &image_,
                                                         CompositeOperator composition_ )
  : _composition(composition_),
    _x(x_),
    _y(y_),
    _width(width_),
    _height(height_),
    _image(new Image(image_))
{
}
Magick::DrawableCompositeImage::DrawableCompositeImage ( double x_, double y_,
                                                         double width_, double height_,
                                                         const std::string &filename_ )
  :_composition(CopyCompositeOp),
   _x(x_),
   _y(y_),
   _width(width_),
   _height(height_),
   _image(new Image(filename_))
{
}
Magick::DrawableCompositeImage::DrawableCompositeImage ( double x_, double y_,
                                                         double width_, double height_,
                                                         const Image &image_ )
  :_composition(CopyCompositeOp),
   _x(x_),
   _y(y_),
   _width(width_),
   _height(height_),
   _image(new Image(image_))
{
}
Magick::DrawableCompositeImage::DrawableCompositeImage ( double x_, double y_,
                                                         const std::string &filename_ )
  : _composition(CopyCompositeOp),
    _x(x_),
    _y(y_),
    _width(0),
    _height(0),
    _image(new Image(filename_))
{
  _width=_image->columns();
  _height=_image->rows();
}
Magick::DrawableCompositeImage::DrawableCompositeImage ( double x_, double y_,
                                                         const Image &image_ )
  : _composition(CopyCompositeOp),
    _x(x_),
    _y(y_),
    _width(0),
    _height(0),
    _image(new Image(image_))
{
  _width=_image->columns();
  _height=_image->rows();
}
Magick::DrawableCompositeImage::~DrawableCompositeImage( void )
{
  delete _image;
}
// Copy constructor
Magick::DrawableCompositeImage::DrawableCompositeImage ( const Magick::DrawableCompositeImage& original_ )
  :  Magick::DrawableBase(original_),
     _composition(original_._composition),
     _x(original_._x),
     _y(original_._y),
     _width(original_._width),
     _height(original_._height),
     _image(new Image(*original_._image))
{
}
// Assignment operator
Magick::DrawableCompositeImage& Magick::DrawableCompositeImage::operator= (const Magick::DrawableCompositeImage& original_ )
{
  // If not being set to ourself
  if ( this != &original_ )
    {
      _composition = original_._composition;
      _x = original_._x;
      _y = original_._y;
      _width = original_._width;
      _height = original_._height;
      delete _image;
      _image = new Image(*original_._image);
    }
  return *this;
}
void Magick::DrawableCompositeImage::filename( const std::string &filename_ )
{
  delete _image;
  _image = new Image(filename_);
}
std::string Magick::DrawableCompositeImage::filename( void ) const
{
  return _image->fileName();
}

void Magick::DrawableCompositeImage::image( const Image &image_ )
{
  delete _image;
  _image = new Image(image_);
}
Magick::Image Magick::DrawableCompositeImage::image( void ) const
{
  return *_image;
}

// Specify image format used to output Base64 inlined image data.
void Magick::DrawableCompositeImage::magick( std::string magick_ )
{
  _image->magick( magick_ );
}
std::string Magick::DrawableCompositeImage::magick( void )
{
  return _image->magick();
}

void Magick::DrawableCompositeImage::operator()( MagickLib::DrawContext context_ ) const
{
  DrawComposite( context_, _composition, _x, _y, _width, _height, _image->constImage() );
}

// Line
void Magick::DrawableLine::operator()( MagickLib::DrawContext context_ ) const
{
  DrawLine( context_, _startX, _startY, _endX, _endY );
}

// Change pixel matte value to transparent using PaintMethod
void Magick::DrawableMatte::operator()( MagickLib::DrawContext context_ ) const
{
  DrawMatte( context_, _x, _y, _paintMethod );
}

// Point
void Magick::DrawablePoint::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPoint( context_, _x, _y );
}

// Text pointsize
void Magick::DrawablePointSize::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetFontSize( context_, _pointSize );
}

// Polygon (Coordinate list must contain at least three members)
Magick::DrawablePolygon::DrawablePolygon ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
void Magick::DrawablePolygon::operator()( MagickLib::DrawContext context_ ) const
{
  size_t num_coords = _coordinates.size();
  PointInfo *coordinates = new PointInfo[num_coords];

  PointInfo *q = coordinates;
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  while( p != _coordinates.end() )
    {
      q->x = p->x();
      q->y = p->y();
      q++;
      p++;
    }

  DrawPolygon( context_, num_coords, coordinates );
  delete [] coordinates;
}

// Polyline (Coordinate list must contain at least three members)
Magick::DrawablePolyline::DrawablePolyline ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
void Magick::DrawablePolyline::operator()( MagickLib::DrawContext context_ ) const
{
  size_t num_coords = _coordinates.size();
  PointInfo *coordinates = new PointInfo[num_coords];

  PointInfo *q = coordinates;
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  while( p != _coordinates.end() )
    {
      q->x = p->x();
      q->y = p->y();
      q++;
      p++;
    }

  DrawPolyline( context_, num_coords, coordinates );
  delete [] coordinates;
}

// Pop Graphic Context
void Magick::DrawablePopGraphicContext::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPopGraphicContext( context_ );
}

// Push Graphic Context
void Magick::DrawablePushGraphicContext::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPushGraphicContext( context_ );
}

// Pop (terminate) Pattern definition
void Magick::DrawablePopPattern::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPopPattern( context_ );
}

// Push Pattern definition
void Magick::DrawablePushPattern::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPushPattern( context_, _id.c_str(), _x, _y, _width, _height );
}

// Rectangle
void Magick::DrawableRectangle::operator()( MagickLib::DrawContext context_ ) const
{
  DrawRectangle( context_, _upperLeftX, _upperLeftY, _lowerRightX, _lowerRightY );
}

// Apply Rotation
void Magick::DrawableRotation::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetRotate( context_, _angle );
}

// Round Rectangle
void Magick::DrawableRoundRectangle::operator()( MagickLib::DrawContext context_ ) const
{
  DrawRoundRectangle( context_, _centerX,_centerY, _width,_hight,
                      _cornerWidth, _cornerHeight);
}

// Apply Scaling
void Magick::DrawableScaling::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetScale( context_, _x, _y );
}

// Apply Skew in the X direction
void Magick::DrawableSkewX::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetSkewX( context_, _angle );
}

// Apply Skew in the Y direction
void Magick::DrawableSkewY::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetSkewY( context_, _angle );
}

// Stroke dasharray
Magick::DrawableDashArray::DrawableDashArray( const double* dasharray_ )
  : _dasharray(0)
{
  dasharray( dasharray_ );
}
// Deprecated, do not use for new code, and migrate existing code to using double*
Magick::DrawableDashArray::DrawableDashArray( const unsigned int* dasharray_ )
  : _dasharray(0)
{
  dasharray( dasharray_ );
}
Magick::DrawableDashArray::DrawableDashArray(const Magick::DrawableDashArray& original_)
  : _dasharray(0)
{
  dasharray( original_._dasharray );
}
Magick::DrawableDashArray& Magick::DrawableDashArray::operator=(const Magick::DrawableDashArray &original_)
{
  if( this != &original_ )
    {
      dasharray( original_._dasharray );
    }
  return *this;
}
void Magick::DrawableDashArray::dasharray ( const double* dasharray_ )
{
  LiberateMemory(reinterpret_cast<void**>(&_dasharray));

  if(dasharray_)
    {
      // Count elements in dash array
      unsigned int n = 0;
      {
        const double *p = dasharray_;
        while(*p++ != 0)
          n++;
      }

      // Allocate elements
      _dasharray=static_cast<double*>(AcquireMemory((n+1)*sizeof(double)));
      // Copy elements
      {
        double *q = _dasharray;
        const double *p = dasharray_;
        while( *p )
          *q++=*p++;
        *q=0;
      }
    }
}
// This method is deprecated.  Don't use for new code, and migrate existing
// code to the const double* version.
void Magick::DrawableDashArray::dasharray( const unsigned int* dasharray_ )
{
  LiberateMemory(reinterpret_cast<void**>(&_dasharray));

  if(dasharray_)
    {
      // Count elements in dash array
      unsigned int n = 0;
      {
        const unsigned int *p = dasharray_;
        while(*p++ != 0)
          n++;
      }

      // Allocate elements
      _dasharray=static_cast<double*>(AcquireMemory((n+1)*sizeof(double)));
      // Copy elements
      {
        double *q = _dasharray;
        const unsigned int *p = dasharray_;
        while( *p )
          *q++=static_cast<double>(*p++);
        *q=0;
      }
    }
}
void Magick::DrawableDashArray::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetStrokeDashArray( context_, _dasharray );
}

// Stroke dashoffset
void Magick::DrawableDashOffset::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetStrokeDashOffset( context_, _offset );
}

// Stroke linecap
void Magick::DrawableStrokeLineCap::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetStrokeLineCap( context_, _linecap );
}

// Stroke linejoin
void Magick::DrawableStrokeLineJoin::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetStrokeLineJoin( context_, _linejoin );
}

// Stroke miterlimit
void Magick::DrawableMiterLimit::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetStrokeMiterLimit( context_, _miterlimit );
}

// Stroke color
void Magick::DrawableStrokeColor::operator()( MagickLib::DrawContext context_ ) const
{
  PixelPacket color = static_cast<PixelPacket>(_color);
  DrawSetStrokeColor( context_, &color );
}

// Stroke antialias
void Magick::DrawableStrokeAntialias::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetStrokeAntialias( context_, static_cast<int>(_flag) );
}

// Stroke opacity
void Magick::DrawableStrokeOpacity::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetStrokeOpacity( context_, _opacity );
}

// Stroke width
void Magick::DrawableStrokeWidth::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetStrokeWidth( context_, _width );
}

// Draw text at point
void Magick::DrawableText::operator()( MagickLib::DrawContext context_ ) const
{
  DrawAnnotation( context_, _x, _y, reinterpret_cast<const unsigned char*>(_text.c_str()) );
}

// Apply Translation
void Magick::DrawableTranslation::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetTranslate( context_, _x, _y );
}

// Text antialias
void Magick::DrawableTextAntialias::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetTextAntialias( context_, static_cast<int>(_flag) );
}

// Set the size of the viewbox
void Magick::DrawableViewbox::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetViewbox( context_, _x1, _y1, _x2, _y2 );
}

//
// Path Classes
//

// Path Arc
void Magick::PathArcAbs::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::PathArcArgs>::const_iterator p = _coordinates.begin();
       p != _coordinates.end(); p++ )
    {
      DrawPathEllipticArcAbsolute( context_, p->radiusX(), p->radiusY(),
                                   p->xAxisRotation(), p->largeArcFlag(),
                                   p->sweepFlag(), p->x(), p->y() );
    }
}
void Magick::PathArcRel::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::PathArcArgs>::const_iterator p = _coordinates.begin();
       p != _coordinates.end(); p++ )
    {
      DrawPathEllipticArcRelative( context_, p->radiusX(), p->radiusY(),
                                   p->xAxisRotation(), p->largeArcFlag(),
                                   p->sweepFlag(), p->x(), p->y() );
    }
}

//
// Path Closepath
//
void Magick::PathClosePath::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPathClose( context_ );
}

//
// Path Curveto (Cubic Bezier)
//
void Magick::PathCurvetoAbs::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::PathCurvetoArgs>::const_iterator p = _args.begin();
       p != _args.end(); p++ )
    {
      DrawPathCurveToAbsolute( context_, p->x1(), p->y1(), p->x2(), p->y2(),
                               p->x(), p->y() );
    }
}
void Magick::PathCurvetoRel::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::PathCurvetoArgs>::const_iterator p = _args.begin();
       p != _args.end(); p++ )
    {
      DrawPathCurveToRelative( context_, p->x1(), p->y1(), p->x2(), p->y2(),
                               p->x(), p->y() );
    }
}
void Magick::PathSmoothCurvetoAbs::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
       p != _coordinates.end(); p++ )
    {
      double x2 = p->x();
      double y2 = p->y();
      p++;
      if(p != _coordinates.end() )
        DrawPathCurveToSmoothAbsolute( context_, x2, y2, p->x(), p->y() );
    }
}
void Magick::PathSmoothCurvetoRel::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
       p != _coordinates.end(); p++ )
    {
      double x2 = p->x();
      double y2 = p->y();
      p++;
      if(p != _coordinates.end() )
        DrawPathCurveToSmoothRelative( context_, x2, y2, p->x(), p->y() );
    }
}

//
// Quadratic Curveto (Quadratic Bezier)
//
void Magick::PathQuadraticCurvetoAbs::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::PathQuadraticCurvetoArgs>::const_iterator p = _args.begin();
       p != _args.end(); p++ )
    {
      DrawPathCurveToQuadraticBezierAbsolute( context_, p->x1(), p->y1(), p->x(), p->y() );
    }
}
void Magick::PathQuadraticCurvetoRel::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::PathQuadraticCurvetoArgs>::const_iterator p = _args.begin();
       p != _args.end(); p++ )
    {
      DrawPathCurveToQuadraticBezierRelative( context_, p->x1(), p->y1(), p->x(), p->y() );
    }
}
void Magick::PathSmoothQuadraticCurvetoAbs::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
       p != _coordinates.end(); p++ )
    {
      DrawPathCurveToQuadraticBezierSmoothAbsolute( context_, p->x(), p->y() );
    }
}
void Magick::PathSmoothQuadraticCurvetoRel::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
       p != _coordinates.end(); p++ )
    {
      DrawPathCurveToQuadraticBezierSmoothRelative( context_, p->x(), p->y() );
    }
}

//
// Path Lineto
//

void Magick::PathLinetoAbs::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
       p != _coordinates.end(); p++ )
    {
      DrawPathLineToAbsolute( context_, p->x(), p->y() );
    }
}
void Magick::PathLinetoRel::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
       p != _coordinates.end(); p++ )
    {
      DrawPathLineToRelative( context_, p->x(), p->y() );
    }
}

//
// Path Horizontal Lineto
//

void Magick::PathLinetoHorizontalAbs::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPathLineToHorizontalAbsolute( context_, _x );
}
void Magick::PathLinetoHorizontalRel::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPathLineToHorizontalRelative( context_, _x );
}

//
// Path Vertical Lineto
//

void Magick::PathLinetoVerticalAbs::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPathLineToVerticalAbsolute( context_, _y );
}
void Magick::PathLinetoVerticalRel::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPathLineToVerticalRelative( context_, _y );
}

//
// Path Moveto
//

void Magick::PathMovetoAbs::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
       p != _coordinates.end(); p++ )
    {
      DrawPathMoveToAbsolute( context_, p->x(), p->y() );
    }
}
void Magick::PathMovetoRel::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
       p != _coordinates.end(); p++ )
    {
      DrawPathMoveToRelative( context_, p->x(), p->y() );
    }
}
