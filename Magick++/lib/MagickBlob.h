// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Reference counted container class for Binary Large Objects (BLOBs)
//

#if !defined(Blob_header)
#define Blob_header

namespace Magick
{

  // Forward decl
  class BlobRef;

  class Blob
  {
  public:
    // Default constructor
    Blob ( void );

    // Construct object with data, making a copy of the supplied data.
    Blob ( const void* data_, unsigned long length_ );

    // Copy constructor (reference counted)
    Blob ( const Blob& blob_ );

    // Destructor (reference counted)
    virtual ~Blob ();

    // Assignment operator (reference counted)
    Blob operator= ( const Blob& blob_ );

    // Update object contents, making a copy of the supplied data.
    // Any existing data in the object is deallocated.
    void          update ( const void* data_, unsigned long length_ );

    // Obtain pointer to data
    const void*   data ( void ) const;

    // Obtain data length
    unsigned long length ( void ) const;

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
    // Construct with data
    BlobRef ( const void* data_, unsigned long length_ );

    // Destructor (actually destroys data)
    ~BlobRef ( void );

    // Copy constructor and assignment are not supported
    BlobRef (const BlobRef&);
    BlobRef operator= (const BlobRef&);

    void *        _data;     // Blob data
    unsigned long _length;   // Blob length
    int           _refCount; // Reference count
  };

} // namespace Magick

// Obtain pointer to data
inline const void* Magick::Blob::data( void ) const
{
  return _blobRef->_data;
}

// Obtain data length
inline unsigned long Magick::Blob::length( void ) const
{
  return _blobRef->_length;
}

#endif // Blob_header
