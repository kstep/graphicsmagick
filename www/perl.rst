.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=====================================
GraphicsMagick Perl API -- PerlMagick
=====================================

.. contents::
  :local:

Introduction
============

PerlMagick is an objected-oriented Perl interface to GraphicsMagick. Use
the module to read, manipulate, or write an image or image sequence from
within a Perl script. This makes it very suitable for Web CGI scripts.
You must have GraphicsMagick 1.0.0 or above and Perl version 5.005_02 or
greater installed on your system for either of these utilities to work.
There are a number of useful scripts available to show you the value of
PerlMagick. The PerlMagick demo directory provides a number of sample
demos.

Installation
============

UNIX
----

PerlMagick is installed by default by installing GraphicsMagick.
Installing PerlMagick as a subordinate package of GraphicsMagick is the
best way to avoid problems.

For Unix, you typically need to be root to install the software. There
are ways around this. Consult the Perl manual pages for more information.

Windows XP - Windows 8
----------------------

Please note that a nice GUI installer is available for GraphicsMagick.
PerlMagick is included in this installer. If you are using the installer,
then there is no need to compile PerlMagick.

After GraphicsMagick has been compiled from the GraphicsMagick Windows
source distribution using Microsoft Visual C++, PerlMagick may be
manually built and installed by opening a CLI window and performing the
following steps::

  cd PerlMagick
  copy Makefile.nt Makefile.PL
  perl Makefile.PL
  nmake
  nmake install

See the PerlMagick Windows HowTo page for further installation
instructions.

Running the Regression Tests
----------------------------

To verify a correct installation, type::

  make test

Use nmake test under Windows. There are a few demonstration scripts
available to exercise many of the functions PerlMagick can perform. Type

::

  cd demo
  make

You are now ready to utilize the PerlMagick methods from within your Perl
scripts.

Overview
========

Any script that wants to use PerlMagick methods must first define the
methods within its namespace and instantiate an image object. Do this
with

::

  use Graphics::Magick;
  $image=Graphics::Magick->new;

Note that this differs from the ImageMagick version of PerlMagick which
uses the namespace Image::Magick. Any PerlMagick code written for the
ImageMagick version of PerlMagick requires a global substition of
Image::Magick to Graphics::Magick in order to work with the
GraphicsMagick version.

The new() method takes the same parameters as SetAttribute . For example::

  $image=Graphics::Magick->new(size=>'384x256');

Next you will want to read an image or image sequence, manipulate it, and
then display or write it. The input and output methods for PerlMagick are
defined in Read or Write an Image. See Set an Image Attribute for methods
that affect the way an image is read or written. Refer to Manipulate an
Image for a list of methods to transform an image. Get an Image Attribute
describes how to retrieve an attribute for an image. Refer to Create an
Image Montage for details about tiling your images as thumbnails on a
background. Finally, some methods do not neatly fit into any of the
categories just mentioned. Review Miscellaneous Methods for a list of
these methods.

Once you are finished with a PerlMagick object you should consider
destroying it. Each image in an image sequence is stored in either
virtual memory or as a file in the system's temporary file directory.
This can potentially add up to megabytes of memory or disk. Upon
destroying a PerlMagick object, the memory is returned for use by other
Perl methods. The recommended way to destroy an object is with undef

::

  undef $image;

To delete all the images but retain the Graphics::Magick object use

::

  @$image = ();

and finally, to delete a single image from a multi-image sequence, use

::

   undef $image->[x];

The next section illustrates how to use various PerlMagick methods to
manipulate an image sequence.

Some of the PerlMagick methods require external programs such as
Ghostscript. This may require an explicit path in your PATH environment
variable to work properly. For example,

::

 $ENV{PATH}='/bin:/usr/bin:/usr/local/bin';


Example Script
==============

Here is an example script to get you started::

  #!/usr/local/bin/perl
  use Graphics::Magick;
  my($image, $status);
  $image = Graphics::Magick->new;
  $status = $image->Read('girl.png', 'logo.png', 'rose.png');
  warn "$status" if "$status";
  $status = $image->Crop(geometry=>'100x100+100+100');
  warn "$status" if "$status";
  $status = $image->Write('x.gif');
  warn "$status" if "$status";

The script reads three images, crops them, and writes a single image as a
GIF animation sequence. In many cases you may want to access individual
images of a sequence. The next example illustrates how this is done::

  #!/usr/local/bin/perl
  use Graphics::Magick;
  my($image, $p, $q);
  $image = new Graphics::Magick;
  $image->Read('x1.png');
  $image->Read('j*.jpg');
  $image->Read('k.miff[1, 5, 3]');
  $image->Contrast();
  for ($x = 0; $image->[x]; $x++)
  {
    $image->[x]->Frame('100x200') if $image->[x]->Get('magick') eq 'GIF';
    undef $image->[x] if $image->[x]->Get('columns') < 100;
  }
  $p = $image->[1];
  $p->Draw(stroke=>'red', primitive=>'rectangle', points=>'20,20 100,100');
  $q = $p->Montage();
  undef $image;
  $q->Write('x.miff');

Suppose you want to start out with a 100 by 100 pixel white canvas with a
red pixel in the center. Try

::

  $image = Graphics::Magick->new;
  $image->Set(size=>'100x100');
  $image->ReadImage('xc:white');
  $image->Set('pixel[49,49]'=>'red');

Or suppose you want to convert your color image to grayscale::

  $image->Quantize(colorspace=>'gray');

Here we annotate an image with a Taipai TrueType font::

  $text = 'Works like magick!';
  $image->Annotate(font=>'kai.ttf', pointsize=>40, fill=>'green', text=>$text);

Other clever things you can do with a PerlMagick objects include

::

  $i = $#$p"+1"; # return the number of images associated with object p
  push(@$q, @$p); # push the images from object p onto object q
  @$p = (); # delete the images but not the object p
  $p->Convolve([1, 2, 1, 2, 4, 2, 1, 2, 1]); # 3x3 Gaussian kernel

Read or Write an Image
======================

Use the methods listed below to either read, write, or display an image
or image sequence.

.. table:: Read or Write Methods

   +-------+-------------+-------------------+-------------------------------+
   |Method | Parameters  |   Return Value    |          Description          |
   +-------+-------------+-------------------+-------------------------------+
   |Read   |one or more  |the number of      |read an image or image sequence|
   |       |filenames    |images read        |                               |
   +-------+-------------+-------------------+-------------------------------+
   |Write  |filename     |the number of      |write an image or image        |
   |       |             |images written     |sequence                       |
   +-------+-------------+-------------------+-------------------------------+
   |Display|server name  |the number of      |display the image or image     |
   |       |             |images displayed   |sequence to an X server        |
   +-------+-------------+-------------------+-------------------------------+
   |Animate|server name  |the number of      |animate image sequence to an X |
   |       |             |images animated    |server                         |
   +-------+-------------+-------------------+-------------------------------+

For convenience, methods Write(), Display(), and Animate() can take any
parameter that SetAttribute knows about. For example,

::

  $image->Write(filename=>'image.png', compression=>'None');

Use - as the filename to method Read() to read from standard in or to
method Write() to write to standard out::

  binmode STDOUT;
  $image->Write('png:-');

To read an image from a disk file, use::
  $image = Graphics::Magick->new;
  $filename = 'test.gif';
  $status = $image->Read ($filename);

and to write the image back to the disk file, use::

  $status = $image->Write($filename);

To read an image in the GIF format from a PERL filehandle, use::

  $image = Graphics::Magick->new;
  open(IMAGE, 'image.gif');
  $status = $image->Read(file=>\*IMAGE);
  close(IMAGE);

To write an image in the PNG format to a PERL filehandle, use::

  $filename = "image.png";
  open(IMAGE, ">$filename");
  $status = $image->Write(file=>\*IMAGE, filename=>$filename);
  close(IMAGE);

If %0Nd appears in the filename, it is interpreted as a printf format
specification and the specification is replaced with the specified
decimal encoding of the scene number. For example,

::

  image%03d.miff

converts files image000.miff, image001.miff, etc.

You can optionally add Image to any method name. For example, ReadImage()
is an alias for method Read().


Manipulate an Image
===================

Once you create an image with, for example, method ReadImage() you may want
to operate on it. Below is a list of all the image manipulations methods
available to you with PerlMagick. There are examples of select PerlMagick
methods. Here is an example call to an image manipulation method::

  $image->Crop(geometry=>'100x100"+1"0+20');
  $image->[x]->Frame("100x200");

Image method parameters are often redundant. For example, a 'geometry'
string parameter (e.g. 800x600+10+20) is equivalent to the explicit use of
width, height, x, and y, parameters.

The following image manipulation methods are available:

.. table:: Image Manipulation Methods

   +-----------------+---------------------------------+---------------------+
   |     Method      |           Parameters            |     Description     |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |Local adaptive       |
   |                 |                                 |thresholding. Width  |
   |                 |                                 |and height specify   |
   |                 |geometry=>geometry, width=>      |the size of the local|
   |AdaptiveThreshold|integer, height=> integer, offset|region while offset  |
   |                 |=>integer                        |specifies the amount |
   |                 |                                 |to subtract from the |
   |                 |                                 |average of the       |
   |                 |                                 |region.              |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |Add noise to an image|
   |                 |                                 |across the red,      |
   |                 |noise=>{Uniform, Gaussian,       |green, and blue,     |
   |AddNoise         |Multiplicative, Impulse,         |channels. Set the    |
   |                 |Laplacian, Poisson, Random}      |image colorspace to  |
   |                 |                                 |GRAY to obtain       |
   |                 |                                 |intensity noise.     |
   +-----------------+---------------------------------+---------------------+
   |                 |affine=>array of float values,   |                     |
   |AffineTransform  |translate=>float, float, scale=> |Affine transform     |
   |                 |float, float, rotate=>float,     |image                |
   |                 |skewX=>float, skewY=>float       |                     |
   +-----------------+---------------------------------+---------------------+
   |                 |text=>string, font=>string,      |                     |
   |                 |family=>string, style=>{Normal,  |                     |
   |                 |Italic, Oblique, Any}, stretch=> |                     |
   |                 |{Normal, UltraCondensed,         |                     |
   |                 |ExtraCondensed, Condensed,       |                     |
   |                 |SemiCondensed, SemiExpanded,     |                     |
   |                 |Expanded, ExtraExpanded,         |                     |
   |                 |UltraExpanded}, weight=>integer, |                     |
   |                 |pointsize=>integer, density=>    |annotate an image    |
   |                 |geometry, stroke=> color name,   |with text. See       |
   |                 |strokewidth=>integer, fill=>color|QueryFontMetrics to  |
   |Annotate         |name, undercolor=>color name,    |get font metrics     |
   |                 |geometry=>geometry, gravity=>    |without rendering any|
   |                 |{NorthWest, North, NorthEast,    |text.                |
   |                 |West, Center, East, SouthWest,   |                     |
   |                 |South, SouthEast}, antialias=>   |                     |
   |                 |{true, false}, x=>integer, y=>   |                     |
   |                 |integer, affine=>array of float  |                     |
   |                 |values, translate=>float, float, |                     |
   |                 |scale=>float, float, rotate=>    |                     |
   |                 |float. skewX=>float, skewY=>     |                     |
   |                 |float, align=>{Left, Center,     |                     |
   |                 |Right}, encoding=>{UTF-8}        |                     |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |blur the image with a|
   |                 |geometry=>geometry, radius=>     |Gaussian operator of |
   |Blur             |double, sigma=> double           |the given radius and |
   |                 |                                 |standard deviation   |
   |                 |                                 |(sigma).             |
   +-----------------+---------------------------------+---------------------+
   |                 |geometry=>geometry, width=>      |surround the image   |
   |Border           |integer, height=> integer, fill=>|with a border of     |
   |                 |color name                       |color                |
   +-----------------+---------------------------------+---------------------+
   |                 |channel=>{Red, Cyan, Green,      |extract a channel    |
   |Channel          |Magenta, Blue, Yellow, Opacity,  |from the image       |
   |                 |Black, Matte, All, Gray}         |                     |
   +-----------------+---------------------------------+---------------------+
   |Charcoal         |order=>integer                   |simulate a charcoal  |
   |                 |                                 |drawing              |
   +-----------------+---------------------------------+---------------------+
   |                 |geometry=>geometry, width=>      |                     |
   |Chop             |integer, height=> integer, x=>   |chop an image        |
   |                 |integer, y=>integer              |                     |
   +-----------------+---------------------------------+---------------------+
   |Coalesce         |                                 |merge a sequence of  |
   |                 |                                 |images               |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |apply any clipping   |
   |Clip             |                                 |path information as  |
   |                 |                                 |an image clip mask.  |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |changes the color    |
   |                 |                                 |value of any pixel   |
   |                 |                                 |that matches the     |
   |                 |                                 |color of the target  |
   |                 |geometry=>geometry, x=>integer, y|pixel and is a       |
   |ColorFloodfill   |=>integer , fill=>color name,    |neighbor. If you     |
   |                 |bordercolor=> color name         |specify a border     |
   |                 |                                 |color, the color     |
   |                 |                                 |value is changed for |
   |                 |                                 |any neighbor pixel   |
   |                 |                                 |that is not that     |
   |                 |                                 |color.               |
   +-----------------+---------------------------------+---------------------+
   |Colorize         |fill=>color name, opacity=>string|colorize the image   |
   |                 |                                 |with the fill color  |
   +-----------------+---------------------------------+---------------------+
   |Comment          |string                           |add a comment to your|
   |                 |                                 |image                |
   +-----------------+---------------------------------+---------------------+
   |Compare          |image=>image-handle              |compare image to a   |
   |                 |                                 |reference image      |
   +-----------------+---------------------------------+---------------------+
   |                 |image=>image-handle, compose=>{  |                     |
   |                 |Over, In, Out, Atop, Xor, Plus,  |                     |
   |                 |Minus, Add, Subtract,            |                     |
   |                 |Difference, Multiply, Bumpmap,   |                     |
   |                 |Copy, CopyRed, CopyGreen,        |                     |
   |                 |CopyBlue, CopyOpacity,           |                     |
   |                 |Clear, Dissolve, Displace,       |                     |
   |                 |Modulate, Threshold, No, Darken, |                     |
   |                 |Lighten, Hue, Saturate,          |                     |
   |Composite        |Colorize, Luminize, Screen,      |composite one image  |
   |                 |Overlay, CopyCyan, CopyMagenta,  |onto another         |
   |                 |CopyYellow, CopyBlack, Divide,   |                     |
   |                 |HardLight},                      |                     |
   |                 |mask=>                           |                     |
   |                 |image-handle, geometry=>geometry,|                     |
   |                 |x=>integer, y=>integer, gravity=>|                     |
   |                 |{NorthWest, North, NorthEast,    |                     |
   |                 |West, Center, East, SouthWest,   |                     |
   |                 |South, SouthEast}, opacity=>     |                     |
   |                 |integer, tile=>{True, False},    |                     |
   |                 |rotate=>double, color=>color name|                     |
   +-----------------+---------------------------------+---------------------+
   |Contrast         |sharpen=>{True, False}           |enhance or reduce the|
   |                 |                                 |image contrast       |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |apply a convolution  |
   |                 |                                 |kernel to the image. |
   |                 |coefficients=>array of float     |Given a kernel order |
   |Convolve         |values                           |, you would supply   |
   |                 |                                 |order*order float    |
   |                 |                                 |values (e.g. 3x3     |
   |                 |                                 |implies 9 values).   |
   +-----------------+---------------------------------+---------------------+
   |                 |geometry=>geometry, width=>      |                     |
   |Crop             |integer, height=> integer, x=>   |crop an image        |
   |                 |integer, y=>integer              |                     |
   +-----------------+---------------------------------+---------------------+
   |CycleColormap    |amount=>integer                  |displace image       |
   |                 |                                 |colormap by amount   |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |break down an image  |
   |Deconstruct      |                                 |sequence into        |
   |                 |                                 |constituent parts    |
   +-----------------+---------------------------------+---------------------+
   |Despeckle        |                                 |reduce the speckles  |
   |                 |                                 |within an image      |
   +-----------------+---------------------------------+---------------------+
   |                 |primitive=>{point, line,         |                     |
   |                 |rectangle, roundRectangle, arc,  |                     |
   |                 |ellipse, circle, polyline,       |                     |
   |                 |polygon, ,bezier, path, color,   |                     |
   |                 |matte, text, image, @filename},  |                     |
   |                 |points=>string , method=>{Point, |                     |
   |                 |Replace, Floodfill, FillToBorder,|                     |
   |                 |Reset}, stroke=> color name, fill|annotate an image    |
   |Draw             |=>color name, tile=>image-handle,|with one or more     |
   |                 |strokewidth=>float, antialias=>  |graphic primitives   |
   |                 |{true, false}, bordercolor=>color|                     |
   |                 |name, x=> float, y=>float, affine|                     |
   |                 |=>array of float values,         |                     |
   |                 |translate=>float, float, scale=> |                     |
   |                 |float, float, rotate=>float.     |                     |
   |                 |skewX=>float, skewY=> float      |                     |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |enhance edges within |
   |Edge             |radius=>double                   |the image with a     |
   |                 |                                 |convolution filter of|
   |                 |                                 |the given radius.    |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |emboss the image with|
   |                 |geometry=>geometry, radius=>     |a convolution filter |
   |Emboss           |double, sigma=> double           |of the given radius  |
   |                 |                                 |and standard         |
   |                 |                                 |deviation (sigma).   |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |apply a digital      |
   |Enhance          |                                 |filter to enhance a  |
   |                 |                                 |noisy image          |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |perform histogram    |
   |Equalize         |                                 |equalization to the  |
   |                 |                                 |image                |
   +-----------------+---------------------------------+---------------------+
   |Flatten          |                                 |flatten a sequence of|
   |                 |                                 |images               |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |create a mirror image|
   |                 |                                 |by reflecting the    |
   |Flip             |                                 |image scanlines in   |
   |                 |                                 |the vertical         |
   |                 |                                 |direction            |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |create a mirror image|
   |                 |                                 |by reflecting the    |
   |Flop             |                                 |image scanlines in   |
   |                 |                                 |the horizontal       |
   |                 |                                 |direction            |
   +-----------------+---------------------------------+---------------------+
   |                 |geometry=>geometry, width=>      |surround the image   |
   |Frame            |integer, height=> integer, inner |with an ornamental   |
   |                 |=>integer, outer=>integer, fill=>|border               |
   |                 |color name                       |                     |
   +-----------------+---------------------------------+---------------------+
   |Gamma            |gamma=>string, red=>double, green|gamma correct the    |
   |                 |=>double , blue=>double          |image                |
   +-----------------+---------------------------------+---------------------+
   |Implode          |amount=>double                   |implode image pixels |
   |                 |                                 |about the center     |
   +-----------------+---------------------------------+---------------------+
   |Label            |string                           |assign a label to an |
   |                 |                                 |image                |
   +-----------------+---------------------------------+---------------------+
   |                 |level=>string, 'black-point'=>   |adjust the level of  |
   |Level            |double, 'mid-point'=>double,     |image contrast       |
   |                 |'white-point'=>double            |                     |
   +-----------------+---------------------------------+---------------------+
   |Magnify          |                                 |double the size of an|
   |                 |                                 |image                |
   +-----------------+---------------------------------+---------------------+
   |                 |image=>image-handle, dither=>    |choose a particular  |
   |Map              |{True, False}                    |set of colors from   |
   |                 |                                 |this image           |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |changes the matte    |
   |                 |                                 |value of any pixel   |
   |                 |                                 |that matches the     |
   |                 |                                 |color of the target  |
   |                 |geometry=>geometry, x=>integer, y|pixel and is a       |
   |MatteFloodfill   |=>integer , matte=>integer,      |neighbor. If you     |
   |                 |bordercolor=>color name          |specify a border     |
   |                 |                                 |color, the matte     |
   |                 |                                 |value is changed for |
   |                 |                                 |any neighbor pixel   |
   |                 |                                 |that is not that     |
   |                 |                                 |color.               |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |replace each pixel   |
   |MedianFilter     |radius=>double                   |with the median      |
   |                 |                                 |intensity pixel of a |
   |                 |                                 |neighborhood.        |
   +-----------------+---------------------------------+---------------------+
   |Minify           |                                 |half the size of an  |
   |                 |                                 |image                |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |vary the brightness, |
   |Modulate         |brightness=>double, saturation=> |saturation, and hue  |
   |                 |double, hue=> double             |of an image by the   |
   |                 |                                 |specified percentage |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |blur the image with a|
   |                 |                                 |Gaussian operator of |
   |                 |geometry=>geometry, radius=>     |the given radius and |
   |MotionBlur       |double, sigma=> double, angle=>  |standard deviation   |
   |                 |double                           |(sigma) at the given |
   |                 |                                 |angle to simulate the|
   |                 |                                 |effect of motion     |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |replace every pixel  |
   |                 |                                 |with its             |
   |Negate           |gray=>{True, False}              |complementary color  |
   |                 |                                 |(white becomes black,|
   |                 |                                 |yellow becomes blue, |
   |                 |                                 |etc.)                |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |transform image to   |
   |Normalize        |                                 |span the full range  |
   |                 |                                 |of color values      |
   +-----------------+---------------------------------+---------------------+
   |OilPaint         |radius=>integer                  |simulate an oil      |
   |                 |                                 |painting             |
   +-----------------+---------------------------------+---------------------+
   |                 |color=>color name, fill=> color  |change this color to |
   |Opaque           |name                             |the fill color within|
   |                 |                                 |the image            |
   +-----------------+---------------------------------+---------------------+
   |                 |colors=>integer, colorspace=>    |                     |
   |                 |{RGB, Gray, Transparent, OHTA,   |                     |
   |                 |XYZ, YCbCr, YIQ, YPbPr, YUV,     |preferred number of  |
   |Quantize         |CMYK}, treedepth=> integer,      |colors in the image  |
   |                 |dither=>{True, False},           |                     |
   |                 |measure_error=>{True, False},    |                     |
   |                 |global_colormap=>{True, False}   |                     |
   +-----------------+---------------------------------+---------------------+
   |Profile          |name=>{ICM, IPTC}, profile=>blob |add or remove ICC or |
   |                 |                                 |IPTC image profile   |
   +-----------------+---------------------------------+---------------------+
   |                 |geometry=>geometry, width=>      |lighten or darken    |
   |Raise            |integer, height=> integer, x=>   |image edges to create|
   |                 |integer, y=>integer, raise=>     |a 3-D effect         |
   |                 |{True, False}                    |                     |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |reduce noise in the  |
   |ReduceNoise      |radius=>double                   |image with a noise   |
   |                 |                                 |peak elimination     |
   |                 |                                 |filter               |
   +-----------------+---------------------------------+---------------------+
   |                 |geometry=>geometry, width=>      |                     |
   |                 |integer, height=> integer, filter|scale image to       |
   |                 |=>{Point, Box, Triangle, Hermite,|desired size. Specify|
   |Resize           |Hanning, Hamming, Blackman,      |blur > 1 for blurry  |
   |                 |Gaussian, Quadratic, Cubic,      |or < 1 for sharp     |
   |                 |Catrom, Mitchell, Lanczos,       |                     |
   |                 |Bessel, Sinc}, blur=>double      |                     |
   +-----------------+---------------------------------+---------------------+
   |                 |geometry=>geometry, x=>integer, y|roll an image        |
   |Roll             |=>integer                        |vertically or        |
   |                 |                                 |horizontally         |
   +-----------------+---------------------------------+---------------------+
   |Rotate           |degrees=>double, color=>color    |rotate an image      |
   |                 |name                             |                     |
   +-----------------+---------------------------------+---------------------+
   |Sample           |geometry=>geometry, width=>      |scale image with     |
   |                 |integer, height=> integer        |pixel sampling       |
   +-----------------+---------------------------------+---------------------+
   |Scale            |geometry=>geometry, width=>      |scale image to       |
   |                 |integer, height=> integer        |desired size         |
   +-----------------+---------------------------------+---------------------+
   |                 |colorspace=>{RGB, Gray,          |segment an image by  |
   |                 |Transparent, OHTA, XYZ, YCbCr,   |analyzing the        |
   |Segment          |YCC, YIQ, YPbPr, YUV, CMYK},     |histograms of the    |
   |                 |verbose={True, False}, cluster=> |color components and |
   |                 |double, smooth= double           |identifying units    |
   |                 |                                 |that are homogeneous |
   +-----------------+---------------------------------+---------------------+
   |                 |geometry=>geometry, azimuth=>    |shade the image using|
   |Shade            |double, elevation=> double, gray |a distant light      |
   |                 |=>{true, false}                  |source               |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |sharpen the image    |
   |                 |geometry=>geometry, radius=>     |with a Gaussian      |
   |Sharpen          |double, sigma=> double           |operator of the given|
   |                 |                                 |radius and standard  |
   |                 |                                 |deviation (sigma).   |
   +-----------------+---------------------------------+---------------------+
   |Shave            |geometry=>geometry, width=>      |shave pixels from the|
   |                 |integer, height=> integer        |image edges          |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |shear the image along|
   |Shear            |geometry=>geometry, x=>double, y |the X or Y axis by a |
   |                 |=>double color=>color name       |positive or negative |
   |                 |                                 |shear angle          |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |generate an SHA-256  |
   |Signature        |                                 |message digest for   |
   |                 |                                 |the image pixel      |
   |                 |                                 |stream               |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |negate all pixels    |
   |Solarize         |threshold=>integer               |above the threshold  |
   |                 |                                 |level                |
   +-----------------+---------------------------------+---------------------+
   |Spread           |amount=>integer                  |displace image pixels|
   |                 |                                 |by a random amount   |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |composites two images|
   |                 |                                 |and produces a single|
   |Stereo           |image=>image-handle              |image that is the    |
   |                 |                                 |composite of a left  |
   |                 |                                 |and right image of a |
   |                 |                                 |stereo pair          |
   +-----------------+---------------------------------+---------------------+
   |                 |image=>image-handle, offset=>    |hide a digital       |
   |Stegano          |integer                          |watermark within the |
   |                 |                                 |image                |
   +-----------------+---------------------------------+---------------------+
   |Swirl            |degrees=>double                  |swirl image pixels   |
   |                 |                                 |about the center     |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |name of texture to   |
   |Texture          |texture=>image-handle            |tile onto the image  |
   |                 |                                 |background           |
   +-----------------+---------------------------------+---------------------+
   |Threshold        |threshold=>string                |threshold the image  |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |make this color      |
   |Transparent      |color=>color name                |transparent within   |
   |                 |                                 |the image            |
   +-----------------+---------------------------------+---------------------+
   |                 |                                 |remove edges that are|
   |Trim             |                                 |the background color |
   |                 |                                 |from the image       |
   +-----------------+---------------------------------+---------------------+
   |                 |geometry=>geometry, radius=>     |sharpen the image    |
   |UnsharpMask      |double, sigma=> double, amount=> |with the unsharp mask|
   |                 |double, threshold=>double        |algorithm.           |
   +-----------------+---------------------------------+---------------------+
   |Wave             |geometry=>geometry, amplitude=>  |alter an image along |
   |                 |double, wavelength=> double      |a sine wave          |
   +-----------------+---------------------------------+---------------------+

Note, that the geometry parameter is a short cut for the width and height
parameters (e.g. geometry=>'106x80' is equivalent to width=>106,
height=>80).

You can specify @filename in both Annotate() and Draw(). This reads the
text or graphic primitive instructions from a file on disk. For example,

::

  $image->Draw(fill=>'red', primitive=>'rectangle',
  points=>'20,20 100,100 40,40 200,200 60,60 300,300');

Is equivalent to

::

  $image->Draw(fill=>'red', primitive=>'@draw.txt');

Where draw.txt is a file on disk that contains this::

  rectangle 20, 20 100, 100
  rectangle 40, 40 200, 200
  rectangle 60, 60 300, 300

The text parameter for methods, Annotate(), Comment(), Draw(), and
Label() can include the image filename, type, width, height, or other
image attribute by embedding these special format characters::

  %b file size
  %d comment
  %d directory
  %e filename extension
  %f filename
  %h height
  %m magick
  %p page number
  %s scene number
  %t top of filename
  %w width
  %x x resolution
  %y y resolution
  \n newline
  \r carriage return

For example,

::

  text=>"%m:%f %wx%h"

produces an annotation of MIFF:bird.miff 512x480 for an image titled
bird.miff and whose width is 512 and height is 480.

You can optionally add Image to any method name. For example, TrimImage()
is an alias for method Trim().

Most of the attributes listed above have an analog in convert. See the
documentation for a more detailed description of these attributes.


Set an Image Attribute
======================

Use method Set() to set an image attribute. For example,

::

  $image->Set(dither=>'True');
  $image->[$x]->Set(delay=>3);

And here is a list of all the image attributes you can set:

.. table:: Image Attributes

   +----------------+------------------------------------+-------------------+
   |   Attribute    |               Values               |    Description    |
   +----------------+------------------------------------+-------------------+
   |                |                                    |join images into a |
   |adjoin          |{True, False}                       |single multi-image |
   |                |                                    |file               |
   +----------------+------------------------------------+-------------------+
   |antialias       |{True, False}                       |remove pixel       |
   |                |                                    |aliasing           |
   +----------------+------------------------------------+-------------------+
   |authenticate    |string                              |decrypt image with |
   |                |                                    |this password.     |
   +----------------+------------------------------------+-------------------+
   |background      |color name                          |image background   |
   |                |                                    |color              |
   +----------------+------------------------------------+-------------------+
   |                |                                    |chromaticity blue  |
   |blue-primary    |x-value, y-value                    |primary point (e.g.|
   |                |                                    |0.15, 0.06)        |
   +----------------+------------------------------------+-------------------+
   |bordercolor     |color name                          |set the image      |
   |                |                                    |border color       |
   +----------------+------------------------------------+-------------------+
   |                |                                    |Associate a clip   |
   |clip-mask       |image                               |mask with the      |
   |                |                                    |image.             |
   +----------------+------------------------------------+-------------------+
   |                |                                    |color name (e.g.   |
   |colormap[i]     |color name                          |red) or hex value  |
   |                |                                    |(e.g. #ccc) at     |
   |                |                                    |position i         |
   +----------------+------------------------------------+-------------------+
   |colorspace      |{RGB, CMYK}                         |type of colorspace |
   +----------------+------------------------------------+-------------------+
   |comment         |string                              |Append to the image|
   |                |                                    |comment.           |
   +----------------+------------------------------------+-------------------+
   |compression     |{None, BZip, Fax, Group4, JPEG,     |type of image      |
   |                |LosslessJPEG, LZW, RLE, Zip, LZMA   |compression        |
   |                |JPEG2000, JBIG1, JBIG2}             |                   |
   +----------------+------------------------------------+-------------------+
   |                |{No, Configure, Annotate, Render,   |log copious        |
   |debug           |Transform, Locale, Coder, X11,      |debugging          |
   |                |Cache, Blob, Deprecate, User,       |information  for   |
   |                |Resource, TemporaryFile,            |one or more event  |
   |                |Exception,All}                      |types              |
   +----------------+------------------------------------+-------------------+
   |                |                                    |this many 1/100ths |
   |                |                                    |of a second must   |
   |delay           |integer                             |expire before      |
   |                |                                    |displaying the next|
   |                |                                    |image in a sequence|
   +----------------+------------------------------------+-------------------+
   |                |                                    |vertical and       |
   |density         |geometry                            |horizontal         |
   |                |                                    |resolution in      |
   |                |                                    |pixels of the image|
   +----------------+------------------------------------+-------------------+
   |disk-limit      |integer                             |set disk resource  |
   |                |                                    |limit in megabytes |
   +----------------+------------------------------------+-------------------+
   |dispose         |{Undefined, None, Background,       |GIF disposal method|
   |                |Previous}                           |                   |
   +----------------+------------------------------------+-------------------+
   |                |                                    |apply error        |
   |dither          |{True, False}                       |diffusion to the   |
   |                |                                    |image              |
   +----------------+------------------------------------+-------------------+
   |display         |string                              |specifies the X    |
   |                |                                    |server to contact  |
   +----------------+------------------------------------+-------------------+
   |endian          |{Undefined, LSB, MSB, Native}       |specifies the      |
   |                |                                    |ordering of bytes  |
   |                |                                    |in a multi-byte    |
   |                |                                    |word. MSB is       |
   |                |                                    |big-endian, LSB is |
   |                |                                    |little-endian, and |
   |                |                                    |Native is whatever |
   |                |                                    |the current host   |
   |                |                                    |uses by default.   |
   +----------------+------------------------------------+-------------------+
   |file            |filehandle                          |set the image      |
   |                |                                    |filehandle         |
   +----------------+------------------------------------+-------------------+
   |filename        |string                              |set the image      |
   |                |                                    |filename           |
   +----------------+------------------------------------+-------------------+
   |                |                                    |The fill color     |
   |fill            |color                               |paints any areas   |
   |                |                                    |inside the outline |
   |                |                                    |of drawn shape.    |
   +----------------+------------------------------------+-------------------+
   |                |                                    |use this font when |
   |font            |string                              |annotating the     |
   |                |                                    |image with text    |
   +----------------+------------------------------------+-------------------+
   |                |                                    |colors within this |
   |fuzz            |integer                             |distance are       |
   |                |                                    |considered equal   |
   +----------------+------------------------------------+-------------------+
   |gamma           |double                              |gamma level of the |
   |                |                                    |image              |
   +----------------+------------------------------------+-------------------+
   |                |{Forget, NorthWest, North,          |type of image      |
   |Gravity         |NorthEast, West, Center, East,      |gravity            |
   |                |SouthWest, South, SouthEast}        |                   |
   +----------------+------------------------------------+-------------------+
   |                |                                    |chromaticity green |
   |green-primary   |x-value, y-value                    |primary point (e.g.|
   |                |                                    |0.3, 0.6)          |
   +----------------+------------------------------------+-------------------+
   |index[x, y]     |string                              |colormap index at  |
   |                |                                    |position (x, y)    |
   +----------------+------------------------------------+-------------------+
   |interlace       |{None, Line, Plane, Partition}      |the type of        |
   |                |                                    |interlacing scheme |
   +----------------+------------------------------------+-------------------+
   |                |                                    |add Netscape loop  |
   |iterations      |integer                             |extension to your  |
   |                |                                    |GIF animation      |
   +----------------+------------------------------------+-------------------+
   |label           |string                              |Append to the image|
   |                |                                    |label.             |
   +----------------+------------------------------------+-------------------+
   |                |                                    |add Netscape loop  |
   |loop            |integer                             |extension to your  |
   |                |                                    |GIF animation      |
   +----------------+------------------------------------+-------------------+
   |magick          |string                              |set the image      |
   |                |                                    |format             |
   +----------------+------------------------------------+-------------------+
   |matte           |{True, False}                       |True if the image  |
   |                |                                    |has transparency   |
   +----------------+------------------------------------+-------------------+
   |mattecolor      |color name                          |set the image matte|
   |                |                                    |color              |
   +----------------+------------------------------------+-------------------+
   |map-limit       |integer                             |set map resource   |
   |                |                                    |limit in megabytes |
   +----------------+------------------------------------+-------------------+
   |memory-limit    |integer                             |set memory resource|
   |                |                                    |limit in megabytes |
   +----------------+------------------------------------+-------------------+
   |monochrome      |{True, False}                       |transform the image|
   |                |                                    |to black and white |
   +----------------+------------------------------------+-------------------+
   |                |{ Letter, Tabloid, Ledger, Legal,   |preferred size and |
   |page            |Statement, Executive, A3, A4, A5,   |location of an     |
   |                |B4, B5, Folio, Quarto, 10x14} or    |image canvas       |
   |                |geometry                            |                   |
   +----------------+------------------------------------+-------------------+
   |                |                                    |hex value (e.g. #  |
   |pixel[x, y]     |string                              |ccc) at position (x|
   |                |                                    |, y)               |
   +----------------+------------------------------------+-------------------+
   |                |                                    |pointsize of the   |
   |pointsize       |integer                             |Postscript or      |
   |                |                                    |TrueType font      |
   +----------------+------------------------------------+-------------------+
   |                |{ Rotate, Shear, Roll, Hue,         |                   |
   |                |Saturation, Brightness, Gamma,      |                   |
   |                |Spiff, Dull, Grayscale, Quantize,   |                   |
   |                |Despeckle, ReduceNoise, AddNoise,   |type of preview for|
   |preview         |Sharpen, Blur, Threshold,           |the Preview image  |
   |                |EdgeDetect, Spread, Solarize, Shade,|format             |
   |                |Raise, Segment, Swirl, Implode,     |                   |
   |                |Wave, OilPaint, Charcoal,           |                   |
   |                |JPEG}                               |                   |
   +----------------+------------------------------------+-------------------+
   |quality         |integer                             |JPEG/MIFF/PNG      |
   |                |                                    |compression level  |
   +----------------+------------------------------------+-------------------+
   |                |                                    |chromaticity red   |
   |red-primary     |x-value, y-value                    |primary point (e.g.|
   |                |                                    |0.64, 0.33)        |
   +----------------+------------------------------------+-------------------+
   |rendering-intent|{Undefined, Saturation, Perceptual, |the type of        |
   |                |Absolute, Relative}                 |rendering intent   |
   +----------------+------------------------------------+-------------------+
   |                |                                    |horizontal and     |
   |sampling-factor |geometry                            |vertical sampling  |
   |                |                                    |factor             |
   +----------------+------------------------------------+-------------------+
   |scene           |integer                             |image scene number |
   +----------------+------------------------------------+-------------------+
   |subimage        |integer                             |subimage of an     |
   |                |                                    |image sequence     |
   +----------------+------------------------------------+-------------------+
   |                |                                    |number of images   |
   |subrange        |integer                             |relative to the    |
   |                |                                    |base image         |
   +----------------+------------------------------------+-------------------+
   |server          |string                              |specifies the X    |
   |                |                                    |server to contact  |
   +----------------+------------------------------------+-------------------+
   |size            |string                              |width and height of|
   |                |                                    |a raw image        |
   +----------------+------------------------------------+-------------------+
   |                |                                    |The stroke color   |
   |stroke          |color                               |paints along the   |
   |                |                                    |outline of a shape.|
   +----------------+------------------------------------+-------------------+
   |tile            |string                              |tile name          |
   +----------------+------------------------------------+-------------------+
   |                |                                    |name of texture to |
   |texture         |string                              |tile onto the image|
   |                |                                    |background         |
   +----------------+------------------------------------+-------------------+
   |                |{Bilevel, Grayscale, GrayscaleMatte,|                   |
   |type            |Palette, PaletteMatte, TrueColor,   |image type         |
   |                |TrueColorMatte, ColorSeparation,    |                   |
   |                |ColorSeparationMatte, Optimize }    |                   |
   +----------------+------------------------------------+-------------------+
   |units           |{ Undefined, PixelsPerInch,         |units of image     |
   |                |PixelsPerCentimeters}               |resolution         |
   +----------------+------------------------------------+-------------------+
   |                |                                    |print detailed     |
   |verbose         |{True, False}                       |information about  |
   |                |                                    |the image          |
   +----------------+------------------------------------+-------------------+
   |virtual-pixel   |{Constant, Edge, Mirror, Tile}      |the virtual pixel  |
   |                |                                    |method             |
   +----------------+------------------------------------+-------------------+
   |                |                                    |chromaticity white |
   |white-point     |x-value, y-value                    |point (e.g. 0.3127,|
   |                |                                    |0.329)             |
   +----------------+------------------------------------+-------------------+

Note, that the geometry parameter is a short cut for the width and height
parameters (e.g. geometry=>'106x80' is equivalent to width=>106, height=>
80).

SetAttribute() is an alias for method Set().

Most of the attributes listed above have an analog in gm convert. See the
gm documentation for a more detailed description of these attributes.


Get an Image Attribute
======================

Use method Get() to get an image attribute. For example,

::

  ($a, $b, $c) = $image->Get('colorspace', 'magick', 'adjoin');
  $width = $image->[3]->Get('columns');

In addition to all the attributes listed in Set an Image Attribute , you
can get these additional attributes:

.. table:: Image Attributes

   +-------------+----------+------------------------------------------------+
   |  Attribute  |  Values  |                  Description                   |
   +-------------+----------+------------------------------------------------+
   |base-columns |integer   |base image width (before transformations)       |
   +-------------+----------+------------------------------------------------+
   |base-filename|string    |base image filename (before transformations)    |
   +-------------+----------+------------------------------------------------+
   |base-rows    |integer   |base image height (before transformations)      |
   +-------------+----------+------------------------------------------------+
   |class        |{Direct,  |image class                                     |
   |             |Pseudo}   |                                                |
   +-------------+----------+------------------------------------------------+
   |colors       |integer   |number of unique colors in the image            |
   +-------------+----------+------------------------------------------------+
   |comment      |string    |image comment                                   |
   +-------------+----------+------------------------------------------------+
   |columns      |integer   |image width                                     |
   +-------------+----------+------------------------------------------------+
   |depth        |integer   |image depth                                     |
   +-------------+----------+------------------------------------------------+
   |directory    |string    |tile names from within an image montage         |
   +-------------+----------+------------------------------------------------+
   |error        |double    |the mean error per pixel computed with methods  |
   |             |          |Compare() or Quantize()                         |
   +-------------+----------+------------------------------------------------+
   |filesize     |integer   |number of bytes of the image on disk            |
   +-------------+----------+------------------------------------------------+
   |format       |string    |get the descriptive image format                |
   +-------------+----------+------------------------------------------------+
   |geometry     |string    |image geometry                                  |
   +-------------+----------+------------------------------------------------+
   |height       |integer   |the number of rows or height of an image        |
   +-------------+----------+------------------------------------------------+
   |id           |integer   |GraphicsMagick registry id                      |
   +-------------+----------+------------------------------------------------+
   |label        |string    |image label                                     |
   +-------------+----------+------------------------------------------------+
   |maximum-error|double    |the normalized max error per pixel computed with|
   |             |          |methods Compare() or Quantize()                 |
   +-------------+----------+------------------------------------------------+
   |mean-error   |double    |the normalized mean error per pixel computed    |
   |             |          |with methods Compare() or Quantize()            |
   +-------------+----------+------------------------------------------------+
   |montage      |geometry  |tile size and offset within an image montage    |
   +-------------+----------+------------------------------------------------+
   |rows         |integer   |the number of rows or height of an image        |
   +-------------+----------+------------------------------------------------+
   |signature    |string    |SHA-256 message digest associated with the image|
   |             |          |pixel stream                                    |
   +-------------+----------+------------------------------------------------+
   |taint        |{True,    |True if the image has been modified             |
   |             |False}    |                                                |
   +-------------+----------+------------------------------------------------+
   |width        |integer   |the number of columns or width of an image      |
   +-------------+----------+------------------------------------------------+
   |x-resolution |integer   |x resolution of the image                       |
   +-------------+----------+------------------------------------------------+
   |y-resolution |integer   |y resolution of the image                       |
   +-------------+----------+------------------------------------------------+

GetAttribute() is an alias for method Get().

Most of the attributes listed above have an analog in convert. See the
documentation for a more detailed description of these attributes.


Create an Image Montage
=======================

Use method Montage() to create a composite image by combining several
separate images. The images are tiled on the composite image with the
name of the image optionally appearing just below the individual tile.
For example,

::

  $image->Montage(geometry=>'160x160', tile=>'2x2', texture=>'granite:');

And here is a list of Montage() parameters you can set:

.. table:: Montage Parameters

   +-----------+-------------------------------------+-----------------------+
   | Parameter |               Values                |      Description      |
   +-----------+-------------------------------------+-----------------------+
   |background |color name                           |background color name  |
   +-----------+-------------------------------------+-----------------------+
   |borderwidth|integer                              |image border width     |
   +-----------+-------------------------------------+-----------------------+
   |           |{Over, In, Out, Atop, Xor, Plus,     |                       |
   |compose    |Minus, Add, Subtract, Difference,    |composite operator     |
   |           |Bumpmap, Copy, Mask, Dissolve, Clear,|                       |
   |           |Displace}                            |                       |
   +-----------+-------------------------------------+-----------------------+
   |filename   |string                               |name of montage image  |
   +-----------+-------------------------------------+-----------------------+
   |fill       |color name                           |fill color for         |
   |           |                                     |annotations            |
   +-----------+-------------------------------------+-----------------------+
   |font       |string                               |X11 font name          |
   +-----------+-------------------------------------+-----------------------+
   |frame      |geometry                             |surround the image with|
   |           |                                     |an ornamental border   |
   +-----------+-------------------------------------+-----------------------+
   |           |                                     |preferred tile and     |
   |geometry   |geometry                             |border size of each    |
   |           |                                     |tile of the composite  |
   |           |                                     |image                  |
   +-----------+-------------------------------------+-----------------------+
   |           |{NorthWest, North, NorthEast, West,  |direction image        |
   |gravity    |Center, East, SouthWest, South,      |gravitates to within a |
   |           |SouthEast}                           |tile                   |
   +-----------+-------------------------------------+-----------------------+
   |ICM        |blob                                 |color information      |
   |           |                                     |profile                |
   +-----------+-------------------------------------+-----------------------+
   |IPTC       |blob                                 |newswire information   |
   |           |                                     |profile                |
   +-----------+-------------------------------------+-----------------------+
   |label      |string                               |assign a label to an   |
   |           |                                     |image                  |
   +-----------+-------------------------------------+-----------------------+
   |mode       |{Frame, Unframe, Concatenate}        |thumbnail framing      |
   |           |                                     |options                |
   +-----------+-------------------------------------+-----------------------+
   |           |                                     |pointsize of the       |
   |pointsize  |integer                              |Postscript or TrueType |
   |           |                                     |font                   |
   +-----------+-------------------------------------+-----------------------+
   |shadow     |{True, False}                        |add a shadow beneath a |
   |           |                                     |tile to simulate depth |
   +-----------+-------------------------------------+-----------------------+
   |stroke     |color name                           |stroke color for       |
   |           |                                     |annotations            |
   +-----------+-------------------------------------+-----------------------+
   |           |                                     |name of texture to tile|
   |texture    |string                               |onto the image         |
   |           |                                     |background             |
   +-----------+-------------------------------------+-----------------------+
   |tile       |geometry                             |number of tiles per row|
   |           |                                     |and column             |
   +-----------+-------------------------------------+-----------------------+
   |title      |string                               |assign a title to the  |
   |           |                                     |image montage          |
   +-----------+-------------------------------------+-----------------------+
   |           |                                     |make this color        |
   |transparent|string                               |transparent within the |
   |           |                                     |image                  |
   +-----------+-------------------------------------+-----------------------+

Note, that the geometry parameter is a short cut for the width and height
parameters (e.g. geometry=>'106x80' is equivalent to width=>106, height=>
80).

MontageImage() is an alias for method Montage().

Most of the attributes listed above have an analog in montage. See the
documentation for a more detailed description of these attributes.


Working with Blobs
==================

A blob contains data that directly represent a particular image format in
memory instead of on disk. PerlMagick supports blobs in any of these
image formats and provides methods to convert a blob to or from a
particular image format.

.. table:: Blob Methods

   +-----------+----------+------------------------+-------------------------+
   |  Method   |Parameters|      Return Value      |       Description       |
   +-----------+----------+------------------------+-------------------------+
   |           |any image |an array of image data  |convert an image or image|
   |ImageToBlob|attribute |in the respective image |sequence to an array of  |
   |           |          |format                  |blobs                    |
   +-----------+----------+------------------------+-------------------------+
   |BlobToImage|one or    |the number of blobs     |convert one or more blobs|
   |           |more blobs|converted to an image   |to an image              |
   +-----------+----------+------------------------+-------------------------+

ImageToBlob() returns the image data in their respective formats. You can
then print it, save it to an ODBC database, write it to a file, or pipe
it to a display program::

  @blobs = $image->ImageToBlob();
  open(DISPLAY,"| display -") || die;
  binmode DISPLAY;
  print DISPLAY $blobs[0];
  close DISPLAY;

Method BlobToImage() returns an image or image sequence converted from
the supplied blob::

  @blob=$db->GetImage();
  $image=Graphics::Magick->new(magick=>'jpg');
  $image->BlobToImage(@blob);

Miscellaneous Methods
=====================

The Append() method append a set of images. For example,

::

  $p = $image->Append(stack=>{true,false});

appends all the images associated with object $image. By default, images
are stacked left-to-right. Set stack to True to stack them top-to-bottom.

The Average() method averages a set of images. For example,

::

  $p = $image->Average();

averages all the images associated with object $image.

The Clone() method copies a set of images. For example,

::

  $p = $image->Clone();

copies all the images from object $q to $p. You can use this method for
single or multi-image sequences.

The Morph() method morphs a set of images. Both the image pixels and size
are linearly interpolated to give the appearance of a meta-morphosis from
one image to the next::

  $p = $image->Morph(frames=>integer);

where frames is the number of in-between images to generate. The default
is 1.

Mosaic() creates an mosaic from an image sequence.

Method Mogrify() is a single entry point for the image manipulation
methods (Manipulate an Image). The parameters are the name of a method
followed by any parameters the method may require. For example, these
calls are equivalent::

  $image->Crop('340x256+0+0');
  $image->Mogrify('crop', '340x256+0+0');

Method MogrifyRegion() applies a transform to a region of the image. It
is similar to Mogrify() but begins with the region geometry. For example,
suppose you want to brighten a 100x100 region of your image at location
(40, 50)::

  $image->MogrifyRegion('100x100+40+50', 'modulate', brightness=>50);

Ping() is a convenience method that returns information about an image
without having to read the image into memory. It returns the width,
height, file size in bytes, and the file format of the image. You can
specify more than one filename but only one filehandle::

  ($width, $height, $size, $format) = $image->Ping('logo.png');
  ($width, $height, $size, $format) = $image->Ping(file=>\*IMAGE);
  ($width, $height, $size, $format) = $image->Ping(blob=>@blob);

This is a more efficient and less memory intensive way to query if an
image exists and what its characteristics are.

To have full control over text positioning you need font metric
information. Use

::

  ($x_ppem, $y_ppem, $ascender, $descender, $width, $height, $max_advance) =
  $image->QueryFontMetrics(parameters);

  Where parameters is any parameter of the Annotate method.  The
  'text' parameter must be specified since there can be no default for
  the text to render. The return values are

  * character width
  * character height
  * ascender
  * descender
  * text width
  * text height
  * maximum horizontal advance

Call QueryColor() with no parameters to return a list of known colors
names or specify one or more color names to get these attributes: red,
green, blue, and opacity value.

::

  @colors = $image->QueryColor();
  ($red, $green, $blue, $opacity) = $image->QueryColor('cyan');
  ($red, $green, $blue, $opacity) = $image->QueryColor('#716bae');

QueryColorname() accepts a color value and returns its respective name or
hex value;

::

  $name = $image->QueryColorname('rgba(80,60,0,0)');

Call QueryFont() with no parameters to return a list of known fonts or
specify one or more font names to get these attributes: font name,
description, family, style, stretch, weight, encoding, foundry, format,
metrics, and glyphs values.

::

  @fonts = $image->QueryFont();
  $weight = ($image->QueryFont('Helvetica'))[5];

Call QueryFormat() with no parameters to return a list of known image
formats or specify one or more format names to get these attributes:
adjoin, blob support, raw, decoder, encoder, description, and module.

::

  @formats = $image->QueryFormat();
  ($adjoin, $blob_support, $raw, $decoder, $encoder, $description, $module) = $image->QueryFormat('gif');

Use RemoteCommand() to send a command to an already running display or
animate application. The only parameter is the name of the image file to
display or animate.

Finally, the Transform() method accepts a fully-qualified geometry
specification for cropping or resizing one or more images. For example,

::

  $p = $image->Transform(crop=>'100x100');

You can optionally add Image to any method name above. For example,
PingImage() is an alias for method Ping().


Handling Errors
===============

All PerlMagick methods return an undefined string context upon success. If
any problems occur, the error is returned as a string with an embedded
numeric status code. A status code less than 400 is a warning. This means
that the operation did not complete but was recoverable to some degree. A
numeric code greater or equal to 400 is an error and indicates the
operation failed completely. Here is how errors are returned for the
different methods:

  + Methods which return a number (e.g. Read(), Write())::

      $status = $image->Read(...);
      warn "$status" if "$status"; # print the error message
      $status =~ /(\d+)/;
      print $1; # print the error number
      print 0+$status; # print the number of images read

  + Methods which operate on an image (e.g. Resize(), Crop())::

      $status = $image->Crop(...);
      warn "$status" if "$status"; # print the error message
      $status =~ /(\d+)/;
      print $1; # print the error number

  + Methods which return images (Average(), Montage(), Clone()) should be
    checked for errors this way::

      $status = $image->Montage(...);
      warn "$status" if !ref($status); # print the error message
      $status =~ /(\d+)/;
      print $1; # print the error number

Here is an example error message::

  Error 400: Memory allocation failed

Below is a list of error and warning codes:

.. table:: Error and Warning Codes

   +----+----------------------+---------------------------------------------+
   |Code|       Mnemonic       |                 Description                 |
   +----+----------------------+---------------------------------------------+
   |0   |Success               |method completed without an error or warning |
   +----+----------------------+---------------------------------------------+
   |300 |ResourceLimitWarning  |a program resource is exhausted (e.g. not    |
   |    |                      |enough memory)                               |
   +----+----------------------+---------------------------------------------+
   |305 |TypeWarning           |A font is unavailable; a substitution may    |
   |    |                      |have occurred                                |
   +----+----------------------+---------------------------------------------+
   |310 |OptionWarning         |a command-line option was malformed          |
   +----+----------------------+---------------------------------------------+
   |315 |DelegateWarning       |an GraphicsMagick delegate returned a warning|
   +----+----------------------+---------------------------------------------+
   |320 |MissingDelegateWarning|the image type can not be read or written    |
   |    |                      |because the appropriate Delegate is missing  |
   +----+----------------------+---------------------------------------------+
   |325 |CorruptImageWarning   |the image file may be corrupt                |
   +----+----------------------+---------------------------------------------+
   |330 |FileOpenWarning       |the image file could not be opened           |
   +----+----------------------+---------------------------------------------+
   |335 |BlobWarning           |a binary large object could not be allocated |
   +----+----------------------+---------------------------------------------+
   |340 |StreamWarning         |there was a problem reading or writing from a|
   |    |                      |stream                                       |
   +----+----------------------+---------------------------------------------+
   |345 |CacheWarning          |pixels could not be saved to the pixel cache |
   +----+----------------------+---------------------------------------------+
   |350 |CoderWarning          |there was a problem with an image coder      |
   +----+----------------------+---------------------------------------------+
   |355 |ModuleWarning         |there was a problem with an image module     |
   +----+----------------------+---------------------------------------------+
   |360 |DrawWarning           |a drawing operation failed                   |
   +----+----------------------+---------------------------------------------+
   |365 |ImageWarning          |the operation could not complete due to an   |
   |    |                      |incompatible image                           |
   +----+----------------------+---------------------------------------------+
   |380 |XServerWarning        |an X resource is unavailable                 |
   +----+----------------------+---------------------------------------------+
   |385 |MonitorWarning        |there was a problem with prgress monitor     |
   +----+----------------------+---------------------------------------------+
   |390 |RegistryWarning       |there was a problem getting or setting the   |
   |    |                      |registry                                     |
   +----+----------------------+---------------------------------------------+
   |395 |ConfigureWarning      |there was a problem getting a configuration  |
   |    |                      |file                                         |
   +----+----------------------+---------------------------------------------+
   |400 |ResourceLimitError    |a program resource is exhausted (e.g. not    |
   |    |                      |enough memory)                               |
   +----+----------------------+---------------------------------------------+
   |405 |TypeError             |A font is unavailable; a substitution may    |
   |    |                      |have occurred                                |
   +----+----------------------+---------------------------------------------+
   |410 |OptionError           |a command-line option was malformed          |
   +----+----------------------+---------------------------------------------+
   |415 |DelegateError         |an GraphicsMagick delegate returned a warning|
   +----+----------------------+---------------------------------------------+
   |420 |MissingDelegateError  |the image type can not be read or written    |
   |    |                      |because the appropriate Delegate is missing  |
   +----+----------------------+---------------------------------------------+
   |425 |CorruptImageError     |the image file may be corrupt                |
   +----+----------------------+---------------------------------------------+
   |430 |FileOpenError         |the image file could not be opened           |
   +----+----------------------+---------------------------------------------+
   |435 |BlobError             |a binary large object could not be allocated |
   +----+----------------------+---------------------------------------------+
   |440 |StreamError           |there was a problem reading or writing from a|
   |    |                      |stream                                       |
   +----+----------------------+---------------------------------------------+
   |445 |CacheError            |pixels could not be saved to the pixel cache |
   +----+----------------------+---------------------------------------------+
   |450 |CoderError            |there was a problem with an image coder      |
   +----+----------------------+---------------------------------------------+
   |455 |ModuleError           |there was a problem with an image module     |
   +----+----------------------+---------------------------------------------+
   |460 |DrawError             |a drawing operation failed                   |
   +----+----------------------+---------------------------------------------+
   |465 |ImageError            |the operation could not complete due to an   |
   |    |                      |incompatible image                           |
   +----+----------------------+---------------------------------------------+
   |480 |XServerError          |an X resource is unavailable                 |
   +----+----------------------+---------------------------------------------+
   |480 |MonitorError          |there was a progress monitor error           |
   +----+----------------------+---------------------------------------------+
   |490 |RegistryError         |there was a problem getting or setting the   |
   |    |                      |registry                                     |
   +----+----------------------+---------------------------------------------+
   |495 |ConfigureError        |there was a problem getting a configuration  |
   |    |                      |file                                         |
   +----+----------------------+---------------------------------------------+

The following illustrates how you can use a numeric status code::

  $status = $image->Read('rose.png');
  $status =~ /(\d+)/;
  die "unable to continue" if ($1 == ResourceLimitError);


-------------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2015
