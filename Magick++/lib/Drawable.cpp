// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Implementation of Drawable (Graphic objects)
//

#define MAGICK_IMPLEMENTATION

#include <string>
#include <cstdarg>
#include <iostream>
#include <strstream>

using namespace std;

#include "Magick++/Drawable.h"


std::ostream& Magick::operator<<( std::ostream& stream_,
				  const Magick::Coordinate &coordinate_)
{
  stream_ << coordinate_._x << "," << coordinate_._y;
  return stream_;
}

//
// Drawable implementation
//


//
// Drawable Objects
//

// Ellipse
void Magick::Drawable::ellipse( const Magick::Coordinate &originCoordinate_, 
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
void Magick::Drawable::fillEllipse( const Magick::Coordinate &originCoordinate_, 
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

// Colorize at point using PaintMethod
void Magick::Drawable::color( const Magick::Coordinate &coordinate_,
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
void Magick::Drawable::matte( const Magick::Coordinate &coordinate_,
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
void Magick::Drawable::text ( const Magick::Coordinate &coordinate_,
			      std::string text_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "text " << coordinate_
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

// Draw image at point
void Magick::Drawable::image ( const Magick::Coordinate &coordinate_,
			       const std::string &image_ )
{
  char buffer[MaxTextExtent + 1];
  ostrstream buffstr( buffer, sizeof(buffer));
  buffstr << "image " << coordinate_ << " \"";
  for ( unsigned int i = 0; i < image_.length(); ++i )
    {
      if ( image_[i] == '"' )
	buffstr << "\\";
      buffstr << image_[i];
    }
  buffstr  << "\"" << ends;
  _primitive.assign( buffer );
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
