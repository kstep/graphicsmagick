// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001
//
// Definition and implementation of template functions for using
// Magick::Image with STL containers.
//

#ifndef Magick_STL_header
#define Magick_STL_header

#include <algorithm>
#include <functional>
#include <iterator>
#include "Magick++/CoderInfo.h"
#include "Magick++/Drawable.h"
#include "Magick++/Exception.h"
#include "Magick++/Montage.h"
#include "Magick++/Include.h"
#include "Magick++/Functions.h"

namespace Magick
{
  //
  // STL function object declarations/definitions
  //

  // Function objects provide the means to invoke an operation on one
  // or more image objects in an STL-compatable container.  The
  // arguments to the function object constructor(s) are compatable
  // with the arguments to the equivalent Image class method and
  // provide the means to supply these options when the function
  // object is invoked.

  // For example, to read a GIF animation, set the color red to
  // transparent for all frames, and write back out:
  //
  // list<image> images;
  // readImages( &images, "animation.gif" );
  // for_each( images.begin(), images.end(), transparentImage( "red" ) );
  // writeImages( images.begin(), images.end(), "animation.gif" );
  
  // Add noise to image with specified noise type
  class addNoiseImage : public std::unary_function<Image&,void>
  {
  public:
    addNoiseImage ( NoiseType noiseType_ )
      : _noiseType( noiseType_ ) { }

    void operator()( Image &image_ )
      {
	image_.addNoise( _noiseType );
      }
  private:
    NoiseType _noiseType;
  };

  // Annotate image (draw text on image)
  class annotateImage : public std::unary_function<Image&,void>
  {
  public:
    // Annotate using specified text, and placement location
    annotateImage ( const std::string &text_,
                    const Geometry &geometry_ )
      : _text( text_ ),
	_geometry( geometry_ ),
	_gravity( NorthWestGravity ),
        _degrees( 0 ) { }
    // Annotate using specified text, bounding area, and placement
    // gravity
    annotateImage ( const std::string &text_,
		    const Geometry &geometry_,
		    GravityType gravity_ )
      : _text( text_ ),
	_geometry( geometry_ ),
	_gravity( gravity_ ),
        _degrees( 0 ) { }
    // Annotate with text using specified text, bounding area,
    // placement gravity, and rotation.
    annotateImage ( const std::string &text_,
                    const Geometry &geometry_,
                    GravityType gravity_,
                    double degrees_ )
      : _text( text_ ),
        _geometry( geometry_ ),
        _gravity( gravity_ ),
        _degrees( degrees_ ) { }
    // Annotate with text (bounding area is entire image) and
    // placement gravity.
    annotateImage ( const std::string &text_,
		    GravityType gravity_ )
      : _text( text_ ),
	_geometry( ),
	_gravity( gravity_ ),
        _degrees( 0 ) { }
    
    void operator()( Image &image_ )
      {
        image_.annotate( _text, _geometry, _gravity, _degrees );
      }
  private:
    const std::string   _text;
    const Geometry      _geometry;
    const GravityType   _gravity;
    const double        _degrees;
  };

  // Blur image with specified blur factor
  class blurImage : public std::unary_function<Image&,void>
  {
  public:
    blurImage( const double radius_ = 1, const double sigma_ = 0.5 )
      : _radius( radius_ ),
        _sigma( sigma_ ){ }

    void operator()( Image &image_ )
      {
	image_.blur( _radius, _sigma );
      }
  private:
    const double _radius;
    const double _sigma;
  };

  // Border image (add border to image)
  class borderImage : public std::unary_function<Image&,void>
  {
  public:
    borderImage( const Geometry &geometry_ = borderGeometryDefault  )
      : _geometry( geometry_ ) { }

    void operator()( Image &image_ )
      {
	image_.border( _geometry );
      }
  private:
    const Geometry _geometry;
  };

  // Charcoal effect image (looks like charcoal sketch)
  class charcoalImage : public std::unary_function<Image&,void>
  {
  public:
    charcoalImage( const double radius_ = 1, const double sigma_ = 0.5  )
      : _radius( radius_ ),
        _sigma( sigma_ ) { }

    void operator()( Image &image_ )
      {
	image_.charcoal( _radius, _sigma );
      }
  private:
    const double _radius;
    const double _sigma;
  };

  // Chop image (remove vertical or horizontal subregion of image)
  class chopImage : public std::unary_function<Image&,void>
  {
  public:
    chopImage( const Geometry &geometry_ )
      : _geometry( geometry_ ) { }

    void operator()( Image &image_ )
      {
	image_.chop( _geometry );
      }
  private:
    const Geometry _geometry;
  };

  // Colorize image using pen color at specified percent opacity
  class colorizeImage : public std::unary_function<Image&,void>
  {
  public:
    colorizeImage( const unsigned int opacityRed_,
                   const unsigned int opacityGreen_,
                   const unsigned int opacityBlue_,
		   const Color &penColor_ )
      : _opacityRed ( opacityRed_ ),
        _opacityGreen ( opacityGreen_ ),
        _opacityBlue ( opacityBlue_ ),
	_penColor( penColor_ ) { }
    colorizeImage( const unsigned int opacity_,
                   const Color &penColor_ )
      : _opacityRed ( opacity_ ),
        _opacityGreen ( opacity_ ),
        _opacityBlue ( opacity_ ),
	_penColor( penColor_ ) { }

    void operator()( Image &image_ )
      {
	image_.colorize( _opacityRed, _opacityGreen, _opacityBlue, _penColor );
      }
  private:
    const unsigned int _opacityRed;
    const unsigned int _opacityGreen;
    const unsigned int _opacityBlue;
    const Color _penColor;
  };

  // Convert the image colorspace representation
  class colorSpaceImage : public std::unary_function<Image&,void>
  {
  public:
    colorSpaceImage( ColorspaceType colorSpace_ )
      : _colorSpace( colorSpace_ ) { }

    void operator()( Image &image_ )
      {
	image_.colorSpace( _colorSpace );
      }
  private:
    const ColorspaceType _colorSpace;
  };

  // Comment image (add comment string to image)
  class commentImage : public std::unary_function<Image&,void>
  {
  public:
    commentImage( const std::string &comment_ )
      : _comment( comment_ ) { }

    void operator()( Image &image_ )
      {
	image_.comment( _comment );
      }
  private:
    const std::string _comment;
  };

  // Compose an image onto another at specified offset and using
  // specified algorithm
  class compositeImage : public std::unary_function<Image&,void>
  {
  public:
    compositeImage( const Image &compositeImage_,
		    int xOffset_,
		    int yOffset_,
		    CompositeOperator compose_ = InCompositeOp )
      : _compositeImage( compositeImage_ ),
	_xOffset ( xOffset_ ),
	_yOffset ( yOffset_ ),
	_compose ( compose_ ) {}
    compositeImage( const Image &compositeImage_,
		    const Geometry &offset_,
		    CompositeOperator compose_ = InCompositeOp )
      : _compositeImage( compositeImage_ ),
	_xOffset ( offset_.xOff() ),
	_yOffset ( offset_.yOff() ),
	_compose ( compose_ ) {}
    
    void operator()( Image &image_ )
      {
	image_.composite( _compositeImage, _xOffset, _yOffset, _compose );
      }
  private:
    const Image             _compositeImage;
    const int               _xOffset;
    const int               _yOffset;
    const CompositeOperator _compose;
  };

  // Contrast image (enhance intensity differences in image)
  class contrastImage : public std::unary_function<Image&,void>
  {
  public:
    contrastImage( unsigned int sharpen_ )
      : _sharpen( sharpen_ ) { }

    void operator()( Image &image_ )
      {
	image_.contrast( _sharpen );
      }
  private:
    const unsigned int _sharpen;
  };

  // Crop image (subregion of original image)
  class cropImage : public std::unary_function<Image&,void>
  {
  public:
    cropImage( const Geometry &geometry_ )
      : _geometry( geometry_ ) { }

    void operator()( Image &image_ )
      {
	image_.crop( _geometry );
      }
  private:
    const Geometry _geometry;
  };

  // Cycle image colormap
  class cycleColormapImage : public std::unary_function<Image&,void>
  {
  public:
    cycleColormapImage( int amount_ )
      : _amount( amount_ ) { }

    void operator()( Image &image_ )
      {
	image_.cycleColormap( _amount );
      }
  private:
    const int _amount;
  };

  // Despeckle image (reduce speckle noise)
  class despeckleImage : public std::unary_function<Image&,void>
  {
  public:
    despeckleImage( void ) { }

    void operator()( Image &image_ )
      {
	image_.despeckle( );
      }
  private:
  };

  // Draw on image
  class drawImage : public std::unary_function<Image&,void>
  {
  public:
    // Draw on image using a single drawable
    // Store in list to make implementation easier
    drawImage( const Drawable &drawable_ )
      : _drawableList()
      {
        _drawableList.push_back( drawable_ );
      }
    // Draw on image using a drawable list
    drawImage( const std::list<Drawable> &drawable_ )
      : _drawableList( drawable_ ) {}

    void operator()( Image &image_ )
      {
	image_.draw( _drawableList );
      }

  private:
    std::list<Drawable> _drawableList;
  };

  // Edge image (hilight edges in image)
  class edgeImage : public std::unary_function<Image&,void>
  {
  public:
    edgeImage( double radius_ = 0.0  )
      : _radius( radius_ ) { }

    void operator()( Image &image_ )
      {
	image_.edge( _radius );
      }
  private:
    const double _radius;
  };

  // Emboss image (hilight edges with 3D effect)
  class embossImage : public std::unary_function<Image&,void>
  {
  public:
    embossImage( void )
      : _radius( 1 ),
        _sigma( 0.5 ) { }
    embossImage( const double radius_, const double sigma_ )
      : _radius( radius_ ),
        _sigma( sigma_ ) { }

    void operator()( Image &image_ )
      {
	image_.emboss( _radius, _sigma );
      }
  private:
    const double _radius;
    const double _sigma;
  };

  // Enhance image (minimize noise)
  class enhanceImage : public std::unary_function<Image&,void>
  {
  public:
    enhanceImage( void ) { }

    void operator()( Image &image_ )
      {
	image_.enhance( );
      }
  private:
  };

  // Equalize image (histogram equalization)
  class equalizeImage : public std::unary_function<Image&,void>
  {
  public:
    equalizeImage( void ) { }

    void operator()( Image &image_ )
      {
	image_.equalize( );
      }
  private:
  };

  // Color to use when filling drawn objects
  class fillColorImage : public std::unary_function<Image&,void>
  {
  public:
    fillColorImage( const Color &fillColor_ )
      : _fillColor( fillColor_ ) { }

    void operator()( Image &image_ )
      {
	image_.fillColor( _fillColor );
      }
  private:
    const Color _fillColor;
  };

  // Flip image (reflect each scanline in the vertical direction)
  class flipImage : public std::unary_function<Image&,void>
  {
  public:
    flipImage( void ) { }

    void operator()( Image &image_ )
      {
	image_.flip( );
      }
  private:
  };

  // Flood-fill image with color
  class floodFillColorImage : public std::unary_function<Image&,void>
  {
  public:
    // Flood-fill color across pixels starting at target-pixel and
    // stopping at pixels matching specified border color.
    // Uses current fuzz setting when determining color match.
    floodFillColorImage( unsigned int x_, unsigned int y_,
			 const Color &fillColor_ )
      : _x(x_),
	_y(y_),
	_fillColor(fillColor_),
	_borderColor() { }
    floodFillColorImage( const Geometry &point_,
			 const Color &fillColor_ )
      : _x(point_.xOff()),
	_y(point_.yOff()),
	_fillColor(fillColor_),
	_borderColor() { }

    // Flood-fill color across pixels starting at target-pixel and
    // stopping at pixels matching specified border color.
    // Uses current fuzz setting when determining color match.
    floodFillColorImage( unsigned int x_, unsigned int y_,
			 const Color &fillColor_,
			 const Color &borderColor_ )
      : _x(x_),
	_y(y_),
	_fillColor(fillColor_),
	_borderColor(borderColor_) { }
    floodFillColorImage( const Geometry &point_,
			 const Color &fillColor_,
			 const Color &borderColor_ )
      : _x(point_.xOff()),
	_y(point_.yOff()),
	_fillColor(fillColor_),
	_borderColor(borderColor_) { }

    void operator()( Image &image_ )
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
  private:
    const unsigned int   _x;
    const unsigned int   _y;
    const Color          _fillColor;
    const Color          _borderColor;
  };

  // Flood-fill image with texture
  class floodFillTextureImage : public std::unary_function<Image&,void>
  {
  public:
    // Flood-fill texture across pixels that match the color of the
    // target pixel and are neighbors of the target pixel.
    // Uses current fuzz setting when determining color match.
    floodFillTextureImage( unsigned int x_, unsigned int y_,
			   const Image &texture_ )
      : _x(x_),
	_y(y_),
	_texture(texture_),
	_borderColor() { }
    floodFillTextureImage( const Geometry &point_,
			   const Image &texture_ )
      : _x(point_.xOff()),
	_y(point_.yOff()),
	_texture(texture_),
	_borderColor() { }

    // Flood-fill texture across pixels starting at target-pixel and
    // stopping at pixels matching specified border color.
    // Uses current fuzz setting when determining color match.
    floodFillTextureImage( unsigned int x_, unsigned int y_,
			   const Image &texture_,
			   const Color &borderColor_ )
      : _x(x_),
	_y(y_),
	_texture(texture_),
	_borderColor(borderColor_) { }
    floodFillTextureImage( const Geometry &point_,
			   const Image &texture_,
			   const Color &borderColor_ )
      : _x(point_.xOff()),
	_y(point_.yOff()),
	_texture(texture_),
	_borderColor(borderColor_) { }

    void operator()( Image &image_ )
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
  private:
    const unsigned int  _x;
    const unsigned int  _y;
    const Image         _texture;
    const Color         _borderColor;
  };

  // Flop image (reflect each scanline in the horizontal direction)
  class flopImage : public std::unary_function<Image&,void>
  {
  public:
    flopImage( void ) { }

    void operator()( Image &image_ )
      {
	image_.flop( );
      }
  private:
  };

  // Frame image
  class frameImage : public std::unary_function<Image&,void>
  {
  public:
    frameImage( const Geometry &geometry_ = frameGeometryDefault )
      : _width( geometry_.width() ),
	_height( geometry_.height() ),
	_outerBevel( geometry_.xOff() ),
	_innerBevel( geometry_.yOff() ) { }
    frameImage( unsigned int width_, unsigned int height_,
		int innerBevel_ = 6, int outerBevel_ = 6 )
      : _width( width_ ),
	_height( height_ ),
	_outerBevel( outerBevel_ ),
	_innerBevel( innerBevel_ ) { }

    void operator()( Image &image_ )
      {
	image_.frame( _width, _height, _innerBevel, _outerBevel );
      }
  private:
    unsigned int _width;
    unsigned int _height;
    int          _outerBevel;
    int          _innerBevel;
  };

  // Gamma correct image
  class gammaImage : public std::unary_function<Image&,void>
  {
  public:
    gammaImage( double gamma_ )
      : _gammaRed( gamma_ ),
	_gammaGreen( gamma_ ),
	_gammaBlue( gamma_ ) { }
    gammaImage ( double gammaRed_,
		 double gammaGreen_,
		 double gammaBlue_ )
      : _gammaRed( gammaRed_ ),
	_gammaGreen( gammaGreen_ ),
	_gammaBlue( gammaBlue_ ) { }

    void operator()( Image &image_ )
      {
	image_.gamma( _gammaRed, _gammaGreen, _gammaBlue );
      }
  private:
    double _gammaRed;
    double _gammaGreen;
    double _gammaBlue;
  };

  // Gaussian blur image
  // The number of neighbor pixels to be included in the convolution
  // mask is specified by 'width_'. The standard deviation of the
  // gaussian bell curve is specified by 'sigma_'.
  class gaussianBlurImage : public std::unary_function<Image&,void>
  {
  public:
    gaussianBlurImage( double width_, double sigma_ )
      : _width( width_ ),
	_sigma( sigma_ ) { }

    void operator()( Image &image_ )
      {
	image_.gaussianBlur( _width, _sigma );
      }
  private:
    double _width;
    double _sigma;
  };

  // Implode image (special effect)
  class implodeImage : public std::unary_function<Image&,void>
  {
  public:
    implodeImage( double factor_ = 50  )
      : _factor( factor_ ) { }

    void operator()( Image &image_ )
      {
	image_.implode( _factor );
      }
  private:
    const double _factor;
  };

  // Set image validity. Valid images become empty (inValid) if
  // argument is false.
  class isValidImage : public std::unary_function<Image&,void>
  {
  public:
    isValidImage( bool isValid_  )
      : _isValid( isValid_ ) { }

    void operator()( Image &image_ )
      {
	image_.isValid( _isValid );
      }
  private:
    const bool _isValid;
  };

  // Label image
  class labelImage : public std::unary_function<Image&,void>
  {
  public:
    labelImage( const std::string &label_ )
      : _label( label_ ) { }

    void operator()( Image &image_ )
      {
	image_.label( _label );
      }
  private:
    const std::string _label;
  };

  // Extract channel from image
  class channelImage : public std::unary_function<Image&,void>
  {
  public:
    channelImage( ChannelType channel_ )
      : _channel( channel_ ) { }

    void operator()( Image &image_ )
      {
	image_.channel( _channel );
      }
  private:
    const ChannelType _channel;
  };

  // Magnify image by integral size
  class magnifyImage : public std::unary_function<Image&,void>
  {
  public:
    magnifyImage( void ) { }

    void operator()( Image &image_ )
      {
	image_.magnify( );
      }
  private:
  };

  // Remap image colors with closest color from reference image
  class mapImage : public std::unary_function<Image&,void>
  {
  public:
    mapImage( const Image &mapImage_ , bool dither_ = false )
      : _mapImage( mapImage_ ),
	_dither( dither_ ) { }

    void operator()( Image &image_ )
      {
	image_.map( _mapImage, _dither );
      }
  private:
    const Image   _mapImage;
    const bool    _dither;
  };

  // Floodfill designated area with a matte value
  class matteFloodfillImage : public std::unary_function<Image&,void>
  {
  public:
    matteFloodfillImage( const Color &target_ ,
			 unsigned int matte_,
			 int x_, int y_,
			 PaintMethod method_ )
      : _target( target_ ),
	_matte( matte_ ),
	_x( x_ ),
	_y( y_ ),
	_method( method_ ) { }

    void operator()( Image &image_ )
      {
	image_.matteFloodfill( _target, _matte, _x, _y, _method );
      }
  private:
    const Color   _target;
    unsigned int  _matte;
    int           _x;
    int           _y;
    PaintMethod   _method;
  };

  // Filter image by replacing each pixel component with the median
  // color in a circular neighborhood
  class medianFilterImage : public std::unary_function<Image&,void>
  {
  public:
    medianFilterImage( const double radius_ = 0.0 )
      : _radius( radius_ ) { }

    void operator()( Image &image_ )
      {
	image_.medianFilter( _radius );
      }
  private:
    const double _radius;
  };

  // Reduce image by integral size
  class minifyImage : public std::unary_function<Image&,void>
  {
  public:
    minifyImage( void ) { }

    void operator()( Image &image_ )
      {
	image_.minify( );
      }
  private:
  };

  // Modulate percent hue, saturation, and brightness of an image
  class modulateImage : public std::unary_function<Image&,void>
  {
  public:
    modulateImage( double brightness_,
		   double saturation_,
		   double hue_ )
      : _brightness( brightness_ ),
	_saturation( saturation_ ),
	_hue( hue_ ) { }

    void operator()( Image &image_ )
      {
	image_.modulate( _brightness, _saturation, _hue );
      }
  private:
    double _brightness;
    double _saturation;
    double _hue;
  };

  // Negate colors in image.  Set grayscale to only negate grayscale
  // values in image.
  class negateImage : public std::unary_function<Image&,void>
  {
  public:
    negateImage( bool grayscale_ = false )
      : _grayscale( grayscale_ ) { }

    void operator()( Image &image_ )
      {
	image_.negate( _grayscale );
      }
  private:
    const bool _grayscale;
  };

  // Normalize image (increase contrast by normalizing the pixel
  // values to span the full range of color values)
  class normalizeImage : public std::unary_function<Image&,void>
  {
  public:
    normalizeImage( void ) { }

    void operator()( Image &image_ )
      {
	image_.normalize( );
      }
  private:
  };  

  // Oilpaint image (image looks like oil painting)
  class oilPaintImage : public std::unary_function<Image&,void>
  {
  public:
    oilPaintImage( unsigned int radius_ = 3 )
      : _radius( radius_ ) { }

    void operator()( Image &image_ )
      {
	image_.oilPaint( _radius );
      }
  private:
    const unsigned int _radius;
  };

  // Set or attenuate the image opacity channel. If the image pixels
  // are opaque then they are set to the specified opacity value,
  // otherwise they are blended with the supplied opacity value.  The
  // value of opacity_ ranges from 0 (completely opaque) to
  // MaxRGB. The defines OpaqueOpacity and TransparentOpacity are
  // available to specify completely opaque or completely transparent,
  // respectively.
  class opacityImage : public std::unary_function<Image&,void>
  {
  public:
    opacityImage( unsigned int opacity_ )
      : _opacity( opacity_ ) { }

    void operator()( Image &image_ )
      {
	image_.opacity( _opacity );
      }
  private:
    const unsigned int _opacity;
  };

  // Change color of opaque pixel to specified pen color.
  class opaqueImage : public std::unary_function<Image&,void>
  {
  public:
    opaqueImage( const Color &opaqueColor_,
		 const Color &penColor_ )
      : _opaqueColor( opaqueColor_ ),
	_penColor( penColor_ ) { }

    void operator()( Image &image_ )
      {
	image_.opaque( _opaqueColor, _penColor );
      }
  private:
    const Color & _opaqueColor;
    const Color & _penColor;
  };

  // Quantize image (reduce number of colors)
  class quantizeImage : public std::unary_function<Image&,void>
  {
  public:
    quantizeImage( bool measureError_ = false )
      : _measureError( measureError_ ) { }

    void operator()( Image &image_ )
      {
	image_.quantize( _measureError );
      }
  private:
    const bool _measureError;
  };

  // Raise image (lighten or darken the edges of an image to give a
  // 3-D raised or lowered effect)
  class raiseImage : public std::unary_function<Image&,void>
  {
  public:
    raiseImage( const Geometry &geometry_ = raiseGeometryDefault,
		bool raisedFlag_ = false )
      : _geometry( geometry_ ),
	_raisedFlag( raisedFlag_ ) { };

    void operator()( Image &image_ )
      {
	image_.raise( _geometry, _raisedFlag );
      }
  private:
    const Geometry   _geometry;
    bool             _raisedFlag;
  };

  // Reduce noise in image using a noise peak elimination filter
  class reduceNoiseImage : public std::unary_function<Image&,void>
  {
  public:
    reduceNoiseImage( void )
      : _order(3) { }
    reduceNoiseImage ( unsigned int order_ )
      : _order(order_) { }

    void operator()( Image &image_ )
      {
	image_.reduceNoise( _order );
      }
  private:
    unsigned int _order;
  };

  // Roll image (rolls image vertically and horizontally) by specified
  // number of columnms and rows)
  class rollImage : public std::unary_function<Image&,void>
  {
  public:
    rollImage( const Geometry &roll_ )
      : _columns( roll_.width() ),
	_rows( roll_.height() ) { }
    rollImage( int columns_,
	       int rows_ )
      : _columns( columns_ ),
	_rows( rows_ ) { }

    void operator()( Image &image_ )
      {
	image_.roll( _columns, _rows );
      }
  private:
    int _columns;
    int _rows;
  };

  // Rotate image counter-clockwise by specified number of degrees.
  class rotateImage : public std::unary_function<Image&,void>
  {
  public:
    rotateImage( double degrees_ )
      : _degrees( degrees_ ) { }

    void operator()( Image &image_ )
      {
	image_.rotate( _degrees );
      }
  private:
    const double       _degrees;
  };

  // Resize image by using pixel sampling algorithm
  class sampleImage : public std::unary_function<Image&,void>
  {
  public:
    sampleImage( const Geometry &geometry_ )
      : _geometry( geometry_ ) { }

    void operator()( Image &image_ )
      {
	image_.sample( _geometry );
      }
  private:
    const Geometry  _geometry;
  };  

  // Resize image by using simple ratio algorithm
  class scaleImage : public std::unary_function<Image&,void>
  {
  public:
    scaleImage( const Geometry &geometry_ )
      : _geometry( geometry_ ) { }

    void operator()( Image &image_ )
      {
	image_.scale( _geometry );
      }
  private:
    const Geometry  _geometry;
  };

  // Segment (coalesce similar image components) by analyzing the
  // histograms of the color components and identifying units that are
  // homogeneous with the fuzzy c-means technique.
  // Also uses QuantizeColorSpace and Verbose image attributes
  class segmentImage : public std::unary_function<Image&,void>
  {
  public:
    segmentImage( double clusterThreshold_ = 1.0, 
		  double smoothingThreshold_ = 1.5 )
      : _clusterThreshold( clusterThreshold_ ),
	_smoothingThreshold( smoothingThreshold_ ) { }

    void operator()( Image &image_ )
      {
	image_.segment( _clusterThreshold, _smoothingThreshold );
      }
  private:
    double  _clusterThreshold;
    double  _smoothingThreshold;
  };

  // Shade image using distant light source
  class shadeImage : public std::unary_function<Image&,void>
  {
  public:
    shadeImage( double clusterThreshold_ = 1.0, 
		double smoothingThreshold_ = 1.5 )
      : _clusterThreshold( clusterThreshold_ ),
	_smoothingThreshold( smoothingThreshold_ ) { }

    void operator()( Image &image_ )
      {
	image_.shade( _clusterThreshold, _smoothingThreshold );
      }
  private:
    double  _clusterThreshold;
    double  _smoothingThreshold;
  };

  // Sharpen pixels in image
  class sharpenImage : public std::unary_function<Image&,void>
  {
  public:
    sharpenImage( const double radius_ = 1, const double sigma_ = 0.5 )
      : _radius( radius_ ),
        _sigma( sigma_ ) { }

    void operator()( Image &image_ )
      {
	image_.sharpen( _radius, _sigma );
      }
  private:
    const double _radius;
    const double _sigma;
  };

  // Shave pixels from image edges.
  class shaveImage : public std::unary_function<Image&,void>
  {
  public:
    shaveImage( const Geometry &geometry_ )
      : _geometry( geometry_ ) { }

    void operator()( Image &image_ )
      {
	image_.shave( _geometry );
      }
  private:
    const Geometry _geometry;
  };


  // Shear image (create parallelogram by sliding image by X or Y axis)
  class shearImage : public std::unary_function<Image&,void>
  {
  public:
    shearImage( double xShearAngle_,
		double yShearAngle_ )
      : _xShearAngle( xShearAngle_ ),
	_yShearAngle( yShearAngle_ ) { }

    void operator()( Image &image_ )
      {
	image_.shear( _xShearAngle, _yShearAngle );
      }
  private:
    const double _xShearAngle;
    const double _yShearAngle;
  };

  // Solarize image (similar to effect seen when exposing a
  // photographic film to light during the development process)
  class solarizeImage : public std::unary_function<Image&,void>
  {
  public:
    solarizeImage( double factor_ )
      : _factor( factor_ ) { }

    void operator()( Image &image_ )
      {
	image_.solarize( _factor );
      }
  private:
    const double _factor;
  };

  // Spread pixels randomly within image by specified ammount
  class spreadImage : public std::unary_function<Image&,void>
  {
  public:
    spreadImage( unsigned int amount_ = 3 )
      : _amount( amount_ ) { }

    void operator()( Image &image_ )
      {
	image_.spread( _amount );
      }
  private:
    const unsigned int _amount;
  };

  // Add a digital watermark to the image (based on second image)
  class steganoImage : public std::unary_function<Image&,void>
  {
  public:
    steganoImage( const Image &waterMark_ )
      : _waterMark( waterMark_ ) { }

    void operator()( Image &image_ )
      {
	image_.stegano( _waterMark );
      }
  private:
    const Image _waterMark;
  };

  // Create an image which appears in stereo when viewed with red-blue glasses
  // (Red image on left, blue on right)
  class stereoImage : public std::unary_function<Image&,void>
  {
  public:
    stereoImage( const Image &rightImage_ )
      : _rightImage( rightImage_ ) { }

    void operator()( Image &image_ )
      {
	image_.stereo( _rightImage );
      }
  private:
    const Image _rightImage;
  };

  // Color to use when drawing object outlines
  class strokeColorImage : public std::unary_function<Image&,void>
  {
  public:
    strokeColorImage( const Color &strokeColor_ )
      : _strokeColor( strokeColor_ ) { }

    void operator()( Image &image_ )
      {
	image_.strokeColor( _strokeColor );
      }
  private:
    const Color _strokeColor;
  };

  // Swirl image (image pixels are rotated by degrees)
  class swirlImage : public std::unary_function<Image&,void>
  {
  public:
    swirlImage( double degrees_ )
      : _degrees( degrees_ ) { }

    void operator()( Image &image_ )
      {
	image_.swirl( _degrees );
      }
  private:
    const double _degrees;
  };

  // Channel a texture on image background
  class textureImage : public std::unary_function<Image&,void>
  {
  public:
    textureImage( const Image &texture_ )
      : _texture( texture_ ) { }

    void operator()( Image &image_ )
      {
	image_.texture( _texture );
      }
  private:
    const Image _texture;
  };

  // Threshold image
  class thresholdImage : public std::unary_function<Image&,void>
  {
  public:
    thresholdImage( double threshold_ )
      : _threshold( threshold_ ) { }

    void operator()( Image &image_ )
      {
	image_.threshold( _threshold );
      }
  private:
    const double _threshold;
  };

  // Transform image based on image and crop geometries
  class transformImage : public std::unary_function<Image&,void>
  {
  public:
    transformImage( const Geometry &imageGeometry_ )
      : _imageGeometry( imageGeometry_ ),
	_cropGeometry( ) { }
    transformImage( const Geometry &imageGeometry_,
		    const Geometry &cropGeometry_  )
      : _imageGeometry( imageGeometry_ ),
	_cropGeometry( cropGeometry_ ) { }

    void operator()( Image &image_ )
      {
	if ( _cropGeometry.isValid() )
	  image_.transform( _imageGeometry, _cropGeometry );
	else
	  image_.transform( _imageGeometry );
      }
  private:
    const Geometry _imageGeometry;
    const Geometry _cropGeometry;
  };

  // Set image color to transparent
  class transparentImage : public std::unary_function<Image&,void>
  {
  public:
    transparentImage( const Color& color_ )
      : _color( color_ ) { }

    void operator()( Image &image_ )
      {
	image_.transparent( _color );
      }
  private:
    const Color _color;
  };

  // Trim edges that are the background color from the image
  class trimImage : public std::unary_function<Image&,void>
  {
  public:
    trimImage( void ) { }

    void operator()( Image &image_ )
      {
	image_.trim( );
      }
  private:
  };

  // Map image pixels to a sine wave
  class waveImage : public std::unary_function<Image&,void>
  {
  public:
    waveImage( double amplitude_ = 25.0,
	       double wavelength_ = 150.0 )
      : _amplitude( amplitude_ ),
	_wavelength( wavelength_ ) { }

    void operator()( Image &image_ )
      {
	image_.wave( _amplitude, _wavelength );
      }
  private:
    const double _amplitude;
    const double _wavelength;
  };

  // Zoom image to specified size.
  class zoomImage : public std::unary_function<Image&,void>
  {
  public:
    zoomImage( const Geometry &geometry_ )
      : _geometry( geometry_ ) { }

    void operator()( Image &image_ )
      {
	image_.zoom( _geometry );
      }
  private:
    const Geometry _geometry;
  };

  //
  // Function object image attribute accessors
  //

  // Anti-alias Postscript and TrueType fonts (default true)
  class antiAliasImage : public std::unary_function<Image&,void>
  {
  public:
    antiAliasImage( bool flag_ )
      : _flag( flag_ ) { }

    void operator()( Image &image_ )
      {
	image_.antiAlias( _flag );
      }
  private:
    const bool _flag;
  };

  // Join images into a single multi-image file
  class adjoinImage : public std::unary_function<Image&,void>
  {
  public:
    adjoinImage( bool flag_ )
      : _flag( flag_ ) { }

    void operator()( Image &image_ )
      {
	image_.adjoin( _flag );
      }
  private:
    const bool _flag;
  };

  // Time in 1/100ths of a second which must expire before displaying
  // the next image in an animated sequence.
  class animationDelayImage : public std::unary_function<Image&,void>
  {
  public:
    animationDelayImage( unsigned int delay_ )
      : _delay( delay_ ) { }

    void operator()( Image &image_ )
      {
	image_.animationDelay( _delay );
      }
  private:
    const unsigned int _delay;
  };

  // Number of iterations to loop an animation (e.g. Netscape loop
  // extension) for.
  class animationIterationsImage : public std::unary_function<Image&,void>
  {
  public:
    animationIterationsImage( unsigned int iterations_ )
      : _iterations( iterations_ ) { }

    void operator()( Image &image_ )
      {
	image_.animationIterations( _iterations );
      }
  private:
    const unsigned int _iterations;
  };

  // Image background color
  class backgroundColorImage : public std::unary_function<Image&,void>
  {
  public:
    backgroundColorImage( const Color &color_ )
      : _color( color_ ) { }

    void operator()( Image &image_ )
      {
	image_.backgroundColor( _color );
      }
  private:
    const Color _color;
  };

  // Name of texture image to tile onto the image background
  class backgroundTextureImage : public std::unary_function<Image&,void>
  {
  public:
    backgroundTextureImage( const std::string &backgroundTexture_ )
      : _backgroundTexture( backgroundTexture_ ) { }

    void operator()( Image &image_ )
      {
	image_.backgroundTexture( _backgroundTexture );
      }
  private:
    const std::string _backgroundTexture;
  };

  // Image border color
  class borderColorImage : public std::unary_function<Image&,void>
  {
  public:
    borderColorImage( const Color &color_ )
      : _color( color_ ) { }

    void operator()( Image &image_ )
      {
	image_.borderColor( _color );
      }
  private:
    const Color _color;
  };

  // Text bounding-box base color (default none)
  class boxColorImage : public std::unary_function<Image&,void>
  {
  public:
    boxColorImage( const Color &boxColor_ )
      : _boxColor( boxColor_ ) { }

    void operator()( Image &image_ )
      {
	image_.boxColor( _boxColor );
      }
  private:
    const Color _boxColor;
  };

  // Chromaticity blue primary point (e.g. x=0.15, y=0.06)
  class chromaBluePrimaryImage : public std::unary_function<Image&,void>
  {
  public:
    chromaBluePrimaryImage( float x_, float y_ )
      : _x( x_ ),
	_y( y_ ) { }

    void operator()( Image &image_ )
      {
	image_.chromaBluePrimary( _x, _y );
      }
  private:
    const float _x;
    const float _y;
  };

  // Chromaticity green primary point (e.g. x=0.3, y=0.6)
  class chromaGreenPrimaryImage : public std::unary_function<Image&,void>
  {
  public:
    chromaGreenPrimaryImage( float x_, float y_ )
      : _x( x_ ),
	_y( y_ ) { }

    void operator()( Image &image_ )
      {
	image_.chromaGreenPrimary( _x, _y );
      }
  private:
    const float _x;
    const float _y;
  };

  // Chromaticity red primary point (e.g. x=0.64, y=0.33)
  class chromaRedPrimaryImage : public std::unary_function<Image&,void>
  {
  public:
    chromaRedPrimaryImage( float x_, float y_ )
      : _x( x_ ),
	_y( y_ ) { }

    void operator()( Image &image_ )
      {
	image_.chromaRedPrimary( _x, _y );
      }
  private:
    const float _x;
    const float _y;
  };

  // Chromaticity white point (e.g. x=0.3127, y=0.329)
  class chromaWhitePointImage : public std::unary_function<Image&,void>
  {
  public:
    chromaWhitePointImage( float x_, float y_ )
      : _x( x_ ),
	_y( y_ ) { }

    void operator()( Image &image_ )
      {
	image_.chromaWhitePoint( _x, _y );
      }
  private:
    const float _x;
    const float _y;
  };

  // Colors within this distance are considered equal
  class colorFuzzImage : public std::unary_function<Image&,void>
  {
  public:
    colorFuzzImage( double fuzz_ )
      : _fuzz( fuzz_ ) { }

    void operator()( Image &image_ )
      {
	image_.colorFuzz( _fuzz );
      }
  private:
    const double _fuzz;
  };

  // Color at colormap position index_
  class colorMapImage : public std::unary_function<Image&,void>
  {
  public:
    colorMapImage( unsigned int index_, const Color &color_ )
      : _index( index_ ),
	_color( color_ ) { }

    void operator()( Image &image_ )
      {
	image_.colorMap( _index, _color );
      }
  private:
    const unsigned int _index;
    const Color        _color;
  };

  // Compression type
  class compressTypeImage : public std::unary_function<Image&,void>
  {
  public:
    compressTypeImage( CompressionType compressType_ )
      : _compressType( compressType_ ) { }

    void operator()( Image &image_ )
      {
	image_.compressType( _compressType );
      }
  private:
    const CompressionType _compressType;
  };

  // Vertical and horizontal resolution in pixels of the image
  class densityImage : public std::unary_function<Image&,void>
  {
  public:
    densityImage( const Geometry &geomery_ )
      : _geomery( geomery_ ) { }

    void operator()( Image &image_ )
      {
	image_.density( _geomery );
      }
  private:
    const Geometry _geomery;
  };

  // Image depth (bits allocated to red/green/blue components)
  class depthImage : public std::unary_function<Image&,void>
  {
  public:
    depthImage( unsigned int depth_ )
      : _depth( depth_ ) { }

    void operator()( Image &image_ )
      {
	image_.depth( _depth );
      }
  private:
    const unsigned int _depth;
  };

  // Image file name
  class fileNameImage : public std::unary_function<Image&,void>
  {
  public:
    fileNameImage( const std::string &fileName_ )
      : _fileName( fileName_ ) { }

    void operator()( Image &image_ )
      {
	image_.fileName( _fileName );
      }
  private:
    const std::string _fileName;
  };

  // Filter to use when resizing image
  class filterTypeImage : public std::unary_function<Image&,void>
  {
  public:
    filterTypeImage( FilterTypes filterType_ )
      : _filterType( filterType_ ) { }

    void operator()( Image &image_ )
      {
	image_.filterType( _filterType );
      }
  private:
    const FilterTypes _filterType;
  };

  // Text rendering font
  class fontImage : public std::unary_function<Image&,void>
  {
  public:
    fontImage( const std::string &font_ )
      : _font( font_ ) { }

    void operator()( Image &image_ )
      {
	image_.font( _font );
      }
  private:
    const std::string _font;
  };

  // Font point size
  class fontPointsizeImage : public std::unary_function<Image&,void>
  {
  public:
    fontPointsizeImage( unsigned int pointsize_ )
      : _pointsize( pointsize_ ) { }

    void operator()( Image &image_ )
      {
	image_.fontPointsize( _pointsize );
      }
  private:
    const unsigned int _pointsize;
  };

  // GIF disposal method
  class gifDisposeMethodImage : public std::unary_function<Image&,void>
  {
  public:
    gifDisposeMethodImage( unsigned int disposeMethod_ )
      : _disposeMethod( disposeMethod_ ) { }

    void operator()( Image &image_ )
      {
	image_.gifDisposeMethod( _disposeMethod );
      }
  private:
    const unsigned int _disposeMethod;
  };

  // Type of interlacing to use
  class interlaceTypeImage : public std::unary_function<Image&,void>
  {
  public:
    interlaceTypeImage( InterlaceType interlace_ )
      : _interlace( interlace_ ) { }

    void operator()( Image &image_ )
      {
	image_.interlaceType( _interlace );
      }
  private:
    const InterlaceType _interlace;
  };

  // Linewidth for drawing vector objects (default one)
  class lineWidthImage : public std::unary_function<Image&,void>
  {
  public:
    lineWidthImage( double lineWidth_ )
      : _lineWidth( lineWidth_ ) { }

    void operator()( Image &image_ )
      {
	image_.lineWidth( _lineWidth );
      }
  private:
    const double _lineWidth;
  };

  // File type magick identifier (.e.g "GIF")
  class magickImage : public std::unary_function<Image&,void>
  {
  public:
    magickImage( const std::string &magick_ )
      : _magick( magick_ ) { }

    void operator()( Image &image_ )
      {
	image_.magick( _magick );
      }
  private:
    const std::string _magick;
  };

  // Image supports transparent color
  class matteImage : public std::unary_function<Image&,void>
  {
  public:
    matteImage( bool matteFlag_ )
      : _matteFlag( matteFlag_ ) { }

    void operator()( Image &image_ )
      {
	image_.matte( _matteFlag );
      }
  private:
    const bool _matteFlag;
  };

  // Transparent color
  class matteColorImage : public std::unary_function<Image&,void>
  {
  public:
    matteColorImage( const Color &matteColor_ )
      : _matteColor( matteColor_ ) { }

    void operator()( Image &image_ )
      {
	image_.matteColor( _matteColor );
      }
  private:
    const Color _matteColor;
  };

  // Transform image to black and white
  class monochromeImage : public std::unary_function<Image&,void>
  {
  public:
    monochromeImage( bool monochromeFlag_ )
      : _monochromeFlag( monochromeFlag_ ) { }

    void operator()( Image &image_ )
      {
	image_.monochrome( _monochromeFlag );
      }
  private:
    const bool _monochromeFlag;
  };

  // Pen color
  class penColorImage : public std::unary_function<Image&,void>
  {
  public:
    penColorImage( const Color &penColor_ )
      : _penColor( penColor_ ) { }

    void operator()( Image &image_ )
      {
	image_.penColor( _penColor );
      }
  private:
    const Color _penColor;
  };

  // Pen texture image.
  class penTextureImage : public std::unary_function<Image&,void>
  {
  public:
    penTextureImage( const Image &penTexture_ )
      : _penTexture( penTexture_ ) { }

    void operator()( Image &image_ )
      {
	image_.penTexture( _penTexture );
      }
  private:
    const Image _penTexture;
  };

  // Set pixel color at location x & y.
  class pixelColorImage : public std::unary_function<Image&,void>
  {
  public:
    pixelColorImage( unsigned int x_, unsigned int y_,
		     const Color &color_)
      : _x( x_ ),
	_y( y_ ),
	_color( color_ ) { }

    void operator()( Image &image_ )
      {
	image_.pixelColor( _x, _y, _color );
      }
  private:
    const unsigned int _x;
    const unsigned int _y;
    const Color        _color;
  };

  // Postscript page size.
  class pageImage : public std::unary_function<Image&,void>
  {
  public:
    pageImage( const Geometry &pageSize_ )
      : _pageSize( pageSize_ ) { }

    void operator()( Image &image_ )
      {
	image_.page( _pageSize );
      }
  private:
    const Geometry _pageSize;
  };

  // JPEG/MIFF/PNG compression level (default 75).
  class qualityImage : public std::unary_function<Image&,void>
  {
  public:
    qualityImage( unsigned int quality_ )
      : _quality( quality_ ) { }

    void operator()( Image &image_ )
      {
	image_.quality( _quality );
      }
  private:
    const unsigned int _quality;
  };

  // Maximum number of colors to quantize to
  class quantizeColorsImage : public std::unary_function<Image&,void>
  {
  public:
    quantizeColorsImage( unsigned int colors_ )
      : _colors( colors_ ) { }

    void operator()( Image &image_ )
      {
	image_.quantizeColors( _colors );
      }
  private:
    const unsigned int _colors;
  };

  // Colorspace to quantize in.
  class quantizeColorSpaceImage : public std::unary_function<Image&,void>
  {
  public:
    quantizeColorSpaceImage( ColorspaceType colorSpace_ )
      : _colorSpace( colorSpace_ ) { }

    void operator()( Image &image_ )
      {
	image_.quantizeColorSpace( _colorSpace );
      }
  private:
    const ColorspaceType _colorSpace;
  };

  // Dither image during quantization (default true).
  class quantizeDitherImage : public std::unary_function<Image&,void>
  {
  public:
    quantizeDitherImage( bool ditherFlag_ )
      : _ditherFlag( ditherFlag_ ) { }

    void operator()( Image &image_ )
      {
	image_.quantizeDither( _ditherFlag );
      }
  private:
    const bool _ditherFlag;
  };

  // Quantization tree-depth
  class quantizeTreeDepthImage : public std::unary_function<Image&,void>
  {
  public:
    quantizeTreeDepthImage( unsigned int treeDepth_ )
      : _treeDepth( treeDepth_ ) { }

    void operator()( Image &image_ )
      {
	image_.quantizeTreeDepth( _treeDepth );
      }
  private:
    const unsigned int _treeDepth;
  };

  // The type of rendering intent
  class renderingIntentImage : public std::unary_function<Image&,void>
  {
  public:
    renderingIntentImage( RenderingIntent renderingIntent_ )
      : _renderingIntent( renderingIntent_ ) { }

    void operator()( Image &image_ )
      {
	image_.renderingIntent( _renderingIntent );
      }
  private:
    const RenderingIntent _renderingIntent;
  };

  // Units of image resolution
  class resolutionUnitsImage : public std::unary_function<Image&,void>
  {
  public:
    resolutionUnitsImage( ResolutionType resolutionUnits_ )
      : _resolutionUnits( resolutionUnits_ ) { }

    void operator()( Image &image_ )
      {
	image_.resolutionUnits( _resolutionUnits );
      }
  private:
    const ResolutionType _resolutionUnits;
  };

  // Image scene number
  class sceneImage : public std::unary_function<Image&,void>
  {
  public:
    sceneImage( unsigned int scene_ )
      : _scene( scene_ ) { }

    void operator()( Image &image_ )
      {
	image_.scene( _scene );
      }
  private:
    const unsigned int _scene;
  };

  // Width and height of a raw image
  class sizeImage : public std::unary_function<Image&,void>
  {
  public:
    sizeImage( const Geometry &geometry_ )
      : _geometry( geometry_ ) { }

    void operator()( Image &image_ )
      {
	image_.size( _geometry );
      }
  private:
    const Geometry _geometry;
  };

  // Subimage of an image sequence
  class subImageImage : public std::unary_function<Image&,void>
  {
  public:
    subImageImage( unsigned int subImage_ )
      : _subImage( subImage_ ) { }

    void operator()( Image &image_ )
      {
	image_.subImage( _subImage );
      }
  private:
    const unsigned int _subImage;
  };

  // Number of images relative to the base image
  class subRangeImage : public std::unary_function<Image&,void>
  {
  public:
    subRangeImage( unsigned int subRange_ )
      : _subRange( subRange_ ) { }

    void operator()( Image &image_ )
      {
	image_.subRange( _subRange );
      }
  private:
    const unsigned int _subRange;
  };

  // Tile name
  class tileNameImage : public std::unary_function<Image&,void>
  {
  public:
    tileNameImage( const std::string &tileName_ )
      : _tileName( tileName_ ) { }

    void operator()( Image &image_ )
      {
	image_.tileName( _tileName );
      }
  private:
    const std::string _tileName;
  };

  // Image storage type
  class typeImage : public std::unary_function<Image&,void>
  {
  public:
    typeImage( Magick::ImageType type_ )
      : _type( type_ ) { }

    void operator()( Image &image_ )
      {
	image_.type( _type );
      }
  private:
    const Magick::ImageType _type;
  };


  // Print detailed information about the image
  class verboseImage : public std::unary_function<Image&,void>
  {
  public:
    verboseImage( const bool verbose_ )
      : _verbose( verbose_ ) { }

    void operator()( Image &image_ )
      {
	image_.verbose( _verbose );
      }
  private:
    const bool _verbose;
  };

  // FlashPix viewing parameters
  class viewImage : public std::unary_function<Image&,void>
  {
  public:
    viewImage( const std::string &view_ )
      : _view( view_ ) { }

    void operator()( Image &image_ )
      {
	image_.view( _view );
      }
  private:
    const std::string _view;
  };

  // X11 display to display to, obtain fonts from, or to capture
  // image from
  class x11DisplayImage : public std::unary_function<Image&,void>
  {
  public:
    x11DisplayImage( const std::string &display_ )
      : _display( display_ ) { }

    void operator()( Image &image_ )
      {
	image_.x11Display( _display );
      }
  private:
    const std::string _display;
  };

  //////////////////////////////////////////////////////////
  //
  // Implementation template definitions. Not for end-use.
  //
  //////////////////////////////////////////////////////////

  // Link images together into an image list based on the ordering of
  // the container implied by the iterator. This step is done in
  // preparation for use with ImageMagick functions which operate on
  // lists of images.
  // Images are selected by range, first_ to last_ so that a subset of
  // the container may be selected.  Specify first_ via the
  // container's begin() method and last_ via the container's end()
  // method in order to specify the entire container.
  template <class InputIterator>
  void linkImages( InputIterator first_,
		   InputIterator last_ ) {

    MagickLib::Image* previous = 0;
    int scene = 0;
    for ( InputIterator iter = first_; iter != last_; ++iter )
      {
	// Unless we reduce the reference count to one, the same image
	// structure may occur more than once in the container, causing
	// the linked list to fail.
	iter->modifyImage();

	MagickLib::Image* current = iter->image();

	current->previous = previous;
	current->next     = 0;
	current->orphan   = static_cast<int>(false); // In a list

	if ( previous != 0)
	  previous->next = current;

	current->scene=scene;
	++scene;

	previous = current;
      }
  }

  // Remove links added by linkImages. This should be called after the
  // ImageMagick function call has completed to reset the image list
  // back to its pristine un-linked state.
  template <class InputIterator>
  void unlinkImages( InputIterator first_,
		     InputIterator last_ ) {
    for( InputIterator iter = first_; iter != last_; ++iter )
      {
	MagickLib::Image* image = iter->image();
	image->previous = 0;
	image->next = 0;
	image->orphan = static_cast<int>(true); // Stand-alone
      }
  }

  // Insert images in image list into existing container (appending to container)
  // The images should not be deleted since only the image ownership is passed.
  // The options copied into the object.
  template <class Container>
  void insertImages( Container *sequence_,
		     MagickLib::Image* images_,
		     Options &options_ ) {
    MagickLib::Image *image = images_;
    if ( image )
      {
	do
	  {
	    MagickLib::Image* next_image = image->next;
	    image->next = 0;
	  
	    if (next_image != 0)
	      next_image->previous=0;
	  
	    sequence_->push_back( Magick::Image( image, &options_ ) );
	  
	    image=next_image;
	  } while( image );
      
	return;
      }
  }

  ///////////////////////////////////////////////////////////////////
  //
  // Template definitions for documented API
  //
  ///////////////////////////////////////////////////////////////////

  template <class InputIterator>
  void animateImages( InputIterator first_,
		      InputIterator last_ ) {
    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );
    linkImages( first_, last_ );
    MagickLib::AnimateImages( first_->imageInfo(), first_->image() );
    MagickLib::GetImageException( first_->image(), &exceptionInfo );
    unlinkImages( first_, last_ );
    throwException( exceptionInfo );
  }

  // Append images from list into single image in either horizontal or
  // vertical direction.
  template <class InputIterator>
  void appendImages( Image *appendedImage_,
		     InputIterator first_,
		     InputIterator last_,
		     bool stack_ = false) {
    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );
    linkImages( first_, last_ );
    MagickLib::Image* image = MagickLib::AppendImages( first_->image(),
						       stack_,
						       &exceptionInfo ); 
    unlinkImages( first_, last_ );
    appendedImage_->replaceImage( image );
    throwException( exceptionInfo );
  }

  // Average a set of images.
  // All the input images must be the same size in pixels.
  template <class InputIterator>
  void averageImages( Image *averagedImage_,
		      InputIterator first_,
		      InputIterator last_ ) {
    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );
    linkImages( first_, last_ );
    MagickLib::Image* image = MagickLib::AverageImages( first_->image(),
							&exceptionInfo );
    unlinkImages( first_, last_ );
    averagedImage_->replaceImage( image );
    throwException( exceptionInfo );
  }

  // Merge a sequence of images.
  // This is useful for GIF animation sequences that have page
  // offsets and disposal methods.
  // The images are modified in-place. No pointer re-assignments are performed.
  template <class InputIterator>
  void coalesceImages( InputIterator first_,
		       InputIterator last_ ) {
    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );
    linkImages( first_, last_ );
    MagickLib::CoalesceImages( first_->image(), &exceptionInfo );
    unlinkImages( first_, last_ );
    throwException( exceptionInfo );
  }

  // Return format coders matching specified conditions.
  //
  // The default (if no match terms are supplied) is to return all
  // available format coders.
  //
  // For example, to return all readable formats:
  //  list<CoderInfo> coderList;
  //  coderInfoList( &coderList, TrueMatch, AnyMatch, AnyMatch)
  //
  template <class Container >
  void coderInfoList( Container *container_,
                      MatchType isReadable_ = AnyMatch,
                      MatchType isWritable_ = AnyMatch,
                      MatchType isMultiFrame_ = AnyMatch
                      ) {
    // Obtain first entry in MagickInfo list
    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );
    MagickLib::MagickInfo *magickInfo = const_cast<MagickLib::MagickInfo *>(MagickLib::GetMagickInfo( "*", &exceptionInfo ));
    throwException( exceptionInfo );
    if( !magickInfo )
      throwExceptionExplicit(MagickLib::MissingDelegateError,
                             "Coder list not returned!", 0 );

    // Clear out container
    container_->clear();

    for ( ; magickInfo != 0; magickInfo=magickInfo->next)
      {
        // Skip stealth coders
        if ( magickInfo->stealth )
          continue;

        CoderInfo coderInfo( magickInfo );

        // Test isReadable_
        if ( isReadable_ != AnyMatch &&
             (( coderInfo.isReadable() && isReadable_ != TrueMatch ) ||
              ( !coderInfo.isReadable() && isReadable_ != FalseMatch )) )
          continue;

        // Test isWritable_
        if ( isWritable_ != AnyMatch &&
             (( coderInfo.isWritable() && isWritable_ != TrueMatch ) ||
              ( !coderInfo.isWritable() && isWritable_ != FalseMatch )) )
          continue;

        // Test isMultiFrame_
        if ( isMultiFrame_ != AnyMatch &&
             (( coderInfo.isMultiFrame() && isMultiFrame_ != TrueMatch ) ||
              ( !coderInfo.isMultiFrame() && isMultiFrame_ != FalseMatch )) )
          continue;

        // Append matches to container
        container_->push_back( coderInfo );
      }
  }

  // Break down an image sequence into constituent parts.  This is
  // useful for creating GIF or MNG animation sequences.
  template <class InputIterator, class Container >
  void deconstructImages( Container *deconstructedImages_,
                          InputIterator first_,
                          InputIterator last_ ) {
    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );

    // Build image list
    linkImages( first_, last_ );
    MagickLib::Image* images = MagickLib::DeconstructImages( first_->image(),
                                                             &exceptionInfo);
    // Unlink image list
    unlinkImages( first_, last_ );

    // Ensure container is empty
    deconstructedImages_->clear();
    Magick::Options options;

    // Move images to container
    insertImages( deconstructedImages_, images, options );

    // Report any error
    throwException( exceptionInfo );
  }

  //
  // Display an image sequence
  //
  template <class InputIterator>
  void displayImages( InputIterator first_,
		      InputIterator last_ ) {
    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );
    linkImages( first_, last_ );
    MagickLib::DisplayImages( first_->imageInfo(), first_->image() );
    MagickLib::GetImageException( first_->image(), &exceptionInfo );
    unlinkImages( first_, last_ );
    throwException( exceptionInfo );
  }

  // Merge a sequence of image frames which represent image layers.
  // This is useful for combining Photoshop layers into a single image.
  template <class InputIterator>
  void flattenImages( Image *flattendImage_,
		      InputIterator first_,
		      InputIterator last_ ) {
    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );
    linkImages( first_, last_ );
    MagickLib::Image* image = MagickLib::FlattenImages( first_->image(),
							&exceptionInfo );
    unlinkImages( first_, last_ );
    flattendImage_->replaceImage( image );
    throwException( exceptionInfo );
  }

  // Replace the colors of a sequence of images with the closest color
  // from a reference image.
  // Set dither_ to true to enable dithering.  Set measureError_ to
  // true in order to evaluate quantization error.
  template <class InputIterator>
  void mapImages( InputIterator first_,
		  InputIterator last_,
		  const Image& mapImage_,
		  bool dither_ = false,
		  bool measureError_ = false ) {

    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );
    linkImages( first_, last_ );
    MagickLib::MapImages( first_->image(),
			  mapImage_.constImage(),
			  dither_ );
    MagickLib::GetImageException( first_->image(), &exceptionInfo );
    if ( exceptionInfo.severity != MagickLib::UndefinedException )
      {
	unlinkImages( first_, last_ );
	throwException( exceptionInfo );
      }

    MagickLib::Image* image = first_->image();
    while( image )
      {
	// Calculate quantization error
	if ( measureError_ )
	  {
	    MagickLib::QuantizationError( image );
	    if ( image->exception.severity > MagickLib::UndefinedException )
	      {
		unlinkImages( first_, last_ );
		throwException( exceptionInfo );
	      }
	  }
	
	// Udate DirectClass representation of pixels
	MagickLib::SyncImage( image );
	if ( image->exception.severity > MagickLib::UndefinedException )
	  {
	    unlinkImages( first_, last_ );
	    throwException( exceptionInfo );
	  }

	// Next image
	image=image->next;
      }

    unlinkImages( first_, last_ );
  }

  // Create a composite image by combining several separate images.
  template <class Container, class InputIterator>
  void montageImages( Container *montageImages_,
		      InputIterator first_,
		      InputIterator last_,
		      const Montage &montageOpts_ ) {

    MagickLib::MontageInfo* montageInfo =
      static_cast<MagickLib::MontageInfo*>(MagickLib::AcquireMemory(sizeof(MagickLib::MontageInfo)));

    // Update montage options with those set in montageOpts_
    montageOpts_.updateMontageInfo( *montageInfo );

    // Update options which must transfer to image options
    if ( montageOpts_.label().length() != 0 )
      first_->label( montageOpts_.label() );

    // Create linked image list
    linkImages( first_, last_ );

    // Reset output container to pristine state
    montageImages_->clear();

    // Do montage
    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );
    MagickLib::Image *images = MagickLib::MontageImages( first_->image(),
							 montageInfo,
							 &exceptionInfo );
    if ( images != 0 )
      {
	Magick::Options options;
	insertImages( montageImages_, images, options );
      }

    // Clean up any allocated data in montageInfo
    MagickLib::DestroyMontageInfo( montageInfo );

    // Unlink linked image list
    unlinkImages( first_, last_ );

    // Report any montage error
    throwException( exceptionInfo );

    // Apply transparency to montage images
    if ( montageImages_->size() > 0 && montageOpts_.transparentColor().isValid() )
      {
	for_each( first_, last_, transparentImage( montageOpts_.transparentColor() ) );
      }

    // Report any transparentImage() error
    MagickLib::GetImageException( first_->image(), &exceptionInfo );
    throwException( exceptionInfo );
  }

  // Morph a set of images
  template <class InputIterator, class Container >
  void morphImages( Container *morphedImages_,
		    InputIterator first_,
		    InputIterator last_,
		    unsigned int frames_ ) {
    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );

    // Build image list
    linkImages( first_, last_ );
    MagickLib::Image* images = MagickLib::MorphImages( first_->image(), frames_,
						       &exceptionInfo);
    // Unlink image list
    unlinkImages( first_, last_ );

    // Ensure container is empty
    morphedImages_->clear();
    Magick::Options options;

    // Move images to container
    insertImages( morphedImages_, images, options );

    // Report any error
    throwException( exceptionInfo );
  }

  // Inlay a number of images to form a single coherent picture.
  template <class InputIterator>
  void mosaicImages( Image *mosaicImage_,
		     InputIterator first_,
		     InputIterator last_ ) {
    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );
    linkImages( first_, last_ );
    MagickLib::Image* image = MagickLib::MosaicImages( first_->image(),
						       &exceptionInfo ); 
    unlinkImages( first_, last_ );
    mosaicImage_->replaceImage( image );
    throwException( exceptionInfo );
  }

  // Quantize colors in images using current quantization settings
  // Set measureError_ to true in order to measure quantization error
  template <class InputIterator>
  void quantizeImages( InputIterator first_,
		       InputIterator last_,
		       bool measureError_ = false ) {
    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );

    linkImages( first_, last_ );

    MagickLib::QuantizeImages( first_->options()->quantizeInfo(),
			       first_->image() );
    MagickLib::GetImageException( first_->image(), &exceptionInfo );
    if ( exceptionInfo.severity > MagickLib::UndefinedException )
      {
	unlinkImages( first_, last_ );
	throwException( exceptionInfo );
      }

    MagickLib::Image* image = first_->image();
    while( image != 0 )
      {
	// Calculate quantization error
	if ( measureError_ )
	  MagickLib::QuantizationError( image );

	// Update DirectClass representation of pixels
	MagickLib::SyncImage( image );

	// Next image
	image=image->next;
      }

    unlinkImages( first_, last_ );
  }

  // Read images into existing container (appending to container)
  template <class Container>
  void readImages( Container *sequence_,
		   const std::string &imageSpec_ ) {
    Magick::Options options;
    options.fileName( imageSpec_ );
    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );
    MagickLib::Image* images =  MagickLib::ReadImage( options.imageInfo(), &exceptionInfo );
    insertImages( sequence_, images, options );
    throwException( exceptionInfo );
  }
  template <class Container>
  void readImages( Container *sequence_,
		   const Blob &blob_ ) {
    Magick::Options options;
    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );
    MagickLib::Image *images = MagickLib::BlobToImage( options.imageInfo(),
						       static_cast<void *>(blob_.data()),
						       blob_.length(), &exceptionInfo );
    insertImages( sequence_, images, options );
    throwException( exceptionInfo );
  }

  // Write Images
  template <class InputIterator>
  void writeImages( InputIterator first_,
		    InputIterator last_,
		    const std::string &imageSpec_,
		    bool adjoin_ = true ) {

    // Save original image settings
    std::string origFileName = first_->fileName();
    std::string origMagick = first_->magick();
    bool origAdjoin = first_->adjoin();

    first_->fileName( imageSpec_ );
    first_->adjoin( adjoin_ );

    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );

    linkImages( first_, last_ );
    int errorStat = MagickLib::WriteImage( first_->imageInfo(),
					   first_->image() );
    unlinkImages( first_, last_ );

    // Restore original image settings
    first_->fileName( origFileName );
    first_->magick( origMagick );
    first_->adjoin( origAdjoin );

    if ( errorStat != false )
      return;

    MagickLib::GetImageException( first_->image(), &exceptionInfo );
    throwException( exceptionInfo );
  }
  // Write images to BLOB
  template <class InputIterator>
  void writeImages( InputIterator first_,
		    InputIterator last_,
		    Blob *blob_,
		    bool adjoin_ = true) {
    // Save original image settings
    std::string origMagick = first_->magick();
    bool origAdjoin = first_->adjoin();

    first_->adjoin( adjoin_ );

    linkImages( first_, last_ );

    MagickLib::ExceptionInfo exceptionInfo;
    MagickLib::GetExceptionInfo( &exceptionInfo );
    size_t length = 2048; // Efficient size for small images
    void* data = MagickLib::ImageToBlob( first_->imageInfo(),
					 first_->image(),
					 &length,
					 &exceptionInfo);
    blob_->updateNoCopy( data, length );

    unlinkImages( first_, last_ );

    // Restore original image settings
    first_->magick( origMagick );
    first_->adjoin( origAdjoin );

    throwException( exceptionInfo );
  }



} // namespace Magick

#endif // Magick_STL_header
