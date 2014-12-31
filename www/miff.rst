.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

========================
Magick Image File Format
========================

Magick Image File Format (MIFF) is a platform-independent format for
storing bitmap images. MIFF was originally invented by John Cristy for
ImageMagick, but is also a native format of GraphicsMagick. It is useful
as an efficient lossless working file format which assures that all of
the image attributes used by GraphicsMagick are preserved. Several
lossless compression algorithms are available in order to save space.

Description
===========

A MIFF image file consist of two sections. The first section is a header
composed of keywords describing the image in text form. The next section
is the binary image data. The header is separated from the image data by
a : character immediately followed by a ctrl-Z.

The MIFF header is composed entirely of LATIN-1 characters. The fields in
the header are keyword and value combination in the keyword=value format,
with each keyword and value separated by an equal sign (=). Each
keyword=value combination is delimited by at least one control or
whitespace character. Comments may appear in the header section and are
always delimited by braces. The MIFF header always ends with a colon (:)
character, followed by a ctrl-Z character. It is also common to precede
the colon with a formfeed and a newline character. The formfeed prevents
the listing of binary data when using more(1) under Unix where the ctrl-Z
has the same effect with the type command on the Win32 command line.

The following is a list of keyword=value combinations that may be found
in a MIFF file:

background-color=color

border-color=color

matte-color=color

  these optional keywords reflects the image background, border, and
  matte colors respectively. A color can be a name (e.g. white) or a hex
  value (e.g. #ccc).

class=DirectClass

class=PseudoClass

  the type of binary image data stored in the MIFF file. If this keyword
  is not present, DirectClass image data is assumed.

colors=value

  the number of colors in a DirectClass image. For a PseudoClass image,
  this keyword specifies the size of the colormap. If this keyword is not
  present in the header, and the image is PseudoClass, a linear 256 color
  grayscale colormap is used with the image data. The maximum number of
  colormap entries is 65535.

columns=value

  the width of the image in pixels. This is a required keyword and has no
  default.

colorspace=RGB

colorspace=CMYK

  the colorspace of the pixel data. The default is RGB.

compression=BZip

compression=RLE

compression=Zip

  the type of algorithm used to compress the image data. If this keyword
  is not present, the image data is assumed to be uncompressed.

delay <1/100ths of a second>

  the interframe delay in an image sequence. The maximum delay is 65535.

depth=8

depth=16

depth=32

  depth of a single color value representing values from 0 to 255 (depth
  8), 65535 (depth 16), or 4294967295 (depth 32). If this keyword is
  absent, a depth of 8 is assumed.

dispose value

  GIF disposal method. Here are the valid methods:

    +-----------+------------------------------------------------+
    | Disposal  |                  Description                   |
    +-----------+------------------------------------------------+
    |     0     | No disposal specified.                         |
    +-----------+------------------------------------------------+
    |     1     | Do not dispose between frames.                 |
    +-----------+------------------------------------------------+
    |           | Overwrite frame with background color from     |
    |     2     | header.                                        |
    +-----------+------------------------------------------------+
    |     3     | Overwrite with previous frame.                 |
    +-----------+------------------------------------------------+

gamma=value

  gamma of the image. If it is not specified, a gamma of 1.0 (linear
  brightness response) is assumed,

id=GraphicsMagick

  identify the file as a MIFF-format image file. This keyword is required
  and has no default. Although this keyword can appear anywhere in the
  header, it should start as the first keyword of the header in column 1.
  This will allow programs like file(1) to easily identify the file as
  MIFF.

iterations value

  the number of times an image sequence loops before stopping.

label={value}

  this optional keyword defines a short title or caption for the image.
  If any whitespace appears in the label, it must be enclosed within
  double quotes.

matte=True

matte=False

  specifies whether a DirectClass image has matte data. Matte data is
  generally useful for image compositing. This keyword has no meaning for
  pseudocolor images.

montage=<width>x<height>{+-}<xoffset>{+-}<yoffset>

  size and location of the individual tiles of a composite image. See
  X(1) for details about the geometry specification. Use this keyword
  when the image is a composite of a number of different tiles. A tile
  consists of an image and optionally a border and a label. < width> is
  the size in pixels of each individual tile in the horizontal direction
  and <height> is the size in the vertical direction. Each tile must have
  an equal number of pixels in width and equal in height. However, the
  width can differ from the height. <xoffset> is the offset in number of
  pixels from the vertical edge of the composite image where the first
  tile of a row begins and <yoffset> is the offset from the horizontal
  edge where the first tile of a column begins. If this keyword is
  specified, a directory of tile names must follow the image header. The
  format of the directory is explained below.

page=<width>x<height>{+-}<xoffset>{+-}<yoffset>

  preferred size and location of an image canvas.

profile-icc=value

  the number of bytes in the International Color Consortium color
  profile. The profile is defined by the ICC profile specification.

profile-iptc=value

  the number of bytes in the IPTC Newsphoto profile. The profile is
  defined by the IPTC specification.

profile-name=value

  the number of bytes in the generic profile name where name identifies
  the profile. Name is substituted with any LATIN-1 string to form a
  unique generic profile identifier.

red-primary=x,y

green-primary=x,y

blue-primary=x,y

white-point=x,y

  these optional keywords reflect the chromaticity primaries and white point.

rendering-intent=saturation

rendering-intent=perceptual

rendering-intent=absolute

rendering-intent=relative

  Rendering intent is the CSS-1 property that has been defined by the
  International Color Consortium.

resolution=<x-resolution>x<y-resolution>

  vertical and horizontal resolution of the image. See units for the
  specific resolution units (e.g. pixels per inch).

rows=value

  the height of the image in pixels. This is a required keyword and has
  no default.

scene=value

  the sequence number for this MIFF image file. This optional keyword is
  used when a MIFF image file is one in a sequence of files used in an
  animation.

signature=value

  this optional keyword contains a string that uniquely identifies the
  image pixel contents. NIST's SHA-256 message digest algorithm is
  recommended.

units=pixels-per-inch

units=pixels-per-centimeter

  image resolution units.

Other key value pairs are permitted. If a value contains whitespace it must be
enclosed with braces as illustrated here::

  id=GraphicsMagick
  class=PseudoClass colors=256
  compression=RLE
  columns=1280 rows=1024
  scene=1
  signature=d79e1c308aa5bbcdeea8ed63df412da9
  copyright={Copyright (c) 2000 Mortimer Snerd}
  <FF>
  :

Note that keyword=value combinations may be separated by newlines or
spaces and may occur in any order within the header. Comments (within
braces) may appear anywhere before the colon.

The elements shown in the following table may appear after the header and
before the image data. These elements appear in the order described in
the following table if the keyword indicates that they exist.

+----------------+----------------+-------------------------------------------+
|    Element     |    Keyword     |                Description                |
+----------------+----------------+-------------------------------------------+
| Image          |    montage     | The directory consists of a name for each |
| directory      |                | tile of the composite image separated by  |
|                |                | a newline character. The list is          |
|                |                | terminated with a NULL character.         |
+----------------+----------------+-------------------------------------------+
| ICC Profile    | profile-icc    | Binary color profile.                     |
+----------------+----------------+-------------------------------------------+
| IPTC Profile   | profile-iptc   | Binary IPTC Newsphoto profile.            |
+----------------+----------------+-------------------------------------------+
| Generic        | profile-<name> | Binary generic profile. Multiple named    |
| Profiles       |                | generic profiles may exist.               |
+----------------+----------------+-------------------------------------------+

Next comes the binary image data itself. How the image data is formatted
depends upon the class of the image as specified (or not specified) by
the value of the class keyword in the header. All numeric values in the
binary section are written with the most significant bytes occuring first
(big-endian ordering).

DirectClass images (class=DirectClass) are continuous-tone, images stored
as RGB (red, green, blue), RGBA (red, green, blue, alpha), CMYK (cyan,
yellow, magenta, black), and CMYKA (cyan, yellow, magenta, black, alpha)
intensity values as defined by the colorspace and matte keywords. The
size of each intensity value depends on the depth of the image. The
depth, number of bytes, and numeric range of each value are shown in the
following table:

  +--------+----------------+---------------+
  | Depth  |    Bytes Per   | Value Range   |
  |        |    Value       |               |
  +--------+----------------+---------------+
  |   8    |       1        | 0..255        |
  +--------+----------------+---------------+
  |   16   |       2        | 0..65535      |
  +--------+----------------+---------------+
  |   32   |       4        | 0..4294967295 |
  +--------+----------------+---------------+
        
The alpha value (if it occurs) represents the degree of pixel opacity
(zero is totally transparent).

PseudoClass images (class=PseudoClass) are colormapped RGB images. The
colormap is stored as a series of red, green, and blue pixel values. The
size of each colormap value depends on the image depth, as shown in the
following table:

  +-----------+-------------+------------------+----------------+
  |   Depth   |  Bytes Per  |   Value Range    | Bytes Per      |
  |           |  Value      |                  | Colormap Entry |
  +-----------+-------------+------------------+----------------+
  |     8     |      1      | 0..255           |       3        |
  +-----------+-------------+------------------+----------------+
  |    16     |      2      | 0..65535         |       6        |
  +-----------+-------------+------------------+----------------+
  |    32     |      4      | 0..4294967295    |       12       |
  +-----------+-------------+------------------+----------------+

The number of colormap entries is defined by the colors keyword. The
colormap data occurs immediately following the header (or image directory
if the montage keyword is in the header). Immediately following the
colormap data is the PseudoClass image data. PseudoClass image data is an
array of index values into the color map. The number of bytes comprising
the index value depends on the number of colors in the image. The
following table shows the number of bytes in each colormap index as
determined by the colors keyword:

  +----------------+----------------+--------------------+
  |     Colors     |   Bytes Per    |    Index Range     |
  |                |   Index        |                    |
  +----------------+----------------+--------------------+
  |     <=256      |       1        | 0..255             |
  +----------------+----------------+--------------------+
  |    <=65535     |       2        | 0..65535           |
  +----------------+----------------+--------------------+
  |  <=4294967295  |       4        | 0..4294967295      |
  +----------------+----------------+--------------------+

If matte is true, each colormap index is immediately followed by an
equally-sized alpha value. The alpha value represents the degree of pixel
opacity (zero is totally transparent).

The image data in a MIFF file may be uncompressed, runlength encoded, Zip
compressed, or BZip compressed. The compression keyword in the header
defines how the image data is compressed. Uncompressed pixels are stored
one scanline at a time in row order. Runlength encoded compression counts
runs of identical adjacent pixels and stores the pixels followed by a
length byte (the number of identical pixels minus 1). Zip and BZip
compression compresses each row of an image and precedes the compressed
row with the length of compressed pixel bytes as a word in most
significant byte first order.

MIFF files may contain more than one image. Simply concatenate each
individual image (composed of a header and image data) into one file.

Authors
=======

John Cristy, magick-users@imagemagick.org ImageMagick Studio LLC.

Maintained since 2002 by Bob Friesenhahn, GraphicsMagick Group.

-------------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2015
