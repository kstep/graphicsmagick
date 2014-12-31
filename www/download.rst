.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=======================
GraphicsMagick Download
=======================

.. _Bob Friesenhahn : mailto:bfriesen@graphicsmagick.org
.. _GraphicsMagick FTP : ftp://ftp.graphicsmagick.org/pub/GraphicsMagick/
.. _delegates : ftp://ftp.graphicsmagick.org/pub/GraphicsMagick/delegates/
.. _linux : ftp://ftp.graphicsmagick.org/pub/GraphicsMagick/linux/
.. _snapshots : ftp://ftp.graphicsmagick.org/pub/GraphicsMagick/snapshots/
.. _windows : ftp://ftp.graphicsmagick.org/pub/GraphicsMagick/windows/
.. _Cygwin : http://www.cygwin.com/
.. _MinGW : http://www.mingw.org/
.. _SourceForge Download : http://sourceforge.net/projects/graphicsmagick/files/
.. _ftp.graphicsmagick.org: ftp://ftp.graphicsmagick.org/pub/GraphicsMagick/

.. contents::
  :local:

The source distribution of GraphicsMagick as well as pre-compiled
binaries are available at the sites listed here (unless otherwise
noted).  At the moment, the fastest way to obtain formal releases is
from the `SourceForge Download`_ page since SourceForge has lots of
bandwidth, and many mirror sites. Snapshot packages should usually be
retrieved from the `GraphicsMagick FTP`_ site or one of its mirrors.
There are mirrors of the ftp site in Czechoslovakia and Poland.

Ftp Site Organization
---------------------

The organization of the GraphicsMagick directory on ftp sites is as
follows:

`GraphicsMagick FTP`_

   Contains the core GraphicsMagick sources in a file named similar to
   GraphicsMagick-1.3.tar.bz2, GraphicsMagick-1.3.tar.gz, or
   GraphicsMagick-1.3.tar.xz. These core sources are sufficient to
   compile a minimal GraphicsMagick on a Unix system (including
   MacOS-X), or under the Cygwin and MinGW environments for Windows.
   Additional development packages need to be installed in advance to
   support formats like JPEG and PNG.

`delegates`_

   Add on third-party libraries and applications which extend the
   formats supported by GraphicsMagick may be obtained from this
   directory. Windows users can usually ignore the contents of this
   directory since all common delegates are included in the Windows
   source and binaries packages.

`linux`_

  Linux source RPM (SRPM) packages for Red Hat Linux, other Linux
  systems, or any system with RPM installed, may be found here.

`snapshots`_

  Snapshots (between releases) copies of GraphicsMagick may be found
  here. New snapshots may be cut every few days as time
  permits. Quality may vary (but is usually excellent).

`windows`_

  Installable binary packages and the extended source code package for
  use with Microsoft Visual C++ (including configured delegates and a
  Visual C++ workspace) may be found here.

Download Sites
--------------

Here are some known download sites for GraphicsMagick:

  `SourceForge file download <https://sourceforge.net/projects/graphicsmagick/files/>`_ (http protocol)

  `Czechoslovakian ftp mirror <http://78.108.103.11/MIRROR/ftp/GraphicsMagick/>`_ (http protocol)

  `Polish ftp mirror via ftp <ftp://ftp.icm.edu.pl/pub/unix/graphics/GraphicsMagick/>`_ (ftp protocol)

  `Polish ftp mirror via http <http://ftp.icm.edu.pl/pub/unix/graphics/GraphicsMagick/>`_ (http protocol)

--------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2015
