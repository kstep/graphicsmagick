// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Implementation of Blob
//

#define MAGICK_IMPLEMENTATION

#include <cstring>
#include <new>

using namespace std;

#include <MagickBlob.h>
#include <MagickInclude.h>

//
// Implementation of Magick::Blob
//

// Default constructor
Magick::Blob::Blob ( void )
{
  _blobRef = new Magick::BlobRef( 0, 0 );
}

// Construct with data
Magick::Blob::Blob ( const void* data_, unsigned long length_ )
{
  _blobRef = new Magick::BlobRef( data_, length_ );
}

// Copy constructor (reference counted)
Magick::Blob::Blob ( const Magick::Blob& blob_ )
  : _blobRef(blob_._blobRef)
{
  // Increase reference count
  ++_blobRef->_refCount;
}

// Destructor (reference counted)
Magick::Blob::~Blob ()
{
    if ( --_blobRef->_refCount == 0 )
    {
      // Delete old blob reference with associated data
      delete _blobRef;
    }
}

// Assignment operator (reference counted)
Magick::Blob Magick::Blob::operator= ( const Magick::Blob& blob_ )
{
  ++blob_._blobRef->_refCount;
  if ( --_blobRef->_refCount == 0 )
    {
      delete _blobRef;
    }
  _blobRef = blob_._blobRef;

  return *this;
}

// Update object contents, making a copy of the supplied data.
// Any existing data in the object is deallocated.
void Magick::Blob::update ( const void* data_, unsigned long length_ )
{
  if ( --_blobRef->_refCount == 0 )
    {
      // Delete old blob reference with associated data
      delete _blobRef;
    }

  _blobRef = new Magick::BlobRef( data_, length_ );
}

//
// Implementation of Magick::BlobRef
//

// Construct with data
Magick::BlobRef::BlobRef ( const void* data_, unsigned long length_ )
  : _data(0),
    _length(length_),
    _refCount(0)
{
  if ( data_ != 0 )
    {
      // Use ImageMagick's allocator to avoid possible problems from mixed
      // allocators
      _data = MagickLib::AllocateMemory( length_ );
      
      if ( _data == 0 )
	throw std::bad_alloc();
      
      memcpy( _data, data_, length_ );
    }
}

// Destructor (actually destroys data)
Magick::BlobRef::~BlobRef ( void )
{
  if ( _data )
    MagickLib::FreeMemory( _data );

  _data = 0;
  _length = 0;
  _refCount = 0;
}
