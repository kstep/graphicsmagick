.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

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
              Perl Magic, image processing, software development, image, software,
              Magick++


.. _animate : animate.html
.. _batch: batch.html
.. _benchmark: benchmark.html
.. _compare : compare.html
.. _composite : composite.html
.. _conjure : conjure.html
.. _convert : convert.html
.. _display : display.html
.. _identify : identify.html
.. _import : import.html
.. _mogrify : mogrify.html
.. _montage : montage.html
.. _programming : programming.html
.. _time: time.html
.. _version: version.html


GraphicsMagick provides a powerful command line utility `gm`, which
may be used to access all GraphicsMagick functions. Gm uses a
consistent set of options (`see options documentation
<GraphicsMagick.html>`_).  GraphicsMagick provides access to major
commands via a single executable command-line program; for example, to
use the "convert" sub-command, type ``gm convert ...``. The available
commands are as follows:

===============  =========================================================================
animate_         Animate a sequence of images
batch_           Executes an arbitary number of utility commands
benchmark_       Measure and report utility command performance.
compare_         Compare two images using statistics and/or visual differencing
composite_       Composite images together
conjure_         Execute a Magick Scripting Language (MSL) XML script
convert_         Convert an image or sequence of images
display_         Display an image on a workstation running X
identify_        Describe an image or image sequence
import_          Capture an application or X server screen
mogrify_         Transform an image or sequence of images
montage_         Create a composite image (in a grid) from separate images
time_            Time the execution of a utility command.
version_         Report GraphicsMagick version, features, and build options.
===============  =========================================================================

--------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2015

