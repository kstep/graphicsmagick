====================
 Magick::Image Class
====================

.. _`Blob` : Blob.html
.. _`Bob Friesenhahn` : mailto:bfriesen@simple.dallas.tx.us
.. _`CoderInfo` : CoderInfo.html
.. _`Color` : Color.html
.. _`Drawable` : Drawable.html
.. _`Enumerations` : Enumerations.html
.. _`Exception` : Exception.html
.. _`Geometry` : Geometry.html
.. _`GraphicsMagick` : ../index.html
.. _`PixelPacket` : PixelPacket.html
.. _`Pixels` : Pixels.html
.. _`STL` : STL.html
.. _`TypeMetric` : TypeMetric.html
.. _`image design` : ImageDesign.html
.. _`special format characters` : FormatCharacters.html

.. contents::
   :depth: 1

Introduction
------------

Image is the primary object in Magick++ and represents a single image
frame (see `image design`_). The `STL`_ interface must be used to
operate on image sequences or images (e.g. of format GIF, TIFF, MIFF,
Postscript, & MNG) which are comprized of multiple image
frames. Individual frames of a multi-frame image may be requested by
adding array-style notation to the end of the file name
(e.g. "animation.gif[3]" retrieves the fourth frame of a GIF
animation.  Various image manipulation operations may be applied to
the image. Attributes may be set on the image to influence the
operation of the manipulation operations. The `Pixels`_ class provides
low-level access to image pixels. As a convenience, including
<Magick++.h> is sufficient in order to use the complete Magick++
API. The Magick++ API is enclosed within the Magick namespace so you
must either add the prefix " Magick:: " to each class/enumeration name
or add the statement " using namespace Magick;" after including the
Magick++.h header.

The preferred way to allocate Image objects is via automatic
allocation (on the stack). There is no concern that allocating Image
objects on the stack will excessively enlarge the stack since Magick++
allocates all large data objects (such as the actual image data) from
the heap. Use of automatic allocation is preferred over explicit
allocation (via new) since it is much less error prone and allows use
of C++ scoping rules to avoid memory leaks. Use of automatic
allocation allows Magick++ objects to be assigned and copied just like
the C++ intrinsic data types (e.g. 'int '), leading to clear and easy
to read code. Use of automatic allocation leads to naturally
exception-safe code since if an exception is thrown, the object is
automatically deallocated once the stack unwinds past the scope of the
allocation (not the case for objects allocated via new ).

Image is very easy to use. For example, here is a the source to a
program which reads an image, crops it, and writes it to a new file
(the exception handling is optional but strongly recommended)::

    #include <Magick++.h>
    #include <iostream>
    using namespace std;
    using namespace Magick;
    int main(int argc,char **argv)
    {
      // Construct the image object. Seperating image construction from the
      // the read operation ensures that a failure to read the image file
      // doesn't render the image object useless.
      Image image;

      try {
        // Read a file into image object
        image.read( "girl.gif" );

        // Crop the image to specified size (width, height, xOffset, yOffset)
        image.crop( Geometry(100,100, 100, 100) );

        // Write the image to a file
        image.write( "x.gif" );
      }
      catch( Exception &error_ )
        {
          cout << "Caught exception: " << error_.what() << endl;
          return 1;
        }
      return 0;
    }

The following is the source to a program which illustrates the use of
Magick++'s efficient reference-counted assignment and copy-constructor
operations which minimize use of memory and eliminate unncessary copy
operations (allowing Image objects to be efficiently assigned, and
copied into containers).  The program accomplishes the following:

1. Read master image.
2. Assign master image to second image.
3. Zoom second image to the size 640x480.
4. Assign master image to a third image.
5. Zoom third image to the size 800x600.
6. Write the second image to a file.
7. Write the third image to a file.

::

    #include <Magick++.h>
    #include <iostream>
    using namespace std;
    using namespace Magick;
    int main(int argc,char **argv)
    {
        Image master("horse.jpg");
        Image second = master;
        second.zoom("640x480");
        Image third = master;
        third.zoom("800x600");
        second.write("horse640x480.jpg");
        third.write("horse800x600.jpg");
        return 0;
    }

During the entire operation, a maximum of three images exist in memory
and the image data is never copied.

The following is the source for another simple program which creates a
100 by 100 pixel white image with a red pixel in the center and writes
it to a file::

    #include <Magick++.h>
    using namespace std;
    using namespace Magick;
    int main(int argc,char **argv)
    {
        Image image( "100x100", "white" );
        image.pixelColor( 49, 49, "red" );
        image.write( "red_pixel.png" );
        return 0;
    }

If you wanted to change the color image to grayscale, you could add
the lines::

    image.quantizeColorSpace( GRAYColorspace );
    image.quantizeColors( 256 );
    image.quantize( ); 

or, more simply::

    image.type( GrayscaleType );

prior to writing the image.

BLOBs
-----

While encoded images (e.g. JPEG) are most often written-to and
read-from a disk file, encoded images may also reside in
memory. Encoded images in memory are known as BLOBs (Binary Large
OBjects) and may be represented using the `Blob`_ class. The encoded
image may be initially placed in memory by reading it directly from a
file, reading the image from a database, memory-mapped from a disk
file, or could be written to memory by Magick++. Once the encoded
image has been placed within a `Blob`_, it may be read into a Magick++
Image via a constructor or read() . Likewise, a Magick++ image may be
written to a `Blob`_ via write().

An example of using Image to write to a `Blob`_ follows::

    #include <Magick++.h>
    using namespace std;
    using namespace Magick;
    int main(int argc,char **argv)
    {
        // Read GIF file from disk
        Image image( "giraffe.gif" );

        // Write to BLOB in JPEG format
        Blob blob;
        image.magick( "JPEG" ) // Set JPEG output format
        image.write( &blob );

        [ Use BLOB data (in JPEG format) here ]

        return 0;
    }

likewise, to read an image from a `Blob`_, you could use one of the
following examples:

[ Entry condition for the following examples is that data is pointer
to encoded image data and length represents the size of the data ]

::

    Blob blob( data, length );
    Image image( blob );

or

::

    Blob blob( data, length );
    Image image;
    image.read( blob);

Some images do not contain their size or format so the size and format
must be specified in advance::

    Blob blob( data, length );
    Image image;
    image.size( "640x480")
    image.magick( "RGBA" );
    image.read( blob);

Construct An Image
------------------

An Image may be constructed in a number of ways. It may be constructed
from a file, a URL, or an encoded image (e.g. JPEG) contained in an
in-memory `Blob`_ . The following Image constructors and assignment
operators are available:

Construct from image file or image specification::

    Image( const std::string &imageSpec_ )
    
Construct a blank image canvas of specified size and `color`_::

    Image( const Geometry &size_, const Color &color_ )

Construct Image from in-memory `Blob`_::

    Image ( const Blob &blob_ )

Construct Image of specified size from in-memory `Blob`_::

    Image ( const Blob &blob_, const Geometry &size_ )

Construct Image of specified size and depth from in-memory `Blob`_::

    Image ( const Blob &blob_, const Geometry &size,
            const unsigned int depth )

Construct Image of specified size, depth, and format from in-memory `Blob`_::

    Image ( const Blob &blob_, const Geometry &size,
            const unsigned int depth_,
	    const std::string &magick_ )

Construct Image of specified size, and format from in-memory `Blob`_::

    Image ( const Blob &blob_, const Geometry &size,
            const std::string &magick_ )

Construct an image based on an array of raw pixels, of specified type
and mapping, in memory::

    Image ( const unsigned int width_,
            const unsigned int height_,
            const std::string &map_,
            const StorageType type_,
            const void *pixels_ )

Default constructor::

    Image( void )

Copy constructor::

    Image ( const Image & image_ )
    
Assignment operator::

    Image& operator= ( const Image &image_ )

Read Or Write An Image
----------------------

.. contents::
   :local:

ping
++++

Ping is similar to read_ except only enough of the image is read to
determine the image columns, rows, and filesize.  Access the
columns(), rows(), and fileSize() attributes after invoking ping.  The
image pixels are not valid after calling ping::

    void            ping ( const std::string &imageSpec_ )
    
Ping is similar to read except only enough of the image is read
to determine the image columns, rows, and filesize.  Access the
columns(), rows(), and fileSize() attributes after invoking
ping.  The image pixels are not valid after calling ping::

    void            ping ( const Blob &blob_ )

read
++++
    
Read single image frame into current object.  Use ping_ instead if you
want to obtain the basic attributes of the image without reading the
whole file/blob::

    void            read ( const std::string &imageSpec_ )

Read single image frame of specified size into current object::

    void            read ( const Geometry &size_,
			   const std::string &imageSpec_ )

Read single image frame from in-memory `Blob`_::

    void            read ( const Blob        &blob_ )

Read single image frame of specified size from in-memory `Blob`_::

    void            read ( const Blob        &blob_,
			   const Geometry    &size_ )

Read single image frame of specified size and depth from in-memory
`Blob`_::

    void            read ( const Blob         &blob_,
			   const Geometry     &size_,
			   const unsigned int depth_ )

Read single image frame of specified size, depth, and format from
in-memory `Blob`_::

    void            read ( const Blob         &blob_,
			   const Geometry     &size_,
			   const unsigned int depth_,
			   const std::string  &magick_ )

Read single image frame of specified size, and format from in-memory
`Blob`_::

    void            read ( const Blob         &blob_,
			   const Geometry     &size_,
			   const std::string  &magick_ )

Read single image frame from an array of raw pixels, with
specified storage type (ConstituteImage), e.g.
``image.read( 640, 480, "RGB", 0, pixels )``::

    void            read ( const unsigned int width_,
                           const unsigned int height_,
                           const std::string &map_,
                           const StorageType  type_,
                           const void        *pixels_ )

write
+++++
    
Write single image frame to a file::

    void            write ( const std::string &imageSpec_ )

Write single image frame to in-memory `Blob`_, with optional format and
adjoin parameters::

    void            write ( Blob *blob_ )

    void            write ( Blob *blob_,
			    const std::string &magick_ )

    void            write ( Blob *blob_,
			    const std::string &magick_,
			    const unsigned int depth_ )

Write single image frame to an array of pixels with storage type
specified by user (DispatchImage), e.g.  ``image.write( 0, 0, 640, 1,
"RGB", 0, pixels )``::

    void            write ( const int x_,
                            const int y_,
                            const unsigned int columns_,
                            const unsigned int rows_,
                            const std::string& map_,
                            const StorageType type_,
                            void *pixels_ )


Manipulate An Image
-------------------

Image supports access to all the single-image (versus image-list)
manipulation operations provided by the GraphicsMagick library. If you
must process a multi-image file (such as an animation), the `STL`_
interface , which provides a multi-image abstraction on top of Image,
must be used.

Image manipulation methods are very easy to use.  For example::

    Image image;
    image.read("myImage.tiff");
    image.addNoise(GaussianNoise);
    image.write("myImage.tiff");

adds gaussian noise to the image file "myImage.tiff".

The following image manipulation methods are available:

.. contents::
   :local:

adaptiveThreshold
+++++++++++++++++

Apply adaptive thresholding to the image (see
http://www.dai.ed.ac.uk/HIPR2/adpthrsh.htm). Adaptive thresholding is
useful if the ideal threshold level is not known in advance, or if the
illumination gradient is not constant across the image. Adaptive
thresholding works by evaulating the mean (average) of a pixel region
(size specified by width and height) and using the mean as the
thresholding value. In order to remove residual noise from the
background, the threshold may be adjusted by subtracting a constant
offset (default zero) from the mean to compute the threshold::

    void            adaptiveThreshold ( const unsigned int width,
                                        const unsigned int height,
                                        const unsigned offset = 0 )

addNoise
++++++++

Add noise to image with the specified noise type::

    void            addNoise ( const NoiseType noiseType_ )

addNoiseChannel
+++++++++++++++

Add noise to an image channel with the specified noise type. The
`channel` parameter specifies the channel to add noise to.  The
`noiseType` parameter specifies the type of noise::

    void            addNoiseChannel ( const ChannelType channel_,
                                      const NoiseType noiseType_)

affineTransform
+++++++++++++++

Transform image by specified affine (or free transform) matrix::

    void            affineTransform ( const DrawableAffine &affine )

annotate
++++++++

Annotate image (draw text on image)

Gravity effects text placement in bounding area according to these
rules:

NorthWestGravity
  text bottom-left corner placed at top-left
NorthGravity
  text bottom-center placed at top-center
NorthEastGravity
  text bottom-right corner placed at top-right
WestGravity
  text left-center placed at left-center
CenterGravity
  text center placed at center
EastGravity
  text right-center placed at right-center
SouthWestGravity
  text top-left placed at bottom-left
SouthGravity
  text top-center placed at bottom-center
SouthEastGravity
  text top-right placed at bottom-right

Annotate using specified text, and placement location::

    void            annotate ( const std::string &text_,
			       const Geometry &location_ )

Annotate using specified text, bounding area, and placement gravity::

    void            annotate ( const std::string &text_,
			       const Geometry &boundingArea_,
			       const GravityType gravity_ )

Annotate with text using specified text, bounding area, placement
gravity, and rotation::

    void            annotate ( const std::string &text_,
			       const Geometry &boundingArea_,
			       const GravityType gravity_,
			       const double degrees_ )

Annotate with text (bounding area is entire image) and placement
gravity::

    void            annotate ( const std::string &text_,
			       const GravityType gravity_ )

blur
++++
    
Blur an image with the specified blur factor.

The `radius` parameter specifies the radius of the Gaussian, in
pixels, not counting the center pixel.  The `sigma` parameter
specifies the standard deviation of the Laplacian, in pixels::

    void            blur ( const double radius_ = 0.0,
                           const double sigma_ = 1.0  )

blurChannel
+++++++++++

Blur an image channel with the specified blur factor.

The `channel` parameter specifies the channel to modify. The `radius`
parameter specifies the radius of the Gaussian, in pixels, not
counting the center pixel.  The `sigma` parameter specifies the
standard deviation of the Laplacian, in pixels::

    void            blurChannel ( const ChannelType channel_,
                                  const double radius_ = 0.0,
                                  const double sigma_ = 1.0  )

border
++++++

Border image (add border to image).  The `color`_ of the border is
specified by the borderColor attribute::

    void            border ( const Geometry &geometry_
                             = borderGeometryDefault )

channel
+++++++

Extract channel from image. Use this option to extract a particular
channel from the image.  MatteChannel for example, is useful for
extracting the opacity values from an image::

    void            channel ( const ChannelType channel_ )

channelDepth
++++++++++++

Set or obtain modulus channel depth::

    void            channelDepth ( const ChannelType channel_,
                                   const unsigned int depth_ )

    unsigned int    channelDepth ( const ChannelType channel_ )

charcoal
++++++++

Charcoal effect image (looks like charcoal sketch).

The `radius` parameter specifies the radius of the Gaussian, in
pixels, not counting the center pixel.  The `sigma` parameter
specifies the standard deviation of the Laplacian, in pixels::

    void            charcoal ( const double radius_ = 0.0,
                               const double sigma_ = 1.0 )

chop
++++

Chop image (remove vertical or horizontal subregion of image)::

    void            chop ( const Geometry &geometry_ )

colorize
++++++++

Colorize image with pen `color`_, using specified percent opacity for
red, green, and blue quantums::

    void            colorize ( const unsigned int opacityRed_,
                               const unsigned int opacityGreen_,
                               const unsigned int opacityBlue_,
			       const Color &penColor_ )

Colorize image with pen `color`_, using specified percent opacity::

    void            colorize ( const unsigned int opacity_,
			       const Color &penColor_ )

comment
+++++++
    
Comment image (add comment string to image). By default, each image is
commented with its file name. Use this method to assign a specific
comment to the image.  Optionally you can include the image filename,
type, width, height, or other image attributes by embedding `special
format characters`_::

    void            comment ( const std::string &comment_ )

compare
+++++++

Compare current image with another image.  Sets meanErrorPerPixel,
normalizedMaxError, and normalizedMeanError in the current
image. False is returned if the images are identical. An ErrorOption
exception is thrown if the reference image columns, rows, colorspace,
or matte differ from the current image::

    bool            compare ( const Image &reference_ )

composite
+++++++++

Compose an image onto another at specified x and y offset and using a
specified algorithm::

    void            composite ( const Image &compositeImage_,
				const int xOffset_,
				const int yOffset_,
				const CompositeOperator compose_
                                = InCompositeOp )

    void            composite ( const Image &compositeImage_,
				const Geometry &offset_,
				const CompositeOperator compose_
                                = InCompositeOp )

    void            composite ( const Image &compositeImage_,
				const GravityType gravity_,
				const CompositeOperator compose_
                                = InCompositeOp )

contrast
++++++++
    
Contrast image (enhance intensity differences in image)::

    void            contrast ( const unsigned int sharpen_ )

convolve
++++++++

Convolve image.  Applies a user-specified convolution to the image.
The `order` parameter represents the number of columns and rows in the
filter kernel while `kernel` is a two-dimensional array of doubles
representing the convolution kernel to apply::

    void            convolve ( const unsigned int order_,
                               const double *kernel_ )

crop
++++

Crop image (return subregion of original image)::

    void            crop ( const Geometry &geometry_ )

cycleColormap
+++++++++++++
    
Cycle (rotate) image colormap::

    void            cycleColormap ( const int amount_ )

despeckle
+++++++++

Despeckle image (reduce speckle noise)::

    void            despeckle ( void )

display
+++++++

Display image on screen. Caution: if an image format is is not
compatible with the display visual (e.g. JPEG on a colormapped
display) then the original image will be altered. Use a copy of the
original if this is a problem:

    void            display ( void )

draw
++++
    
Draw shape or text on image using a single `drawable`_ object::

    void            draw ( const Drawable &drawable_ );

Draw shapes or text on image using a set of `Drawable`_ objects
contained in an `STL`_ list. Use of this method improves drawing
performance and allows batching draw objects together in a list for
repeated use::

    void            draw ( const std::list<Magick::Drawable> &drawable_ );

edge
++++

Edge image (hilight edges in image).  The radius is the radius of the
pixel neighborhood.. Specify a radius of zero for automatic radius
selection::

    void            edge ( const double radius_ = 0.0 )

emboss
++++++
    
Emboss image (hilight edges with 3D effect).  The `radius` parameter
specifies the radius of the Gaussian, in pixels, not counting the
center pixel.  The `sigma` parameter specifies the standard deviation
of the Laplacian, in pixels::

    void            emboss ( const double radius_ = 0.0,
                             const double sigma_ = 1.0)

enhance
+++++++
    
Enhance image (minimize noise)::

    void            enhance ( void );

equalize
++++++++
    
Equalize image (histogram equalization)::

    void            equalize ( void )

erase
+++++

Set all image pixels to the current background color::

    void            erase ( void )

flip
++++
    
Flip image (reflect each scanline in the vertical direction)::

    void            flip ( void )

floodFillColor
++++++++++++++

Flood-fill `color`_ across pixels that match the `color`_ of the target
pixel and are neighbors of the target pixel.  Uses current fuzz
setting when determining `color`_ match::

    void            floodFillColor( const unsigned int x_,
                                    const unsigned int y_,
				    const Color &fillColor_ )

    void            floodFillColor( const Geometry &point_,
				    const Color &fillColor_ )

Flood-fill `color`_ across pixels starting at target-pixel and stopping
at pixels matching specified border `color`_.  Uses current fuzz setting
when determining `color`_ match::

    void            floodFillColor( const unsigned int x_,
                                    const unsigned int y_,
				    const Color &fillColor_,
				    const Color &borderColor_ )

    void            floodFillColor( const Geometry &point_,
				    const Color &fillColor_,
				    const Color &borderColor_ )

floodFillOpacity
++++++++++++++++

Flood-fill pixels matching `color`_ (within fuzz factor) of target
pixel(x,y) with replacement opacity value using method::

    void            floodFillOpacity ( const unsigned int x_,
                                       const unsigned int y_,
                                       const unsigned int opacity_,
                                       const PaintMethod method_ )

floodFillTexture
++++++++++++++++

Flood-fill texture across pixels that match the `color`_ of the
target pixel and are neighbors of the target pixel.
Uses current fuzz setting when determining `color`_ match::

    void            floodFillTexture( const unsigned int x_,
                                      const unsigned int y_,
				      const Image &texture_ )

    void            floodFillTexture( const Geometry &point_,
				      const Image &texture_ )

Flood-fill texture across pixels starting at target-pixel and
stopping at pixels matching specified border `color`_.
Uses current fuzz setting when determining `color`_ match::

    void            floodFillTexture( const unsigned int x_,
                                      const unsigned int y_,
				      const Image &texture_,
				      const Color &borderColor_ )

    void            floodFillTexture( const Geometry &point_,
				      const Image &texture_,
				      const Color &borderColor_ )

flop
++++

Flop image (reflect each scanline in the horizontal direction)::

    void            flop ( void );

frame
+++++
    
Draw a decorative frame around the image::

    void            frame ( const Geometry &geometry_ = frameGeometryDefault )

    void            frame ( const unsigned int width_,
                            const unsigned int height_,
			    const int innerBevel_ = 6,
                            const int outerBevel_ = 6 )

gamma
+++++
    
Gamma correct the image or individual image channels::

    void            gamma ( const double gamma_ )

    void            gamma ( const double gammaRed_,
			    const double gammaGreen_,
			    const double gammaBlue_ )

gaussianBlur
++++++++++++

Gaussian blur image.  The number of neighbor pixels to be included in
the convolution mask is specified by `width`. The standard deviation
of the gaussian bell curve is specified by `sigma`::

    void            gaussianBlur ( const double width_, const double sigma_ )

gaussianBlurChannel
+++++++++++++++++++

Gaussian blur image channel.  The number of neighbor pixels to be
included in the convolution mask is specified by `width`. The
standard deviation of the gaussian bell curve is specified by
`sigma`::

    void            gaussianBlurChannel ( const ChannelType channel_,
                                          const double width_,
                                          const double sigma_ )

implode
+++++++
    
Implode image (special effect)::

    void            implode ( const double factor_ )

label
+++++
    
Assign a label to an image. Use this option to assign a specific label
to the image. Optionally you can include the image filename, type,
width, height, or scene number in the label by embedding `special
format characters`_. If the first character of string is @, the image
label is read from a file titled by the remaining characters in the
string. When converting to Postscript, use this option to specify a
header string to print above the image::

    void            label ( const std::string &label_ )

level
+++++

Level image to increase image contrast, and/or adjust image
gamma. Adjust the levels of the image by scaling the colors falling
between specified white and black points to the full available quantum
range. The parameters provided represent the black, mid (gamma), and
white points.  The black point specifies the darkest color in the
image. Colors darker than the black point are set to zero. Mid point
(gamma) specifies a gamma correction to apply to the image. White
point specifies the lightest color in the image.  Colors brighter than
the white point are set to the maximum quantum value. The black and
white point have the valid range 0 to MaxRGB while mid (gamma) has a
useful range of 0 to ten::

    void            level ( const double black_point,
                            const double white_point,
                            const double mid_point=1.0 )

levelChannel
++++++++++++

Level image channel to increase image contrast, and/or adjust image
gamma. Adjust the levels of the image channel by scaling the colors
falling between specified white and black points to the full available
quantum range. The parameters provided represent the black, mid
(gamma), and white points.  The black point specifies the darkest
color in the image. Colors darker than the black point are set to
zero. Mid point (gamma) specifies a gamma correction to apply to the
image. White point specifies the lightest color in the image.  Colors
brighter than the white point are set to the maximum quantum
value. The black and white point have the valid range 0 to MaxRGB
while mid (gamma) has a useful range of 0 to ten::

    void            levelChannel ( const ChannelType channel,
                                   const double black_point,
                                   const double white_point,
                                   const double mid_point=1.0 )

magnify
+++++++

Magnify image by integral size (double the dimensions)::

    void            magnify ( void )

map
+++
    
Remap image colors with closest color from a reference image. Set
`dither` to true in to apply Floyd/Steinberg error diffusion to the
image. By default, color reduction chooses an optimal set of colors
that best represent the original image. Alternatively, you can choose
a particular set of colors from an image file with this option::

    void            map ( const Image &mapImage_ ,
                          const bool dither_ = false )

matteFloodfill
++++++++++++++
    
Floodfill designated area with a replacement opacity value::

    void            matteFloodfill ( const Color &target_ ,
				     const unsigned int opacity_,
				     const int x_, const int y_,
				     const PaintMethod method_ )

medianFilter
++++++++++++

Filter image by replacing each pixel component with the median color
in a circular neighborhood::

    void            medianFilter ( const double radius_ = 0.0 )

minify
++++++
    
Reduce image by integral (half) size::

    void            minify ( void )

modifyImage
+++++++++++

Prepare to update image (copy if reference > 1). Normally Magick++'s
implicit reference counting takes care of all instance management.  In
the rare case that the automatic instance management does not work,
use this method to assure that there is only one reference to the
image to be modified.  It should be used in the cases where a
GraphicsMagick C function is used directly on an image which may have
multiple references::

    void            modifyImage ( void )

modulate
++++++++
    
Modulate percent hue, saturation, and brightness of an image.
Modulation of saturation and brightness is as a ratio of the current
value (1.0 for no change). Modulation of hue is an absolute rotation
of -180 degrees to +180 degrees from the current position
corresponding to an argument range of 0 to 2.0 (1.0 for no change)::

    void            modulate ( const double brightness_,
			       const double saturation_,
			       const double hue_ )

motionBlur
++++++++++

Motion blur image with specified blur factor. The `radius` parameter
specifies the radius of the Gaussian, in pixels, not counting the
center pixel.  The `sigma` parameter specifies the standard
deviation of the Laplacian, in pixels.  The `angle` parameter
specifies the angle the object appears to be comming from (zero
degrees is from the right)::

    void            motionBlur ( const double radius_,
                                 const double sigma_,
                                 const double angle_ )

negate
++++++
    
Negate colors in image.  Set `grayscale` to only negate grayscale
values in image::

    void            negate ( const bool grayscale_ = false )

normalize
+++++++++
    
Normalize image (increase contrast by normalizing the pixel values to
span the full range of color values)::

    void            normalize ( void )

oilPaint
++++++++
    
Oilpaint image (image looks like an oil painting)::

    void            oilPaint ( const double radius_ = 3.0 )

opacity
+++++++

Set or attenuate the opacity channel in the image. If the image pixels
are opaque then they are set to the specified opacity value, otherwise
they are blended with the supplied opacity value.  The value of
`opacity` ranges from 0 (completely opaque) to MaxRGB. The defines
`OpaqueOpacity` and `TransparentOpacity` are available to specify
completely opaque or completely transparent, respectively::

    void            opacity ( const unsigned int opacity_ )

opaque
++++++

Change `color`_ of specified opaque pixel to specified pen `color`_::

    void            opaque ( const Color &opaqueColor_,
			     const Color &penColor_ )

quantize
++++++++

Quantize image (reduce number of colors). Set `measureError` to true
in order to calculate error attributes::

    void            quantize ( const bool measureError_ = false )

quantumOperator
+++++++++++++++

Apply an arithmetic or bitwise operator to the image pixel quantums::

    void            quantumOperator ( const ChannelType channel_,
                                      const QuantumOperator operator_,
                                      double rvalue_)

    void            quantumOperator ( const int x_,const int y_,
                                      const unsigned int columns_,
                                      const unsigned int rows_,
                                      const ChannelType channel_,
                                      const QuantumOperator operator_,
                                      const double rvalue_)

process
+++++++

Execute a named process module using an argc/argv syntax similar to
that accepted by a C 'main' routine. An exception is thrown if the
requested process module doesn't exist, fails to load, or fails during
execution::

    void            process ( std::string name_,
                              const int argc_,
                              char **argv_ )

raise
+++++

Raise image (lighten or darken the edges of an image to give a 3-D
raised or lowered effect)::

    void            raise ( const Geometry &geometry_ = "6x6+0+0",
			    const bool raisedFlag_ = false )

randomThreshold
+++++++++++++++

Random threshold image.

Changes the value of individual pixels based on the intensity
of each pixel compared to a random threshold.  The result is a
low-contrast, two color image.  The `thresholds` argument is a
geometry containing LOWxHIGH thresholds.  If the string
contains 2x2, 3x3, or 4x4, then an ordered dither of order 2,
3, or 4 will be performed instead.  If a `channel` argument is
specified then only the specified channel is altered.  This is
a very fast alternative to 'quantize' based dithering::

    void            randomThreshold( const Geometry &thresholds_ )

randomThresholdChannel
++++++++++++++++++++++

Random threshold image channel.

Changes the value of individual pixels based on the intensity of each
pixel compared to a random threshold.  The result is a low-contrast,
two color image.  The `thresholds` argument is a geometry containing
LOWxHIGH thresholds.  If the string contains 2x2, 3x3, or 4x4, then an
ordered dither of order 2, 3, or 4 will be performed instead.  If a
`channel` argument is specified then only the specified channel is
altered.  This is a very fast alternative to 'quantize' based
dithering::

    void            randomThresholdChannel( const Geometry &thresholds_,
                                            const ChannelType channel_ )


reduceNoise
+++++++++++

Reduce noise in image using a noise peak elimination filter::

    void            reduceNoise ( void )

    void            reduceNoise ( const double order_ )

roll
++++
    
Roll image (rolls image vertically and horizontally) by specified
number of columnms and rows)::

    void            roll ( const Geometry &roll_ )

    void            roll ( const unsigned int columns_,
			   const unsigned int rows_ )

rotate
++++++
    
Rotate image counter-clockwise by specified number of degrees::

    void            rotate ( const double degrees_ )

sample
++++++
    
Resize image by using pixel sampling algorithm::

    void            sample ( const Geometry &geometry_ )

scale
+++++
    
Resize image by using simple ratio algorithm which provides good
quality::

    void            scale ( const Geometry &geometry_ )

segment
+++++++
    
Segment (coalesce similar image components) by analyzing the
histograms of the color components and identifying units that are
homogeneous with the fuzzy c-means technique.  A histogram is built
for the image.  This histogram is filtered to reduce noise and a
second derivative of the histogram plot is built and used to identify
potential cluster colors (peaks in the histogram).  The cluster colors
are then validated by scanning through all of the pixels to see how
many pixels fall within each cluster.  Some candidate cluster colors
may not match any of the image pixels at all and should be discarded.
Specify `clusterThreshold`, as the number of pixels matching a cluster
color in order for the cluster to be considered
valid. `SmoothingThreshold` eliminates noise in the second derivative
of the histogram. As the value is increased, you can expect a smoother
second derivative.  The default is 1.5::

    void            segment ( const double clusterThreshold_ = 1.0, 
			      const double smoothingThreshold_ = 1.5 )

shade
+++++
    
Shade image using distant light source. Specify `azimuth` and
`elevation` as the position of the light source. By default, the
shading results as a grayscale image.. Set `colorShading` to true to
shade the red, green, and blue components of the image::

    void            shade ( const double azimuth_ = 30,
			    const double elevation_ = 30,
			    const bool   colorShading_ = false )

sharpen
+++++++
    
Sharpen pixels in image.  The `radius` parameter specifies the radius
of the Gaussian, in pixels, not counting the center pixel.  The
`sigma` parameter specifies the standard deviation of the Laplacian,
in pixels::

    void            sharpen ( const double radius_ = 0.0,
                              const double sigma_ = 1.0 )

sharpenChannel
++++++++++++++

Sharpen pixels in image channel.  The `radius` parameter specifies the
radius of the Gaussian, in pixels, not counting the center pixel.  The
`sigma` parameter specifies the standard deviation of the Laplacian,
in pixels::

    void            sharpenChannel ( const ChannelType channel_,
                                     const double radius_ = 0.0,
                                     const double sigma_ = 1.0 )

shave
+++++

Shave pixels from image edges::

    void            shave ( const Geometry &geometry_ )

shear
+++++
    
Shear image (create parallelogram by sliding image by X or Y
axis). Shearing slides one edge of an image along the X or Y axis,
creating a parallelogram.  An X direction shear slides an edge along
the X axis, while a Y direction shear slides an edge along the Y axis.
The amount of the shear is controlled by a shear angle.  For X
direction shears, x degrees is measured relative to the Y axis, and
similarly, for Y direction shears y degrees is measured relative to
the X axis. Empty triangles left over from shearing the image are
filled with the `color`_ defined as borderColor::

    void            shear ( const double xShearAngle_,
			    const double yShearAngle_ )

solarize
++++++++
    
Solarize image (similar to effect seen when exposing a photographic
film to light during the development process)::

    void            solarize ( const double factor_ = 50.0 )

spread
++++++
    
Spread pixels randomly within image by specified ammount::

    void            spread ( const unsigned int amount_ = 3 )

stegano
+++++++
    
Add a digital watermark to the image (based on second image)::

    void            stegano ( const Image &watermark_ )

stereo
++++++
    
Create an image which appears in stereo when viewed with red-blue
glasses (Red image on left, blue on right)::

    void            stereo ( const Image &rightImage_ )

swirl
+++++
    
Swirl image (image pixels are rotated by degrees)::

    void            swirl ( const double degrees_ )

texture
+++++++
    
Channel a texture on pixels matching image background `color`_::

    void            texture ( const Image &texture_ )

threshold
+++++++++
    
Threshold image channels (below threshold becomes black, above
threshold becomes white).  The range of the threshold parameter is 0
to MaxRGB::

    void            threshold ( const double threshold_ )

transform
+++++++++
    
Transform image based on image and crop geometries. Crop geometry is
optional::

    void            transform ( const Geometry &imageGeometry_ )

    void            transform ( const Geometry &imageGeometry_,
				const Geometry &cropGeometry_  )

transparent
+++++++++++

Add matte channel to image, setting pixels matching `color`_ to
transparent::

    void            transparent ( const Color &color_ )

trim
++++
    
Trim edges that are the background `color`_ from the image::

    void            trim ( void )

type
++++

Convert the image representation to the specified type or retrieve the
current image type.  If the image is reduced to an inferior type, then
image information may be lost (e.g. color changed to grayscale).

Available enumerations for the `type` parameter:

  BilevelType
    black/white
  GrayscaleType
    grayscale
  GrayscaleMatteType
    grayscale with alpha (opacity) channel
  PaletteType
    colormapped
  PaletteMatteType
    colormapped with transparency
  TrueColorType
    true (full) color
  TrueColorMatteType
    true (full) color with alpha (opacity) channel
  ColorSeparationType
    Cyan, magenta, yellow, and black
  ColorSeparationMatteType
    Cyan, magenta, yellow, and black with alpha (opacity) channel
  OptimizeType
    Optimize the image type to best represent the existing pixels

::

    void            type ( const ImageType type_ )

    ImageType       type ( void ) const

unsharpmask
+++++++++++

Replace image with a sharpened version of the original image using the
unsharp mask algorithm.

  `radius`
    the radius of the Gaussian, in pixels, not counting the
    center pixel.
  `sigma`
    the standard deviation of the Gaussian, in pixels.
  `amount`
    the percentage of the difference between the original and
    the blur image that is added back into the original.
  `threshold`
     the threshold in pixels needed to apply the diffence amount.

::

    void            unsharpmask ( const double radius_,
                                  const double sigma_,
                                  const double amount_,
                                  const double threshold_ )

unsharpmaskChannel
++++++++++++++++++

Replace image channel with a sharpened version of the original image
using the unsharp mask algorithm.

  `channel`
    image channel to modify.
  `radius`
    the radius of the Gaussian, in pixels, not counting the
    center pixel.
  `sigma`
    the standard deviation of the Gaussian, in pixels.
  `amount`
    the percentage of the difference between the original and
    the blur image that is added back into the original.
  `threshold`
     the threshold in pixels needed to apply the diffence amount.

::

    void            unsharpmaskChannel ( const ChannelType channel_,
                                         const double radius_,
                                         const double sigma_,
                                         const double amount_,
                                         const double threshold_ );

wave
++++

Map image pixels to a sine wave::

    void            wave ( const double amplitude_ = 25.0,
                           const double wavelength_ = 150.0 )

zoom
++++
    
Zoom (resize) image to specified size::

    void            zoom ( const Geometry &geometry_ )


Set/Get Image Attributes
------------------------

Image attributes are set and obtained via methods in Image. Except for
methods which accept pointer arguments (e.g. chromaBluePrimary) all
methods return attributes by value.

Image attributes are easily used. For example, to set the resolution
of the TIFF file "file.tiff" to 150 dots-per-inch (DPI) in both the
horizontal and vertical directions, you can use the following example
code::

    string filename("file.tiff");
    Image image;
    image.read(filename);
    image.resolutionUnits(PixelsPerInchResolution);
    image.density(Geometry(150,150));   // could also use image.density("150x150")
    image.write(filename)

The following image attribute methods are available:

.. contents::
   :local:

adjoin
++++++

Join images into a single multi-image file::

    void            adjoin ( const bool flag_ )

    bool            adjoin ( void ) const

antiAlias
+++++++++
    
Control antialiasing of rendered Postscript and Postscript or TrueType
fonts. Enabled by default::

    void            antiAlias( const bool flag_ )

    bool            antiAlias( void )

animationDelay
++++++++++++++
    
Time in 1/100ths of a second (0 to 65535) which must expire before
displaying the next image in an animated sequence. This option is
useful for regulating the animation of a sequence of GIF images within
Netscape::

    void            animationDelay ( const unsigned int delay_ )

    unsigned int    animationDelay ( void ) const

animationIterations
+++++++++++++++++++
    
Number of iterations to loop an animation (e.g. Netscape loop
extension) for::

    void            animationIterations ( const unsigned int iterations_ )

    unsigned int    animationIterations ( void ) const

attribute
+++++++++

Access an arbitrary named image attribute. Any number of named
attributes may be attached to the image. For example, the image
comment is a named image attribute with the name "comment". EXIF tags
are attached to the image as named attributes. Use the syntax
"EXIF:<tag>" to request an EXIF tag similar to "EXIF:DateTime"::

    void            attribute ( const std::string name_,
                                const std::string value_ )

    std::string     attribute ( const std::string name_ )

backgroundColor
+++++++++++++++
    
Image background `color`_::

    void            backgroundColor ( const Color &color_ )

    Color           backgroundColor ( void ) const

backgroundTexture
+++++++++++++++++
    
Image file name to use as the background texture. Does not modify
image pixels::

    void            backgroundTexture (const std::string &backgroundTexture_ )

    std::string     backgroundTexture ( void ) const

baseColumns
+++++++++++
    
Base image width (before transformations)::

    unsigned int    baseColumns ( void ) const

baseFilename
++++++++++++
    
Base image filename (before transformations)::

    std::string     baseFilename ( void ) const

baseRows
++++++++

Base image height (before transformations)::

    unsigned int    baseRows ( void ) const

borderColor
+++++++++++
    
Image border `color`_::

    void            borderColor ( const Color &color_ )

    Color           borderColor ( void ) const

boundingBox
+++++++++++

Return smallest bounding box enclosing non-border pixels. The
current fuzz value is used when discriminating between pixels.
This is the crop bounding box used by ``crop(Geometry(0,0))``::

    Geometry        boundingBox ( void ) const

boxColor
++++++++
    
Base `color`_ that annotation text is rendered on (default none)::

    void            boxColor ( const Color &boxColor_ )

    Color           boxColor ( void ) const

cacheThreshold
++++++++++++++

Pixel cache threshold in megabytes.  Once this memory threshold is
exceeded, all subsequent pixels cache operations are to/from disk.
This setting is shared by all Image objects::

    static void     cacheThreshold ( const unsigned int threshold_ )

chromaBluePrimary
+++++++++++++++++
    
Chromaticity blue primary point (e.g. x=0.15, y=0.06)::

    void            chromaBluePrimary ( const double x_, const double y_ )

    void            chromaBluePrimary ( double *x_, double *y_ ) const

chromaGreenPrimary
++++++++++++++++++
    
Chromaticity green primary point (e.g. x=0.3, y=0.6)::

    void            chromaGreenPrimary ( const double x_, const double y_ )

    void            chromaGreenPrimary ( double *x_, double *y_ ) const

chromaRedPrimary
++++++++++++++++
    
Chromaticity red primary point (e.g. x=0.64, y=0.33)::

    void            chromaRedPrimary ( const double x_, const double y_ )

    void            chromaRedPrimary ( double *x_, double *y_ ) const

chromaWhitePoint
++++++++++++++++
    
Chromaticity white point (e.g. x=0.3127, y=0.329)::

    void            chromaWhitePoint ( const double x_, const double y_ )
    void            chromaWhitePoint ( double *x_, double *y_ ) const

classType
+++++++++
    
Image class (DirectClass or PseudoClass).  NOTE: setting a DirectClass
image to PseudoClass will result in the loss of color information if
the number of colors in the image is greater than the maximum palette
size (either 256 or 65536 entries depending on the value of
QuantumDepth when ImageMagick was built)::

    void            classType ( const ClassType class_ )

    ClassType       classType ( void ) const

clipMask
++++++++

Associate a clip mask image with the current image. The clip mask
image must have the same dimensions as the current image or an
exception is thrown. Clipping occurs wherever pixels are transparent
in the clip mask image. Clipping Pass an invalid image to unset an
existing clip mask::

    void            clipMask ( const Image & clipMask_ )

    Image           clipMask ( void  ) const

colorFuzz
+++++++++
    
Colors within this distance are considered equal. A number of
algorithms search for a target color. By default the color must be
exact. Use this option to match colors that are close to the target
color in RGB space::

    void            colorFuzz ( const double fuzz_ )

    double          colorFuzz ( void ) const

colorMap
++++++++
    
`Color`_ at colormap position `index`::

    void            colorMap ( const unsigned int index_,
                               const Color &color_ )

    Color           colorMap ( const unsigned int index_ ) const

colorMapSize
++++++++++++

Number of entries in the colormap. Setting the colormap size may
extend or truncate the colormap. The maximum number of supported
entries is specified by the MaxColormapSize constant, and is dependent
on the value of QuantumDepth when GraphicsMagick is compiled. An
exception is thrown if more entries are requested than may be
supported. Care should be taken when truncating the colormap to ensure
that the image colormap indexes reference valid colormap entries::

    void            colorMapSize ( const unsigned int entries_ )

    unsigned int    colorMapSize ( void )

colorSpace
++++++++++

The colorspace (e.g. CMYK) used to represent the image pixel colors::

    void            colorSpace( const ColorspaceType colorSpace_ )

    ColorspaceType  colorSpace ( void ) const

columns
+++++++

Image width::

    unsigned int    columns ( void ) const

comment
+++++++
    
Image comment::

    std::string     comment ( void ) const

compose
+++++++

Composition operator to be used when composition is implicitly
used (such as for image flattening)::

    void            compose (const CompositeOperator compose_)

    CompositeOperator compose ( void ) const

compressType
++++++++++++
    
Image compresion type. The default is the compression type of the
input image file::

    void            compressType ( const CompressionType compressType_ )

    CompressionType compressType ( void ) const

debug
+++++

Enable printing of debug messages from GraphicsMagick as it executes::

    void            debug ( const bool flag_ )

    bool            debug ( void ) const

defineValue
+++++++++++

Set or obtain a definition string to applied when encoding or decoding
the specified format. The meanings of the definitions are format
specific. The format is designated by the `magick` argument, the
format-specific key is designated by `key`, and the associated value
is specified by `value`. See the defineSet() method if the key must be
removed entirely::

    void            defineValue ( const std::string &magick_,
                                  const std::string &key_,
                                  const std::string &value_ )

    std::string     defineValue ( const std::string &magick_,
                                  const std::string &key_ ) const

defineSet
+++++++++

Set or obtain a definition flag to applied when encoding or decoding
the specified format. Similar to the defineValue() method except that
passing the `flag` value 'true' creates a value-less define with that
format and key. Passing the `flag` value 'false' removes any existing
matching definition. The method returns 'true' if a matching key
exists, and 'false' if no matching key exists::

    void            defineSet ( const std::string &magick_,
                                const std::string &key_,
                                bool flag_ )

    bool            defineSet ( const std::string &magick_,
                                const std::string &key_ ) const

density
+++++++

Vertical and horizontal resolution in pixels of the image. This option
specifies an image density when decoding a Postscript or Portable
Document page. Often used with `psPageSize`::

    void            density ( const Geometry &geomery_ )

    Geometry        density ( void ) const

depth
+++++

Image depth (bits allocated to red/green/blue components). Used to
specify the bit depth when reading or writing raw images or when the
output format supports multiple depths. Defaults to the quantum depth
that GraphicsMagick is compiled with::

    void            depth ( const unsigned int depth_ )

    unsigned int    depth ( void ) const

directory
+++++++++

Tile names from within an image montage::

    std::string     directory ( void ) const

endian
++++++

Endianness (`LSBEndian` like Intel, `MSBEndian` like SPARC, or
`NativeEndian` for what this computer uses) for image formats which
support endian-specific options::

    void            endian ( const EndianType endian_ )

    EndianType      endian ( void ) const

fileName
++++++++

Image file name::

    void            fileName ( const std::string &fileName_ )

    std::string     fileName ( void ) const

fileSize
++++++++

Number of bytes of the image on disk::

    off_t          fileSize ( void ) const

fillColor
+++++++++

`Color`_ to use when filling drawn objects::

    void            fillColor ( const Color &fillColor_ )

    Color           fillColor ( void ) const

fillPattern
+++++++++++

Pattern to use while filling drawn objects::

    void            fillPattern ( const Image &fillPattern_ )

    Image           fillPattern ( void  ) const

fillRule
++++++++

Rule to use when filling drawn objects::

    void            fillRule ( const FillRule &fillRule_ )

    FillRule        fillRule ( void ) const

filterType
++++++++++

Filter to use when resizing image. The reduction filter employed has a
sigificant effect on the time required to resize an image and the
resulting quality. The default filter is Lanczos which has been shown
to produce high quality results when reducing most images::

    void            filterType ( const FilterTypes filterType_ )

    FilterTypes     filterType ( void ) const

font
++++

Text rendering font. If the font is a fully qualified X server font
name, the font is obtained from an X server. To use a TrueType font,
precede the TrueType filename with an @. Otherwise, specify a
Postscript font name (e.g. "helvetica").::

    void            font ( const std::string &font_ )

    std::string     font ( void ) const

fontPointsize
+++++++++++++

Text rendering font point size::

    void            fontPointsize ( const double pointSize_ )

    double          fontPointsize ( void ) const

fontTypeMetrics
+++++++++++++++

Obtain font metrics (see `TypeMetric`_) for text string given current
font, pointsize, and density settings.  This information is necessary
in order to do fancy layout of text::

    void            fontTypeMetrics( const std::string &text_,
                                     TypeMetric *metrics )

format
++++++

Long image format description::

    std::string     format ( void ) const

gamma
+++++

Gamma level of the image.  Gamma is a pow() function which converts
between the linear light representation and the representation for the
computer display.  Most computer images are gamma corrected to 2.2
(1/0.4545) so that each step results in a visually linear step on a
computer or video display::

    double          gamma ( void ) const

geometry
++++++++

Preferred size of the image when encoding::

    Geometry        geometry ( void ) const

gifDisposeMethod
++++++++++++++++

GIF disposal method. This option (specific to the GIF file format) is
used to control how successive frames are rendered (how the preceding
frame is disposed of) when creating a GIF animation::

    void            gifDisposeMethod ( const unsigned int disposeMethod_ )

    unsigned int    gifDisposeMethod ( void ) const

iccColorProfile
+++++++++++++++

ICC color profile. Supplied via a `Blob`_ since Magick++/ and
GraphicsMagick do not currently support formating this data structure
directly.  Specifications are available from the International Color
Consortium for the format of ICC color profiles::

    void            iccColorProfile( const Blob &colorProfile_ )

    Blob            iccColorProfile( void ) const

interlaceType
+++++++++++++

The type of interlacing scheme (default `NoInterlace` ). This option
is used to specify the type of interlacing scheme for raw image
formats such as RGB or YUV. `NoInterlace` means do not interlace,
`LineInterlace` uses scanline interlacing, and `PlaneInterlace` uses
plane interlacing. `PartitionInterlace` is like `PlaneInterlace`
except the different planes are saved to individual files (e.g.
image.R, image.G, and image.B). Use `LineInterlace` or
`PlaneInterlace` to create an interlaced GIF or progressive JPEG
image::

    void            interlaceType ( const InterlaceType interlace_ )

    InterlaceType   interlaceType ( void ) const

iptcProfile
+++++++++++

IPTC profile. Supplied via a `Blob`_ since Magick++ and GraphicsMagick do
not currently support formating this data structure
directly. Specifications are available from the International Press
Telecommunications Council for IPTC profiles::

    void            iptcProfile( const Blob& iptcProfile_ )

    Blob            iptcProfile( void ) const

isValid
+++++++

Does object contain valid image? Set to `false` in order to invalidate
the image. Images constructed via the default constructor are invalid
images and isValid() will return false::

    void            isValid ( const bool isValid_ )

    bool            isValid ( void ) const

label
+++++

Image label::

    std::string     label ( void ) const

lineWidth
+++++++++

Stroke width for drawing vector objects (default one)
This method is now deprecated. Please use strokeWidth instead::

    void            lineWidth ( const double lineWidth_ )

    double          lineWidth ( void ) const

magick
++++++

File type magick identifier (.e.g "GIF")::

    void            magick ( const std::string &magick_ )

    std::string     magick ( void ) const

matte
+++++
    
Image supports transparency (matte channel)::

    void            matte ( const bool matteFlag_ )

    bool            matte ( void ) const

matteColor
++++++++++
    
Image matte (frame) `color`_::

    void            matteColor ( const Color &matteColor_ )

    Color           matteColor ( void ) const

meanErrorPerPixel
+++++++++++++++++
    
The mean error per pixel computed when an image is color reduced. This
parameter is only valid if verbose is set to true and the image has
just been quantized::

    double          meanErrorPerPixel ( void ) const

modulusDepth
++++++++++++

Image modulus depth (minimum number of bits required to support
red/green/blue components without loss of accuracy). The pixel modulus
depth may be decreased by supplying a value which is less than the
current value, updating the pixels (reducing accuracy) to the new
depth. The pixel modulus depth can not be increased over the current
value using this method::

    void            modulusDepth ( const unsigned int modulusDepth_ )

    unsigned int    modulusDepth ( void ) const

monochrome
++++++++++

Transform image to black and white while color reducing (quantizing)::

    void            monochrome ( const bool monochromeFlag_ )

    bool            monochrome ( void ) const

montageGeometry
+++++++++++++++

Tile size and offset within an image montage. Only valid for montage
images::

    Geometry        montageGeometry ( void ) const

normalizedMaxError
++++++++++++++++++

The normalized max error per pixel computed when an image is color
reduced. This parameter is only valid if verbose is set to true and
the image has just been quantized::

    double          normalizedMaxError ( void ) const

normalizedMeanError
+++++++++++++++++++

The normalized mean error per pixel computed when an image is color
reduced. This parameter is only valid if verbose is set to true and
the image has just been quantized::

    double          normalizedMeanError ( void ) const

orientation
+++++++++++

Image orientation.  Supported by some file formats such as DPX and
TIFF. Useful for turning the right way up::

    void            orientation ( const OrientationType orientation_ )

    OrientationType orientation ( void ) const

page
++++

Preferred size and location of an image canvas.

Use this option to specify the dimensions and position of the
Postscript page in dots per inch or a TEXT page in pixels. This option
is typically used in concert with density .

Page may also be used to position a GIF image (such as for a scene in
an animation)::

    void            page ( const Geometry &pageSize_ )

    Geometry        page ( void ) const

pixelColor
++++++++++

Get/set pixel `color`_ at location x & y::

    void            pixelColor ( const unsigned int x_,
                                 const unsigned int y_,
				 const Color &color_ )

    Color           pixelColor ( const unsigned int x_,
                                 const unsigned int y_ ) const

profile
+++++++

Add or remove a named profile to/from the image. Remove the
profile by passing an empty `Blob`_ (e.g. Blob()). Valid names are
"*", "8BIM", "ICM", "IPTC", or a user/format-defined profile name::

    void            profile( const std::string name_,
                             const Blob &colorProfile_ )

Retrieve a named profile from the image. Valid names are:
"8BIM", "8BIMTEXT", "APP1", "APP1JPEG", "ICC", "ICM", & "IPTC"
or an existing user/format-defined profile name::

    Blob            profile( const std::string name_ ) const

quality
+++++++

JPEG/MIFF/PNG compression level (default 75)::

    void            quality ( const unsigned int quality_ )

    unsigned int    quality ( void ) const

quantizeColors
++++++++++++++
    
Maximum number of colors to quantize to::

    void            quantizeColors ( const unsigned int colors_ )

    unsigned int    quantizeColors ( void ) const

quantizeColorSpace
++++++++++++++++++
    
Colorspace to quantize in (default RGB). Empirical evidence suggests
that distances in color spaces such as YUV or YIQ correspond to
perceptual color differences more closely than do distances in RGB
space. These color spaces may give better results when color reducing
an image::

    void            quantizeColorSpace ( const ColorspaceType colorSpace_ )

    ColorspaceType  quantizeColorSpace ( void ) const

quantizeDither
++++++++++++++
    
Apply Floyd/Steinberg error diffusion to the image. The basic strategy
of dithering is to trade intensity resolution for spatial resolution
by averaging the intensities of several neighboring pixels. Images
which suffer from severe contouring when reducing colors can be
improved with this option. The quantizeColors or monochrome option
must be set for this option to take effect::

    void            quantizeDither ( const bool ditherFlag_ )

    bool            quantizeDither ( void ) const

quantizeTreeDepth
+++++++++++++++++

Depth of the quantization color classification tree. Values of 0 or 1
allow selection of the optimal tree depth for the color reduction
algorithm. Values between 2 and 8 may be used to manually adjust the
tree depth::

    void            quantizeTreeDepth ( const unsigned int treeDepth_ )

    unsigned int    quantizeTreeDepth ( void ) const

renderingIntent
+++++++++++++++

The type of rendering intent (used when applying an ICC color
profile)::

    void            renderingIntent ( const RenderingIntent renderingIntent_ )

    RenderingIntent renderingIntent ( void ) const

resolutionUnits
+++++++++++++++

Units of image resolution::

    void            resolutionUnits ( const ResolutionType resolutionUnits_ )

    ResolutionType  resolutionUnits ( void ) const

rows
++++

The number of pixel rows in the image::

    unsigned int    rows ( void ) const

scene
+++++

Image scene number::

    void            scene ( const unsigned int scene_ )

    unsigned int    scene ( void ) const

signature
+++++++++

Image textual signature.  Set `force` to true in order to re-calculate
the signature regardless of whether the image data has been modified::

    std::string     signature ( const bool force_ = false ) const

size
++++

Width and height of a raw image (an image which does not support width
and height information).  Size may also be used to affect the image
size read from a multi-resolution format (e.g. Photo CD, JBIG, or
JPEG::

    void            size ( const Geometry &geometry_ )

    Geometry        size ( void ) const

statistics
++++++++++

Obtain image statistics. Statistics are normalized to the range
of 0.0 to 1.0 and are output to the specified ImageStatistics
structure::

    void            statistics ( ImageStatistics *statistics ) const

strokeAntiAlias
+++++++++++++++

Enable/disable stroke anti-aliasing::

    void            strokeAntiAlias( const bool flag_ )

    bool            strokeAntiAlias( void ) const

strokeColor
+++++++++++

`Color`_ to use when drawing object outlines::

    void            strokeColor ( const Color &strokeColor_ )

    Color           strokeColor ( void ) const

strokeDashArray
+++++++++++++++

Specify the pattern of dashes and gaps used to stroke paths. The
strokeDashArray represents a zero-terminated array of numbers that
specify the lengths of alternating dashes and gaps in pixels. If an
odd number of values is provided, then the list of values is repeated
to yield an even number of values.  A typical `strokeDashArray` array
might contain the members 5 3 2 0, where the zero value indicates the
end of the pattern array::

    void            strokeDashArray ( const double* strokeDashArray_ )

    const double*   strokeDashArray ( void ) const

strokeDashOffset
++++++++++++++++

While drawing using a dash pattern, specify distance into the
dash pattern to start the dash (default 0)::

    void            strokeDashOffset ( const double strokeDashOffset_ )

    double          strokeDashOffset ( void ) const

strokeLineCap
+++++++++++++

Specify the shape to be used at the end of open subpaths when
they are stroked. Values of LineCap are UndefinedCap, ButtCap,
RoundCap, and SquareCap::

    void            strokeLineCap ( const LineCap lineCap_ )

    LineCap         strokeLineCap ( void ) const

strokeLineJoin
++++++++++++++
    
Specify the shape to be used at the corners of paths (or other
vector shapes) when they are stroked. Values of LineJoin are
UndefinedJoin, MiterJoin, RoundJoin, and BevelJoin::

    void            strokeLineJoin ( const LineJoin lineJoin_ )

    LineJoin        strokeLineJoin ( void ) const

strokeMiterLimit
++++++++++++++++

Specify miter limit. When two line segments meet at a sharp
angle and miter joins have been specified for 'lineJoin', it is
possible for the miter to extend far beyond the thickness of
the line stroking the path. The miterLimit' imposes a limit on
the ratio of the miter length to the 'lineWidth'. The default
value of this parameter is 4::

    void            strokeMiterLimit ( const unsigned int miterLimit_ )

    unsigned int    strokeMiterLimit ( void ) const

strokePattern
+++++++++++++

Pattern image to use while stroking object outlines::

    void            strokePattern ( const Image &strokePattern_ )

    Image           strokePattern ( void  ) const

strokeWidth
+++++++++++

Stroke width for drawing vector objects (default one)::

    void            strokeWidth ( const double strokeWidth_ )

    double          strokeWidth ( void ) const

subImage
++++++++

Subimage of an image sequence::

    void            subImage ( const unsigned int subImage_ )

    unsigned int    subImage ( void ) const

subRange
++++++++

Number of images relative to the base image::

    void            subRange ( const unsigned int subRange_ )

    unsigned int    subRange ( void ) const

textEncoding
++++++++++++

Annotation text encoding (e.g. "UTF-16")::

    void            textEncoding ( const std::string &encoding_ )

    std::string     textEncoding ( void ) const

tileName
++++++++

Tile name::

    void            tileName ( const std::string &tileName_ )

    std::string     tileName ( void ) const

totalColors
+++++++++++

Number of colors in the image::

    unsigned long   totalColors ( void )

transformOrigin
+++++++++++++++

Origin of coordinate system to use when annotating with text or drawing::

    void            transformOrigin ( const double x_,const  double y_ )

transformRotation
+++++++++++++++++

Rotation to use when annotating with text or drawing::

    void            transformRotation ( const double angle_ )

transformReset
++++++++++++++

Reset transformation parameters to default::

    void            transformReset ( void )

transformScale
++++++++++++++

Scale to use when annotating with text or drawing::

    void            transformScale ( const double sx_, const double sy_ )

transformSkewX
++++++++++++++

Skew to use in X axis when annotating with text or drawing::

    void            transformSkewX ( const double skewx_ )

transformSkewY
++++++++++++++

Skew to use in Y axis when annotating with text or drawing::

    void            transformSkewY ( const double skewy_ )

verbose
+++++++

Print detailed information about the image::

    void            verbose ( const bool verboseFlag_ )

    bool            verbose ( void ) const

view
++++
    
FlashPix viewing parameters::

    void            view ( const std::string &view_ )

    std::string     view ( void ) const

x11Display
++++++++++

X11 display to display to, obtain fonts from, or to capture
image from::

    void            x11Display ( const std::string &display_ )

    std::string     x11Display ( void ) const

xResolution
+++++++++++

x resolution of the image::

    double          xResolution ( void ) const

yResolution
+++++++++++

y resolution of the image::

    double          yResolution ( void ) const


Low-Level Image Pixel Access
----------------------------

Image pixels (of type `PixelPacket`_ ) may be accessed directly via
the Image Pixel Cache .  The image pixel cache is a rectangular window
into the actual image pixels (which may be in memory, memory-mapped
from a disk file, or entirely on disk). Two interfaces exist to access
the Image Pixel Cache. The interface described here (part of the Image
class) supports only one view at a time. See the `Pixels`_ class for a
more abstract interface which supports simultaneous pixel views (up to
the number of rows). As an analogy, the interface described here
relates to the `Pixels`_ class as stdio's gets() relates to
fgets(). The `Pixels`_ class provides the more general form of the
interface.

Obtain existing image pixels via getPixels(). Create a new pixel
region using setPixels().

In order to ensure that only the current generation of the image is
modified, the Image's modifyImage() method should be invoked to reduce
the reference count on the underlying image to one. If this is not
done, then it is possible for a previous generation of the image to be
modified due to the use of reference counting when copying or
constructing an Image.

Depending on the capabilities of the operating system, and the
relationship of the window to the image, the pixel cache may be a copy
of the pixels in the selected window, or it may be the actual image
pixels. In any case calling syncPixels() insures that the base image
is updated with the contents of the modified pixel cache. The method
readPixels() supports copying foreign pixel data formats into the
pixel cache according to the QuantumTypes. The method writePixels()
supports copying the pixels in the cache to a foreign pixel
representation according to the format specified by QuantumTypes.

The pixel region is effectively a small image in which the pixels may
be accessed, addressed, and updated, as shown in the following
example:

.. |pixel_cache| image:: Cache.png

|pixel_cache|

::

   // Construct image based on an existing file
   Image image("cow.png");

   // Ensure that there are no other references to this image.
   image.modifyImage();

   // Set the image type to TrueColor DirectClass representation.
   image.type(TrueColorType);

   // Request pixel region with size 60x40, and top origin at 20x30
   int columns = 60;
   PixelPacket *pixel_cache = image.getPixels(20,30,columns,40);

   // Set pixel at column 5, and row 10 in the pixel cache to red.
   int column = 5;
   int row = 10;
   PixelPacket *pixel = pixel_cache+row*columns*sizeof(PixelPacket)+column;
   *pixel = Color("red");

   // Save changes to underlying image .
   image.syncPixels();

    // Save updated image to file.
   image.write("horse.png");

The image cache supports the following methods:

.. contents::
   :local:

getConstPixels
++++++++++++++

Transfers read-only pixels from the image to the pixel cache as
defined by the specified region::

    const PixelPacket* getConstPixels ( const int x_, const int y_,
                                        const unsigned int columns_,
                                        const unsigned int rows_ ) const

getIndexes
++++++++++

Obtain mutable image pixel indexes (valid for PseudoClass images)::

    IndexPacket* getIndexes ( void )

getConstIndexes
+++++++++++++++

Obtain immutable image pixel indexes (valid for PseudoClass images)::

    const IndexPacket* getConstIndexes ( void ) const

getPixels
+++++++++

Transfers pixels from the image to the pixel cache as defined by the
specified region. Modified pixels may be subsequently transferred back
to the image via syncPixels.  This method is valid for DirectClass
images::

    PixelPacket* getPixels ( const int x_, const int y_,
			     const unsigned int columns_,
                             const unsigned int rows_ )

setPixels
+++++++++

Allocates a pixel cache region to store image pixels as defined by the
region rectangle.  This area is subsequently transferred from the
pixel cache to the image via syncPixels::

    PixelPacket* setPixels ( const int x_, const int y_,
			     const unsigned int columns_,
                             const unsigned int rows_ )

syncPixels
++++++++++

Transfers the image cache pixels to the image::

    void syncPixels ( void )

readPixels
++++++++++

Transfers one or more pixel components from a buffer or file into the
image pixel cache of an image.  Used to support image decoders::

    void readPixels ( const QuantumType quantum_,
		      const unsigned char *source_ )

writePixels
+++++++++++
    
Transfers one or more pixel components from the image pixel cache to a
buffer or file.  Used to support image encoders::

    void writePixels ( const QuantumType quantum_,
		       unsigned char *destination_ )


.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| `Bob Friesenhahn`_ 1999 - 2009


