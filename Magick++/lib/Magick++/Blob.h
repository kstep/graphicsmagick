// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001
//
// Reference counted container class for Binary Large Objects (BLOBs)
//

#if !defined(Magick_Blob_header)
#define Magick_Blob_header

#include "Magick++/Thread.h"

namespace Magick
{
  // Forward decl
  class BlobRef;

  class Blob
  {

    friend class Image;

  public:

    enum Allocator
    {
      MallocAllocator,
      NewAllocator
    };

    // Default constructor
    Blob ( void );

    // Construct object with data, making a copy of the supplied data.
    Blob ( const void* data_, size_t length_ );

    // Copy constructor (reference counted)
    Blob ( const Blob& blob_ );

    // Destructor (reference counted)
    virtual       ~Blob ();

    // Assignment operator (reference counted)
    Blob          operator= ( const Blob& blob_ );

    // Update object contents, making a copy of the supplied data.
    // Any existing data in the object is deallocated.
    void          update ( const void* data_, size_t length_ );

    // Update object contents, using supplied pointer directly (no
    // copy). Any existing data in the object is deallocated.  The user
    // must ensure that the pointer supplied is not deleted or
    // otherwise modified after it has been supplied to this method.
    // Specify allocator_ as "MallocAllocator" if memory is allocated
    // via the C language malloc() function, or "NewAllocator" if
    // memory is allocated via C++ 'new'.
    void          updateNoCopy ( void* data_, size_t length_,
                                 Allocator allocator_ = NewAllocator );

    // Obtain pointer to data
    const void*   data ( void ) const;

    // Obtain data length
    size_t length ( void ) const;

  protected:

  private:
    BlobRef * _blobRef;
  };

  //
  // Reference counted access to Blob data
  // (Private implementation class)
  //
  class BlobRef {
    friend class Blob;
  public:
    // There are no public methods in this class

  private:
    // Construct with data, making private copy of data
    BlobRef ( const void* data_, size_t length_ );

    // Destructor (actually destroys data)
    ~BlobRef ( void );

    // Copy constructor and assignment are not supported
    BlobRef (const BlobRef&);
    BlobRef operator= (const BlobRef&);

    void *          _data;     // Blob data
    size_t          _length;   // Blob length
    Blob::Allocator _allocator; // Memory allocation system in use
    int             _refCount; // Reference count
    MutexLock       _mutexLock;// Mutex lock
  };

} // namespace Magick

// Obtain pointer to data
inline const void* Magick::Blob::data( void ) const
{
  return _blobRef->_data;
}

// Obtain data length
inline size_t Magick::Blob::length( void ) const
{
  return _blobRef->_length;
}

#endif // Magick_Blob_header
