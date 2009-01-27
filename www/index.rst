=======================================
GraphicsMagick Image Processing System
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
              Perl Magic, WebMagick, Web Magic, image processing,
              software development, image, software, Magick++


.. _GraphicsMagic-1.1 : http://www.graphicsmagick.org/1.1/
.. _download GraphicsMagic-1.1 : http://sourceforge.net/project/showfiles.php?group_id=73485
.. _GraphicsMagic-1.2 : http://www.graphicsmagick.org/1.2/
.. _download GraphicsMagic-1.2 : http://sourceforge.net/project/showfiles.php?group_id=73485
.. _GraphicsMagic-1.3 : http://www.graphicsmagick.org/1.3/
.. _download GraphicsMagic-1.3 : http://sourceforge.net/project/showfiles.php?group_id=73485

.. _programming : programming.html

========================  ===============================================
Legacy Stable Branch:     1.1.14__ (Released June 5, 2008) download__
Previous Stable Branch:   1.2.7__ (Released January 16, 2009) download__
Current Stable Branch:    1.3.5__ (Released January 26, 2009) download__
========================  ===============================================

__ `GraphicsMagic-1.1`_
__ `download GraphicsMagic-1.1`_
__ `GraphicsMagic-1.2`_
__ `download GraphicsMagic-1.2`_
__ `GraphicsMagic-1.3`_
__ `download GraphicsMagic-1.3`_


Check http://www.GraphicsMagick.org/ for the latest version of this page.

.. _ImageMagick : http://www.imagemagick.org/
.. _Open Source Initiative : http://www.opensource.org/
.. _FSF : http://www.fsf.org/
.. _GPL Version 2 :  http://www.fsf.org/licenses/licenses.html
.. _OpenMP : OpenMP.html
.. _`benchmarks` : benchmarks.html
.. _`SLOCCount` : http://www.dwheeler.com/sloccount/

GraphicsMagick is the swiss army knife of image processing. Comprised of
248K physical lines (according to `SLOCCount`_) of source code in the
base package (or 891K including 3rd party libraries) it provides a robust
and efficient collection of tools and libraries which support reading,
writing, and manipulating an image in over 88 major formats including
important formats like DPX, GIF, JPEG, JPEG-2000, PNG, PDF, PNM, and TIFF.

Image processing is multi-threaded using OpenMP_ so that CPU-bound tasks
scale linearly as processor cores are added. OpenMP_ support requires
compilation with GCC 4.2 (or later), or use of any C compiler supporting
at least the OpenMP_ 2.0 specification.

GraphicsMagick is quite portable, and compiles under almost every general
purpose operating system that runs on 32-bit or 64-bit CPUs.
GraphicsMagick is available for virtually any Unix or Unix-like system,
including Linux. It also runs under `Windows <INSTALL-windows.html>`_
2000 and later (Windows 2000, XP, and Vista), and MacOS-X. The source
code still supports execution under Windows '98.

GraphicsMagick supports huge images and has been tested with
gigapixel-size images. GraphicsMagick can create new images on the fly,
making it suitable for building dynamic Web applications. GraphicsMagick
may be used to resize, rotate, sharpen, color reduce, or add special
effects to an image and save the result in the same or differing image
format. Image processing operations are available from the command line,
as well as through C, C++, Perl, Tcl, Ruby, or Windows COM programming
interfaces. With some modification, language extensions for ImageMagick
may be used.

GraphicsMagick is originally derived from ImageMagick_ 5.5.2 but has been
completely independent of the ImageMagick project since then. Since the
fork from ImageMagick in 2002, many improvements have been made (`see
news <NEWS.html>`_) by `many authors <authors.html>`_ using an open
development model but without breaking the API or utilities operation.

Here are some reasons to prefer GraphicsMagick over ImageMagick:

  * GM is much more efficient (see the `benchmarks`_) so it gets the job
    done faster using fewer resources.

  * GM is much smaller and tighter.

  * GM is used to process billions of files at the world's largest photo
    sites.
  
  * GM does not does not conflict with other installed software.

  * GM suffers from fewer security issues and exploits.
  
  * GM comes with a comprehensive manual page.
  
  * GM provides API and ABI stability and managed releases that you can
    count on.
  
  * GM provides detailed yet comprehensible ChangeLog and NEWS files.
  
  * GM is available for free, and may be used to support both open and
    proprietary applications.
  
  * GM is distributed under an X11-style license ("MIT License"),
    approved by the `Open Source Initiative`_.

  * GM developers contribute to other free projects for the public good.

GraphicsMagick is `copyrighted <Copyright.html>`_ by the GraphicsMagick
Group as well as many others.

Here are just a few `examples <images/examples.jpg>`_ of what GraphicsMagick
can do:

  * Convert an image from one format to another (e.g. TIFF to JPEG)
  
  * Resize, rotate, sharpen, color reduce, or add special effects to an
    image
  
  * Create a montage of image thumbnails  
  
  * Create a transparent image suitable for use on the Web
  
  * Turn a group of images into a GIF animation sequence
  
  * Create a composite image by combining several separate images  
  
  * Draw shapes or text on an image  
  
  * Decorate an image with a border or frame  
  
  * Describe the format and characteristics of an image


.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2009

