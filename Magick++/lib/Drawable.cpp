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

// Constructor
Magick::Drawable::Drawable ( void )
  : dp(0)
{
}

// Construct from DrawableBase
Magick::Drawable::Drawable ( const Magick::DrawableBase& original_ )
  : dp(original_.copy())
{
}

// Destructor
Magick::Drawable::~Drawable ( void )
{
  delete dp;
  dp = 0;
}

// Copy constructor
Magick::Drawable::Drawable ( const Magick::Drawable& original_ )
  : dp(original_.dp? original_.dp->copy(): 0)
{
}

// Assignment operator
Magick::Drawable& Magick::Drawable::operator= (const Magick::Drawable& original_ )
{
  if (this != &original_)
    {
      delete dp;
      dp = (original_.dp ? original_.dp->copy() : 0);
    }
  return *this;
}

// Operator to invoke contained object
void Magick::Drawable::operator()( MagickLib::DrawContext context_ ) const
{
  if(dp)
    dp->operator()( context_ );
}

// Constructor
Magick::VPath::VPath ( void )
  : dp(0)
{
}

// Construct from VPathBase
Magick::VPath::VPath ( const Magick::VPathBase& original_ )
  : dp(original_.copy())
{
}

// Destructor
/* virtual */ Magick::VPath::~VPath ( void )
{
  delete dp;
  dp = 0;
}

// Copy constructor
Magick::VPath::VPath ( const Magick::VPath& original_ )
  : dp(original_.dp? original_.dp->copy(): 0)
{
}

// Assignment operator
Magick::VPath& Magick::VPath::operator= (const Magick::VPath& original_ )
{
  if (this != &original_)
    {
      delete dp;
      dp = (original_.dp ? original_.dp->copy() : 0);
    }
  return *this;
}

// Operator to invoke contained object
void Magick::VPath::operator()( MagickLib::DrawContext context_ ) const
{
  if(dp)
    dp->operator()( context_ );
}

//
// Drawable Objects
//

// Affine (scaling, rotation, and translation)
Magick::DrawableAffine::~DrawableAffine( void )
{
}
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
Magick::DrawableArc::~DrawableArc( void )
{
}
void Magick::DrawableArc::operator()( MagickLib::DrawContext context_ ) const
{
  DrawArc( context_, _startX, _startY, _endX, _endY, _startDegrees, _endDegrees );
}

//
// Bezier curve
//
// Construct from coordinates (Coordinate list must contain at least three members)
Magick::DrawableBezier::DrawableBezier ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
// Copy constructor
Magick::DrawableBezier::DrawableBezier( const Magick::DrawableBezier& original_ )
  : _coordinates(original_._coordinates)
{
}
// Destructor
Magick::DrawableBezier::~DrawableBezier( void )
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
Magick::DrawableCircle::~DrawableCircle ( void )
{
}
void Magick::DrawableCircle::operator()( MagickLib::DrawContext context_ ) const
{
  DrawCircle( context_, _originX, _originY, _perimX, _perimY );
}

// Colorize at point using PaintMethod
Magick::DrawableColor::~DrawableColor( void )
{
}
void Magick::DrawableColor::operator()( MagickLib::DrawContext context_ ) const
{
  DrawColor( context_, _x, _y, _paintMethod );
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
                                                         const Magick::Image &image_,
                                                         Magick::CompositeOperator composition_ )
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
                                                         const Magick::Image &image_ )
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
                                                         const Magick::Image &image_ )
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

void Magick::DrawableCompositeImage::image( const Magick::Image &image_ )
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

// Drawable Path
void Magick::DrawablePath::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPathStart( context_ );

  for( std::list<Magick::VPath>::const_iterator p = _path.begin();
       p != _path.end(); p++ )
    p->operator()( context_ );

  DrawPathFinish( context_ );
}

// Ellipse
Magick::DrawableEllipse::~DrawableEllipse( void )
{
}
void Magick::DrawableEllipse::operator()( MagickLib::DrawContext context_ ) const
{
  DrawEllipse( context_, _originX, _originY, _radiusX, _radiusY, _arcStart, _arcEnd );
}

// Specify drawing fill color
Magick::DrawableFillColor::DrawableFillColor( const Magick::Color &color_ )
  : _color(color_)
{
}
Magick::DrawableFillColor::DrawableFillColor( const Magick::DrawableFillColor& original_ )
  : _color(original_._color)
{
}
Magick::DrawableFillColor::~DrawableFillColor( void )
{
}
void Magick::DrawableFillColor::operator()( MagickLib::DrawContext context_ ) const
{
  PixelPacket color = static_cast<PixelPacket>(_color);
  DrawSetFillColor( context_, &color );
}

// Specify drawing fill fule
Magick::DrawableFillRule::~DrawableFillRule ( void )
{
}
void Magick::DrawableFillRule::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetFillRule( context_, _fillRule );
}

// Specify drawing fill opacity
Magick::DrawableFillOpacity::~DrawableFillOpacity ( void )
{
}
void Magick::DrawableFillOpacity::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetFillOpacity( context_, _opacity );
}

// Specify text font
Magick::DrawableFont::~DrawableFont ( void )
{
}
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

// Specify text font
Magick::DrawableFont::DrawableFont ( const std::string &font_ )
  : _font(font_),
    _family(),
    _style(Magick::AnyStyle),
    _weight(400),
    _stretch(Magick::NormalStretch)
{
}
Magick::DrawableFont::DrawableFont ( const std::string &family_,
                                     Magick::StyleType style_,
                                     const unsigned long weight_,
                                     Magick::StretchType stretch_ )
  : _font(),
    _family(family_),
    _style(style_),
    _weight(weight_),
    _stretch(stretch_)
{
}
Magick::DrawableFont::DrawableFont ( const Magick::DrawableFont& original_ )
  : _font(original_._font),
    _family(original_._family),
    _style(original_._style),
    _weight(original_._weight),
    _stretch(original_._stretch)
{
}

// Specify text positioning gravity
Magick::DrawableGravity::~DrawableGravity ( void )
{
}
void Magick::DrawableGravity::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetGravity( context_, _gravity );
}

// Line
Magick::DrawableLine::~DrawableLine ( void )
{
}
void Magick::DrawableLine::operator()( MagickLib::DrawContext context_ ) const
{
  DrawLine( context_, _startX, _startY, _endX, _endY );
}

// Change pixel matte value to transparent using PaintMethod
Magick::DrawableMatte::~DrawableMatte ( void )
{
}
void Magick::DrawableMatte::operator()( MagickLib::DrawContext context_ ) const
{
  DrawMatte( context_, _x, _y, _paintMethod );
}

// Drawable Path
Magick::DrawablePath::DrawablePath ( const std::list<Magick::VPath> &path_ )
  : _path(path_)
{
}
Magick::DrawablePath::DrawablePath ( const Magick::DrawablePath& original_ )
  : _path(original_._path)
{
}
Magick::DrawablePath::~DrawablePath ( void )
{
}

// Point
Magick::DrawablePoint::~DrawablePoint ( void )
{
}
void Magick::DrawablePoint::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPoint( context_, _x, _y );
}

// Text pointsize
Magick::DrawablePointSize::~DrawablePointSize ( void )
{
}
void Magick::DrawablePointSize::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetFontSize( context_, _pointSize );
}

// Polygon (Coordinate list must contain at least three members)
Magick::DrawablePolygon::DrawablePolygon ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
Magick::DrawablePolygon::DrawablePolygon ( const Magick::DrawablePolygon& original_ )
  : _coordinates(original_._coordinates)
{
}
Magick::DrawablePolygon::~DrawablePolygon ( void )
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
Magick::DrawablePolyline::DrawablePolyline ( const Magick::DrawablePolyline& original_ )
  : _coordinates(original_._coordinates)
{
}
Magick::DrawablePolyline::~DrawablePolyline ( void )
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
Magick::DrawablePopGraphicContext::~DrawablePopGraphicContext ( void )
{
}
void Magick::DrawablePopGraphicContext::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPopGraphicContext( context_ );
}

// Push Graphic Context
Magick::DrawablePushGraphicContext::~DrawablePushGraphicContext ( void )
{
}
void Magick::DrawablePushGraphicContext::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPushGraphicContext( context_ );
}

// Pop (terminate) Pattern definition
Magick::DrawablePopPattern::~DrawablePopPattern ( void )
{
}
void Magick::DrawablePopPattern::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPopPattern( context_ );
}

// Push Pattern definition
Magick::DrawablePushPattern::DrawablePushPattern ( const std::string &id_, long x_, long y_,
                                                   long width_, long height_ )
  : _id(id_),
    _x(x_),
    _y(y_),
    _width(width_),
    _height(height_)
{
}
Magick::DrawablePushPattern::DrawablePushPattern ( const Magick::DrawablePushPattern& original_ )
  : _id(original_._id),
    _x(original_._x),
    _y(original_._y),
    _width(original_._width),
    _height(original_._height)
{
}
Magick::DrawablePushPattern::~DrawablePushPattern ( void )
{
}
void Magick::DrawablePushPattern::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPushPattern( context_, _id.c_str(), _x, _y, _width, _height );
}

// Rectangle
Magick::DrawableRectangle::~DrawableRectangle ( void )
{
}
void Magick::DrawableRectangle::operator()( MagickLib::DrawContext context_ ) const
{
  DrawRectangle( context_, _upperLeftX, _upperLeftY, _lowerRightX, _lowerRightY );
}

// Apply Rotation
Magick::DrawableRotation::~DrawableRotation ( void )
{
}
void Magick::DrawableRotation::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetRotate( context_, _angle );
}

// Round Rectangle
Magick::DrawableRoundRectangle::~DrawableRoundRectangle ( void )
{
}
void Magick::DrawableRoundRectangle::operator()( MagickLib::DrawContext context_ ) const
{
  DrawRoundRectangle( context_, _centerX,_centerY, _width,_hight,
                      _cornerWidth, _cornerHeight);
}

// Apply Scaling
Magick::DrawableScaling::~DrawableScaling ( void )
{
}
void Magick::DrawableScaling::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetScale( context_, _x, _y );
}

// Apply Skew in the X direction
Magick::DrawableSkewX::~DrawableSkewX ( void )
{
}
void Magick::DrawableSkewX::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetSkewX( context_, _angle );
}

// Apply Skew in the Y direction
Magick::DrawableSkewY::~DrawableSkewY ( void )
{
}
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
Magick::DrawableDashArray::~DrawableDashArray( void )
{
  delete _dasharray;
  _dasharray = 0;
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
Magick::DrawableDashOffset::~DrawableDashOffset ( void )
{
}
void Magick::DrawableDashOffset::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetStrokeDashOffset( context_, _offset );
}

// Stroke linecap
Magick::DrawableStrokeLineCap::~DrawableStrokeLineCap ( void )
{
}
void Magick::DrawableStrokeLineCap::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetStrokeLineCap( context_, _linecap );
}

// Stroke linejoin
Magick::DrawableStrokeLineJoin::~DrawableStrokeLineJoin ( void )
{
}
void Magick::DrawableStrokeLineJoin::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetStrokeLineJoin( context_, _linejoin );
}

// Stroke miterlimit
Magick::DrawableMiterLimit::~DrawableMiterLimit ( void )
{
}
void Magick::DrawableMiterLimit::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetStrokeMiterLimit( context_, _miterlimit );
}

// Stroke antialias
Magick::DrawableStrokeAntialias::~DrawableStrokeAntialias ( void )
{
}
void Magick::DrawableStrokeAntialias::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetStrokeAntialias( context_, static_cast<int>(_flag) );
}

// Stroke color
Magick::DrawableStrokeColor::DrawableStrokeColor ( const Magick::Color &color_ )
  : _color(color_)
{
}
Magick::DrawableStrokeColor::DrawableStrokeColor ( const Magick::DrawableStrokeColor& original_ )
  : _color(original_._color)
{
}
Magick::DrawableStrokeColor::~DrawableStrokeColor ( void )
{
}
void Magick::DrawableStrokeColor::operator()( MagickLib::DrawContext context_ ) const
{
  PixelPacket color = static_cast<PixelPacket>(_color);
  DrawSetStrokeColor( context_, &color );
}

// Stroke opacity
Magick::DrawableStrokeOpacity::~DrawableStrokeOpacity ( void )
{
}
void Magick::DrawableStrokeOpacity::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetStrokeOpacity( context_, _opacity );
}

// Stroke width
Magick::DrawableStrokeWidth::~DrawableStrokeWidth ( void )
{
}
void Magick::DrawableStrokeWidth::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetStrokeWidth( context_, _width );
}

// Draw text at point
Magick::DrawableText::DrawableText ( const double x_, const double y_,
                                     const std::string &text_ )
  : _x(x_),
    _y(y_),
    _text(text_)
{
}
Magick::DrawableText::DrawableText( const Magick::DrawableText& original_ )
  : _x(original_._x),
    _y(original_._y),
    _text(original_._text)
{
}
Magick::DrawableText::~DrawableText ( void )
{
}
void Magick::DrawableText::operator()( MagickLib::DrawContext context_ ) const
{
  DrawAnnotation( context_, _x, _y, reinterpret_cast<const unsigned char*>(_text.c_str()) );
}

// Text antialias
Magick::DrawableTextAntialias::~DrawableTextAntialias ( void )
{
}
void Magick::DrawableTextAntialias::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetTextAntialias( context_, static_cast<int>(_flag) );
}

// Decoration (text decoration)
Magick::DrawableTextDecoration::~DrawableTextDecoration( void )
{
}
void Magick::DrawableTextDecoration::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetTextDecoration( context_, _decoration );
}

// Apply Translation
Magick::DrawableTranslation::~DrawableTranslation ( void )
{
}
void Magick::DrawableTranslation::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetTranslate( context_, _x, _y );
}

// Set the size of the viewbox
Magick::DrawableViewbox::~DrawableViewbox ( void )
{
}
void Magick::DrawableViewbox::operator()( MagickLib::DrawContext context_ ) const
{
  DrawSetViewbox( context_, _x1, _y1, _x2, _y2 );
}

//
// Path Classes
//

// Path Arc
Magick::PathArcAbs::PathArcAbs ( const Magick::PathArcArgs &coordinates_ )
  : _coordinates(1,coordinates_)
{
}
Magick::PathArcAbs::PathArcAbs ( const std::list<Magick::PathArcArgs> &coordinates_ )
  : _coordinates(coordinates_)
{
}
Magick::PathArcAbs::PathArcAbs ( const Magick::PathArcAbs& original_ )
  : _coordinates(original_._coordinates)
{
}
Magick::PathArcAbs::~PathArcAbs ( void )
{
}
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

Magick::PathArcRel::PathArcRel ( const Magick::PathArcArgs &coordinates_ )
  : _coordinates(1,coordinates_)
{
}
Magick::PathArcRel::PathArcRel ( const std::list<Magick::PathArcArgs> &coordinates_ )
  : _coordinates(coordinates_)
{
}
Magick::PathArcRel::PathArcRel ( const Magick::PathArcRel& original_ )
  : _coordinates(original_._coordinates)
{
}
Magick::PathArcRel::~PathArcRel ( void )
{
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
Magick::PathClosePath::~PathClosePath ( void )
{
}
void Magick::PathClosePath::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPathClose( context_ );
}

//
// Path Curveto (Cubic Bezier)
//
Magick::PathCurvetoAbs::PathCurvetoAbs ( const Magick::PathCurvetoArgs &args_ )
  : _args(1,args_)
{
}
Magick::PathCurvetoAbs::PathCurvetoAbs ( const std::list<Magick::PathCurvetoArgs> &args_ )
  : _args(args_)
{
}
Magick::PathCurvetoAbs::PathCurvetoAbs ( const Magick::PathCurvetoAbs& original_ )
 : _args(original_._args)
{
}
Magick::PathCurvetoAbs::~PathCurvetoAbs ( void )
{
}
void Magick::PathCurvetoAbs::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::PathCurvetoArgs>::const_iterator p = _args.begin();
       p != _args.end(); p++ )
    {
      DrawPathCurveToAbsolute( context_, p->x1(), p->y1(), p->x2(), p->y2(),
                               p->x(), p->y() );
    }
}
Magick::PathCurvetoRel::PathCurvetoRel ( const Magick::PathCurvetoArgs &args_ )
  : _args(1,args_)
{
}
Magick::PathCurvetoRel::PathCurvetoRel ( const std::list<Magick::PathCurvetoArgs> &args_ )
  : _args(args_)
{
}
Magick::PathCurvetoRel::PathCurvetoRel ( const Magick::PathCurvetoRel& original_ )
  : _args(original_._args)
{
}
Magick::PathCurvetoRel::~PathCurvetoRel ( void )
{
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
Magick::PathSmoothCurvetoAbs::PathSmoothCurvetoAbs ( const Magick::Coordinate &coordinates_ )
  : _coordinates(1,coordinates_)
{
}
Magick::PathSmoothCurvetoAbs::PathSmoothCurvetoAbs ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
Magick::PathSmoothCurvetoAbs::PathSmoothCurvetoAbs ( const Magick::PathSmoothCurvetoAbs& original_ )
  : _coordinates(original_._coordinates)
{
}
Magick::PathSmoothCurvetoAbs::~PathSmoothCurvetoAbs ( void )
{
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
Magick::PathSmoothCurvetoRel::PathSmoothCurvetoRel ( const Magick::Coordinate &coordinates_ )
  : _coordinates(1,coordinates_)
{
}
Magick::PathSmoothCurvetoRel::PathSmoothCurvetoRel ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
Magick::PathSmoothCurvetoRel::PathSmoothCurvetoRel ( const Magick::PathSmoothCurvetoRel& original_ )
  : _coordinates(original_._coordinates)
{
}
Magick::PathSmoothCurvetoRel::~PathSmoothCurvetoRel ( void )
{
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
Magick::PathQuadraticCurvetoAbs::PathQuadraticCurvetoAbs ( const Magick::PathQuadraticCurvetoArgs &args_ )
  : _args(1,args_)
{
}
Magick::PathQuadraticCurvetoAbs::PathQuadraticCurvetoAbs ( const std::list<Magick::PathQuadraticCurvetoArgs> &args_ )
  : _args(args_)
{
}
Magick::PathQuadraticCurvetoAbs::PathQuadraticCurvetoAbs ( const Magick::PathQuadraticCurvetoAbs& original_ )
  : _args(original_._args)
{
}
Magick::PathQuadraticCurvetoAbs::~PathQuadraticCurvetoAbs ( void )
{
}
void Magick::PathQuadraticCurvetoAbs::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::PathQuadraticCurvetoArgs>::const_iterator p = _args.begin();
       p != _args.end(); p++ )
    {
      DrawPathCurveToQuadraticBezierAbsolute( context_, p->x1(), p->y1(), p->x(), p->y() );
    }
}

Magick::PathQuadraticCurvetoRel::PathQuadraticCurvetoRel ( const Magick::PathQuadraticCurvetoArgs &args_ )
  : _args(1,args_)
{
}
Magick::PathQuadraticCurvetoRel::PathQuadraticCurvetoRel ( const std::list<Magick::PathQuadraticCurvetoArgs> &args_ )
  : _args(args_)
{
}
Magick::PathQuadraticCurvetoRel::PathQuadraticCurvetoRel ( const Magick::PathQuadraticCurvetoRel& original_ )
  : _args(original_._args)
{
}
Magick::PathQuadraticCurvetoRel::~PathQuadraticCurvetoRel ( void )
{
}
void Magick::PathQuadraticCurvetoRel::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::PathQuadraticCurvetoArgs>::const_iterator p = _args.begin();
       p != _args.end(); p++ )
    {
      DrawPathCurveToQuadraticBezierRelative( context_, p->x1(), p->y1(), p->x(), p->y() );
    }
}
Magick::PathSmoothQuadraticCurvetoAbs::PathSmoothQuadraticCurvetoAbs ( const Magick::Coordinate &coordinate_ )
  : _coordinates(1,coordinate_)
{
}
Magick::PathSmoothQuadraticCurvetoAbs::PathSmoothQuadraticCurvetoAbs ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
Magick::PathSmoothQuadraticCurvetoAbs::PathSmoothQuadraticCurvetoAbs ( const Magick::PathSmoothQuadraticCurvetoAbs& original_ )
  : _coordinates(original_._coordinates)
{
}
Magick::PathSmoothQuadraticCurvetoAbs::~PathSmoothQuadraticCurvetoAbs ( void )
{
}
void Magick::PathSmoothQuadraticCurvetoAbs::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
       p != _coordinates.end(); p++ )
    {
      DrawPathCurveToQuadraticBezierSmoothAbsolute( context_, p->x(), p->y() );
    }
}
Magick::PathSmoothQuadraticCurvetoRel::PathSmoothQuadraticCurvetoRel ( const Magick::Coordinate &coordinate_ )
  : _coordinates(1,coordinate_)
{
}
Magick::PathSmoothQuadraticCurvetoRel::PathSmoothQuadraticCurvetoRel ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
Magick::PathSmoothQuadraticCurvetoRel::PathSmoothQuadraticCurvetoRel ( const PathSmoothQuadraticCurvetoRel& original_ )
  : _coordinates(original_._coordinates)
{
}
Magick::PathSmoothQuadraticCurvetoRel::~PathSmoothQuadraticCurvetoRel ( void )
{
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
Magick::PathLinetoAbs::PathLinetoAbs ( const Magick::Coordinate& coordinate_  )
  : _coordinates(1,coordinate_)
{
}
Magick::PathLinetoAbs::PathLinetoAbs ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
Magick::PathLinetoAbs::PathLinetoAbs ( const Magick::PathLinetoAbs& original_ )
  : _coordinates(original_._coordinates)
{
}
Magick::PathLinetoAbs::~PathLinetoAbs ( void )
{
}
void Magick::PathLinetoAbs::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
       p != _coordinates.end(); p++ )
    {
      DrawPathLineToAbsolute( context_, p->x(), p->y() );
    }
}
Magick::PathLinetoRel::PathLinetoRel ( const Magick::Coordinate& coordinate_  )
  : _coordinates(1,coordinate_)
{
}
Magick::PathLinetoRel::PathLinetoRel ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
Magick::PathLinetoRel::PathLinetoRel ( const Magick::PathLinetoRel& original_ )
  : _coordinates(original_._coordinates)
{
}
Magick::PathLinetoRel::~PathLinetoRel ( void )
{
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

Magick::PathLinetoHorizontalAbs::~PathLinetoHorizontalAbs ( void )
{
}
void Magick::PathLinetoHorizontalAbs::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPathLineToHorizontalAbsolute( context_, _x );
}
Magick::PathLinetoHorizontalRel::~PathLinetoHorizontalRel ( void )
{
}
void Magick::PathLinetoHorizontalRel::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPathLineToHorizontalRelative( context_, _x );
}

//
// Path Vertical Lineto
//
Magick::PathLinetoVerticalAbs::~PathLinetoVerticalAbs ( void )
{
}
void Magick::PathLinetoVerticalAbs::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPathLineToVerticalAbsolute( context_, _y );
}
Magick::PathLinetoVerticalRel::~PathLinetoVerticalRel ( void )
{
}
void Magick::PathLinetoVerticalRel::operator()( MagickLib::DrawContext context_ ) const
{
  DrawPathLineToVerticalRelative( context_, _y );
}

//
// Path Moveto
//

Magick::PathMovetoAbs::PathMovetoAbs ( const Magick::Coordinate &coordinate_ )
  : _coordinates(1,coordinate_)
{
}
Magick::PathMovetoAbs::PathMovetoAbs ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
Magick::PathMovetoAbs::PathMovetoAbs ( const Magick::PathMovetoAbs& original_ )
  : _coordinates(original_._coordinates)
{
}
Magick::PathMovetoAbs::~PathMovetoAbs ( void )
{
}
void Magick::PathMovetoAbs::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
       p != _coordinates.end(); p++ )
    {
      DrawPathMoveToAbsolute( context_, p->x(), p->y() );
    }
}
Magick::PathMovetoRel::PathMovetoRel ( const Magick::Coordinate &coordinate_ )
  : _coordinates(1,coordinate_)
{
}
Magick::PathMovetoRel::PathMovetoRel ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
Magick::PathMovetoRel::PathMovetoRel ( const Magick::PathMovetoRel& original_ )
  : _coordinates(original_._coordinates)
{
}
Magick::PathMovetoRel::~PathMovetoRel ( void )
{
}
void Magick::PathMovetoRel::operator()( MagickLib::DrawContext context_ ) const
{
  for( std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
       p != _coordinates.end(); p++ )
    {
      DrawPathMoveToRelative( context_, p->x(), p->y() );
    }
}
