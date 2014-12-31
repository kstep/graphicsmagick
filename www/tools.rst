.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=======================================
GraphicsMagick Tools
=======================================

.. meta::
   :description: GraphicsMagick is a robust collection of tools and libraries to read,
                 write, and manipulate an image in any of the more popular
                 image formats including GIF, JPEG, PNG, PDF, and Photo CD.
                 With GraphicsMagick you can create GIFs dynamically making it
                 suitable for Web applications.  You can also resize, rotate,
                 sharpen, color reduce, or add special effects to an image and
                 save your completed work in the same or differing image format.

   :keywords: GraphicsMagick, Image Magick, Image Magic, PerlMagick, Perl Magick,
              Perl Magic, image processing, software development, image, software,
              Magick++

.. _programming : programming.html
.. _Animate : animate.html
.. _Compare : compare.html
.. _Composite : composite.html
.. _Compare : compare.html
.. _Conjure : conjure.html
.. _Convert : convert.html
.. _Display : display.html
.. _Identify : identify.html
.. _Import : import.html
.. _Mogrify : mogrify.html
.. _Montage : montage.html
.. _PerlMagick : perl.html

.. contents::
  :local:

Animate: animate a sequence of images
=====================================

Animate_ displays a sequence of images or an animation on any display
running an X server.

Compare: compare two images using statistics and/or visual differencing
=======================================================================

Compare_ compares two images using either a specified standard
statistical metric (MAE, MSE, PAE, PSNR, RMSE), or a specified visual
differencing method (assign, threshold, tint, xor). The statistical
comparison produces a textual display of metric values while the visual
differencing method writes a difference image with the differences
annotated using the specified algorithm.  For example::

  % gm convert input.jpg -blur 0x1.5 output.jpg
  % gm compare -metric MSE input.jpg output.jpg
  Image Difference (MeanSquaredError):
             Normalized    Absolute
            ============  ==========
       Red: 0.0014374614       94.2
     Green: 0.0014396270       94.3
      Blue: 0.0014464548       94.8
     Total: 0.0014411811       94.4

Composite: composite images together
====================================

Composite_ blends and merges images to create new images.

Conjure: execute a Magick Scripting Language (MSL) XML script
=============================================================

Conjure_ interprets and executes scripts in the Magick Scripting Language
(MSL). The interpreter is called conjure and here is an example script::

  <?xml version="1.0" encoding="UTF-8"?>
  <image size="400x400" >
    <read filename="image.gif" />
    <get width="base-width" height="base-height" />
    <resize geometry="%[dimensions]" />
    <get width="width" height="height" />
    <print output=
      "Image sized from %[base-width]x%[base-height]
       to %[width]x%[height].\n" />
    <write filename="image.png" />
  </image>

which is invoked like

::

    conjure -dimensions 400x400 incantation.msl

All operations closely follow the key/value pairs defined in the
PerlMagick_ documentation, unless otherwise noted.

Convert: convert an image or sequence of images
===============================================

Convert_ converts an input file using one image format to an output file
with using any of the supported writeable image formats. A large number
of image processing operations may be performed on the image before it is
written. By default, the input image format is determined by its magic
number. To specify a particular image format, precede the filename with
an image format name and a colon (i.e. ps:image) or specify the image
type as the filename suffix (i.e. image.ps). Specify file as - for
standard input or output. If file has the extension .Z, the file is
decoded with uncompress.  For example::

  % gm convert input.jpg -rotate 90 rotated.tiff

Display: display an image on a workstation running X
====================================================

Display_ is a machine architecture independent image processing and
display program. It can display an image on any workstation display
running an X server. The image can be displayed as background image of
any window.

Identify: describe an image or image sequence
=============================================

Identify_ describes the format and characteristics of one or more image
files. It will also report if an image is incomplete or corrupt. The
information displayed includes the scene number, the file name, the width
and height of the image, whether the image is colormapped or not, the
number of colors in the image, the number of bytes in the image, the
format of the image (JPEG, PNM, etc.), and finally the number of seconds
it took to read and process the image.  For example::

  % gm identify tiger-1200-rgb16.tiff
  tiger-1200-rgb16.tiff TIFF 9083x9450+0+0 DirectClass 16-bit 491.2M 0.000u 0:01

Import: capture an application or X server screen
=================================================

Import_ reads an image from any visible window on an X server and outputs
it as an image file. You can capture a single window, the entire screen,
or any rectangular portion of the screen.
     
The target window can be specified by id, name, or may be selected by
clicking the mouse in the desired window. If you press a button and then
drag, a rectangle will form which expands and contracts as the mouse
moves. To save the portion of the screen defined by the rectangle, just
release the button. The keyboard bell is rung once at the beginning of
the screen capture and twice when it completes.  For example::

  % gm import capture.tiff

Mogrify: transform an image or sequence of images
=================================================

Mogrify_ transforms an image or a sequence of images "in place". These
transforms include image scaling, image rotation, color reduction, and
others. The transmogrified image overwrites the original image. Mogrify_
is very similar to Convert_ except that it can operate on many images at
once, and overwrites the input files by default. However, Mogrify_ may
also be used to convert file formats, and send modified files to another
directory. For example, the following reads several JPEG files and writes
a rotated version of them in TIFF format::

  % gm mogrify -rotate 90 -format tiff image1.jpg image2.jpg image3.jpg

Montage: create a composite image (in a grid) from separate images
==================================================================

Montage_ creates a composite by combining several separate images. The
images are tiled on a composite image with the name of the image and its
properties optionally appearing just below the individual tile.

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2015


