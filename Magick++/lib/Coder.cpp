// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Implementation of Coder, a image encode/decode wrapper.
//
// NOTE: This is a work in progress and is not expected to work -- yet.
//

#define MAGICK_IMPLEMENTATION

using namespace std;

#include "Magick++/Include.h"
#include "Magick++/Functions.h"
#include "Magick++/Coder.h"

//
// Implementation of Magick::Coder
//

// Destructor
/* virtual */
Magick::Coder::~Coder ()
{
  // Remove registration
  UnregisterMagickInfo( _tag.c_str() );
}

// Decoder method (callback)
/* virtual */
Magick::Image Magick::Coder::decoder ( Image& image_ )
{
  return image_;
}

// Encoder method (callback)
// Returns true if image is successfully written.
/* virtual */
bool Magick::Coder::encoder ( const Image& image_ )
{
  return false;
}

// Magick method (callback).
// Returns true if magick format is supported by the decoder.
/* virtual */
bool Magick::Coder::magick ( const unsigned char *magick_,
			     const unsigned int length_ )
{
  return false;
};

extern "C" {
  static MagickLib::Image *decoder( const MagickLib::ImageInfo *imageInfo_,
                                    MagickLib::ExceptionInfo *exception_ );
  static unsigned int encoder( const MagickLib::ImageInfo *imageInfo_,
			       MagickLib::Image *image_);
  static unsigned int magick( const unsigned char *magick_,
			      const unsigned int length_ );
}

// Image decoder callback (C linkage)
static MagickLib::Image *decoder( const MagickLib::ImageInfo *imageInfo_,
                                  MagickLib::ExceptionInfo *exception_ )
{

  Magick::Image image;
  MagickLib::MagickInfo* info = 0; // FIXME!

  if ( info->data == 0 )
    return 0;

  // The decoder may modify ImageInfo so the original ImageInfo struct
  // must be updated for the caller (no copy can be made).

  MagickLib::QuantizeInfo* quantizeInfo = 
    reinterpret_cast<MagickLib::QuantizeInfo*>(MagickLib::AcquireMemory(sizeof(MagickLib::QuantizeInfo)));
  MagickLib::GetQuantizeInfo( quantizeInfo );

  Magick::Coder* coder = static_cast<Magick::Coder*>(info->data);
  //  coder->decoder( Magick::Options( imageInfo_, quantizeInfo ) );
  coder->decoder( image );
}

// Image encoder callback (C linkage)
// Returns True on success
static unsigned int encoder( const MagickLib::ImageInfo *imageInfo_,
			     MagickLib::Image *image_)
{
  MagickLib::MagickInfo* info = 0; // FIXME!

  if ( info->data == 0 )
    return 0;

  Magick::Coder* coder = static_cast<Magick::Coder*>(info->data);
//   return static_cast<unsigned int>
//     (coder->encoder(Magick::Image( imageInfo_, image_ )));
}

// Image magick callback (C linkage)
// Returns True on success
static unsigned int magick( const unsigned char *magick_,
                            const unsigned int length_ )
{
  MagickLib::MagickInfo* info = 0; // FIXME!

  if ( info->data == 0 )
    return 0;

  Magick::Coder* coder = static_cast<Magick::Coder*>(info->data);
  return static_cast<unsigned int>(coder->magick( magick_, length_ ));
}


// Constructor.

// The object callbacks are automatically registered when the base
// class constructor is invoked from the derived class.
Magick::Coder::Coder (
		      std::string tag_,		// format magick string
		      bool decoderSupport_,	// decoder method implemented
		      bool encoderSupport_,	// encoder method implemented
		      bool magickSupport_,	// magick method implemented
		      bool adjoin_,		// format supports multiple frames
		      bool blobSupport_,	// implementation supports BLOBs
		      bool rawImage_,		// format does not include size
		      std::string description_	// format description
		      )
{
  // Save format tag for use in destructor
  _tag = tag_;

  // Register coder callbacks
  MagickInfo* info = SetMagickInfo(tag_.c_str());
//   info->decoder = decoderSupport_ ? decoder : 0;
//   info->encoder = encoderSupport_ ? encoder : 0;
//   info->magick  = magickSupport_  ? magick  : 0;
  info->adjoin  = static_cast<unsigned int>(adjoin_);
  info->blob_support = static_cast<unsigned int>(blobSupport_);
  info->raw     = static_cast<unsigned int>(rawImage_);
  info->tag     = 0;
  Magick::CloneString( &info->description, description_ );
  info->module  = 0;
  info->data = static_cast<void *>(this); // coder object
  RegisterMagickInfo ( info );
}
