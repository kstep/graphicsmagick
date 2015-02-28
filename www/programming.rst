.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=======================================
GraphicsMagick Programming Interfaces
=======================================

.. meta::
   :description: GraphicsMagick is a robust collection of tools and libraries to read,
                 write, and manipulate an image in any of the more popular
                 image formats including GIF, JPEG, PNG, PDF, and Photo CD.
                 With GraphicsMagick you can create GIFs dynamically making it
                 suitable for Web applications.  You can also resize, rotate,
                 sharpen, color reduce, or add special effects to an image and
                 save your completed work in the same or differing image format.

   :keywords: GraphicsMagick, Image Magick, Image Magic, PerlMagick, Perl Magick,
              Perl Magic, image processing, software development, image, software,
              Magick++


.. _`C Core` : api/api.html
.. _`C Wand` : wand/wand.html
.. _C++ : Magick++/index.html
.. _`node.js` : http://aheckmann.github.com/gm/
.. _`Lua` : http://github.com/clementfarabet/graphicsmagick
.. _PHP : http://pecl.php.net/package/gmagick
.. _Perl : perl.html
.. _Python: https://bitbucket.org/hhatto/pgmagick/
.. _Ruby : http://rmagick.rubyforge.org/
.. _Tcl/Tk : http://www.graphicsmagick.org/TclMagick/doc/
.. _Windows OLE : ImageMagickObject.html
.. _`Windows .NET` : https://graphicsmagick.codeplex.com/



GraphicsMagick capabilities may be accessed from many languages and scripting
environments via programming APIs as shown in the following table:

===============  ====================================================================
Language         Description
===============  ====================================================================
`C Core`_        C language API for the lowest-level core programming interface.
`C Wand`_        C language API for the mid-level Wand API programming interface.
`C++`_           Magick++ provides an abstract object-oriented C++ interface.
`Lua`_           The gm.Image API provides an interface to GraphicsMagick's Wand API.
`node.js`_	 Graphicsmagick for node.js provides server-side support for
                 Javascript programming with node.js.
PHP_		 Gmagick provides a small and fast extension for PHP.
Perl_            PerlMagick provides an object-oriented Perl interface.
Python_          PgMagick provides the power and ease of the C++ API, but in Python.
Ruby_            RMagick provides a Ruby language extension  
`Tcl/Tk`_        TclMagick provides a scripting environment based on Tcl or Tcl/Tk.
`Windows .NET`_  GraphicsMagick.NET provides a Windows .NET programming interface,
                 allowing use of GraphicsMagick features via a stand-alone package.
`Windows OLE`_   The ImageMagickObject OLE control supports utility-style access via
                 a COM+ object.
===============  ====================================================================

*Some of these languages and scripting environments are supported by the
GraphicsMagick Group while others are developed and supported by third parties.*

--------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2015
