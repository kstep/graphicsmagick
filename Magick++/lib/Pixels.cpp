// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Pixels Implementation
//

#define MAGICK_IMPLEMENTATION

#include "Magick++/Pixels.h"

namespace Magick
{


}

// Construct pixel view using specified image.
Magick::Pixels::Pixels( Magick::Image &image_ )
  : _image(image_),
    _view(MagickLib::OpenCacheView(image_.image())),
    _pixels(0),
    _x(0),
    _y(0),
    _columns(0),
    _rows(0)
{
  if (!_view)
    throwException( MagickLib::ResourceLimitError, "Out of pixel views" );
}

// Destroy pixel view
Magick::Pixels::~Pixels( void )
{
  if ( _view )
    MagickLib::CloseCacheView( _view );
}

// Transfer pixels from the image to the pixel view as defined by
// the specified region. Modified pixels may be subsequently
// transferred back to the image via sync.
    
Magick::PixelPacket* Magick::Pixels::get ( unsigned int x_,
					   unsigned int y_,
					   unsigned int columns_,
					   unsigned int rows_ )
{
  if ( ( x_ + columns_ > _image.columns()) ||
       ( y_ + rows_ > _image.rows()) )
    throwException( MagickLib::OptionError,
		    "View requested outside of image" );
  _x = x_;
  _y = y_;
  _columns = columns_;
  _rows = rows_;

  _pixels = MagickLib::GetCacheView( _view, x_, y_,
				     columns_, rows_ );
  if ( _pixels == 0 )
    throwException( MagickLib::OptionError, "Failed to get pixels" );
  
  return _pixels;
}

// Transfers the image view pixels to the image.
void Magick::Pixels::sync ( void )
{
  if( !MagickLib::SyncCacheView( _view ) )
    throwException( MagickLib::OptionError, "Failed to sync pixels" );
}
    
// Allocate a pixel view region to store image pixels as defined
// by the region rectangle.  This area is subsequently transferred
// from the pixel view to the image via 'sync'.
Magick::PixelPacket* Magick::Pixels::set ( unsigned int x_,
					   unsigned int y_,
					   unsigned int columns_,
					   unsigned int rows_ )
{
  if ( ( x_ + columns_ > _image.columns()) ||
       ( y_ + rows_ > _image.rows()) )
    throwException( MagickLib::OptionError,
		    "View requested outside of image" );

  _x = x_;
  _y = y_;
  _columns = columns_;
  _rows = rows_;

  _pixels = MagickLib::SetCacheView( _view, x_, y_,
				     columns_, rows_ );
  if ( !_pixels )
    throwException( MagickLib::OptionError, "Failed to set pixels" );
  
  return _pixels;
}

// Return pixel colormap index array
Magick::IndexPacket* Magick::Pixels::indexes ( void )
{
  if ( _image.constImage()->c_class != Magick::PseudoClass )
    throwException( MagickLib::OptionError,
		    "Image class does not support colormap");

  return MagickLib::GetIndexes( _image.image() );
}
