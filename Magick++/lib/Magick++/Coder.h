// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Base class for image format encoders and decoders.
//
// The methods decoder, encoder, and magick are callbacks which are
// invoked by the library to decode, encode, or identify an image.
//

#if !defined(Coder_header)
#define Coder_header

#include <string>

#include "Magick++/Image.h"
#include "Magick++/Options.h"

namespace Magick
{
  class Coder
  {

  public:

    // Destructor
    virtual ~Coder ();

    // Decoder method (callback)
    virtual Image decoder ( const Options& options_ );

    // Encoder method (callback)
    // Returns true if image is successfully written.
    virtual bool encoder ( const Image& image_ );

    // Magick method (callback).
    // Returns true if magick format is supported by the decoder.
    virtual bool magick ( const unsigned char *magick_,
			  const unsigned int length_ );

  protected:

    // Constructor.
    // The object is automatically registered when the base class
    // constructor is invoked from the derived class.
    Coder (
	   std::string tag_,		// format magick string
	   bool decoderSupport_,	// decoder method implemented
	   bool encoderSupport_,	// encoder method implemented
	   bool magickSupport_,		// magick method implemented
	   bool adjoin_,		// format supports multiple frames
	   bool blobSupport_,		// implementation supports BLOBs
	   bool rawImage_,		// format does not include size
	   std::string description_	// format description
	   );

  private:

    // Copy constructor (not allowed)
    Coder ( const Coder& coder_ );

    // Assignment operator (not allowed)
    Coder operator= ( const Coder& coder_ );

    std::string  _tag;
  };

} // namespace Magick

#endif // Coder_header
