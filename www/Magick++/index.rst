===============================
Magick++ API for GraphicsMagick
===============================

.. _GraphicsMagick : ../index.html
.. _NEWS : NEWS.html
.. _ChangeLog : ChangeLog.html
.. _`Standard Template Library` : http://www.sgi.com/tech/stl/
.. _STL : http://www.sgi.com/tech/stl/
.. _deque : http://www.sgi.com/tech/stl/Deque.html
.. _vector : http://www.sgi.com/tech/stl/Vector.html
.. _list : http://www.sgi.com/tech/stl/List.html
.. _map : http://www.sgi.com/tech/stl/Map.html
.. _documentation : Documentation.html
.. _ftp : ../download.html
.. _CVS : ../CVS.html
.. _PerlMagick : ../perl.html
.. _`GraphicsMagick Bug Tracker` : http://sourceforge.net/projects/graphicsmagick/
.. _`Bob Friesenhahn` : mailto:bfriesen@simple.dallas.tx.us

Introduction
------------

*Magick++* is the object-oriented C++ API to the GraphicsMagick_
image-processing library, the most comprehensive open-source image
processing package available. Read the latest NEWS_ and ChangeLog_ for
*Magick++*.

*Magick++* supports an object model which is inspired by PerlMagick_.
Images support implicit reference counting so that copy constructors and
assignment incur almost no cost. The cost of actually copying an image
(if necessary) is done just before modification and this copy is managed
automatically by *Magick++*. De-referenced copies are automatically
deleted. The image objects support value (rather than pointer) semantics
so it is trivial to support multiple generations of an image in memory at
one time.

*Magick++* provides integrated support for the `Standard Template
Library`_ (STL_) so that the powerful containers available (e.g. deque_,
vector_, list_, and map_) can be used to write programs similar to those
possible with PERL & PerlMagick_. STL-compatible template versions of
GraphicsMagick's list-style operations are provided so that operations
may be performed on multiple images stored in STL containers.

Documentation
-------------

Detailed `documentation`_ are provided for all *Magick++* classes, class
methods, and template functions which comprise the API.

Obtaining Magick++
------------------

*Magick++* is included as part of GraphicsMagick_ source releases and may
be retrieved via `ftp`_ or `CVS`_.

Installation
------------

Installation is very easy since *Magick++* is part of GraphicsMagick_ and
is built by default. Once GraphicsMagick_ is built, then *Magick++* is
available for use.

Usage
-----

A helper script named *GraphicsMagick++-config* is installed in the same
directory as the GraphicsMagick *gm* program under Unix which assists
with recalling compilation options required to compile and link programs
which depend on *Magick++*. For example, the following command will
compile and link the source file example.cpp to produce the executable
example (notice that quotes are backward quotes)::

  c++ -o example example.cpp `GraphicsMagick++-config --cppflags --cxxflags
    --ldflags --libs`

Windows users may get started by manually editing a project file for one
of the *Magick++* demo programs.

It is useful to initialize the GraphicsMagick library prior to using the
*Magick++* library. This initialization is performed by passing the path
to the GraphicsMagick DLLs (assumed to be in the same directory as your
program) to the InitializeMagick() function call. This is commonly
performed by providing the path to your program (argv[0]) as shown in the
following example::

  int main( int /*argc*/, char ** argv)
  {
    InitializeMagick(*argv);

If you don't have the path to your executable, then pass NULL and usually
the library will be found anyway. Besides helping to find the
GraphicsMagick DLL/library and configuration files, InitializeMagick()
initializes all of the semaphores necessary for a multi-threaded program
to be thread safe.

Reporting Bugs
--------------

Please report any bugs via the `GraphicsMagick Bug Tracker`_. Questions
regarding usage should be directed to `Bob Friesenhahn`_.

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| Bob Friesenhahn 1999 - 2008


