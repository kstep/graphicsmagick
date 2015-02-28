.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

================
Magick::Geometry
================

Geometry provides a convenient means to specify a geometry
argument. The object may be initialized from a C string or C++ string
containing a geometry specification. It may also be initialized by
more efficient parameterized constructors.

X11 Geometry Specifications
---------------------------

X11 geometry specifications are in the form
"<width>x<height>{+-}<xoffset>{+-}<yoffset>" (where width, height,
xoffset, and yoffset are numbers) for specifying the size and
placement location for an object.

The width and height parts of the geometry specification are measured
in pixels. The xoffset and yoffset parts are also measured in pixels
and are used to specify the distance of the placement coordinate from
the left and top and edges of the image, respectively. Both types of
offsets are measured from the indicated edge of the object to the
corresponding edge of the image. The X offset may be specified in the
following ways:

=========  =========================================================
+xoffset   The left edge of the object is to be placed xoffset
           pixels in from the left edge of the image.
-xoffset   The left edge of the object is to be placed outside the
           image, xoffset pixels out from the left edge of the image.
=========  =========================================================

The Y offset has similar meanings:

=========  =========================================================
+yoffset   The top edge of the object is to be yoffset pixels below
           the top edge of the image.
-yoffset   The top edge of the object is to be yoffset pixels above
           the top edge of the image.
=========  =========================================================

Offsets must be given as pairs; in other words, in order to specify
either xoffset or yoffset both must be present.

GraphicsMagick Extensions To X11 Geometry Specifications
--------------------------------------------------------

GraphicsMagick has added a number of qualifiers to the standard
geometry string for use when resizing images. The form of an extended
geometry string is
"<width>x<height>{+-}<xoffset>{+-}<yoffset>{%}{@}{!}{^}{<}{>}". Extended
geometry strings should only be used when resizing an image. Using an
extended geometry string for other applications may cause the API call
to fail. The available qualifiers are shown in the following table:

.. table:: GraphicsMagick Geometry Qualifiers

   =========  ======================================================
   Qualifier  Description
   =========  ======================================================
   %          Interpret width and height as a percentage of the current size.
   !          Resize to width and height exactly, loosing original aspect ratio.
   <          Resize only if the image is smaller than the geometry specification.
   >          Resize only if the image is greater than the geometry specification.
   @          Resize such that width and height are a maximum area in total pixels.
   ^          Dimensions are treated as minimum rather than maximum values.
   =========  ======================================================

Postscript Page Size Extension To Geometry Specifications
---------------------------------------------------------

Any geometry string specification supplied to the Geometry constructor
is considered to be a Postscript page size nickname if the first
character is not numeric. The page size nickname is not case
sensitive. The Geometry constructor converts these page size
specifications into the equivalent numeric geometry string
specification (preserving any offset component) prior to conversion to
the internal object format. Postscript page size specifications are
short-hand for the pixel geometry required to fill a page of that
size. Since the 11x17 inch page size used in the US starts with a
digit, it is not supported as a Postscript page size
nickname. Instead, substitute the geometry specification "792x1224>"
when 11x17 output is desired.

.. table:: Postscript Page Size Nicknames

   ====================  ==========================================
   Postscript Nickname   Equivalent Extended Geometry Specification
   ====================  ==========================================
   A0                    2384x3370>
   A1                    1684x2384>
   A10                   73x105>
   A2                    1191x1684>
   A3                    842x1191>
   A4                    595x842>
   A4SMALL               595x842>
   A5                    420x595>
   A6                    297x420>
   A7                    210x297>
   A8                    148x210>
   A9                    105x148>
   ARCHA                 648x864>
   ARCHB                 864x1296>
   ARCHC                 1296x1728>
   ARCHD                 1728x2592>
   ARCHE                 2592x3456>
   B0                    2920x4127>
   B1                    2064x2920>
   B10                   91x127>
   B2                    1460x2064>
   B3                    1032x1460>
   B4                    729x1032>
   B5                    516x729>
   B6                    363x516>
   B7                    258x363>
   B8                    181x258>
   B9                    127x181>
   C0                    2599x3676>
   C1                    1837x2599>
   C2                    1298x1837>
   C3                    918x1296>
   C4                    649x918>
   C5                    459x649>
   C6                    323x459>
   C7                    230x323>
   EXECUTIVE             540x720>
   FLSA                  612x936>
   FLSE                  612x936>
   FOLIO                 612x936>
   HALFLETTER            396x612>
   ISOB0                 2835x4008>
   ISOB1                 2004x2835>
   ISOB10                88x125>
   ISOB2                 1417x2004>
   ISOB3                 1001x1417>
   ISOB4                 709x1001>
   ISOB5                 499x709>
   ISOB6                 354x499>
   ISOB7                 249x354>
   ISOB8                 176x249>
   ISOB9                 125x176>
   LEDGER                1224x792>
   LEGAL                 612x1008>
   LETTER                612x792>
   LETTERSMALL           612x792>
   QUARTO                610x780>
   STATEMENT             396x612>
   TABLOID               792x1224>
   ====================  ==========================================

The following is the definition of the Magick::Geometry class::

  class Geometry
  {
  public:
    
    Geometry ( unsigned int width_,
	       unsigned int height_,
	       unsigned int xOff_ = 0,
	       unsigned int yOff_ = 0,
	       bool xNegative_ = false,
	       bool yNegative_ = false );
    Geometry ( const std::string &geometry_ );
    Geometry ( const char * geometry_ );
    Geometry ( const Geometry &geometry_ );
    Geometry ( );
    ~Geometry ( void );
    
    // Width
    void          width ( unsigned int width_ );
    unsigned int  width ( void ) const;
    
    // Height
    void          height ( unsigned int height_ );
    unsigned int  height ( void ) const;
    
    // X offset from origin
    void          xOff ( unsigned int xOff_ );
    unsigned int  xOff ( void ) const;
    
    // Y offset from origin
    void          yOff ( unsigned int yOff_ );
    unsigned int  yOff ( void ) const;
    
    // Sign of X offset negative? (X origin at right)
    void          xNegative ( bool xNegative_ );
    bool          xNegative ( void ) const;
    
    // Sign of Y offset negative? (Y origin at bottom)
    void          yNegative ( bool yNegative_ );
    bool          yNegative ( void ) const;
    
    // Width and height are expressed as percentages
    void          percent ( bool percent_ );
    bool          percent ( void ) const;

    // Resize without preserving aspect ratio (!)
    void          aspect ( bool aspect_ );
    bool          aspect ( void ) const;
    
    // Resize if image is greater than size (>)
    void          greater ( bool greater_ );
    bool          greater ( void ) const;
    
    // Resize if image is less than size (<)
    void          less ( bool less_ );
    bool          less ( void ) const;

    // Resize image to fit total pixel area specified by dimensions (@).
    void          limitPixels ( bool limitPixels_ );
    bool          limitPixels ( void ) const;

    // Dimensions are treated as minimum rather than maximum values (^)
    void          fillArea ( bool fillArea_ );
    bool          fillArea ( void ) const;
    
    // Does object contain valid geometry?
    void          isValid ( bool isValid_ );
    bool          isValid ( void ) const;
    
    // Set via geometry string
    const Geometry& operator = ( const std::string &geometry_ );
    const Geometry& operator = ( const char * geometry_ );

    // Assignment operator
    Geometry& operator= ( const Geometry& Geometry_ );
    
    // Return geometry string
    operator std::string() const;
    
  };

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| Bob Friesenhahn 1999 - 2015

