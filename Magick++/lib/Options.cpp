// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001
//
// Implementation of Options
//
// A wrapper around DrawInfo, ImageInfo, and QuantizeInfo
//

#define MAGICK_IMPLEMENTATION

#include <string>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "Magick++/Options.h"
#include "Magick++/Functions.h"
#include "Magick++/Exception.h"
#include "Magick++/Include.h"

#define DegreesToRadians(x) ((x)*3.14159265358979323846/180.0)

// Constructor
Magick::Options::Options( void )
  : _imageInfo(static_cast<ImageInfo*>(AcquireMemory(sizeof(ImageInfo)))),
    _quantizeInfo(static_cast<QuantizeInfo*>(AcquireMemory(sizeof(QuantizeInfo)))),
    _drawInfo(static_cast<DrawInfo*>(AcquireMemory( sizeof(DrawInfo))))
{
  // Initialize image info with defaults
  GetImageInfo( _imageInfo );
  
  // Initialize quantization info
  GetQuantizeInfo( _quantizeInfo );

  // Initialize drawing info
  GetDrawInfo( _imageInfo, _drawInfo );
}

// Copy constructor
Magick::Options::Options( const Magick::Options& options_ )
  : _imageInfo(CloneImageInfo( options_._imageInfo )),
    _quantizeInfo(CloneQuantizeInfo(options_._quantizeInfo)),
    _drawInfo(CloneDrawInfo(_imageInfo, options_._drawInfo))
{
}

// Construct using raw structures
Magick::Options::Options( const MagickLib::ImageInfo* imageInfo_,
                          const MagickLib::QuantizeInfo* quantizeInfo_,
                          const MagickLib::DrawInfo* drawInfo_ )
: _imageInfo(0),
  _quantizeInfo(0),
  _drawInfo(0)
{
  _imageInfo = CloneImageInfo(imageInfo_);
  _quantizeInfo = CloneQuantizeInfo(quantizeInfo_);
  _drawInfo = CloneDrawInfo(imageInfo_,drawInfo_);
}

// Destructor
Magick::Options::~Options()
{
  // Destroy image info
  DestroyImageInfo( _imageInfo );
  
  // Destroy quantization info
  DestroyQuantizeInfo( _quantizeInfo );

  // Destroy drawing info
  DestroyDrawInfo( _drawInfo );
}

/*
 * Methods for setting image attributes
 *
 */

void Magick::Options::backgroundColor ( const Magick::Color &color_ )
{
  _imageInfo->background_color = color_;
}
Magick::Color Magick::Options::backgroundColor ( void ) const
{
  return Magick::Color( _imageInfo->background_color );
}

void Magick::Options::backgroundTexture ( const std::string &backgroundTexture_ )
{
  if ( backgroundTexture_.length() == 0 )
    LiberateMemory( reinterpret_cast<void**>(&_imageInfo->texture) );
  else
    Magick::CloneString( &_imageInfo->texture, backgroundTexture_ );
}
std::string Magick::Options::backgroundTexture ( void ) const
{
  if ( _imageInfo->texture )
    return std::string( _imageInfo->texture );
  else
    return std::string();
}

void Magick::Options::borderColor ( const Color &color_ )
{
  _imageInfo->border_color = color_;
  _drawInfo->border_color = color_;
}
Magick::Color Magick::Options::borderColor ( void ) const
{
  return Magick::Color( _imageInfo->border_color );
}

// Text bounding-box base color
void Magick::Options::boxColor ( const Magick::Color &boxColor_ )
{
  _drawInfo->box = boxColor_;
}
Magick::Color Magick::Options::boxColor ( void ) const
{
  return Magick::Color( _drawInfo->box );
}

void Magick::Options::strokeDashArray ( const unsigned int* strokeDashArray_ )
{
  LiberateMemory(reinterpret_cast<void**>(&_drawInfo->dash_pattern));
	_drawInfo->dash_pattern = NULL;

  if(strokeDashArray_)
    {
      // Count elements in dash array
      int x;
      for (x=0; strokeDashArray_[x]; x++);
      // Allocate elements
      _drawInfo->dash_pattern =
        static_cast<unsigned int*>(AcquireMemory((x+1)*sizeof(unsigned int)));
      // Copy elements
      memcpy(_drawInfo->dash_pattern,strokeDashArray_,
             (x+1)*sizeof(unsigned int));
    }
}
const unsigned int* Magick::Options::strokeDashArray ( void ) const
{
  return _drawInfo->dash_pattern;
}

void Magick::Options::density ( const Magick::Geometry &density_ )
{
  if ( !density_.isValid() )
    LiberateMemory( reinterpret_cast<void**>(&_imageInfo->density) );
  else
    Magick::CloneString( &_imageInfo->density, density_ );
}
Magick::Geometry Magick::Options::density ( void ) const
{
  if ( _imageInfo->density )
    return Geometry( _imageInfo->density );

  return Geometry();
}

void Magick::Options::fileName ( const std::string &fileName_ )
{
  fileName_.copy( _imageInfo->filename, MaxTextExtent-1 );
  _imageInfo->filename[ fileName_.length() ] = 0;
}
std::string Magick::Options::fileName ( void ) const
{
  return std::string( _imageInfo->filename );
}

// Color to use when drawing inside an object
void Magick::Options::fillColor ( const Magick::Color &fillColor_ )
{
  _drawInfo->fill = fillColor_;
}
Magick::Color Magick::Options::fillColor ( void ) const
{
  return _drawInfo->fill;
}

// Rule to use when filling drawn objects
void Magick::Options::fillRule ( const Magick::FillRule &fillRule_ )
{
  _drawInfo->fill_rule = fillRule_;
}
Magick::FillRule Magick::Options::fillRule ( void ) const
{
  return _drawInfo->fill_rule;
}

void Magick::Options::font ( const std::string &font_ )
{
  if ( font_.length() == 0 )
    {
      LiberateMemory( reinterpret_cast<void**>(&_imageInfo->font) );
      LiberateMemory( reinterpret_cast<void**>(&_drawInfo->font) );
    }
  else
    {
      Magick::CloneString( &_imageInfo->font, font_ );
      Magick::CloneString( &_drawInfo->font, font_ );
    }
}
std::string Magick::Options::font ( void ) const
{
  if ( _imageInfo->font )
    return std::string( _imageInfo->font );
  
  return std::string();
}

std::string Magick::Options::format ( void ) const
{
  ExceptionInfo exception;

  const MagickInfo * magick_info = 0;
  GetExceptionInfo(&exception);
  if ( _imageInfo->magick && ( *_imageInfo->magick != '\0' ))
    magick_info = GetMagickInfo( _imageInfo->magick , &exception);
  
  if (( magick_info != 0 ) && 
      ( *magick_info->description != '\0' ))
    return std::string( magick_info->description );
  
  return std::string();
}

void Magick::Options::magick ( const std::string &magick_ )
{
  ExceptionInfo exception;

  FormatString( _imageInfo->filename, "%.1024s:", magick_.c_str() );
  GetExceptionInfo(&exception);
  SetImageInfo( _imageInfo, 1, &exception);
  if ( _imageInfo->magick == '\0' )
    throwExceptionExplicit( OptionWarning,
			    "Unrecognized image format",
			    magick_.c_str() );
}
std::string Magick::Options::magick ( void ) const
{
  if ( _imageInfo->magick && *_imageInfo->magick )
    return std::string( _imageInfo->magick );
  
  return std::string();
}

void Magick::Options::matteColor ( const Magick::Color &matteColor_ )
{
  _imageInfo->matte_color = matteColor_;
}
Magick::Color Magick::Options::matteColor ( void ) const
{
  return Magick::Color( _imageInfo->matte_color );
}

void Magick::Options::page ( const Magick::Geometry &pageSize_ )
{
  if ( !pageSize_.isValid() )
    LiberateMemory( reinterpret_cast<void**>(&_imageInfo->page) );
  else
    Magick::CloneString( &_imageInfo->page, pageSize_ );
}
Magick::Geometry Magick::Options::page ( void ) const
{
  if ( _imageInfo->page )
    return Geometry( _imageInfo->page );

    return Geometry();
}

void Magick::Options::penTexture ( const MagickLib::Image *penTexture_ )
{
  if ( _drawInfo->tile )
    {
      DestroyImage( _drawInfo->tile );
      _drawInfo->tile = 0;
    }

  if ( penTexture_ )
    {
      ExceptionInfo exceptionInfo;
      GetExceptionInfo( &exceptionInfo );
      _drawInfo->tile =
	CloneImage( const_cast<MagickLib::Image*>(penTexture_),
		    0,
		    0,
		    static_cast<int>(true),
		    &exceptionInfo );
      throwException( exceptionInfo );
    }
}
const MagickLib::Image* Magick::Options::penTexture ( void  ) const
{
  return _drawInfo->tile;
}

void Magick::Options::resolutionUnits ( Magick::ResolutionType resolutionUnits_ )
{
  _imageInfo->units = resolutionUnits_;
}
Magick::ResolutionType Magick::Options::resolutionUnits ( void ) const
{
  return static_cast<Magick::ResolutionType>(_imageInfo->units);
}

void Magick::Options::size ( const Geometry &geometry_ )
{
  LiberateMemory( reinterpret_cast<void**>(&_imageInfo->size) );

  if ( geometry_.isValid() )
    Magick::CloneString( &_imageInfo->size, geometry_ );
}
Magick::Geometry Magick::Options::size ( void ) const
{
  if ( _imageInfo->size )
    return Geometry( _imageInfo->size );

  return Geometry();
}

// Color to use when drawing object outlines
void Magick::Options::strokeColor ( const Magick::Color &strokeColor_ )
{
  _drawInfo->stroke = strokeColor_;
}
Magick::Color Magick::Options::strokeColor ( void ) const
{
  return _drawInfo->stroke;
}

void Magick::Options::tileName ( const std::string &tileName_ )
{
  if ( tileName_.length() == 0 )
    LiberateMemory( reinterpret_cast<void**>(&_imageInfo->tile) );
  else
    Magick::CloneString( &_imageInfo->tile, tileName_ );
}
std::string Magick::Options::tileName ( void ) const
{
  if ( _imageInfo->tile )
    return std::string( _imageInfo->tile );
  return std::string();
}

// Origin of coordinate system to use when annotating with text or drawing
void Magick::Options::transformOrigin ( double tx_, double ty_ )
{
  AffineMatrix current = _drawInfo->affine;
  AffineMatrix affine;
  affine.sx=1.0;
  affine.rx=0.0;
  affine.ry=0.0;
  affine.sy=1.0;
  affine.tx=0.0;
  affine.ty=0.0;

  affine.tx = tx_;
  affine.ty = ty_;

  _drawInfo->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
  _drawInfo->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
  _drawInfo->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
  _drawInfo->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
  _drawInfo->affine.tx=current.sx*affine.tx+current.ry*affine.ty+current.tx;
  _drawInfo->affine.ty=current.rx*affine.tx+current.sy*affine.ty+current.ty;
}

// Reset transformation parameters to default
void Magick::Options::transformReset ( void )
{
  _drawInfo->affine.sx=1.0;
  _drawInfo->affine.rx=0.0;
  _drawInfo->affine.ry=0.0;
  _drawInfo->affine.sy=1.0;
  _drawInfo->affine.tx=0.0;
  _drawInfo->affine.ty=0.0;
}

// Rotation to use when annotating with text or drawing
void Magick::Options::transformRotation ( double angle_ )
{
  AffineMatrix current = _drawInfo->affine;
  AffineMatrix affine;
  affine.sx=1.0;
  affine.rx=0.0;
  affine.ry=0.0;
  affine.sy=1.0;
  affine.tx=0.0;
  affine.ty=0.0;

  affine.sx=cos(DegreesToRadians(fmod(angle_,360.0)));
  affine.rx=(-sin(DegreesToRadians(fmod(angle_,360.0))));
  affine.ry=sin(DegreesToRadians(fmod(angle_,360.0)));
  affine.sy=cos(DegreesToRadians(fmod(angle_,360.0)));

  _drawInfo->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
  _drawInfo->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
  _drawInfo->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
  _drawInfo->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
  _drawInfo->affine.tx=current.sx*affine.tx+current.ry*affine.ty+current.tx;
  _drawInfo->affine.ty=current.rx*affine.tx+current.sy*affine.ty+current.ty;
}

// Scale to use when annotating with text or drawing
void Magick::Options::transformScale ( double sx_, double sy_ )
{
  AffineMatrix current = _drawInfo->affine;
  AffineMatrix affine;
  affine.sx=1.0;
  affine.rx=0.0;
  affine.ry=0.0;
  affine.sy=1.0;
  affine.tx=0.0;
  affine.ty=0.0;

  affine.sx = sx_;
  affine.sy = sy_;

  _drawInfo->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
  _drawInfo->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
  _drawInfo->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
  _drawInfo->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
  _drawInfo->affine.tx=current.sx*affine.tx+current.ry*affine.ty+current.tx;
  _drawInfo->affine.ty=current.rx*affine.tx+current.sy*affine.ty+current.ty;
}

// Skew to use in X axis when annotating with text or drawing
void Magick::Options::transformSkewX ( double skewx_ )
{
  AffineMatrix current = _drawInfo->affine;
  AffineMatrix affine;
  affine.sx=1.0;
  affine.rx=0.0;
  affine.ry=0.0;
  affine.sy=1.0;
  affine.tx=0.0;
  affine.ty=0.0;

  affine.sx=1.0;
  affine.ry=tan(DegreesToRadians(fmod(skewx_,360.0)));
  affine.sy=1.0;

  _drawInfo->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
  _drawInfo->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
  _drawInfo->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
  _drawInfo->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
  _drawInfo->affine.tx=current.sx*affine.tx+current.ry*affine.ty+current.tx;
  _drawInfo->affine.ty=current.rx*affine.tx+current.sy*affine.ty+current.ty;
}

// Skew to use in Y axis when annotating with text or drawing
void Magick::Options::transformSkewY ( double skewy_ )
{
  AffineMatrix current = _drawInfo->affine;
  AffineMatrix affine;
  affine.sx=1.0;
  affine.rx=0.0;
  affine.ry=0.0;
  affine.sy=1.0;
  affine.tx=0.0;
  affine.ty=0.0;

  affine.sx=1.0;
  affine.rx=tan(DegreesToRadians(fmod(skewy_,360.0)));
  affine.sy=1.0;

  _drawInfo->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
  _drawInfo->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
  _drawInfo->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
  _drawInfo->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
  _drawInfo->affine.tx=current.sx*affine.tx+current.ry*affine.ty+current.tx;
  _drawInfo->affine.ty=current.rx*affine.tx+current.sy*affine.ty+current.ty;
}

void Magick::Options::view ( const std::string &view_ )
{
  if ( view_.length() == 0 )
    LiberateMemory( reinterpret_cast<void**>(&_imageInfo->view) );
  else
    Magick::CloneString( &_imageInfo->view, view_ );
}
std::string Magick::Options::view ( void ) const
{
  if ( _imageInfo->view )
    return std::string( _imageInfo->view );

  return std::string();
}

void Magick::Options::x11Display ( const std::string &display_ )
{
  if ( display_.length() == 0 )
    LiberateMemory( reinterpret_cast<void**>(&_imageInfo->server_name) );
  else
    Magick::CloneString( &_imageInfo->server_name, display_ );
}
std::string Magick::Options::x11Display ( void ) const
{
  if ( _imageInfo->server_name )
    return std::string( _imageInfo->server_name );

    return std::string();
}
