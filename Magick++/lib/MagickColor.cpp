// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Color Implementation
//

#define MAGICK_IMPLEMENTATION

#include <string>
#include <iostream>
#include <iomanip>
#include <strstream>

using namespace std;

#include <MagickColor.h>
#include <MagickFunctions.h>

//
// Color operator fuctions
//
int Magick::operator == ( const Magick::Color& left_, const Magick::Color& right_ )
{
  return ( ( left_.isValid()      == right_.isValid() ) && 
	   ( left_.redQuantum()   == right_.redQuantum() ) &&
	   ( left_.greenQuantum() == right_.greenQuantum() ) &&
	   ( left_.blueQuantum()  == right_.blueQuantum() )
	  );
}
int Magick::operator != ( const Magick::Color& left_, const Magick::Color& right_ )
{
  return ( ! (left_ == right_) );
}
int Magick::operator >  ( const Magick::Color& left_, const Magick::Color& right_ )
{
  return ( !( left_ < right_ ) && ( left_ != right_ ) );
}
// Compare color intensities (similar to ImageMagick Intensity macro)
int Magick::operator <  ( const Magick::Color& left_, const Magick::Color& right_ )
{
  return (
	  (
	   (unsigned int)
	   ( left_.redQuantum()   * 77 +
	     left_.greenQuantum() * 150 +
	     left_.blueQuantum()  * 29
	     ) >> 8
	   )
	  <
	  (
	   (unsigned int)
	   ( right_.redQuantum()   * 77 +
	     right_.greenQuantum() * 150 +
	     right_.blueQuantum()  * 29
	     ) >> 8
	   )
	  );
}
int Magick::operator >= ( const Magick::Color& left_, const Magick::Color& right_ )
{
  return ( ( left_ > right_ ) || ( left_ == right_ ) );
}
int Magick::operator <= ( const Magick::Color& left_, const Magick::Color& right_ )
{
  return ( ( left_ < right_ ) || ( left_ == right_ ) );
}

//
// Color Implementation
//
Magick::Color::Color ( Quantum red_, Quantum green_, Quantum blue_ )
  : _red(0),
    _green(0),
    _blue(0),
    _isValid(false)
{
  redQuantum   ( red_   );
  greenQuantum ( green_ );
  blueQuantum  ( blue_  );
}

Magick::Color::Color ( const std::string x11color_ )
  : _red(0),
    _green(0),
    _blue(0),
    _isValid(false)
{
  *this = x11color_; // Use operator = implementation
}

Magick::Color::Color ( const char * x11color_ )
  : _red(0),
    _green(0),
    _blue(0),
    _isValid(false)
{
  *this = x11color_; // Use operator = implementation
}

Magick::Color::Color ( void )
  : _red(0),
    _green(0),
    _blue(0),
    _isValid(false)
{
}

Magick::Color::~Color( void )
{
  // Nothing to do
}

void  Magick::Color::redQuantum ( Quantum red_ )
{
  _red = (Quantum) (red_ > MaxRGB ? MaxRGB : red_);
  _isValid = true;
}

Magick::Quantum Magick::Color::redQuantum ( void ) const
{
  return _red;
}

void Magick::Color::greenQuantum ( Quantum green_ )
{
  _green = (Quantum) (green_ > MaxRGB ? MaxRGB : green_);
  _isValid = true;
}

Magick::Quantum  Magick::Color::greenQuantum ( void ) const
{
  return _green;
}

void  Magick::Color::blueQuantum ( Quantum blue_ )
{
  _blue = (Quantum) (blue_ > MaxRGB ? MaxRGB : blue_);
  _isValid = true;
}

Magick::Quantum Magick::Color::blueQuantum ( void ) const
{
  return _blue;
}

// Does object contain valid color?
void Magick::Color::isValid ( bool valid_ )
{
  _isValid = valid_;
}
bool Magick::Color::isValid ( void ) const
{
  return _isValid;
}

// Set color via X11 color specification string
const Magick::Color& Magick::Color::operator = ( std::string x11color_ )
{
  isValid ( false );

  MagickLib::XColor target_color;
  if ( Magick::XQueryColorDatabase( x11color_, target_color ) )
    {
      redQuantum( XDownScale(target_color.red) );
      greenQuantum( XDownScale(target_color.green) );
      blueQuantum( XDownScale(target_color.blue) );
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
    return std::string("");

#if defined(QuantumLeap)
  const int width = 4;
#else
  int width = 2;
#endif

  char colorbuf[17];
  ostrstream colorstr( colorbuf, sizeof(colorbuf ));
  colorstr.setf(ios::right | ios::uppercase );
  colorstr.fill('0');
  colorstr << "#"
	   << hex
	   << setw(width) << (unsigned int)_red
	   << setw(width) << (unsigned int)_green
	   << setw(width) << (unsigned int)_blue
	   << ends;

  return std::string(colorbuf);
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

  MagickLib::HSLTransform ( hue_, saturation_, luminosity_,
			    &red, &green, &blue );

  redQuantum   ( red );
  greenQuantum ( green );
  blueQuantum  ( blue );
}

Magick::ColorHSL::ColorHSL ( )
  : Color ()
{
}

Magick::ColorHSL::~ColorHSL ( )
{
  // Nothing to do
}

void Magick::ColorHSL::hue ( double hue_ )
{
  double hue_val, saturation_val, luminosity_val;
  MagickLib::TransformHSL ( redQuantum(), greenQuantum(), blueQuantum(),
			    &hue_val, &saturation_val, &luminosity_val );

  hue_val = hue_;

  Quantum red, green, blue;
  MagickLib::HSLTransform ( hue_val, saturation_val, luminosity_val,
			    &red, &green, &blue
 );

  redQuantum   ( red );
  greenQuantum ( green );
  blueQuantum  ( blue );
}

double Magick::ColorHSL::hue ( void ) const
{
  double hue_val, saturation_val, luminosity_val;
  MagickLib::TransformHSL ( redQuantum(), greenQuantum(), blueQuantum(),
			    &hue_val, &saturation_val, &luminosity_val );
  return hue_val;
}

void Magick::ColorHSL::saturation ( double saturation_ )
{
  double hue_val, saturation_val, luminosity_val;
  MagickLib::TransformHSL ( redQuantum(), greenQuantum(), blueQuantum(),
			    &hue_val, &saturation_val, &luminosity_val );
  
  saturation_val = saturation_;
  
  Quantum red, green, blue;
  MagickLib::HSLTransform ( hue_val, saturation_val, luminosity_val,
			    &red, &green, &blue
 );

  redQuantum   ( red );
  greenQuantum ( green );
  blueQuantum  ( blue );
}

double Magick::ColorHSL::saturation ( void ) const
{
  double hue_val, saturation_val, luminosity_val;
  MagickLib::TransformHSL ( redQuantum(), greenQuantum(), blueQuantum(),
			    &hue_val, &saturation_val, &luminosity_val );
  return saturation_val;
}

void Magick::ColorHSL::luminosity ( double luminosity_ )
{
  double hue_val, saturation_val, luminosity_val;
  MagickLib::TransformHSL ( redQuantum(), greenQuantum(), blueQuantum(),
			    &hue_val, &saturation_val, &luminosity_val );
  
  luminosity_val = luminosity_;
  
  Quantum red, green, blue;
  MagickLib::HSLTransform ( hue_val, saturation_val, luminosity_val,
			    &red, &green, &blue
 );
  
  redQuantum   ( red );
  greenQuantum ( green );
  blueQuantum  ( blue );
}

double Magick::ColorHSL::luminosity ( void ) const
{
  double hue_val, saturation_val, luminosity_val;
  MagickLib::TransformHSL ( redQuantum(), greenQuantum(), blueQuantum(),
			    &hue_val, &saturation_val, &luminosity_val );
  return luminosity_val;
}

//
// ColorGray Implementation
//
Magick::ColorGray::ColorGray ( double shade_ )
  : Color ( ScaleDoubleToQuantum( shade_ ),
	    ScaleDoubleToQuantum( shade_ ),
	    ScaleDoubleToQuantum( shade_ ) )
{
}

Magick::ColorGray::ColorGray ( void )
  : Color ()
{
}

Magick::ColorGray::~ColorGray ()
{
  // Nothing to do
}

void Magick::ColorGray::shade ( double shade_ )
{
  Quantum gray = ScaleDoubleToQuantum( shade_ );
  redQuantum   ( gray );
  greenQuantum ( gray );
  blueQuantum  ( gray );
}

double Magick::ColorGray::shade ( void ) const
{
  return ScaleQuantumToDouble ( greenQuantum() );
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

Magick::ColorMono::ColorMono ( void )
  : Color ()
{
}

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

//
// ColorRGB Implementation
//
Magick::ColorRGB::ColorRGB ( double red_, double green_, double blue_ )
  : Color ( ScaleDoubleToQuantum(red_),
	    ScaleDoubleToQuantum(green_),
	    ScaleDoubleToQuantum(blue_) )
{
}
Magick::ColorRGB::ColorRGB ( const std::string x11color_ )
  : Color( x11color_ )
{
}
Magick::ColorRGB::ColorRGB ( void )
  : Color ()
{
}
Magick::ColorRGB::~ColorRGB ( void )
{
  // Nothing to do
}

void Magick::ColorRGB::red ( double red_ )
{
  redQuantum( ScaleDoubleToQuantum(red_) );
}

double Magick::ColorRGB::red ( void ) const
{
  return ScaleQuantumToDouble( redQuantum() );
}

void Magick::ColorRGB::green ( double green_ )
{
  greenQuantum( ScaleDoubleToQuantum(green_) );
}

double Magick::ColorRGB::green ( void ) const
{
  return ScaleQuantumToDouble( greenQuantum() );
}

void Magick::ColorRGB::blue ( double blue_ )
{
  blueQuantum( ScaleDoubleToQuantum(blue_) );
}

double Magick::ColorRGB::blue ( void ) const
{
  return ScaleQuantumToDouble( blueQuantum() );
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
Magick::ColorYUV::ColorYUV ( double y_, double u_, double v_ )
  : Color ( ScaleDoubleToQuantum(y_ + 1.13980 * v_ ),
	    ScaleDoubleToQuantum(y_ - 0.39380 * u_ - 0.58050 * v_ ),
	    ScaleDoubleToQuantum(y_ + 2.02790 * u_ ) )
{
}

Magick::ColorYUV::ColorYUV ( void )
  : Color ()
{
}

Magick::ColorYUV::~ColorYUV ( void )
{
  // Nothing to do
}

void Magick::ColorYUV::u ( double u_ )
{
  double V = v();
  double Y = y();

  redQuantum   ( ScaleDoubleToQuantum( Y + 1.13980 * V ) );
  greenQuantum ( ScaleDoubleToQuantum( Y - 0.39380 * u_ - 0.58050 * V ) );
  blueQuantum  ( ScaleDoubleToQuantum( Y + 2.02790 * u_ ) );
}

double Magick::ColorYUV::u ( void ) const
{
  return ScaleQuantumToDouble( -0.14740 * redQuantum() - 0.28950 *
			       greenQuantum() + 0.43690 * blueQuantum() );
}

void Magick::ColorYUV::v ( double v_ )
{
  double U = u();
  double Y = y();

  redQuantum   ( ScaleDoubleToQuantum( Y + 1.13980 * v_ ) );
  greenQuantum ( ScaleDoubleToQuantum( Y - 0.39380 * U - 0.58050 * v_ ) );
  blueQuantum  ( ScaleDoubleToQuantum( Y + 2.02790 * U ) );
}

double Magick::ColorYUV::v ( void ) const
{
  return ScaleQuantumToDouble(  0.61500 * redQuantum() - 0.51500 *
				greenQuantum() - 0.10000 * blueQuantum() );
}

void Magick::ColorYUV::y ( double y_ )
{
  double U = u();
  double V = v();

  redQuantum   ( ScaleDoubleToQuantum( y_ + 1.13980 * V ) );
  greenQuantum ( ScaleDoubleToQuantum( y_ - 0.39380 * U - 0.58050 * V ) );
  blueQuantum  ( ScaleDoubleToQuantum( y_ + 2.02790 * U ) );
}

double Magick::ColorYUV::y ( void ) const
{
  return ScaleQuantumToDouble(  0.29900 * redQuantum() + 0.58700 *
				greenQuantum() + 0.11400 * blueQuantum() );
}
