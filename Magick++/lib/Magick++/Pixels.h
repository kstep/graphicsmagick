// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Representation of a pixel view.
//

#if !defined(Pixels_header)
#define Pixels_header

#include "Magick++/Include.h"
#include "Magick++/Color.h"
#include "Magick++/Image.h"
#include "Magick++/Exception.h"

namespace Magick
{
  
  template <class ColorClass>
  class Pixels : public ColorClass
  {
  public:
    // Construct pixel view using specified image.
    Pixels( Image &image_ )
      : _image(image_),
	_view(MagickLib::OpenCacheView(image_.image()))
      {
	if (!_view)
	  throwException( MagickLib::ResourceLimitError, "Out of pixel views" );
      }

    // Destroy pixel view
    ~Pixels( void )
      {
	if ( view )
	  MagickLib::CloseCacheView( view );
      }
    
    //////////////////////////////////////////////////////////////////////
    //
    // Pixel Access Methods
    //
    //////////////////////////////////////////////////////////////////////

    // Transfer pixels from the image to the pixel view as defined by
    // the specified region. Modified pixels may be subsequently
    // transferred back to the image via syncPixels.
    
    PixelPacket* getPixels ( int x_, int y_,
			     unsigned int columns_, unsigned int rows_ )
      {
	_pixels = MagickLib::GetCacheView( _view, x_, y_,
					   columns_, rows_ );
	if (!_pixels )
	  throwException( OptionError, "Failed to get pixels" );

	return _pixels;
      }
    
    // Transfers the image view pixels to the image.
    void syncPixels ( void )
      {
	MagickLib::SyncCacheView( _view );
      }
    
    // Allocate a pixel view region to store image pixels as defined
    // by the region rectangle.  This area is subsequently transferred
    // from the pixel view to the image via syncPixels.
    PixelPacket* setPixels ( int x_, int y_,
			     unsigned int columns_, unsigned int rows_ )
      {
	_pixels = MagickLib::SetCacheView( _view, x_, y_,
					   columns_, rows_ );
	if (!_pixels )
	  throwException( OptionError, "Failed to set pixels" );

	return _pixels;
      }
#if 0
    // Transfer one or more pixel components from a buffer or file
    // into the image pixel view of an image.  Used to support image
    // decoders.
    void readPixel ( QuantumTypes quantum_,
		     unsigned char *source_ )
      {
	MagickLib::ReadPixelCache( _image.image(), quantum_, source_ );
      }
    
    // Transfer one or more pixel components from the image pixel
    // view to a buffer or file.  Used to support image encoders.
    void writePixel ( QuantumTypes quantum_,
		      unsigned char *destination_ )
      {
	MagickLib::WritePixelCache( _image.image(), quantum_, destination_ );
      }
#endif
  private:

    Image                  _image;  // Image reference
    MagickLib::ViewInfo*   _view;   // Image view handle

  }; // class Pixels

} // Magick namespace

#endif // Pixels_header
