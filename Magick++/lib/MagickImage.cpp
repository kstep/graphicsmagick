// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Implementation of Image
//

#define MAGICK_IMPLEMENTATION

#include <string>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <errno.h>

using namespace std;

#include <MagickImage.h>
#include <MagickFunctions.h>
#include <MagickLastError.h>
#include <MagickInclude.h>

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
  // Ensure that error collection object is instantiated
  LastError::instance();

  // Initialize, Allocate and Read images
  read( imageSpec_ );
}

// Create a blank image canvas of specified size and color
Magick::Image::Image( const Geometry &size_,
		      const Color &color_ )
  : _imgRef(new ImageRef)
{
  // Ensure that error collection object is instantiated
  LastError::instance();

  // xc: prefix specifies an X11 color string
  std::string imageSpec("xc:");
  imageSpec += color_;

  // Set image size
  size( size_ );

  // Initialize, Allocate and Read images
  read( imageSpec );
}

// Construct without initial images
Magick::Image::Image( void )
  : _imgRef(new ImageRef)
{
  // Ensure that error collection object is instantiated
  LastError::instance();
}

// Destructor
Magick::Image::~Image()
{
  if ( --_imgRef->_refCount == 0 )
    {
      delete _imgRef;
      _imgRef = (ImageRef*)NULL;
    }
}

// Add noise to image
void Magick::Image::addNoise( NoiseType noiseType_ )
{
  replaceImage( MagickLib::AddNoiseImage ( _imgRef->image(),
					   (MagickLib::NoiseType)noiseType_ ) );
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

  MagickLib::AnnotateInfo annotateInfo;
  MagickLib::GetAnnotateInfo( _imgRef->options()->imageInfo(),
			      &annotateInfo );

  Magick::CloneString ( &annotateInfo.text, text_ );

  if ( location_.isValid() ){
    // For some reason width and height parameters in the geometry 
    // causes AnnotateImage to place text incorrectly.
    char buffer[MaxTextExtent];
    MagickLib::FormatString(buffer,"%+d%+d", location_.xOff(), location_.yOff() );
    Magick::CloneString ( &annotateInfo.geometry, buffer );
  }

  annotateInfo.gravity = gravity_;
  annotateInfo.degrees = degrees_;

  MagickLib::AnnotateImage( _imgRef->image(), &annotateInfo );
  MagickLib::DestroyAnnotateInfo( &annotateInfo );

  throwMagickError();
}

// Text & gravity
void Magick::Image::annotate ( const std::string &text_,
			       GravityType gravity_ )
{
  modifyImage();

  MagickLib::AnnotateInfo annotateInfo;
  MagickLib::GetAnnotateInfo( _imgRef->options()->imageInfo(),
			      &annotateInfo );

  Magick::CloneString ( &annotateInfo.text, text_ );

  annotateInfo.gravity = gravity_;

  MagickLib::AnnotateImage( _imgRef->image(), &annotateInfo );
  MagickLib::DestroyAnnotateInfo( &annotateInfo );

  throwMagickError();
}

// Blur image
void Magick::Image::blur( double factor_)
{
  replaceImage( MagickLib::BlurImage( _imgRef->image(), factor_) );
}

// Add border to image
// Only uses width & height
void Magick::Image::border( const Geometry &geometry_ )
{
  MagickLib::RectangleInfo borderInfo;
  borderInfo.width  = geometry_.width();
  borderInfo.height = geometry_.height();

  replaceImage( MagickLib::BorderImage( _imgRef->image(), &borderInfo) );
}

// Charcoal-effect image
void Magick::Image::charcoal( double factor_ )
{
  modifyImage();

  quantizeDither( true );
  quantizeColorSpace( GRAYColorspace );
  quantize( );
  edge( factor_ );
  blur( factor_ );
  normalize( );
  negate( true );

  throwMagickError();
}

// Chop image
void Magick::Image::chop( const Geometry &geometry_ )
{
  MagickLib::RectangleInfo chopInfo;
  chopInfo.width  = geometry_.width();
  chopInfo.height = geometry_.height();
  chopInfo.x      = geometry_.xOff();
  if ( geometry_.xNegative() )
    chopInfo.x = - chopInfo.x;
  chopInfo.y      = geometry_.yOff();
  if ( geometry_.yNegative() )
    chopInfo.y = - chopInfo.y;

  replaceImage( MagickLib::ChopImage( _imgRef->image(), &chopInfo) );
}

// Colorize
void Magick::Image::colorize ( const Color &opaqueColor_,
			       const Color &penColor_ )
{
  if ( !opaqueColor_.isValid() )
    {
      LastError err( MagickLib::OptionError, "Opaque color argument is invalid" );
      err.throwException();
    }

  if ( !penColor_.isValid() )
    {
      LastError err( MagickLib::OptionError, "Pen color argument is invalid" );
      err.throwException();
    }

  std::string opaque_str(opaqueColor_);
  std::string pen_str(penColor_);

  modifyImage();
  MagickLib::ColorizeImage ( _imgRef->image(), opaque_str.c_str(),
			     pen_str.c_str() );
  throwMagickError();
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

  Magick::ParseImageGeometry ( offset, x, y, width, height );

  MagickLib::CompositeImage( _imgRef->image(),
			     compose_,
			     compositeImage_._imgRef->image(),
			     x, y );
  throwMagickError();
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
  Magick::ParseImageGeometry ( offset_, x, y, width, height );

  MagickLib::CompositeImage( _imgRef->image(),
			     compose_,
			     compositeImage_._imgRef->image(),
			     x, y );
  throwMagickError();
}

// Condense
void Magick::Image::condense ( void )
{
  modifyImage();
  MagickLib::CondenseImage( _imgRef->image() );
  throwMagickError();
}

// Contrast image
void Magick::Image::contrast ( unsigned int sharpen_ )
{
  modifyImage();
  MagickLib::ContrastImage ( _imgRef->image(), sharpen_ );
  throwMagickError();
}

// Crop image
void Magick::Image::crop ( const Geometry &geometry_ )
{
  MagickLib::RectangleInfo cropInfo;
  cropInfo.width  = geometry_.width();
  cropInfo.height = geometry_.height();
  cropInfo.x      = geometry_.xOff();
  if ( geometry_.xNegative() )
    cropInfo.x = - cropInfo.x;
  cropInfo.y      = geometry_.yOff();
  if ( geometry_.yNegative() )
    cropInfo.y = - cropInfo.y;

  replaceImage( MagickLib::CropImage( _imgRef->image(), &cropInfo ) );
}

// Cycle Color Map
void Magick::Image::cycleColormap ( int amount_ )
{
  modifyImage();
  MagickLib::CycleColormapImage( _imgRef->image(), amount_ );
  throwMagickError();
}

// Despeckle
void Magick::Image::despeckle ( void )
{
  replaceImage( MagickLib::DespeckleImage( _imgRef->image() ) );
}

// Display image
void Magick::Image::display( void ) const
{
  MagickLib::DisplayImages( _imgRef->options()->imageInfo(), _imgRef->image() );
}

// Draw on image using single drawable
void Magick::Image::draw ( const Drawable &drawable_ )
{
  modifyImage();
  MagickLib::AnnotateInfo annotateInfo;
  MagickLib::GetAnnotateInfo( _imgRef->options()->imageInfo(), &annotateInfo );

  // Update texture image if we have one
  if ( penTexture().isValid() )
    {
      if ( annotateInfo.tile )
	MagickLib::DestroyImage( annotateInfo.tile );
      annotateInfo.tile = MagickLib::CloneImage( penTexture().image(),
						 penTexture().columns(),
						 penTexture().rows(),
						 True);
    }

  Magick::CloneString( &annotateInfo.primitive, drawable_.primitive() );
  MagickLib::DrawImage( _imgRef->image(), &annotateInfo );
  MagickLib::DestroyAnnotateInfo( &annotateInfo );
  throwMagickError();
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
      p++;
    }

  modifyImage();
  MagickLib::AnnotateInfo annotateInfo;
  MagickLib::GetAnnotateInfo( _imgRef->options()->imageInfo(), &annotateInfo );

  // Update texture image if we have one
  if ( penTexture().isValid() )
    {
      if ( annotateInfo.tile )
	MagickLib::DestroyImage( annotateInfo.tile );
      annotateInfo.tile = MagickLib::CloneImage( penTexture().image(),
						 penTexture().columns(),
						 penTexture().rows(),
						 True);
    }

  Magick::CloneString( &annotateInfo.primitive, primitives );
  MagickLib::DrawImage( _imgRef->image(), &annotateInfo );
  MagickLib::DestroyAnnotateInfo( &annotateInfo );
  throwMagickError();
}

// Hilight edges in image
void Magick::Image::edge ( double factor_ )
{
  replaceImage( MagickLib::EdgeImage( _imgRef->image(), factor_ ) );
}

// Emboss image (hilight edges)
void Magick::Image::emboss ( void )
{
  replaceImage( MagickLib::EmbossImage( _imgRef->image() ) );
}

// Enhance image (minimize noise)
void Magick::Image::enhance ( void )
{
  replaceImage( MagickLib::EnhanceImage( _imgRef->image() ) );
}

// Equalize image (histogram equalization)
void Magick::Image::equalize ( void )
{
  modifyImage();
  MagickLib::EqualizeImage( _imgRef->image() );
  throwMagickError();
}

// Flip image (reflect each scanline in the vertical direction)
void Magick::Image::flip ( void )
{
  replaceImage( MagickLib::FlipImage( _imgRef->image() ) );
}

// Flood-fill color across pixels that match the color of the
// target pixel and are neighbors of the target pixel.
// Uses current fuzz setting when determining color match.
void Magick::Image::floodFillColor( int x_, int y_,
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
void Magick::Image::floodFillColor( int x_, int y_,
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
void Magick::Image::floodFillTexture( int x_, int y_,
				      const Magick::Image &texture_ )
{
  modifyImage();

  if (!MagickLib::UncondenseImage( _imgRef->image() ) )
    return;

  MagickLib::RunlengthPacket target;
  // FIXME: should throw exception if x or y is out of bounds
  target=(*PixelOffset( _imgRef->image(), x_ % columns(), y_ % rows() ));

  MagickLib::ColorFloodfillImage ( _imgRef->image(), &target, texture_._imgRef->image(),
				   x_, y_, FloodfillMethod );
  throwMagickError();
}
void Magick::Image::floodFillTexture( const Magick::Geometry &point_,
				      const Magick::Image &texture_ )
{
  floodFillTexture( point_.xOff(), point_.yOff(), texture_ );
}

// Flood-fill texture across pixels starting at target-pixel and
// stopping at pixels matching specified border color.
// Uses current fuzz setting when determining color match.
void Magick::Image::floodFillTexture( int x_, int y_,
				      const Magick::Image &texture_,
				      const Magick::Color &borderColor_ )
{
  modifyImage();

  MagickLib::RunlengthPacket target;
  target.red = borderColor_.redQuantum();
  target.green = borderColor_.greenQuantum();
  target.blue = borderColor_.blueQuantum();
  target.length = 1;
  target.index = 0;

  MagickLib::ColorFloodfillImage ( _imgRef->image(), &target, texture_._imgRef->image(),
				   x_, y_, FillToBorderMethod );
  throwMagickError();
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
  replaceImage( MagickLib::FlopImage( _imgRef->image() ) );
}

// Frame image
void Magick::Image::frame ( const Geometry &geometry_ )
{
  MagickLib::FrameInfo info;

  info.x           = geometry_.width();
  info.y           = geometry_.height();
  info.width       = columns() + ( info.x << 1 );
  info.height      = rows() + ( info.y << 1 );
  info.outer_bevel = geometry_.xOff();
  info.inner_bevel = geometry_.yOff();
  replaceImage( MagickLib::FrameImage( _imgRef->image(), &info ) );
}
void Magick::Image::frame ( unsigned int width_, unsigned int height_,
			    int innerBevel_, int outerBevel_ )
{
  MagickLib::FrameInfo info;
  info.x           = width_;
  info.y           = height_;
  info.width       = columns() + ( info.x << 1 );
  info.height      = rows() + ( info.y << 1 );
  info.outer_bevel = outerBevel_;
  info.inner_bevel = innerBevel_;
  replaceImage( MagickLib::FrameImage( _imgRef->image(), &info ) );
}

// Gamma correct image
void Magick::Image::gamma ( double gamma_ )
{
  char gamma[MaxTextExtent + 1];
  MagickLib::FormatString( gamma, "%3.6f", gamma_);

  modifyImage();
  MagickLib::GammaImage ( _imgRef->image(), gamma );
}
void Magick::Image::gamma ( double gammaRed_,
			    double gammaGreen_,
			    double gammaBlue_ )
{
  char gamma[MaxTextExtent + 1];
  MagickLib::FormatString( gamma, "%3.6f/%3.6f/%3.6f/",
			   gammaRed_, gammaGreen_, gammaBlue_);

  modifyImage();
  MagickLib::GammaImage ( _imgRef->image(), gamma );
  throwMagickError();
}

// Implode image
void Magick::Image::implode ( double factor_ )
{
  replaceImage( MagickLib::ImplodeImage( _imgRef->image(), factor_ ) );
}

// Label image
void Magick::Image::label ( const std::string &label_ )
{
  modifyImage();
  MagickLib::LabelImage ( _imgRef->image(), label_.c_str() );
  throwMagickError();
}

// Extract layer from image
void Magick::Image::layer ( LayerType layer_ )
{
  modifyImage();
  MagickLib::LayerImage ( _imgRef->image(), layer_ );
  throwMagickError();
}

// Magnify image by integral size
void Magick::Image::magnify ( void )
{
  replaceImage( MagickLib::MagnifyImage( _imgRef->image() ) );
}

// Remap image colors with closest color from reference image
void Magick::Image::map ( const Image &mapImage_ , bool dither_ )
{
  modifyImage();
  MagickLib::MapImage ( _imgRef->image(), mapImage_._imgRef->image(),
			dither_ );
  throwMagickError();
}

// Floodfill designated area with a matte value
void Magick::Image::matteFloodfill ( const Color &target_ ,
				     unsigned int matte_,
				     int x_, int y_,
				     Magick::PaintMethod method_ )
{
  if ( !target_.isValid() )
    {
      LastError err( MagickLib::OptionError, "Target color argument is invalid" );
      err.throwException();
    }

  MagickLib::RunlengthPacket rllPacket;
  rllPacket.red = target_.redQuantum();
  rllPacket.green = target_.greenQuantum();
  rllPacket.blue = target_.blueQuantum();
  rllPacket.length = 1;
  rllPacket.index = 0;

  modifyImage();
  MagickLib::MatteFloodfillImage ( _imgRef->image(), &rllPacket, matte_,
				   x_, y_, method_ );
  throwMagickError();
}

// Reduce image by integral size
void Magick::Image::minify ( void )
{
  replaceImage( MagickLib::MinifyImage( _imgRef->image() ) );
}

// Modulate percent hue, saturation, and brightness of an image
void Magick::Image::modulate ( double brightness_,
			       double saturation_,
			       double hue_ )
{
  char modulate[MaxTextExtent + 1];
  MagickLib::FormatString( modulate, "%3.6f/%3.6f/%3.6f",
			   brightness_, saturation_, hue_);

  modifyImage();
  MagickLib::ModulateImage( _imgRef->image(), modulate );
  throwMagickError();
}

// Negate image.  Set grayscale_ to true to effect grayscale values
// only
void Magick::Image::negate ( bool grayscale_ )
{
  modifyImage();
  MagickLib::NegateImage ( _imgRef->image(), grayscale_ );
  throwMagickError();
}

// Normalize image
void Magick::Image::normalize ( void )
{
  modifyImage();
  MagickLib::NormalizeImage ( _imgRef->image() );
  throwMagickError();
}

// Oilpaint image
void Magick::Image::oilPaint ( unsigned int radius_ )
{
  replaceImage( MagickLib::OilPaintImage( _imgRef->image(), radius_ ) );
}

// Change the color of an opaque pixel to the pen color.
void Magick::Image::opaque ( const Color &opaqueColor_,
			     const Color &penColor_ )
{
  if ( !opaqueColor_.isValid() )
    {
      LastError err( MagickLib::OptionError, "Opaque color argument is invalid" );
      err.throwException();
    }
  if ( !penColor_.isValid() )
    {
      LastError err( MagickLib::OptionError, "Pen color argument is invalid" );
      err.throwException();
    }

  std::string opaque = opaqueColor_;
  std::string pen = penColor_;

  modifyImage();
  MagickLib::OpaqueImage ( _imgRef->image(), opaque.c_str(), pen.c_str() );
  throwMagickError();
}

// Ping is similar to read except only enough of the image is read to
// determine the image columns, rows, and filesize.  Access the
// columns(), rows(), and fileSize() attributes after invoking ping.
// The image data is not valid after calling ping.
void Magick::Image::ping ( const std::string &imageSpec_ )
{
  _imgRef->options()->fileName( imageSpec_ );
  replaceImage( MagickLib::PingImage( _imgRef->options()->imageInfo() ));
}

// Quantize colors in image using current quantization settings
// Set measureError_ to true in order to measure quantization error
void Magick::Image::quantize ( bool measureError_  )
{
  modifyImage();

  if ( ( classType() == DirectClass ) ||
       ( totalColors() > _imgRef->options()->quantizeColors() ))
    MagickLib::QuantizeImage( _imgRef->options()->quantizeInfo(),
			      _imgRef->image() );

  if ( measureError_ )
    MagickLib::QuantizationError( _imgRef->image() );

  // Udate DirectClass representation of pixels
  MagickLib::SyncImage( _imgRef->image() );
  throwMagickError();
}

// Raise image (lighten or darken the edges of an image to give a 3-D
// raised or lowered effect)
void Magick::Image::raise ( const Geometry &geometry_ ,
			    bool raisedFlag_ )
{
  MagickLib::RectangleInfo raiseInfo;
  raiseInfo.width  = geometry_.width();
  raiseInfo.height = geometry_.height();
  raiseInfo.x      = geometry_.xOff();
  if ( geometry_.xNegative() )
    raiseInfo.x = - raiseInfo.x;
  raiseInfo.y      = geometry_.yOff();
  if ( geometry_.yNegative() )
    raiseInfo.y = - raiseInfo.y;

  modifyImage();
  MagickLib::RaiseImage ( _imgRef->image(), &raiseInfo, raisedFlag_ );
  throwMagickError();
}

// Read image into current object
void Magick::Image::read ( const std::string &imageSpec_ )
{
  
  _imgRef->options()->fileName( imageSpec_ );
  replaceImage( MagickLib::ReadImage( _imgRef->options()->imageInfo() ));
}

// Read image of specified size into current object
void Magick::Image::read ( const Geometry &size_,
			   const std::string &imageSpec_ )
{
  size( size_ );
  read( imageSpec_ );
}

// Reduce noise in image
void Magick::Image::reduceNoise ( void )
{
  replaceImage( MagickLib::ReduceNoiseImage( _imgRef->image() ) );
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
  replaceImage( MagickLib::RollImage( _imgRef->image(), xOff, yOff ) );
}
void Magick::Image::roll ( int columns_, int rows_ )
{
  replaceImage( MagickLib::RollImage( _imgRef->image(), columns_, rows_ ) );
}

// Rotate image
void Magick::Image::rotate ( double degrees_, bool crop_,
				  unsigned int sharpen_ )
{
  replaceImage( MagickLib::RotateImage( _imgRef->image(), degrees_, crop_,
			       sharpen_ ) );
}

// Sample image
void Magick::Image::sample ( const Geometry &geometry_ )
{
  int x = 0;
  int y = 0;
  unsigned int width = columns();
  unsigned int height = rows();
  Magick::ParseImageGeometry ( geometry_, x, y, width, height );

  replaceImage( MagickLib::SampleImage( _imgRef->image(), width, height ) );
}

// Scale image
void Magick::Image::scale ( const Geometry &geometry_ )
{
  int x = 0;
  int y = 0;
  unsigned int width = columns();
  unsigned int height = rows();
  Magick::ParseImageGeometry ( geometry_, x, y, width, height );

  replaceImage( MagickLib::ScaleImage( _imgRef->image(), width, height ) );
}

// Segment (coalesce similar image components) by analyzing the
// histograms of the color components and identifying units that are
// homogeneous with the fuzzy c-means technique.
void Magick::Image::segment ( double clusterThreshold_, 
			      double smoothingThreshold_ )
{
  modifyImage();
  MagickLib::SegmentImage ( _imgRef->image(),
			    _imgRef->options()->quantizeColorSpace(),
			    _imgRef->options()->verbose(),
			    clusterThreshold_,
			    smoothingThreshold_ );
  MagickLib::SyncImage( _imgRef->image() );
  throwMagickError();
}

// Shade image using distant light source
void Magick::Image::shade ( double azimuth_,
			    double elevation_,
			    bool   colorShading_ )
{
  replaceImage( MagickLib::ShadeImage( _imgRef->image(), colorShading_,
				       azimuth_, elevation_) );
}

// Sharpen pixels in image
void Magick::Image::sharpen ( double factor_ )
{
  replaceImage( MagickLib::SharpenImage( _imgRef->image(), factor_ ) );
}

// Shear image
void Magick::Image::shear ( double xShearAngle_,
			    double yShearAngle_,
			    bool crop_ )
{
  replaceImage( MagickLib::ShearImage( _imgRef->image(), xShearAngle_,
				       yShearAngle_, crop_ ) );
}

// Solarize image (similar to effect seen when exposing a photographic
// film to light during the development process)
void Magick::Image::solarize ( double factor_ )
{
  modifyImage();
  MagickLib::SolarizeImage ( _imgRef->image(), factor_ );
  throwMagickError();
}

// Spread pixels randomly within image by specified ammount
void Magick::Image::spread ( unsigned int amount_ )
{
  replaceImage( MagickLib::SpreadImage( _imgRef->image(), amount_ ) );
}

// Add a digital watermark to the image (based on second image)
void Magick::Image::stegano ( const Image &watermark_ )
{
  replaceImage( MagickLib::SteganoImage( _imgRef->image(),
					 watermark_._imgRef->image() ) );
}

// Stereo image (left image is current image)
void Magick::Image::stereo ( const Image &rightImage_ )
{
  replaceImage( MagickLib::StereoImage( _imgRef->image(),
					rightImage_._imgRef->image() ) );
}

// Swirl image
void Magick::Image::swirl ( double degrees_ )
{
  replaceImage( MagickLib::SwirlImage( _imgRef->image(), degrees_ ) );
}

// Texture image
void Magick::Image::texture ( const Image &texture_ )
{
  modifyImage();
  MagickLib::TextureImage( _imgRef->image(), texture_._imgRef->image() );
  throwMagickError();
}

// Threshold image
void Magick::Image::threshold ( double threshold_ )
{
  modifyImage();
  MagickLib::ThresholdImage( _imgRef->image(), threshold_ );
  throwMagickError();
}

// Transform image based on image geometry only
void Magick::Image::transform ( const Geometry &imageGeometry_ )
{
  modifyImage();
  MagickLib::TransformImage ( &(_imgRef->image()), (char *)0,
			      std::string(imageGeometry_).c_str() );
  throwMagickError();
}
// Transform image based on image and crop geometries
void Magick::Image::transform ( const Geometry &imageGeometry_,
				const Geometry &cropGeometry_ )
{
  modifyImage();
  MagickLib::TransformImage ( &(_imgRef->image()), std::string(cropGeometry_).c_str(),
			      std::string(imageGeometry_).c_str() );
  throwMagickError();
}

void Magick::Image::transformColorSpace( ColorspaceType colorSpace_ )
{
  MagickLib::Image* image = _imgRef->image();
  
  // Nothing to do?
  if ( image->colorspace == colorSpace_ )
    return;

  modifyImage();

  if ( image->colorspace == RGBColorspace ||
       image->colorspace == TransparentColorspace ||
       image->colorspace == GRAYColorspace )
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
      MagickLib::RGBTransformImage( _imgRef->image(), colorSpace_ );
      throwMagickError();
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
      MagickLib::TransformRGBImage( _imgRef->image(), colorSpace_ );
      throwMagickError();
      return;
    }
}

// Add matte image to image, setting pixels matching color to transparent
void Magick::Image::transparent ( const Color &color_ )
{
  if ( !color_.isValid() )
    {
      LastError err( MagickLib::OptionError, "Color argument is invalid." );
      err.throwException();
    }

  std::string color = color_;

  modifyImage();
  MagickLib::TransparentImage ( _imgRef->image(), color.c_str() );
  throwMagickError();
}

// Trim edges that are the background color from the image
void Magick::Image::trim ( void )
{
  // width=0, height=0 trims edges
  Geometry cropInfo(0,0);
  crop ( cropInfo );
}

// Un-condense image (Uncompresses runlength-encoded pixels packets to
// a rectangular array of pixels)
void Magick::Image::uncondense ( void )
{
  modifyImage();

  MagickLib::UncondenseImage( _imgRef->image() );

  throwMagickError();
}

// Map image pixels to a sine wave
void Magick::Image::wave ( double amplitude_, double wavelength_ )
{
  replaceImage( MagickLib::WaveImage( _imgRef->image(), amplitude_,
				      wavelength_ ) );
}

// Write image to file
void Magick::Image::write( const std::string &imageSpec_ )
{
  modifyImage();
  fileName( imageSpec_ );
  MagickLib::WriteImage( _imgRef->options()->imageInfo(), _imgRef->image() );
  throwMagickError();
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
  Magick::ParseImageGeometry ( geometry_, x, y, width, height );

  replaceImage( MagickLib::ZoomImage( _imgRef->image(), width, height) );
}

/*
 * Methods for setting image attributes
 *
 */

// Join images into a single multi-image file
void Magick::Image::adjoin ( bool flag_ )
{
  modifyImage();
  _imgRef->options()->adjoin( flag_ );
}
bool Magick::Image::adjoin ( void ) const
{
  return _imgRef->options()->adjoin();
}

// Remove pixel aliasing
void Magick::Image::antiAlias( bool flag_ )
{
  modifyImage();
  _imgRef->options()->antiAlias( static_cast<unsigned int>(flag_) );
}
bool Magick::Image::antiAlias( void )
{
  return static_cast<bool>( _imgRef->options()->antiAlias( ) );
}

void Magick::Image::animationDelay ( unsigned int delay_ )
{
  modifyImage();

  if ( _imgRef->_image )
    _imgRef->_image->delay = delay_;

  _imgRef->options()->animationDelay( delay_ );
}
unsigned int Magick::Image::animationDelay ( void ) const
{
  return _imgRef->options()->animationDelay();
  //return _imgRef->image()->delay;
}

void Magick::Image::animationIterations ( unsigned int iterations_ )
{
  modifyImage();

  if ( _imgRef->_image )
    _imgRef->_image->iterations = iterations_;

  _imgRef->options()->animationIterations( iterations_ );
}
unsigned int Magick::Image::animationIterations ( void ) const
{
  return _imgRef->options()->animationIterations( );
  //return _imgRef->image()->iterations;
}

void Magick::Image::backgroundColor ( const Color &color_ )
{
  modifyImage();

  MagickLib::Image * image = _imgRef->_image;
  if ( image )
    {
      if ( color_.isValid() )
	{
	  image->background_color.red   = color_.redQuantum();
	  image->background_color.green = color_.greenQuantum();
	  image->background_color.blue  = color_.blueQuantum();
	}
      else
	{
	  image->background_color.red   = 0;
	  image->background_color.green = 0;
	  image->background_color.blue  = 0;
	}
    }

  _imgRef->options()->backgroundColor( color_ );

}
Magick::Color Magick::Image::backgroundColor ( void ) const
{
  return _imgRef->options()->backgroundColor( );
//   Image * image = _imgRef->image();
//   return Color( image->background_color.red,
// 		      image->background_color.green,
// 		      image->background_color.blue );
}

void Magick::Image::backgroundTexture ( const std::string &backgroundTexture_ )
{
  modifyImage();
  _imgRef->options()->backgroundTexture( backgroundTexture_ );
}
std::string Magick::Image::backgroundTexture ( void ) const
{
  return _imgRef->options()->backgroundTexture( );
}

unsigned int Magick::Image::baseColumns ( void ) const
{
  return _imgRef->image()->magick_columns;
}

std::string Magick::Image::baseFilename ( void ) const
{
  return std::string(_imgRef->image()->magick_filename);
}

unsigned int Magick::Image::baseRows ( void ) const
{
  return _imgRef->image()->magick_rows;
}

void Magick::Image::borderColor ( const Color &color_ )
{
  modifyImage();

  MagickLib::Image * image = _imgRef->_image;
  if ( image )
    {
      if ( color_.isValid() )
	{
	  image->border_color.red   = color_.redQuantum();
	  image->border_color.green = color_.greenQuantum();
	  image->border_color.blue  = color_.blueQuantum();
	}
      else
	{
	  image->border_color.red   = 0;
	  image->border_color.green = 0;
	  image->border_color.blue  = 0;
	}
    }

  _imgRef->options()->borderColor( color_ );
}
Magick::Color Magick::Image::borderColor ( void ) const
{
  return _imgRef->options()->borderColor( );

//   Image * image = _imgRef->image();
//   return Color ( image->border_color.red,
// 		       image->border_color.green,
// 		       image->border_color.blue );
}

// Text bounding-box base color
void Magick::Image::boxColor ( const Color &boxColor_ )
{
  modifyImage();
  _imgRef->options()->boxColor( boxColor_ );
}
Magick::Color Magick::Image::boxColor ( void ) const
{
  return _imgRef->options()->boxColor( );
}

void Magick::Image::chromaBluePrimary ( float x_, float y_ )
{
  modifyImage();
  MagickLib::Image * image = _imgRef->image();
  image->chromaticity.blue_primary.x = x_;
  image->chromaticity.blue_primary.y = y_;
}
void Magick::Image::chromaBluePrimary ( float *x_, float *y_ ) const
{
  MagickLib::Image * image = _imgRef->image();
  *x_ = image->chromaticity.blue_primary.x;
  *y_ = image->chromaticity.blue_primary.y;
}

void Magick::Image::chromaGreenPrimary ( float x_, float y_ )
{
  modifyImage();
  MagickLib::Image * image = _imgRef->image();
  image->chromaticity.green_primary.x = x_;
  image->chromaticity.green_primary.y = y_;
}
void Magick::Image::chromaGreenPrimary ( float *x_, float *y_ ) const
{
  MagickLib::Image * image = _imgRef->image();
  *x_ = image->chromaticity.green_primary.x;
  *y_ = image->chromaticity.green_primary.y;
}

void Magick::Image::chromaRedPrimary ( float x_, float y_ )
{
  modifyImage();
  MagickLib::Image * image = _imgRef->image();
  image->chromaticity.red_primary.x = x_;
  image->chromaticity.red_primary.y = y_;
}
void Magick::Image::chromaRedPrimary ( float *x_, float *y_ ) const
{
  MagickLib::Image * image = _imgRef->image();
  *x_ = image->chromaticity.red_primary.x;
  *y_ = image->chromaticity.red_primary.y;
}

void Magick::Image::chromaWhitePoint ( float x_, float y_ )
{
  modifyImage();
  MagickLib::Image * image = _imgRef->image();
  image->chromaticity.white_point.x = x_;
  image->chromaticity.white_point.y = y_;
}
void Magick::Image::chromaWhitePoint ( float *x_, float *y_ ) const
{
  MagickLib::Image * image = _imgRef->image();
  *x_ = image->chromaticity.white_point.x;
  *y_ = image->chromaticity.white_point.y;
}

Magick::ClassType Magick::Image::classType ( void ) const
{
  return (Magick::ClassType)_imgRef->image()->c_class;
}

void Magick::Image::colorFuzz ( unsigned int fuzz_ )
{
  modifyImage();

  if ( _imgRef->_image )
    _imgRef->_image->fuzz = fuzz_;

  _imgRef->options()->colorFuzz( fuzz_ );
}
unsigned int Magick::Image::colorFuzz ( void ) const
{
  return _imgRef->options()->colorFuzz( );
//   return _imgRef->image()->fuzz;
}

void Magick::Image::colorMap ( unsigned int index_,
			       const Color &color_ )
{
  if ( color_.isValid() )
    {
      MagickLib::Image * image = _imgRef->image();
      if ( image->c_class == MagickLib::DirectClass )
	{
	  errno = 0;
	  LastError err( MagickLib::OptionError,
			 "Image class does not support colormap" );
	  err.throwException();
	}
      
      modifyImage();

      if ( index_ > image->colors )
	{
	  errno = 0;
	  LastError err( MagickLib::OptionError,
			 "color index is greater than maximum image color index" );
	  err.throwException();
	  //index_ %= image->colors;
	}

      if ( image->colormap )
	{
	  MagickLib::ColorPacket *color = image->colormap + index_;
	  
	  color->red   = color_.redQuantum();
	  color->green = color_.greenQuantum();
	  color->blue  = color_.blueQuantum();
	  return;
	}

      errno = 0;
      LastError err( MagickLib::OptionError,
		     "Image does not contain colormap" );
      err.throwException();
    }

  errno = 0;
  LastError err( MagickLib::OptionError, "Color argument is invalid" );
  err.throwException();
}
Magick::Color Magick::Image::colorMap ( unsigned int index_ ) const
{
  MagickLib::Image * image = _imgRef->image();
  if ( image->colormap )
    {

      if ( index_ > image->colors )
	{
	  errno = 0;
	  LastError err( MagickLib::OptionError,
			 "color index is greater than maximum image color index" );
	  err.throwException();
	  //index_ %= image->colors;
	}

      MagickLib::ColorPacket *color = image->colormap + index_;

      return Magick::Color( color->red, color->green, color->blue );
    }

  errno = 0;
  Magick::LastError err( MagickLib::CorruptImageError,
			 "Image does not contain colormap" );
  err.throwException();

  return Color();
}

unsigned int Magick::Image::columns ( void ) const
{
  return _imgRef->image()->columns;
}

// Comment
void Magick::Image::comment ( const std::string &comment_ )
{
  modifyImage();
  MagickLib::Image * image = _imgRef->image();

  if ( image->comments )
    {
      MagickLib::FreeMemory( image->comments );
      image->comments = 0;
    }

  if ( comment_.length() > 0 )
    {
      image->comments=MagickLib::TranslateText((ImageInfo *) NULL,
					       image,
					       comment_.c_str());
    }

  throwMagickError();
}
std::string Magick::Image::comment ( void ) const
{
  MagickLib::Image * image = _imgRef->image();
  if ( image->comments )
    return std::string( image->comments );

  return std::string(); // Intentionally no exception
}

void Magick::Image::compressType ( Magick::CompressionType compressType_ )
{
  modifyImage();

  if ( _imgRef->_image )
    _imgRef->_image->compression = compressType_;

  _imgRef->options()->compressType( compressType_ );
}
Magick::CompressionType Magick::Image::compressType ( void ) const
{
  return _imgRef->options()->compressType( );
  //  return _imgRef->image()->compression;
}

void Magick::Image::density ( const Geometry &density_ )
{
  modifyImage();
  _imgRef->options()->density( density_ );
}
Magick::Geometry Magick::Image::density ( void ) const
{
  return _imgRef->options()->density( );
}

void Magick::Image::depth ( unsigned int depth_ )
{
  modifyImage();

  if ( _imgRef->_image )
    _imgRef->_image->depth = depth_;

  _imgRef->options()->depth( depth_ );
}
unsigned int Magick::Image::depth ( void ) const
{
  if ( _imgRef->_image )
    return _imgRef->image()->depth;

  return _imgRef->options()->depth( );
}

std::string Magick::Image::directory ( void ) const
{
  MagickLib::Image * image = _imgRef->image();
  if ( image->directory )
    return std::string( image->directory );

  errno = 0;
  LastError err( MagickLib::CorruptImageWarning,
		 "Image does not contain a directory." );
  err.throwException();

  return std::string();
}

void Magick::Image::fileName ( const std::string &fileName_ )
{
  modifyImage();

  MagickLib::Image * image = _imgRef->_image;
  if ( image )
    {
      fileName_.copy( image->filename,
		      sizeof(_imgRef->image()->filename) - 1 );
      image->filename[ fileName_.length() ] = 0; // Null terminate
    }

  _imgRef->options()->fileName( fileName_ );
  
}
std::string Magick::Image::fileName ( void ) const
{
  return _imgRef->options()->fileName( );
  //  return std::string(_imgRef->image()->filename);
}

unsigned int Magick::Image::fileSize ( void ) const
{
  return _imgRef->image()->filesize;
}

void Magick::Image::filterType ( Magick::FilterType filterType_ )
{
  modifyImage();

  if ( _imgRef->_image )
    _imgRef->_image->filter = filterType_;

  // FIXME: No longer supported in ImageInfo
//   _imgRef->options()->filterType ( filterType_ );
}
Magick::FilterType Magick::Image::filterType ( void ) const
{
  // FIXME: No longer supported in ImageInfo
//   return _imgRef->options()->filterType ( );
  return _imgRef->image()->filter;
}

void Magick::Image::font ( const std::string &font_ )
{
  modifyImage();
  _imgRef->options()->font( font_ );
}
std::string Magick::Image::font ( void ) const
{
  return _imgRef->options()->font( );
}

void Magick::Image::fontPointsize ( unsigned int pointSize_ )
{
  modifyImage();
  _imgRef->options()->fontPointsize( pointSize_ );
}
unsigned int Magick::Image::fontPointsize ( void ) const
{
  return _imgRef->options()->fontPointsize( );
}

std::string Magick::Image::format ( void ) const
{
  //  return _imgRef->options()->format ( );
  const MagickLib::MagickInfo * magick_info
    = MagickLib::GetMagickInfo( _imgRef->image()->magick );

  if (( magick_info != (MagickLib::MagickInfo *)0 ) && 
      ( *magick_info->description != '\0' ))
    return std::string(magick_info->description);

  LastError err( MagickLib::CorruptImageWarning,
		 "Unrecognized image magick type." );
  err.throwException();

  return std::string();
}

double Magick::Image::gamma ( void ) const
{
  return _imgRef->image()->gamma;
}

Magick::Geometry Magick::Image::geometry ( void ) const
{
  if ( _imgRef->image()->geometry )
    {
      return Geometry(_imgRef->image()->geometry);
    }

  Magick::LastError err( MagickLib::OptionWarning,
			 "Image does not contain a geometry." );
  err.throwException();

  return Geometry();
}

void Magick::Image::gifDisposeMethod ( unsigned int disposeMethod_ )
{
  modifyImage();

  if ( _imgRef->_image )
    _imgRef->_image->dispose = disposeMethod_;

  _imgRef->options()->gifDisposeMethod( disposeMethod_ );
}
unsigned int Magick::Image::gifDisposeMethod ( void ) const
{
  // It would be better to return an enumeration

  return _imgRef->options()->gifDisposeMethod( );

//   return(_imgRef->image()->dispose);
}

// ICC color profile (BLOB)
void Magick::Image::iccColorProfile( const Magick::Blob &colorProfile_ )
{
  MagickLib::ProfileInfo * color_profile = &(_imgRef->image()->color_profile);
  if ( color_profile->info )
    {
      MagickLib::FreeMemory( color_profile->info );
      color_profile->info = 0;
    }
  color_profile->length = 0;

  if ( colorProfile_.data() != 0 )
    {
      color_profile->info =
	static_cast<unsigned char*>(MagickLib::AllocateMemory(colorProfile_.length()));

      if ( color_profile->info == 0 )
	throw std::bad_alloc();

      memcpy( color_profile->info, colorProfile_.data(), colorProfile_.length());
      color_profile->length = colorProfile_.length();
    }
}
Magick::Blob Magick::Image::iccColorProfile( void ) const
{
  MagickLib::ProfileInfo * color_profile = &(_imgRef->image()->color_profile);
  return Blob( color_profile->info, color_profile->length );
}

void Magick::Image::interlaceType ( Magick::InterlaceType interlace_ )
{
  modifyImage();

  if ( _imgRef->_image )
    _imgRef->_image->interlace = interlace_;

  _imgRef->options()->interlaceType ( interlace_ );
}
Magick::InterlaceType Magick::Image::interlaceType ( void ) const
{
  return _imgRef->options()->interlaceType ( );
  //  return _imgRef->image()->interlace;
}

// IPTC profile (BLOB)
void Magick::Image::iptcProfile( const Magick::Blob &iptcProfile_ )
{
  MagickLib::ProfileInfo * iptc_profile = &(_imgRef->image()->iptc_profile);
  if ( iptc_profile->info )
    {
      MagickLib::FreeMemory( iptc_profile->info );
      iptc_profile->info = 0;
    }
  iptc_profile->length = 0;

  if ( iptcProfile_.data() != 0 )
    {
      iptc_profile->info =
	static_cast<unsigned char*>(MagickLib::AllocateMemory(iptcProfile_.length()));

      if ( iptc_profile->info == 0 )
	throw std::bad_alloc();

      memcpy( iptc_profile->info, iptcProfile_.data(), iptcProfile_.length());

      iptc_profile->length = iptcProfile_.length();
    }
}
Magick::Blob Magick::Image::iptcProfile( void ) const
{
  MagickLib::ProfileInfo * iptc_profile = &(_imgRef->image()->iptc_profile);
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
  if ( _imgRef->_image )
    return true;

  return false;
}

std::string Magick::Image::label ( void ) const
{

  MagickLib::Image *image = _imgRef->image();
  if ( image->label )
    return image->label;

  return std::string();
}

// Linewidth for drawing lines, circles, ellipses, etc.
void Magick::Image::lineWidth ( unsigned int lineWidth_ )
{
  modifyImage();

  _imgRef->options()->lineWidth( lineWidth_ );
}
unsigned int Magick::Image::lineWidth ( void ) const
{
  return _imgRef->options()->lineWidth( );
}

void Magick::Image::magick ( const std::string &magick_ )
{
  modifyImage();

  if ( _imgRef->_image )
    {
      magick_.copy( _imgRef->_image->magick,
		    sizeof(_imgRef->_image->magick) - 1 );
      _imgRef->_image->magick[ magick_.length() ] = 0;
    }

  _imgRef->options()->magick( magick_ );
}
std::string Magick::Image::magick ( void ) const
{
  if ( _imgRef->_image && *_imgRef->_image->magick != '\0' )
    return std::string(_imgRef->image()->magick);

  return _imgRef->options()->magick( );
}

void Magick::Image::matte ( bool matteFlag_ )
{
  modifyImage();
  _imgRef->image()->matte = matteFlag_;
}
bool Magick::Image::matte ( void ) const
{
  if ( _imgRef->image()->matte )
    return true;
  else
    return false;
}

void Magick::Image::matteColor ( const Color &matteColor_ )
{
  modifyImage();
  
  MagickLib::Image * image = _imgRef->_image;
  if ( image )
    {
      if ( matteColor_.isValid() )
	{
	  image->matte_color.red   = matteColor_.redQuantum();
	  image->matte_color.green = matteColor_.greenQuantum();
	  image->matte_color.blue  = matteColor_.blueQuantum();
	}
    }
  else
    {
      // Set to default matte color
      Color tmpColor( "BDBDBD" );
      image->matte_color.red   = tmpColor.redQuantum();
      image->matte_color.green = tmpColor.greenQuantum();
      image->matte_color.blue  = tmpColor.blueQuantum();
    }
  
  _imgRef->options()->matteColor( matteColor_ );
}
Magick::Color Magick::Image::matteColor ( void ) const
{
  if ( _imgRef->_image )
    return Color( _imgRef->_image->matte_color.red,
		  _imgRef->_image->matte_color.green,
		  _imgRef->_image->matte_color.blue );

  return _imgRef->options()->matteColor( );
}

double Magick::Image::meanErrorPerPixel ( void ) const
{
  return(_imgRef->image()->mean_error_per_pixel);
}

void Magick::Image::monochrome ( bool monochromeFlag_ )
{
  modifyImage();

  _imgRef->options()->monochrome( monochromeFlag_ );
}
bool Magick::Image::monochrome ( void ) const
{
  return _imgRef->options()->monochrome( );
}

Magick::Geometry Magick::Image::montageGeometry ( void ) const
{
  MagickLib::Image * image = _imgRef->image();
  if ( image->montage )
    return Magick::Geometry(image->montage);

  LastError err( MagickLib::CorruptImageWarning,
		 "Image does not contain a montage." );
  err.throwException();

  return Magick::Geometry();
}

double Magick::Image::normalizedMaxError ( void ) const
{
  return(_imgRef->image()->normalized_maximum_error);
}

double Magick::Image::normalizedMeanError ( void ) const
{
  return _imgRef->image()->normalized_mean_error;
}

unsigned int Magick::Image::packets ( void ) const
{
  return _imgRef->image()->packets;
}

unsigned int Magick::Image::packetSize ( void ) const
{
  return _imgRef->image()->packet_size;
}

void Magick::Image::penColor ( const Color &penColor_ )
{
  modifyImage();
  _imgRef->options()->penColor( penColor_ );
}
Magick::Color Magick::Image::penColor ( void  ) const
{
  return _imgRef->options()->penColor( );
}

void Magick::Image::penTexture ( const Image &penTexture_ )
{
  modifyImage();
  _imgRef->options()->penTexture( penTexture_._imgRef->_image );
}

Magick::Image  Magick::Image::penTexture ( void  ) const
{
  Image texture;
  
  const MagickLib::Image* tmpTexture =_imgRef->options()->penTexture( );

  if ( tmpTexture )
    texture.replaceImage( MagickLib::CloneImage( tmpTexture,
						 tmpTexture->columns,
						 tmpTexture->rows,
						 True ) );
  return texture;
//   return _imgRef->options()->penTexture( );
}

void Magick::Image::pixelColor ( unsigned int x_, unsigned int y_,
				 const Color &color_ )
{
  if ( color_.isValid() )
    {

      MagickLib::Image * image = _imgRef->image();

      // Test arguments to ensure they are within the image.
      if ( y_ > image->rows || x_ > image->columns )
	{
	  Magick::LastError err( MagickLib::OptionError,
				 "Access outside of image boundary." );
	  err.throwException();
	}

      modifyImage();

      // Uncondense image into rectangular array of packets
      if ( !MagickLib::UncondenseImage( image ) )
	return;

      // Calculate location of color packet
      MagickLib::RunlengthPacket* packet = image->pixels+(y_*image->columns+x_);

      // Updating DirectClass pixels invalidates colormap so set to DirectClass type
      image->c_class = MagickLib::DirectClass;

      // Set RGB
      packet->red   = color_.redQuantum();
      packet->green = color_.greenQuantum();
      packet->blue  = color_.blueQuantum();

      // Set alpha
      if ( classType() == DirectClass )
	packet->index = color_.alphaQuantum();

      return;
    }

  LastError err( MagickLib::OptionError, "Color argument is invalid" );
  err.throwException();
}
Magick::Color Magick::Image::pixelColor ( unsigned int x_,
					  unsigned int y_ ) const
{
  // Access image.
  MagickLib::Image * image = _imgRef->image();
  if ( !image->pixels )
    {
      Magick::LastError err( MagickLib::CorruptImageError,
			     "Image does not contain pixel data." );
      err.throwException();
    }

  // Uncondense image into rectangular array of packets
  if ( !MagickLib::UncondenseImage( image ) ) 
    {
      Magick::LastError *err = LastError::instance();
      err->throwException();
    }

  // Test arguments to ensure they are within the image.
  if ( y_ > image->rows || x_ > image->columns )
    {
      Magick::LastError err( MagickLib::OptionError,
			     "Access outside of image boundary." );
      err.throwException();
    }

  // Calculate location of color packet
  MagickLib::RunlengthPacket* packet = image->pixels+(y_*image->columns+x_);

  // If DirectClass and support a matte plane, then alpha is supported.
  if ( matte() && classType() == DirectClass )
    return Color ( packet->red,
		   packet->green,
		   packet->blue,
		   packet->index );
  
  return Color ( packet->red,
		 packet->green,
		 packet->blue );
}

void Magick::Image::psPageSize ( const Magick::Geometry &pageSize_ )
{
  modifyImage();
  _imgRef->options()->psPageSize( pageSize_ );

  if ( _imgRef->_image )
    Magick::CloneString( &_imgRef->_image->page,
			 _imgRef->options()->psPageSize() );

}
Magick::Geometry Magick::Image::psPageSize ( void ) const
{
  return _imgRef->options()->psPageSize();
}

void Magick::Image::quality ( unsigned int quality_ )
{
  modifyImage();
  _imgRef->options()->quality( quality_ );
}
unsigned int Magick::Image::quality ( void ) const
{
  return _imgRef->options()->quality( );
}

void Magick::Image::quantizeColors ( unsigned int colors_ )
{
  modifyImage();
  _imgRef->options()->quantizeColors( colors_ );
}
unsigned int Magick::Image::quantizeColors ( void ) const
{
  return _imgRef->options()->quantizeColors( );
}

void Magick::Image::quantizeColorSpace ( Magick::ColorspaceType colorSpace_ )
{
  modifyImage();
  _imgRef->options()->quantizeColorSpace( colorSpace_ );
}
Magick::ColorspaceType Magick::Image::quantizeColorSpace ( void ) const
{
  return _imgRef->options()->quantizeColorSpace( );
}

void Magick::Image::quantizeDither ( bool ditherFlag_ )
{
  modifyImage();
  _imgRef->options()->quantizeDither( ditherFlag_ );
}
bool Magick::Image::quantizeDither ( void ) const
{
  return _imgRef->options()->quantizeDither( );
}

// Quantization error.  Only valid if verbose is set to true
// prior to executing quantize and the value is read back
// immediately.
unsigned int Magick::Image::quantizeError ( void )
{
  return _imgRef->image()->total_colors;
}

void Magick::Image::quantizeTreeDepth ( unsigned int treeDepth_ )
{
  modifyImage();
  _imgRef->options()->quantizeTreeDepth( treeDepth_ );
}
unsigned int Magick::Image::quantizeTreeDepth ( void ) const
{
  return _imgRef->options()->quantizeTreeDepth( );
}

void Magick::Image::renderingIntent ( Magick::RenderingIntent renderingIntent_ )
{
  modifyImage();
  _imgRef->image()->rendering_intent = renderingIntent_;
}
Magick::RenderingIntent Magick::Image::renderingIntent ( void ) const
{
  return (Magick::RenderingIntent)_imgRef->image()->rendering_intent;
}

void Magick::Image::resolutionUnits ( Magick::ResolutionType resolutionUnits_ )
{
  modifyImage();

  if ( _imgRef->_image )
    _imgRef->_image->units = resolutionUnits_;

  _imgRef->options()->resolutionUnits( resolutionUnits_ );
}
Magick::ResolutionType Magick::Image::resolutionUnits ( void ) const
{
  return _imgRef->options()->resolutionUnits( );
  //  return _imgRef->image()->units;
}

unsigned int Magick::Image::rows ( void ) const
{
  return _imgRef->image()->rows;
}

void Magick::Image::scene ( unsigned int scene_ )
{
  modifyImage();
  _imgRef->image()->scene = scene_;
}
unsigned int Magick::Image::scene ( void ) const
{
  return _imgRef->image()->scene;
}

std::string Magick::Image::signature ( bool force_ ) const
{
  // Unlike most operations that modify image attributes, this the
  // signature operation is not considered a modification of image
  // data so we do not force a copy of the image if we calculate its
  // signature.

  // Re-calculate image signature
  MagickLib::Image * image = _imgRef->image();
  if ( force_ ||
       !image->signature ||
       image->tainted )
    {
      MagickLib::SignatureImage( image );
    }

  return std::string( image->signature );
}

void Magick::Image::size ( const Geometry &geometry_ )
{
  modifyImage();
  _imgRef->options()->size( geometry_ );
}
Magick::Geometry Magick::Image::size ( void ) const
{
  return _imgRef->options()->size( );
}

void Magick::Image::subImage ( unsigned int subImage_ )
{
  modifyImage();
  _imgRef->options()->subImage( subImage_ );
}
unsigned int Magick::Image::subImage ( void ) const
{
  return _imgRef->options()->subImage( );
}

void Magick::Image::subRange ( unsigned int subRange_ )
{
  modifyImage();
  _imgRef->options()->subRange( subRange_ );
}
unsigned int Magick::Image::subRange ( void ) const
{
  return _imgRef->options()->subRange( );
}

std::string Magick::Image::text ( void ) const
{
  MagickLib::Image * image = _imgRef->image();
  if ( image->text )
    return std::string(image->text);

  return std::string(); // Intentionally no exception
}

void Magick::Image::tileName ( const std::string &tileName_ )
{
  modifyImage();
  _imgRef->options()->tileName( tileName_ );
}
std::string Magick::Image::tileName ( void ) const
{
  return _imgRef->options()->tileName( );
}

unsigned long Magick::Image::totalColors ( void ) const
{
  return MagickLib::GetNumberColors( _imgRef->image(), (FILE *) NULL);
}

Magick::ImageType Magick::Image::type ( void ) const
{
  return (Magick::ImageType)MagickLib::GetImageType( _imgRef->image() );
}

void Magick::Image::verbose ( bool verboseFlag_ )
{
  modifyImage();
  _imgRef->options()->verbose( verboseFlag_ );
}
bool Magick::Image::verbose ( void ) const
{
  return _imgRef->options()->verbose( );
}

void Magick::Image::view ( const std::string &view_ )
{
  modifyImage();
  _imgRef->options()->view( view_ );
}
std::string Magick::Image::view ( void ) const
{
  return _imgRef->options()->view( );
}

void Magick::Image::x11Display ( const std::string &display_ )
{
  modifyImage();
  _imgRef->options()->x11Display( display_ );
}
std::string Magick::Image::x11Display ( void ) const
{
  return _imgRef->options()->x11Display( );
}

double Magick::Image::xResolution ( void ) const
{
  return _imgRef->image()->x_resolution;
}
double Magick::Image::yResolution ( void ) const
{
  return _imgRef->image()->y_resolution;
}

// Copy Constructor
Magick::Image::Image( const Image & image_ )
  : _imgRef(image_._imgRef)
{
  // Increase reference count
  ++_imgRef->_refCount;
}

// Operator =
Magick::Image Magick::Image::operator=( const Magick::Image &image_ )
{
  ++image_._imgRef->_refCount;
  if ( --_imgRef->_refCount == 0 )
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
// Construct using MagickLib::Image and Magick::Options
//
Magick::Image::Image ( MagickLib::Image* image_, Magick::Options* options_ )
  : _imgRef(new ImageRef( image_, options_) )
{
}

//
// Replace current image
//
MagickLib::Image * Magick::Image::replaceImage( MagickLib::Image* replacement_ )
{
  if ( _imgRef->_refCount == 1 )
    {
      // We own the image.  Destroy existing image.
      if ( _imgRef->_image )
	MagickLib::DestroyImages( _imgRef->_image );
	  
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

  throwMagickError();

  return replacement_;
}

//
// Prepare to modify image pixels
// Replace current image with copy if reference count > 1
//
void Magick::Image::modifyImage( void )
{
  // Nothing to do if we are sole owner of image
  if ( _imgRef->_refCount == 1 )
    return;

  MagickLib::Image* image = _imgRef->image();
  replaceImage( MagickLib::CloneImage( image,
				       image->columns,
				       image->rows,
				       True) );
  return;
}

//
// Test for an ImageMagick reported error and throw exception if one
// has been reported.
//
void Magick::Image::throwMagickError( void )
{
  LastError* errPtr = LastError::instance();
  if ( errPtr->isError() )
    errPtr->throwException();
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

// // Construct with an image and options
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
}

// Destructor
Magick::ImageRef::~ImageRef( void )
{
  if ( _image )
    {
      MagickLib::DestroyImages( _image );
      _image = (MagickLib::Image *)NULL;
    }

  delete _options;
  _options = (Options *)NULL;
}

// Report Image missing error (throws exception)
void Magick::ImageRef::imageMissing ( void ) const
{
  LastError err( MagickLib::UndefinedError,
		 "Object does not contain image." );
  err.throwException();
}

// Report options missing error (throws exception)
void Magick::ImageRef::optionsMissing( void ) const
{
      Magick::LastError err( MagickLib::UndefinedError,
			     "Object does not contain options." );
      err.throwException();
}
