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
#include "Magick++/Options.h"
#include "Magick++/Functions.h"
#include "Magick++/Exception.h"
#include "Magick++/Include.h"

// Constructor
Magick::Options::Options( void )
  : _imageInfo((ImageInfo*)AcquireMemory( sizeof(ImageInfo) ) ),
    _quantizeInfo((QuantizeInfo*)AcquireMemory( sizeof(QuantizeInfo) )),
    _annotateInfo((AnnotateInfo*)AcquireMemory( sizeof(AnnotateInfo) )),
    _drawInfo((DrawInfo*)AcquireMemory( sizeof(DrawInfo) ))
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
    LiberateMemory( (void**)&_imageInfo->texture );
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
    LiberateMemory( (void**)&_imageInfo->density );
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
      LiberateMemory( (void**)&_imageInfo->font );
      LiberateMemory( (void**)&_drawInfo->font );
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
  
  if (( magick_info != (MagickInfo *)0 ) && 
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
    LiberateMemory( (void**)&_imageInfo->page );
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
      _drawInfo->tile = (MagickLib::Image*)NULL;
    }

  if ( penTexture_ )
    {
      ExceptionInfo exceptionInfo;
      GetExceptionInfo( &exceptionInfo );
      _drawInfo->tile =
	CloneImage( const_cast<MagickLib::Image*>(penTexture_),
		    0,
		    0,
		    (int)true,
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
  return (Magick::ResolutionType)_imageInfo->units;
}

void Magick::Options::size ( const Geometry &geometry_ )
{
  LiberateMemory( (void**)&_imageInfo->size );

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
    LiberateMemory( (void**)&_imageInfo->tile );
  else
    Magick::CloneString( &_imageInfo->tile, tileName_ );
}
std::string Magick::Options::tileName ( void ) const
{
  if ( _imageInfo->tile )
    return std::string( _imageInfo->tile );
  return std::string();
}

void Magick::Options::view ( const std::string &view_ )
{
  if ( view_.length() == 0 )
    LiberateMemory( (void**)&_imageInfo->view );
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
    LiberateMemory( (void**)&_imageInfo->server_name );
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
    (AnnotateInfo*)AcquireMemory( sizeof(AnnotateInfo) );
  if ( annotate_info )
    {
      GetAnnotateInfo( _imageInfo, annotate_info );
      DestroyAnnotateInfo( _annotateInfo );
      _annotateInfo=annotate_info;
    }
}
