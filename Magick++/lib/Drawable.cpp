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
// Drawable implementation
//
// Constructor
Magick::Drawable::Drawable( void )
  : _primitive()
{
  // All components are self-initializing
}

// Destructor
Magick::Drawable::~Drawable( void )
{
  // Nothing to do
}

// Copy constructor
Magick::Drawable::Drawable ( const Drawable & drawable_ )
  : _primitive(drawable_._primitive)
{
}

// Assignment operator
const Magick::Drawable& Magick::Drawable::operator = ( const Drawable& drawable_ )
{
  // If not being set to ourself
  if ( this != &drawable_ )
    {
      _primitive = drawable_._primitive;
    }
  return *this;
}

// Common implementation of methods which take one Coordinate argument
void Magick::Drawable::one_arg_impl ( const char* command_,
				      const Magick::Coordinate &coordinate_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << command_ << " " << coordinate_ << ends;
  _primitive.assign(buffer);
}

// Common implementation of methods which take two Coordinate arguments
void Magick::Drawable::two_arg_impl ( const char* command_,
				      const Magick::Coordinate &startCoordinate_,
				      const Magick::Coordinate &endCoordinate_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << command_ << " " << startCoordinate_ << " " << endCoordinate_ << ends;
  _primitive.assign( buffer );
}

// Common implementation of methods which take a list argument
void Magick::Drawable::list_arg_impl ( const char* command_,
				       const std::list<Magick::Coordinate> &coordinates_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));

  buffstr << command_;

  std::list<Magick::Coordinate>::const_iterator p = coordinates_.begin();

  while ( p != coordinates_.end() )
    {
      buffstr << " "
	      << *p;
      p++;
    }

  buffstr << ends;
  _primitive.assign( buffer );
}

//
// Drawable Objects
//

// Affine (scaling, rotation, and translation)
Magick::DrawableAffine::DrawableAffine ( double scaleX_, double scaleY_,
                                         double rotationX_, double rotationY_,
                                         double translationX_, double translationY_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "affine "
          << scaleX_
          << ","
          << rotationX_
          << ","
          << rotationY_
          << ","
          << scaleY_
          << ","
          << translationX_
          << ","
          << translationY_
          << ends;
  _primitive.assign( buffer );
  cout << "Primitive: \"" << _primitive << "\"" << endl;
}

// Angle (text drawing angle)
Magick::DrawableAngle::DrawableAngle ( double angle_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "angle "
          << angle_
          << ends;
  _primitive.assign( buffer );
}

// Arc
Magick::DrawableArc::DrawableArc ( double startX_, double startY_,
                                   double endX_, double endY_,
                                   double startDegrees_, double endDegrees_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "arc"
          << " " << Coordinate(startX_,startY_)
          << " " << Coordinate(endX_,endY_)
          << " " << Coordinate(startDegrees_,endDegrees_)
          << ends;
  _primitive.assign( buffer );
}

// Bezier curve (Coordinate list must contain at least three members)
Magick::DrawableBezier::DrawableBezier ( const std::list<Magick::Coordinate> &coordinates_ )
{
  list_arg_impl( "bezier", coordinates_ );
}

// Circle
Magick::DrawableCircle::DrawableCircle ( double originX_,
                                         double originY_,
                                         double perimX_,
                                         double perimY_ )
{
  two_arg_impl( "circle",
                Coordinate( originX_, originY_ ),
                Coordinate( perimX_, perimY_ ) );
}

// Colorize at point using PaintMethod
Magick::DrawableColor::DrawableColor ( double x_, double y_,
                                       Magick::PaintMethod paintMethod_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  
  buffstr << "color " << Coordinate(x_,y_) << " ";
  
  switch ( paintMethod_ )
    {
    case PointMethod :
      buffstr << "point";
      break;
    case ReplaceMethod :
      buffstr << "replace";
      break;
    case FloodfillMethod :
      buffstr << "floodfill";
      break;
    case FillToBorderMethod :
      buffstr << "filltoborder";
      break;
    case ResetMethod :
      buffstr << "reset";
      break;
    default :
      {
	buffstr << "point";
      }
    }
  buffstr << ends;

  _primitive.assign(buffer);
}

// Decoration (text decoration)
Magick::DrawableDecoration::DrawableDecoration ( Magick::DecorationType decoration_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "decorate ";

  switch ( decoration_ )
    {
    case NoDecoration:
      buffstr << "none";
      break;
    case UnderlineDecoration:
      buffstr << "underline";
      break;
    case OverlineDecoration:
      buffstr << "overline";
      break;
    case LineThroughDecoration:
      buffstr << "line-through";
      break;
    default :
      {
        buffstr << "none";
      }
    }
  buffstr << ends;

  _primitive.assign( buffer );
}

// Ellipse
Magick::DrawableEllipse::DrawableEllipse ( double originX_,
                                           double originY_, 
                                           double width_,
                                           double height_,
                                           double arcStart_,
                                           double arcEnd_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "ellipse " << Coordinate(originX_,originY_)
	  << " " << width_ << "," << height_
	  << " " << arcStart_ << "," << arcEnd_
	  << ends;
  _primitive.assign( buffer );
}

// Specify drawing fill color
Magick::DrawableFillColor::DrawableFillColor ( const Magick::Color &color_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "fill "
          << string(color_)
          << ends;
  _primitive.assign( buffer );
}

// Specify drawing fill opacity
Magick::DrawableFillOpacity::DrawableFillOpacity ( double opacity_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "fill-opacity "
          << opacity_
          << ends;
  _primitive.assign( buffer );
}

// Specify text font
Magick::DrawableFont::DrawableFont ( const std::string &font_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "font \""
          << font_
          << "\""
          << ends;
  _primitive.assign( buffer );
  cout << "Primitive: \"" << _primitive << "\"" << endl;
}

// Specify text positioning gravity
Magick::DrawableGravity::DrawableGravity ( Magick::GravityType gravity_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "gravity ";

  switch ( gravity_ )
    {
      case ForgetGravity :
        {
          buffstr << "ForgetGravity";
          break;
        }
      case NorthWestGravity :
        {
          buffstr << "NorthWestGravity";
          break;
        }
      case NorthGravity :
        {
          buffstr << "NorthGravity";
          break;
        }
      case NorthEastGravity :
        {
          buffstr << "NorthEastGravity";
          break;
        }
      case WestGravity :
        {
          buffstr << "WestGravity";
          break;
        }
      case CenterGravity :
        {
          buffstr << "CenterGravity";
          break;
        }
      case EastGravity :
        {
          buffstr << "EastGravity";
          break;
        }
      case SouthWestGravity :
        {
          buffstr << "SouthWestGravity";
          break;
        }
      case SouthGravity :
        {
          buffstr << "SouthGravity";
          break;
        }
      case SouthEastGravity :
        {
          buffstr << "SouthEastGravity";
          break;
        }
      case StaticGravity :
        {
          buffstr << "StaticGravity";
          break;
        }

    default :
      {
        buffstr << "ForgetGravity";
      }
    }
  buffstr << ends;

  _primitive.assign( buffer );
}

// Draw image at point
Magick::DrawableImage::DrawableImage ( double x_,
                                       double y_,
                                       double width_,
                                       double height_,
                                       const std::string &image_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "image "
          << Coordinate( x_, y_)
          << " " << Coordinate( width_, height_)
          << " \"";
  for ( unsigned int i = 0; i < image_.length(); ++i )
    {
      if ( image_[i] == '"' )
	buffstr << "\\";
      buffstr << image_[i];
    }
  buffstr  << "\"" << ends;
  _primitive.assign( buffer );
}

// Line
Magick::DrawableLine::DrawableLine ( double startX_,
                                     double startY_,
                                     double endX_,
                                     double endY_ )
{
  two_arg_impl( "line",
                Coordinate( startX_, startY_ ),
                Coordinate( endX_, endY_ ) );
}

// Change pixel matte value to transparent using PaintMethod
Magick::DrawableMatte::DrawableMatte ( double x_, double y_,
                                       Magick::PaintMethod paintMethod_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  
  buffstr << "matte " << Coordinate( x_, y_ ) << " ";
  
  switch ( paintMethod_ )
    {
    case PointMethod :
      buffstr << "point";
      break;
    case ReplaceMethod :
      buffstr << "replace";
      break;
    case FloodfillMethod :
      buffstr << "floodfill";
      break;
    case FillToBorderMethod :
      buffstr << "filltoborder";
      break;
    case ResetMethod :
      buffstr << "reset";
      break;
    default :
      {
	buffstr << "point";
      }
    }
  buffstr << ends;

  _primitive.assign(buffer);
}

// Point
Magick::DrawablePoint::DrawablePoint ( double x_,
                                       double y_ )
{
  one_arg_impl( "point", Coordinate( x_, y_ ) );
}

// Text pointsize
Magick::DrawablePointSize::DrawablePointSize( double pointSize_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "pointsize "
          << pointSize_
          << ends;

  _primitive.assign( buffer );
}

// Polygon (Coordinate list must contain at least three members)
Magick::DrawablePolygon::DrawablePolygon ( const std::list<Magick::Coordinate> &coordinates_ )
{
  list_arg_impl( "polygon", coordinates_ );
}

// Polyline (Coordinate list must contain at least three members)
Magick::DrawablePolyline::DrawablePolyline ( const std::list<Magick::Coordinate> &coordinates_ )
{
  list_arg_impl( "polyline", coordinates_ );
}

// Rectangle
Magick::DrawableRectangle::DrawableRectangle ( double upperLeftX_,
                                               double upperLeftY_,
                                               double lowerRightX_,
                                               double lowerRightY_ )
{
  two_arg_impl( "rectangle",
                Coordinate( upperLeftX_, upperLeftY_ ),
                Coordinate( lowerRightX_, lowerRightY_ ) );
}

// Apply Rotation
Magick::DrawableRotation::DrawableRotation ( double angle_ )
  : DrawableAffine( -cos(DegreesToRadians(fmod(angle_,360.0))), // affine[0]
                    -cos(DegreesToRadians(fmod(angle_,360.0))), // affine[3]
                    sin(DegreesToRadians(fmod(angle_,360.0))),  // affine[1]
                    -sin(DegreesToRadians(fmod(angle_,360.0))), // affine[2]
                    0,
                    0
                    )
{
  // FIXME, should be based on defaults from ImageInfo?
}


// Round Rectangle
Magick::DrawableRoundRectangle::DrawableRoundRectangle
( double centerX_, double centerY_,
  double width_, double hight_,
  double cornerWidth_, double cornerHeight_ )
{                                                      
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "roundRectangle"
          << " " << Coordinate(centerX_,centerY_)
          << " " << Coordinate(width_,hight_)
          << " " << Coordinate(cornerWidth_,cornerHeight_)
          << ends;
  _primitive.assign( buffer );
}

// Apply Scaling
Magick::DrawableScaling::DrawableScaling ( double x_, double y_ )
  : DrawableAffine( x_, y_, 0, 0, 0, 0 )
{
  // FIXME, should be based on defaults from ImageInfo?
}

// Stroke color
Magick::DrawableStrokeColor::DrawableStrokeColor ( const Magick::Color &color_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "stroke "
          << string(color_)
          << ends;

  _primitive.assign( buffer );
}

// Stroke opacity
Magick::DrawableStrokeOpacity::DrawableStrokeOpacity ( double opacity_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "stroke-opacity "
          << opacity_
          << ends;

  _primitive.assign( buffer );
}

// Stroke width
Magick::DrawableStrokeWidth::DrawableStrokeWidth ( double width_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "stroke-width "
          << width_
          << ends;

  _primitive.assign( buffer );
}

// Draw text at point
Magick::DrawableText::DrawableText ( double x_,
                                     double y_,
                                     std::string text_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "text " << Coordinate( x_, y_)
	  << " \"";
  for ( unsigned int i = 0; i < text_.length(); ++i )
    {
      if ( text_[i] == '"' )
	buffstr << "\\";
      buffstr << text_[i];
    }
  buffstr << "\"" << ends;

  _primitive.assign( buffer );
}

// Apply Translation
Magick::DrawableTranslation::DrawableTranslation ( double x_, double y_ )
  : DrawableAffine( 1.0, 1.0, 0, 0, x_, y_ )
{
  // FIXME, should be based on defaults from ImageInfo?
}
