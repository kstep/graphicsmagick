// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Implementation of Image
//

#define MAGICK_IMPLEMENTATION

#include <string>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <errno.h>

using namespace std;

#include "Magick++/Image.h"
#include "Magick++/Functions.h"
#include "Magick++/Pixels.h"
#include "Magick++/Include.h"

//
// Friend functions to compare Image objects
//

int Magick::operator == ( const Magick::Image& left_, const Magick::Image& right_ )
{
  // If image pixels and signature are the same, then the image is identical
  return ( ( left_.rows() == right_.rows() ) &&
	   ( left_.columns() == right_.columns() ) &&
	   ( left_.signature() == right_.signature() )
	   );
}
int Magick::operator != ( const Magick::Image& left_, const Magick::Image& right_ )
{
  return ( ! (left_ == right_) );
}
int Magick::operator >  ( const Magick::Image& left_, const Magick::Image& right_ )
{
  return ( !( left_ < right_ ) && ( left_ != right_ ) );
}
int Magick::operator <  ( const Magick::Image& left_, const Magick::Image& right_ )
{
  // If image pixels are less, then image is smaller
  return ( ( left_.rows() * left_.columns() ) <
	   ( right_.rows() * right_.columns() )
	   );
}
int Magick::operator >= ( const Magick::Image& left_, const Magick::Image& right_ )
{
  return ( ( left_ > right_ ) || ( left_ == right_ ) );
}
int Magick::operator <= ( const Magick::Image& left_, const Magick::Image& right_ )
{
  return ( ( left_ < right_ ) || ( left_ == right_ ) );
}

//
// Image object implementation
//

// Construct from image file or image specification
Magick::Image::Image( const std::string &imageSpec_ )
  : _imgRef(new ImageRef)
{
  // Initialize, Allocate and Read images
  read( imageSpec_ );
}

// Construct a blank image canvas of specified size and color
Magick::Image::Image( const Geometry &size_,
		      const Color &color_ )
  : _imgRef(new ImageRef)
{
  // xc: prefix specifies an X11 color string
  std::string imageSpec("xc:");
  imageSpec += color_;

  // Set image size
  size( size_ );

  // Initialize, Allocate and Read images
  read( imageSpec );
}

// Construct Image from in-memory BLOB
Magick::Image::Image ( const Blob &blob_ )
  : _imgRef(new ImageRef)
{
  // Initialize, Allocate and Read images
  read( blob_ );
}

// Construct Image of specified size from in-memory BLOB
Magick::Image::Image ( const Blob &blob_,
		       const Geometry &size_ )
  : _imgRef(new ImageRef)
{
  // Read from Blob
  read( blob_, size_ );
}

// Construct Image of specified size and depth from in-memory BLOB
Magick::Image::Image ( const Blob &blob_,
		       const Geometry &size_,
		       unsigned int depth_ )
  : _imgRef(new ImageRef)
{
  // Read from Blob
  read( blob_, size_, depth_ );
}

// Construct Image of specified size, depth, and format from in-memory BLOB
Magick::Image::Image ( const Blob &blob_,
		       const Geometry &size_,
		       unsigned int depth_,
		       const std::string &magick_ )
  : _imgRef(new ImageRef)
{
  // Read from Blob
  read( blob_, size_, depth_, magick_ );
}

// Construct Image of specified size, and format from in-memory BLOB
Magick::Image::Image ( const Blob &blob_,
		       const Geometry &size_,
		       const std::string &magick_ )
  : _imgRef(new ImageRef)
{
  // Read from Blob
  read( blob_, size_, magick_ );
}

// Default constructor
Magick::Image::Image( void )
  : _imgRef(new ImageRef)
{
}

// Destructor
/* virtual */
Magick::Image::~Image()
{
  bool doDelete = false;
  {
    Lock( _imgRef->_mutexLock );
    if ( --_imgRef->_refCount == 0 )
      doDelete = true;
  }

  if ( doDelete )
    {
      delete _imgRef;
      _imgRef = (ImageRef*)NULL;
    }
}

// Add noise to image
void Magick::Image::addNoise( NoiseType noiseType_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    AddNoiseImage ( image(),
		    noiseType_,
		    &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Annotate image with text
// Text & location
void Magick::Image::annotate ( const std::string &text_,
			       const Geometry &location_ )
{
  annotate ( text_, location_,  NorthWestGravity, (double)0 );
}
// Text, location, & gravity
void Magick::Image::annotate ( const std::string &text_,
			       const Geometry &location_,
			       GravityType gravity_ )
{
  annotate ( text_, location_, gravity_, (double)0 );
}
// Text, location, degrees, gravity
void Magick::Image::annotate ( const std::string &text_,
			       const Geometry &location_,
			       GravityType gravity_,
			       double degrees_ )
{
  modifyImage();

  char geometry[MaxTextExtent];

  AnnotateInfo *annotateInfo
    = options()->annotateInfo();
  
  annotateInfo->text = const_cast<char *>(text_.c_str());

  if ( location_.isValid() ){
    // For some reason width and height parameters in the geometry 
    // causes AnnotateImage to place text incorrectly.
    FormatString(geometry,"%+d%+d", location_.xOff(), location_.yOff() );
    annotateInfo->geometry = geometry;
  }

  annotateInfo->gravity = gravity_;
  annotateInfo->degrees = degrees_;

  AnnotateImage( image(), annotateInfo );

  annotateInfo->text = 0;
  annotateInfo->geometry = 0;

  throwImageException();
}

// Text & gravity
void Magick::Image::annotate ( const std::string &text_,
			       GravityType gravity_ )
{
  modifyImage();

  AnnotateInfo *annotateInfo
    = options()->annotateInfo();

  annotateInfo->text = const_cast<char *>(text_.c_str());

  annotateInfo->gravity = gravity_;
  annotateInfo->degrees = 0.0;

  AnnotateImage( image(), annotateInfo );

  annotateInfo->gravity = NorthWestGravity;
  annotateInfo->text = 0;

  throwImageException();
}

// Blur image
void Magick::Image::blur( unsigned int order_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    BlurImage( image(), order_, &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Add border to image
// Only uses width & height
void Magick::Image::border( const Geometry &geometry_ )
{
  RectangleInfo borderInfo;
  borderInfo.width  = geometry_.width();
  borderInfo.height = geometry_.height();

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    BorderImage( image(), &borderInfo, &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}


// Charcoal-effect image
void Magick::Image::charcoal( unsigned int order_ )
{
  quantizeDither( true );
  quantizeColorSpace( GRAYColorspace );
  quantize( );
  edge( order_ );
  blur( order_ );
  normalize( );
  negate( true );
}

// Chop image
void Magick::Image::chop( const Geometry &geometry_ )
{
  RectangleInfo chopInfo;
  chopInfo.width  = geometry_.width();
  chopInfo.height = geometry_.height();
  chopInfo.x      = geometry_.xOff();
  if ( geometry_.xNegative() )
    chopInfo.x = - chopInfo.x;
  chopInfo.y      = geometry_.yOff();
  if ( geometry_.yNegative() )
    chopInfo.y = - chopInfo.y;

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ChopImage( image(), &chopInfo, &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Colorize
void Magick::Image::colorize ( const Color &opaqueColor_,
			       const Color &penColor_ )
{
  if ( !opaqueColor_.isValid() )
    {
      throwException( OptionError,
		      "Opaque color argument is invalid");
    }

  if ( !penColor_.isValid() )
    {
      throwException( OptionError,
		      "Pen color argument is invalid");
    }

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ColorizeImage ( image(), std::string(opaqueColor_).c_str(),
		    penColor_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Composite two images
void Magick::Image::composite ( const Image &compositeImage_,
				int xOffset_,
				int yOffset_,
				CompositeOperator compose_ )
{
  // Image supplied as compositeImage is composited with current image and
  // results in updating current image.
  modifyImage();

  int x = xOffset_;
  int y = yOffset_;
  unsigned int width = columns();
  unsigned int height = rows();

  Geometry offset( 0, 0, xOffset_, yOffset_ );

  ParseImageGeometry (((std::string)offset).c_str(),
		      &x, &y,
		      &width, &height );

  CompositeImage( image(),
		  compose_,
		  const_cast<Image &>(compositeImage_).image(),
		  x, y );
  throwImageException();
}
void Magick::Image::composite ( const Image &compositeImage_,
				const Geometry &offset_,
				CompositeOperator compose_ )
{
  modifyImage();

  int x = offset_.xOff();
  int y = offset_.yOff();
  unsigned int width = columns();
  unsigned int height = rows();

  ParseImageGeometry (((std::string)offset_).c_str(),
		      &x, &y,
		      &width, &height );

  CompositeImage( image(),
		  compose_,
		  const_cast<Image &>(compositeImage_).image(),
		  x, y );
  throwImageException();
}
void Magick::Image::composite ( const Image &compositeImage_,
				GravityType gravity_,
				CompositeOperator compose_ )
{
  modifyImage();

  int x = 0;
  int y = 0;

  switch (gravity_)
    {
    case NorthWestGravity:
      {
	x = 0;
	y = 0;
	break;
      }
    case NorthGravity:
      {
	x = (columns() - compositeImage_.columns()) >> 1;
	y = 0;
	break;
      }
    case NorthEastGravity:
      {
	x = columns() - compositeImage_.columns();
	y = 0;
	break;
      }
    case WestGravity:
      {
	x = 0;
	y = (rows() - compositeImage_.rows()) >> 1;
	break;
      }
    case ForgetGravity:
    case StaticGravity:
    case CenterGravity:
    default:
      {
	x = (columns() - compositeImage_.columns()) >> 1;
	y = (rows() - compositeImage_.rows()) >> 1;
	break;
      }
    case EastGravity:
      {
	x = columns() - compositeImage_.columns();
	y = (rows() - compositeImage_.rows()) >> 1;
	break;
      }
    case SouthWestGravity:
      {
	x = 0;
	y = rows() - compositeImage_.rows();
	break;
      }
    case SouthGravity:
      {
	x =  (columns() - compositeImage_.columns()) >> 1;
	y = rows() - compositeImage_.rows();
	break;
      }
    case SouthEastGravity:
      {
	x = columns() - compositeImage_.columns();
	y = rows() - compositeImage_.rows();
	break;
      }
    }

  CompositeImage( image(),
		  compose_,
		  const_cast<Image &>(compositeImage_).image(),
		  x, y );
  throwImageException();
}

// Contrast image
void Magick::Image::contrast ( unsigned int sharpen_ )
{
  modifyImage();
  ContrastImage ( image(), sharpen_ );
  throwImageException();
}

// Crop image
void Magick::Image::crop ( const Geometry &geometry_ )
{
  RectangleInfo cropInfo;
  cropInfo.width  = geometry_.width();
  cropInfo.height = geometry_.height();
  cropInfo.x      = geometry_.xOff();
  if ( geometry_.xNegative() )
    cropInfo.x = - cropInfo.x;
  cropInfo.y      = geometry_.yOff();
  if ( geometry_.yNegative() )
    cropInfo.y = - cropInfo.y;

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    CropImage( image(),
	       &cropInfo,
	       &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Cycle Color Map
void Magick::Image::cycleColormap ( int amount_ )
{
  modifyImage();
  CycleColormapImage( image(), amount_ );
  throwImageException();
}

// Despeckle
void Magick::Image::despeckle ( void )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    DespeckleImage( image(), &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Display image
void Magick::Image::display( void )
{
  DisplayImages( imageInfo(), image() );
}

// Draw on image using single drawable
void Magick::Image::draw ( const Drawable &drawable_ )
{
  modifyImage();

  DrawInfo *drawInfo
    = options()->drawInfo();

//   cout << "Primitive:" << drawable_.primitive() << endl;
  drawInfo->primitive = const_cast<char*>(drawable_.primitive().c_str());
  DrawImage( image(), drawInfo );
  drawInfo->primitive = 0;

  throwImageException();
}

// Draw on image using a drawable list
void Magick::Image::draw ( const std::list<Magick::Drawable> &drawable_ )
{
  std::list<Magick::Drawable>::const_iterator p = drawable_.begin();
  std::string primitives;
  while ( p != drawable_.end() )
    {
      if (primitives.length() != 0 )
	primitives.append( " " );
      primitives.append( p->primitive() );
      ++p;
    }
  Magick::Drawable drawable;
  drawable.primitive( primitives );
  draw( drawable );
}

// Hilight edges in image
void Magick::Image::edge ( unsigned int factor_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    EdgeImage( image(), factor_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Emboss image (hilight edges)
void Magick::Image::emboss ( unsigned int order_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    EmbossImage( image(), order_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Enhance image (minimize noise)
void Magick::Image::enhance ( void )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    EnhanceImage( image(), &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Equalize image (histogram equalization)
void Magick::Image::equalize ( void )
{
  modifyImage();
  EqualizeImage( image() );
  throwImageException();
}

// Flip image (reflect each scanline in the vertical direction)
void Magick::Image::flip ( void )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    FlipImage( image(), &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Flood-fill color across pixels that match the color of the
// target pixel and are neighbors of the target pixel.
// Uses current fuzz setting when determining color match.
void Magick::Image::floodFillColor( unsigned int x_, unsigned int y_,
				    const Magick::Color &fillColor_ )
{
  Geometry size(1,1);
  Image fillImage( size, fillColor_ );
  floodFillTexture( x_, y_, fillImage );
}
void Magick::Image::floodFillColor( const Geometry &point_,
				    const Magick::Color &fillColor_ )
{
  Geometry size(1,1);
  Image fillImage( size, fillColor_ );
  floodFillTexture( point_, fillImage );
}

// Flood-fill color across pixels starting at target-pixel and
// stopping at pixels matching specified border color.
// Uses current fuzz setting when determining color match.
void Magick::Image::floodFillColor( unsigned int x_, unsigned int y_,
				    const Magick::Color &fillColor_,
				    const Magick::Color &borderColor_ )
{
  Geometry size(1,1);
  Image fillImage( size, fillColor_ );
  floodFillTexture( x_, y_, fillImage, borderColor_ );
}
void Magick::Image::floodFillColor( const Geometry &point_,
				    const Magick::Color &fillColor_,
				    const Magick::Color &borderColor_ )
{
  Geometry size(1,1);
  Image fillImage( size, fillColor_ );
  floodFillTexture( point_, fillImage, borderColor_ );
}

// Flood-fill texture across pixels that match the color of the
// target pixel and are neighbors of the target pixel.
// Uses current fuzz setting when determining color match.
void Magick::Image::floodFillTexture( unsigned int x_, unsigned int y_,
				      const Magick::Image &texture_ )
{
  modifyImage();

  // Test arguments to ensure they are within the image.
  if ( y_ > rows() || x_ > columns() )
    {
      throwException( OptionError,
		      "Access outside of image boundary" );
    }

  // Get pixel view
  Pixels pixels(*this);
  // Fill image
  PixelPacket *packet = pixels.get(x_, y_, 1, 1 );
  if (packet)
    ColorFloodfillImage ( image(),
			*packet,
			const_cast<Image &>(texture_).image(),
			x_, y_, FloodfillMethod );
  throwImageException();
}
void Magick::Image::floodFillTexture( const Magick::Geometry &point_,
				      const Magick::Image &texture_ )
{
  floodFillTexture( point_.xOff(), point_.yOff(), texture_ );
}

// Flood-fill texture across pixels starting at target-pixel and
// stopping at pixels matching specified border color.
// Uses current fuzz setting when determining color match.
void Magick::Image::floodFillTexture( unsigned int x_, unsigned int y_,
				      const Magick::Image &texture_,
				      const Magick::Color &borderColor_ )
{
  modifyImage();
  PixelPacket target = borderColor_;
  ColorFloodfillImage ( image(), target,
			const_cast<Image &>(texture_).image(),
			x_, y_, FillToBorderMethod );
  throwImageException();
}
void  Magick::Image::floodFillTexture( const Magick::Geometry &point_,
				       const Magick::Image &texture_,
				       const Magick::Color &borderColor_ )
{
  floodFillTexture( point_.xOff(), point_.yOff(), texture_, borderColor_ );
}

// Flop image (reflect each scanline in the horizontal direction)
void Magick::Image::flop ( void )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    FlopImage( image(), &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Frame image
void Magick::Image::frame ( const Geometry &geometry_ )
{
  FrameInfo info;

  info.x           = geometry_.width();
  info.y           = geometry_.height();
  info.width       = columns() + ( info.x << 1 );
  info.height      = rows() + ( info.y << 1 );
  info.outer_bevel = geometry_.xOff();
  info.inner_bevel = geometry_.yOff();

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    FrameImage( image(), &info, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}
void Magick::Image::frame ( unsigned int width_, unsigned int height_,
			    int innerBevel_, int outerBevel_ )
{
  FrameInfo info;
  info.x           = width_;
  info.y           = height_;
  info.width       = columns() + ( info.x << 1 );
  info.height      = rows() + ( info.y << 1 );
  info.outer_bevel = outerBevel_;
  info.inner_bevel = innerBevel_;

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    FrameImage( image(), &info, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Gamma correct image
void Magick::Image::gamma ( double gamma_ )
{
  char gamma[MaxTextExtent + 1];
  FormatString( gamma, "%3.6f", gamma_);

  modifyImage();
  GammaImage ( image(), gamma );
}
void Magick::Image::gamma ( double gammaRed_,
			    double gammaGreen_,
			    double gammaBlue_ )
{
  char gamma[MaxTextExtent + 1];
  FormatString( gamma, "%3.6f/%3.6f/%3.6f/",
		gammaRed_, gammaGreen_, gammaBlue_);

  modifyImage();
  GammaImage ( image(), gamma );
  throwImageException();
}

// Gaussian blur image
// The number of neighbor pixels to be included in the convolution
// mask is specified by 'width_'. The standard deviation of the
// gaussian bell curve is specified by 'sigma_'.
void Magick::Image::gaussianBlur ( double width_, double sigma_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    GaussianBlurImage( image(), width_, sigma_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Implode image
void Magick::Image::implode ( double factor_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ImplodeImage( image(), factor_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}


// Extract layer from image
void Magick::Image::layer ( LayerType layer_ )
{
  modifyImage();
  LayerImage ( image(), layer_ );
  throwImageException();
}

// Magnify image by integral size
void Magick::Image::magnify ( void )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    MagnifyImage( image(), &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Remap image colors with closest color from reference image
void Magick::Image::map ( const Image &mapImage_ , bool dither_ )
{
  modifyImage();
  MapImage ( image(),
			const_cast<MagickLib::Image*>(mapImage_.constImage()),
			dither_ );
  throwImageException();
}

// Floodfill designated area with a matte value
void Magick::Image::matteFloodfill ( const Color &target_ ,
				     unsigned int matte_,
				     int x_, int y_,
				     Magick::PaintMethod method_ )
{
  if ( !target_.isValid() )
    {
      throwException( OptionError,
		      "Target color argument is invalid" );
    }

  modifyImage();

  PixelPacket rllPacket = target_;
  MatteFloodfillImage ( image(), rllPacket, matte_,
				   x_, y_, method_ );
  throwImageException();
}

// Filter image by replacing each pixel component with the median
// color in a circular neighborhood
void Magick::Image::medianFilter ( unsigned int radius_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    MedianFilterImage ( image(), radius_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Reduce image by integral size
void Magick::Image::minify ( void )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    MinifyImage( image(), &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Modulate percent hue, saturation, and brightness of an image
void Magick::Image::modulate ( double brightness_,
			       double saturation_,
			       double hue_ )
{
  char modulate[MaxTextExtent + 1];
  FormatString( modulate, "%3.6f/%3.6f/%3.6f",
		brightness_, saturation_, hue_);

  modifyImage();
  ModulateImage( image(), modulate );
  throwImageException();
}

// Negate image.  Set grayscale_ to true to effect grayscale values
// only
void Magick::Image::negate ( bool grayscale_ )
{
  modifyImage();
  NegateImage ( image(), grayscale_ );
  throwImageException();
}

// Normalize image
void Magick::Image::normalize ( void )
{
  modifyImage();
  NormalizeImage ( image() );
  throwImageException();
}

// Oilpaint image
void Magick::Image::oilPaint ( unsigned int radius_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    OilPaintImage( image(), radius_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Change the color of an opaque pixel to the pen color.
void Magick::Image::opaque ( const Color &opaqueColor_,
			     const Color &penColor_ )
{
  if ( !opaqueColor_.isValid() )
    {
      throwException( OptionError,
		      "Opaque color argument is invalid" );
    }
  if ( !penColor_.isValid() )
    {
      throwException( OptionError,
		      "Pen color argument is invalid" );
    }

  modifyImage();
  OpaqueImage ( image(), opaqueColor_, penColor_ );
  throwImageException();
}

// Ping is similar to read except only enough of the image is read to
// determine the image columns, rows, and filesize.  Access the
// columns(), rows(), and fileSize() attributes after invoking ping.
// The image data is not valid after calling ping.
void Magick::Image::ping ( const std::string &imageSpec_ )
{
  options()->fileName( imageSpec_ );
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* image =
    PingImage( imageInfo(), &exceptionInfo );
  replaceImage( image );
  throwException( exceptionInfo );
}

// Quantize colors in image using current quantization settings
// Set measureError_ to true in order to measure quantization error
void Magick::Image::quantize ( bool measureError_  )
{
  modifyImage();

  if ( ( classType() == DirectClass ) ||
       ( totalColors() > options()->quantizeColors() ))
    QuantizeImage( options()->quantizeInfo(),
			      image() );

  if ( measureError_ )
    QuantizationError( image() );

  // Udate DirectClass representation of pixels
  SyncImage( image() );
  throwImageException();
}

// Raise image (lighten or darken the edges of an image to give a 3-D
// raised or lowered effect)
void Magick::Image::raise ( const Geometry &geometry_ ,
			    bool raisedFlag_ )
{
  RectangleInfo raiseInfo;
  raiseInfo.width  = geometry_.width();
  raiseInfo.height = geometry_.height();
  raiseInfo.x      = geometry_.xOff();
  if ( geometry_.xNegative() )
    raiseInfo.x = - raiseInfo.x;
  raiseInfo.y      = geometry_.yOff();
  if ( geometry_.yNegative() )
    raiseInfo.y = - raiseInfo.y;

  modifyImage();
  RaiseImage ( image(), &raiseInfo, raisedFlag_ );
  throwImageException();
}

// Read image into current object
void Magick::Image::read ( const std::string &imageSpec_ )
{
  options()->fileName( imageSpec_ );

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* image =
    ReadImage( imageInfo(), &exceptionInfo );
  replaceImage( image );
  throwException( exceptionInfo );
}

// Read image of specified size into current object
void Magick::Image::read ( const Geometry &size_,
			   const std::string &imageSpec_ )
{
  size( size_ );
  read( imageSpec_ );
}

// Read image from in-memory BLOB
void Magick::Image::read ( const Blob &blob_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* image =
    BlobToImage( imageInfo(),
		 static_cast<const char *>(blob_.data()),
		 blob_.length(), &exceptionInfo );
  replaceImage( image );
  throwException( exceptionInfo );
}

// Read image of specified size from in-memory BLOB
void  Magick::Image::read ( const Blob &blob_,
			    const Geometry &size_ )
{
  // Set image size
  size( size_ );
  // Read from Blob
  read( blob_ );
}

// Read image of specified size and depth from in-memory BLOB
void Magick::Image::read ( const Blob &blob_,
			   const Geometry &size_,
			   unsigned int depth_ )
{
  // Set image size
  size( size_ );
  // Set image depth
  depth( depth_ );
  // Read from Blob
  read( blob_ );
}

// Read image of specified size, depth, and format from in-memory BLOB
void Magick::Image::read ( const Blob &blob_,
			   const Geometry &size_,
			   unsigned int depth_,
			   const std::string &magick_ )
{
  // Set image size
  size( size_ );
  // Set image depth
  depth( depth_ );
  // Set image magick
  magick( magick_ );
  // Read from Blob
  read( blob_ );
}

// Read image of specified size, and format from in-memory BLOB
void Magick::Image::read ( const Blob &blob_,
			   const Geometry &size_,
			   const std::string &magick_ )
{
  // Set image size
  size( size_ );
  // Set image magick
  magick( magick_ );
  // Read from Blob
  read( blob_ );
}

// Reduce noise in image
void Magick::Image::reduceNoise ( unsigned int order_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ReduceNoiseImage( image(), order_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Roll image
void Magick::Image::roll ( const Geometry &roll_ )
{
  int xOff = roll_.xOff();
  if ( roll_.xNegative() )
    xOff = - xOff;
  int yOff = roll_.yOff();
  if ( roll_.yNegative() )
    yOff = - yOff;

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    RollImage( image(), xOff, yOff, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}
void Magick::Image::roll ( int columns_, int rows_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    RollImage( image(), columns_, rows_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Rotate image
void Magick::Image::rotate ( double degrees_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    RotateImage( image(), degrees_, &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Sample image
void Magick::Image::sample ( const Geometry &geometry_ )
{
  int x = 0;
  int y = 0;
  unsigned int width = columns();
  unsigned int height = rows();

  ParseImageGeometry (((std::string)geometry_).c_str(),
		      &x, &y,
		      &width, &height );

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    SampleImage( image(), width, height, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Scale image
void Magick::Image::scale ( const Geometry &geometry_ )
{
  int x = 0;
  int y = 0;
  unsigned int width = columns();
  unsigned int height = rows();

  ParseImageGeometry (((std::string)geometry_).c_str(),
		      &x, &y,
		      &width, &height );

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ScaleImage( image(), width, height, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Segment (coalesce similar image components) by analyzing the
// histograms of the color components and identifying units that are
// homogeneous with the fuzzy c-means technique.
void Magick::Image::segment ( double clusterThreshold_, 
			      double smoothingThreshold_ )
{
  modifyImage();
  SegmentImage ( image(),
		 options()->quantizeColorSpace(),
		 options()->verbose(),
		 clusterThreshold_,
		 smoothingThreshold_ );
  throwImageException();
  SyncImage( image() );
  throwImageException();
}

// Shade image using distant light source
void Magick::Image::shade ( double azimuth_,
			    double elevation_,
			    bool   colorShading_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ShadeImage( image(),
		colorShading_,
		azimuth_,
		elevation_,
		&exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Sharpen pixels in image
void Magick::Image::sharpen ( unsigned int order_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    SharpenImage( image(),
			     order_,
			     &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Shear image
void Magick::Image::shear ( double xShearAngle_,
			    double yShearAngle_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ShearImage( image(),
		xShearAngle_,
		yShearAngle_,
		&exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Solarize image (similar to effect seen when exposing a photographic
// film to light during the development process)
void Magick::Image::solarize ( double factor_ )
{
  modifyImage();
  SolarizeImage ( image(), factor_ );
  throwImageException();
}

// Spread pixels randomly within image by specified ammount
void Magick::Image::spread ( unsigned int amount_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    SpreadImage( image(),
		 amount_,
		 &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Add a digital watermark to the image (based on second image)
void Magick::Image::stegano ( const Image &watermark_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    SteganoImage( image(),
		  const_cast<Image &>(watermark_).image(),
		  &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Stereo image (left image is current image)
void Magick::Image::stereo ( const Image &rightImage_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    StereoImage( image(),
		 const_cast<Image &>(rightImage_).image(),
		 &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Swirl image
void Magick::Image::swirl ( double degrees_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    SwirlImage( image(), degrees_,
		&exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Texture image
void Magick::Image::texture ( const Image &texture_ )
{
  modifyImage();
  TextureImage( image(), const_cast<Image &>(texture_).image() );
  throwImageException();
}

// Threshold image
void Magick::Image::threshold ( double threshold_ )
{
  modifyImage();
  ThresholdImage( image(), threshold_ );
  throwImageException();
}

// Transform image based on image geometry only
void Magick::Image::transform ( const Geometry &imageGeometry_ )
{
  modifyImage();
  TransformImage ( &(image()), (char *)0,
		   std::string(imageGeometry_).c_str() );
  throwImageException();
}
// Transform image based on image and crop geometries
void Magick::Image::transform ( const Geometry &imageGeometry_,
				const Geometry &cropGeometry_ )
{
  modifyImage();
  TransformImage ( &(image()), std::string(cropGeometry_).c_str(),
		   std::string(imageGeometry_).c_str() );
  throwImageException();
}

// Add matte image to image, setting pixels matching color to transparent
void Magick::Image::transparent ( const Color &color_ )
{
  if ( !color_.isValid() )
    {
      throwException( OptionError,
		      "Color argument is invalid" );
    }

  std::string color = color_;

  modifyImage();
  TransparentImage ( image(), color_ );
  throwImageException();
}

// Trim edges that are the background color from the image
void Magick::Image::trim ( void )
{
  // width=0, height=0 trims edges
  Geometry cropInfo(0,0);
  crop ( cropInfo );
}

// Map image pixels to a sine wave
void Magick::Image::wave ( double amplitude_, double wavelength_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    WaveImage( image(),
	       amplitude_,
	       wavelength_,
	       &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Write image to file
void Magick::Image::write( const std::string &imageSpec_ )
{
  modifyImage();
  fileName( imageSpec_ );
  WriteImage( imageInfo(), image() );
  throwImageException();
}

// Write image to in-memory BLOB
void Magick::Image::write ( Blob *blob_ )
{
  size_t length = 2048; // Efficient size for small images
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  void* data = ImageToBlob( imageInfo(),
			    image(),
			    &length,
			    &exceptionInfo);
  throwException( exceptionInfo );
  blob_->updateNoCopy( data, length );
  throwImageException();
}
void Magick::Image::write ( Blob *blob_,
			    const std::string &magick_ )
{
  magick(magick_);
  size_t length = 2048; // Efficient size for small images
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  void* data = ImageToBlob( imageInfo(),
			    image(),
			    &length,
			    &exceptionInfo);
  throwException( exceptionInfo );
  blob_->updateNoCopy( data, length );
  throwImageException();
}
void Magick::Image::write ( Blob *blob_,
			    const std::string &magick_,
			    unsigned int depth_ )
{
  magick(magick_);
  depth(depth_);
  size_t length = 2048; // Efficient size for small images
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  void* data = ImageToBlob( imageInfo(),
			    image(),
			    &length,
			    &exceptionInfo);
  throwException( exceptionInfo );
  blob_->updateNoCopy( data, length );
  throwImageException();
}


// Zoom image
void Magick::Image::zoom( const Geometry &geometry_ )
{
  // Calculate new size.  This code should be supported using binary arguments
  // in the ImageMagick library.
  int x = 0;
  int y = 0;
  unsigned int width = columns();
  unsigned int height = rows();

  ParseImageGeometry (((std::string)geometry_).c_str(),
		      &x, &y,
		      &width, &height );

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ZoomImage( image(),
	       width,
	       height,
	       &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

/*
 * Methods for setting image attributes
 *
 */

// Join images into a single multi-image file
void Magick::Image::adjoin ( bool flag_ )
{
  modifyImage();
  options()->adjoin( flag_ );
}
bool Magick::Image::adjoin ( void ) const
{
  return constOptions()->adjoin();
}

// Remove pixel aliasing
void Magick::Image::antiAlias( bool flag_ )
{
  modifyImage();
  options()->antiAlias( static_cast<unsigned int>(flag_) );
}
bool Magick::Image::antiAlias( void )
{
  return static_cast<bool>( options()->antiAlias( ) );
}

void Magick::Image::animationDelay ( unsigned int delay_ )
{
  modifyImage();
  image()->delay = delay_;
  options()->animationDelay( delay_ );
}
unsigned int Magick::Image::animationDelay ( void ) const
{
  return constOptions()->animationDelay();
  //return constImage()->delay;
}

void Magick::Image::animationIterations ( unsigned int iterations_ )
{
  modifyImage();
  image()->iterations = iterations_;
  options()->animationIterations( iterations_ );
}
unsigned int Magick::Image::animationIterations ( void ) const
{
  return constOptions()->animationIterations( );
  //return image()->iterations;
}

void Magick::Image::backgroundColor ( const Color &color_ )
{
  modifyImage();

  if ( color_.isValid() )
    {
      image()->background_color.red   = color_.redQuantum();
      image()->background_color.green = color_.greenQuantum();
      image()->background_color.blue  = color_.blueQuantum();
    }
  else
    {
      image()->background_color.red   = 0;
      image()->background_color.green = 0;
      image()->background_color.blue  = 0;
    }

  options()->backgroundColor( color_ );
}
Magick::Color Magick::Image::backgroundColor ( void ) const
{
  return constOptions()->backgroundColor( );
//   Image * image = image();
//   return Color( image->background_color.red,
// 		      image->background_color.green,
// 		      image->background_color.blue );
}

void Magick::Image::backgroundTexture ( const std::string &backgroundTexture_ )
{
  modifyImage();
  options()->backgroundTexture( backgroundTexture_ );
}
std::string Magick::Image::backgroundTexture ( void ) const
{
  return constOptions()->backgroundTexture( );
}

unsigned int Magick::Image::baseColumns ( void ) const
{
  return constImage()->magick_columns;
}

std::string Magick::Image::baseFilename ( void ) const
{
  return std::string(constImage()->magick_filename);
}

unsigned int Magick::Image::baseRows ( void ) const
{
  return constImage()->magick_rows;
}

void Magick::Image::borderColor ( const Color &color_ )
{
  modifyImage();

  if ( color_.isValid() )
    {
      image()->border_color.red   = color_.redQuantum();
      image()->border_color.green = color_.greenQuantum();
      image()->border_color.blue  = color_.blueQuantum();
    }
  else
    {
      image()->border_color.red   = 0;
      image()->border_color.green = 0;
      image()->border_color.blue  = 0;
    }

  options()->borderColor( color_ );
}
Magick::Color Magick::Image::borderColor ( void ) const
{
  return constOptions()->borderColor( );

//   Image * image = image();
//   return Color ( image->border_color.red,
// 		       image->border_color.green,
// 		       image->border_color.blue );
}

// Text bounding-box base color
void Magick::Image::boxColor ( const Color &boxColor_ )
{
  modifyImage();
  options()->boxColor( boxColor_ );
}
Magick::Color Magick::Image::boxColor ( void ) const
{
  return constOptions()->boxColor( );
}

// Pixel cache threshold.  Once this threshold is exceeded, all
// subsequent pixels cache operations are to/from disk.
// This setting is shared by all Image objects.
/* static */
void Magick::Image::cacheThreshold ( unsigned int threshold_ )
{
  SetCacheThreshold( threshold_ );
}

void Magick::Image::chromaBluePrimary ( float x_, float y_ )
{
  modifyImage();
  image()->chromaticity.blue_primary.x = x_;
  image()->chromaticity.blue_primary.y = y_;
}
void Magick::Image::chromaBluePrimary ( float *x_, float *y_ ) const
{
  *x_ = constImage()->chromaticity.blue_primary.x;
  *y_ = constImage()->chromaticity.blue_primary.y;
}

void Magick::Image::chromaGreenPrimary ( float x_, float y_ )
{
  modifyImage();
  image()->chromaticity.green_primary.x = x_;
  image()->chromaticity.green_primary.y = y_;
}
void Magick::Image::chromaGreenPrimary ( float *x_, float *y_ ) const
{
  *x_ = constImage()->chromaticity.green_primary.x;
  *y_ = constImage()->chromaticity.green_primary.y;
}

void Magick::Image::chromaRedPrimary ( float x_, float y_ )
{
  modifyImage();
  image()->chromaticity.red_primary.x = x_;
  image()->chromaticity.red_primary.y = y_;
}
void Magick::Image::chromaRedPrimary ( float *x_, float *y_ ) const
{
  *x_ = constImage()->chromaticity.red_primary.x;
  *y_ = constImage()->chromaticity.red_primary.y;
}

void Magick::Image::chromaWhitePoint ( float x_, float y_ )
{
  modifyImage();
  image()->chromaticity.white_point.x = x_;
  image()->chromaticity.white_point.y = y_;
}
void Magick::Image::chromaWhitePoint ( float *x_, float *y_ ) const
{
  *x_ = constImage()->chromaticity.white_point.x;
  *y_ = constImage()->chromaticity.white_point.y;
}

void Magick::Image::classType ( Magick::ClassType class_ )
{
  if ( classType() == PseudoClass && class_ == DirectClass )
    {
      // Use SyncImage to synchronize the DirectClass pixels with the
      // color map and then set to DirectClass type.
      modifyImage();
      SyncImage( image() );
      FreeMemory( (void**)&(image()->colormap) );
      image()->c_class = (MagickLib::ClassType)DirectClass;
      return;
    }

  if ( classType() == DirectClass && class_ == PseudoClass )
    {
      // Quantize to create PseudoClass color map
      modifyImage();
      quantizeColors(MaxRGB + 1);
      quantize();
      image()->c_class = (MagickLib::ClassType)PseudoClass;
    }
}
Magick::ClassType Magick::Image::classType ( void ) const
{
  return (Magick::ClassType)constImage()->c_class;
}

void Magick::Image::colorFuzz ( unsigned int fuzz_ )
{
  modifyImage();
  image()->fuzz = fuzz_;
  options()->colorFuzz( fuzz_ );
}
unsigned int Magick::Image::colorFuzz ( void ) const
{
  return constOptions()->colorFuzz( );
//   return constImage()->fuzz;
}

// Set color in colormap at index
void Magick::Image::colorMap ( unsigned int index_,
			       const Color &color_ )
{
  if ( !color_.isValid() )
    throwException( OptionError,
		    "Color argument is invalid");
  
  if ( constImage()->c_class != PseudoClass )
    throwException( OptionError,
		    "Image class does not support colormap");

  if ( index_ > constImage()->colors )
    throwException( OptionError,
		    "Color index is greater than maximum image color index");

  if ( !constImage()->colormap )
    throwException( OptionError,
		    "Image does not contain colormap");

  modifyImage();

  *(image()->colormap + index_) = color_;
}
// Return color in colormap at index
Magick::Color Magick::Image::colorMap ( unsigned int index_ ) const
{
  if ( constImage()->c_class != PseudoClass )
    throwException( OptionError,
		    "Image class does not support colormap");

  if ( !constImage()->colormap )
    throwException( CorruptImageError,
		    "Image does not contain colormap");

  if ( index_ > constImage()->colors )
    throwException( OptionError,
		    "Color index is greater than maximum image color index");

  PixelPacket *color = constImage()->colormap + index_;
  return Magick::Color( color->red, color->green, color->blue );
}

// Image colorspace
void Magick::Image::colorSpace( ColorspaceType colorSpace_ )
{
  // Nothing to do?
  if ( image()->colorspace == colorSpace_ )
    return;

  modifyImage();

  if ( image()->colorspace == RGBColorspace ||
       image()->colorspace == TransparentColorspace ||
       image()->colorspace == GRAYColorspace )
    {
      // Convert the image to an alternate colorspace representation
      // In:			Out:
      // RGBColorspace		RGBColorspace (no conversion)
      // TransparentColorspace	TransparentColorspace (no conversion)
      // GRAYColorspace		GRAYColorspace (no conversion if really Gray)
      // RGBColorspace		CMYKColorspace
      // RGBColorspace		GRAYColorspace
      // RGBColorspace		OHTAColorspace
      // RGBColorspace		sRGBColorspace
      // RGBColorspace		XYZColorspace
      // RGBColorspace		YCbCrColorspace
      // RGBColorspace		YCCColorspace
      // RGBColorspace		YIQColorspace
      // RGBColorspace		YPbPrColorspace
      // RGBColorspace		YUVColorspace
      RGBTransformImage( image(), colorSpace_ );
      throwImageException();
      return;
    }

  if ( colorSpace_ == RGBColorspace ||
       colorSpace_ == TransparentColorspace ||
       colorSpace_ == GRAYColorspace )
    {
      // Convert the image from an alternate colorspace representation
      // In:				Out:
      // CMYKColorspace		RGBColorspace
      // RGBColorspace		RGBColorspace (no conversion)
      // GRAYColorspace		GRAYColorspace (no conversion)
      // TransparentColorspace	TransparentColorspace (no conversion)
      // OHTAColorspace		RGBColorspace
      // sRGBColorspace		RGBColorspace
      // XYZColorspace		RGBColorspace
      // YCbCrColorspace		RGBColorspace
      // YCCColorspace		RGBColorspace
      // YIQColorspace		RGBColorspace
      // YPbPrColorspace		RGBColorspace
      // YUVColorspace		RGBColorspace
      TransformRGBImage( image(), colorSpace_ );
      throwImageException();
      return;
    }
}
Magick::ColorspaceType Magick::Image::colorSpace ( void ) const
{
  return constImage()->colorspace;
}

// Image columns
unsigned int Magick::Image::columns ( void ) const
{
  return constImage()->columns;
}

// Comment
void Magick::Image::comment ( const std::string &comment_ )
{
  modifyImage();

  if ( comment_.length() > 0 )
    SetImageAttribute( image(), "Comment", comment_.c_str() );
  else
    SetImageAttribute( image(), "Comment", NULL );

  throwImageException();
}
std::string Magick::Image::comment ( void ) const
{
  ImageAttribute * attribute =
    GetImageAttribute( constImage(), "Comment" );

  if ( attribute )
    return std::string( attribute->value );

  return std::string(); // Intentionally no exception
}

void Magick::Image::compressType ( Magick::CompressionType compressType_ )
{
  modifyImage();
  image()->compression = compressType_;
  options()->compressType( compressType_ );
}
Magick::CompressionType Magick::Image::compressType ( void ) const
{
  return constOptions()->compressType( );
  //  return constImage()->compression;
}

void Magick::Image::density ( const Geometry &density_ )
{
  modifyImage();
  options()->density( density_ );
  if ( density_.isValid() )
    {
      image()->x_resolution = density_.width();
      if ( density_.height() != 0 )
	image()->y_resolution = density_.height();
      else
	image()->y_resolution = density_.width();
    }
  else
    {
      // Reset to default
      image()->x_resolution = 0;
      image()->y_resolution = 0;
    }
}
Magick::Geometry Magick::Image::density ( void ) const
{
  return constOptions()->density( );
}

void Magick::Image::depth ( unsigned int depth_ )
{
  modifyImage();
  image()->depth = depth_;
  options()->depth( depth_ );
}
unsigned int Magick::Image::depth ( void ) const
{
  return constImage()->depth;
  //  return constOptions()->depth( );
}

std::string Magick::Image::directory ( void ) const
{
  if ( constImage()->directory )
    return std::string( constImage()->directory );

  throwException( CorruptImageWarning,
		  "Image does not contain a directory");

  return std::string();
}

void Magick::Image::fileName ( const std::string &fileName_ )
{
  modifyImage();

  fileName_.copy( image()->filename,
		  sizeof(image()->filename) - 1 );
  image()->filename[ fileName_.length() ] = 0; // Null terminate
  
  options()->fileName( fileName_ );
  
}
std::string Magick::Image::fileName ( void ) const
{
  return constOptions()->fileName( );
  //  return std::string( constImage()->filename );
}

unsigned int Magick::Image::fileSize ( void ) const
{
  return constImage()->filesize;
}

void Magick::Image::filterType ( Magick::FilterType filterType_ )
{
  modifyImage();
  image()->filter = filterType_;
}
Magick::FilterType Magick::Image::filterType ( void ) const
{
  return constImage()->filter;
}

void Magick::Image::font ( const std::string &font_ )
{
  modifyImage();
  options()->font( font_ );
}
std::string Magick::Image::font ( void ) const
{
  return constOptions()->font( );
}

void Magick::Image::fontPointsize ( double pointSize_ )
{
  modifyImage();
  options()->fontPointsize( pointSize_ );
}
double Magick::Image::fontPointsize ( void ) const
{
  return constOptions()->fontPointsize( );
}

std::string Magick::Image::format ( void ) const
{
  //  return options()->format ( );
  const MagickInfo * magick_info
    = GetMagickInfo( constImage()->magick );

  if (( magick_info != (MagickInfo *)0 ) && 
      ( *magick_info->description != '\0' ))
    return std::string(magick_info->description);

  throwException( CorruptImageWarning,
		  "Unrecognized image magick type");

  return std::string();
}

double Magick::Image::gamma ( void ) const
{
  return constImage()->gamma;
}

Magick::Geometry Magick::Image::geometry ( void ) const
{
  if ( constImage()->geometry )
    {
      return Geometry(constImage()->geometry);
    }

  throwException( OptionWarning,
		  "Image does not contain a geometry");

  return Geometry();
}

void Magick::Image::gifDisposeMethod ( unsigned int disposeMethod_ )
{
  modifyImage();
  image()->dispose = disposeMethod_;
  options()->gifDisposeMethod( disposeMethod_ );
}
unsigned int Magick::Image::gifDisposeMethod ( void ) const
{
  // FIXME: It would be better to return an enumeration

  return constOptions()->gifDisposeMethod( );

//   return(image()->dispose);
}

// ICC color profile (BLOB)
void Magick::Image::iccColorProfile( const Magick::Blob &colorProfile_ )
{
  ProfileInfo * color_profile = &(image()->color_profile);
  FreeMemory( (void**)&color_profile->info );
  color_profile->length = 0;

  if ( colorProfile_.data() != 0 )
    {
      color_profile->info = new unsigned char[colorProfile_.length()];
      memcpy( color_profile->info, colorProfile_.data(),
              colorProfile_.length());
      color_profile->length = colorProfile_.length();
    }
}
Magick::Blob Magick::Image::iccColorProfile( void ) const
{
  const ProfileInfo * color_profile = &(constImage()->color_profile);
  return Blob( color_profile->info, color_profile->length );
}

void Magick::Image::interlaceType ( Magick::InterlaceType interlace_ )
{
  modifyImage();
  image()->interlace = interlace_;
  options()->interlaceType ( interlace_ );
}
Magick::InterlaceType Magick::Image::interlaceType ( void ) const
{
  return constOptions()->interlaceType ( );
  //  return image()->interlace;
}

// IPTC profile (BLOB)
void Magick::Image::iptcProfile( const Magick::Blob &iptcProfile_ )
{
  ProfileInfo * iptc_profile = &(image()->iptc_profile);
  FreeMemory( (void**)&iptc_profile->info );
  iptc_profile->length = 0;

  if ( iptcProfile_.data() != 0 )
    {
      iptc_profile->info = new unsigned char[iptcProfile_.length()];
      memcpy( iptc_profile->info, iptcProfile_.data(),
              iptcProfile_.length());

      iptc_profile->length = iptcProfile_.length();
    }
}
Magick::Blob Magick::Image::iptcProfile( void ) const
{
  const ProfileInfo * iptc_profile = &(constImage()->iptc_profile);
  return Blob( iptc_profile->info, iptc_profile->length );
}

// Does object contain valid image?
void Magick::Image::isValid ( bool isValid_ )
{
  if ( isValid_ == false )
    {
      delete _imgRef;
      _imgRef = new ImageRef;
    }
  else
    {
      if ( !isValid() )
	{
	  // Construct with single-pixel black image to make
	  // image valid.  This is an obvious hack.
	  size( Geometry(1,1) );
	  read( "xc:#000000" );
	}
    }
}

bool Magick::Image::isValid ( void ) const
{
  if ( rows() && columns() )
    return true;

  return false;
}

// Label image
void Magick::Image::label ( const std::string &label_ )
{
  modifyImage();
  if ( label_.length() > 0 )
    SetImageAttribute ( image(), "Label", label_.c_str() );
  else
    SetImageAttribute ( image(), "Label", NULL );
  throwImageException();
}
std::string Magick::Image::label ( void ) const
{
  ImageAttribute * attribute =
    GetImageAttribute( constImage(), "Label" );

  if ( attribute )
    return std::string( attribute->value );

  return std::string();
}

// Linewidth for drawing lines, circles, ellipses, etc.
void Magick::Image::lineWidth ( double lineWidth_ )
{
  modifyImage();
  options()->lineWidth( lineWidth_ );
}
double Magick::Image::lineWidth ( void ) const
{
  return constOptions()->lineWidth( );
}

void Magick::Image::magick ( const std::string &magick_ )
{
  modifyImage();

  magick_.copy( image()->magick,
		sizeof(image()->magick) - 1 );
  image()->magick[ magick_.length() ] = 0;
  
  options()->magick( magick_ );
}
std::string Magick::Image::magick ( void ) const
{
  if ( *(constImage()->magick) != '\0' )
    return std::string(constImage()->magick);

  return constOptions()->magick( );
}

void Magick::Image::matte ( bool matteFlag_ )
{
  modifyImage();
  image()->matte = matteFlag_;
}
bool Magick::Image::matte ( void ) const
{
  if ( constImage()->matte )
    return true;
  else
    return false;
}

void Magick::Image::matteColor ( const Color &matteColor_ )
{
  modifyImage();
  
  if ( matteColor_.isValid() )
    {
      image()->matte_color.red   = matteColor_.redQuantum();
      image()->matte_color.green = matteColor_.greenQuantum();
      image()->matte_color.blue  = matteColor_.blueQuantum();

      options()->matteColor( matteColor_ ); 
    }
  else
    {
      // Set to default matte color
      Color tmpColor( "#BDBDBD" );
      image()->matte_color.red   = tmpColor.redQuantum();
      image()->matte_color.green = tmpColor.greenQuantum();
      image()->matte_color.blue  = tmpColor.blueQuantum();

      options()->matteColor( tmpColor );
    }
}
Magick::Color Magick::Image::matteColor ( void ) const
{
  return Color( constImage()->matte_color.red,
		constImage()->matte_color.green,
		constImage()->matte_color.blue );
}

double Magick::Image::meanErrorPerPixel ( void ) const
{
  return(constImage()->mean_error_per_pixel);
}

void Magick::Image::monochrome ( bool monochromeFlag_ )
{
  modifyImage();
  options()->monochrome( monochromeFlag_ );
}
bool Magick::Image::monochrome ( void ) const
{
  return constOptions()->monochrome( );
}

Magick::Geometry Magick::Image::montageGeometry ( void ) const
{
  if ( constImage()->montage )
    return Magick::Geometry(constImage()->montage);

  throwException( CorruptImageWarning,
		  "Image does not contain a montage" );

  return Magick::Geometry();
}

double Magick::Image::normalizedMaxError ( void ) const
{
  return(constImage()->normalized_maximum_error);
}

double Magick::Image::normalizedMeanError ( void ) const
{
  return constImage()->normalized_mean_error;
}

void Magick::Image::penColor ( const Color &penColor_ )
{
  modifyImage();
  options()->penColor( penColor_ );
}
Magick::Color Magick::Image::penColor ( void  ) const
{
  return constOptions()->penColor( );
}

void Magick::Image::penTexture ( const Image &penTexture_ )
{
  modifyImage();
  options()->penTexture( const_cast<Image &>(penTexture_).constImage() );
}

Magick::Image  Magick::Image::penTexture ( void  ) const
{
  // FIXME: This is inordinately innefficient
  Image texture;
  
  const MagickLib::Image* tmpTexture = constOptions()->penTexture( );

  if ( tmpTexture )
    {
      ExceptionInfo exceptionInfo;
      GetExceptionInfo( &exceptionInfo );
      MagickLib::Image* image =
	CloneImage( const_cast<MagickLib::Image *>(tmpTexture),
			       tmpTexture->columns,
			       tmpTexture->rows,
			       (int)true,
			       &exceptionInfo);
      texture.replaceImage( image );
      throwException( exceptionInfo );
    }
  return texture;
}

void Magick::Image::pixelColor ( unsigned int x_, unsigned int y_,
				 const Color &color_ )
{
  if ( color_.isValid() )
    {
      // Test arguments to ensure they are within the image.
      if ( y_ > rows() || x_ > columns() )
	throwException( OptionError,
			"Access outside of image boundary" );
      
      modifyImage();

      // Set image to DirectClass
      classType( DirectClass );

      // Get pixel view
      Pixels pixels(*this);
      // Set pixel value
      *(pixels.get(x_, y_, 1, 1 )) = color_;
      // Tell ImageMagick that pixels have been updated
      pixels.sync();

      return;
    }

  throwException( OptionError,
		  "Color argument is invalid" );
}
Magick::Color Magick::Image::pixelColor ( unsigned int x_,
					  unsigned int y_ )
{
  // Test arguments to ensure they are within the image.
  if ( y_ > rows() || x_ > columns() )
    throwException( OptionError,
		    "Access outside of image boundary" );

  // Get pixel view
  Pixels pixels(*this);
  // Return value
  return Color( *(pixels.get(x_, y_, 1, 1 )) );
}

// Preferred size and location of an image canvas.
void Magick::Image::page ( const Magick::Geometry &pageSize_ )
{
  modifyImage();
  options()->page( pageSize_ );
  image()->page = pageSize_;
}
Magick::Geometry Magick::Image::page ( void ) const
{
  return Geometry( constImage()->page.width,
		   constImage()->page.height,
		   constImage()->page.x,
		   constImage()->page.y );
}

void Magick::Image::quality ( unsigned int quality_ )
{
  modifyImage();
  options()->quality( quality_ );
}
unsigned int Magick::Image::quality ( void ) const
{
  return constOptions()->quality( );
}

void Magick::Image::quantizeColors ( unsigned int colors_ )
{
  modifyImage();
  options()->quantizeColors( colors_ );
}
unsigned int Magick::Image::quantizeColors ( void ) const
{
  return constOptions()->quantizeColors( );
}

void Magick::Image::quantizeColorSpace ( Magick::ColorspaceType colorSpace_ )
{
  modifyImage();
  options()->quantizeColorSpace( colorSpace_ );
}
Magick::ColorspaceType Magick::Image::quantizeColorSpace ( void ) const
{
  return constOptions()->quantizeColorSpace( );
}

void Magick::Image::quantizeDither ( bool ditherFlag_ )
{
  modifyImage();
  options()->quantizeDither( ditherFlag_ );
}
bool Magick::Image::quantizeDither ( void ) const
{
  return constOptions()->quantizeDither( );
}

void Magick::Image::quantizeTreeDepth ( unsigned int treeDepth_ )
{
  modifyImage();
  options()->quantizeTreeDepth( treeDepth_ );
}
unsigned int Magick::Image::quantizeTreeDepth ( void ) const
{
  return constOptions()->quantizeTreeDepth( );
}

void Magick::Image::renderingIntent ( Magick::RenderingIntent renderingIntent_ )
{
  modifyImage();
  image()->rendering_intent = renderingIntent_;
}
Magick::RenderingIntent Magick::Image::renderingIntent ( void ) const
{
  return (Magick::RenderingIntent)constImage()->rendering_intent;
}

void Magick::Image::resolutionUnits ( Magick::ResolutionType resolutionUnits_ )
{
  modifyImage();
  image()->units = resolutionUnits_;
  options()->resolutionUnits( resolutionUnits_ );
}
Magick::ResolutionType Magick::Image::resolutionUnits ( void ) const
{
  return constOptions()->resolutionUnits( );
  //  return image()->units;
}

unsigned int Magick::Image::rows ( void ) const
{
  return constImage()->rows;
}

void Magick::Image::scene ( unsigned int scene_ )
{
  modifyImage();
  image()->scene = scene_;
}
unsigned int Magick::Image::scene ( void ) const
{
  return constImage()->scene;
}

std::string Magick::Image::signature ( bool force_ ) const
{
  // Unlike most operations that modify image attributes, this the
  // signature operation is not considered a modification of image
  // data so we do not force a copy of the image if we calculate its
  // signature.

  // Re-calculate image signature
  if ( force_ ||
       !GetImageAttribute(constImage(), "Signature") ||
       constImage()->taint )
    {
      MagickLib::SignatureImage( const_cast<MagickLib::Image *>(constImage()) );
    }

  ImageAttribute * attribute =
    GetImageAttribute(constImage(), "Signature");

  return std::string( attribute->value );
}

void Magick::Image::size ( const Geometry &geometry_ )
{
  modifyImage();
  options()->size( geometry_ );
  image()->rows = geometry_.height();
  image()->columns = geometry_.width();
}
Magick::Geometry Magick::Image::size ( void ) const
{
  return Magick::Geometry( constImage()->columns, constImage()->rows );
  //  return constOptions()->size( );
}

void Magick::Image::subImage ( unsigned int subImage_ )
{
  modifyImage();
  options()->subImage( subImage_ );
}
unsigned int Magick::Image::subImage ( void ) const
{
  return constOptions()->subImage( );
}

void Magick::Image::subRange ( unsigned int subRange_ )
{
  modifyImage();
  options()->subRange( subRange_ );
}
unsigned int Magick::Image::subRange ( void ) const
{
  return constOptions()->subRange( );
}

void Magick::Image::tileName ( const std::string &tileName_ )
{
  modifyImage();
  options()->tileName( tileName_ );
}
std::string Magick::Image::tileName ( void ) const
{
  return constOptions()->tileName( );
}

unsigned long Magick::Image::totalColors ( void )
{
  return GetNumberColors( image(), (FILE *) NULL);
}

Magick::ImageType Magick::Image::type ( void ) const
{
  return (Magick::ImageType)MagickLib::GetImageType( const_cast<MagickLib::Image *>(constImage()) );
}

void Magick::Image::verbose ( bool verboseFlag_ )
{
  modifyImage();
  options()->verbose( verboseFlag_ );
}
bool Magick::Image::verbose ( void ) const
{
  return constOptions()->verbose( );
}

void Magick::Image::view ( const std::string &view_ )
{
  modifyImage();
  options()->view( view_ );
}
std::string Magick::Image::view ( void ) const
{
  return constOptions()->view( );
}

void Magick::Image::x11Display ( const std::string &display_ )
{
  modifyImage();
  options()->x11Display( display_ );
}
std::string Magick::Image::x11Display ( void ) const
{
  return constOptions()->x11Display( );
}

double Magick::Image::xResolution ( void ) const
{
  return constImage()->x_resolution;
}
double Magick::Image::yResolution ( void ) const
{
  return constImage()->y_resolution;
}

// Copy Constructor
Magick::Image::Image( const Image & image_ )
  : _imgRef(image_._imgRef)
{
  Lock( _imgRef->_mutexLock );

  // Increase reference count
  ++_imgRef->_refCount;
}

// Assignment operator
Magick::Image Magick::Image::operator=( const Magick::Image &image_ )
{
  {
    Lock(image_._imgRef->_mutexLock);
    ++image_._imgRef->_refCount;
  }

  bool doDelete = false;
  {
    Lock( _imgRef->_mutexLock );
    if ( --_imgRef->_refCount == 0 )
      doDelete = true;
  }

  if ( doDelete )
    {
      // Delete old image reference with associated image and options.
      delete _imgRef;
      _imgRef = (Magick::ImageRef*)0;
    }
  // Use new image reference
  _imgRef = image_._imgRef;

  return *this;
}

/////////////////////////////////////////////////////////////////////
//
// No end-user methods beyond this point
//
/////////////////////////////////////////////////////////////////////


//
// Construct using Image and Magick::Options
//
Magick::Image::Image ( MagickLib::Image* image_, Magick::Options* options_ )
  : _imgRef(new ImageRef( image_, options_))
{
}

//
// Replace current image
//
MagickLib::Image * Magick::Image::replaceImage( MagickLib::Image* replacement_ )
{
  if ( replacement_ )
    {
      Lock( _imgRef->_mutexLock );
      if ( _imgRef->_refCount == 1 )
	{
	  // We own the image.  Destroy existing image.
	  if ( _imgRef->_image )
	    DestroyImages( _imgRef->_image );
	  
	  // Set reference image pointer to new image
	  _imgRef->image(replacement_);
	}
      else
	{
	  // De-reference old reference object
	  --_imgRef->_refCount;

	  // Initialize new reference object with new image pointer
	  // and copy of options
	  _imgRef = new ImageRef( replacement_,
				  _imgRef->_options );
	}
    }

  return replacement_;
}

//
// Prepare to modify image or image options
// Replace current image and options with copy if reference count > 1
//
void Magick::Image::modifyImage( void )
{
  // Nothing to do if we are sole owner of image
  {
    Lock( _imgRef->_mutexLock );
    if ( _imgRef->_refCount == 1 )
      return;
  }

  MagickLib::Image* image = _imgRef->image();
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    CloneImage( image,
		image->columns,
		image->rows,
		true,
		&exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
  return;
}

//
// Test for an ImageMagick reported error and throw exception if one
// has been reported.
//
void Magick::Image::throwImageException( void )
{
  if ( constImage()->exception.severity == UndefinedException )
    return;

  // Save exception
  const ExceptionInfo except = image()->exception;
  // Reset original exception to defaults
  GetExceptionInfo( &image()->exception );
  // Throw exception
  throwException( except );
}

/////////////////////////////////////////////
//
// ImageRef image handle implementation
//
/////////////////////////////////////////////

// // Construct with an image and default options
Magick::ImageRef::ImageRef ( MagickLib::Image * image_ )
  : _image(image_),
    _options(new Options),
    _refCount(1)
{
}

// Construct with an image and options
// Inserts Image* in image, but copies Options into image.
Magick::ImageRef::ImageRef ( MagickLib::Image * image_,
			     const Options * options_ )
  : _image(image_),
    _options(0),
    _refCount(1)
{
  _options = new Options( *options_ );
}

// Default constructor
Magick::ImageRef::ImageRef ( void )
  : _image(0),
    _options(new Options),
    _refCount(1)
{
  // Allocate default image
  _image = AllocateImage( _options->imageInfo() );

  // Test for error and throw exception (like throwImageException())
  throwException(_image->exception);
}

// Destructor
Magick::ImageRef::~ImageRef( void )
{
  if ( _image )
    {
      DestroyImages( _image );
      _image = (MagickLib::Image *)NULL;
    }

  delete _options;
  _options = (Options *)NULL;
}

//
// Cleanup class to ensure that ImageMagick singletons are destroyed
// so as to avoid any resemblence to a memory leak (which seems to
// confuse users)
//
namespace Magick
{

  class MagickCleanUp
  {
  public:
    ~MagickCleanUp( void )
      {
	DestroyMagickInfo();
	DestroyDelegateInfo();
      }
  };

  // The destructor for this object is invoked when the destructors for
  // static objects in this translation unit are invoked.
  static MagickCleanUp magickCleanUpGuard;
}
