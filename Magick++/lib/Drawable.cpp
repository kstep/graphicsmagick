// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Implementation of Drawable (Graphic objects)
//

#define MAGICK_IMPLEMENTATION

#include <math.h>
#include <string>
#include <iostream>
#include <strstream>

#include "Magick++/Drawable.h"

using namespace std;

#define DegreesToRadians(x) ((x)*3.14159265358979323846/180.0)

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
          << " " << Coordinate(_startX,_startY)
          << " " << Coordinate(_endX,_endY)
          << " " << Coordinate(_startDegrees,_endDegrees);
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
          << Coordinate( _originX, _originY )
          << " "
          << Coordinate( _perimX, _perimY );
}

// Colorize at point using PaintMethod
void Magick::DrawableColor::print (std::ostream& stream_) const
{
  stream_ << "color "
          << Coordinate(_x,_y)
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
Magick::DrawableTextDecoration::DrawableTextDecoration ( Magick::DecorationType decoration_ )
  : _decoration(decoration_)
{
}
// Support a polymorphic print-to-stream operator
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
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableTextDecoration::copy() const
{
  return new DrawableTextDecoration(*this);
}

// Ellipse
Magick::DrawableEllipse::DrawableEllipse ( double originX_,
                                           double originY_, 
                                           double width_,
                                           double height_,
                                           double arcStart_,
                                           double arcEnd_ )
  : _originX(originX_),
    _originY(originY_),
    _width(width_),
    _height(height_),
    _arcStart(arcStart_),
    _arcEnd(arcEnd_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableEllipse::print (std::ostream& stream_) const
{
  stream_ << "ellipse "
          << Coordinate(_originX,_originY)
	  << " " << _width << "," << _height
	  << " " << _arcStart << "," << _arcEnd;
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableEllipse::copy() const
{
  return new DrawableEllipse(*this);
}

// Specify drawing fill color
Magick::DrawableFillColor::DrawableFillColor ( const Magick::Color &color_ )
  : _color(color_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableFillColor::print (std::ostream& stream_) const
{
  stream_ << "fill "
          << string(_color);
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableFillColor::copy() const
{
  return new DrawableFillColor(*this);
}

// Specify drawing fill opacity
Magick::DrawableFillOpacity::DrawableFillOpacity ( double opacity_ )
  : _opacity(opacity_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableFillOpacity::print (std::ostream& stream_) const
{
  stream_ << "fill-opacity "
          << _opacity;
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableFillOpacity::copy() const
{
  return new DrawableFillOpacity(*this);
}

// Specify text font
Magick::DrawableFont::DrawableFont ( const std::string &font_ )
  : _font(font_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableFont::print (std::ostream& stream_) const
{
  stream_ << "font "
          << _font;
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableFont::copy() const
{
  return new DrawableFont(*this);
}

// Specify text positioning gravity
Magick::DrawableGravity::DrawableGravity ( Magick::GravityType gravity_ )
  : _gravity(gravity_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableGravity::print (std::ostream& stream_) const
{
  stream_ << "gravity ";

  switch ( _gravity )
    {
      case ForgetGravity :
        {
          stream_ << "ForgetGravity";
          break;
        }
      case NorthWestGravity :
        {
          stream_ << "NorthWestGravity";
          break;
        }
      case NorthGravity :
        {
          stream_ << "NorthGravity";
          break;
        }
      case NorthEastGravity :
        {
          stream_ << "NorthEastGravity";
          break;
        }
      case WestGravity :
        {
          stream_ << "WestGravity";
          break;
        }
      case CenterGravity :
        {
          stream_ << "CenterGravity";
          break;
        }
      case EastGravity :
        {
          stream_ << "EastGravity";
          break;
        }
      case SouthWestGravity :
        {
          stream_ << "SouthWestGravity";
          break;
        }
      case SouthGravity :
        {
          stream_ << "SouthGravity";
          break;
        }
      case SouthEastGravity :
        {
          stream_ << "SouthEastGravity";
          break;
        }
      case StaticGravity :
        {
          stream_ << "StaticGravity";
          break;
        }

    default :
      {
        stream_ << "ForgetGravity";
      }
    }
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableGravity::copy() const
{
  return new DrawableGravity(*this);
}

// Draw image at point
Magick::DrawableCompositeImage::DrawableCompositeImage ( double x_,
                                                         double y_,
                                                         double width_,
                                                         double height_,
                                                         const std::string &image_ )
  : _x(x_),
    _y(y_),
    _width(width_),
    _height(height_),
    _image(image_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableCompositeImage::print (std::ostream& stream_) const
{
  stream_ << "image "
          << Coordinate( _x, _y)
          << " " << Coordinate( _width, _height)
          << " \"";
  for ( unsigned int i = 0; i < _image.length(); ++i )
    {
      if ( _image[i] == '"' )
	stream_ << "\\";
      stream_ << _image[i];
    }
  stream_  << "\"";
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableCompositeImage::copy() const
{
  return new DrawableCompositeImage(*this);
}

// Line
Magick::DrawableLine::DrawableLine ( double startX_,
                                     double startY_,
                                     double endX_,
                                     double endY_ )
  : _startX(startX_),
    _startY(startY_),
    _endX(endX_),
    _endY(endY_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableLine::print (std::ostream& stream_) const
{
  stream_ << "line "
          << Coordinate( _startX, _startY )
          << " "
          << Coordinate( _endX, _endY );
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableLine::copy() const
{
  return new DrawableLine(*this);
}

// Change pixel matte value to transparent using PaintMethod
Magick::DrawableMatte::DrawableMatte ( double x_, double y_,
                                       Magick::PaintMethod paintMethod_ )
  : _x(x_),
    _y(y_),
    _paintMethod(paintMethod_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableMatte::print (std::ostream& stream_) const
{
  stream_ << "matte "
          << Coordinate( _x, _y )
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
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableMatte::copy() const
{
  return new DrawableMatte(*this);
}

// Point
void Magick::DrawablePoint::print (std::ostream& stream_) const
{
  stream_ << "point "
          << Coordinate( _x, _y );
}

// Text pointsize
void Magick::DrawablePointSize::print (std::ostream& stream_) const
{
  stream_ << "pointsize "
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

// Rectangle
void Magick::DrawableRectangle::print (std::ostream& stream_) const
{
  stream_ << "rectangle "
          << Coordinate( _upperLeftX, _upperLeftY )
          << " "
          << Coordinate( _lowerRightX, _lowerRightY );
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
          << " " << Coordinate(_centerX,_centerY)
          << " " << Coordinate(_width,_hight)
          << " " << Coordinate(_cornerWidth,_cornerHeight);
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
          << Coordinate( _x, _y)
	  << " \"";
  for ( unsigned int i = 0; i < _text.length(); ++i )
    {
      if ( _text[i] == '"' )
	stream_ << "\\";
      stream_ << _text[i];
    }
  stream_ << "\"";
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

//
// Path Classes
//

//
// PathArcAbs
//

// Print coordinate to stream.
std::ostream& operator<<( std::ostream& stream_, const Magick::PathArcArgs& args_
)
{
  stream_ << args_._radiusX
          << ","
          << args_._radiusY
          << " "
          << args_._xAxisRotation
          << " ";

  if ( args_._largeArcFlag )
    stream_ << "1 ";
  else
    stream_ << "0 ";

  if ( args_._sweepFlag )
    stream_ << "1 ";
  else
    stream_ << "0 ";

  stream_ << args_._x
          << ","
          << args_._y;

  return stream_;
}
// Support a polymorphic print-to-stream operator
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
std::ostream& operator<<( std::ostream& stream_, const Magick::PathCurvetoArgs& args_
)
{
  stream_ 
    << args_._x1
    << " "
    << args_._y1
    << " "
    << args_._x2
    << " "
    << args_._y2
    << " "
    << args_._x
    << " "
    << args_._y;
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
std::ostream& operator<<( std::ostream& stream_, const Magick::PathQuadraticCurvetoArgs& args_)
{
  stream_ 
    << args_._x1
    << " "
    << args_._y1
    << " "
    << args_._x
    << " "
    << args_._y;
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
