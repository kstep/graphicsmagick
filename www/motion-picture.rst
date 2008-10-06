======================================
GraphicsMagick Motion Picture Features
======================================

.. meta::
   :description: Describes GraphicsMagick's support for Cineon and SMPTE
                 DPX formats.

   :keywords: GraphicsMagick, Cineon, DPX, SMPTE 268M, Motion Picture

.. _GraphicsMagick : index.html
.. _SMPTE : http://www.smpte.org/
.. _Everyone's Hero : http://www.everyonesherodvd.com/flash/ehero.html
.. _Spirit 4K DataCine : http://www.grassvalley.com/products/film/spirit_4k/
.. _Northlight Scanner : http://www.filmlight.ltd.uk/
.. _Lasergraphics : http://www.lasergraphics.com/
.. _Grass Valley Viper : http://www.grassvalley.com/products/cameras/viper/
.. _DALSA Origin : http://www.dalsa.com/dc/4K_products/origin_main.asp
.. _typical pixel resolutions : http://www.surrealroad.com/digital/index.php/archives/2005/standard-data-resolutions/
.. _Digital Intermediates : http://www.digitalintermediates.org/
.. _Library Of Congress : http://www.digitalpreservation.gov/
.. _Christopher Reeve : http://en.wikipedia.org/wiki/Christopher_Reeve

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


.. contents::
  :local:

Introduction
============

Features have been added to GraphicsMagick_ 1.2 (and later) to make it
more suitable for processing images used by the motion picture (film and
high-definition) industry. In particular, industrial strength support for
SMPTE_ DPX Version 2.0 (SMPTE_ 268M-2003) has been added for
GraphicsMagick_ 1.2. Legacy 10-bit Kodak Cineon is also supported. A new
logarithmic RGB space based on Kodak's Cineon log definition has been
added to support images which use a logarithmic density encoding (with
range of 0 to 2.048) according to Kodak specifications. Support for
legacy Kodak 4.5 Cineon format has been improved.

GraphicsMagick_'s DPX support has already been put to use in the
industry. It was used to perform 16-bit TIFF to DPX conversion for
`Christopher Reeve`_'s last film "`Everyone's Hero`_". While this may not
be `Christopher Reeve`_'s most memorable work, the film's images turned
out great, and the kids are sure to enjoy the film.

The modern motion picture industry usually shoots on film and then uses a
machine known as a *datacine* (e.g. `Spirit 4K DataCine`_ or Cintel DSX)
or a *scanner* (e.g. `Northlight Scanner`_ or `Lasergraphics`_ Director) to
convert each film image into an individual image file. An alternative to
this is to use a high definition CCD camera such as the `Grass Valley
Viper`_ or the `DALSA Origin`_ to capture images to uncompressed files
directly. A rather inferior approach is to use a video high definition
compressed format like HDCAM and convert each frame to a file for
processing.

Images scanned from film are usually in either a log colorspace similar
to Cineon Log (10 bits per sample is most common), or in a linear format
(16 bits per sample is common). Some cameras (such as the `Grass Valley
Viper`_) use log encoding similar to Cineon Log. Log encoding has the
advantage that fewer bits are necessary to cover the extended dynamic
range of film, and that the log encoding approximates the density
characteristics of film (approximately linear when log encoded).

While there is some continued use of the legacy Kodak Cineon format due
to existing equipment and software, the standard image file format for
motion picture frames is SMPTE_ DPX (SMPTE_ 268M-2003). This format is
somewhat similar to the Kodak Cineon format, but has the advantage that
it is an industry standard rather than a vendor standard. DPX files are
practically always uncompressed. The design of DPX places header content
at fixed offsets, and the image raster data offsets may be computed, so
DPX files may be read or updated without rewriting the entire file.

Film images are usually captured at *2K* resolution (82 pixels/mm),
*4K* resolution (164 pixels/mm), or even *8K* resolution (328
pixels/mm), where the actual resolution values approximate the horizontal
dimension of the image. A table of `typical pixel resolutions`_ for various
film sizes may be found on the `Digital Intermediates`_ site. File sizes may
be quite large and range in size from 8MB to as much as 180MB. The common
10-bit *2K* format consumes 12MB of disk while a 10-bit *4K* scan
consumes 50MB of disk. A typical movie has an average of 144K frames (at
24 frames per second) so just one movie consumes terabytes of storage.
Businesses which specialize in handling digital intermediates may have
available on-line storage ranging from several tens of terabytes to
several hundred terabytes. While *2K* has been the standard working
resolution due to storage requirements and processing time, recently
there is a trend to scanning at *4K* resolution since most experts say
that this is the resolution necessary to capture the full resolution of a
35mm frame (but some claim more!). IMAX film (10X the area of 35mm film!)
can likely support *21K* resolution once someone builds a machine
capable of doing so but is currently scanned at *8K* resolution. Some
shops scan at *4K* but then reduce the image to *2K* as the working
format so that the full resolution is available for later use, but the
processing of the feature may be done on a normal *2K* schedule. An
alternative is to do the work at *2K* but record a record of all the
changes so that the changes may be re-played to obtain similar results at
*4K*.

The end result of the effort of encoding motion picture images to digital
image files is to have a digital master which does not degrade over time
and which has captured the maximum available information from the
original film print. This digital master may be targeted toward producing
film prints for showing in a theater, used to create DVDs, used as a
master for *digital cinema* for viewing in suitably equipped theaters,
or used to support television broadcasts.

Processing film images is a specialty business. Many business exist which
offer specialized software and hardware to process film images. Common
tasks are *color grading* (adjusting color for a correct film print),
dust busting, and compositing. While GraphicsMagick_ could (with some
effort) be used to accomplish many of these things it can never expect to
compete in these specialized areas. GraphicsMagick_ lacks the high-quality
graphical user interface necessary for a colorist or artist to interact
with a sequence of images. GraphicsMagick_ lacks the experienced and
dedicated team necessary to assure the desired results with particular
media.

Applications
============

The strength of GraphicsMagick_ versus specialized proprietary software
are its cost (absolutely free!), open source availability (user is able
to fix software flaws or tailor software to meet specific needs), general
purpose image processing capabilities, deep image capabilities (up to
32-bits per sample), excellent performance, platform independence, lack
of encumbering usage licenses, and robust implementation. Examples of
areas where GraphicsMagick_ may be used are:

  * View the image on a display.
  * Scaling (for example, *4K* to *2K* or 1920x1080 HD with excellent quality)
  * Cropping
  * Rotation
  * Filtering
  * ICC ICM profile-based color management and transformations
  * Gamma adjustment
  * Color adjustment
  * Conversion to grayscale
  * Text annotations
  * Compositions
  * Drawing on images (for example drawing markers on image)
  * Conversion to and from other formats (e.g. Kodak Cineon, TIFF, JPEG, SGI,
  * Postscript, PNG, and PNM)

GraphicsMagick_ performs well when compared with other software. Since it
is free and portable, it may be installed on any available system with no
license fees. For example, if a studio has a computing cluster of several
hundred Linux, SGI IRIX, Solaris, FreeBSD, or Mac OS X systems, then
GraphicsMagick_ may be installed across all systems and used for any
desired purpose.

GraphicsMagick_'s DPX file format support is very comprehensive. It goes
beyond the DPX format support in other applications by striving to
implement the complete DPX specification rather than just a few commonly
used sub-formats. The capabilities of GraphicsMagick_'s DPX support are as
follows:

DPX features
============

Basic
-----

  * Anything which can be read, can also be written.

  * All DPX header information (including the user specific area) are stored as
    image attributes and restored when the image is written.

  * Image source header information is updated appropriately.

Colorspaces
-----------

  * Linear RGB

  * Cineon Log RGB (default density range = 2.048)

  * Grayscale (Luma)

  * Rec. 601 and Rec. 709 YCbCr (4:4:4 and 4:2:2). Below-black and above-white
    values are clipped.

Storage
-------

  * Bits per sample of 1, 8, 10, 12, and 16.

  * Packed, or fill type A or B for 10/12 bits.

  * All RGB-oriented element types (R, G, B, A, RGB, RGBA, ABGR).

  * YCbCr

  * Planar (multi-element) storage fully supported.

  * Alpha may be stored in a separate element.

  * Big and little endian storage.

Yet to be supported
-------------------

  * Composite video.

  * Floating point formats (32 and 64 bits)

  * Depth channel (not supportable within GraphicsMagick_).

  * Studio (reduced range) YCbCr and RGB.

The software is written efficiently so the performance when reading and
writing files is limited by the performance of the file I/O subsystem.
The software is designed to avoid seeking while reading and writing so
that files may be read and written over pipes, or via a user provided
file descriptor.

Using GraphicsMagick
====================

Image Resize
------------

GraphicsMagick_ is easy to use. The following is an example of scaling a
*4K* 16 bit scan to a *2K* *Academy* 10 bit image using the convert_
command::

   gm convert 4k.dpx -resize 1828x1556 -depth 10 2k.dpx

The above example uses the default resizing filters which are optimized
for quality, but take longer than some other filters. The *box* resize
filter provides reasonably good scaling in a reasonable amount of time::

   gm convert 4k.dpx -filter box -resize 1828x1556 -depth 10 2k.dpx

The above example command takes about 4 seconds (on an Apple 2.5GHz G5
PowerMac or Intel 2.4GHz Xeon) to down-convert from a 131MB *5K*
(5232x4376) original 16-bit scan from a NorthLight scanner to a 11MB
*2K* 10-bit working image. Operations on more typical *2K* images
take about a quarter of a second.

Annotate Image
--------------

The following example shows how GraphicsMagick_'s resize capability may be
combined with its powerful drawing capability to take a full size source image
and produce a smaller (720x576) version which includes the image filename and
timecode at the top of the image, and a logo *bug* image in the bottom right
corner::

  gm convert infile.dpx -resize '720x576!' \
    -draw 'fill "white";text-undercolor "Blue";font "Helvetica";font-size 18;\
       text 10,20 "%f (%[DPX:tv.time.code])";image atop 500,400 0,0 "gm-125x80t.png"' \
    outfile.dpx

As may be seen, the argument to -draw can become extremely long, so to make
things easy, the drawing commands may be placed in a simple text file and
passed by reference to the draw comand:

First lets check what we edited into our drawing command file::

   % cat drawcmd.txt
   fill "white"
   text-undercolor "Blue"
   font "Helvetica"
   font-size 18
   text 10,20 "%f (%[DPX:tv.time.code])"
   image atop 500,400 "0,0 "gm-125x80t.png"

and now we can apply it by passing the filename prefixed with a '@' to the
-draw command::

   gm convert infile.dpx -resize '720x576!' -draw '@drawcmd.txt' outfile.dpx

The ``0,0`` in the image composition command argument says to use the image as
is. If the composited image should be automatically resized, then simply
replace the ``0,0`` with the desired size.

There are a number of powerful scripting environments for GraphicsMagick_. One
of these is RMagick (Ruby language interface to GraphicsMagick_). In Ruby, the
same effect may be obtained via a script that looks like::

   #! /usr/local/bin/ruby -w
   require 'RMagick'
   include Magick
   img = Image.read('infile.dpx')[0]
   frog = Image.read('gm-125x80t.png')[0]
   gc = Draw.new
   gc.fill('white')
   gc.text_undercolor("Blue")
   gc.font("Helvetica")
   gc.font_size(18)
   gc.text(10, 20, "%f (%[DPX:tv.time.code])")
   gc.composite(500, 400, 0, 0, frog, AtopCompositeOp)
   gc.draw(img)
   img.write('outfile.dpx')

In addition to Ruby, there are scripting interfaces for Perl, Python, Tcl, and
Ch (C-like scripting language).

Colorspace Transformation
-------------------------

To convert an RGB file to a 4:2:2 YCbCr file in Rec 709 space::

   gm convert 2k.dpx -depth 10 -colorspace Rec709YCbCr -sampling-factor 4:2:2 2k-ycbcr.dpx

Modifying An Image In-Place
---------------------------

Besides convert, which converts from one file to another, there is mogrify_
which transforms the file in place. A temporary file is used (if necessary) to
ensure that the existing image file is not damaged if something goes wrong
(e.g., not enough disk space). Note that unlike some applications supporting
DPX/Cineon, when a file is modifed *in-place* , it is completely re-written.
While GraphicsMagick_ makes every attempt to preserve header information, some
previously existing features of the file (such as the offset to the pixel data)
may change.

A typical mogrify command is

::

   gm mogrify -resize 1828x1556 -depth 10 file-0001.dpx file-0002.dpx

Multiple files may be specified on the command line so the same command may
process hundreds of files in one invocation.

Unix users can use the find and xargs programs to perform operations on any
number of files::

   find /assets/001 -name '*.dpx' -print | \
     xargs gm mogrify -resize 1828x1556 -depth 10

Xargs works by pasting as many file names as possible on the end of the command
provided to it.

The GNU version of xargs provides an added benefit. It is able to run several
commands in the background. This means that if your system has multiple CPUs,
it can take advantage of all the CPUs while still using one command::

   find /assets/001 -name '*.dpx' -print | \
     xargs --max-procs 3 --max-args 25 gm mogrify -resize 1828x1556 -depth 10

The mogrify command supports the -output-directory option to sent files to a
different directory than the input files. This allows processing a large number
of files without overwriting the input files::

   mkdir dest
   cd source
   gm mogrify -output-directory ../dest -resize 1828x1556 -depth 10 '*.dpx'

Note that the entire input file path specification is preserved when composing
the output path so that the input file path is simply appended to the output
directory path. Also, unless the -create-directories option is added, the user
is responsible for creating any necessary destination directories. As an
example of the path composition algorithm, if the input file name is specified
as source/file.dpx and the output directory is specified as dest, then the
output file path will be dest/source/file.dpx.

Here is an incantation which recursively processes all DPX files under source
and sends the result to a similar directory tree under dest.

::

   mkdir dest
   cd source
   find . name '*.dpx' -print | xargs gm mogrify -output-directory ../dest \
     -create-directories -resize 1828x1556 -depth 10

Creating A Contact Sheet
------------------------

GraphicsMagick_ may be used to create a contact sheet (grid of thumbnails with
name and size) by using the *VID* pseudoformat which accepts a wildcarded
argument of files (protected by quotes!) to read. The output files are buffered
while files are being read so there is a practical limit to the number of files
which may be processed at once. To output to a Postscript file::

   gm convert "vid:*.dpx" "contact-sheet.ps"

or to a PDF file::

    gm convert "vid:*.dpx" "contact-sheet.pdf"

or to a sequence of JPEG files ranging from contact-sheet-000.jpg to
contact-sheet-999.jpg::

   gm convert "vid:*.dpx" "contact-sheet-%03d.jpg"

or to a MIFF file which may be used to interactively browse the original files
using 'gm display'::

   gm convert "vid:*.dpx" "contact-sheet.miff"

Animating A Sequence
--------------------

GraphicsMagick_ may be used to animate an image sequence on an X11
display using the animate_ subcommand. Frames are buffered in memory
(pre-loaded into the X11 server) so the number of frames which may be
animated at once is limited. GraphicsMagick_ has been used to animate
1080P (1920x1080) images at 24 frames per second with at least 300 frames
in the sequence.More frames may be buffered on 64-bit systems. Many more
frames may be animated by preparing a reduced set of frames in advance.

To visualize an animation at 24 frames per second (delay (1/24)*100) use

::

   gm animate -delay 4.17 'Frame_*.dpx'

In order to obtain a preview of a larger sequence, and if the frames are
numbered, a broader span of time may be animated by selecting every 10^th frame
(terminating with zero) to animate at 2.4 frames per second::

   gm animate -delay 41.7 'Frame_*0.dpx'

Displaying One Image Frame
--------------------------

An image frame may be displayed on an X11 server using the display_
subcommand. By default the name of the image file is displayed in the
title bar. By specifying the format of the title, other useful
information such as the time code (see the DPX Attributes section for
more details) may be included in the window title::

   gm display -title '%f (%[DPX:tv.time.code])' foo.dpx

Viewing A Sequence
------------------

A sequence of images may be displayed on an X11 server using the display_
subcommand. Unlike 'gm animate' there are no arbitrary limits when
displaying a sequence this way. Unlike 'gm animate' the inter-frame delay
can not be set to less than a second (100 ticks is one second).

::

   gm display +progress -delay 100 'Frame_*.dpx'

Options And Attributes
======================

Command options
---------------

The following command options are particularly useful when dealing with
DPX files:

-colorspace {CineonLog|RGB|Gray|Rec601Luma|Rec709Luma|Rec601YCbCr|Rec709YCbCr}
    Specifies the colorspace to be used when saving the DPX file. CineonLog
    selects log encoding according to Kodak Cineon specifications. RGB selects
    linear RGB encoding. Gray selects linear gray encoding similar to RGB, but
    with a single channel. Rec601Luma requests that RGB is converted to a gray
    image using Rec601 Luma. Rec709Luma requests that RGB is converted to a
    gray image using Rec709Luma. Rec601YCbCr requests that the image is saved
    as YCbCr according to Rec601 (SDTV) specifications. Rec709CbCr requests
    that the image is saved as YCbCr according to Rec709 (HDTV) specifications.
-endian {lsb|msb}
    Specifies the endian order to use when writing the DPX file. GraphicsMagick_
    writes big-endian DPX files by default since they are the most portable.
    Other implementations may use the native order of the host CPU (e.g.
    little-endian when using an Intel 'x86 CPU).
-depth <value>
    Specifies the number of bits to preserve in a color sample. By default the
    output file is written with the same number of bits as the input file. For
    example, if the input file is 16 bits, it may be reduced to 10 bits via
    '-depth 10'.
-define dpx:bits-per-sample=<value>
    If the dpx:bits-per-sample key is defined, GraphicsMagick_ will write DPX
    images with the specified bits per sample, overriding any existing depth
    value. If this option is not specified, then the value is based on the
    existing image depth value from the original image file. The DPX standard
    supports bits per sample values of 1, 8, 10, 12, and 16. Many DPX readers
    demand a sample size of 10 bits with type A padding (see below).
-define dpx:colorspace={rgb|cineonlog}
    Use the dpx:colorspace option when reading a DPX file to specify the
    colorspace the DPX file uses. This overrides the colorspace type
    implied by the DPX header (if any). Currently files with the transfer
    characteristic Printing Density are assumed to be log encoded density
    while files marked as Linear are assumed to be linear. Hint: use
    ``-define dpx:colorspace=rgb`` in order to avoid the log to linear
    transformation for DPX files which use Printing Density.
-define dpx:packing-method={packed|a|b|lsbpad|msbpad}
    DPX samples may be output within 32-bit words. They may be tightly packed
    end-to-end within the words ("packed"), padded with null bits to the right
    of the sample ("a" or "lsbpad"), or padded with null bits to the left of
    the sample ("b" or "msbpad"). This option only has an effect for sample
    sizes of 10 or 12 bits. If samples are not packed, the DPX standard
    recommends type A padding. Many DPX readers demand a sample size of 10 bits
    with type A padding.
-define dpx:pixel-endian={lsb|msb}
    DPX pixels should use the endian order that the DPX header specifies.
    Sometimes there is a mis-match and the pixels use a different endian order
    than the file header specifies. For example, the file header may specify
    little endian, but the pixels are in big-endian order. To work around that
    use -define dpx-pixel-endian=msb when reading the file. Likewise, this
    option may be used to intentionally write the pixels using a different
    order than the header. Files obtained from the `Library Of Congress`_ use
    big-endian 10-bit packed pixels in a file marked as little-endian so this
    option must be used to read such files correctly.
-define dpx:swap-samples={true|false}
    GraphicsMagick_ strives to adhere to the DPX standard but certain aspects of
    the standard can be quite confusing. As a result, some 10-bit DPX files
    have Red and Blue interchanged, or Cb and Cr interchanged due to an
    different interpretation of the standard, or getting the wires crossed. The
    swap-samples option may be supplied when reading or writing in order to
    read or write using the necessary sample order.
-interlace plane
    By default, samples are stored contiguously in a single element when
    possible. Specifying '-interlace plane' causes each sample type (e.g.
    'red') to be stored in its own image element. Planar storage is fully
    supported for grayscale (with alpha) and RGB. For YCbCr, chroma must be
    4:2:2 subsampled in order to use planar storage. While planar storage
    offers a number of benefits, it seems that very few DPX-supporting
    applications support it.
-sampling-factor 4:2:2
    Select 4:2:2 subsampling when saving an image in YCbCr format. Subsampling
    is handled via a general-purpose image resize algorithm (lanczos) rather
    than a dedicated filter so subsampling is slow (but good).
-set reference-white <value>
    Set the 90% white card level (default 685) for Cineon Log.
-set reference-black <value>
    Set the 1% black card level (default 95) for Cineon Log.
-set display-gamma <value>
    Set the display gamma (default 1.7) for Cineon Log.
-set film-gamma <value>
    Set the film gamma (default 0.6) for Cineon Log.
-set soft-clip-offset <value>
    Set the soft clip offset (default 0) when converting to *computer* RGB from
    Cineon Log.

DPX Attributes
--------------

GraphicsMagick_ provides almost full access to DPX header attributes. DPX
header attributes are shown in the output of 'gm identify -verbose' and
may be set using the -define syntax (e.g. '-define
dpx:mp.frame.position=2000') on the command line in order to add a value,
or override an existing value. The attributes in the list below may be
viewed or updated. The names are similar to the attribute descriptions
from the DPX standard.

::

  dpx:file.copyright
  dpx:file.creation.datetime
  dpx:file.creator
  dpx:file.encryption.key
  dpx:file.filename
  dpx:file.project.name
  dpx:file.version
  dpx:image.orientation
  dpx:mp.count
  dpx:mp.film.manufacturer.id
  dpx:mp.film.type
  dpx:mp.format
  dpx:mp.frame.id
  dpx:mp.frame.position
  dpx:mp.frame.rate
  dpx:mp.held.count
  dpx:mp.perfs.offset
  dpx:mp.prefix
  dpx:mp.sequence.length
  dpx:mp.shutter.angle
  dpx:mp.slate.info
  dpx:source.aspect.ratio.horizontal
  dpx:source.aspect.ratio.vertical
  dpx:source.border.validity.bottom
  dpx:source.border.validity.left
  dpx:source.border.validity.right
  dpx:source.border.validity.top
  dpx:source.creation.datetime
  dpx:source.device.name
  dpx:source.device.serialnumber
  dpx:source.filename
  dpx:source.scanned.size.x
  dpx:source.scanned.size.y
  dpx:source.x-center
  dpx:source.x-offset
  dpx:source.x-original-size
  dpx:source.y-center
  dpx:source.y-offset
  dpx:source.y-original-size
  dpx:tv.black.gain
  dpx:tv.black.level
  dpx:tv.breakpoint
  dpx:tv.field.number
  dpx:tv.gama
  dpx:tv.horizontal.sampling.rate
  dpx:tv.integration.time
  dpx:tv.interlace
  dpx:tv.sync.time
  dpx:tv.temporal.sampling.rate
  dpx:tv.time.code
  dpx:tv.user.bits
  dpx:tv.video.signal
  dpx:tv.white.level
  dpx:user.data.id

Specific header values from a DPX file may be displayed quickly using a command
similar to::

   gm identify -format '%[DPX:tv.time.code]' foo.dpx

Use

::

   gm identify -format '%[dpx:*]' foo.dpx

to list all DPX header attributes.

-------------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick_ Group 2002 - 2008


