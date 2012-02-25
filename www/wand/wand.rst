=========================
GraphicsMagick Wand C API
=========================

.. _Drawing : drawing_wand.html
.. _Magick : magick_wand.html
.. _Pixel : pixel_wand.html
.. _types: ../api/types.html

The GraphicsMagick Wand C library provides a mid-level abstract C
language programming interface for GraphicsMagick.  It is based on the
Wand API provided in ImageMagick as of August 2003.  After August
2003, ImageMagick changed its license to one unusable by
GraphicsMagick so this version of the Wand library is not in sync with
the current ImageMagick version.

The API is divided into a number of categories. While reading this
documentation, please reference the types_ documentation as required:

  * Drawing_: Wand vector drawing interfaces.
  * Magick_: Wand image processing interfaces
  * Pixel_: Wand pixel access/update interfaces

To compile on Unix, the command looks something like this::

  gcc -o demo demo.c -O `GraphicsMagickWand-config --cppflags --ldflags --libs`

The GraphicsMagickWand-config script reproduces the options which were used to
compile the GraphicsMagick wand library. Using compatible options ensures that
your program will compile and run.

-------------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2009 - 2012

