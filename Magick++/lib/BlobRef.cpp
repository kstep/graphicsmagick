// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001, 2002
//
// Implementation of Blob
//

#define MAGICK_IMPLEMENTATION

#include "Magick++/Include.h"
#include "Magick++/Thread.h"
#include "Magick++/BlobRef.h"

#include <string.h>

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
      delete [] static_cast<unsigned char*>(_data);
  if ( _allocator == Blob::MallocAllocator )
    LiberateMemory(static_cast<void **>(&_data));
}
