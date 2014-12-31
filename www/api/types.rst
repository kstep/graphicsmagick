.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

====================
GraphicsMagick Types
====================

.. contents::
  :local:


AffineMatrix
============

AffineMatrix defines a 2D affine matrix transform.

::

  typedef struct _AffineMatrix
  {
    double
      sx,
      rx,
      ry,
      sy,
      tx,
      ty;
  } AffineMatrix;

BlobInfo
========

BlobInfo is an opaque pointer reference to the internal structure of an
I/O blob handle.

Cache
=====

::

  typedef void
    *Cache;

ChannelType
===========

::

  typedef enum
  {
    UndefinedChannel,
    RedChannel,     /* RGB Red channel */
    CyanChannel,    /* CMYK Cyan channel */
    GreenChannel,   /* RGB Green channel */
    MagentaChannel, /* CMYK Magenta channel */
    BlueChannel,    /* RGB Blue channel */
    YellowChannel,  /* CMYK Yellow channel */
    OpacityChannel, /* Opacity channel */
    BlackChannel,   /* CMYK Black (K) channel */
    MatteChannel,   /* Same as Opacity channel (deprecated) */
    AllChannels,    /* Color channels */
    GrayChannel     /* Color channels represent an intensity. */
  } ChannelType;
  
ChromaticityInfo
================

The ChromaticityInfo structure is used to represent chromaticity
(colorspace primary coordinates in xy space) values for images in
GraphicsMagick.

The members of the ChromaticityInfo structure are shown in the following
table:

.. table:: ChromaticityInfo Structure Members

   +-------------------+-----------+-----------------------------------------------------------------+
   |      Member       |   Type    |                           Description                           |
   +-------------------+-----------+-----------------------------------------------------------------+
   |red_primary        |PointInfo  |Chromaticity red primary point (e.g. x=0.64, y=0.33)             |
   +-------------------+-----------+-----------------------------------------------------------------+
   |green_primary      |PointInfo  |Chromaticity green primary point (e.g. x=0.3, y=0.6)             |
   +-------------------+-----------+-----------------------------------------------------------------+
   |blue_primary       |PointInfo  |Chromaticity blue primary point (e.g. x=0.15, y=0.06)            |
   +-------------------+-----------+-----------------------------------------------------------------+
   |white_point        |PointInfo  |Chromaticity white point (e.g. x=0.3127, y=0.329)                |
   +-------------------+-----------+-----------------------------------------------------------------+


ClassType
=========

ClassType enumeration specifies the image storage class.

.. table:: ClassType

   +----------------+--------------------------------------------------------------------------------+
   |  Enumeration   |                                  Description                                   |
   +----------------+--------------------------------------------------------------------------------+
   |UndefinedClass  |Unset value.                                                                    |
   +----------------+--------------------------------------------------------------------------------+
   |DirectClass     |Image is composed of pixels which represent literal color values.               |
   +----------------+--------------------------------------------------------------------------------+
   |PseudoClass     |Image is composed of pixels which specify an index in a color palette.          |
   +----------------+--------------------------------------------------------------------------------+


ClipPathUnits
=============

::

  typedef enum
  {
    UserSpace,
    UserSpaceOnUse,
    ObjectBoundingBox
  } ClipPathUnits;


ColorPacket
===========

::

  typedef struct _ColorPacket
  {
    PixelPacket
      pixel;
  
    unsigned short
      index;
  
    unsigned long
      count;
  } ColorPacket;


ColorspaceType
==============

The ColorspaceType enumeration is used to specify the colorspace that
quantization (color reduction and mapping) is done under or to specify
the colorspace when encoding an output image. Colorspaces are ways of
describing colors to fit the requirements of a particular application
(e.g. Television, offset printing, color monitors). Color reduction, by
default, takes place in the RGBColorspace. Empirical evidence suggests
that distances in color spaces such as YUVColorspace or YIQColorspace
correspond to perceptual color differences more closely han do distances
in RGB space. These color spaces may give better results when color
reducing an image. Refer to quantize for more details.

When encoding an output image, the colorspaces RGBColorspace,
CMYKColorspace, and GRAYColorspace may be specified. The CMYKColorspace
option is only applicable when writing TIFF, JPEG, and Adobe Photoshop
bitmap (PSD) files.
 

.. table:: ColorspaceType

   ========================= ======================================================
        Enumeration                              Description
   ========================= ======================================================
   UndefinedColorspace       Unset value.
   RGBColorspace             Red, Green, Blue colorspace.
   GRAYColorspace            Similar to Luma (Y) according to ITU-R 601
   TransparentColorspace     RGB which preserves the matte while quantizing colors.
   OHTAColorspace
   XYZColorspace             CIE XYZ
   YCCColorspace             Kodak PhotoCD PhotoYCC
   YIQColorspace
   YPbPrColorspace
   YUVColorspace             YUV colorspace as used for computer video.
   CMYKColorspace            Cyan, Magenta, Yellow, Black colorspace.
   sRGBColorspace            Kodak PhotoCD sRGB
   HSLColorspace             Hue, saturation, luminosity
   HWBColorspace             Hue, whiteness, blackness
   LABColorspace             ITU LAB
   CineonLogRGBColorspace    RGB data with Cineon Log scaling, 2.048 density range
   Rec601LumaColorspace      Luma (Y) according to ITU-R 601
   Rec601YCbCrColorspace     YCbCr according to ITU-R 601
   Rec709LumaColorspace      Luma (Y) according to ITU-R 709
   Rec709YCbCrColorspace     YCbCr according to ITU-R 709
   ========================= ======================================================

ComplianceType
==============

::

  typedef enum
  {
    UndefinedCompliance = 0x0000,
    NoCompliance = 0x0000,
    SVGCompliance = 0x0001,
    X11Compliance = 0x0002,
    XPMCompliance = 0x0004,
    AllCompliance = 0xffff
  } ComplianceType;


CompositeOperator
=================

CompositeOperator is used to select the image composition algorithm used
to compose a composite image with an image. By default, each of the
composite image pixels are replaced by the corresponding image tile
pixel. Specify CompositeOperator to select a different algorithm.
 
The image compositor requires a matte, or alpha channel in the image for
some operations. This extra channel usually defines a mask which
represents a sort of a cookie-cutter for the image. This is the case when
matte is 255 (full coverage) for pixels inside the shape, zero outside,
and between zero and 255 on the boundary. For certain operations, if
image does not have a matte channel, it is initialized with 0 for any
pixel matching in color to pixel location (0,0), otherwise 255 (to work
properly borderWidth must be 0).

.. table:: CompositeOperator

   ======================  ==========================================================================
        Enumeration                                       Description                                
   ======================  ==========================================================================
   UndefinedCompositeOp    Unset value.
   OverCompositeOp         The result is the union of the the two image shapes with the composite
                           image obscuring image in the region of overlap.
   InCompositeOp           The result is a simply composite image cut by the shape of image. None of
                           the image data of image is included in the result.
   OutCompositeOp          The resulting image is composite image with the shape of image cut out.
   AtopCompositeOp         The result is the same shape as image image, with composite image
                           obscuring image there the image shapes overlap. Note that this differs
                           from OverCompositeOp because the portion of composite image outside of
                           image's shape does not appear in the result.
   XorCompositeOp          The result is the image data from both composite image and image that is
                           outside the overlap region. The overlap region will be blank.
   PlusCompositeOp         The result is just the sum of the  image data. Output values are cropped
                           to 255 (no overflow). This operation is independent of the matte channels.
   MinusCompositeOp        The result of composite image - image, with overflow cropped to zero. The
                           matte chanel is ignored (set to 255, full coverage).
   AddCompositeOp          The result of composite image + image, with overflow wrapping around (mod
                           256).                                                                     
   SubtractCompositeOp     The result of composite image - image, with underflow wrapping around (mod
                           256). The add and subtract operators can be used to perform reversible
                           transformations.
   DifferenceCompositeOp   The result of abs(composite image - image). This is useful for comparing
                           two very similar images.
   BumpmapCompositeOp      The result image shaded by composite image.
   CopyCompositeOp         The resulting image is image replaced with composite image. Here the matte
                           information is ignored.
   CopyRedCompositeOp      The resulting image is the red layer in image replaced with the red layer
                           in composite image. The other layers are copied untouched.
   CopyGreenCompositeOp    The resulting image is the green layer in image replaced with the green
                           layer in composite image. The other layers are copied untouched.
   CopyBlueCompositeOp     The resulting image is the blue layer in image replaced with the blue
                           layer in composite image. The other layers are copied untouched.
   CopyOpacityCompositeOp  The resulting image is the matte layer in image replaced with the matte
                           layer in composite image. The other layers are copied untouched.
   ClearCompositeOp        Pixels in the region are set to Transparent.
   DissolveCompositeOp
   DisplaceCompositeOp
   ModulateCompositeOp     Modulate brightness in HSL space.
   ThresholdCompositeOp
   NoCompositeOp           Do nothing at all.
   DarkenCompositeOp
   LightenCompositeOp
   HueCompositeOp          Copy Hue channel (from HSL colorspace).
   SaturateCompositeOp     Copy Saturation channel (from HSL colorspace).
   ColorizeCompositeOp     Copy Hue and Saturation channels (from HSL colorspace).
   LuminizeCompositeOp     Copy Brightness channel (from HSL colorspace).
   ScreenCompositeOp       [Not yet implemented]
   OverlayCompositeOp      [Not yet implemented]
   CopyCyanCompositeOp	   Copy the Cyan channel.
   CopyMagentaCompositeOp  Copy the Magenta channel.
   CopyYellowCompositeOp   Copy the Yellow channel.
   CopyBlackCompositeOp    Copy the Black channel.
   DivideCompositeOp
   ======================  ==========================================================================

CompressionType
===============

CompressionType is used to express the desired compression type when
encoding an image. Be aware that most image types only support a sub-set
of the available compression types. If the compression type specified is
incompatable with the image, GraphicsMagick selects a compression type
compatable with the image type.
 

.. table:: CompressionType

   +---------------------------+---------------------------------------------------------------------+
   |        Enumeration        |                             Description                             |
   +---------------------------+---------------------------------------------------------------------+
   |UndefinedCompression       |Unset value.                                                         |
   +---------------------------+---------------------------------------------------------------------+
   |NoCompression              |No compression                                                       |
   +---------------------------+---------------------------------------------------------------------+
   |BZipCompression            |BZip (Burrows-Wheeler block-sorting text compression algorithm and   |
   |                           |Huffman coding)  as used by bzip2 utilities                          |
   +---------------------------+---------------------------------------------------------------------+
   |FaxCompression             |CCITT Group 3 FAX compression                                        |
   +---------------------------+---------------------------------------------------------------------+
   |Group4Compression          |CCITT Group 4 FAX compression (used only for TIFF)                   |
   +---------------------------+---------------------------------------------------------------------+
   |JPEGCompression            |JPEG compression                                                     |
   +---------------------------+---------------------------------------------------------------------+
   |LosslessJPEGCompression    |Lossless JPEG compression                                            |
   +---------------------------+---------------------------------------------------------------------+
   |LZWCompression             |Lempel-Ziv-Welch (LZW) compression (caution, patented by Unisys)     |
   +---------------------------+---------------------------------------------------------------------+
   |RLECompression             |Run-Length encoded (RLE) compression                                 |
   +---------------------------+---------------------------------------------------------------------+
   |ZipCompression             |Lempel-Ziv compression (LZ77) as used in PKZIP and GNU gzip.         |
   +---------------------------+---------------------------------------------------------------------+

DecorationType
==============

::

  typedef enum
  {
    NoDecoration,
    UnderlineDecoration,
    OverlineDecoration,
    LineThroughDecoration
  } DecorationType;



DrawContext
===========

::

  typedef struct _DrawContext *DrawContext;


DrawInfo
========

The DrawInfo structure is used to support annotating an image using
drawing commands.
 

.. table:: Methods Supporting DrawInfo

   +----------------------+--------------------------------------------------------------------------+
   |        Method        |                               Description                                |
   +----------------------+--------------------------------------------------------------------------+
   |GetDrawInfo()         |Allocate new structure with defaults set.                                 |
   +----------------------+--------------------------------------------------------------------------+
   |CloneDrawInfo()       |Copy existing structure, allocating new structure in the process.         |
   +----------------------+--------------------------------------------------------------------------+
   |DestroyDrawInfo()     |Deallocate structure, including any members.                              |
   +----------------------+--------------------------------------------------------------------------+
   |DrawImage()           |Render primitives to image.                                               |
   +----------------------+--------------------------------------------------------------------------+

The members of the DrawInfo structure are shown in the following table.
The structure is initialized to reasonable defaults by first initializing
the equivalent members of ImageInfo, and then initializing the entire
structure using GetDrawInfo().
 

.. table:: DrawInfo Structure Members Supporting DrawImage()

   +----------------+--------------+--------------------------------------------------------------------+
   |     Member     |     Type     |                            Description                             |
   +----------------+--------------+--------------------------------------------------------------------+
   |affine          |AffineInfo    |Coordinate transformation (rotation, scaling, and translation).     |
   +----------------+--------------+--------------------------------------------------------------------+
   |border_color    |PixelPacket   |Border color                                                        |
   +----------------+--------------+--------------------------------------------------------------------+
   |box             |PixelPacket   |Text solid background color.                                        |
   +----------------+--------------+--------------------------------------------------------------------+
   |decorate        |DecorationType|Text decoration type.                                               |
   +----------------+--------------+--------------------------------------------------------------------+
   |density         |char *        |Text rendering density in DPI (effects scaling font according to    |
   |                |              |pointsize). E.g. "72x72"                                            |
   +----------------+--------------+--------------------------------------------------------------------+
   |fill            |PixelPacket   |Object internal fill (within outline) color.                        |
   +----------------+--------------+--------------------------------------------------------------------+
   |font            |char *        |Font to use when rendering text.                                    |
   +----------------+--------------+--------------------------------------------------------------------+
   |gravity         |GravityType   |Text placement preference (e.g. NorthWestGravity).                  |
   +----------------+--------------+--------------------------------------------------------------------+
   |linewidth       |double        |Stroke (outline) drawing width in pixels.                           |
   +----------------+--------------+--------------------------------------------------------------------+
   |pointsize       |double        |Font size (also see density).                                       |
   +----------------+--------------+--------------------------------------------------------------------+
   |                |              |Space or new-line delimited list of text drawing primitives (e.g    |
   |primitive       |char *        |"text 100,100 Cockatoo"). See the table Drawing Primitives for the  |
   |                |              |available drawing primitives.                                       |
   +----------------+--------------+--------------------------------------------------------------------+
   |stroke          |PixelPacket   |Object stroke (outline) color.                                      |
   +----------------+--------------+--------------------------------------------------------------------+
   |stroke_antialias|unsigned int  |Set to True (non-zero) to obtain anti-aliased stroke rendering.     |
   +----------------+--------------+--------------------------------------------------------------------+
   |text_antialias  |unsigned int  |Set to True (non-zero) to obtain anti-aliased text rendering.       |
   +----------------+--------------+--------------------------------------------------------------------+
   |tile            |Image *       |Image texture to draw with. Use an image containing a single color  |
   |                |              |(e.g. a 1x1 image) to draw in a solid color.                        |
   +----------------+--------------+--------------------------------------------------------------------+

Drawing Primitives

The drawing primitives shown in the following table may be supplied as a
space or new-line delimited list to the primitive member. Primitives
which set drawing options effect the results from subsequent drawing
operations. See the 'push graphic-context' and 'pop graphic-context'
primitives for a way to control the propagation of drawing options.
 

.. table:: Drawing Primitives

   +----------------+----------------------------+-------------------------------------------------------+
   |   Primitive    |        Arguments           |                        Purpose                        |
   +----------------+----------------------------+-------------------------------------------------------+
   |                |                            |Apply coordinate transformations to support scaling    |
   |affine          |sx,rx,ry,sy,tx,ty           |(s), rotation (r), and translation (t). Angles are     |
   |                |                            |specified in radians. Equivalent to SVG matrix command |
   |                |                            |which supplies a transformation matrix.                |
   +----------------+----------------------------+-------------------------------------------------------+
   |angle           |angle                       |Specify object drawing angle.                          |
   +----------------+----------------------------+-------------------------------------------------------+
   |arc             |startX,startY endX,endY     |Draw an arc.                                           |
   |                |startDegrees,endDegrees     |                                                       |
   +----------------+----------------------------+-------------------------------------------------------+
   |Bezier          |x1,y1, x2,y2, x3,y3, ...,   |Draw a Bezier curve.                                   |
   |                |xN,yN                       |                                                       |
   +----------------+----------------------------+-------------------------------------------------------+
   |circle          |originX,originY             |Draw a circle.                                         |
   |                |perimX,perimY               |                                                       |
   +----------------+----------------------------+-------------------------------------------------------+
   |                |x,y (point|replace|         |Set color in image according to specified colorization |
   |color           |floodfill|filltoborder|     |rule.                                                  |
   |                |reset)                      |                                                       |
   +----------------+----------------------------+-------------------------------------------------------+
   |decorate        |(none|underline|overline|   |Specify text decoration.                               |
   |                |line-through)               |                                                       |
   +----------------+----------------------------+-------------------------------------------------------+
   |                |originX,originY             |                                                       |
   |ellipse         |width,height                |Draw an ellipse.                                       |
   |                |arcStart,arcEnd             |                                                       |
   +----------------+----------------------------+-------------------------------------------------------+
   |fill            |colorspec                   |Specifiy object filling color.                         |
   +----------------+----------------------------+-------------------------------------------------------+
   |fill-opacity    |opacity                     |Specify object fill opacity.                           |
   +----------------+----------------------------+-------------------------------------------------------+
   |font            |fontname                    |Specify text drawing font.                             |
   +----------------+----------------------------+-------------------------------------------------------+
   |                |(NorthWest,North,NorthEast, |                                                       |
   |gravity         |West,Center,East,           |Specify text positioning gravity.                      |
   |                |SouthWest,South,SouthEast)  |                                                       |
   +----------------+----------------------------+-------------------------------------------------------+
   |                |                            |Composite image at position, scaled to specified width |
   |image           |x,y width,height filename   |and height, and specified filename. If width or height |
   |                |                            |is zero, scaling is not performed.                     |
   +----------------+----------------------------+-------------------------------------------------------+
   |line            |startX,startY endX,endY     |Draw a line.                                           |
   +----------------+----------------------------+-------------------------------------------------------+
   |                |x,y (point|replace|         |Set matte in image according to specified colorization |
   |matte           |floodfill|filltoborder|     |rule.                                                  |
   |                |reset)                      |                                                       |
   +----------------+----------------------------+-------------------------------------------------------+
   |opacity         |fillOpacity strokeOpacity   |Specify drawing fill and stroke opacities.             |
   +----------------+----------------------------+-------------------------------------------------------+
   |path            |'SVG-compatible path        |Draw using SVG-compatible path drawing commands.       |
   |                |arguments'                  |                                                       |
   +----------------+----------------------------+-------------------------------------------------------+
   |point           |x,y                         |Set point to fill color.                               |
   +----------------+----------------------------+-------------------------------------------------------+
   |pointsize       |pointsize                   |Specify text drawing pointsize (scaled to density).    |
   +----------------+----------------------------+-------------------------------------------------------+
   |polygon         |x1,y1, x2,y2, x3,y3, ...,   |Draw a polygon.                                        |
   |                |xN,yN                       |                                                       |
   +----------------+----------------------------+-------------------------------------------------------+
   |polyline        |x1,y1, x2,y2, x3,y3, ...,   |Draw a polyline.                                       |
   |                |xN,yN                       |                                                       |
   +----------------+----------------------------+-------------------------------------------------------+
   |                |                            |Remove options set since previous "push                |
   |pop             |graphic-context             |graphic-context" command. Options revert to those in   |
   |                |                            |effect prior to pushing the graphic context.           |
   +----------------+----------------------------+-------------------------------------------------------+
   |push            |graphic-context             |Specify new graphic context.                           |
   +----------------+----------------------------+-------------------------------------------------------+
   |rect            |upperLeftX,upperLeftY       |Draw a rectangle.                                      |
   |                |lowerRightX,lowerRightY     |                                                       |
   +----------------+----------------------------+-------------------------------------------------------+
   |                |                            |Specify coordiante space rotation. Subsequent objects  |
   |rotate          |angle                       |are drawn with coordate space rotated by specified     |
   |                |                            |angle.                                                 |
   +----------------+----------------------------+-------------------------------------------------------+
   |                |centerX,centerY             |                                                       |
   |roundrectangle  |width,hight                 |Draw a rectangle with rounded corners.                 |
   |                |cornerWidth,cornerHeight    |                                                       |
   +----------------+----------------------------+-------------------------------------------------------+
   |stroke          |colorspec                   |Specify object stroke (outline) color.                 |
   +----------------+----------------------------+-------------------------------------------------------+
   |stroke-antialias|stroke_antialias (0 or 1)   |Specify if stroke should be antialiased or not.        |
   +----------------+----------------------------+-------------------------------------------------------+
   |stroke-dash     |value                       |Specify pattern to be used when drawing stroke.        |
   +----------------+----------------------------+-------------------------------------------------------+
   |stroke-opacity  |opacity                     |Specify opacity of stroke drawing color.               |
   +----------------+----------------------------+-------------------------------------------------------+
   |stroke-width    |linewidth                   |Specify stroke (outline) width in pixels.              |
   +----------------+----------------------------+-------------------------------------------------------+
   |text            |x,y "some text"             |Draw text at position.                                 |
   +----------------+----------------------------+-------------------------------------------------------+
   |text-antialias  |text_antialias (0 or 1)     |Specify if rendered text is to be antialiased (blend   |
   |                |                            |edges).                                                |
   +----------------+----------------------------+-------------------------------------------------------+
   |scale           |x,y                         |Specify scaling to be applied to coordintate space for |
   |                |                            |subsequent drawing commands.                           |
   +----------------+----------------------------+-------------------------------------------------------+
   |translate       |x,y                         |Specify center of coordinate space to use for          |
   |                |                            |subsequent drawing commands.                           |
   +----------------+----------------------------+-------------------------------------------------------+

EndianType
==========

::

  typedef enum
  {
    UndefinedEndian,
    LSBEndian,            /* "little" endian */
    MSBEndian,            /* "big" endian */
    NativeEndian          /* native endian */
  } EndianType;

ErrorHandler
============

::

  typedef void
    (*ErrorHandler)(const ExceptionType,const char *,const char *);


ExceptionInfo
=============

::

  typedef struct _ExceptionInfo
  {
    char
      *reason,
      *description;
  
    ExceptionType
      severity;
  
    unsigned long
      signature;
  } ExceptionInfo;



ExceptionType
=============

::

  typedef enum
  {
    UndefinedException,
    WarningException = 300,
    ResourceLimitWarning = 300,
    TypeWarning = 305,
    OptionWarning = 310,
    DelegateWarning = 315,
    MissingDelegateWarning = 320,
    CorruptImageWarning = 325,
    FileOpenWarning = 330,
    BlobWarning = 335,
    StreamWarning = 340,
    CacheWarning = 345,
    CoderWarning = 350,
    ModuleWarning = 355,
    DrawWarning = 360,
    ImageWarning = 365,
    XServerWarning = 380,
    MonitorWarning = 385,
    RegistryWarning = 390,
    ConfigureWarning = 395,
    ErrorException = 400,
    ResourceLimitError = 400,
    TypeError = 405,
    OptionError = 410,
    DelegateError = 415,
    MissingDelegateError = 420,
    CorruptImageError = 425,
    FileOpenError = 430,
    BlobError = 435,
    StreamError = 440,
    CacheError = 445,
    CoderError = 450,
    ModuleError = 455,
    DrawError = 460,
    ImageError = 465,
    XServerError = 480,
    MonitorError = 485,
    RegistryError = 490,
    ConfigureError = 495,
    FatalErrorException = 700,
    ResourceLimitFatalError = 700,
    TypeFatalError = 705,
    OptionFatalError = 710,
    DelegateFatalError = 715,
    MissingDelegateFatalError = 720,
    CorruptImageFatalError = 725,
    FileOpenFatalError = 730,
    BlobFatalError = 735,
    StreamFatalError = 740,
    CacheFatalError = 745,
    CoderFatalError = 750,
    ModuleFatalError = 755,
    DrawFatalError = 760,
    ImageFatalError = 765,
    XServerFatalError = 780,
    MonitorFatalError = 785,
    RegistryFatalError = 790,
    ConfigureFatalError = 795
  } ExceptionType;


FillRule
========

::

  typedef enum
  {
    UndefinedRule,
    EvenOddRule,
    NonZeroRule
  } FillRule;


FilterTypes
===========

FilterTypes is used to adjust the filter algorithm used when resizing
images. Different filters experience varying degrees of success with
various images and can take significantly different amounts of processing
time. GraphicsMagick uses the LanczosFilter by default since this filter
has been shown to provide the best results for most images in a
reasonable amount of time. Other filter types (e.g. TriangleFilter) may
execute much faster but may show artifacts when the image is re-sized or
around diagonal lines. The only way to be sure is to test the filter with
sample images.

.. table:: FilterTypes

   +----------------------------------------------+--------------------------------------------------+
   |                 Enumeration                  |                   Description                    |
   +----------------------------------------------+--------------------------------------------------+
   |UndefinedFilter                               |Unset value.                                      |
   +----------------------------------------------+--------------------------------------------------+
   |PointFilter                                   |Point Filter                                      |
   +----------------------------------------------+--------------------------------------------------+
   |BoxFilter                                     |Box Filter                                        |
   +----------------------------------------------+--------------------------------------------------+
   |TriangleFilter                                |Triangle Filter                                   |
   +----------------------------------------------+--------------------------------------------------+
   |HermiteFilter                                 |Hermite Filter                                    |
   +----------------------------------------------+--------------------------------------------------+
   |HanningFilter                                 |Hanning Filter                                    |
   +----------------------------------------------+--------------------------------------------------+
   |HammingFilter                                 |Hamming Filter                                    |
   +----------------------------------------------+--------------------------------------------------+
   |BlackmanFilter                                |Blackman Filter                                   |
   +----------------------------------------------+--------------------------------------------------+
   |GaussianFilter                                |Gaussian Filter                                   |
   +----------------------------------------------+--------------------------------------------------+
   |QuadraticFilter                               |Quadratic Filter                                  |
   +----------------------------------------------+--------------------------------------------------+
   |CubicFilter                                   |Cubic Filter                                      |
   +----------------------------------------------+--------------------------------------------------+
   |CatromFilter                                  |Catrom Filter                                     |
   +----------------------------------------------+--------------------------------------------------+
   |MitchellFilter                                |Mitchell Filter                                   |
   +----------------------------------------------+--------------------------------------------------+
   |LanczosFilter                                 |Lanczos Filter                                    |
   +----------------------------------------------+--------------------------------------------------+
   |BesselFilter                                  |Bessel Filter                                     |
   +----------------------------------------------+--------------------------------------------------+
   |SincFilter                                    |Sinc Filter                                       |
   +----------------------------------------------+--------------------------------------------------+

FrameInfo
=========

::

  typedef struct _FrameInfo
  {
    unsigned long
      width,
      height;
  
    long
      x,
      y,
      inner_bevel,
      outer_bevel;
  } FrameInfo;


GravityType
===========

GravityType specifies positioning of an object (e.g. text, image) within
a bounding region (e.g. an image). Gravity provides a convenient way to
locate objects irrespective of the size of the bounding region, in other
words, you don't need to provide absolute coordinates in order to
position an object. A common default for gravity is NorthWestGravity.
 

.. table:: GravityType

   +--------------------------+----------------------------------------------------------------------+
   |       Enumeration        |                             Description                              |
   +--------------------------+----------------------------------------------------------------------+
   |ForgetGravity             |Don't use gravity.                                                    |
   +--------------------------+----------------------------------------------------------------------+
   |NorthWestGravity          |Position object at top-left of region.                                |
   +--------------------------+----------------------------------------------------------------------+
   |NorthGravity              |Postiion object at top-center of region                               |
   +--------------------------+----------------------------------------------------------------------+
   |NorthEastGravity          |Position object at top-right of region                                |
   +--------------------------+----------------------------------------------------------------------+
   |WestGravity               |Position object at left-center of region                              |
   +--------------------------+----------------------------------------------------------------------+
   |CenterGravity             |Position object at center of region                                   |
   +--------------------------+----------------------------------------------------------------------+
   |EastGravity               |Position object at right-center of region                             |
   +--------------------------+----------------------------------------------------------------------+
   |SouthWestGravity          |Position object at left-bottom of region                              |
   +--------------------------+----------------------------------------------------------------------+
   |SouthGravity              |Position object at bottom-center of region                            |
   +--------------------------+----------------------------------------------------------------------+
   |SouthEastGravity          |Position object at bottom-right of region                             |
   +--------------------------+----------------------------------------------------------------------+

Image
=====

The Image structure represents an GraphicsMagick image. It is initially
allocated by AllocateImage() and deallocated by DestroyImage(). The
functions ReadImage(), ReadImages(), BlobToImage() and CreateImage()
return a new image. Use CloneImage() to copy an image. An image consists
of a structure containing image attributes as well as the image pixels.

The image pixels are represented by the structure PixelPacket and are
cached in-memory, or on disk, depending on the cache threshold setting.
This cache is known as the "pixel cache". Pixels in the cache may not be
edited directly. They must first be made visible from the cache via a
pixel view. A pixel view is a rectangular view of the pixels as defined
by a starting coordinate, and a number of rows and columns. When
considering the varying abilities of multiple platforms, the most
reliably efficient pixel view is comprized of part, or all, of one image
row.

There are two means of accessing pixel views. When using the default
view, the pixels are made visible and accessable by using the
GetImagePixels() method which provides access to a specified region of
the image. After the view has been updated, the pixels may be saved back
to the cache in their original positions via SyncImagePixels(). In order
to create an image with new contents, or to blindly overwrite existing
contents, the method SetImagePixels() is used to reserve a pixel view
corresponding to a region in the pixel cache. Once the pixel view has
been updated, it may be written to the cache via SyncImagePixels(). The
function GetIndexes() provides access to the image colormap, represented
as an array of type IndexPacket.

A more flexible interface to the image pixels is via the Cache View
interface. This interface supports multiple pixel cache views (limited by
the amount of available memory), each of which are identified by a handle
(of type ViewInfo). Use OpenCacheView() to obtain a new cache view,
CloseCacheView() to discard a cache view, GetCacheViewPixels() to access
an existing pixel region, SetCacheView() to define a new pixel region,
and SyncCacheViewPixels() to save the updated pixel region. The function
GetCacheViewIndexes() provides access to the colormap indexes associated
with the pixel view.

When writing encoders and decoders for new image formats, it is
convenient to have a high-level interface available which supports
converting between external pixel representations and GraphicsMagick's
own representation. Pixel components (red, green, blue, opacity, RGB, or
RGBA) may be transferred from a user-supplied buffer into the default
view by using ImportImagePixelArea(), or from an allocated view via
ImportViewPixelArea(). Pixel components may be transferred from the
default view into a user-supplied buffer by using ExportImagePixelArea(),
or from an allocated view via ExportViewPixelArea(). Use of this
high-level interface helps protect image coders from changes to
GraphicsMagick's pixel representation and simplifies the implementation.

The members of the Image structure are shown in the following table:
 

.. table:: Image Structure Members

   +--------------------------------+------------------+----------------------------------------------------+
   |             Member             |       Type       |                    Description                     |
   +--------------------------------+------------------+----------------------------------------------------+
   | background_color               | PixelPacket      | Image background color                             |
   +--------------------------------+------------------+----------------------------------------------------+
   | blur                           | double           | Blur factor to apply to the image when zooming     |
   +--------------------------------+------------------+----------------------------------------------------+
   | border_color                   | PixelPacket      | Image border color                                 |
   +--------------------------------+------------------+----------------------------------------------------+
   | chromaticity                   | ChromaticityInfo | Red, green, blue, and white-point chromaticity     |
   |                                |                  | values.                                            |
   +--------------------------------+------------------+----------------------------------------------------+
   | colormap                       | PixelPacket *    | PseudoColor palette array.                         |
   +--------------------------------+------------------+----------------------------------------------------+
   | colors                         | unsigned int     | The desired number of colors. Used by              |
   |                                |                  | QuantizeImage().                                   |
   +--------------------------------+------------------+----------------------------------------------------+
   |                                |                  | Image pixel interpretation.If the colorspace is    |
   |                                |                  | RGB the pixels are red, green, blue. If matte is   |
   | colorspace                     | ColorspaceType   | true, then red, green, blue, and index. If it is   |
   |                                |                  | CMYK, the pixels are cyan, yellow, magenta, black. |
   |                                |                  | Otherwise the colorspace is ignored.               |
   +--------------------------------+------------------+----------------------------------------------------+
   | columns                        | unsigned int     | Image width                                        |
   +--------------------------------+------------------+----------------------------------------------------+
   | compression                    | CompressionType  | Image compresion type. The default is the          |
   |                                |                  | compression type of the specified image file.      |
   +--------------------------------+------------------+----------------------------------------------------+
   |                                |                  | Time in 1/100ths of a second (0 to 65535) which    |
   |                                |                  | must expire before displaying the next image in an |
   | delay                          | unsigned int     | animated sequence. This option is useful for       |
   |                                |                  | regulating the animation of a sequence of GIF      |
   |                                |                  | images within Netscape.                            |
   +--------------------------------+------------------+----------------------------------------------------+
   | depth                          | unsigned int     | Image depth.  Number of encoding bits per sample.  |
   |                                |                  | Usually 8 or 16, but sometimes 10 or 12.           |
   +--------------------------------+------------------+----------------------------------------------------+
   |                                |                  | Tile names from within an image montage. Only      |
   | directory                      | char *           | valid after calling MontageImages() or reading a   |
   |                                |                  | MIFF file which contains a directory.              |
   +--------------------------------+------------------+----------------------------------------------------+
   |                                |                  | GIF disposal method. This option is used to        |
   | dispose                        | unsigned int     | control how successive frames are rendered (how    |
   |                                |                  | the preceding frame is disposed of) when creating  |
   |                                |                  | a GIF animation.                                   |
   +--------------------------------+------------------+----------------------------------------------------+
   | exception                      | ExceptionInfo    | Record of any error which occurred when updating   |
   |                                |                  | image.                                             |
   +--------------------------------+------------------+----------------------------------------------------+
   | filename                       | char             | Image file name to read or write.                  |
   |                                | [MaxTextExtent]  |                                                    |
   +--------------------------------+------------------+----------------------------------------------------+
   |                                |                  | Filter to use when resizing image. The reduction   |
   |                                |                  | filter employed has a significant effect on the    |
   | filter                         | FilterTypes      | time required to resize an image and the resulting |
   |                                |                  | quality. The default filter is Lanczos which has   |
   |                                |                  | been shown to produce high quality results when    |
   |                                |                  | reducing most images.                              |
   +--------------------------------+------------------+----------------------------------------------------+
   |                                |                  | Colors within this distance are considered equal.  |
   |                                |                  | A number of algorithms search for a target color.  |
   | fuzz                           | int              | By default the color must be exact. Use this       |
   |                                |                  | option to match colors that are close to the       |
   |                                |                  | target color in RGB space.                         |
   +--------------------------------+------------------+----------------------------------------------------+
   |                                |                  | Gamma level of the image. The same color image     |
   |                                |                  | displayed on two different workstations may look   |
   | gamma                          | double           | different due to differences in the display        |
   |                                |                  | monitor. Use gamma correction to adjust for this   |
   |                                |                  | color difference.                                  |
   +--------------------------------+------------------+----------------------------------------------------+
   | geometry                       | char *           | Preferred size of the image when encoding.         |
   +--------------------------------+------------------+----------------------------------------------------+
   |                                |                  | The type of interlacing scheme (default            |
   |                                |                  | NoInterlace). This option is used to specify the   |
   |                                |                  | type of interlacing scheme for raw image formats   |
   |                                |                  | such as RGB or YUV. NoInterlace means do not       |
   |                                |                  | interlace, LineInterlace uses scanline             |
   | interlace                      | InterlaceType    | interlacing, and PlaneInterlace uses plane         |
   |                                |                  | interlacing. PartitionInterlace is like            |
   |                                |                  | PlaneInterlace except the different planes are     |
   |                                |                  | saved to individual files (e.g. image.R, image.G,  |
   |                                |                  | and image.B). Use LineInterlace or PlaneInterlace  |
   |                                |                  | to create an interlaced GIF or progressive JPEG    |
   |                                |                  | image.                                             |
   +--------------------------------+------------------+----------------------------------------------------+
   | iterations                     | unsigned int     | Number of iterations to loop an animation (e.g.    |
   |                                |                  | Netscape loop extension) for.                      |
   +--------------------------------+------------------+----------------------------------------------------+
   | magick                         | char             | Image encoding format (e.g. "GIF").                |
   |                                | [MaxTextExtent]  |                                                    |
   +--------------------------------+------------------+----------------------------------------------------+
   | magick_columns                 | unsigned int     | Base image width (before transformations)          |
   +--------------------------------+------------------+----------------------------------------------------+
   | magick_filename                | char             | Base image filename (before transformations)       |
   |                                | [MaxTextExtent]  |                                                    |
   +--------------------------------+------------------+----------------------------------------------------+
   | magick_rows                    | unsigned int     | Base image height (before transformations)         |
   +--------------------------------+------------------+----------------------------------------------------+
   | matte                          | unsigned int     | If non-zero, then the index member of pixels       |
   |                                |                  | represents the alpha channel.                      |
   +--------------------------------+------------------+----------------------------------------------------+
   | matte_color                    | PixelPacket      | Image matte (transparent) color                    |
   +--------------------------------+------------------+----------------------------------------------------+
   | montage                        | char *           | Tile size and offset within an image montage. Only |
   |                                |                  | valid for montage images.                          |
   +--------------------------------+------------------+----------------------------------------------------+
   | next                           | struct _Image *  | Next image frame in sequence                       |
   +--------------------------------+------------------+----------------------------------------------------+
   | offset                         | int              | Number of initial bytes to skip over when reading  |
   |                                |                  | raw image.                                         |
   +--------------------------------+------------------+----------------------------------------------------+
   | orientation                    | OrientationType  | Orientation of the image. Specifies scanline       |
   |                                |                  | orientation and starting coordinate of image.      |
   +--------------------------------+------------------+----------------------------------------------------+
   | page                           | RectangleInfo    | Equivalent size of Postscript page.                |
   +--------------------------------+------------------+----------------------------------------------------+
   | previous                       | struct _Image *  | Previous image frame in sequence.                  |
   +--------------------------------+------------------+----------------------------------------------------+
   | rendering_intent               | RenderingIntent  | The type of rendering intent.                      |
   +--------------------------------+------------------+----------------------------------------------------+
   | rows                           | unsigned int     | Image height                                       |
   +--------------------------------+------------------+----------------------------------------------------+
   | scene                          | unsigned int     | Image frame scene number.                          |
   +--------------------------------+------------------+----------------------------------------------------+
   |                                |                  | Image storage class. If DirectClass then the image |
   | storage_class                  | ClassType        | packets contain valid RGB or CMYK colors. If       |
   |                                |                  | PseudoClass then the image has a colormap          |
   |                                |                  | referenced by pixel's index member.                |
   +--------------------------------+------------------+----------------------------------------------------+
   |                                |                  | Describes a tile within an image.  For example, if |
   | tile_info                      | RectangleInfo    | your images is 640x480 you may only want 320x256   |
   |                                |                  | with an offset of +128+64. It is used for raw      |
   |                                |                  | formats such as RGB and CMYK as well as for TIFF.  |
   +--------------------------------+------------------+----------------------------------------------------+
   | timer                          | TimerInfo        | Support for measuring actual (user + system) and   |
   |                                |                  | elapsed execution time.                            |
   +--------------------------------+------------------+----------------------------------------------------+
   |                                |                  | The number of colors in the image after            |
   | total_colors                   | unsigned long    | QuantizeImage(), or QuantizeImages() if the        |
   |                                |                  | verbose flag was set before the call. Calculated   |
   |                                |                  | by GetNumberColors().                              |
   +--------------------------------+------------------+----------------------------------------------------+
   | units                          | ResolutionType   | Units of image resolution                          |
   +--------------------------------+------------------+----------------------------------------------------+
   | x_resolution                   | double           | Horizontal resolution of the image                 |
   +--------------------------------+------------------+----------------------------------------------------+
   | y_resolution                   | double           | Vertical resolution of the image                   |
   +--------------------------------+------------------+----------------------------------------------------+

ImageInfo
=========

The ImageInfo structure is used to supply option information to the
functions AllocateImage(), AnimateImages(), BlobToImage(),
CloneAnnotateInfo(), DisplayImages(), GetAnnotateInfo(), ImageToBlob(),
PingImage(), ReadImage(), ReadImages(), and, WriteImage(). These
functions update information in ImageInfo to reflect attributes of the
current image.

Use CloneImageInfo() to duplicate an existing ImageInfo structure or
allocate a new one. Use DestroyImageInfo() to deallocate memory
associated with an ImageInfo structure. Use GetImageInfo() to initialize
an existing ImageInfo structure. Use SetImageInfo() to set image type
information in the ImageInfo structure based on an existing image.

The members of the ImageInfo structure are shown in the following table:
 

.. table:: ImageInfo Structure Members

   +----------------+---------------+-------------------------------------------------------------------+
   |     Member     |     Type      |                            Description                            |
   +----------------+---------------+-------------------------------------------------------------------+
   |adjoin          |unsigned int   |Join images into a single multi-image file.                        |
   +----------------+---------------+-------------------------------------------------------------------+
   |antialias       |unsigned int   |Control antialiasing of rendered Postscript and Postscript or      |
   |                |               |TrueType fonts. Enabled by default.                                |
   +----------------+---------------+-------------------------------------------------------------------+
   |background_color|PixelPacket    |Image background color.                                            |
   +----------------+---------------+-------------------------------------------------------------------+
   |border_color    |PixelPacket    |Image border color.                                                |
   +----------------+---------------+-------------------------------------------------------------------+
   |                |               |Image pixel interpretation.If the colorspace is RGB the pixels are |
   |colorspace      |ColorspaceType |red, green, blue. If matte is true, then red, green, blue, and     |
   |                |               |index. If it is CMYK, the pixels are cyan, yellow, magenta, black. |
   |                |               |Otherwise the colorspace is ignored.                               |
   +----------------+---------------+-------------------------------------------------------------------+
   |compression     |CompressionType|Image compresion type. The default is the compression type of the  |
   |                |               |specified image file.                                              |
   +----------------+---------------+-------------------------------------------------------------------+
   |                |               |Time in 1/100ths of a second (0 to 65535) which must expire before |
   |delay           |char *         |displaying the next image in an animated sequence. This option is  |
   |                |               |useful for regulating the animation of a sequence of GIF images    |
   |                |               |within Netscape.                                                   |
   +----------------+---------------+-------------------------------------------------------------------+
   |                |               |Vertical and horizontal resolution in pixels of the image. This    |
   |density         |char *         |option specifies an image density when decoding a Postscript or    |
   |                |               |Portable Document page. Often used with page.                      |
   +----------------+---------------+-------------------------------------------------------------------+
   |depth           |unsigned int   |Image depth (8 or 16). QuantumLeap must be defined before a depth  |
   |                |               |of 16 is valid.                                                    |
   +----------------+---------------+-------------------------------------------------------------------+
   |                |               |GIF disposal method. This option is used to control how successive |
   |dispose         |char *         |frames are rendered (how the preceding frame is disposed of) when  |
   |                |               |creating a GIF animation.                                          |
   +----------------+---------------+-------------------------------------------------------------------+
   |                |               |Apply Floyd/Steinberg error diffusion to the image. The basic      |
   |                |               |strategy of dithering is to trade intensity resolution for spatial |
   |dither          |unsigned int   |resolution by averaging the intensities of several neighboring     |
   |                |               |pixels. Images which suffer from severe contouring when reducing   |
   |                |               |colors can be improved with this option. The colors or monochrome  |
   |                |               |option must be set for this option to take effect.                 |
   +----------------+---------------+-------------------------------------------------------------------+
   |                |               |Stdio stream to read image from or write image to. If set,         |
   |file            |FILE *         |ImageMagick will read from or write to the stream rather than      |
   |                |               |opening a file. Used by ReadImage() and WriteImage(). The stream is|
   |                |               |closed when the operation completes.                               |
   +----------------+---------------+-------------------------------------------------------------------+
   |filename        |char           |Image file name to read or write.                                  |
   |                |[MaxTextExtent]|                                                                   |
   +----------------+---------------+-------------------------------------------------------------------+
   |fill            |PixelPacket    |Drawing object fill color.                                         |
   +----------------+---------------+-------------------------------------------------------------------+
   |                |               |Text rendering font. If the font is a fully qualified X server font|
   |font            |char *         |name, the font is obtained from an X server. To use a TrueType     |
   |                |               |font, precede the TrueType filename with an @. Otherwise, specify a|
   |                |               |Postscript font name (e.g. "helvetica").                           |
   +----------------+---------------+-------------------------------------------------------------------+
   |                |               |Colors within this distance are considered equal. A number of      |
   |fuzz            |int            |algorithms search for a target color. By default the color must be |
   |                |               |exact. Use this option to match colors that are close to the target|
   |                |               |color in RGB space.                                                |
   +----------------+---------------+-------------------------------------------------------------------+
   |                |               |The type of interlacing scheme (default NoInterlace). This option  |
   |                |               |is used to specify the type of interlacing scheme for raw image    |
   |                |               |formats such as RGB or YUV. NoInterlace means do not interlace,    |
   |interlace       |InterlaceType  |LineInterlace uses scanline interlacing, and PlaneInterlace uses   |
   |                |               |plane interlacing. PartitionInterlace is like PlaneInterlace except|
   |                |               |the different planes are saved to individual files (e.g. image.R,  |
   |                |               |image.G, and image.B). Use LineInterlace or PlaneInterlace to      |
   |                |               |create an interlaced GIF or progressive JPEG image.                |
   +----------------+---------------+-------------------------------------------------------------------+
   |iterations      |char *         |Number of iterations to loop an animation (e.g. Netscape loop      |
   |                |               |extension) for.                                                    |
   +----------------+---------------+-------------------------------------------------------------------+
   |linewidth       |unsigned int   |Line width for drawing lines, circles, ellipses, etc.              |
   +----------------+---------------+-------------------------------------------------------------------+
   |magick          |char           |Image encoding format (e.g. "GIF").                                |
   |                |[MaxTextExtent]|                                                                   |
   +----------------+---------------+-------------------------------------------------------------------+
   |matte_color     |PixelPacket    |Image matte (transparent) color.                                   |
   +----------------+---------------+-------------------------------------------------------------------+
   |monochrome      |unsigned int   |Transform the image to black and white.                            |
   +----------------+---------------+-------------------------------------------------------------------+
   |page            |char *         |Equivalent size of Postscript page.                                |
   +----------------+---------------+-------------------------------------------------------------------+
   |                |               |Set to True to read enough of the image to determine the image     |
   |ping            |unsigned int   |columns, rows, and filesize. The columns, rows, and size attributes|
   |                |               |are valid after invoking ReadImage() while ping is set. The image  |
   |                |               |data is not valid after calling ReadImage() if ping is set.        |
   +----------------+---------------+-------------------------------------------------------------------+
   |pointsize       |double         |Text rendering font point size.                                    |
   +----------------+---------------+-------------------------------------------------------------------+
   |preview_type    |PreviewType    |Image manipulation preview option. Used by 'display'.              |
   +----------------+---------------+-------------------------------------------------------------------+
   |quality         |unsigned int   |JPEG/MIFF/PNG compression level (default 75).                      |
   +----------------+---------------+-------------------------------------------------------------------+
   |server_name     |char *         |X11 display to display to obtain fonts from, or to capture image   |
   |                |               |from.                                                              |
   +----------------+---------------+-------------------------------------------------------------------+
   |                |               |Width and height of a raw image (an image which does not support   |
   |size            |char *         |width and height information). Size may also be used to affect the |
   |                |               |image size read from a multi-resolution format (e.g. Photo CD,     |
   |                |               |JBIG, or JPEG.                                                     |
   +----------------+---------------+-------------------------------------------------------------------+
   |stroke          |PixelPacket    |Drawing object outline color.                                      |
   +----------------+---------------+-------------------------------------------------------------------+
   |subimage        |unsigned int   |Subimage of an image sequence.                                     |
   +----------------+---------------+-------------------------------------------------------------------+
   |subrange        |unsigned int   |Number of images relative to the base image.                       |
   +----------------+---------------+-------------------------------------------------------------------+
   |texture         |char *         |Image filename to use as background texture.                       |
   +----------------+---------------+-------------------------------------------------------------------+
   |tile            |char *         |Tile name.                                                         |
   +----------------+---------------+-------------------------------------------------------------------+
   |units           |ResolutionType |Units of image resolution.                                         |
   +----------------+---------------+-------------------------------------------------------------------+
   |verbose         |unsigned int   |Print detailed information about the image if True.                |
   +----------------+---------------+-------------------------------------------------------------------+
   |view            |char *         |FlashPix viewing parameters.                                       |
   +----------------+---------------+-------------------------------------------------------------------+

ImageType
=========

ImageType indicates the type classification of the image.

.. table:: ImageType

   +------------------------------+------------------------------------------------------------------+
   |         Enumeration          |                           Description                            |
   +------------------------------+------------------------------------------------------------------+
   |UndefinedType                 |Unset value.                                                      |
   +------------------------------+------------------------------------------------------------------+
   |BilevelType                   |Monochrome image                                                  |
   +------------------------------+------------------------------------------------------------------+
   |GrayscaleType                 |Grayscale image                                                   |
   +------------------------------+------------------------------------------------------------------+
   |PaletteType                   |Indexed color (palette) image                                     |
   +------------------------------+------------------------------------------------------------------+
   |PaletteMatteType              |Indexed color (palette) image with opacity                        |
   +------------------------------+------------------------------------------------------------------+
   |TrueColorType                 |Truecolor image                                                   |
   +------------------------------+------------------------------------------------------------------+
   |TrueColorMatteType            |Truecolor image with opacity                                      |
   +------------------------------+------------------------------------------------------------------+
   |ColorSeparationType           |Cyan/Yellow/Magenta/Black (CYMK) image                            |
   +------------------------------+------------------------------------------------------------------+

IndexPacket
===========

IndexPacket is the type used for a colormap index. An array of type
IndexPacket is used to represent an image in PseudoClass type. Currently
supported IndexPacket underlying types are 'unsigned char' and 'unsigned
short'. The type is selected at build time according to the QuantumDepth
setting.

InterlaceType
=============

InterlaceType specifies the ordering of the red, green, and blue pixel
information in the image. Interlacing is usually used to make image
information available to the user faster by taking advantage of the space
vs time tradeoff. For example, interlacing allows images on the Web to be
recognizable sooner and satellite images to accumulate/render with image
resolution increasing over time.

Use LineInterlace or PlaneInterlace to create an interlaced GIF or
progressive JPEG image.
 
.. table:: InterlaceType

   +------------------+------------------------------------------------------------------------------+
   |   Enumeration    |                                 Description                                  |
   +------------------+------------------------------------------------------------------------------+
   |UndefinedInterlace|Unset value.                                                                  |
   +------------------+------------------------------------------------------------------------------+
   |NoInterlace       |Don't interlace image (RGBRGBRGBRGBRGBRGB...)                                 |
   +------------------+------------------------------------------------------------------------------+
   |LineInterlace     |Use scanline interlacing (RRR...GGG...BBB...RRR...GGG...BBB...)               |
   +------------------+------------------------------------------------------------------------------+
   |PlaneInterlace    |Use plane interlacing (RRRRRR...GGGGGG...BBBBBB...)                           |
   +------------------+------------------------------------------------------------------------------+
   |PartitionInterlace|Similar to plane interlaing except that the different planes are saved to     |
   |                  |individual files (e.g. image.R, image.G, and image.B)                         |
   +------------------+------------------------------------------------------------------------------+

LayerType
=========

LayerType is used as an argument when doing color separations. Use
LayerType when extracting a layer from an image. MatteLayer is useful for
extracting the opacity values from an image.

.. table:: LayerType

   +---------------------------+---------------------------------------------------------------------+
   |        Enumeration        |                             Description                             |
   +---------------------------+---------------------------------------------------------------------+
   |UndefinedLayer             |Unset value.                                                         |
   +---------------------------+---------------------------------------------------------------------+
   |RedLayer                   |Select red layer                                                     |
   +---------------------------+---------------------------------------------------------------------+
   |GreenLayer                 |Select green layer                                                   |
   +---------------------------+---------------------------------------------------------------------+
   |BlueLayer                  |Select blue layer                                                    |
   +---------------------------+---------------------------------------------------------------------+
   |MatteLayer                 |Select matte (opacity values) layer                                  |
   +---------------------------+---------------------------------------------------------------------+

MagickInfo
==========

The MagickInfo structure is used by GraphicsMagick to register support
for an image format. The MagickInfo structure is allocated with default
parameters by calling SetMagickInfo(). Image formats are registered by
calling RegisterMagickInfo() which adds the initial structure to a linked
list (at which point it is owned by the list). A pointer to the structure
describing a format may be obtained by calling GetMagickInfo(). Pass the
argument NULL to obtain the first member of this list. A human-readable
list of registered image formats may be printed to a file descriptor by
calling ListMagickInfo().

Support for formats may be provided as a module which is part of the
GraphicsMagick library, provided by a module which is loaded dynamically
at run-time, or directly by the linked program. Users of GraphicsMagick
will normally want to create a loadable-module, or support encode/decode
of an image format directly from within their program.

Sample Module:

The following shows sample code for a module called "GIF" (gif.c). Note
that the names of the Register and Unregister call-back routines are
calculated at run-time, and therefore must follow the rigid naming scheme
RegisterFORMATImage and UnregisterFORMATImage, respectively, where FORMAT
is the upper-cased name of the module file::

  /* Read image */
  Image *ReadGIFImage(const ImageInfo *image_info)
  {
    [ decode the image ... ]
  }
  
  /* Write image */
  unsigned int WriteGIFImage(const ImageInfo *image_info,Image *image)
  {
    [ encode the image ... ]
  }
  
  /* Module call-back to register support for formats */
  void RegisterGIFImage(void)
  {
    MagickInfo *entry;
    entry=SetMagickInfo("GIF");
    entry->decoder=ReadGIFImage;
    entry->encoder=WriteGIFImage;
    entry->description="CompuServe graphics interchange format";
    entry->module="GIF";
    RegisterMagickInfo(entry);

    entry=SetMagickInfo("GIF87");
    entry->decoder=ReadGIFImage;
    entry->encoder=WriteGIFImage;
    entry->adjoin=False;
    entry->description="CompuServe graphics interchange format (version 87a)";
    entry->module="GIF";
    RegisterMagickInfo(entry);
  }
  
  /* Module call-back to unregister support for formats */
  Export void UnregisterGIFImage(void)
  {
    UnregisterMagickInfo("GIF");
    UnregisterMagickInfo("GIF87");
  }

Sample Application Code

Image format support provided within the user's application does not need
to implement the RegisterFORMATImage and UnregisterFORMATImage call-back
routines. Instead, the application takes responsibility for the
registration itself. An example follows::

  /* Read image */
  Image *ReadGIFImage(const ImageInfo *image_info)
  {
    [ decode the image ... ]
  }
  /* Write image */
  unsigned int WriteGIFImage(const ImageInfo *image_info,Image *image)
  {
    [ encode the image ... ]
  }
  #include <stdio.h>
  int main( void )
  {
    struct MagickInfo* info;
    info = SetMagickInfo("GIF");
    if ( info == (MagickInfo*)NULL )
    exit(1);
    info->decoder = ReadGIFImage;
    info->encoder = WriteGIFImage;
    info->adjoin = False;
    info->description = "CompuServe graphics interchange format";
    /* Add MagickInfo structure to list */
    RegisterMagickInfo(info);
    info = GetMagickInfo("GIF");
    [ do something with info ... ]
    ListMagickInfo( stdout );
    return;
  }

MagickInfo Structure Definition

The members of the MagickInfo structure are shown in the following table:

.. table:: MagickInfo Structure Members

   +------------+--------------------+------------------------------------------------------------------+
   |   Member   |       Type         |                           Description                            |
   +------------+--------------------+------------------------------------------------------------------+
   |adjoin      |unsigned int        |Set to non-zero (True) if this file format supports multi-frame   |
   |            |                    |images.                                                           |
   +------------+--------------------+------------------------------------------------------------------+
   |            |                    |Set to non-zero (True) if the encoder and decoder for this format |
   |blob_support|unsigned int        |supports operating on arbitrary BLOBs (rather than only disk      |
   |            |                    |files).                                                           |
   +------------+--------------------+------------------------------------------------------------------+
   |            |                    |User specified data. A way to pass any sort of data structure to  |
   |data        |void *              |the endoder/decoder. To set this, GetMagickInfo() must be called  |
   |            |                    |to first obtain a pointer to the registered structure since it can|
   |            |                    |not be set via a RegisterMagickInfo() parameter.                  |
   +------------+--------------------+------------------------------------------------------------------+
   |            |Image \*(\*decoder) |                                                                  |
   |decoder     |(const ImageInfo    |Function to decode image data and return GraphicsMagick Image.    |
   |            |\*)                 |                                                                  |
   +------------+--------------------+------------------------------------------------------------------+
   |description |char *              |Long form image format description (e.g. "CompuServe graphics     |
   |            |                    |interchange format").                                             |
   +------------+--------------------+------------------------------------------------------------------+
   |            |unsigned int        |                                                                  |
   |encoder     |(\*encoder)(const   |Function to encode image data with options passed via ImageInfo   |
   |            |ImageInfo \*, Image |and image represented by Image.                                   |
   |            |\*)                 |                                                                  |
   +------------+--------------------+------------------------------------------------------------------+
   |module      |char *              |Name of module (e.g. "GIF") which registered this format. Set to  |
   |            |                    |NULL if format is not registered by a module.                     |
   +------------+--------------------+------------------------------------------------------------------+
   |name        |const char *        |Magick string (e.g. "GIF") which identifies this format.          |
   +------------+--------------------+------------------------------------------------------------------+
   |next        |MagickInfo          |Next MagickInfo struct in linked-list. NULL if none.              |
   +------------+--------------------+------------------------------------------------------------------+
   |previous    |MagickInfo          |Previous MagickInfo struct in linked-list. NULL if none.          |
   +------------+--------------------+------------------------------------------------------------------+
   |raw         |unsigned int        |Image format does not contain size (must be specified in          |
   |            |                    |ImageInfo).                                                       |
   +------------+--------------------+------------------------------------------------------------------+

MonitorHandler
==============

MonitorHandler is the function type to be used for the progress monitor
callback. Its definition is as follows::

  typedef unsigned int
    (*MonitorHandler)(const char *text, const magick_int64_t quantum,
      const magick_uint64_t span, ExceptionInfo *exception);

The operation of the monitor handler is described in the following table:

.. table:: MonitorHandler Parameters

   +------------------------+------------------------+-----------------------------------------------+
   |       Parameter        |          Type          |                  Description                  |
   +------------------------+------------------------+-----------------------------------------------+
   | return status          | unsigned int           | The progress monitor should normally return   |
   |                        |                        | True (a non-zero value) if the operation is   |
   |                        |                        | to continue. If the progress monitor returns  |
   |                        |                        | false, then the operation is will be aborted. |
   |                        |                        | This mechanism allows a user to terminate a   |
   |                        |                        | process which is taking too long to complete. |
   +------------------------+------------------------+-----------------------------------------------+
   | text                   | const char *           | A description of the current operation being  |
   |                        |                        | performed.                                    |
   +------------------------+------------------------+-----------------------------------------------+
   | quantum                | const magick_int64_t   | A value within the range of 0 to span which   |
   |                        |                        | indicates the degree of progress.             |
   +------------------------+------------------------+-----------------------------------------------+
   | span                   | const magick_uint64_t  | The total range that quantum will span.       |
   +------------------------+------------------------+-----------------------------------------------+
   | exception              | exceptionInfo *        | If the progress monitor returns False (abort  |
   |                        |                        | operation), it should also update the         |
   |                        |                        | structure passed via the exception parameter  |
   |                        |                        | so that an error message may be reported to   |
   |                        |                        | the user.                                     |
   +------------------------+------------------------+-----------------------------------------------+

MontageInfo
===========

::

  typedef struct _MontageInfo
  {
    char
      *geometry,
      *tile,
      *title,
      *frame,
      *texture,
      *font;
  
    double
      pointsize;
  
    unsigned long
      border_width;
  
    unsigned int
      shadow;
  
    PixelPacket
      fill,
      stroke,
      background_color,
      border_color,
      matte_color;
  
    GravityType
      gravity;
  
    char
      filename[MaxTextExtent];
  
    unsigned long
      signature;
  } MontageInfo;
  

NoiseType
=========

NoiseType is used as an argument to select the type of noise to be added
to the image.
 
.. table:: NoiseType

   +----------------------------------------------+--------------------------------------------------+
   |                 Enumeration                  |                   Description                    |
   +----------------------------------------------+--------------------------------------------------+
   |UniformNoise                                  |Uniform noise                                     |
   +----------------------------------------------+--------------------------------------------------+
   |GaussianNoise                                 |Gaussian noise                                    |
   +----------------------------------------------+--------------------------------------------------+
   |MultiplicativeGaussianNoise                   |Multiplicative Gaussian noise                     |
   +----------------------------------------------+--------------------------------------------------+
   |ImpulseNoise                                  |Impulse noise                                     |
   +----------------------------------------------+--------------------------------------------------+
   |LaplacianNoise                                |Laplacian noise                                   |
   +----------------------------------------------+--------------------------------------------------+
   |PoissonNoise                                  |Poisson noise                                     |
   +----------------------------------------------+--------------------------------------------------+

OrientationType
===============

OrientationType specifies the orientation of the image. Useful for when
the image is produced via a different ordinate system, the camera was
turned on its side, or the page was scanned sideways.

.. table:: OrientationType

   +------------------------+----------------------------+-------------------------------------------+
   |      Enumeration       |     Scanline Direction     |              Frame Direction              |
   +------------------------+----------------------------+-------------------------------------------+
   |UndefinedOrientation    |Unknown                     |Unknown                                    |
   +------------------------+----------------------------+-------------------------------------------+
   |TopLeftOrientation      |Left to right               |Top to bottom                              |
   +------------------------+----------------------------+-------------------------------------------+
   |TopRightOrientation     |Right to left               |Top to bottom                              |
   +------------------------+----------------------------+-------------------------------------------+
   |BottomRightOrientation  |Right to left               |Bottom to top                              |
   +------------------------+----------------------------+-------------------------------------------+
   |BottomLeftOrientation   |Left to right               |Bottom to top                              |
   +------------------------+----------------------------+-------------------------------------------+
   |LeftTopOrientation      |Top to bottom               |Left to right                              |
   +------------------------+----------------------------+-------------------------------------------+
   |RightTopOrientation     |Top to bottom               |Right to left                              |
   +------------------------+----------------------------+-------------------------------------------+
   |RightBottomOrientation  |Bottom to top               |Right to left                              |
   +------------------------+----------------------------+-------------------------------------------+
   |LeftBottomOrientation   |Bottom to top               |Left to right                              |
   +------------------------+----------------------------+-------------------------------------------+

PaintMethod
===========

PaintMethod specifies how pixel colors are to be replaced in the image.
It is used to select the pixel-filling algorithm employed.

.. table:: PaintMethod

   +------------------+------------------------------------------------------------------------------+
   |   Enumeration    |                                 Description                                  |
   +------------------+------------------------------------------------------------------------------+
   |PointMethod       |Replace pixel color at point.                                                 |
   +------------------+------------------------------------------------------------------------------+
   |ReplaceMethod     |Replace color for all image pixels matching color at point.                   |
   +------------------+------------------------------------------------------------------------------+
   |FloodfillMethod   |Replace color for pixels surrounding point until encountering pixel that fails|
   |                  |to match color at point.                                                      |
   +------------------+------------------------------------------------------------------------------+
   |FillToBorderMethod|Replace color for pixels surrounding point until encountering pixels matching |
   |                  |border color.                                                                 |
   +------------------+------------------------------------------------------------------------------+
   |ResetMethod       |Replace colors for all pixels in image with pen color.                        |
   +------------------+------------------------------------------------------------------------------+

PixelPacket
===========

The PixelPacket structure is used to represent DirectClass color pixels
in GraphicsMagick. If the image is indicated as a PseudoClass image, its
DirectClass representation is only valid immediately after calling
SyncImage(). If an image is set as PseudoClass and the DirectClass
representation is modified, the image should then be set as DirectClass.
Use QuantizeImage() to restore the PseudoClass colormap if the
DirectClass representation is modified.

The members of the PixelPacket structure are shown in the following table:
 

.. table:: PixelPacket Structure Members

   +----------+---------+----------------------------------------------------------------------------+
   |          |         |                               Interpretation                               |
   |  Member  |  Type   +----------------------+-------------------------------+---------------------+
   |          |         |  RGBColorspace (3)   |   RGBColorspace + matte(3)    |   CMYKColorspace    |
   +----------+---------+----------------------+-------------------------------+---------------------+
   |red       |Quantum  |Red                   |Red                            |Cyan                 |
   +----------+---------+----------------------+-------------------------------+---------------------+
   |green     |Quantum  |Green                 |Green                          |Magenta              |
   +----------+---------+----------------------+-------------------------------+---------------------+
   |blue      |Quantum  |Blue                  |Blue                           |Yellow               |
   +----------+---------+----------------------+-------------------------------+---------------------+
   |opacity   |Quantum  |Ignored               |Opacity                        |Black                |
   +----------+---------+----------------------+-------------------------------+---------------------+

Notes:

 1. Quantum is an unsigned short (MaxRGB=65535) if GraphicsMagick is
    built using -DQuantumDepth=16 Otherwise it is an unsigned char
    (MaxRGB=255).

 2. SyncImage() may be used to synchronize the DirectClass color pixels
    to the current PseudoClass colormap.

 3. For pixel representation purposes, all colorspaces are treated like
    RGBColorspace except for CMYKColorspace.


PointInfo
=========

The PointInfo structure is used by the ChromaticityInfo structure to
specify chromaticity point values. This defines the boundaries and gammut
(range of included color) of the colorspace.

The members of the PointInfo structure are shown in the following table:
 

.. table:: PointInfo Structure Members

   +-----------------------------+------------------------+------------------------------------------+
   |           Member            |          Type          |               Description                |
   +-----------------------------+------------------------+------------------------------------------+
   |x                            |double                  |X ordinate                                |
   +-----------------------------+------------------------+------------------------------------------+
   |y                            |double                  |Y ordinate                                |
   +-----------------------------+------------------------+------------------------------------------+
   |z                            |double                  |Z ordinate                                |
   +-----------------------------+------------------------+------------------------------------------+

ProfileInfo
===========

The ProfileInfo structure is used to represent ICC or IPCT profiles in
GraphicsMagick (stored as an opaque BLOB).

The members of the ProfileInfo structure are shown in the following table:
 
.. table:: ProfileInfo Structure Members

   +-----------------------+--------------------------------------+----------------------------------+
   |        Member         |                 Type                 |           Description            |
   +-----------------------+--------------------------------------+----------------------------------+
   |length                 |unsigned int                          |Profile length                    |
   +-----------------------+--------------------------------------+----------------------------------+
   |info                   |unsigned char *                       |Profile data                      |
   +-----------------------+--------------------------------------+----------------------------------+

QuantizeInfo
============

The QuantizeInfo structure is used to support passing parameters to
GraphicsMagick's color quantization (reduction) functions. Color
quantization is the process of analyzing one or more images, and
calculating a color palette which best approximates the image within a
specified colorspace, and then adjusting the image pixels to use the
calculated color palette. The maximum number of colors allowed in the
color palette may be specified.
 

.. table:: Methods Supporting QuantizeInfo

   +-------------------+------------------------------------------------------------------------------+
   |      Method       |                                 Description                                  |
   +-------------------+------------------------------------------------------------------------------+
   |GetQuantizeInfo()  |Allocate new structure with defaults set.                                     |
   +-------------------+------------------------------------------------------------------------------+
   |CloneQuantizeInfo()|Copy existing structure, allocating new structure in the process.             |
   +-------------------+------------------------------------------------------------------------------+
   |DestroyQuantizeInfo|Deallocate structure, including any members.                                  |
   |()                 |                                                                              |
   +-------------------+------------------------------------------------------------------------------+
   |QuantizeImage      |Analyzes the colors within a reference image and chooses a fixed number of    |
   |                   |colors to represent the image.                                                |
   +-------------------+------------------------------------------------------------------------------+
   |QuantizeImages     |Analyzes the colors within a set of reference images and chooses a fixed      |
   |                   |number of colors to represent the set.                                        |
   +-------------------+------------------------------------------------------------------------------+

.. table:: QuantizeInfo Structure Members

   +-------------+--------------+----------------------------------------------------------------------+
   |   Member    |     Type     |                             Description                              |
   +-------------+--------------+----------------------------------------------------------------------+
   |             |              |The colorspace to quantize in. Color reduction, by default, takes     |
   |             |              |place in the RGB color space.  Empirical  evidence  suggests that     |
   |colorspace   |ColorspaceType|distances in color spaces such as YUV or YIQ  correspond  to          |
   |             |              |perceptual  color differences more closely  than  do distances in RGB |
   |             |              |space. The Transparent color space behaves uniquely in that it        |
   |             |              |preserves the matte channel of the image if it exists.                |
   +-------------+--------------+----------------------------------------------------------------------+
   |             |              |Set to True (non-zero) to apply Floyd/Steinberg error diffusion to the|
   |dither       |unsigned int  |image. When the size of the color palette is less than the image      |
   |             |              |colors, this trades off spacial resolution for color resolution by    |
   |             |              |dithering to achieve a similar looking image.                         |
   +-------------+--------------+----------------------------------------------------------------------+
   |measure_error|unsigned int  |Set to True (non-zero) to calculate quantization errors when          |
   |             |              |quantizing the image.                                                 |
   +-------------+--------------+----------------------------------------------------------------------+
   |             |              |Specify the maximum number of colors in the output image. Must be     |
   |number_colors|unsigned int  |equal to, or less than MaxRGB, which is determined by the value of    |
   |             |              |QuantumLeap when GraphicsMagick was compiled.                         |
   +-------------+--------------+----------------------------------------------------------------------+
   |signature    |unsigned long |???                                                                   |
   +-------------+--------------+----------------------------------------------------------------------+
   |             |              |Specify the tree depth to use while quantizing. The values zero and   |
   |             |              |one support automatic tree depth determination. The tree depth may be |
   |tree_depth   |unsigned int  |forced via values ranging from two to eight. The ideal tree depth     |
   |             |              |depends on the characteristics of the input image, and may be         |
   |             |              |determined through experimentation.                                   |
   +-------------+--------------+----------------------------------------------------------------------+

Quantum
========

Quantum is the base type ('unsigned char', 'unsigned short', 'unsigned
int') used to store a pixel component (e.g. 'R' is one pixel component of
an RGB pixel).


QuantumType
===========

QuantumType is used to indicate the source or destination format of
entire pixels, or components of pixels ("Quantums") while they are being
read, or written to, a pixel cache. The validity of these format
specifications depends on whether the Image pixels are in RGB format,
RGBA format, or CMYK format. The pixel Quantum size is determined by the
Image depth (eight or sixteen bits).

.. table:: RGB(A) Image Quantums

   +-----------------+-------------------------------------------------------------------------------+
   |   Enumeration   |                                  Description                                  |
   +-----------------+-------------------------------------------------------------------------------+
   |IndexQuantum     |PseudoColor colormap indices (valid only for image with colormap)              |
   +-----------------+-------------------------------------------------------------------------------+
   |RedQuantum       |Red pixel Quantum                                                              |
   +-----------------+-------------------------------------------------------------------------------+
   |GreenQuantum     |Green pixel Quantum                                                            |
   +-----------------+-------------------------------------------------------------------------------+
   |BlueQuantum      |Blue pixel Quantum                                                             |
   +-----------------+-------------------------------------------------------------------------------+
   |OpacityQuantum   |Opacity (Alpha) Quantum                                                        |
   +-----------------+-------------------------------------------------------------------------------+

.. table:: CMY(K) Image Quantum

   +--------------------------------------+----------------------------------------------------------+
   |             Enumeration              |                       Description                        |
   +--------------------------------------+----------------------------------------------------------+
   |CyanQuantum                           |Cyan pixel Quantum                                        |
   +--------------------------------------+----------------------------------------------------------+
   |MagentaQuantum                        |Magenta pixel Quantum                                     |
   +--------------------------------------+----------------------------------------------------------+
   |YellowQuantum                         |Yellow pixel Quantum                                      |
   +--------------------------------------+----------------------------------------------------------+
   |BlackQuantum                          |Black pixel Quantum                                       |
   +--------------------------------------+----------------------------------------------------------+

.. table:: Grayscale Image Quantums

   +--------------------------------------------------------+----------------------------------------+
   |                      Enumeration                       |              Description               |
   +--------------------------------------------------------+----------------------------------------+
   |GrayQuantum                                             |Gray pixel                              |
   +--------------------------------------------------------+----------------------------------------+
   |GrayOpacityQuantum                                      |Pixel opacity                           |
   +--------------------------------------------------------+----------------------------------------+

.. table:: Entire Pixels (Expressed in Byte Order)

   +---------------------------+---------------------------------------------------------------------+
   |        Enumeration        |                             Description                             |
   +---------------------------+---------------------------------------------------------------------+
   |RGBQuantum                 |RGB pixel (24 or 48 octets)                                          |
   +---------------------------+---------------------------------------------------------------------+
   |RGBAQuantum                |RGBA pixel (32 or 64 octets)                                         |
   +---------------------------+---------------------------------------------------------------------+
   |CMYKQuantum                |CMYK pixel (32 or 64 octets)                                         |
   +---------------------------+---------------------------------------------------------------------+

RectangleInfo
=============

The RectangleInfo structure is used to represent positioning information
in GraphicsMagick.

The members of the RectangleInfo structure are shown in the following
table:

.. table:: RectangleInfo Structure Members

   +-------------------+------------------------+----------------------------------------------------+
   |      Member       |          Type          |                    Description                     |
   +-------------------+------------------------+----------------------------------------------------+
   |width              |unsigned int            |Rectangle width                                     |
   +-------------------+------------------------+----------------------------------------------------+
   |height             |unsigned int            |Rectangle height                                    |
   +-------------------+------------------------+----------------------------------------------------+
   |x                  |int                     |Rectangle horizontal offset                         |
   +-------------------+------------------------+----------------------------------------------------+
   |y                  |int                     |Rectangle vertical offset                           |
   +-------------------+------------------------+----------------------------------------------------+

RegistryType
============

::

  typedef enum
  {
    UndefinedRegistryType,
    ImageRegistryType,
    ImageInfoRegistryType
  } RegistryType;


RenderingIntent
===============

Rendering intent is a concept defined by ICC Spec ICC.1:1998-09, "File
Format for Color Profiles". GraphicsMagick uses RenderingIntent in order
to support ICC Color Profiles.

From the specification: "Rendering intent specifies the style of
reproduction to be used during the evaluation of this profile in a
sequence of profiles. It applies specifically to that profile in the
sequence and not to the entire sequence. Typically, the user or
application will set the rendering intent dynamically at runtime or
embedding time."

.. table:: RenderingIntent

   +----------------+--------------------------------------------------------------------------------+
   |  Enumeration   |                                  Description                                   |
   +----------------+--------------------------------------------------------------------------------+
   |UndefinedIntent |Unset value.                                                                    |
   +----------------+--------------------------------------------------------------------------------+
   |SaturationIntent|A rendering intent that specifies the saturation of the pixels in the image is  |
   |                |preserved perhaps at the expense of accuracy in hue and lightness.              |
   +----------------+--------------------------------------------------------------------------------+
   |                |A rendering intent that specifies the full gamut of the image is compressed or  |
   |PerceptualIntent|expanded to fill the gamut of the destination device. Gray balance is preserved |
   |                |but colorimetric accuracy might not be preserved.                               |
   +----------------+--------------------------------------------------------------------------------+
   |AbsoluteIntent  |Absolute colorimetric                                                           |
   +----------------+--------------------------------------------------------------------------------+
   |RelativeIntent  |Relative colorimetric                                                           |
   +----------------+--------------------------------------------------------------------------------+

ResolutionType
==============

By default, GraphicsMagick defines resolutions in pixels per inch.
ResolutionType provides a means to adjust this.

.. table:: ResolutionType

   +-----------------------------+-------------------------------------------------------------------+
   |         Enumeration         |                            Description                            |
   +-----------------------------+-------------------------------------------------------------------+
   |UndefinedResolution          |Unset value.                                                       |
   +-----------------------------+-------------------------------------------------------------------+
   |PixelsPerInchResolution      |Density specifications are specified in units of pixels per inch   |
   |                             |(english units).                                                   |
   +-----------------------------+-------------------------------------------------------------------+
   |PixelsPerCentimeterResolution|Density specifications are specified in units of pixels per        |
   |                             |centimeter (metric units).                                         |
   +-----------------------------+-------------------------------------------------------------------+

ResourceType
============

::

  typedef enum
  {
    UndefinedResource,
    FileResource,
    MemoryResource,
    MapResource,
    DiskResource
  } ResourceType;


SegmentInfo
===========

::

  typedef struct _SegmentInfo
  {
    double
      x1,
      y1,
      x2,
      y2;
  } SegmentInfo;

SignatureInfo
=============

::

  typedef struct _SignatureInfo
  {
    unsigned long
      digest[8],
      low_order,
      high_order;
  
    long
      offset;
  
    unsigned char
      message[SignatureSize];
  } SignatureInfo;


StorageType
===========

::

  typedef enum
  {
    CharPixel,
    ShortPixel,
    IntegerPixel,
    LongPixel,
    FloatPixel,
    DoublePixel
  } StorageType;


StreamHandler
=============

::

  typedef unsigned int
    (*StreamHandler)(const Image *,const void *,const size_t);

StretchType
===========

::

  typedef enum
  {
    NormalStretch,
    UltraCondensedStretch,
    ExtraCondensedStretch,
    CondensedStretch,
    SemiCondensedStretch,
    SemiExpandedStretch,
    ExpandedStretch,
    ExtraExpandedStretch,
    UltraExpandedStretch,
    AnyStretch
  } StretchType;


StyleType
=========

::

  typedef enum
  {
    NormalStyle,
    ItalicStyle,
    ObliqueStyle,
    AnyStyle
  } StyleType;


TypeMetric
==========

::

  typedef struct _TypeMetric
  {
    PointInfo
      pixels_per_em;
  
    double
      ascent,
      descent,
      width,
      height,
      max_advance;
  
    SegmentInfo
      bounds;
  
    double
      underline_position,
      underline_thickness;
  } TypeMetric;


ViewInfo
========

ViewInfo represents a handle to a pixel view, which represents a uniquely
selectable rectangular region of pixels. The only limit on the number of
views is the amount of available memory. Each Image contains a collection
of default views (one view per thread) so that the image may be usefully
accessed without needing to explicitly allocate pixel views.

::

  typedef void *ViewInfo;


VirtualPixelMethod
==================

::

  typedef enum
  {
    UndefinedVirtualPixelMethod,
    ConstantVirtualPixelMethod,
    EdgeVirtualPixelMethod,
    MirrorVirtualPixelMethod,
    TileVirtualPixelMethod
  } VirtualPixelMethod;


MagickXResourceInfo
===================

::

  typedef struct _XResourceInfo
  {
    XrmDatabase
      resource_database;
  
    ImageInfo
      *image_info;
  
    QuantizeInfo
      *quantize_info;
  
    unsigned long
      colors;
  
    unsigned int
      close_server,
      backdrop;
  
    char
      *background_color,
      *border_color;
  
    char
      *client_name;
  
    XColormapType
      colormap;
  
    unsigned int
      border_width,
      color_recovery,
      confirm_exit,
      delay;
  
    char
      *display_gamma;
  
    char
      *font,
      *font_name[MaxNumberFonts],
      *foreground_color;
  
    unsigned int
      display_warnings,
      gamma_correct;
  
    char
      *icon_geometry;
  
    unsigned int
      iconic,
      immutable;
  
    char
      *image_geometry;
  
    char
      *map_type,
      *matte_color,
      *name;
  
    unsigned int
      magnify,
      pause;  char
      *pen_colors[MaxNumberPens];
  
    char
      *text_font,
      *title;
  
    int
      quantum;
  
    unsigned int
      update,
      use_pixmap,
      use_shared_memory;
  
    unsigned long
      undo_cache;
  
    char
      *visual_type,
      *window_group,
      *window_id,
      *write_filename;
  
    Image
      *copy_image;
  
    int
      gravity;
  
    char
      home_directory[MaxTextExtent];
  } XResourceInfo;

-------------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2015

