// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2002
//
// Implementation of STL classes and functions
//

#include <Magick++/Image.h>
#include <Magick++/STL.h>
#define MAGICK_IMPLEMENTATION

// Add noise to image with specified noise type
Magick::addNoiseImage::addNoiseImage( Magick::NoiseType noiseType_ )
  : _noiseType( noiseType_ )
{
}
void Magick::addNoiseImage::operator()( Magick::Image &image_ )
{
  image_.addNoise( _noiseType );
}

// Annotate image (draw text on image)

// Annotate using specified text, and placement location
Magick::annotateImage::annotateImage ( const std::string &text_,
                                       const Magick::Geometry &geometry_ )
      : _text( text_ ),
	_geometry( geometry_ ),
	_gravity( Magick::NorthWestGravity ),
        _degrees( 0 )
{
}
// Annotate using specified text, bounding area, and placement gravity
Magick::annotateImage::annotateImage ( const std::string &text_,
                                       const Magick::Geometry &geometry_,
                                       const Magick::GravityType gravity_ )
  : _text( text_ ),
    _geometry( geometry_ ),
    _gravity( gravity_ ),
    _degrees( 0 )
{
}
// Annotate with text using specified text, bounding area, placement
// gravity, and rotation.
Magick::annotateImage::annotateImage ( const std::string &text_,
                    const Magick::Geometry &geometry_,
                    const Magick::GravityType gravity_,
                    const double degrees_ )
      : _text( text_ ),
        _geometry( geometry_ ),
        _gravity( gravity_ ),
        _degrees( degrees_ )
{
}
// Annotate with text (bounding area is entire image) and placement
// gravity.
Magick::annotateImage::annotateImage ( const std::string &text_,
                                       const Magick::GravityType gravity_ )
  : _text( text_ ),
    _geometry( ),
    _gravity( gravity_ ),
    _degrees( 0 )
{
}
void Magick::annotateImage::operator()( Magick::Image &image_ )
{
  image_.annotate( _text, _geometry, _gravity, _degrees );
}

// Blur image with specified blur factor
Magick::blurImage::blurImage( const double radius_, const double sigma_  )
      : _radius( radius_ ),
        _sigma( sigma_ )
{
}
void Magick::blurImage::operator()( Magick::Image &image_ )
{
  image_.blur( _radius, _sigma );
}

// Border image (add border to image)
Magick::borderImage::borderImage( const Magick::Geometry &geometry_ )
  : _geometry( geometry_ )
{
}
void Magick::borderImage::operator()( Magick::Image &image_ )
{
  image_.border( _geometry );
}

// Charcoal effect image (looks like charcoal sketch)
Magick::charcoalImage::charcoalImage( const double radius_, const double sigma_ )
      : _radius( radius_ ),
        _sigma( sigma_ )
{
}
void Magick::charcoalImage::operator()( Magick::Image &image_ )
{
  image_.charcoal( _radius, _sigma );
}

// Chop image (remove vertical or horizontal subregion of image)
Magick::chopImage::chopImage( const Magick::Geometry &geometry_ )
  : _geometry( geometry_ )
{
}
void Magick::chopImage::operator()( Magick::Image &image_ )
{
  image_.chop( _geometry );
}

// Colorize image using pen color at specified percent opacity
Magick::colorizeImage::colorizeImage( const unsigned int opacityRed_,
                                      const unsigned int opacityGreen_,
                                      const unsigned int opacityBlue_,
                                      const Magick::Color &penColor_ )
  : _opacityRed ( opacityRed_ ),
    _opacityGreen ( opacityGreen_ ),
    _opacityBlue ( opacityBlue_ ),
    _penColor( penColor_ )
{
}
Magick::colorizeImage::colorizeImage( const unsigned int opacity_,
                                      const Magick::Color &penColor_ )
  : _opacityRed ( opacity_ ),
    _opacityGreen ( opacity_ ),
    _opacityBlue ( opacity_ ),
    _penColor( penColor_ )
{
}
void Magick::colorizeImage::operator()( Magick::Image &image_ )
{
  image_.colorize( _opacityRed, _opacityGreen, _opacityBlue, _penColor );
}

// Convert the image colorspace representation
Magick::colorSpaceImage::colorSpaceImage( Magick::ColorspaceType colorSpace_ )
  : _colorSpace( colorSpace_ )
{
}
void Magick::colorSpaceImage::operator()( Magick::Image &image_ )
{
  image_.colorSpace( _colorSpace );
}

// Comment image (add comment string to image)
Magick::commentImage::commentImage( const std::string &comment_ )
  : _comment( comment_ )
{
}
void Magick::commentImage::operator()( Magick::Image &image_ )
{
  image_.comment( _comment );
}

// Compose an image onto another at specified offset and using
// specified algorithm
Magick::compositeImage::compositeImage( const Magick::Image &compositeImage_,
                                        int xOffset_,
                                        int yOffset_,
                                        Magick::CompositeOperator compose_  )
  : _compositeImage( compositeImage_ ),
    _xOffset ( xOffset_ ),
    _yOffset ( yOffset_ ),
    _compose ( compose_ )
{
}
Magick::compositeImage::compositeImage( const Magick::Image &compositeImage_,
                                        const Magick::Geometry &offset_,
                                        Magick::CompositeOperator compose_  )
  : _compositeImage( compositeImage_ ),
    _xOffset ( offset_.xOff() ),
    _yOffset ( offset_.yOff() ),
    _compose ( compose_ )
{
}
void Magick::compositeImage::operator()( Image &image_ )
{
  image_.composite( _compositeImage, _xOffset, _yOffset, _compose );
}

// Contrast image (enhance intensity differences in image)
Magick::contrastImage::contrastImage( const unsigned int sharpen_ )
  : _sharpen( sharpen_ )
{
}
void Magick::contrastImage::operator()( Magick::Image &image_ )
{
  image_.contrast( _sharpen );
}

// Crop image (subregion of original image)
Magick::cropImage::cropImage( const Magick::Geometry &geometry_ )
  : _geometry( geometry_ )
{
}
void Magick::cropImage::operator()( Magick::Image &image_ )
{
  image_.crop( _geometry );
}

// Cycle image colormap
Magick::cycleColormapImage::cycleColormapImage( const int amount_ )
  : _amount( amount_ )
{
}
void Magick::cycleColormapImage::operator()( Magick::Image &image_ )
{
  image_.cycleColormap( _amount );
}

// Despeckle image (reduce speckle noise)
Magick::despeckleImage::despeckleImage( void )
{
}
void Magick::despeckleImage::operator()( Magick::Image &image_ )
{
  image_.despeckle( );
}

// Draw on image
Magick::drawImage::drawImage( const Magick::Drawable &drawable_ )
  : _drawableList()
{
  _drawableList.push_back( drawable_ );
}
Magick::drawImage::drawImage( const std::list<Magick::Drawable> &drawable_ )
  : _drawableList( drawable_ )
{
}
void Magick::drawImage::operator()( Magick::Image &image_ )
{
  image_.draw( _drawableList );
}

// Edge image (hilight edges in image)
Magick::edgeImage::edgeImage( const double radius_ )
  : _radius( radius_ )
{
}
void Magick::edgeImage::operator()( Magick::Image &image_ )
{
  image_.edge( _radius );
}

// Emboss image (hilight edges with 3D effect)
Magick::embossImage::embossImage( void )
  : _radius( 1 ),
    _sigma( 0.5 )
{
}
Magick::embossImage::embossImage( const double radius_, const double sigma_ )
  : _radius( radius_ ),
    _sigma( sigma_ )
{
}
void Magick::embossImage::operator()( Magick::Image &image_ )
{
  image_.emboss( _radius, _sigma );
}

// Enhance image (minimize noise)
Magick::enhanceImage::enhanceImage( void )
{
}
void Magick::enhanceImage::operator()( Magick::Image &image_ )
{
  image_.enhance( );
}

// Equalize image (histogram equalization)
Magick::equalizeImage::equalizeImage( void )
{
}
void Magick::equalizeImage::operator()( Magick::Image &image_ )
{
  image_.equalize( );
}

// Color to use when filling drawn objects
Magick::fillColorImage::fillColorImage( const Magick::Color &fillColor_ )
  : _fillColor( fillColor_ )
{
}
void Magick::fillColorImage::operator()( Magick::Image &image_ )
{
  image_.fillColor( _fillColor );
}

// Flip image (reflect each scanline in the vertical direction)
Magick::flipImage::flipImage( void )
{
}
void Magick::flipImage::operator()( Magick::Image &image_ )
{
  image_.flip( );
}

// Flood-fill image with color
// Flood-fill color across pixels starting at target-pixel and
// stopping at pixels matching specified border color.  Uses current
// fuzz setting when determining color match.
Magick::floodFillColorImage::floodFillColorImage( const unsigned int x_,
                                                  const unsigned int y_,
                                                  const Magick::Color &fillColor_ )
  : _x(x_),
    _y(y_),
    _fillColor(fillColor_),
    _borderColor()
{
}
Magick::floodFillColorImage::floodFillColorImage( const Magick::Geometry &point_,
                                                  const Magick::Color &fillColor_ )
  : _x(point_.xOff()),
    _y(point_.yOff()),
    _fillColor(fillColor_),
    _borderColor()
{
}
// Flood-fill color across pixels starting at target-pixel and
// stopping at pixels matching specified border color.  Uses current
// fuzz setting when determining color match.
Magick::floodFillColorImage::floodFillColorImage( const unsigned int x_,
                                                  const unsigned int y_,
                                                  const Magick::Color &fillColor_,
                                                  const Magick::Color &borderColor_ )
  : _x(x_),
    _y(y_),
    _fillColor(fillColor_),
    _borderColor(borderColor_)
{
}
Magick::floodFillColorImage::floodFillColorImage( const Geometry &point_,
                                                  const Color &fillColor_,
                                                  const Color &borderColor_ )
  : _x(point_.xOff()),
    _y(point_.yOff()),
    _fillColor(fillColor_),
    _borderColor(borderColor_)
{
}
void Magick::floodFillColorImage::operator()( Magick::Image &image_ )
{
  if ( _borderColor.isValid() )
    {
      image_.floodFillColor( _x, _y, _fillColor, _borderColor );
    }
  else
    {
      image_.floodFillColor( _x, _y, _fillColor );
    }
}

// Flood-fill image with texture

// Flood-fill texture across pixels that match the color of the target
// pixel and are neighbors of the target pixel.  Uses current fuzz
// setting when determining color match.
Magick::floodFillTextureImage::floodFillTextureImage( const unsigned int x_,
                                                      const unsigned int y_,
                                                      const Magick::Image &texture_ )
  : _x(x_),
    _y(y_),
    _texture(texture_),
    _borderColor()
{
}
Magick::floodFillTextureImage::floodFillTextureImage( const Magick::Geometry &point_,
                                                      const Magick::Image &texture_ )
  : _x(point_.xOff()),
    _y(point_.yOff()),
    _texture(texture_),
    _borderColor()
{
}
// Flood-fill texture across pixels starting at target-pixel and
// stopping at pixels matching specified border color.  Uses current
// fuzz setting when determining color match.
Magick::floodFillTextureImage::floodFillTextureImage( const unsigned int x_,
                                                      const unsigned int y_,
                                                      const Magick::Image &texture_,
                                                      const Magick::Color &borderColor_ )
  : _x(x_),
    _y(y_),
    _texture(texture_),
    _borderColor(borderColor_)
{
}
Magick::floodFillTextureImage::floodFillTextureImage( const Magick::Geometry &point_,
                                                      const Magick::Image &texture_,
                                                      const Magick::Color &borderColor_ )
  : _x(point_.xOff()),
    _y(point_.yOff()),
    _texture(texture_),
    _borderColor(borderColor_)
{
}
void Magick::floodFillTextureImage::operator()( Magick::Image &image_ )
{
  if ( _borderColor.isValid() )
    {
      image_.floodFillTexture( _x, _y, _texture, _borderColor );
    }
  else
    {
      image_.floodFillTexture( _x, _y, _texture );
    }
}

// Flop image (reflect each scanline in the horizontal direction)
Magick::flopImage::flopImage( void )
{
}
void Magick::flopImage::operator()( Magick::Image &image_ )
{
  image_.flop( );
}

// Frame image
Magick::frameImage::frameImage( const Magick::Geometry &geometry_ )
  : _width( geometry_.width() ),
    _height( geometry_.height() ),
    _outerBevel( geometry_.xOff() ),
    _innerBevel( geometry_.yOff() )
{
}
Magick::frameImage::frameImage( const unsigned int width_, const unsigned int height_,
                                const int innerBevel_, const int outerBevel_ )
  : _width( width_ ),
    _height( height_ ),
    _outerBevel( outerBevel_ ),
    _innerBevel( innerBevel_ )
{
}
void Magick::frameImage::operator()( Magick::Image &image_ )
{
  image_.frame( _width, _height, _innerBevel, _outerBevel );
}

// Gamma correct image
Magick::gammaImage::gammaImage( const double gamma_ )
  : _gammaRed( gamma_ ),
    _gammaGreen( gamma_ ),
    _gammaBlue( gamma_ )
{
}
Magick::gammaImage::gammaImage ( const double gammaRed_,
                                 const double gammaGreen_,
                                 const double gammaBlue_ )
  : _gammaRed( gammaRed_ ),
    _gammaGreen( gammaGreen_ ),
    _gammaBlue( gammaBlue_ )
{
}
void Magick::gammaImage::operator()( Magick::Image &image_ )
{
  image_.gamma( _gammaRed, _gammaGreen, _gammaBlue );
}

// Gaussian blur image
// The number of neighbor pixels to be included in the convolution
// mask is specified by 'width_'. The standard deviation of the
// gaussian bell curve is specified by 'sigma_'.
Magick::gaussianBlurImage::gaussianBlurImage( const double width_,
                                              const double sigma_ )
  : _width( width_ ),
    _sigma( sigma_ )
{
}
void Magick::gaussianBlurImage::operator()( Magick::Image &image_ )
{
  image_.gaussianBlur( _width, _sigma );
}

// Implode image (special effect)
Magick::implodeImage::implodeImage( const double factor_  )
  : _factor( factor_ )
{
}
void Magick::implodeImage::operator()( Magick::Image &image_ )
{
  image_.implode( _factor );
}

// Set image validity. Valid images become empty (inValid) if argument
// is false.
Magick::isValidImage::isValidImage( const bool isValid_  )
  : _isValid( isValid_ )
{
}
void Magick::isValidImage::operator()( Magick::Image &image_ )
{
  image_.isValid( _isValid );
}

// Label image
Magick::labelImage::labelImage( const std::string &label_ )
  : _label( label_ )
{
}
void Magick::labelImage::operator()( Magick::Image &image_ )
{
  image_.label( _label );
}

// Extract channel from image
Magick::channelImage::channelImage( const Magick::ChannelType channel_ )
  : _channel( channel_ )
{
}
void Magick::channelImage::operator()( Magick::Image &image_ )
{
  image_.channel( _channel );
}

// Magnify image by integral size
Magick::magnifyImage::magnifyImage( void )
{
}
void Magick::magnifyImage::operator()( Magick::Image &image_ )
{
  image_.magnify( );
}

// Remap image colors with closest color from reference image
Magick::mapImage::mapImage( const Magick::Image &mapImage_ ,
                            const bool dither_ )
  : _mapImage( mapImage_ ),
    _dither( dither_ )
{
}
void Magick::mapImage::operator()( Magick::Image &image_ )
{
  image_.map( _mapImage, _dither );
}

// Floodfill designated area with a matte value
Magick::matteFloodfillImage::matteFloodfillImage( const Color &target_ ,
                                                  const unsigned int matte_,
                                                  const int x_, const int y_,
                                                  const PaintMethod method_ )
  : _target( target_ ),
    _matte( matte_ ),
    _x( x_ ),
    _y( y_ ),
    _method( method_ )
{
}
void Magick::matteFloodfillImage::operator()( Magick::Image &image_ )
{
  image_.matteFloodfill( _target, _matte, _x, _y, _method );
}

// Filter image by replacing each pixel component with the median
// color in a circular neighborhood
Magick::medianFilterImage::medianFilterImage( const double radius_  )
  : _radius( radius_ )
{
}
void Magick::medianFilterImage::operator()( Magick::Image &image_ )
{
  image_.medianFilter( _radius );
}

// Reduce image by integral size
Magick::minifyImage::minifyImage( void )
{
}
void Magick::minifyImage::operator()( Magick::Image &image_ )
{
  image_.minify( );
}

// Modulate percent hue, saturation, and brightness of an image
Magick::modulateImage::modulateImage( const double brightness_,
                                      const double saturation_,
                                      const double hue_ )
  : _brightness( brightness_ ),
    _saturation( saturation_ ),
    _hue( hue_ )
{
}
void Magick::modulateImage::operator()( Magick::Image &image_ )
{
  image_.modulate( _brightness, _saturation, _hue );
}

// Negate colors in image.  Set grayscale to only negate grayscale
// values in image.
Magick::negateImage::negateImage( const bool grayscale_  )
  : _grayscale( grayscale_ )
{
}
void Magick::negateImage::operator()( Magick::Image &image_ )
{
  image_.negate( _grayscale );
}

// Normalize image (increase contrast by normalizing the pixel values
// to span the full range of color values)
Magick::normalizeImage::normalizeImage( void )
{
}
void Magick::normalizeImage::operator()( Magick::Image &image_ )
{
  image_.normalize( );
}

// Oilpaint image (image looks like oil painting)
Magick::oilPaintImage::oilPaintImage( const double radius_ )
  : _radius( radius_ )
{
}
void Magick::oilPaintImage::operator()( Magick::Image &image_ )
{
  image_.oilPaint( _radius );
}

// Set or attenuate the image opacity channel. If the image pixels are
// opaque then they are set to the specified opacity value, otherwise
// they are blended with the supplied opacity value.  The value of
// opacity_ ranges from 0 (completely opaque) to MaxRGB. The defines
// OpaqueOpacity and TransparentOpacity are available to specify
// completely opaque or completely transparent, respectively.
Magick::opacityImage::opacityImage( const unsigned int opacity_ )
  : _opacity( opacity_ )
{
}
void Magick::opacityImage::operator()( Magick::Image &image_ )
{
  image_.opacity( _opacity );
}

// Change color of opaque pixel to specified pen color.
Magick::opaqueImage::opaqueImage( const Magick::Color &opaqueColor_,
                                  const Magick::Color &penColor_ )
  : _opaqueColor( opaqueColor_ ),
    _penColor( penColor_ )
{
}
void Magick::opaqueImage::operator()( Magick::Image &image_ )
{
  image_.opaque( _opaqueColor, _penColor );
}

// Quantize image (reduce number of colors)
Magick::quantizeImage::quantizeImage( const bool measureError_  )
  : _measureError( measureError_ )
{
}
void Magick::quantizeImage::operator()( Image &image_ )
{
  image_.quantize( _measureError );
}

// Raise image (lighten or darken the edges of an image to give a 3-D
// raised or lowered effect)
Magick::raiseImage::raiseImage( const Magick::Geometry &geometry_ ,
                                const bool raisedFlag_  )
  : _geometry( geometry_ ),
    _raisedFlag( raisedFlag_ )
{
}
void Magick::raiseImage::operator()( Magick::Image &image_ )
{
  image_.raise( _geometry, _raisedFlag );
}

// Reduce noise in image using a noise peak elimination filter
Magick::reduceNoiseImage::reduceNoiseImage( void )
  : _order(3)
{
}
Magick::reduceNoiseImage::reduceNoiseImage ( const unsigned int order_ )
      : _order(order_)
{
}
void Magick::reduceNoiseImage::operator()( Image &image_ )
{
  image_.reduceNoise( _order );
}

// Roll image (rolls image vertically and horizontally) by specified
// number of columnms and rows)
Magick::rollImage::rollImage( const Magick::Geometry &roll_ )
  : _columns( roll_.width() ),
    _rows( roll_.height() )
{
}
Magick::rollImage::rollImage( const int columns_,
                              const int rows_ )
  : _columns( columns_ ),
    _rows( rows_ )
{
}
void Magick::rollImage::operator()( Magick::Image &image_ )
{
  image_.roll( _columns, _rows );
}

// Rotate image counter-clockwise by specified number of degrees.
Magick::rotateImage::rotateImage( const double degrees_ )
  : _degrees( degrees_ )
{
}
void Magick::rotateImage::operator()( Magick::Image &image_ )
{
  image_.rotate( _degrees );
}

// Resize image by using pixel sampling algorithm
Magick::sampleImage::sampleImage( const Magick::Geometry &geometry_ )
  : _geometry( geometry_ )
{
}
void Magick::sampleImage::operator()( Magick::Image &image_ )
{
  image_.sample( _geometry );
}

// Resize image by using simple ratio algorithm
Magick::scaleImage::scaleImage( const Magick::Geometry &geometry_ )
  : _geometry( geometry_ )
{
}
void Magick::scaleImage::operator()( Magick::Image &image_ )
{
  image_.scale( _geometry );
}

// Segment (coalesce similar image components) by analyzing the
// histograms of the color components and identifying units that are
// homogeneous with the fuzzy c-means technique.  Also uses
// QuantizeColorSpace and Verbose image attributes
Magick::segmentImage::segmentImage( const double clusterThreshold_ , 
                                    const double smoothingThreshold_ )
  : _clusterThreshold( clusterThreshold_ ),
    _smoothingThreshold( smoothingThreshold_ )
{
}
void Magick::segmentImage::operator()( Magick::Image &image_ )
{
  image_.segment( _clusterThreshold, _smoothingThreshold );
}

// Shade image using distant light source
Magick::shadeImage::shadeImage( const double clusterThreshold_, 
                                const double smoothingThreshold_ )
  : _clusterThreshold( clusterThreshold_ ),
    _smoothingThreshold( smoothingThreshold_ )
{
}
void Magick::shadeImage::operator()( Magick::Image &image_ )
{
  image_.shade( _clusterThreshold, _smoothingThreshold );
}

// Sharpen pixels in image
Magick::sharpenImage::sharpenImage( const double radius_, const double sigma_ )
  : _radius( radius_ ),
    _sigma( sigma_ )
{
}
void Magick::sharpenImage::operator()( Magick::Image &image_ )
{
  image_.sharpen( _radius, _sigma );
}

// Shave pixels from image edges.
Magick::shaveImage::shaveImage( const Magick::Geometry &geometry_ )
  : _geometry( geometry_ )
{
}
void Magick::shaveImage::operator()( Magick::Image &image_ )
{
  image_.shave( _geometry );
}

// Shear image (create parallelogram by sliding image by X or Y axis)
Magick::shearImage::shearImage( const double xShearAngle_,
                                const double yShearAngle_ )
  : _xShearAngle( xShearAngle_ ),
    _yShearAngle( yShearAngle_ )
{
}
void Magick::shearImage::operator()( Magick::Image &image_ )
{
  image_.shear( _xShearAngle, _yShearAngle );
}

// Solarize image (similar to effect seen when exposing a photographic
// film to light during the development process)
Magick::solarizeImage::solarizeImage( const double factor_ )
  : _factor( factor_ )
{
}
void Magick::solarizeImage::operator()( Magick::Image &image_ )
{
  image_.solarize( _factor );
}

// Spread pixels randomly within image by specified ammount
Magick::spreadImage::spreadImage( const unsigned int amount_ )
  : _amount( amount_ )
{
}
void Magick::spreadImage::operator()( Magick::Image &image_ )
{
  image_.spread( _amount );
}

// Add a digital watermark to the image (based on second image)
Magick::steganoImage::steganoImage( const Magick::Image &waterMark_ )
  : _waterMark( waterMark_ )
{
}
void Magick::steganoImage::operator()( Magick::Image &image_ )
{
  image_.stegano( _waterMark );
}

// Create an image which appears in stereo when viewed with red-blue
// glasses (Red image on left, blue on right)
Magick::stereoImage::stereoImage( const Magick::Image &rightImage_ )
  : _rightImage( rightImage_ )
{
}
void Magick::stereoImage::operator()( Magick::Image &image_ )
{
  image_.stereo( _rightImage );
}

// Color to use when drawing object outlines
Magick::strokeColorImage::strokeColorImage( const Magick::Color &strokeColor_ )
  : _strokeColor( strokeColor_ )
{
}
void Magick::strokeColorImage::operator()( Magick::Image &image_ )
{
  image_.strokeColor( _strokeColor );
}

// Swirl image (image pixels are rotated by degrees)
Magick::swirlImage::swirlImage( const double degrees_ )
  : _degrees( degrees_ )
{
}
void Magick::swirlImage::operator()( Magick::Image &image_ )
{
  image_.swirl( _degrees );
}

// Channel a texture on image background
Magick::textureImage::textureImage( const Magick::Image &texture_ )
  : _texture( texture_ )
{
}
void Magick::textureImage::operator()( Magick::Image &image_ )
{
  image_.texture( _texture );
}

// Threshold image
Magick::thresholdImage::thresholdImage( const double threshold_ )
  : _threshold( threshold_ )
{
}
void Magick::thresholdImage::operator()( Magick::Image &image_ )
{
  image_.threshold( _threshold );
}

// Transform image based on image and crop geometries
Magick::transformImage::transformImage( const Magick::Geometry &imageGeometry_ )
  : _imageGeometry( imageGeometry_ ),
    _cropGeometry( )
{
}
Magick::transformImage::transformImage( const Magick::Geometry &imageGeometry_,
                                        const Geometry &cropGeometry_  )
  : _imageGeometry( imageGeometry_ ),
    _cropGeometry( cropGeometry_ )
{
}
void Magick::transformImage::operator()( Magick::Image &image_ )
{
  if ( _cropGeometry.isValid() )
    image_.transform( _imageGeometry, _cropGeometry );
  else
    image_.transform( _imageGeometry );
}

// Set image color to transparent
Magick::transparentImage::transparentImage( const Magick::Color& color_ )
  : _color( color_ )
{
}
void Magick::transparentImage::operator()( Magick::Image &image_ )
{
  image_.transparent( _color );
}

// Trim edges that are the background color from the image
Magick::trimImage::trimImage( void )
{
}
void Magick::trimImage::operator()( Magick::Image &image_ )
{
  image_.trim( );
}

// Map image pixels to a sine wave
Magick::waveImage::waveImage( const double amplitude_,
                              const double wavelength_ )
  : _amplitude( amplitude_ ),
    _wavelength( wavelength_ )
{
}
void Magick::waveImage::operator()( Magick::Image &image_ )
{
  image_.wave( _amplitude, _wavelength );
}

// Zoom image to specified size.
Magick::zoomImage::zoomImage( const Magick::Geometry &geometry_ )
  : _geometry( geometry_ )
{
}
void Magick::zoomImage::operator()( Magick::Image &image_ )
{
  image_.zoom( _geometry );
}
