==========================
Magick++ API Documentation
==========================

.. _Blob : Blob.html
.. _CoderInfo : CoderInfo.html
.. _Color : Color.html
.. _Drawable : Drawable.html
.. _Exception : Exception.html
.. _Geometry : Geometry.html
.. _Image : Image.html
.. _Montage : Montage.html
.. _Pixels : Pixels.html
.. _STL : STL.html
.. _TypeMetric : TypeMetric.html
.. _`Image::fontTypeMetrics` : Image.html#fontTypeMetrics
.. _`algorithms and function objects` : STL.html
.. _`coderInfoList` : STL.html#coderInfoList

*Magick++* provides a simple C++ API to the GraphicsMagick image
processing library which supports reading and writing a huge number of
image formats as well as supporting a broad spectrum of traditional image
processing operations. The GraphicsMagick C API is complex and the data
structures are not well documented. *Magick++* provides access to most of
the features available from the C API but in a simple object-oriented and
well-documented framework.

*Magick++* is intended to support commercial-grade application
development. In order to avoid possible conflicts with the user's
application, all symbols contained in *Magick++* (included by the header
<Magick++.h>) are scoped to the namespace Magick. Symbols from the
GraphicsMagick C library are imported under the MagickLib namespace to
avoid possible conflicts and GraphicsMagick macros are only included
within the *Magick++* implementation so they won't impact the user's
application.

The core class in *Magick++* is the `Image`_ class. The `Image`_ class
provides methods to manipulate a single image frame (e.g. a JPEG image).
Standard Template Library (`STL`_) compatible algorithms and function
objects are provided in order to manipulate multiple image frames or to
read and write file formats which support multiple image frames (e.g. GIF
animations, MPEG animations, and Postscript files).

The `Image`_ class supports reference-counted memory management which
supports the semantics of an intrinsic variable type (e.g. 'int') with an
extremely efficient operator = and copy constructor (only a pointer is
assigned) while ensuring that the image data is replicated as required so
that it the image may be modified without impacting earlier generations.
Since the `Image`_ class manages heap memory internally, images are best
allocated via C++ automatic (stack-based) memory allocation. This support
allows most programs using *Magick++* to be written without using any
pointers, simplifying the implementation and avoiding the risks of using
pointers. When a program uses automatic memory allocation to allocate
*Magick++* images, that aspect of the program becomes naturally
exception-safe and thread-safe.

The image class uses a number of supportive classes in order to specify
arguments. Colors are specified via the `Color`_ class. Colors specified
in X11-style string form are implicitly converted to the `Color`_ class.
Geometry arguments (those specifying width, height, and/or x and y
offset) are specified via the `Geometry`_ class. Similar to the `Color`_
class, geometries specified as an X11-style string are implicitly
converted to the `Geometry`_ class. Two dimensional drawable objects are
specified via the `Drawable`_ class. Drawable objects may be provided as
a single object or as a list of objects to be rendered using the current
image options. `Montage`_ options (a montage is a rendered grid of
thumbnails in one image) are specified via the `Montage`_ class.

Errors are reported using C++ exceptions derived from the `Exception`_
class, which is itself derived from the standard C++ exception class.
Exceptions are reported synchronous with the operation and are caught by
the first matching try block as the stack is unraveled. This allows a
clean coding style in which multiple related *Magick++* commands may be
executed with errors handled as a unit rather than line-by-line. Since
the `Image`_ object provides reference-counted memory management,
unreferenced images on the stack are automatically cleaned up, avoiding
the potential for memory leaks.

For ease of access, the documentation for the available user-level
classes is available via the following table:


.. table:: *Magick++* User-Level Classes

   +-------------+----------------------------------------------------------+
   |`Blob`_      |Binary Large OBject container.                            |
   +-------------+----------------------------------------------------------+
   |`CoderInfo`_ |Report information about supported image formats (use with|
   |             |`coderInfoList`_\(\))                                     |
   +-------------+----------------------------------------------------------+
   |`Color`_     |Color specification.                                      |
   +-------------+----------------------------------------------------------+
   |`Drawable`_  |Drawable shape (for input to 'draw').                     |
   +-------------+----------------------------------------------------------+
   |`Exception`_ |C++ exception objects.                                    |
   +-------------+----------------------------------------------------------+
   |`Geometry`_  |Geometry specification.                                   |
   +-------------+----------------------------------------------------------+
   |`Image`_     |Image frame.  This is the primary object in *Magick++*.   |
   +-------------+----------------------------------------------------------+
   |`Montage`_   |Montage options for montageImages().                      |
   +-------------+----------------------------------------------------------+
   |`Pixels`_    |Low-level access to image pixels.                         |
   +-------------+----------------------------------------------------------+
   |`STL`_       |STL algorithms and function objects for operating on      |
   |             |containers of image frames.                               |
   +-------------+----------------------------------------------------------+
   |`TypeMetric`_|Container for font type metrics \(use with                |
   |             |`Image::fontTypeMetrics`_).                               |
   +-------------+----------------------------------------------------------+


.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| Bob Friesenhahn 1999 - 2008


