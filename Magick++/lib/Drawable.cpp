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

// Print coordinate to stream.
std::ostream& Magick::operator<<( std::ostream& stream_, const Magick::Coordinate& coordinate_
)
{
  stream_ << coordinate_._x << "," << coordinate_._y;
  return stream_;
}

//
// DrawableBase implementation
//
// Constructor
Magick::DrawableBase::DrawableBase( void )
{
  // All components are self-initializing
}

// Destructor
Magick::DrawableBase::~DrawableBase( void )
{
  // Nothing to do
}

// Copy constructor
// Magick::DrawableBase::DrawableBase ( const DrawableBase & drawable_ )
// {
// }

// Assignment operator
// const Magick::DrawableBase& Magick::DrawableBase::operator = ( const Magick::DrawableBase& original_ )
// {
//   // If not being set to ourself
//   if ( this != &original_ )
//     {
      
//     }
//   return *this;
// }

ostream& Magick::operator<< (ostream& stream_,
                             const Magick::DrawableBase& drawable_)
{
  drawable_.print (stream_);
  return stream_;
}

//
// Drawable surrogate implementation
//
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

// Print object to stream
ostream& Magick::operator<< (ostream& stream_,
                             const Magick::Drawable& drawable_)

{
  if (drawable_.dp != 0)
    stream_ << *drawable_.dp;
  return stream_;
}

//
// Drawable Objects
//

// Affine (scaling, rotation, and translation)
Magick::DrawableAffine::DrawableAffine ( double scaleX_, double scaleY_,
                                         double rotationX_, double rotationY_,
                                         double translationX_, double translationY_ )
  : _scaleX(scaleX_),
    _scaleY(scaleY_),
    _rotationX(rotationX_),
    _rotationY(rotationY_),
    _translationX(translationX_),
    _translationY(translationY_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableAffine::print (ostream& stream_) const
{
  stream_ << "affine "
          << _scaleX
          << ","
          << _rotationX
          << ","
          << _rotationY
          << ","
          << _scaleY
          << ","
          << _translationX
          << ","
          << _translationY;
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableAffine::copy() const
{
  return new DrawableAffine(*this);
}

// Angle (text drawing angle)
Magick::DrawableAngle::DrawableAngle ( double angle_ )
  : _angle(angle_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableAngle::print (ostream& stream_) const
{
  stream_ << "angle "
          << _angle;
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableAngle::copy() const
{
  return new DrawableAngle(*this);
}

// Arc
Magick::DrawableArc::DrawableArc ( double startX_, double startY_,
                                   double endX_, double endY_,
                                   double startDegrees_, double endDegrees_ )
  : _startX(startX_),
    _startY(startY_),
    _endX(endX_),
    _endY(endY_),
    _startDegrees(startDegrees_),
    _endDegrees(endDegrees_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableArc::print (ostream& stream_) const
{
  stream_ << "arc"
          << " " << Coordinate(_startX,_startY)
          << " " << Coordinate(_endX,_endY)
          << " " << Coordinate(_startDegrees,_endDegrees);
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableArc::copy() const
{
  return new DrawableArc(*this);
}

// Bezier curve (Coordinate list must contain at least three members)
Magick::DrawableBezier::DrawableBezier ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableBezier::print (ostream& stream_) const
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
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableBezier::copy() const
{
  return new DrawableBezier(*this);
}

// Circle
Magick::DrawableCircle::DrawableCircle ( double originX_,
                                         double originY_,
                                         double perimX_,
                                         double perimY_ )
  : _originX(originX_),
    _originY(originY_),
    _perimX(perimX_),
    _perimY(perimY_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableCircle::print (ostream& stream_) const
{
  stream_ << "circle "
          << Coordinate( _originX, _originY )
          << " "
          << Coordinate( _perimX, _perimY );
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableCircle::copy() const
{
  return new DrawableCircle(*this);
}

// Colorize at point using PaintMethod
Magick::DrawableColor::DrawableColor ( double x_, double y_,
                                       Magick::PaintMethod paintMethod_ )
  : _x(x_),
    _y(y_),
    _paintMethod(paintMethod_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableColor::print (ostream& stream_) const
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
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableColor::copy() const
{
  return new DrawableColor(*this);
}

// Decoration (text decoration)
Magick::DrawableTextDecoration::DrawableTextDecoration ( Magick::DecorationType decoration_ )
  : _decoration(decoration_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableTextDecoration::print (ostream& stream_) const
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
void Magick::DrawableEllipse::print (ostream& stream_) const
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
void Magick::DrawableFillColor::print (ostream& stream_) const
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
void Magick::DrawableFillOpacity::print (ostream& stream_) const
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
void Magick::DrawableFont::print (ostream& stream_) const
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
void Magick::DrawableGravity::print (ostream& stream_) const
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
void Magick::DrawableCompositeImage::print (ostream& stream_) const
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
void Magick::DrawableLine::print (ostream& stream_) const
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
void Magick::DrawableMatte::print (ostream& stream_) const
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
Magick::DrawablePoint::DrawablePoint ( double x_,
                                       double y_ )
  : _x(x_),
    _y(y_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawablePoint::print (ostream& stream_) const
{
  stream_ << "point "
          << Coordinate( _x, _y );
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawablePoint::copy() const
{
  return new DrawablePoint(*this);
}

// Text pointsize
Magick::DrawablePointSize::DrawablePointSize( double pointSize_ )
  : _pointSize(pointSize_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawablePointSize::print (ostream& stream_) const
{
  stream_ << "pointsize "
          << _pointSize;
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawablePointSize::copy() const
{
  return new DrawablePointSize(*this);
}

// Polygon (Coordinate list must contain at least three members)
Magick::DrawablePolygon::DrawablePolygon ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawablePolygon::print (ostream& stream_) const
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
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawablePolygon::copy() const
{
  return new DrawablePolygon(*this);
}

// Polyline (Coordinate list must contain at least three members)
Magick::DrawablePolyline::DrawablePolyline ( const std::list<Magick::Coordinate> &coordinates_ )
  : _coordinates(coordinates_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawablePolyline::print (ostream& stream_) const
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
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawablePolyline::copy() const
{
  return new DrawablePolyline(*this);
}

// Rectangle
Magick::DrawableRectangle::DrawableRectangle ( double upperLeftX_,
                                               double upperLeftY_,
                                               double lowerRightX_,
                                               double lowerRightY_ )
  : _upperLeftX(upperLeftX_),
    _upperLeftY(upperLeftY_),
    _lowerRightX(lowerRightX_),
    _lowerRightY(lowerRightY_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableRectangle::print (ostream& stream_) const
{
  stream_ << "rectangle "
          << Coordinate( _upperLeftX, _upperLeftY )
          << Coordinate( _lowerRightX, _lowerRightY );
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableRectangle::copy() const
{
  return new DrawableRectangle(*this);
}

// Apply Rotation
Magick::DrawableRotation::DrawableRotation ( double angle_ )
//   : DrawableAffine( -cos(DegreesToRadians(fmod(angle_,360.0))), // affine[0]
//                     -cos(DegreesToRadians(fmod(angle_,360.0))), // affine[3]
//                     sin(DegreesToRadians(fmod(angle_,360.0))),  // affine[1]
//                     -sin(DegreesToRadians(fmod(angle_,360.0))), // affine[2]
//                     0,
//                     0
//                     )
  : DrawableAffine( cos(DegreesToRadians(fmod(angle_,360.0))), // affine[0]
                    cos(DegreesToRadians(fmod(angle_,360.0))), // affine[3]
                    -sin(DegreesToRadians(fmod(angle_,360.0))),  // affine[1]
                    sin(DegreesToRadians(fmod(angle_,360.0))), // affine[2]
                    0,
                    0
                    )
{
  // FIXME, should be based on defaults from ImageInfo?
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableRotation::print (ostream& stream_) const
{
  DrawableAffine::print( stream_ );
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableRotation::copy() const
{
  return new DrawableRotation(*this);
}

// Round Rectangle
Magick::DrawableRoundRectangle::DrawableRoundRectangle
( double centerX_, double centerY_,
  double width_, double hight_,
  double cornerWidth_, double cornerHeight_ )
  : _centerX(centerX_),
    _centerY(centerY_),
    _width(width_),
    _hight(hight_),
    _cornerWidth(cornerWidth_),
    _cornerHeight(cornerHeight_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableRoundRectangle::print (ostream& stream_) const
{
  stream_ << "roundRectangle"
          << " " << Coordinate(_centerX,_centerY)
          << " " << Coordinate(_width,_hight)
          << " " << Coordinate(_cornerWidth,_cornerHeight);
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableRoundRectangle::copy() const
{
  return new DrawableRoundRectangle(*this);
}

// Apply Scaling
Magick::DrawableScaling::DrawableScaling ( double x_, double y_ )
  : DrawableAffine( x_, y_, 0, 0, 0, 0 )
{
  // FIXME, should be based on defaults from ImageInfo?
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableScaling::print (ostream& stream_) const
{
  DrawableAffine::print( stream_ );
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableScaling::copy() const
{
  return new DrawableScaling(*this);
}

// Stroke color
Magick::DrawableStrokeColor::DrawableStrokeColor ( const Magick::Color &color_ )
  : _color(color_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableStrokeColor::print (ostream& stream_) const
{
  stream_ << "stroke "
          << string(_color);
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableStrokeColor::copy() const
{
  return new DrawableStrokeColor(*this);
}

// Stroke opacity
Magick::DrawableStrokeOpacity::DrawableStrokeOpacity ( double opacity_ )
  : _opacity(opacity_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableStrokeOpacity::print (ostream& stream_) const
{
  stream_ << "stroke-opacity "
          << _opacity;
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableStrokeOpacity::copy() const
{
  return new DrawableStrokeOpacity(*this);
}

// Stroke width
Magick::DrawableStrokeWidth::DrawableStrokeWidth ( double width_ )
  : _width(width_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableStrokeWidth::print (ostream& stream_) const
{
  stream_ << "stroke-width "
          << _width;
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableStrokeWidth::copy() const
{
  return new DrawableStrokeWidth(*this);
}

// Draw text at point
Magick::DrawableText::DrawableText ( double x_,
                                     double y_,
                                     const std::string &text_ )
  : _x(x_),
    _y(y_),
    _text(text_)
{
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableText::print (ostream& stream_) const
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
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableText::copy() const
{
  return new DrawableText(*this);
}

// Apply Translation
Magick::DrawableTranslation::DrawableTranslation ( double x_, double y_ )
  : DrawableAffine( 1.0, 1.0, 0, 0, x_, y_ )
{
  // FIXME, should be based on defaults from ImageInfo?
}
// Support a polymorphic print-to-stream operator
void Magick::DrawableTranslation::print (ostream& stream_) const
{
  DrawableAffine::print( stream_ );
}
// Return polymorphic copy of object
Magick::DrawableBase* Magick::DrawableTranslation::copy() const
{
  return new DrawableTranslation(*this);
}
