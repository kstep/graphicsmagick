// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001
//
// Implementation of Blob
//

#define MAGICK_IMPLEMENTATION

#include <string.h>

#include "Magick++/Thread.h"
#include "Magick++/Include.h"
#include "Magick++/Blob.h"

//
// Implementation of Magick::Blob
//

// Default constructor
Magick::Blob::Blob ( void )
  : _blobRef(new Magick::BlobRef( 0, 0 ))
{
}

// Construct with data
Magick::Blob::Blob ( const void* data_, size_t length_ )
  : _blobRef(new Magick::BlobRef( data_, length_ ))
{
}

// Copy constructor (reference counted)
Magick::Blob::Blob ( const Magick::Blob& blob_ )
  : _blobRef(blob_._blobRef)
{
  // Increase reference count
  Lock( &_blobRef->_mutexLock );
  ++_blobRef->_refCount;
}

// Destructor (reference counted)
Magick::Blob::~Blob ()
{
  bool doDelete = false;
  {
    Lock( &_blobRef->_mutexLock );
    if ( --_blobRef->_refCount == 0 )
      doDelete = true;
  }

  if ( doDelete )
    {
      // Delete old blob reference with associated data
      delete _blobRef;
    }
}

// Assignment operator (reference counted)
Magick::Blob Magick::Blob::operator= ( const Magick::Blob& blob_ )
{
  {
    Lock( &blob_._blobRef->_mutexLock );
    ++blob_._blobRef->_refCount;
  }
  bool doDelete = false;
  {
    Lock( &_blobRef->_mutexLock );
    if ( --_blobRef->_refCount == 0 )
      doDelete = true;
  }
  if ( doDelete )
    {
      delete _blobRef;
    }
  _blobRef = blob_._blobRef;

  return *this;
}

// Update object contents, making a copy of the supplied data.
// Any existing data in the object is deallocated.
void Magick::Blob::update ( const void* data_, size_t length_ )
{
  bool doDelete = false;
  {
    Lock( &_blobRef->_mutexLock );
    if ( --_blobRef->_refCount == 0 )
      doDelete = true;
  }
  if ( doDelete )
    {
      // Delete old blob reference with associated data
      delete _blobRef;
    }

  _blobRef = new Magick::BlobRef( data_, length_ );
}

// Update object contents, using supplied pointer directly (no copy)
// Any existing data in the object is deallocated.  The user must
// ensure that the pointer supplied is not deleted or otherwise
// modified after it has been supplied to this method.
void Magick::Blob::updateNoCopy ( void* data_, size_t length_,
                                  Allocator allocator_  )
{
  bool doDelete = false;
  {
    Lock( &_blobRef->_mutexLock );
    if ( --_blobRef->_refCount == 0 )
      doDelete = true;
  }
  if ( doDelete )
    {
      // Delete old blob reference with associated data
      delete _blobRef;
    }
  _blobRef = new Magick::BlobRef( 0, 0 );
  _blobRef->_data   = data_;
  _blobRef->_length = length_;
  _blobRef->_allocator = allocator_;
}

//
// Implementation of Magick::BlobRef
//

// Construct with data, making private copy of data
Magick::BlobRef::BlobRef ( const void* data_,
			   size_t length_ )
  : _data(0),
    _length(length_),
    _allocator(Blob::NewAllocator),
    _refCount(1),
    _mutexLock()
{
  if( data_ )
    {
      _data = new unsigned char[length_];
      memcpy( _data, data_, length_ );
    }
}

// Destructor (actually destroys data)
Magick::BlobRef::~BlobRef ( void )
{
  if ( _allocator == Blob::NewAllocator )
      delete static_cast<unsigned char*>(_data);
  if ( _allocator == Blob::MallocAllocator )
    LiberateMemory(static_cast<void **>(&_data));
}
