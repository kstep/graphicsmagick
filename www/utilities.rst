=======================================
GraphicsMagick Utilities
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
              Perl Magic, CineMagick, PixelMagick, Pixel Magic, WebMagick,
              Web Magic, visualization, image processing, software development,
              simulation, image, software, AniMagick, Animagic,  Magick++


.. _programming : programming.html
.. _animate : animate.html
.. _composite : composite.html
.. _compare : compare.html
.. _conjure : conjure.html
.. _convert : convert.html
.. _display : display.html
.. _identify : identify.html
.. _import : import.html
.. _mogrify : mogrify.html
.. _montage : montage.html


GraphicsMagick provides a powerful command line utility which may be used
to access all GraphicsMagick functions. Gm uses a consistent set of
options (`see options documentation <GraphicsMagick.html>`_).
GraphicsMagick provides access to major command modes via a single
executable command-line program called ``gm``; for example, to use the
"convert" mode, type ``gm convert ...``. The available command modes and
links to their documentation are shown in the following table:

===============  =========================================================================
GM Command Mode  Description
===============  =========================================================================
gm animate_      animate a sequence of images
gm composite_    composite images together
gm conjure_      execute a Magick Scripting Language (MSL) XML script
gm compare_      compare two images using statistics and/or visual differencing
gm convert_      convert an image or sequence of images
gm display_      display an image on a workstation running X
gm identify_     describe an image or image sequence
gm import_       capture an application or X server screen
gm mogrify_      transform an image or sequence of images
gm montage_      create a composite image (in a grid) from separate images
===============  =========================================================================

The ``gm`` utility makes it easy to perform complex image processing
operations using a single command. It may be invoked from scripts, or
from other programs in order to automate the processing of thousands of
images.


.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2008

