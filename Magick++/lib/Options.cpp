// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Implementation of Options
//
// A wrapper around AnnotateInfo, ImageInfo, and QuantizeInfo
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
    _annotateInfo(static_cast<AnnotateInfo*>(AcquireMemory(sizeof(AnnotateInfo)))),
    _drawInfo(static_cast<DrawInfo*>(AcquireMemory( sizeof(DrawInfo))))
{
  // Initialize image info with defaults
  GetImageInfo( _imageInfo );
  
  // Initialize quantization info
  GetQuantizeInfo( _quantizeInfo );

  // Initialize annotate info
  GetAnnotateInfo( _imageInfo, _annotateInfo );

  // Initialize drawing info
  GetDrawInfo( _imageInfo, _drawInfo );
}

// Copy constructor
Magick::Options::Options( const Magick::Options& options_ )
  : _imageInfo(CloneImageInfo( options_._imageInfo )),
    _quantizeInfo(CloneQuantizeInfo(options_._quantizeInfo)),
    _annotateInfo(CloneAnnotateInfo(_imageInfo, options_._annotateInfo)),
    _drawInfo(CloneDrawInfo(_imageInfo, options_._drawInfo))
{
}

// Destructor
Magick::Options::~Options()
{
  // Destroy image info
  DestroyImageInfo( _imageInfo );
  
  // Destroy quantization info
  DestroyQuantizeInfo( _quantizeInfo );

  // Destroy annotation info
  DestroyAnnotateInfo( _annotateInfo );

  // Destroy drawing info
  DestroyDrawInfo( _drawInfo );
}

/*
 * Methods for setting image attributes
 *
 */

void Magick::Options::animationDelay ( unsigned int delay_ )
{
  char tmpbuff[MaxTextExtent + 1];
  FormatString( tmpbuff, "%u", delay_ );
  Magick::CloneString( &_imageInfo->delay, tmpbuff );
}
unsigned int Magick::Options::animationDelay ( void ) const
{
  if ( _imageInfo->delay )
    return atoi( _imageInfo->delay );
  else
    return 0;
}

void Magick::Options::animationIterations ( unsigned int iterations_ )
{
  char tmpbuff[MaxTextExtent + 1];
  FormatString( tmpbuff, "%u", iterations_ );
  Magick::CloneString( &_imageInfo->iterations, tmpbuff );
}
unsigned int Magick::Options::animationIterations ( void ) const
{
  if ( _imageInfo->iterations )
    return atoi( _imageInfo->iterations );
  else
    return 0;
}

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
  _annotateInfo->box = boxColor_;
}
Magick::Color Magick::Options::boxColor ( void ) const
{
  return Magick::Color( _annotateInfo->box );
}

void Magick::Options::density ( const Magick::Geometry &density_ )
{
  if ( !density_.isValid() )
    LiberateMemory( reinterpret_cast<void**>(&_imageInfo->density) );
  else
    Magick::CloneString( &_imageInfo->density, density_ );

  updateAnnotateInfo();
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
  _annotateInfo->fill = fillColor_;
  _drawInfo->fill = fillColor_;
  _imageInfo->fill = fillColor_;
}
Magick::Color Magick::Options::fillColor ( void ) const
{
  return _drawInfo->fill;
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

  updateAnnotateInfo();
}
std::string Magick::Options::font ( void ) const
{
  if ( _imageInfo->font )
    return std::string( _imageInfo->font );
  
  return std::string();
}

std::string Magick::Options::format ( void ) const
{
  const MagickInfo * magick_info = 0;
  if ( _imageInfo->magick && ( *_imageInfo->magick != '\0' ))
    magick_info = GetMagickInfo( _imageInfo->magick );
  
  if (( magick_info != 0 ) && 
      ( *magick_info->description != '\0' ))
    return std::string( magick_info->description );
  
  return std::string();
}

void Magick::Options::gifDisposeMethod ( unsigned int disposeMethod_ )
{
  char tmpbuff[MaxTextExtent + 1];
  FormatString( tmpbuff, "%u", disposeMethod_ );
  Magick::CloneString( &_imageInfo->dispose, tmpbuff );
}
unsigned int Magick::Options::gifDisposeMethod ( void ) const
{
  // It would be better to return an enumeration
  if ( _imageInfo->dispose )
    return ( atoi( _imageInfo->dispose ) );
  
  return 0;
}

void Magick::Options::magick ( const std::string &magick_ )
{
  FormatString( _imageInfo->filename, "%.1024s:", magick_.c_str() );
  SetImageInfo( _imageInfo, 1 );
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
  _annotateInfo->stroke = strokeColor_;
  _drawInfo->stroke = strokeColor_;
  _imageInfo->stroke = strokeColor_;
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
  AffineInfo current = _imageInfo->affine;
  AffineInfo affine;
  affine.sx=1.0;
  affine.rx=0.0;
  affine.ry=0.0;
  affine.sy=1.0;
  affine.tx=0.0;
  affine.ty=0.0;

  affine.tx = tx_;
  affine.ty = ty_;

  _imageInfo->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
  _imageInfo->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
  _imageInfo->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
  _imageInfo->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
  _imageInfo->affine.tx=current.sx*affine.tx+current.ry*affine.ty+current.tx;
  _imageInfo->affine.ty=current.rx*affine.tx+current.sy*affine.ty+current.ty;

  _annotateInfo->affine = _imageInfo->affine;
  _drawInfo->affine     = _imageInfo->affine;
}

// Reset transformation parameters to default
void Magick::Options::transformReset ( void )
{
  _imageInfo->affine.sx=1.0;
  _imageInfo->affine.rx=0.0;
  _imageInfo->affine.ry=0.0;
  _imageInfo->affine.sy=1.0;
  _imageInfo->affine.tx=0.0;
  _imageInfo->affine.ty=0.0;
}

// Rotation to use when annotating with text or drawing
void Magick::Options::transformRotation ( double angle_ )
{
  AffineInfo current = _imageInfo->affine;
  AffineInfo affine;
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

  _imageInfo->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
  _imageInfo->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
  _imageInfo->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
  _imageInfo->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
  _imageInfo->affine.tx=current.sx*affine.tx+current.ry*affine.ty+current.tx;
  _imageInfo->affine.ty=current.rx*affine.tx+current.sy*affine.ty+current.ty;

  _annotateInfo->affine = _imageInfo->affine;
  _drawInfo->affine     = _imageInfo->affine;
}

// Scale to use when annotating with text or drawing
void Magick::Options::transformScale ( double sx_, double sy_ )
{
  AffineInfo current = _imageInfo->affine;
  AffineInfo affine;
  affine.sx=1.0;
  affine.rx=0.0;
  affine.ry=0.0;
  affine.sy=1.0;
  affine.tx=0.0;
  affine.ty=0.0;

  affine.sx = sx_;
  affine.sy = sy_;

  _imageInfo->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
  _imageInfo->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
  _imageInfo->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
  _imageInfo->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
  _imageInfo->affine.tx=current.sx*affine.tx+current.ry*affine.ty+current.tx;
  _imageInfo->affine.ty=current.rx*affine.tx+current.sy*affine.ty+current.ty;

  _annotateInfo->affine = _imageInfo->affine;
  _drawInfo->affine     = _imageInfo->affine;
}

// Skew to use in X axis when annotating with text or drawing
void Magick::Options::transformSkewX ( double skewx_ )
{
  AffineInfo current = _imageInfo->affine;
  AffineInfo affine;
  affine.sx=1.0;
  affine.rx=0.0;
  affine.ry=0.0;
  affine.sy=1.0;
  affine.tx=0.0;
  affine.ty=0.0;

  affine.sx=1.0;
  affine.ry=tan(DegreesToRadians(fmod(skewx_,360.0)));
  affine.sy=1.0;

  _imageInfo->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
  _imageInfo->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
  _imageInfo->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
  _imageInfo->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
  _imageInfo->affine.tx=current.sx*affine.tx+current.ry*affine.ty+current.tx;
  _imageInfo->affine.ty=current.rx*affine.tx+current.sy*affine.ty+current.ty;

  _annotateInfo->affine = _imageInfo->affine;
  _drawInfo->affine = _imageInfo->affine;
}

// Skew to use in Y axis when annotating with text or drawing
void Magick::Options::transformSkewY ( double skewy_ )
{
  AffineInfo current = _imageInfo->affine;
  AffineInfo affine;
  affine.sx=1.0;
  affine.rx=0.0;
  affine.ry=0.0;
  affine.sy=1.0;
  affine.tx=0.0;
  affine.ty=0.0;

  affine.sx=1.0;
  affine.rx=tan(DegreesToRadians(fmod(skewy_,360.0)));
  affine.sy=1.0;

  _imageInfo->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
  _imageInfo->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
  _imageInfo->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
  _imageInfo->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
  _imageInfo->affine.tx=current.sx*affine.tx+current.ry*affine.ty+current.tx;
  _imageInfo->affine.ty=current.rx*affine.tx+current.sy*affine.ty+current.ty;

  _annotateInfo->affine = _imageInfo->affine;
  _drawInfo->affine = _imageInfo->affine;
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

// Update annotation info based on current ImageInfo
void Magick::Options::updateAnnotateInfo( void )
{
  // Update _annotateInfo
  AnnotateInfo* annotate_info =
    static_cast<AnnotateInfo*>(AcquireMemory(sizeof(AnnotateInfo)));
  if ( annotate_info )
    {
      GetAnnotateInfo( _imageInfo, annotate_info );
      DestroyAnnotateInfo( _annotateInfo );
      _annotateInfo=annotate_info;
    }
}
