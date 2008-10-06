=======================
GraphicsMagick Download
=======================

.. meta::
   :description: GraphicsMagick is a robust collection of tools and
                 libraries to read, write, and manipulate an image in any
                 of the more popular image formats including GIF, JPEG,
                 PNG, PDF, and Photo CD. With GraphicsMagick you can
                 create GIFs dynamically making it suitable for Web
                 applications. You can also resize, rotate, sharpen,
                 color reduce, or add special effects to an image and
                 save your completed work in the same or differing image
                 format.

   :keywords: GraphicsMagick, Image Magick, Image Magic, PerlMagick,
              Perl Magick, Perl Magic, CineMagick, PixelMagick, Pixel
              Magic, WebMagick, Web Magic, visualization, image
              processing, software development, simulation, image,
              software, AniMagick, Animagic, Magick++

.. _Bob Friesenhahn : mailto:bfriesen@graphicsmagick.org
.. _GraphicsMagick FTP : ftp://ftp.graphicsmagick.org/pub/GraphicsMagick/
.. _beta : ftp://ftp.graphicsmagick.org/pub/GraphicsMagick/beta/README.html
.. _delegates : ftp://ftp.graphicsmagick.org/pub/GraphicsMagick/delegates/
.. _linux : ftp://ftp.graphicsmagick.org/pub/GraphicsMagick/linux/
.. _snapshots : ftp://ftp.graphicsmagick.org/pub/GraphicsMagick/snapshots/
.. _windows : ftp://ftp.graphicsmagick.org/pub/GraphicsMagick/windows/
.. _Cygwin : http://www.cygwin.com/
.. _MinGW : http://www.mingw.org/
.. _SourceForge Download : http://sourceforge.net/project/showfiles.php?group_id=73485
.. _ftp.graphicsmagick.org: ftp://ftp.graphicsmagick.org/pub/GraphicsMagick/

The source distribution of GraphicsMagick as well as pre-compiled
binaries are available at the sites listed here (unless otherwise noted).
At the moment, the fastest way to obtain formal releases is from the
`SourceForge Download`_ page since SourceForge has lots of bandwidth, and
many mirror sites. Snapshot packages should usually be retrieved from
the `GraphicsMagick FTP`_ site.

The organization of the GraphicsMagick directory on ftp sites is shown in the
following table:

+-----------------+-------------------------------------------------+
|    Directory    |                   Description                   |
+=================+=================================================+
| `GraphicsMagick | Contains the core GraphicsMagick sources in a   |
| FTP`_           | file named similar to                           |
|                 | GraphicsMagick-1.2.tar.gz,                      |
|                 | GraphicsMagick-1.2.tar.bz, or                   |
|                 | GraphicsMagick-1.2.zip. These core sources are  |
|                 | sufficient to compile a minimal GraphicsMagick  |
|                 | on a Unix system (including MacOS-X), or under  |
|                 | the Cygwin and MinGW environments for Windows.  |
+-----------------+-------------------------------------------------+
|         `beta`_ | Beta versions of GraphicsMagick.                |
+-----------------+-------------------------------------------------+
|    `delegates`_ | Add on third-party libraries and applications   |
|                 | which extend the formats supported by           |
|                 | GraphicsMagick may be obtained from this        |
|                 | directory. Windows users can usually ignore the |
|                 | contents of this directory since all common     |
|                 | delegates are included in the Windows source    |
|                 | and binaries packages.                          |
+-----------------+-------------------------------------------------+
|        `linux`_ | Linux source RPM (SRPM) packages for Red Hat    |
|                 | Linux, other Linux systems, or any system with  |
|                 | RPM installed, may be found here.               |
+-----------------+-------------------------------------------------+
|    `snapshots`_ | Snapshots (between releases) copies of          |
|                 | GraphicsMagick may be found here. New snapshots |
|                 | may be cut every few days as time permits.      |
|                 | Quality may vary.                               |
+-----------------+-------------------------------------------------+
|      `windows`_ | Installable binary packages and the extended    |
|                 | source code package for use with Microsoft      |
|                 | Visual C++ (including configured delegates and  |
|                 | a Visual C++ workspace) may be found here.      |
+-----------------+-------------------------------------------------+

If you operate an GraphicsMagick mirror site, or know of additional mirrors of
the GraphicsMagick archives, please send E-mail to Bob Friesenhahn so that they
can be listed here. Known mirror sites are provided with privileged access to
the master site so that they are not denied access when the default user limit
has been reached.

+--------------------------------+-------+----------+-------------------------------+
|              Site              | Max   | Location |             Notes             |
|                                | Users |          |                               |
+================================+=======+==========+===============================+
| `SourceForge Download`_        | 1000  | Fremont, | HTTP download.                |
|                                |       | CA USA   |                               |
+--------------------------------+-------+----------+-------------------------------+
|                                |       |          | Master GraphicsMagick ftp     |
|                                |       |          | site. This is a good site to  |
|                                |       |          | mirror from. If you provide a |
|                                |       | Dallas,  | public mirror of              |
| `ftp.graphicsmagick.org`_      | 5     | Texas    | GraphicsMagick (and can       |
|                                |       | USA      | provide a verifiable URL),    |
|                                |       |          | please send an email to       |
|                                |       |          | bfriesen@graphicsmagick.org   |
|                                |       |          | for your mirror to be given   |
|                                |       |          | priority access.              |
+--------------------------------+-------+----------+-------------------------------+

If you get the message::

 530 User anonymous access denied.

it means the site has reached its limit of anonymous users. You will need to
try another site or try again later.

-------------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2008

