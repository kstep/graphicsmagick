.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=======================
Magick::CoderInfo Class
=======================

The CoderInfo class supports obtaining information about
GraphicsMagick support for an image format (designated by a magick
string). It may be used to provide support for a specific named format
(provided as an argument to the constructor), or as an element of a
container when format support is queried using the coderInfoList()
templated function.

The following code fragment illustrates how CoderInfo may be used::

    CoderInfo info("GIF");
    cout << info->name() << ": (" << info->description() << ") : ";
    cout << "Readable = ";
    if ( info->isReadable() )
      cout << "true";
    else
      cout << "false";
    cout << ", ";
    cout << "Writable = ";
    if ( info->isWritable() )
      cout << "true";
    else
      cout << "false";
    cout << ", ";
    cout << "Multiframe = ";
    if ( info->isMultiframe() )
      cout << "true";
    else
      cout << "false";
    cout << endl; 

The definition of class 'Magick::CoderInfo' is::

  namespace Magick
  {
    class MagickDLLDecl CoderInfo
    {
    public:

      enum MatchType {
        AnyMatch,	// match any coder
        TrueMatch,	// match coder if true
        FalseMatch	// match coder if false
      };

      // Default constructor
      CoderInfo ( void );

      // Copy constructor
      CoderInfo ( const CoderInfo &coder_ );

      // Construct with coder name
      CoderInfo ( const std::string &name_ );

      // Destructor
      ~CoderInfo ( void );

      // Format name
      std::string name( void ) const;

      // Format description
      std::string description( void ) const;

      // Format is readable
      bool isReadable( void ) const;

      // Format is writeable
      bool isWritable( void ) const;

      // Format supports multiple frames
      bool isMultiFrame( void ) const;

      // Assignment operator
      CoderInfo& operator= (const CoderInfo &coder_ );

    };
  }

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| Bob Friesenhahn 1999 - 2015

