.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

===================
Magick::Color Class
===================

Color is the base color class in Magick++. It is a simple container
class for the pixel red, green, blue, and alpha values scaled to fit
GraphicsMagick's Quantum (number of bits in a color component value)
size. Normally users will instantiate a class derived from Color which
supports the color model that fits the needs of the application. The
Color class may be constructed directly from an X11-style color
string. As a perhaps odd design decision, the value transparent black
is considered to represent an unset value (invalid color) in many
cases. This choice was made since it avoided using more memory. The
default Color constructor constructs an invalid color
(i.e. transparent black) and may be used as a parameter in order to
remove a color setting.

Quantum
-------

The base type used to represent color samples in GraphicsMagick is the
Quantum type. Pixels are represented by a structure of Quantum
values. For example, an RGB pixel contains red, green, and blue
quantums, while an RGBA pixel contains red, green, blue, and opacity
quantums. The maximum value that a Quantum can attain is specified by
a constant value represented by the MaxRGB define, which is itself
determined by the number of bits in a Quantum. The QuantumDepth build
option determines the number of bits in a Quantum.

PixelPacket
-----------

PixelPacket is the internal representation of a pixel in
GraphicsMagick. GraphicsMagick may be compiled to support 32, 64, or
128 bit pixels of type PixelPacket. This is controlled by the value of
the QuantumDepth define. The default is 32 bit pixels
(QuantumDepth=8), which provides the best performance and the least
resource consumption. If additional color precision or range is
desired, then GraphicsMagick may be compiled with QuantumDepth=16 or
QuantumDepth=32. The following table shows the relationship between
QuantumDepth, the type of Quantum, and the overall PixelPacket size:

.. table:: Effect Of QuantumDepth Values

   ============  ===============  ================
   QuantumDepth  Quantum Typedef  PixelPacket Size
   ============  ===============  ================
   8             unsigned char    32 bits
   16            unsigned short   64 bits
   32            unsigned int     128 bits
   ============  ===============  ================

Color Class
-----------

The Color base class is not intended to be used directly. Normally a
user will construct a derived class or inherit from this class. Color
arguments are must be scaled to fit the Quantum size according to the
range of MaxRGB. The Color class contains a pointer to a PixelPacket,
which may be allocated by the Color class, or may refer to an existing
pixel in an image.

An alternate way to construct the class is via an X11-compatible color
specification string (e.g. Color(“red”) or Color (“#FF0000”)). Since
the class may be constructed from a string, convenient strings may be
passed in place of an explicit Color object in methods which accept a
reference to Color. Color may also be converted to a std::string for
convenience in user interfaces, and for saving settings to a text
file.

The following is the definition of the Color class::

  // Compare two Color objects regardless of LHS/RHS
  int MagickDLLDecl operator == ( const Magick::Color& left_, const Magick::Color& right_ );
  int MagickDLLDecl operator != ( const Magick::Color& left_, const Magick::Color& right_ );
  int MagickDLLDecl operator >  ( const Magick::Color& left_, const Magick::Color& right_ );
  int MagickDLLDecl operator <  ( const Magick::Color& left_, const Magick::Color& right_ );
  int MagickDLLDecl operator >= ( const Magick::Color& left_, const Magick::Color& right_ );
  int MagickDLLDecl operator <= ( const Magick::Color& left_, const Magick::Color& right_ );

  // Base color class stores RGB components scaled to fit Quantum
  class MagickDLLDecl Color
  {
  public:
    Color ( Quantum red_,
	    Quantum green_,
	    Quantum blue_ );
    Color ( Quantum red_,
	    Quantum green_,
	    Quantum blue_,
	    Quantum alpha_ );
    Color ( const std::string &x11color_ );
    Color ( const char * x11color_ );
    Color ( void );
    virtual        ~Color ( void );
    Color ( const Color & color_ );

    // Red color (range 0 to MaxRGB)
    void           redQuantum ( Quantum red_ );
    Quantum        redQuantum ( void ) const;

    // Green color (range 0 to MaxRGB)
    void           greenQuantum ( Quantum green_ );
    Quantum        greenQuantum ( void ) const;

    // Blue color (range 0 to MaxRGB)
    void           blueQuantum ( Quantum blue_ );
    Quantum        blueQuantum ( void ) const;

    // Alpha level (range OpaqueOpacity=0 to TransparentOpacity=MaxRGB)
    void           alphaQuantum ( Quantum alpha_ );
    Quantum        alphaQuantum ( void ) const;

    // Scaled (to 1.0) version of alpha for use in sub-classes
    // (range opaque=0 to transparent=1.0)
    void           alpha ( double alpha_ );
    double         alpha ( void ) const;
        
    // Does object contain valid color?
    void           isValid ( bool valid_ );
    bool           isValid ( void ) const;
    
    // Set color via X11 color specification string
    const Color& operator= ( const std::string &x11color_ );
    const Color& operator= ( const char * x11color_ );

    // Assignment operator
    Color& operator= ( const Color& color_ );

    // Return X11 color specification string
    /* virtual */ operator std::string() const;

    // Return ImageMagick PixelPacket
    operator PixelPacket() const;

    // Construct color via ImageMagick PixelPacket
    Color ( const PixelPacket &color_ );

    // Set color via ImageMagick PixelPacket
    const Color& operator= ( const PixelPacket &color_ );

  };


Color Derived Classes
---------------------

Available derived color specification classes are shown in the following table:

.. table:: Color Derived Classes

   ==========  ================================================================
   Class       Description
   ==========  ================================================================
   ColorRGB_   Representation of RGB color with red, green, and blue specified
               as ratios (0 to 1)
   ColorGray_  Representation of grayscale RGB color (equal parts red, green,
               and blue) specified as a ratio (0 to 1)
   ColorMono_  Representation of a black/white color (true/false)
   ColorHSL_   Representation of a color in Hue/Saturation/Luminosity (HSL)
               colorspace
   ColorYUV_   Representation of a color in the YUV colorspace
   ==========  ================================================================

ColorRGB
---------

Representation of an RGB color in floating point. All color arguments
have a valid range of 0.0 - 1.0. Note that floating point alpha may be
set via the alpha() method in the Color base class::

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


ColorGray
---------

Representation of a floating point grayscale color (in RGB
colorspace). Grayscale is simply RGB with equal parts of red, green,
and blue. All double arguments have a valid range of 0.0 - 1.0::

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

  };


ColorMono
---------

Representation of a black/white pixel (in RGB colorspace). Color
arguments are constrained to 'false' (black pixel) and 'true' (white
pixel)::

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

  };


ColorHSL
--------

Representation of a color in Hue/Saturation/Luminosity (HSL) colorspace::

  class MagickDLLDecl ColorHSL : public Color
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

  };


ColorYUV
--------

Representation of a color in YUV colorspace (used to encode color for
television transmission).  Argument ranges are

  ==  =================
  Y   0.0 through 1.0
  U   -0.5 through 0.5
  V   -0.5 through 0.5 
  ==  =================

::

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

  };

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| Bob Friesenhahn 1999 - 2015

