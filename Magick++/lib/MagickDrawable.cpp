// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Implementation of Drawable (Graphic objects)
//

#define MAGICK_IMPLEMENTATION

#include <string>
#include <cstdarg>
#include <iostream>
#include <strstream>

using namespace std;

#include <MagickDrawable.h>

Magick::Coordinate::Coordinate ( void )
  : _x(0),
    _y(0)
{
}

Magick::Coordinate::Coordinate ( double x_, double y_ )
  : _x(x_),
    _y(y_)
{
}

Magick::Coordinate::~Coordinate ()
{
  // Nothing to do
}

void Magick::Coordinate::x ( double x_ )
{
  _x = x_;
}

double Magick::Coordinate::x ( void ) const
{
  return _x;
}

void Magick::Coordinate::y ( double y_ )
{
  _y = y_;
}

double Magick::Coordinate::y ( void ) const
{
  return _y;
}

std::ostream& Magick::operator<<( std::ostream& stream_, const Magick::Coordinate& coordinate_)
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

// Completed draw primitive (for ImageMagick)
std::string Magick::Drawable::primitive ( void ) const
{
  // cout << _primitive << endl;
  return _primitive;
}

//
// Drawable Objects
//

// Point
void Magick::Drawable::point ( double x_,
			       double y_ )
{
  point( Coordinate( x_, y_ ) );
}
void Magick::Drawable::point ( Magick::Coordinate coordinate_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "point " << coordinate_ << ends;
  _primitive.assign(buffer);
}

// Line
void Magick::Drawable::line ( double startX_,
			      double startY_,
			      double endX_,
			      double endY_ )
{
  line( Coordinate( startX_, startY_ ), Coordinate( endX_, endY_ ) );
}
void Magick::Drawable::line ( Magick::Coordinate startCoordinate_,
			      Magick::Coordinate endCoordinate_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "line " << startCoordinate_ << " " << endCoordinate_ << ends;
  _primitive.assign( buffer );
}

// Rectangle
void Magick::Drawable::rectangle ( double upperLeftX_,
				   double upperLeftY_,
				   double lowerRightX_,
				   double lowerRightY )
{
  rectangle( Coordinate( upperLeftX_, upperLeftY_ ),
	     Coordinate( lowerRightX_, lowerRightY ) );
}
void Magick::Drawable::rectangle ( Magick::Coordinate upperLeftCoordinate_,
				   Magick::Coordinate lowerRightCoordinate_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "rectangle " << upperLeftCoordinate_ << " " << lowerRightCoordinate_ << ends;
  _primitive.assign( buffer );
}


  // Filled Rectangle
void Magick::Drawable::fillRectangle ( double upperLeftX_,
				       double upperLeftY_,
				       double lowerRightX_,
				       double lowerRightY )
{
  fillRectangle( Coordinate( upperLeftX_, upperLeftY_ ),
		 Coordinate( lowerRightX_, lowerRightY ) );
}
void Magick::Drawable::fillRectangle ( Magick::Coordinate upperLeftCoordinate_,
				       Magick::Coordinate lowerRightCoordinate_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "fillRectangle " << upperLeftCoordinate_ << " " << lowerRightCoordinate_ << ends;
  _primitive.assign( buffer );
}

// Circle
void Magick::Drawable::circle ( double originX_, double originY_,
				double perimX_, double perimY_ )
{
  circle( Coordinate( originX_, originY_ ),
	  Coordinate( perimX_, perimY_ ) );
}
void Magick::Drawable::circle ( Magick::Coordinate originCoordinate_,
				Magick::Coordinate perimCoordinate_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "circle " << originCoordinate_ << " " << perimCoordinate_ << ends;
  _primitive.assign( buffer );
}

// Filled Circle
void Magick::Drawable::fillCircle ( double originX_, double originY_,
				    double perimX_, double perimY_ )
{
  fillCircle( Coordinate( originX_, originY_ ),
	      Coordinate( perimX_, perimY_ ) );
}
void Magick::Drawable::fillCircle ( Magick::Coordinate originCoordinate_,
				    Magick::Coordinate perimCoordinate_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "fillCircle " << originCoordinate_ << " " << perimCoordinate_ << ends;
  _primitive.assign( buffer );
}

// Ellipse
void Magick::Drawable::ellipse ( double originX_, double originY_, 
				 double width_, double height_,
				 double arcStart_, double arcEnd_ )
{
  ellipse( Coordinate( originX_, originY_ ),
	   width_, height_,
	   arcStart_, arcEnd_ );
}
void Magick::Drawable::ellipse( Magick::Coordinate originCoordinate_, 
				double width_, double height_,
				double arcStart_, double arcEnd_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "ellipse " << originCoordinate_
	  << " " << width_ << "," << height_
	  << " " << arcStart_ << "," << arcEnd_
	  << ends;
  _primitive.assign( buffer );
}

// Filled Ellipse
void Magick::Drawable::fillEllipse ( double originX_,
				     double originY_, 
				     double width_,
				     double height_,
				     double arcStart_,
				     double arcEnd_ )
{
  fillEllipse( Coordinate( originX_, originY_ ),
	       width_, height_,
	       arcStart_, arcEnd_ );
}
void Magick::Drawable::fillEllipse( Magick::Coordinate originCoordinate_, 
				    double width_, double height_,
				    double arcStart_, double arcEnd_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "fillEllipse " << originCoordinate_
	  << " " << width_ << "," << height_
	  << " " << arcStart_ << "," << arcEnd_
	  << ends;
  _primitive.assign( buffer );
}

// Polygon (Coordinate list must contain at least three members)
void Magick::Drawable::polygon ( const std::list<Coordinate> &coordinates_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));

  buffstr << "polygon";

  list<Coordinate>::const_iterator p = coordinates_.begin();

  while ( p != coordinates_.end() )
    {
      buffstr << " "
	      << *p;
      p++;
    }

  buffstr << ends;
  _primitive.assign( buffer );
}

// Filled Polygon (Coordinate list must contain at least three members)
void Magick::Drawable::fillPolygon ( const std::list<Coordinate> &coordinates_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));

  buffstr << "fillPolygon";

  std::list<Coordinate>::const_iterator p = coordinates_.begin();

  while ( p != coordinates_.end() )
    {
      buffstr << " "
	      << *p;
      p++;
    }

  buffstr << ends;
  _primitive.assign( buffer );
}

// Colorize at point using PaintMethod
void Magick::Drawable::color ( double x_, double y_,
			       Magick::PaintMethod paintMethod_ )
{
  color( Coordinate( x_, y_ ), paintMethod_ );
}
void Magick::Drawable::color( Magick::Coordinate coordinate_,
			      Magick::PaintMethod paintMethod_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  
  buffstr << "color " << coordinate_ << " ";
  
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

// Change pixel matte value to transparent using PaintMethod
void Magick::Drawable::matte ( double x_, double y_,
			       Magick::PaintMethod paintMethod_ )
{
  matte( Coordinate( x_, y_ ), paintMethod_ );
}
void Magick::Drawable::matte( Magick::Coordinate coordinate_,
			      Magick::PaintMethod paintMethod_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  
  buffstr << "matte " << coordinate_ << " ";
  
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

// Draw text at point
void Magick::Drawable::text ( double x_, double y_, std::string text_ )
{
  text( Coordinate( x_, y_), text_ );
}


void Magick::Drawable::text ( Magick::Coordinate coordinate_,
			      std::string text_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "text " << coordinate_ << text_ << ends;
  _primitive.assign( buffer );
}

// Draw image at point
void Magick::Drawable::image ( double x_, double y_,
			       const std::string &image_ )
{
  image( Coordinate( x_, y_), image_ );
}
void Magick::Drawable::image ( Magick::Coordinate coordinate_,
			       const std::string &image_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "image " << coordinate_ << image_ << ends;
  _primitive.assign( buffer );
}
