// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001
//
// Color Implementation
//

#define MAGICK_IMPLEMENTATION

#include <string>
#include <iostream>
#include <iomanip>
#include <strstream>

using namespace std;

#include "Magick++/Color.h"
#include "Magick++/Functions.h"
#include "Magick++/Exception.h"

//
// Color operator fuctions
//
int Magick::operator == ( const Magick::Color& left_,
			  const Magick::Color& right_ )
{
  return ( ( left_.isValid()      == right_.isValid() ) && 
	   ( left_.redQuantum()   == right_.redQuantum() ) &&
	   ( left_.greenQuantum() == right_.greenQuantum() ) &&
	   ( left_.blueQuantum()  == right_.blueQuantum() )
	  );
}
int Magick::operator != ( const Magick::Color& left_,
			  const Magick::Color& right_ )
{
  return ( ! (left_ == right_) );
}
int Magick::operator >  ( const Magick::Color& left_,
			  const Magick::Color& right_ )
{
  return ( !( left_ < right_ ) && ( left_ != right_ ) );
}
// Compare color intensities (similar to ImageMagick Intensity macro)
int Magick::operator <  ( const Magick::Color& left_,
			  const Magick::Color& right_ )
{
  return (
	  (
	   static_cast<unsigned int>
	   ( left_.redQuantum()   * 77 +
	     left_.greenQuantum() * 150 +
	     left_.blueQuantum()  * 29
	     ) >> 8
	   )
	  <
	  (
	   static_cast<unsigned int>
	   ( right_.redQuantum()   * 77 +
	     right_.greenQuantum() * 150 +
	     right_.blueQuantum()  * 29
	     ) >> 8
	   )
	  );
}
int Magick::operator >= ( const Magick::Color& left_,
			  const Magick::Color& right_ )
{
  return ( ( left_ > right_ ) || ( left_ == right_ ) );
}
int Magick::operator <= ( const Magick::Color& left_,
			  const Magick::Color& right_ )
{
  return ( ( left_ < right_ ) || ( left_ == right_ ) );
}

//
// Color Implementation
//

// Copy constructor
Magick::Color::Color ( const Magick::Color & color_ )
  : _pixel( new PixelPacket ),
    _pixelOwn( true ),
    _pixelType( color_._pixelType )
{
  *_pixel    = *color_._pixel;
}

// Assignment operator
Magick::Color& Magick::Color::operator = ( const Magick::Color& color_ )
{
  // If not being set to ourself
  if ( this != &color_ )
    {
      // Copy pixel value
      *_pixel = *color_._pixel;

      // Copy pixel type
      _pixelType = color_._pixelType;
    }
  return *this;
}

// Set color via X11 color specification string
const Magick::Color& Magick::Color::operator = ( std::string x11color_ )
{
  initPixel();
  PixelPacket target_color;
  if ( QueryColorDatabase( x11color_.c_str(), &target_color ) )
    {
      redQuantum( target_color.red );
      greenQuantum( target_color.green );
      blueQuantum( target_color.blue );
      alphaQuantum( target_color.opacity );

      if ( target_color.opacity > OpaqueOpacity )
	_pixelType = RGBAPixel;
      else
	_pixelType = RGBPixel;
    }
  else
    {
      std::string message = "Color \"";
      message += x11color_;
      message += "\" not valid";
      throw ErrorOption( message );
    }

  return *this;
}

// Set color via X11 color specification C string
const Magick::Color& Magick::Color::operator = ( const char * x11color_ )
{
  *this = std::string(x11color_);
  return *this;
}

// Return X11 color specification string
Magick::Color::operator std::string() const
{

  if ( !isValid() )
    return std::string("none");

  char colorbuf[17];
  ostrstream colorstr( colorbuf, sizeof(colorbuf ));
  colorstr.setf(ios::right | ios::uppercase );
  colorstr.fill('0');

#if QuantumSize == 8
  colorstr << "#"
	   << hex
	   << setw(2) << static_cast<unsigned int>(redQuantum())
	   << setw(2) << static_cast<unsigned int>(greenQuantum())
	   << setw(2) << static_cast<unsigned int>(blueQuantum());
  if ( _pixelType == RGBAPixel )
    colorstr << hex << setw(2) << static_cast<unsigned int>(alphaQuantum());
#else
  colorstr << "#"
	   << hex
	   << setw(4) << static_cast<unsigned int>(redQuantum())
	   << setw(4) << static_cast<unsigned int>(greenQuantum())
	   << setw(4) << static_cast<unsigned int>(blueQuantum());
  if ( _pixelType == RGBAPixel )
    colorstr << hex << setw(4) << (unsigned int)alphaQuantum();
#endif

  colorstr << ends;

  return std::string(colorbuf);
}

// Construct color via ImageMagick PixelPacket
Magick::Color::Color ( PixelPacket &color_ )
  : _pixel(new PixelPacket),
    _pixelOwn(true),	    // We allocated this pixel
    _pixelType(RGBPixel)  // RGB pixel by default
{
  *_pixel = color_;

  if ( color_.opacity != OpaqueOpacity )
    _pixelType = RGBAPixel;
}

// Set color via ImageMagick PixelPacket
const Magick::Color& Magick::Color::operator= ( MagickLib::PixelPacket &color_ )
{
  *_pixel = color_;
  if ( color_.opacity != OpaqueOpacity )
    _pixelType = RGBAPixel;
  else
    _pixelType = RGBPixel;
  return *this;
}

// Return ImageMagick PixelPacket struct based on color.
Magick::Color::operator MagickLib::PixelPacket () const
{
  return *_pixel;
}


//
// ColorHSL Implementation
//

Magick::ColorHSL::ColorHSL ( double hue_,
			     double saturation_,
			     double luminosity_ )
  : Color ()
{
  Quantum red, green, blue;

  HSLTransform ( hue_,
		 saturation_,
		 luminosity_,
		 &red,
		 &green,
		 &blue );

  redQuantum   ( red );
  greenQuantum ( green );
  blueQuantum  ( blue );

}

// Null constructor
Magick::ColorHSL::ColorHSL ( )
  : Color ()
{
}

// Copy constructor from base class
Magick::ColorHSL::ColorHSL ( const Magick::Color & color_ )
  : Color( color_ )
{
}

// Destructor
Magick::ColorHSL::~ColorHSL ( )
{
  // Nothing to do
}

void Magick::ColorHSL::hue ( double hue_ )
{
  double hue_val, saturation_val, luminosity_val;
  TransformHSL ( redQuantum(),
		 greenQuantum(),
		 blueQuantum(),
		 &hue_val,
		 &saturation_val,
		 &luminosity_val );

  hue_val = hue_;

  Quantum red, green, blue;
  HSLTransform ( hue_val,
		 saturation_val,
		 luminosity_val,
		 &red,
		 &green,
		 &blue
		 );

  redQuantum   ( red );
  greenQuantum ( green );
  blueQuantum  ( blue );
}

double Magick::ColorHSL::hue ( void ) const
{
  double hue_val, saturation_val, luminosity_val;
  TransformHSL ( redQuantum(),
		 greenQuantum(),
		 blueQuantum(),
		 &hue_val,
		 &saturation_val,
		 &luminosity_val );
  return hue_val;
}

void Magick::ColorHSL::saturation ( double saturation_ )
{
  double hue_val, saturation_val, luminosity_val;
  TransformHSL ( redQuantum(),
		 greenQuantum(),
		 blueQuantum(),
		 &hue_val,
		 &saturation_val,
		 &luminosity_val );
  
  saturation_val = saturation_;
  
  Quantum red, green, blue;
  HSLTransform ( hue_val,
		 saturation_val,
		 luminosity_val,
		 &red,
		 &green,
		 &blue
		 );

  redQuantum   ( red );
  greenQuantum ( green );
  blueQuantum  ( blue );
}

double Magick::ColorHSL::saturation ( void ) const
{
  double hue_val, saturation_val, luminosity_val;
  TransformHSL ( redQuantum(),
		 greenQuantum(),
		 blueQuantum(),
		 &hue_val,
		 &saturation_val,
		 &luminosity_val );
  return saturation_val;
}

void Magick::ColorHSL::luminosity ( double luminosity_ )
{
  double hue_val, saturation_val, luminosity_val;
  TransformHSL ( redQuantum(),
		 greenQuantum(),
		 blueQuantum(),
		 &hue_val,
		 &saturation_val,
		 &luminosity_val );
  
  luminosity_val = luminosity_;
  
  Quantum red, green, blue;
  HSLTransform ( hue_val,
		 saturation_val,
		 luminosity_val,
		 &red,
		 &green,
		 &blue
		 );
  
  redQuantum   ( red );
  greenQuantum ( green );
  blueQuantum  ( blue );
}

double Magick::ColorHSL::luminosity ( void ) const
{
  double hue_val, saturation_val, luminosity_val;
  TransformHSL ( redQuantum(),
		 greenQuantum(),
		 blueQuantum(),
		 &hue_val,
		 &saturation_val,
		 &luminosity_val );
  return luminosity_val;
}

// Assignment from base class
Magick::ColorHSL& Magick::ColorHSL::operator = ( const Magick::Color& color_ )
{
  *static_cast<Magick::Color*>(this) = color_;
  return *this;
}

//
// ColorGray Implementation
//
Magick::ColorGray::ColorGray ( double shade_ )
  : Color ( scaleDoubleToQuantum( shade_ ),
	    scaleDoubleToQuantum( shade_ ),
	    scaleDoubleToQuantum( shade_ ) )
{
}

// Null constructor
Magick::ColorGray::ColorGray ( void )
  : Color ()
{
}

// Copy constructor from base class
Magick::ColorGray::ColorGray ( const Magick::Color & color_ )
  : Color( color_ )
{
}

// Destructor
Magick::ColorGray::~ColorGray ()
{
  // Nothing to do
}

void Magick::ColorGray::shade ( double shade_ )
{
  Quantum gray = scaleDoubleToQuantum( shade_ );
  redQuantum   ( gray );
  greenQuantum ( gray );
  blueQuantum  ( gray );
}

double Magick::ColorGray::shade ( void ) const
{
  return scaleQuantumToDouble ( greenQuantum() );
}

// Assignment from base class
Magick::ColorGray& Magick::ColorGray::operator = ( const Magick::Color& color_ )
{
  *static_cast<Magick::Color*>(this) = color_;
  return *this;
}

//
// ColorMono Implementation
//
Magick::ColorMono::ColorMono ( bool mono_  )
  : Color ( ( mono_ ? MaxRGB : 0 ),
	    ( mono_ ? MaxRGB : 0 ),
	    ( mono_ ? MaxRGB : 0 ) )
{
}

// Null constructor
Magick::ColorMono::ColorMono ( void )
  : Color ()
{
}

// Copy constructor from base class
Magick::ColorMono::ColorMono ( const Magick::Color & color_ )
  : Color( color_ )
{
}

// Destructor
Magick::ColorMono::~ColorMono ()
{
  // Nothing to do
}

void Magick::ColorMono::mono ( bool mono_ )
{
  redQuantum   ( mono_ ? MaxRGB : 0 );
  greenQuantum ( mono_ ? MaxRGB : 0 );
  blueQuantum  ( mono_ ? MaxRGB : 0 );
}

bool Magick::ColorMono::mono ( void ) const
{
  if ( greenQuantum() )
    return true;
  else
    return false;
}

// Assignment from base class
Magick::ColorMono& Magick::ColorMono::operator = ( const Magick::Color& color_ )
{
  *static_cast<Magick::Color*>(this) = color_;
  return *this;
}

//
// ColorRGB Implementation
//

// Construct from red, green, and blue, components
Magick::ColorRGB::ColorRGB ( double red_,
			     double green_,
			     double blue_ )
  : Color ( scaleDoubleToQuantum(red_),
	    scaleDoubleToQuantum(green_),
	    scaleDoubleToQuantum(blue_) )
{
}
// Null constructor
Magick::ColorRGB::ColorRGB ( void )
  : Color ()
{
}
// Copy constructor from base class
Magick::ColorRGB::ColorRGB ( const Magick::Color & color_ )
  : Color( color_ )
{
}
// Destructor
Magick::ColorRGB::~ColorRGB ( void )
{
  // Nothing to do
}

// Assignment from base class
Magick::ColorRGB& Magick::ColorRGB::operator = ( const Magick::Color& color_ )
{
  *static_cast<Magick::Color*>(this) = color_;
  return *this;
}

//
// ColorYUV Implementation
//

//           R = Y          +1.13980*V
//           G = Y-0.39380*U-0.58050*V
//           B = Y+2.02790*U
//
//         U and V, normally -0.5 through 0.5, must be normalized to the range 0
//         through MaxRGB.
//
//           Y =  0.29900*R+0.58700*G+0.11400*B
//           U = -0.14740*R-0.28950*G+0.43690*B
//           V =  0.61500*R-0.51500*G-0.10000*B
//
//         U and V, normally -0.5 through 0.5, are normalized to the range 0
//         through MaxRGB.  Note that U = 0.493*(B-Y), V = 0.877*(R-Y).
//

// Construct from color components
Magick::ColorYUV::ColorYUV ( double y_,
			     double u_,
			     double v_ )
  : Color ( scaleDoubleToQuantum(y_ + 1.13980 * v_ ),
	    scaleDoubleToQuantum(y_ - 0.39380 * u_ - 0.58050 * v_ ),
	    scaleDoubleToQuantum(y_ + 2.02790 * u_ ) )
{
}
// Null constructor
Magick::ColorYUV::ColorYUV ( void )
  : Color ()
{
}
// Copy constructor from base class
Magick::ColorYUV::ColorYUV ( const Magick::Color & color_ )
  : Color( color_ )
{
}
// Destructor
Magick::ColorYUV::~ColorYUV ( void )
{
  // Nothing to do
}

void Magick::ColorYUV::u ( double u_ )
{
  double V = v();
  double Y = y();

  redQuantum   ( scaleDoubleToQuantum( Y + 1.13980 * V ) );
  greenQuantum ( scaleDoubleToQuantum( Y - 0.39380 * u_ - 0.58050 * V ) );
  blueQuantum  ( scaleDoubleToQuantum( Y + 2.02790 * u_ ) );
}

double Magick::ColorYUV::u ( void ) const
{
  return scaleQuantumToDouble( -0.14740 * redQuantum() - 0.28950 *
			       greenQuantum() + 0.43690 * blueQuantum() );
}

void Magick::ColorYUV::v ( double v_ )
{
  double U = u();
  double Y = y();

  redQuantum   ( scaleDoubleToQuantum( Y + 1.13980 * v_ ) );
  greenQuantum ( scaleDoubleToQuantum( Y - 0.39380 * U - 0.58050 * v_ ) );
  blueQuantum  ( scaleDoubleToQuantum( Y + 2.02790 * U ) );
}

double Magick::ColorYUV::v ( void ) const
{
  return scaleQuantumToDouble(  0.61500 * redQuantum() - 0.51500 *
				greenQuantum() - 0.10000 * blueQuantum() );
}

void Magick::ColorYUV::y ( double y_ )
{
  double U = u();
  double V = v();

  redQuantum   ( scaleDoubleToQuantum( y_ + 1.13980 * V ) );
  greenQuantum ( scaleDoubleToQuantum( y_ - 0.39380 * U - 0.58050 * V ) );
  blueQuantum  ( scaleDoubleToQuantum( y_ + 2.02790 * U ) );
}

double Magick::ColorYUV::y ( void ) const
{
  return scaleQuantumToDouble(  0.29900 * redQuantum() + 0.58700 *
				greenQuantum() + 0.11400 * blueQuantum() );
}

// Assignment from base class
Magick::ColorYUV& Magick::ColorYUV::operator = ( const Magick::Color& color_ )
{
  *static_cast<Magick::Color*>(this) = color_;
  return *this;
}
