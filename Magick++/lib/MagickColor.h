// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Color Implementation
//
#if !defined (Color_header)
#define Color_header

#include <string>

#include <MagickInclude.h>

#define ScaleDoubleToQuantum(quantum) ((unsigned int)(quantum * MaxRGB))
#define ScaleQuantumToDouble(quantum) (((double)quantum)/MaxRGB)

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
    friend class MagickImage;
  public:
    Color ( Quantum red_, Quantum green_, Quantum blue_ );
    Color ( const std::string x11color_ );
    Color ( const char * x11color_ );
    Color ( void );
    virtual        ~Color ( void );

    // Red color (0 to MaxRGB)
    void           redQuantum ( Quantum red_ );
    Quantum        redQuantum ( void ) const;

    // Green color (0 to MaxRGB)
    void           greenQuantum ( Quantum green_ );
    Quantum        greenQuantum ( void ) const;

    // Blue color (0 to MaxRGB)
    void           blueQuantum ( Quantum blue_ );
    Quantum        blueQuantum ( void ) const;
        
    // Does object contain valid color?
    void           isValid ( bool valid_ );
    bool           isValid ( void ) const;
    
    // Set color via X11 color specification string
    const Color& operator = ( std::string x11color_ );
    const Color& operator = ( const char * x11color_ );
    
    // Return X11 color specification string
    /* virtual */ operator std::string() const;
    
  protected:

  private:
    Quantum        _red;
    Quantum        _green;
    Quantum        _blue;
    bool           _isValid; // Set true if usable object
  };

  //
  // HSL Colorspace colors
  //
  class ColorHSL : public Color
  {
  public:
    ColorHSL ( double hue_, double saturation_, double luminosity_ );
    ColorHSL ( );
    /* virtual */  ~ColorHSL ( );
    
    void           hue ( double hue_ );
    double         hue ( void ) const;
    
    void           saturation ( double saturation_ );
    double         saturation ( void ) const;
    
    void           luminosity ( double luminosity_ );
    double         luminosity ( void ) const;
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
    /* virtual */ ~ColorGray ();
    
    void           shade ( double shade_ );
    double         shade ( void ) const;
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
    /* virtual */ ~ColorMono ();
    
    void           mono ( bool mono_ );
    bool           mono ( void ) const;
    
  };
  
  //
  // RGB color
  //
  // All color arguments have a valid range of 0.0 - 1.0.
  class ColorRGB : public Color
  {
  public:
    ColorRGB ( double red_, double green_, double blue_ );
    ColorRGB ( const std::string x11color_ );
    ColorRGB ( void );
    /* virtual */  ~ColorRGB ( void );
    
    void           red ( double red_ );
    double         red ( void ) const;
    
    void           green ( double green_ );
    double         green ( void ) const;
    
    void           blue ( double blue_ );
    double         blue ( void ) const;
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
    /* virtual */ ~ColorYUV ( void );
    
    void           u ( double u_ );
    double         u ( void ) const;
    
    void           v ( double v_ );
    double         v ( void ) const;
    
    void           y ( double y_ );
    double         y ( void ) const;
  };
} // namespace Magick

#endif // Color_header
