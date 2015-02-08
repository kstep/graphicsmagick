.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

============
Magick::Blob
============

Blob provides the means to contain any opaque data. It is named after
the term `Binary Large OBject
<http://en.wikipedia.org/wiki/Binary_large_object>`_ commonly used to
describe unstructured data (such as encoded images) which are stored
in a database. While the function of Blob is very simple (store a
pointer and and size associated with allocated data), the Blob class
provides some very useful capabilities. In particular, it is fully
reference counted just like the Image class.

The Blob class supports value assignment while preserving any
outstanding earlier versions of the object. Since assignment is via a
pointer internally, Blob is efficient enough to be stored directly in
an STL container or any other data structure which requires
assignment. In particular, by storing a Blob in an `associative
container <http://www.sgi.com/tech/stl/AssociativeContainer.html>`_
(such as STL's `map <http://www.sgi.com/tech/stl/Map.html>`_) it is
possible to create simple indexed in-memory "database" of Blobs.

Magick++ currently uses Blob to contain encoded images (e.g. JPEG) as
well as ICC and IPTC profiles. Since Blob is a general-purpose class,
it may be used for other purposes as well.

The following is the definition of the Magick::Blob class::

  class Blob
  {

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
    Blob&         operator= ( const Blob& blob_ );

    // Update object contents from Base64-encoded string representation.
    void          base64 ( const std::string base64_ );
    // Return Base64-encoded string representation.
    std::string   base64 ( void );

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

    // Obtain pointer to data. The user should never try to modify or
    // free this data since the Blob class manages its own data. The
    // user must be finished with the data before allowing the Blob to
    // be destroyed since the pointer is invalid once the Blob is
    // destroyed.
    const void*   data ( void ) const;

    // Obtain data length
    size_t length ( void ) const;

  };

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| Bob Friesenhahn 1999 - 2015

