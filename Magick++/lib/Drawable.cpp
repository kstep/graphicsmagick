// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001, 2002
//
// Implementation of Drawable (Graphic objects)
//

#define MAGICK_IMPLEMENTATION

#include <math.h>
#include <string>
#include <iostream>
#include <strstream>

#include "Magick++/Drawable.h"
#include "Magick++/Image.h"

using namespace std;

//Write Coordinate to stream
std::ostream& Magick::operator<<( std::ostream& stream_,
                                  const Magick::Coordinate& coordinate_)
{
  stream_ << coordinate_.x() << "," << coordinate_.y();
  return stream_;
}

// Write DrawableBase to stream
std::ostream& Magick::operator<< (std::ostream& stream_,
                                  const Magick::DrawableBase& drawable_)
{
  const_cast<const Magick::DrawableBase&>(drawable_).print (stream_);
  return stream_;
}

// Write Drawable to stream
std::ostream& Magick::operator<< (std::ostream& stream_,
                                  const Magick::Drawable& drawable_)
  
{
  if (drawable_.dp != 0)
    stream_ << *drawable_.dp;
  return stream_;
}

// Write VPathBase to stream
std::ostream& Magick::operator<< (std::ostream& stream_,
                                  const Magick::VPathBase& drawable_)
{
  const_cast<const Magick::VPathBase&>(drawable_).print (stream_);
  return stream_;
}

// Write Path to stream
std::ostream& Magick::operator<< (std::ostream& stream_,
                                  const Magick::VPath& drawable_)
  
{
  if (drawable_.dp != 0)
    stream_ << *drawable_.dp;
  return stream_;
}

//
// Drawable Objects
//

// Affine (scaling, rotation, and translation)
void Magick::DrawableAffine::print (std::ostream& stream_) const
{
  stream_ << "affine "
          << _sx
          << ","
          << _rx
          << ","
          << _ry
          << ","
          << _sy
          << ","
          << _tx
          << ","
          << _ty;
}

// Angle (text drawing angle)
void Magick::DrawableAngle::print (std::ostream& stream_) const
{
  stream_ << "angle "
          << _angle;
}

// Arc
void Magick::DrawableArc::print (std::ostream& stream_) const
{
  stream_ << "arc"
          << " " << Magick::Coordinate(_startX,_startY)
          << " " << Magick::Coordinate(_endX,_endY)
          << " " << Magick::Coordinate(_startDegrees,_endDegrees);
}

// Bezier curve (Coordinate list must contain at least three members)
void Magick::DrawableBezier::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  stream_ << "bezier";
  while ( p != _coordinates.end() )
    {
      stream_ << " "
	      << *p;
      p++;
    }
}

// Circle
void Magick::DrawableCircle::print (std::ostream& stream_) const
{
  stream_ << "circle "
          << Magick::Coordinate( _originX, _originY )
          << " "
          << Magick::Coordinate( _perimX, _perimY );
}

// Colorize at point using PaintMethod
void Magick::DrawableColor::print (std::ostream& stream_) const
{
  stream_ << "color "
          << Magick::Coordinate(_x,_y)
          << " ";
  
  switch ( _paintMethod )
    {
    case PointMethod :
      stream_ << "point";
      break;
    case ReplaceMethod :
      stream_ << "replace";
      break;
    case FloodfillMethod :
      stream_ << "floodfill";
      break;
    case FillToBorderMethod :
      stream_ << "filltoborder";
      break;
    case ResetMethod :
      stream_ << "reset";
      break;
    default :
      {
	stream_ << "point";
      }
    }
}

// Drawable Path
void Magick::DrawablePath::print (std::ostream& stream_) const
{
  std::list<Magick::VPath>::const_iterator p = _path.begin();

  stream_ << "path '";
  while ( p != _path.end() )
    {
      stream_ << *p;
      p++;
    }
  stream_ << "'";
}

// Decoration (text decoration)
void Magick::DrawableTextDecoration::print (std::ostream& stream_) const
{
  stream_ << "decorate ";

  switch ( _decoration )
    {
    case NoDecoration:
      stream_ << "none";
      break;
    case UnderlineDecoration:
      stream_ << "underline";
      break;
    case OverlineDecoration:
      stream_ << "overline";
      break;
    case LineThroughDecoration:
      stream_ << "line-through";
      break;
    default :
      {
        stream_ << "none";
      }
    }
}

// Ellipse
void Magick::DrawableEllipse::print (std::ostream& stream_) const
{
  stream_ << "ellipse "
          << Magick::Coordinate(_originX,_originY)
	  << " " << _radiusX << "," << _radiusY
	  << " " << _arcStart << "," << _arcEnd;
}

// Specify drawing fill color
void Magick::DrawableFillColor::print (std::ostream& stream_) const
{
  stream_ << "fill "
          << string(_color);
}

// Specify drawing fill fule
void Magick::DrawableFillRule::print (std::ostream& stream_) const
{
  stream_ << "fill-rule ";

  switch (_fillRule)
    {
    case EvenOddRule :
      {
        stream_ << "evenodd";
        break;
      }
    case NonZeroRule :
      {
        stream_ << "nonzero";
        break;
      }
    default :
      {
        stream_ << "none";
      }
    }
}

// Specify drawing fill opacity
void Magick::DrawableFillOpacity::print (std::ostream& stream_) const
{
  stream_ << "fill-opacity "
          << _opacity;
}

// Specify text font
void Magick::DrawableFont::print (std::ostream& stream_) const
{
  // font
  if(_font.length())
    {
      stream_ << "font '";
      for ( unsigned int i = 0; i < _font.length(); ++i )
        {
          if ( _font[i] == '\'' )
            stream_ << "\\";
          stream_ << _font[i];
        }
      stream_  << "'";
    }

  // font-family
  if(_family.length())
    {
      stream_ << "font-family '";
      for ( unsigned int i = 0; i < _family.length(); ++i )
        {
          if ( _family[i] == '\'' )
            stream_ << "\\";
          stream_ << _family[i];
        }
      stream_  << "'" << endl;

      // font-style
      stream_ << "font-style '";
      switch(_style)
        {
        case NormalStyle:
          stream_ << "normal";
          break;
        case ItalicStyle:
          stream_ <<  "italic";
          break;
        case ObliqueStyle:
          stream_ <<  "oblique";
          break;
        case AnyStyle:
          stream_ << "all";
          break;
        default:
          {
            stream_ << "all";
          }
        }
      stream_ << "'" << endl;

      // font-weight
      stream_ << "font-weight '";
      if(_weight)
        stream_ << _weight;
      else
        stream_ << "all";
      stream_ << "'" << endl;

      // font-stretch
      stream_ << "font-stretch '";
      switch(_stretch)
        {
        case NormalStretch:
          stream_ << "normal";
          break;
        case UltraCondensedStretch:
          stream_ << "ultra-condensed";
          break;
        case ExtraCondensedStretch:
          stream_ << "extra-condensed";
          break;
        case CondensedStretch:
          stream_ << "condensed";
          break;
        case SemiCondensedStretch:
          stream_ << "semi-condensed";
          break;
        case SemiExpandedStretch:
          stream_ << "semi-expanded";
          break;
        case ExpandedStretch:
          stream_ << "expanded";
          break;
        case ExtraExpandedStretch:
          stream_ << "extra-expanded";
          break;
        case UltraExpandedStretch:
          stream_ << "ultra-expanded";
          break;
        case AnyStretch:
          stream_ << "all";
          break;
        default:
          {
            stream_ << "all";
          }
        }
      stream_ << "'";
    }
}

// Specify text positioning gravity
void Magick::DrawableGravity::print (std::ostream& stream_) const
{
  stream_ << "gravity ";

  switch ( _gravity )
    {
    case ForgetGravity :
      {
        stream_ << "Forget";
        break;
      }
    case NorthWestGravity :
      {
        stream_ << "NorthWest";
        break;
      }
    case NorthGravity :
      {
        stream_ << "North";
        break;
      }
    case NorthEastGravity :
      {
        stream_ << "NorthEast";
        break;
      }
    case WestGravity :
      {
        stream_ << "West";
        break;
      }
    case CenterGravity :
      {
        stream_ << "Center";
        break;
      }
    case EastGravity :
      {
        stream_ << "East";
        break;
      }
    case SouthWestGravity :
      {
        stream_ << "SouthWest";
        break;
      }
    case SouthGravity :
      {
        stream_ << "South";
        break;
      }
    case SouthEastGravity :
      {
        stream_ << "SouthEast";
        break;
      }
    case StaticGravity :
      {
        stream_ << "Static";
        break;
      }
    default :
      {
        stream_ << "Forget";
      }
    }
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

void Magick::DrawableCompositeImage::print (std::ostream& stream_) const
{
  stream_ << "image ";

  switch ( _composition )
    {
    case OverCompositeOp:
      stream_ << "over ";
      break;
    case InCompositeOp:
      stream_ << "in ";
      break;
    case OutCompositeOp:
      stream_ << "out ";
      break;
    case AtopCompositeOp:
      stream_ << "atop ";
      break;
    case XorCompositeOp:
      stream_ << "xor ";
      break;
    case PlusCompositeOp:
      stream_ << "plus ";
      break;
    case MinusCompositeOp:
      stream_ << "minus ";
      break;
    case AddCompositeOp:
      stream_ << "add ";
      break;
    case SubtractCompositeOp:
      stream_ << "subtract ";
      break;
    case DifferenceCompositeOp:
      stream_ << "difference ";
      break;
    case MultiplyCompositeOp:
      stream_ << "multiply ";
      break;
    case BumpmapCompositeOp:
      stream_ << "bumpmap ";
      break;
    case CopyCompositeOp:
      stream_ << "copy ";
      break;
    case CopyRedCompositeOp:
      stream_ << "copyred ";
      break;
    case CopyGreenCompositeOp:
      stream_ << "copygreen ";
      break;
    case CopyBlueCompositeOp:
      stream_ << "copyblue ";
      break;
    case CopyOpacityCompositeOp:
      stream_ << "copyopacity ";
      break;
    default:
      {
        stream_ << "copy ";
      }
    }

  stream_ << Magick::Coordinate( _x, _y)
          << " "
          << Magick::Coordinate( _width, _height)
          << " '";
  
  // Pass as Base64-encoded inline image using a data URL
  // (see RFC 2397).  The URL format follows the form:
  // data:[<mediatype>][;base64],<data>
  Blob blob;
  _image->write(&blob);
  
  char *media_type = MagickToMime((_image->magick()).c_str());
  
  char *base64 =
    Base64Encode(static_cast<const unsigned char*>(blob.data()), blob.length());
  
  stream_ << "data:" << media_type << ";base64," << endl;
  
  unsigned int width = 0;
  for( char *p = base64; *p ; p++ )
    {
      if( width == 0 )
        stream_ << " ";
      
      stream_ << *p;
      
      width++;
      
      if( width > 76 )
        {
          stream_ << endl;
          width = 0;
        }
    }
  stream_ << "'";
  
  LiberateMemory(reinterpret_cast<void **>(&media_type));
  LiberateMemory(reinterpret_cast<void **>(&base64));
}

// Line
void Magick::DrawableLine::print (std::ostream& stream_) const
{
  stream_ << "line "
          << Magick::Coordinate( _startX, _startY )
          << " "
          << Magick::Coordinate( _endX, _endY );
}

// Change pixel matte value to transparent using PaintMethod
void Magick::DrawableMatte::print (std::ostream& stream_) const
{
  stream_ << "matte "
          << Magick::Coordinate( _x, _y )
          << " ";
  
  switch ( _paintMethod )
    {
    case PointMethod :
      stream_ << "point";
      break;
    case ReplaceMethod :
      stream_ << "replace";
      break;
    case FloodfillMethod :
      stream_ << "floodfill";
      break;
    case FillToBorderMethod :
      stream_ << "filltoborder";
      break;
    case ResetMethod :
      stream_ << "reset";
      break;
    default :
      {
	stream_ << "point";
      }
    }
}

// Point
void Magick::DrawablePoint::print (std::ostream& stream_) const
{
  stream_ << "point "
          << Magick::Coordinate( _x, _y );
}

// Text pointsize
void Magick::DrawablePointSize::print (std::ostream& stream_) const
{
  stream_ << "font-size "
          << _pointSize;
}

// Polygon (Coordinate list must contain at least three members)
void Magick::DrawablePolygon::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  stream_ << "polygon";
  while ( p != _coordinates.end() )
    {
      stream_ << " "
	      << *p;
      p++;
    }
}

// Polyline (Coordinate list must contain at least three members)
void Magick::DrawablePolyline::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  stream_ << "polyline";
  while ( p != _coordinates.end() )
    {
      stream_ << " "
	      << *p;
      p++;
    }
}

// Pop Graphic Context
void Magick::DrawablePopGraphicContext::print (std::ostream& stream_) const
{
  stream_ << "pop graphic-context";
}

// Push Graphic Context
void Magick::DrawablePushGraphicContext::print (std::ostream& stream_) const
{
  stream_ << "push graphic-context";
}

// Pop (terminate) Pattern definition
void Magick::DrawablePopPattern::print (std::ostream& stream_) const
{
  stream_ << "pop pattern";
}

// Push Pattern definition
void Magick::DrawablePushPattern::print (std::ostream& stream_) const
{
  stream_ << "push pattern "
          << _id
          << " "
          << _x << "," << _y
          << " "
          << _width << "," << _height;
}

// Rectangle
void Magick::DrawableRectangle::print (std::ostream& stream_) const
{
  stream_ << "rectangle "
          << Magick::Coordinate( _upperLeftX, _upperLeftY )
          << " "
          << Magick::Coordinate( _lowerRightX, _lowerRightY );
}

// Apply Rotation
void Magick::DrawableRotation::print (std::ostream& stream_) const
{
  stream_ << "rotate " << _angle;
}

// Round Rectangle
void Magick::DrawableRoundRectangle::print (std::ostream& stream_) const
{
  stream_ << "roundRectangle"
          << " " << Magick::Coordinate(_centerX,_centerY)
          << " " << Magick::Coordinate(_width,_hight)
          << " " << Magick::Coordinate(_cornerWidth,_cornerHeight);
}

// Apply Scaling
void Magick::DrawableScaling::print (std::ostream& stream_) const
{
  stream_ << "scale "  << _x << " " << _y;
}

// Apply Skew in the X direction
void Magick::DrawableSkewX::print (std::ostream& stream_) const
{
  stream_ << "skewX " << _angle;
}

// Apply Skew in the Y direction
void Magick::DrawableSkewY::print (std::ostream& stream_) const
{
  stream_ << "skewY " << _angle;
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
void Magick::DrawableDashArray::print (std::ostream& stream_) const
{
  stream_ << "stroke-dasharray ";
  if( _dasharray )
    {
      unsigned int n;
      for (n=0; _dasharray[n]; n++)
        {
          if(n)
            stream_ << ",";
          stream_ << _dasharray[n];
        }
    }
  else
    {
      stream_ << "none";
    }
}

// Stroke dashoffset
void Magick::DrawableDashOffset::print (std::ostream& stream_) const
{
  stream_ << "stroke-dashoffset " << _offset;
}

// Stroke linecap
void Magick::DrawableStrokeLineCap::print (std::ostream& stream_) const
{
  stream_ << "stroke-linecap ";

  switch( _linecap )
    {
    case ButtCap :
      stream_ << "butt";
      break;
    case RoundCap :
      stream_ << "round";
      break;
    case SquareCap :
      stream_ << "square";
      break;
    case UndefinedCap :
      stream_ << "none";
      break;
    default :
      {
        stream_ << "none";
      }
    }
}

// Stroke linejoin
void Magick::DrawableStrokeLineJoin::print (std::ostream& stream_) const
{
  stream_ << "stroke-linejoin ";

  switch( _linejoin )
    {
    case MiterJoin :
      stream_ << "miter";
      break;
    case RoundJoin :
      stream_ << "round";
      break;
    case BevelJoin :
      stream_ << "bevel";
      break;
    case UndefinedJoin :
      stream_ << "none";
      break;
    default :
      {
        stream_ << "none";
      }
    }
}

// Stroke miterlimit
void Magick::DrawableMiterLimit::print (std::ostream& stream_) const
{
  stream_ << "stroke-miterlimit " << _miterlimit;
}

// Stroke color
void Magick::DrawableStrokeColor::print (std::ostream& stream_) const
{
  stream_ << "stroke "
          << string(_color);
}

// Stroke antialias
void Magick::DrawableStrokeAntialias::print (std::ostream& stream_) const
{
  stream_ << "stroke-antialias ";
  if ( _flag )
    stream_ << "1";
  else
    stream_ << "0";
}

// Stroke opacity
void Magick::DrawableStrokeOpacity::print (std::ostream& stream_) const
{
  stream_ << "stroke-opacity "
          << _opacity;
}

// Stroke width
void Magick::DrawableStrokeWidth::print (std::ostream& stream_) const
{
  stream_ << "stroke-width "
          << _width;
}

// Draw text at point
void Magick::DrawableText::print (std::ostream& stream_) const
{
  stream_ << "text "
          << Magick::Coordinate( _x, _y)
	  << " '";
  for ( unsigned int i = 0; i < _text.length(); ++i )
    {
      if ( _text[i] == '\'' )
        {
          stream_ << "\\";
        }
      stream_ << _text[i];
    }
  stream_ << "\'";
}

// Apply Translation
void Magick::DrawableTranslation::print (std::ostream& stream_) const
{
  stream_ << "translate " << _x << "," << _y;
}

// Text antialias
void Magick::DrawableTextAntialias::print (std::ostream& stream_) const
{
  stream_ << "text-antialias ";
  if ( _flag )
    stream_ << "1";
  else
    stream_ << "0";
}

// Set the size of the viewbox
void Magick::DrawableViewbox::print (std::ostream& stream_) const
{
  stream_ << "viewbox "
          << _x1 << " "
          << _y1 << " "
          << _x2 << " "
          << _y2;
}

//
// Path Classes
//

//
// PathArcAbs
//

// Write PathArcArgs to stream
std::ostream& Magick::operator<<( std::ostream& stream_, const Magick::PathArcArgs& args_ )
{
  stream_ << args_.radiusX()
          << ","
          << args_.radiusY()
          << " "
          << args_.xAxisRotation()
          << " ";

  if ( args_.largeArcFlag() )
    stream_ << "1 ";
  else
    stream_ << "0 ";

  if ( args_.sweepFlag() )
    stream_ << "1 ";
  else
    stream_ << "0 ";

  stream_ << args_.x()
          << ","
          << args_.y();

  return stream_;
}
// Path Arc
void Magick::PathArcAbs::print (std::ostream& stream_) const
{
  std::list<Magick::PathArcArgs>::const_iterator p = _coordinates.begin();

  stream_ << "A";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathArcRel::print (std::ostream& stream_) const
{
  std::list<Magick::PathArcArgs>::const_iterator p = _coordinates.begin();

  stream_ << "a";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}

//
// Path Closepath
//
// Support a polymorphic print-to-stream operator
void Magick::PathClosePath::print (std::ostream& stream_) const
{
  stream_ << "z ";
}

//
// Path Curveto (Cubic Bezier)
//
std::ostream& Magick::operator<<( std::ostream& stream_,
                                  const Magick::PathCurvetoArgs& args_ )
{
  stream_ 
    << args_.x1()
    << " "
    << args_.y1()
    << " "
    << args_.x2()
    << " "
    << args_.y2()
    << " "
    << args_.x()
    << " "
    << args_.y();
  return stream_;
}
// Support a polymorphic print-to-stream operator
void Magick::PathCurvetoAbs::print (std::ostream& stream_) const
{
  std::list<Magick::PathCurvetoArgs>::const_iterator p = _args.begin();

  stream_ << "C";
  while ( p != _args.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathCurvetoRel::print (std::ostream& stream_) const
{
  std::list<Magick::PathCurvetoArgs>::const_iterator p = _args.begin();

  stream_ << "c";
  while ( p != _args.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
// Support a polymorphic print-to-stream operator
void Magick::PathSmoothCurvetoAbs::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
  
  stream_ << "S";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathSmoothCurvetoRel::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
  
  stream_ << "s";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}


//
// Quadratic Curveto (Quadratic Bezier)
//
std::ostream& Magick::operator<<( std::ostream& stream_,
                                  const Magick::PathQuadraticCurvetoArgs& args_)
{
  stream_ 
    << args_.x1()
    << " "
    << args_.y1()
    << " "
    << args_.x()
    << " "
    << args_.y();
  return stream_;
}
// Support a polymorphic print-to-stream operator
void Magick::PathQuadraticCurvetoAbs::print (std::ostream& stream_) const
{
  std::list<Magick::PathQuadraticCurvetoArgs>::const_iterator p = _args.begin();

  stream_ << "Q";
  while ( p != _args.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathQuadraticCurvetoRel::print (std::ostream& stream_) const
{
  std::list<Magick::PathQuadraticCurvetoArgs>::const_iterator p = _args.begin();

  stream_ << "q";
  while ( p != _args.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathSmoothQuadraticCurvetoAbs::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
  
  stream_ << "T";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathSmoothQuadraticCurvetoRel::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
  
  stream_ << "t";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}

//
// Path Lineto
//

// Support a polymorphic print-to-stream operator
void Magick::PathLinetoAbs::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  stream_ << "L";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathLinetoRel::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  stream_ << "l";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}

//
// Path Horizontal Lineto
//

// Support a polymorphic print-to-stream operator
void Magick::PathLinetoHorizontalAbs::print (std::ostream& stream_) const
{
  stream_ << "H" << _x << " ";
}
void Magick::PathLinetoHorizontalRel::print (std::ostream& stream_) const
{
  stream_ << "h" << _x << " ";
}

//
// Path Vertical Lineto
//

// Support a polymorphic print-to-stream operator
void Magick::PathLinetoVerticalAbs::print (std::ostream& stream_) const
{
  stream_ << "V" << _y << " ";
}
void Magick::PathLinetoVerticalRel::print (std::ostream& stream_) const
{
  stream_ << "v" << _y << " ";
}

//
// Path Moveto
//

// Support a polymorphic print-to-stream operator
void Magick::PathMovetoAbs::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  stream_ << "M";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathMovetoRel::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  stream_ << "m";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
