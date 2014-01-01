.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=========================
GraphicsMagick Wand C API
=========================

.. _Drawing : drawing_wand.html
.. _Magick : magick_wand.html
.. _Pixel : pixel_wand.html
.. _types: ../api/types.html

The GraphicsMagick Wand C library provides a mid-level abstract C
language programming interface for GraphicsMagick.  It is originally
based on the Wand API provided in ImageMagick as of August 2003.
After August 2003, ImageMagick changed its license to one unusable by
GraphicsMagick so this version of the Wand library is not completely
in sync with the current ImageMagick version.

The API is divided into a number of categories. While reading this
documentation, please reference the types_ documentation as required:

  * Drawing_: Wand vector drawing interfaces.
  * Magick_: Wand image processing interfaces
  * Pixel_: Wand pixel access/update interfaces

The following is a simple example program which (assuming the program
name is `rotate`) is executed similar to `rotate infile outfile`.  It
reads from file `infile`, rotates the image 30 degrees using a black
background, and writes the result to file `outfile`::

  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include <wand/magick_wand.h>

  int main(int argc,char **argv)
  {
    MagickWand *magick_wand;
    MagickPassFail status = MagickPass;
    const char *infile, *outfile;

    if (argc != 3)
      {
        fprintf(stderr,"Usage: %s: infile outfile\n",argv[0]);
        return 1;
      }

    infile=argv[1];
    outfile=argv[2];

    // Initialize GraphicsMagick API
    InitializeMagick(*argv);

    // Allocate Wand handle
    magick_wand=NewMagickWand();

    // Read input image file
    if (status == MagickPass)
      {
        status = MagickReadImage(magick_wand,infile);
      }

    // Rotate image clockwise 30 degrees with black background
    if (status == MagickPass)
      {
        PixelWand *background;
        background=NewPixelWand();
        PixelSetColor(background,"#000000");
        status = MagickRotateImage(magick_wand,background,30);
        DestroyPixelWand(background);
      }

    // Write output file
    if (status == MagickPass)
      {
        status = MagickWriteImage(magick_wand,outfile);
      }

    // Diagnose any error
    if (status != MagickPass)
      {
        char *description;
        ExceptionType severity;

        description=MagickGetException(magick_wand,&severity);
        (void) fprintf(stderr,"%.1024s (severity %d)\n",
                       description,severity);
      }

    // Release Wand handle
    DestroyMagickWand(magick_wand);

    // Destroy GraphicsMagick API
    DestroyMagick();

    return (status == MagickPass ? 0 : 1);
  }


To compile on Unix, the command looks something like this::

  gcc -o demo demo.c -O `GraphicsMagickWand-config --cppflags --ldflags --libs`

The GraphicsMagickWand-config script reproduces the options which were used to
compile the GraphicsMagick wand library. Using compatible options ensures that
your program will compile and run.

-------------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2009 - 2014

