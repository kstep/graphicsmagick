// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001
//
// Representation of a pixel view.
//

#if !defined(Magick_Pixels_header)
#define Magick_Pixels_header

#include "Magick++/Color.h"
#include "Magick++/Image.h"

namespace Magick
{
  class Pixels
  {
  public:

    // Construct pixel view using specified image.
    Pixels( Magick::Image &image_ );

    // Destroy pixel view
    ~Pixels( void );
    
    // Transfer pixels from the image to the pixel view as defined by
    // the specified region. Modified pixels may be subsequently
    // transferred back to the image via sync.
    
    PixelPacket* get ( unsigned int x_, unsigned int y_,
		       unsigned int columns_, unsigned int rows_ );
    
    // Transfers the image view pixels to the image.
    void sync ( void );
    
    // Allocate a pixel view region to store image pixels as defined
    // by the region rectangle.  This area is subsequently transferred
    // from the pixel view to the image via sync.
    PixelPacket* set ( unsigned int x_, unsigned int y_,
		       unsigned int columns_, unsigned int rows_ );

    // Return pixel colormap index array
    IndexPacket* indexes ( void );

    // Left ordinate of view
    unsigned int x ( void ) const;

    // Top ordinate of view
    unsigned int y ( void ) const;

    // Width of view
    unsigned int columns ( void ) const;

    // Height of view
    unsigned int rows ( void ) const;

#if 0
    // Transfer one or more pixel components from a buffer or file
    // into the image pixel view of an image.  Used to support image
    // decoders.
    void decode ( QuantumType quantum_,
		  unsigned char *source_ )
      {
	MagickLib::ReadPixelCache( _image.image(), quantum_, source_ );
      }
    
    // Transfer one or more pixel components from the image pixel
    // view to a buffer or file.  Used to support image encoders.
    void encode ( QuantumType quantum_,
		  unsigned char *destination_ )
      {
	MagickLib::WritePixelCache( _image.image(), quantum_, destination_ );
      }
#endif
  private:

    // Copying and assigning Pixels is not supported.
    Pixels( const Pixels& pixels_ );
    const Pixels& operator=( const Pixels& pixels_ );

    Magick::Image          _image;   // Image reference
    MagickLib::ViewInfo*   _view;    // Image view handle
    Magick::PixelPacket*   _pixels;  // View pixels
    unsigned int           _x;       // Left ordinate of view
    unsigned int           _y;       // Top ordinate of view
    unsigned int           _columns; // Width of view
    unsigned int           _rows;    // Height of view

  }; // class Pixels

} // Magick namespace

//
// Inline methods
//

// Left ordinate of view
inline unsigned int Magick::Pixels::x ( void ) const
{
  return _x;
}

// Top ordinate of view
inline unsigned int Magick::Pixels::y ( void ) const
{
  return _y;
}

// Width of view
inline unsigned int Magick::Pixels::columns ( void ) const
{
  return _columns;
}

// Height of view
inline unsigned int Magick::Pixels::rows ( void ) const
{
  return _rows;
}

#endif // Magick_Pixels_header
