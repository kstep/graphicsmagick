.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

==================
Magick::TypeMetric
==================

The TypeMetric class provides the means to pass data from the Image
class's TypeMetric method to the user. It provides information
regarding font metrics such as ascent, descent, text width, text
height, and maximum horizontal advance. The units of these font
metrics are in pixels, and that the metrics are dependent on the
current Image font (default Ghostscript's "Helvetica"), pointsize
(default 12 points), and x/y resolution (default 72 DPI) settings.

The pixel units may be converted to points (the standard
resolution-independent measure used by the typesetting industry) via
the following equation::

  size_points = (size_pixels * 72)/resolution

where resolution is in dots-per-inch (DPI). This means that at the
default image resolution, there is one pixel per point.

Note that a font's pointsize is only a first-order approximation of
the font height (ascender + descender) in points. The relationship
between the specified pointsize and the rendered font height is
determined by the font designer.

See `FreeType Glyph Conventions
<http://freetype.sourceforge.net/freetype2/docs/glyphs/index.html>`_
for a detailed description of font metrics related issues.

The following is the definition of the Magick::TypeMetric class::

  class MagickDLLDecl TypeMetric
  {
    friend class Image;
  public:
    
    TypeMetric ( void );
    ~TypeMetric ( void );

    // Ascent, the distance in pixels from the text baseline to the
    // highest/upper grid coordinate used to place an outline point.
    double         ascent ( void ) const;

    // Descent, the distance in pixels from the baseline to the lowest
    // grid coordinate used to place an outline point. Always a
    // negative value.
    double         descent ( void ) const;

    // Text width in pixels.
    double         textWidth ( void ) const;

    // Text height in pixels.
    double         textHeight ( void ) const;

    // Maximum horizontal advance in pixels.
    double         maxHorizontalAdvance ( void ) const;

  };

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| Bob Friesenhahn 1999 - 2015

