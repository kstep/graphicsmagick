// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Color Implementation
//
#if !defined (Color_header)
#define Color_header

#include <string>

#include "Magick++/Include.h"

#define ScaleDoubleToQuantum(quantum) (static_cast<unsigned int>(quantum * MaxRGB))
#define ScaleQuantumToDouble(quantum) ((static_cast<double>(quantum)/MaxRGB))

namespace Magick
{

  class Color;

  // Compare two Color objects regardless of LHS/RHS
  int operator == ( const Magick::Color& left_, const Magick::Color& right_ );
  int operator != ( const Magick::Color& left_, const Magick::Color& right_ );
  int operator >  ( const Magick::Color& left_, const Magick::Color& right_ );
  int operator <  ( const Magick::Color& left_, const Magick::Color& right_ );
  int operator >= ( const Magick::Color& left_, const Magick::Color& right_ );
  int operator <= ( const Magick::Color& left_, const Magick::Color& right_ );

  // Base color class stores RGB components scaled to fit Quantum
  class Color
  {
  public:
    Color ( Quantum red_,
	    Quantum green_,
	    Quantum blue_ );
    Color ( Quantum red_,
	    Quantum green_,
	    Quantum blue_,
	    Quantum alpha_ );
    Color ( const std::string x11color_ );
    Color ( const char * x11color_ );
    Color ( void );
    virtual        ~Color ( void );
    Color ( const Color & color_ );

    // Red color (0 to MaxRGB)
    void           redQuantum ( Quantum red_ );
    Quantum        redQuantum ( void ) const;

    // Green color (0 to MaxRGB)
    void           greenQuantum ( Quantum green_ );
    Quantum        greenQuantum ( void ) const;

    // Blue color (0 to MaxRGB)
    void           blueQuantum ( Quantum blue_ );
    Quantum        blueQuantum ( void ) const;

    // Alpha level (0 = transparent, MaxRGB = opaque)
    void           alphaQuantum ( Quantum alpha_ );
    Quantum        alphaQuantum ( void ) const;

    // Scaled (to 1.0) version of alpha for use in sub-classes
    void           alpha ( double alpha_ );
    double         alpha ( void ) const;
        
    // Does object contain valid color?
    void           isValid ( bool valid_ );
    bool           isValid ( void ) const;
    
    // Set color via X11 color specification string
    const Color& operator= ( std::string x11color_ );
    const Color& operator= ( const char * x11color_ );

    // Assignment operator
    Color& operator= ( const Color& color_ );
    
    // Return X11 color specification string
    /* virtual */ operator std::string() const;

    //
    // Public methods beyond this point are for Magick++ use only.
    //

    // Construct color via ImageMagick PixelPacket
    Color ( PixelPacket &color_ );

    // Set color via ImageMagick PixelPacket
    const Color& operator= ( PixelPacket &color_ );

    // Return ImageMagick PixelPacket
    operator PixelPacket() const;

  protected:

    // PixelType specifies the interpretation of PixelPacket members
    // RGBPixel:
    //   Red      = red;
    //   Green    = green;
    //   Blue     = blue;
    // RGBAPixel:
    //   Red      = red;
    //   Green    = green;
    //   Blue     = blue;
    //   Alpha    = index;
    // CYMKPixel:
    //   Cyan     = red
    //   Yellow   = green
    //   Magenta  = blue
    //   Black(K) = index
    enum PixelType
    {
      RGBPixel,
      RGBAPixel,
      CYMKPixel
    };

    // Constructor to construct with PixelPacket*
    // Used to point Color at a pixel in an image
    Color ( PixelPacket* rep_, PixelType pixelType_ );

    // Set pixel
    // Used to point Color at a pixel in an image
    void pixel ( PixelPacket* rep_, PixelType pixelType_ );

    // PixelPacket represents a color pixel:
    //  red     = red   (range 0 to MaxRGB)
    //  green   = green (range 0 to MaxRGB)
    //  blue    = blue  (range 0 to MaxRGB)
    //  opacity = alpha (range Transparent to Opaque)
    //  index   = PseudoColor colormap index
    PixelPacket*     _pixel;

  private:

    // Common initializer for PixelPacket representation
    void initPixel();

    // Set true if we allocated pixel
    bool                        _pixelOwn;

    // Color type supported by _pixel
    PixelType			_pixelType;

  };

  //
  // HSL Colorspace colors
  //
  class ColorHSL : public Color
  {
  public:
    ColorHSL ( double hue_, double saturation_, double luminosity_ );
    ColorHSL ( void );
    ColorHSL ( const Color & color_ );
    /* virtual */  ~ColorHSL ( );
    
    void           hue ( double hue_ );
    double         hue ( void ) const;
    
    void           saturation ( double saturation_ );
    double         saturation ( void ) const;
    
    void           luminosity ( double luminosity_ );
    double         luminosity ( void ) const;

    // Assignment operator from base class
    ColorHSL& operator= ( const Color& color_ );

  protected:
    // Constructor to construct with PixelPacket*
    ColorHSL ( PixelPacket* rep_, PixelType pixelType_ );
  };
  
  //
  // Grayscale RGB color
  //
  // Grayscale is simply RGB with equal parts of red, green, and blue
  // All double arguments have a valid range of 0.0 - 1.0.
  class ColorGray : public Color
  {
  public:
    ColorGray ( double shade_ );
    ColorGray ( void );
    ColorGray ( const Color & color_ );
    /* virtual */ ~ColorGray ();

    void           shade ( double shade_ );
    double         shade ( void ) const;

    // Assignment operator from base class
    ColorGray& operator= ( const Color& color_ );

  protected:
    // Constructor to construct with PixelPacket*
    ColorGray ( PixelPacket* rep_, PixelType pixelType_ );
  };
  
  //
  // Monochrome color
  //
  // Color arguments are constrained to 'false' (black pixel) and 'true'
  // (white pixel)
  class ColorMono : public Color
  {
  public:
    ColorMono ( bool mono_ );
    ColorMono ( void );
    ColorMono ( const Color & color_ );
    /* virtual */ ~ColorMono ();
    
    void           mono ( bool mono_ );
    bool           mono ( void ) const;

    // Assignment operator from base class
    ColorMono& operator= ( const Color& color_ );

  protected:
    // Constructor to construct with PixelPacket*
    ColorMono ( PixelPacket* rep_, PixelType pixelType_ );
  };
  
  //
  // RGB color
  //
  // All color arguments have a valid range of 0.0 - 1.0.
  class ColorRGB : public Color
  {
  public:
    ColorRGB ( double red_, double green_, double blue_ );
    ColorRGB ( void );
    ColorRGB ( const Color & color_ );
    /* virtual */  ~ColorRGB ( void );
    
    void           red ( double red_ );
    double         red ( void ) const;
    
    void           green ( double green_ );
    double         green ( void ) const;
    
    void           blue ( double blue_ );
    double         blue ( void ) const;

    // Assignment operator from base class
    ColorRGB& operator= ( const Color& color_ );

  protected:
    // Constructor to construct with PixelPacket*
    ColorRGB ( PixelPacket* rep_, PixelType pixelType_ );
  };
  
  //
  // YUV Colorspace color
  //
  // Argument ranges:
  //        Y:  0.0 through 1.0
  //        U: -0.5 through 0.5
  //        V: -0.5 through 0.5
  class ColorYUV : public Color
  {
  public:
    ColorYUV ( double y_, double u_, double v_ );
    ColorYUV ( void );
    ColorYUV ( const Color & color_ );
    /* virtual */ ~ColorYUV ( void );
    
    void           u ( double u_ );
    double         u ( void ) const;
    
    void           v ( double v_ );
    double         v ( void ) const;
    
    void           y ( double y_ );
    double         y ( void ) const;

    // Assignment operator from base class
    ColorYUV& operator= ( const Color& color_ );

  protected:
    // Constructor to construct with PixelPacket*
    ColorYUV ( PixelPacket* rep_, PixelType pixelType_ );
  };
} // namespace Magick

//
// Inlines
//

//
// Color
//

// Common initializer for PixelPacket representation
// Initialized to state that ImageMagick considers to be
// an invalid color.
inline void Magick::Color::initPixel()
{
  _pixel->red     = 0;
  _pixel->green   = 0;
  _pixel->blue    = 0;
  _pixel->opacity = TransparentOpacity;
}

inline void Magick::Color::redQuantum ( Quantum red_ )
{
  _pixel->red = static_cast<Quantum>(red_ > MaxRGB ? MaxRGB : red_);
}

inline Magick::Quantum Magick::Color::redQuantum ( void ) const
{
  return _pixel->red;
}

inline void Magick::Color::greenQuantum ( Quantum green_ )
{
  _pixel->green = static_cast<Quantum>(green_ > MaxRGB ? MaxRGB : green_);
}

inline Magick::Quantum  Magick::Color::greenQuantum ( void ) const
{
  return _pixel->green;
}

inline void  Magick::Color::blueQuantum ( Quantum blue_ )
{
  _pixel->blue = static_cast<Quantum>(blue_ > MaxRGB ? MaxRGB : blue_);
}

inline Magick::Quantum Magick::Color::blueQuantum ( void ) const
{
  return _pixel->blue;
}

inline void  Magick::Color::alphaQuantum ( Quantum alpha_ )
{
  _pixel->opacity =
    static_cast<Quantum>(alpha_ > TransparentOpacity ? TransparentOpacity : alpha_);
}

inline Magick::Quantum Magick::Color::alphaQuantum ( void ) const
{
  return _pixel->opacity;
}

inline Magick::Color::Color ( Quantum red_,
			      Quantum green_,
			      Quantum blue_ )
  : _pixel(new PixelPacket),
    _pixelOwn(true),
    _pixelType(RGBPixel)
{
  redQuantum   ( red_   );
  greenQuantum ( green_ );
  blueQuantum  ( blue_  );
  alphaQuantum ( OpaqueOpacity );
}

inline Magick::Color::Color ( Quantum red_,
			      Quantum green_,
			      Quantum blue_,
			      Quantum alpha_ )
  : _pixel(new PixelPacket),
    _pixelOwn(true),
    _pixelType(RGBAPixel)
{
  redQuantum   ( red_   );
  greenQuantum ( green_ );
  blueQuantum  ( blue_  );
  alphaQuantum ( alpha_ );
}

// Construct from color expressed as C++ string
inline Magick::Color::Color ( const std::string x11color_ )
  : _pixel(new PixelPacket),
    _pixelOwn(true),
    _pixelType(RGBPixel)
{
  initPixel();

  // Use operator = implementation
  *this = x11color_;
}

// Construct from color expressed as C string
inline Magick::Color::Color ( const char * x11color_ )
  : _pixel(new PixelPacket),
    _pixelOwn(true),
    _pixelType(RGBPixel)
{
  initPixel();

  // Use operator = implementation
  *this = x11color_;
}

// Default constructor
inline Magick::Color::Color ( void )
  : _pixel(new PixelPacket),
    _pixelOwn(true),
    _pixelType(RGBPixel)
{
  initPixel();
}

// Protected constructor to construct with PixelPacket*
// Used to point Color at a pixel.
inline Magick::Color::Color ( PixelPacket* rep_, PixelType pixelType_  )
  : _pixel(rep_),
    _pixelOwn(false),
    _pixelType(pixelType_)
{
}

// Destructor
inline Magick::Color::~Color( void )
{
  if ( _pixelOwn )
    delete _pixel;
}


// Scaled version of alpha for use in sub-classes
inline void  Magick::Color::alpha ( double alpha_ )
{
  alphaQuantum( ScaleDoubleToQuantum(alpha_) );
}
inline double Magick::Color::alpha ( void ) const
{
  return ScaleQuantumToDouble( alphaQuantum() );
}

// Does object contain valid color?
inline bool Magick::Color::isValid ( void ) const
{
  return( _pixel->opacity != TransparentOpacity ||
          _pixel->red != 0 ||
          _pixel->green != 0 ||
          _pixel->blue != 0 );
}
inline void Magick::Color::isValid ( bool valid_ )
{
  if ( (valid_ && isValid()) || (!valid_ && !isValid()) )
    return;

  if ( !_pixelOwn )
    {
      _pixel = new PixelPacket;
      _pixelOwn = true;
    }

  initPixel();
}


    // Set pixel
    // Used to point Color at a pixel in an image
inline void Magick::Color::pixel ( PixelPacket* rep_, PixelType pixelType_ )
{
  if ( _pixelOwn )
    delete _pixel;
  _pixel = rep_;
  _pixelOwn = false;
  _pixelType = pixelType_;
}

//
// ColorHSL
//
inline Magick::ColorHSL::ColorHSL ( PixelPacket* rep_,
                                    PixelType pixelType_ )
: Color( rep_, pixelType_ )
{
}

//
// ColorGray
//
inline Magick::ColorGray::ColorGray ( PixelPacket* rep_,
                                      PixelType pixelType_ )
: Color( rep_, pixelType_ )
{
}

//
// ColorMono
//
inline Magick::ColorMono::ColorMono ( PixelPacket* rep_,
                                      PixelType pixelType_ )
  : Color( rep_, pixelType_ )
{
}

//
// ColorRGB
//
inline Magick::ColorRGB::ColorRGB ( PixelPacket* rep_,
                                    PixelType pixelType_ )
  : Color( rep_, pixelType_ )
{
}

inline void Magick::ColorRGB::red ( double red_ )
{
  redQuantum( ScaleDoubleToQuantum(red_) );
}

inline double Magick::ColorRGB::red ( void ) const
{
  return ScaleQuantumToDouble( redQuantum() );
}

inline void Magick::ColorRGB::green ( double green_ )
{
  greenQuantum( ScaleDoubleToQuantum(green_) );
}

inline double Magick::ColorRGB::green ( void ) const
{
  return ScaleQuantumToDouble( greenQuantum() );
}

inline void Magick::ColorRGB::blue ( double blue_ )
{
  blueQuantum( ScaleDoubleToQuantum(blue_) );
}

inline double Magick::ColorRGB::blue ( void ) const
{
  return ScaleQuantumToDouble( blueQuantum() );
}

//
// ColorYUV
//

inline Magick::ColorYUV::ColorYUV ( PixelPacket* rep_,
                                    PixelType pixelType_ )
  : Color( rep_, pixelType_ )
{
}

#endif // Color_header
