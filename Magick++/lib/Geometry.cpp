// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Geometry implementation
//

#define MAGICK_IMPLEMENTATION

#include <string>
#include <iostream>
#include <iomanip>
#include <strstream>

#include <ctype.h> // for isdigit

using namespace std;

#include "Magick++/Geometry.h"
#include "Magick++/Functions.h"

int Magick::operator == ( const Magick::Geometry& left_,
			  const Magick::Geometry& right_ )
{
  return (
	  ( left_.isValid()   == right_.isValid() ) &&
	  ( left_.width()     == right_.width() ) &&
	  ( left_.height()    == right_.height() ) &&
	  ( left_.xOff()      == right_.xOff() ) &&
	  ( left_.yOff()      == right_.yOff() ) &&
	  ( left_.xNegative() == right_.xNegative() ) &&
	  ( left_.yNegative() == right_.yNegative() ) &&
	  ( left_.percent()   == right_.percent() ) &&
	  ( left_.aspect()    == right_.aspect() ) &&
	  ( left_.greater()   == right_.greater() ) &&
	  ( left_.less()      == right_.less() )
	  );
}
int Magick::operator != ( const Magick::Geometry& left_,
			  const Magick::Geometry& right_ )
{
  return ( ! (left_ == right_) );
}
int Magick::operator >  ( const Magick::Geometry& left_,
			  const Magick::Geometry& right_ )
{
  return ( !( left_ < right_ ) && ( left_ != right_ ) );
}
int Magick::operator <  ( const Magick::Geometry& left_,
			  const Magick::Geometry& right_ )
{
  return (
	  ( left_.width() * left_.height() )
	  <
	  ( right_.width() * right_.height() )
	  );
}
int Magick::operator >= ( const Magick::Geometry& left_,
			  const Magick::Geometry& right_ )
{
  return ( ( left_ > right_ ) || ( left_ == right_ ) );
}
int Magick::operator <= ( const Magick::Geometry& left_,
			  const Magick::Geometry& right_ )
{
  return ( ( left_ < right_ ) || ( left_ == right_ ) );
}

// Construct using parameterized arguments
Magick::Geometry::Geometry ( unsigned int width_,
			     unsigned int height_,
			     unsigned int xOff_,
			     unsigned int yOff_,
			     bool xNegative_,
			     bool yNegative_ )
  : _width( width_ ),
    _height( height_ ),
    _xOff( xOff_ ),
    _yOff( yOff_ ),
    _xNegative( xNegative_ ),
    _yNegative( yNegative_ ),
    _isValid( true ),
    _percent( false ),
    _aspect( false ),
    _greater( false ),
    _less( false )
{
}

// Assignment from C++ string
Magick::Geometry::Geometry ( const std::string geometry_ )
  : _width( 0 ),
    _height( 0 ),
    _xOff( 0 ),
    _yOff( 0 ),
    _xNegative( false ),
    _yNegative( false ),
    _isValid( false ),
    _percent( false ),
    _aspect( false ),
    _greater( false ),
    _less( false )
{
  *this = geometry_; // Use assignment operator

}


// Assignment from C character string
Magick::Geometry::Geometry ( const char * geometry_ )
  : _width( 0 ),
    _height( 0 ),
    _xOff( 0 ),
    _yOff( 0 ),
    _xNegative( false ),
    _yNegative( false ),
    _isValid( false ),
    _percent( false ),
    _aspect( false ),
    _greater( false ),
    _less( false )
{
  *this = geometry_; // Use assignment operator
}

// Copy constructor
Magick::Geometry::Geometry ( const Geometry & geometry_ )
  :  _width( geometry_._width ),
     _height( geometry_._height ),
     _xOff( geometry_._xOff ),
     _yOff( geometry_._yOff ),
     _xNegative( geometry_._xNegative ),
     _yNegative( geometry_._yNegative ),
     _isValid ( geometry_._isValid ),
     _percent( geometry_._percent ),
     _aspect( geometry_._aspect ),
     _greater( geometry_._greater ),
     _less( geometry_._less )
{
}

// Default constructor
Magick::Geometry::Geometry ( void )
  : _width( 0 ),
    _height( 0 ),
    _xOff( 0 ),
    _yOff( 0 ),
    _xNegative( false ),
    _yNegative( false ),
    _isValid ( false ),
    _percent( false ),
    _aspect( false ),
    _greater( false ),
    _less( false )
{
}

/* virtual */ Magick::Geometry::~Geometry ( void )
{
  // Nothing to do
}

Magick::Geometry Magick::Geometry::operator = ( const Geometry& geometry_ )
{
  // If not being set to ourself
  if ( this != &geometry_ )
    {
      _width = geometry_._width;
      _height = geometry_._height;
      _xOff = geometry_._xOff;
      _yOff = geometry_._yOff;
      _xNegative = geometry_._xNegative;
      _yNegative = geometry_._yNegative;
      _isValid = geometry_._isValid;
      _percent = geometry_._percent;
      _aspect = geometry_._aspect;
      _greater = geometry_._greater;
      _less = geometry_._less;
    }
  return *this;
}

// Set value via geometry string
/* virtual */ const Magick::Geometry&
Magick::Geometry::operator = ( const std::string &geometry_ )
{
  char
    geom[MaxTextExtent];

  // If argument does not start with digit, presume that it is a
  // page-size specification that needs to be converted to an
  // equivalent geometry specification using PostscriptGeometry()
  strcpy(geom,geometry_.c_str());
  if ( geom[0] != '-' &&
       geom[0] != '+' &&
       !isdigit(static_cast<int>(geom[0])))
    {
      char *pageptr = PostscriptGeometry( geom );
      if ( pageptr )
	{
	  strcpy(geom,pageptr);
	  LiberateMemory( (void **) &pageptr );
	}
    }

  long x = 0;
  long y = 0;
  unsigned long width_val = 0;
  unsigned long height_val = 0;
  int flags = GetGeometry ( geom,
			    &x, &y, &width_val, &height_val );

  if ( ( flags & WidthValue ) != 0 )
    {
      _width = width_val;
      isValid( true );
    }

  if ( ( flags & HeightValue ) != 0 )
    _height = height_val;

  if ( ( flags & XValue ) != 0 )
    {
      _xOff = ::abs(x);
      isValid( true );
    }

  if ( ( flags & YValue ) != 0 )
    {
      _yOff = ::abs(y);
      isValid( true );
    }

  if ( ( flags & XNegative ) != 0 )	
    _xNegative = true;

  if ( ( flags & YNegative ) != 0 )
    _yNegative = true;

  if ( ( flags & PercentValue ) != 0 )
    _percent = true;

  if ( ( flags & AspectValue ) != 0 )
    _aspect = true;

  if ( ( flags & LessValue ) != 0 )
    _less = true;

  if ( ( flags & GreaterValue ) != 0 )
    _greater = true;

  return *this;
}


// Set value via geometry C string
/* virtual */ const Magick::Geometry& Magick::Geometry::operator = ( const char * geometry_ )
{
  *this = std::string(geometry_);
  return *this;
}

// Return geometry string
Magick::Geometry::operator std::string() const
{
  char buffer[32];
  ostrstream geomStr( buffer, sizeof( buffer ) - 1 );

  if ( _width )
    geomStr << _width;

  if ( _width && _height )
    geomStr << "x"
	    << _height;

  if ( _xOff || _yOff )
    {
      if ( _xNegative )
	geomStr << "-";
      else
	geomStr << "+";
      geomStr << _xOff;

      if ( _yNegative )
	geomStr << "-";
      else
	geomStr << "+";
      geomStr << _yOff;
    }

  if ( _percent )
    geomStr << "%";

  if ( _aspect )
    geomStr << "!";

  if ( _greater )
    geomStr << ">";

  if ( _less )
    geomStr << "<";

  geomStr << ends;

  std::string geometry = buffer;
  return geometry;
}

// print object to a stream
ostream& operator<<(ostream& stream_, const Magick::Geometry& geometry_)
{
  std::string geomStr( geometry_ );
  stream_ << geomStr;
  return stream_;
}

// Construct from RectangleInfo
#undef abs
#define abs(x) ((x) >= 0 ? (x) : -(x))
Magick::Geometry::Geometry ( const MagickLib::RectangleInfo &rectangle_ )
  : _width(static_cast<unsigned int>(abs(rectangle_.width))),
    _height(static_cast<unsigned int>(abs(rectangle_.height))),
    _xOff(static_cast<unsigned int>(abs(rectangle_.x))),
    _yOff(static_cast<unsigned int>(abs(rectangle_.y))),
    _xNegative(rectangle_.x < 0 ? true : false),
    _yNegative(rectangle_.y < 0 ? true : false),
    _isValid(true),
    _percent(false),
    _aspect(false),
    _greater(false),
    _less(false)
{    
}

// Return an ImageMagick RectangleInfo struct
Magick::Geometry::operator MagickLib::RectangleInfo() const
{
  RectangleInfo rectangle;
  rectangle.width = _width;
  rectangle.height = _height;
  _xNegative ? rectangle.x = -_xOff : rectangle.x = _xOff;
  _yNegative ? rectangle.y = -_yOff : rectangle.y = _yOff;
  return rectangle;
}
