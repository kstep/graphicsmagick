.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

============
Enumerations
============

.. contents:: Magick++ Enumerations
   :depth: 1

ChannelType
-----------

*ChannelType* is used as an argument when doing color separations. Use
*ChannelType* when extracting a layer from an image. *MatteChannel* is
useful for extracting the opacity values from an image. Note that an
image may be represented in RGB, RGBA, CMYK, or CMYKA, pixel formats
and a channel may only be extracted if it is valid for the current
pixel format.

.. table:: ChannelType

   =====================  ==================================================
   Enumeration            Description
   =====================  ==================================================
   UndefinedChannel       Unset value.
   RedChannel             RGB Red channel
   CyanChannel            CMYK Cyan channel
   GreenChannel           RGB Green channel
   MagentaChannel         CMYK Magenta channel
   BlueChannel            RGB Blue channel
   YellowChannel          CMYK Yellow channel
   OpacityChannel         Opacity channel (inverse of transparency)
   BlackChannel           CMYK Black (K) channel
   MatteChannel           Same as Opacity channel (deprecated)
   AllChannels            All color channels
   GrayChannel            Color channels represent an intensity
   =====================  ==================================================

ClassType
---------

*ClassType* specifies the image storage class. 

.. table:: ClassType

   =====================  ==================================================
   Enumeration            Description
   =====================  ==================================================
   UndefinedClass         Unset value.
   DirectClass            Image is composed of pixels which represent
                          literal color values.
   PseudoClass            Image is composed of pixels which specify an index
                          in a color palette.
   =====================  ==================================================

ColorspaceType
--------------

The *ColorspaceType* enumeration is used to specify the colorspace that
quantization (color reduction and mapping) is done under or to specify
the colorspace when encoding an output image. Colorspaces are ways of
describing colors to fit the requirements of a particular application
(e.g. Television, offset printing, color monitors).  Color reduction,
by default, takes place in the *RGBColorspace*. Empirical evidence
suggests that distances in color spaces such as *YUVColorspace* or
*YIQColorspace* correspond to perceptual color differences more closely
han do distances in RGB space. These color spaces may give better
results when color reducing an image. Refer to quantize for more
details.

When encoding an output image, the colorspaces *RGBColorspace*,
*CMYKColorspace*, *GRAYColorspace*, or *YCbCrColorspace* may be
specified. The CMYKColorspace option is only applicable when writing
TIFF, JPEG, and Adobe Photoshop bitmap (PSD) files.

.. table:: ColorspaceType

   =====================  ==================================================
   Enumeration            Description
   =====================  ==================================================
   UndefinedColorspace    Unset value.
   RGBColorspace          Red-Green-Blue colorspace.
   GRAYColorspace         Grayscale colorspace
   TransparentColorspace  The Transparent color space behaves uniquely in
                          that it preserves the matte channel of the image
                          if it exists.
   OHTAColorspace
   XYZColorspace
   YCbCrColorspace
   YCCColorspace
   YIQColorspace
   YPbPrColorspace
   YUVColorspace          Y-signal, U-signal, and V-signal colorspace. YUV is
                          most widely used to encode color for use in
                          television transmission.
   CMYKColorspace         Cyan-Magenta-Yellow-Black colorspace. CYMK is a
                          subtractive color system used by printers and
                          photographers for the rendering of colors with ink
                          or emulsion,
                          normally on a white surface.
   sRGBColorspace         Kodak PhotoCD sRGB
   =====================  ==================================================

CompositeOperator
-----------------

*CompositeOperator* is used to select the image composition algorithm
used to compose a composite image with an image. By default, each of
the composite image pixels are replaced by the corresponding image
tile pixel. Specify *CompositeOperator* to select a different
algorithm.

.. table:: CompositeOperator

   =======================  ==================================================
   Enumeration              Description
   =======================  ==================================================
   UndefinedCompositeOp     Not defined.
   OverCompositeOp          Union of the two image shapes, with
                            opaque areas of change-image obscuring base-image
                            in the region of overlap.
   InCompositeOp            Change-image cut by the shape of base-image.
                            None of the image data of base-image will be
                            in the result.
   OutCompositeOp           Change-image with the shape of base-image cut out.
   AtopCompositeOp          Same shape as base-image, with change-image
                            obscuring base-image where the image shapes
                            overlap. Note this differs from over because the
                            portion of change-image outside base-image's shape
                            does not appear in the result.
   XorCompositeOp           Image data from both change-image and base-image
                            that is outside the overlap region. The overlap
                            region will be blank
   PlusCompositeOp          Sum of the image data. Output values are
                            cropped to MaxRGB (no overflow). This operation is
                            independent of the matte channels.
   MinusCompositeOp         Change-image - base-image, with underflow cropped
                            to zero. The matte channel is ignored (set to
                            opaque, full coverage)
   AddCompositeOp           Change-image + base-image, with overflow wrapping
                            around (mod MaxRGB+1)
   SubtractCompositeOp      Change-image - base-image, with underflow wrapping
                            around (mod MaxRGB+1). The add and subtract
                            operators can be used to perform reversible
                            transformations
   DifferenceCompositeOp    Absolute value of change-image minus base-image.
   MultiplyCompositeOp      Change-image * base-image. This is useful for the
                            creation of drop-shadows.
   BumpmapCompositeOp       Base-image shaded by change-image
   CopyCompositeOp          Base-image replaced with change-image. Here
                            the matte information is ignored
   CopyRedCompositeOp       Red channel in base-image replaced with
                            the red channel in change-image. The other channels
                            are copied untouched
   CopyGreenCompositeOp     Green channel in base-image replaced with the green
                            channel in change-image. The other channels are
                            copied untouched.
   CopyBlueCompositeOp      Blue channel in base-image replaced with the blue
                            channel in change-image. The other channels are
                            copied untouched.
   CopyOpacityCompositeOp   Opacity channel in base-image replaced with the
                            opacity channel in change-image.  The other
                            channels are copied untouched.
   ClearCompositeOp         Pixels are set to transparent.
   DissolveCompositeOp
   DisplaceCompositeOp
   ModulateCompositeOp
   ThresholdCompositeOp
   NoCompositeOp
   DarkenCompositeOp
   LightenCompositeOp
   HueCompositeOp
   SaturateCompositeOp
   ColorizeCompositeOp
   LuminizeCompositeOp
   ScreenCompositeOp        Not yet implemented (Photoshop & PDF)
   OverlayCompositeOp       Not yet implemented (Photoshop & PDF)
   CopyCyanCompositeOp
   CopyMagentaCompositeOp
   CopyYellowCompositeOp
   CopyBlackCompositeOp     Copy CMYK Black (K) channel.
   DivideCompositeOp        Change-image / base-image. This is useful for 
                            improving the readability of text on unevenly
                            illuminated photos. (by using a gaussian blurred
                            copy of change-image as base-image)
   =======================  ==================================================

CompressionType
---------------

*CompressionType* is used to express the desired compression type when
encoding an image. Be aware that most image types only support a
sub-set of the available compression types. If the compression type
specified is incompatable with the image, GraphicsMagick selects a
compression type compatable with the image type.

.. table:: CompressionType

   =======================  ==================================================
   Enumeration              Description
   =======================  ==================================================
   UndefinedCompression     Unset value.
   NoCompression            No compression
   BZipCompression          BZip as used by bzip2 utilities
   FaxCompression           CCITT Group 3 FAX compression
   Group4Compression        CCITT Group 4 FAX compression (used only for TIFF)
   JPEGCompression          JPEG compression
   LZWCompression           Lempel-Ziv-Welch (LZW) compression
   RLECompression           Run-Length encoded (RLE) compression
   ZipCompression           Lempel-Ziv compression (LZ77) as used in GNU gzip.
   LZMACompression          Lempel-Ziv-Markov chain algorithm
   JPEG2000Compression      ISO/IEC std 15444-1
   JBIG1Compression         ISO/IEC std 11544 / ITU-T rec T.82
   JBIG2Compression         ISO/IEC std 14492 / ITU-T rec T.88
   =======================  ==================================================

DecorationType
--------------

The *DecorationType* enumerations are used to specify line decorations
of rendered text.

.. table:: DecorationType

   =======================  ==================================================
   Enumeration              Description
   =======================  ==================================================
   NoDecoration             No decoration
   UnderlineDecoration      Underlined text
   OverlineDecoration       Overlined text
   LineThroughDecoration    Strike-through text
   =======================  ==================================================

EndianType
----------

The *EndianType* enumerations are used to specify the endian option
for formats which support it (e.g. TIFF).

.. table:: EndianType

   =======================  ==================================================
   Enumeration              Description
   =======================  ==================================================
   UndefinedEndian          Not defined (default)
   LSBEndian                Little endian (like Intel X86 and DEC Alpha)
   MSBEndian                Big endian (like Motorola 68K, Mac PowerPC, &
                            SPARC)
   NativeEndian             Use native endian of this CPU
   =======================  ==================================================

FillRule
--------

*FillRule* specifies the algorithm which is to be used to determine
what parts of the canvas are included inside the shape. See the
documentation on SVG's `fill-rule
<http://www.w3.org/TR/SVG/painting.html#FillRuleProperty>`_ property
for usage details.

.. table:: FillRule

   =======================  ==================================================
   Enumeration              Description
   =======================  ==================================================
   UndefinedRule            Fill rule not specified
   EvenOddRule              See SVG fill-rule evenodd rule.
   NonZeroRule              See SVG fill-rule nonzero rule.
   =======================  ==================================================

FilterTypes
-----------

*FilterTypes* is used to adjust the filter algorithm used when
resizing images. Different filters experience varying degrees of
success with various images and can take sipngicantly different
amounts of processing time. GraphicsMagick uses the *LanczosFilter* by
default since this filter has been shown to provide the best results
for most images in a reasonable amount of time. Other filter types
(e.g. *TriangleFilter*) may execute much faster but may show artifacts
when the image is re-sized or around diagonal lines. The only way to
be sure is to test the filter with sample images.

.. table:: FilterTypes

   =======================  ==================================================
   Enumeration              Description
   =======================  ==================================================
   UndefinedFilter          Unset value.
   PointFilter              Point Filter
   BoxFilter                Box Filter
   TriangleFilter           Triangle Filter
   HermiteFilter            Hermite Filter
   HanningFilter            Hanning Filter
   HammingFilter            Hamming Filter
   BlackmanFilter           Blackman Filter
   GaussianFilter           Gaussian Filter
   QuadraticFilter          Quadratic Filter
   CubicFilter              Cubic Filter
   CatromFilter             Catrom Filter
   MitchellFilter           Mitchell Filter
   LanczosFilter            Lanczos Filter
   BesselFilter             Bessel Filter
   SincFilter               Sinc Filter
   =======================  ==================================================

GravityType
-----------

*GravityType* specifies positioning of an object (e.g. text, image)
within a bounding region (e.g. an image). Gravity provides a
convenient way to locate objects irrespective of the size of the
bounding region, in other words, you don't need to provide absolute
coordinates in order to position an object. A common default for
gravity is *NorthWestGravity*.

.. table:: GravityType

   =======================  ==================================================
   Enumeration              Description
   =======================  ==================================================
   ForgetGravity            Don't use gravity.
   NorthWestGravity         Position object at top-left of region.
   NorthGravity             Postiion object at top-center of region
   NorthEastGravity         Position object at top-right of region
   WestGravity              Position object at left-center of region
   CenterGravity            Position object at center of region
   EastGravity              Position object at right-center of region
   SouthWestGravity         Position object at left-bottom of region
   SouthGravity             Position object at bottom-center of region
   SouthEastGravity         Position object at bottom-right of region
   =======================  ==================================================

ImageType
---------

*ImageType* indicates the type classification of the image. 

.. table:: ImageType

   =======================  ==================================================
   Enumeration              Description
   =======================  ==================================================
   UndefinedType            Unset value.
   BilevelType              Monochrome bi-level image
   GrayscaleType            Grayscale image
   GrayscaleMatteType       Grayscale image with opacity
   PaletteType              Indexed color (palette) image
   PaletteMatteType         Indexed color (palette) image with opacity
   TrueColorType            Truecolor image
   TrueColorMatteType       Truecolor image with opacity
   ColorSeparationType      Cyan/Yellow/Magenta/Black (CYMK) image
   OptimizeType             Optimize type based on image characteristics
   =======================  ==================================================

InterlaceType
-------------

*InterlaceType* specifies the ordering of the red, green, and blue
pixel information in the image. Interlacing is often used to make
image information available to the user faster by taking advantage of
the space vs time tradeoff. For example, interlacing allows images on
the Web to be recognizable sooner and satellite images to
accumulate/render with image resolution increasing over time.

Use *LineInterlace* or *PlaneInterlace* to create an interlaced GIF or
progressive JPEG image.

.. table:: InterlaceType

   =======================  ==================================================
   Enumeration              Description
   =======================  ==================================================
   UndefinedInterlace       Unset value.
   NoInterlace              Don't interlace image (RGBRGBRGBRGBRGBRGB...)
   LineInterlace            Use scanline interlacing
                            (RRR...GGG...BBB...RRR...GGG...BBB...)
   PlaneInterlace           Use plane interlacing (RRRRRR...GGGGGG...BBBBBB...)
   PartitionInterlace       Similar to plane interlaing except that the
                            different planes are saved to individual files
                            (e.g. image.R, image.G, and image.B)
   =======================  ==================================================

ChannelType
-----------

*ChannelType* is used as an argument when doing color separations. Use
*ChannelType* when extracting a layer from an image. *MatteLayer* is
useful for extracting the opacity values from an image.

.. table:: ChannelType

   =======================  ==================================================
   Enumeration              Description
   =======================  ==================================================
   UndefinedLayer           Unset value.
   RedLayer                 Select red layer
   GreenLayer               Select green layer
   BlueLayer                Select blue layer
   MatteLayer               Select matte (opacity values) layer
   =======================  ==================================================

LineCap
-------

The *LineCap* enumerations specify shape to be used at the end of open
subpaths when they are stroked. See SVG's `stroke-linecap
<http://www.w3.org/TR/SVG/painting.html#StrokeLinecapProperty>`_ for
examples.

.. table:: LineCap

   =======================  ==================================================
   Enumeration              Description
   =======================  ==================================================
   UndefinedCap             Unset value.
   ButtCap                  Square ending.
   RoundCap                 Rounded ending (half-circle end with radius of 1/2
                            stroke width).
   SquareCap                Square ending, extended by 1/2 the stroke width at
                            end.
   =======================  ==================================================

LineJoin
--------

The *LineJoin* enumerations specify the shape to be used at the
corners of paths or basic shapes when they are stroked. See SVG's
`stroke-linejoin
<http://www.w3.org/TR/SVG/painting.html#StrokeLinejoinProperty>`_ for
examples.

.. table:: LineJoin

   =======================  ==================================================
   Enumeration              Description
   =======================  ==================================================
   UndefinedJoin            Unset value.
   MiterJoin                Sharp-edged join
   RoundJoin                Rounded-edged join
   BevelJoin                Beveled-edged join
   =======================  ==================================================

NoiseType
---------

*NoiseType* is used as an argument to select the type of noise to be
added to the image.

.. table:: NoiseType

   ===========================  ==============================================
   Enumeration                  Description
   ===========================  ==============================================
   UndefinedNoise               Unset value.
   UniformNoise                 Uniform noise
   GaussianNoise                Gaussian noise
   MultiplicativeGaussianNoise  Multiplicative Gaussian noise
   ImpulseNoise                 Impulse noise
   LaplacianNoise               Laplacian noise
   PoissonNoise                 Poisson noise
   RandomNoise                  Random noise
   ===========================  ==============================================

OrientationType
---------------

*OrientationType* specifies the orientation of the image. Useful for
when the image is produced via a different ordinate system, the camera
was turned on its side, or the page was scanned sideways.

.. table:: OrientationType

   ===========================  ====================  ========================
   Enumeration                  Scanline Direction    Frame Direction
   ===========================  ====================  ========================
   UndefinedOrientation         Unknown               Unknown
   TopLeftOrientation           Left to right         Top to bottom
   TopRightOrientation          Right to left         Top to bottom
   BottomRightOrientation       Right to left         Bottom to top
   BottomLeftOrientation        Left to right         Bottom to top
   LeftTopOrientation           Top to bottom         Left to right
   RightTopOrientation          Top to bottom         Right to left
   RightBottomOrientation       Bottom to top         Right to left
   LeftBottomOrientation        Bottom to top         Left to right
   ===========================  ====================  ========================

PaintMethod
-----------

*PaintMethod* specifies how pixel colors are to be replaced in the
image. It is used to select the pixel-filling algorithm employed.

.. table:: PaintMethod

   ===========================  ==============================================
   Enumeration                  Description
   ===========================  ==============================================
   PointMethod                  Replace pixel color at point.
   ReplaceMethod                Replace color for all image pixels matching
                                color at point.
   FloodfillMethod              Replace color for pixels surrounding point
                                until encountering pixel that fails to match
                                color at point.
   FillToBorderMethod           Replace color for pixels surrounding point
                                until encountering pixels matching border
                                color.
   ResetMethod                  Replace colors for all pixels in image with
                                pen color.
   ===========================  ==============================================

QuantumTypes
------------

*QuantumTypes* is used to indicate the source or destination format of
entire pixels, or components of pixels ("Quantums") while they are
being read, or written to, a pixel cache. The validity of these format
specifications depends on whether the Image pixels are in RGB format,
RGBA format, or CMYK format. The pixel Quantum size is determined by
the Image depth (typically 8, 16, or 32 bits, but any value from 1-64
bits integer or float is supported).


.. table:: RGB(A) Image Quantums

   ===========================  ==============================================
   Enumeration                  Description
   ===========================  ==============================================
   IndexQuantum                 PseudoColor colormap indices
   RedQuantum                   Red pixel Quantum
   GreenQuantum                 Green pixel Quantum
   BlueQuantum                  Blue pixel Quantum
   OpacityQuantum               Opacity (Alpha) Quantum
   ===========================  ==============================================

.. table:: CMY(K) Image Quantum

   ===========================  ==============================================
   Enumeration                  Description
   ===========================  ==============================================
   CyanQuantum                  Cyan pixel Quantum
   MagentaQuantum               Magenta pixel Quantum
   YellowQuantum                Yellow pixel Quantum
   BlackQuantum                 Black pixel Quantum
   ===========================  ==============================================

.. table:: Grayscale Image Quantums

   ===========================  ==============================================
   Enumeration                  Description
   ===========================  ==============================================
   GrayQuantum                  Gray pixel
   GrayOpacityQuantum           Pixel opacity
   ===========================  ==============================================

.. table:: Entire Pixels (Expressed in Byte Order)

   ===========================  ==============================================
   Enumeration                  Description
   ===========================  ==============================================
   RGBQuantum                   RGB pixel (1 to 64 bits)
   RGBAQuantum                  RGBA pixel (1 to 64 bits)
   CMYKQuantum                  CMYK pixel (1 to 64 bits)
   ===========================  ==============================================

RenderingIntent
---------------

Rendering intent is a concept defined by `ICC
<http://www.color.org/>`_ Spec ICC.1:1998-09, "File Format for Color
Profiles". GraphicsMagick uses RenderingIntent in order to support ICC
Color Profiles.

From the specification: "Rendering intent specifies the style of
reproduction to be used during the evaluation of this profile in a
sequence of profiles. It applies specifically to that profile in the
sequence and not to the entire sequence. Typically, the user or
application will set the rendering intent dynamically at runtime or
embedding time."

.. table:: RenderingIntent

   ===========================  ==============================================
   Enumeration                  Description
   ===========================  ==============================================
   UndefinedIntent              Unset value.
   SaturationIntent             A rendering intent that specifies the saturation
                                of the pixels in the image is preserved perhaps
                                at the expense of accuracy in hue and lightness.
   PerceptualIntent             A rendering intent that specifies the full gamut
                                of the image is compressed or expanded to fill
                                the gamut of the destination device. Gray
                                balance is preserved but colorimetric accuracy
                                might not be preserved.
   AbsoluteIntent               Absolute colorimetric
   RelativeIntent               Relative colorimetric
   ===========================  ==============================================

ResolutionType
--------------

By default, GraphicsMagick defines resolutions in pixels per
inch. *ResolutionType* provides a means to adjust this.

.. table:: ResolutionType

   =============================  ============================================
   Enumeration                    Description
   =============================  ============================================
   UndefinedResolution            Unset value.
   PixelsPerInchResolution        Density specifications are specified in units
                                  of pixels per inch (english units).
   PixelsPerCentimeterResolution  Density specifications are specified in units
                                  of pixels per centimeter (metric units).
   =============================  ============================================

StorageType
-----------

The *StorageType* enumerations are used to specify the storage format
of pixels in the source or destination pixel array.

.. table:: StorageType

   ===========================  ==============================================
   Enumeration                  Description
   ===========================  ==============================================
   CharPixel                    Character type
   ShortPixel                   Short type
   IntegerPixel                 Integer type
   FloatPixel                   Float type
   DoublePixel                  Double type
   ===========================  ==============================================

StretchType
-----------

The *StretchType* enumerations are used to specify the relative width
of a font to the regular width for the font family. If the width is
not important, the *AnyStretch* enumeration may be specified for a
wildcard match.

.. table:: StretchType

   ===========================  ==============================================
   Enumeration                  Description
   ===========================  ==============================================
   AnyStretch                   Wildcard match for font stretch
   NormalStretch                Normal width font
   UltraCondensedStretch        Ultra-condensed (narrowest) font
   ExtraCondensedStretch        Extra-condensed font
   CondensedStretch             Condensed font
   SemiCondensedStretch         Semi-Condensed font
   SemiExpandedStretch          Semi-Expanded font
   ExpandedStretch              Expanded font
   ExtraExpandedStretch         Extra-Expanded font
   UltraExpandedStretch         Ultra-expanded (widest) font
   ===========================  ==============================================

StyleType
---------

The *StyleType* enumerations are used to specify the style
(e.g. Italic) of a font. If the style is not important, the *AnyStyle*
enumeration may be specified for a wildcard match.

.. table:: StyleType

   ===========================  ==============================================
   Enumeration                  Description
   ===========================  ==============================================
   AnyStyle                     Wildcard match for font style
   NormalStyle                  Normal font style
   ItalicStyle                  Italic font style
   ObliqueStyle                 Oblique font style
   ===========================  ==============================================

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| Bob Friesenhahn 1999 - 2015

