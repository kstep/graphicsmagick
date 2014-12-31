.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

==================
GraphicsMagick FAQ
==================

.. URL links
.. _animate: animate.html
.. _composite: composite.html
.. _convert: convert.html
.. _display: display.html
.. _import: import.html
.. _install: install.html
.. _montage: montage.html
.. _README: README.html

.. contents:: FAQ Contents

How does GraphicsMagick differ from ImageMagick?
------------------------------------------------

*GraphicsMagick* is originally based on (forked from) ImageMagick
5.5.2 in November 2002, from the version distributed by ImageMagick
Studio LLC, which is itself forked in August 1999 from ImageMagick
developed by E. I. du Pont de Nemours and Company starting in
1992. Other than utilities being executed as sub-commands of the 'gm'
command, the command-line syntax and programming APIs remain entirely
upward compatible with ImageMagick 5.5.2. A better question might be
"*How does ImageMagick differ from ImageMagick?*" since ImageMagick
continues to alter and evolve its interfaces so they are no longer
completely compatible with earlier versions. While GraphicsMagick also
adds new features, it does so in a way which assures that existing
features work as they did before. ImageMagick focuses on adding new
functionality and features and has dramatically mutated several times
since the fork.

GraphicsMagick maintains a stable release branch, maintains a detailed
ChangeLog, and maintains a stable source repository with complete version
history so that changes are controlled, and changes between releases are
accurately described. GraphicsMagick provides continued support for a
release branch. ImageMagick does not offer any of these things.

Since GraphicsMagick is more stable, more time has been spent optimizing
and debugging its code.

GraphicsMagick is much smaller than ImageMagick and has dramatically
fewer dependencies on external libraries. For example, on the FreeBSD
operating system, a fully-featured install of GraphicsMagick depends
on 36 libraries whereas ImageMagick requires 64.  GraphicsMagick's
installation footprint is 3-5X smaller than ImageMagick.

GraphicsMagick is usually faster than ImageMagick. The baseline execution
overhead for simple commands is much lower, and GraphicsMagick is also
more efficient at dealing with large images.

How often does GraphicsMagick pick up new code from ImageMagick?
----------------------------------------------------------------

GraphicsMagick never picks up new code from ImageMagick as distributed
by ImageMagick Studio LLC. Not long after the GraphicsMagick project
was started in November 2002, ImageMagick from ImageMagick Studio LLC
abandoned the MIT X11 style license it had been using since 1992, and
switched between several different licenses until it ended up with one
based on the Apache license, which is intended to penalize projects
which borrow some of its source code, or fork from it. Since that
time, GraphicsMagick has not incorporated any ImageMagick source code.

On November 27, 2003 ImageMagick Studio LLC applied to register
"ImageMagick" as its trademark, and it was awarded this registered
trademark (serial number 78333969) on August 30, 2005.  Those who
re-distribute modified versions of "ImageMagick" (e.g. patched or
improved) under license as "ImageMagick" now face the risk of
arbitrary trademark infringement claims by ImageMagick Studio LLC.

Authors of new features are encouraged to independently contribute
their work to the GraphicsMagick project so that it can be released
under GraphicsMagick's MIT X11 style license without additional
encumberment.  In order for a work to be accepted, it must have been
developed entirely outside the ImageMagick source base to avoid any
possibility of copyright taint.

Are there any plans to use OpenCL or CUDA to use a GPU?
-------------------------------------------------------

It is well known that some math-intensive algorithms run very quickly
on video-card (and stand-alone) GPUs.  Video card vendors encourage
you to buy an expensive video card with quite a lot of installed RAM
and modify applications to use the GPU via their proprietary or
limited-purpose APIs.  GPUs are quite effective at producing images
for real-time display, such as for video games and virtual reality.
GraphicsMagick has been significantly updated to use multiple CPU
cores to speed up the image processing, and work continues to thread
the few remaining algorithms, or remove inefficiencies in algorithms
which don't see as much speed-up as they should.  Linear speedup as
cores are added is typical for CPU-bound algorithms on well-designed
CPUs.  Regardless, `Amdahl's law
<http://en.wikipedia.org/wiki/Amdal%27s_law>`_ is a significant factor
in GraphicsMagick, with non-parallelizable code paths often dominating
the time.

It is my belief that stand-alone GPUs are a poor design (expensive,
inefficient, failure-prone, bandwidth bottlenecked, lack
functionality, are insecure, and are not supported in servers) and
that multi-core CPUs will ultimately prevail.  Functionality which
currently works best in a GPU will simply be integrated into
tomorrow's multi-core CPUs and C compilers will naturally support that
functionality.  Once GPU capabilities are integrated into CPUs, there
will be no more need to develop specialized code for a GPU.  Today 16
core CPUs are readily available for purchase in systems at reasonable
prices, and this trend is certain to continue.

Intel's `Xeon Phi <http://en.wikipedia.org/wiki/Xeon_Phi>`_ offers a
61 core 'x86 CPU in a GPU-like plugin form-factor which provides over
a TeraFLOP of performance.  Being based on power-hungry plug-in cards,
this solution suffers from many of the issues associated with GPUs.
However, since it supports OpenMP, it may be a suitable target for
executing some GraphicsMagick algorithms. Effective use of Xeon Phi
currently requires use of Intel's development tools so from that
standpoint it is not much more open than GPUs.

Future multi-core CPUs will use a similar amount of power to today's
CPUs, will idle at very low power consumption levels, and will fit
into a very small chassis.  Due to this trend, there is no value
obtained by expending energy toward developing specialized code for
today's GPUs.

What is the meaning of "magick"?
--------------------------------

According to the infamous British accultist `Aleister Crowley
<http://en.wikipedia.org/wiki/Aleister_Crowley>`_, the definition of
`magick <http://en.wikipedia.org/wiki/Magick_(Aleister_Crowley)>`_ is
"the science and art of causing change to occur in conformity with the
will".

How can I process many files at once?
-------------------------------------

Use 'gm mogrify'.  The 'mogrify' subcommand is designed to operate on
any number of files in one command.  Normally 'mogrify' overwrites the
input files but the `-output-directory` option (which must appear
before any input file names!) allows sending the modified files to a
different directory (which could be in a subdirectory).  For example::

    gm mogrify -output-directory .thumbs -resize 320x200 *.jpg

If you encounter command line length limitations then you can have
GraphicsMagick expand the file list by quoting the wildcard argument
to prevent it from being expanded by your command shell::

    gm mogrify -output-directory .thumbs -resize 320x200 "*.jpg"

and you can also retrieve a list of files to process from a text file
(e.g. named 'files.txt') like::

    gm mogrify -output-directory .thumbs -resize 320x200 @files.txt

where files.txt has one line per file name.  If the input file paths
contain relative sub-directory paths (e.g. "foo/file1", "bar/file2"),
you can instruct GraphicsMagick to create a similar subdirectory
structure under the output directory by adding the
`-create-directories` option::

    gm mogrify -output-directory .thumbs -create-directories -resize 320x200 @files.txt

Note that the algorithm used to generate output file names is quite
simple.  If -output-directory is "/foo" and the file path is
"bar/none.jpg" then the final path will be "foo/bar/none.jpg".  Based
on this it should be clear that when `-output-directory` is used, file
paths should be relative paths rather than absolute paths or else the
concatenation won't work.


I received the following message, "?????? delegation failed ...". What does it mean?
------------------------------------------------------------------------------------

*GraphicsMagick* uses several freely available packages to perform the
translation of certain image formats (*PostScript*, *MPEG*, etc.). Make
sure these packages are available as described in the README_ file. Also
verify that you have plenty of temporary disk space. If not, set the
MAGICK_TMPDIR (or TMPDIR) environment variable to an area where
sufficient space is available. Finally, for *PostScript*, verify that
Ghostscript supports the *pnmraw* or *ppmraw* device (``gs -h``) and that
the document contains valid *PostScript* statements (``gs image.ps``).

How do I set the transparency index in a GIF image so it displays properly within Mozilla?
------------------------------------------------------------------------------------------

Display your GIF image with display_. Choose *Matte* from the *Image
Edit* command menu and identify a pixel that has the *transparency*
color. Press *Method* and select a matte edit method from a pop-up menu.
Choose from these methods:

* point
* replace
* floodfill

The *point* method changes the matte value of any pixel selected with the
pointer until the button is is released. The *replace* method changes the
matte value of any pixel that matches the color of the pixel you select
with a button press. *Floodfill* changes the matte value of any pixel
that matches the color of the pixel you select with a button press and is
a neighbor.

Select your transparent pixel with the pointer and press a button. The
image is redisplayed with any transparent pixels recolored to the
background color. You can select other pixels or areas to force to
transparent. When you are satisfied, press Return.

Finally, choose *Save* from the command menu and write your GIF image to
a file. **Note that setting transparency works best on a TrueColor or
DirectColor visual**. If your server only exports colormapped visuals you
will need to use a *Standard Colormap* to set transparency. ::

    xstdcmap -best
    gm display -map list image.gif
    [ choose Matte Edit ]
    [ select your transparent pixel then press Return ]
    [ choose Save '

If you do not have the *xstdcmap(1)* program, try ::

    gm display -visual TrueColor image.gif

How can I stop the filenames from changing in the title bar of the animate(1) image window?
-------------------------------------------------------------------------------------------

Animate_ updates the image file name in the title bar of the image window
as each image is displayed from the image sequence. To display just a
single name that will not change, use *-title*::

    gm animate -title "My Image Sequence" images.

The image grabbed by import(1) does not look like the image on my X server. What's wrong?
-----------------------------------------------------------------------------------------

Use the *-descend* option::

    gm import -descend image.miff

or set this X resource::

    gm import.descend: True

By default, import_ quickly grabs the image from the X server. However,
it may not always have the correct colors in some areas. This can happen
when a subwindow has a different colormap than its parent. With
*-descend*, import_ descends the window hierarchy. Descending involves
grabbing the image and colormap of each window or subwindow associated
with the window you select and compositing it on a blank canvas. This can
be significantly slower than just grabbing the top-level window but
ensures the correct image.

How do I animate a digital YUV image sequence?
----------------------------------------------

Suppose your sequence is 72 352x240 frames titled frame0.Y, frame0.U,
frame0.V, frame1.Y, frame1.U, etc. Use this command::

    gm animate -geometry 352x240 -scene 0-71 yuv3:frame%d

How do I change the default *PostScript* page size?
---------------------------------------------------

The default dimensions of a *PostScript* page is 612x792. If you prefer
another default, change the page geometries (PSPageGeometry) in
`magick/image.h` and recompile.

I get a memory allocation error. What can I do?
-----------------------------------------------

Memory allocation is a complex topic in GraphicsMagick and image
processing requires a lot of memory. GraphicsMagick tries to take best
advantage of the resources available by optimizing its use of *virtual*
memory. Virtual memory is normally the amount of RAM (Random Access
Memory) available to the process plus the amount of free space in the
system paging area (known as "swap" under Unix, or a "page file" under
Windows). In addition to the RAM and the paging area, GraphicsMagick is
able to expand the amount of virtual memory available by using
memory-mapped files which cause the file to be treated as extra memory. A
piece of hardware known as the Memory Management Unit (MMU) performs the
magic which allows the system paging area and memory-mapped files to be
treated as more memory by your system's CPU.

Usually when a memory allocation error occurs, it is because the system's
paging area is full, or the programs memory (heap) is fragmented badly
enough that there is no large-enough block of memory available to satisfy
the request. In some cases the operating system may artificially limit
the memory available to the program. Failures of small memory allocations
are generally considered fatal by GraphicsMagick. If it is impossible to
allocate even a little bit of memory, then there is no point in
continuing. When large memory allocations associated with allocating
memory for image pixels fail, GraphicsMagick uses that as a key to know
that it should start using memory-mapped temporary files to increase
virtual memory. Under Unix and other POSIX-compliant systems, these
memory-mapped temporary files are created in either the system default
location (e.g. `/var/tmp`) or the directory set by the TMPDIR environment
variable. Under Windows, temporary files are created in the directory set
by the TEMP or TMP environment variable. Use the MAGICK_TMPDIR
environment variable to specify a directory for GraphicsMagick to write
its temporary files.

If the temporary file directory is too small, or is itself mapped to the
system's paging area (no win!), then GraphicsMagick will fail to allocate
more virtual memory via the temporary file and will return an error.

The single biggest factor in how much memory is required by
GraphicsMagick is the QuantumDepth setting when it was compiled. The
amount of memory (in bytes) required to store a single image in memory is
may be calculated via the equation (QuantumDepth*Rows*Columns*5)/8. As a
means of example, the following table shows the amount of memory consumed
by a single 1024x768 image using the supported QuantumDepth settings:

  ============  ==============
  QuantumDepth  Virtual Memory
  ============  ==============
        8            3MB
       16            8MB
       32           15MB
  ============  ==============

Performing an image processing operation may require that several images
be in memory at one time. In the case of animations, hundreds of images
may be in memory at one time.

*GraphicsMagick* is designed to be general purpose. It can display many
image storage formats (*Monochrome*, *PseudoColor*, or *TrueColor*) on
many different types of X visuals (*StaticGray, StaticColor, PseudoColor,
GrayScale, DirectColor,* or *TrueColor*). To support all these
combinations of image storage formats and X visuals, extra memory is
required. Additionally, animate_ and montage_ store an entire image
sequence in memory.

It is recommended that systems used to run *GraphicsMagick* have at least
96MB of RAM and 140MB free in their paging area. Systems used to process
large images (do the math!) may require considerably more memory to
operate efficiently. GraphicsMagick is proven to be far more efficient at
processing huge images when it is compiled as a 64-bit application on a
64-bit capable CPU. RAM is typically 1000 times faster than disk, so it
is important to keep as much data in RAM as possible (buy lots of RAM).
The temporary file area should have plenty of free space. The size of the
temporary file area is usually the deciding factor as to whether
GraphicsMagick is able to complete an operation. With sufficient free
disk space, GraphicsMagick is primarily limited by your CPU, your
operating system's ability to address memory, and your degree of
patience. GraphicsMagick has been used to process RGB images 64K by 64K
pixels in size!

How do I concatenate three images left-to-right with no borders, frames, or text?
---------------------------------------------------------------------------------

Assume your three images are called image1.ppm, image2.ppm, and
image3.ppm. Type ::

    gm montage -mode concatenate -tile 3x1 image1.ppm image2.ppm \
      image3.ppm concatenated.miff

To concatenate the images top-to-bottom, use -tile 1x3.

For more control over the placement of an image, use composite_. First
create a matte image and position your images onto the matte. For
example, ::

    gm convert -size 350x500 xc:black composite.miff
    gm composite -geometry +0+0 composite.miff image1.gif composite.miff
    gm composite -geometry "+1"00+0 composite.miff image2.gif composite.miff
    gm composite -geometry +0+300 composite.miff image3.gif composite.miff
    gm composite -geometry +0+375 composite.miff image4.gif composite.miff


How do I create a GIF animation sequence to display within Firefox?
-------------------------------------------------------------------

Use convert_ with the *-delay* and *-page* options. The *-delay* option
is used to specify the delay in *1/100ths of a second* between the
display of each frame of the animation. For example, ::

    gm convert -delay 20 frame*.gif animation.gif

You can also declare specific delays for each frame of the image
sequence. For example, if the delay was 20, 10, and 5, use ::

    gm convert -delay 20 frame1.gif -delay 10 frame2.gif \
      -delay 5 frame3.gif animation.gif

Use *-page* to specify the *left* and *top* locations of the image frame ::

    gm convert frame1.gif -page +50"+1"00 frame2.gif -page +0"+1"00 \
      frame3.gif animation.gif

Finally, if you want the image to loop within *FireFox*, use *-loop* ::

    gm convert -loop 50 frame*.gif animation.gif

Note, that all the images are composited into a single multi-image GIF
animation. If you want a single image produced for each frame, use
*+adjoin* ::

    gm convert +adjoin images.* frames%d.gif

When I display a *PostScript* image, white borders are trimmed.
---------------------------------------------------------------

*GraphicsMagick* automatically trims any *PostScript* image as defined by
the bounding box. To preempt this behavior, remove the bounding box
statement from the *Postscript* or explicitly set the page size. For
example, ::

    gm display -page letter image.ps

What are visual image directories? How do I use them?
-----------------------------------------------------

A visual image directory (VID) is an image that contains thumbnails of
one or more images in a file directory. Rather than displaying each
individual image at its full resolution, you can browse the visual image
directory and choose an image to display. You can create a VID with
either of these commands::

    gm montage *.jpg directory.vid
    gm convert 'vid:*.jpg' directory.vid

Of course you can substitute any filenames you desire. Montage_ has many
relevant command line options. You can exercise more control over the
appearance of the VID than with convert_.

Next display the directory::

    gm display directory.vid

Finally browse and select an image to display. Move the pointer to the
image and press button 3.

You can create the VID directory with this command::

    gm display 'vid:*.jpg'

You can also select *Visual Image...* from the *File* menu of the command
widget.

*Note, that creating a VID is time consuming*. Creating them on-the-fly
within display_ may be less convenient than using montage_ or convert_ .
Also, if you create them with montage_. or convert_, you can reuse them
as often as necessary.

Note that a visual image directory is useful for looking at individual
frames of an image sequence::

    gm display vid:movie.mpg

How can I include the window frame when importing a window?
-----------------------------------------------------------

I use the window ID reported by *xwininfo(1)* with import and it does not
include the window manager frame as expected. How can I save the window
with its frame?

By default, *xwininfo(1)* returns the ID of the window you click on. Use
the *-frame* option to get the reparented window ID::

    xwininfo -frame

You can then use the returned window ID with import_::

    gm import -frame -window ID window.miff

I displayed an image and it appears as one solid color. What did I do wrong?
----------------------------------------------------------------------------

A blank image generally means that the image is either corrupt or it has
a matte channel and the matte values are all zero. *GraphicsMagick*
treats a matte value of zero as completely transparent. To determine if
this is the problem, try ::

    gm display +matte image.miff

I received the following message, "???? library is not available...".
---------------------------------------------------------------------

*GraphicsMagick* requires source libraries not included with the
distribution to view or convert certain image formats such as JPEG or
TIFF. The above message means you did not compile the required library
and link with the *GraphicsMagick* utilities. See README_ for the
location of these libraries and compiling instructions.

I want to inspect the values of the matte channel within my image.
------------------------------------------------------------------

View the matte image as a gray scale image. Suppose you have a TIFF image
that has a matte channel and is 640 pixels in width and 480 in height.
Type::

    gm convert image.tiff image.matte
    gm display -size 640x480 gray:image.matte

How can I add one of those cool bevels to my image that I see used on the Web?
------------------------------------------------------------------------------

There are four types of ornamental borders you can add to your image
with GraphicsMagick. Each is listed below with the procedure to use
them with your image. 

* Surround the image with a border of color

  Use -border followed by the width and height of the border. Set the
  color of the border with -bordercolor. For example, to surround your
  image with a red border that is 25 pixels wide on each side, use ::

    gm convert -bordercolor red -border 25x25 image.jpg image.gif

* Lighten or darken image edges to create a 3-D effect

  Use -raise followed by the width of the image edge. For example, to
  create a raised edge effect of 25 pixels, use ::

    gm convert -raise 25 image.jpg image.gif

* Surround the image with an ornamental frame

  Use -frame followed by the width and height of the frame. Set the
  color of the border with -mattecolor. For example, to surround your
  image with a gray frame that is 25 pixels wide on each side, use ::

    gm convert -mattecolor gray -frame 25x25 image.jpg image.gif

* Surround the image with a raised or sunken bevel

  Use -frame followed by the width and height of the bevel. Set the
  color of the border with -mattecolor. This is just like the
  description above except you specify a bevel width that matches the
  frame width. For example, to surround your image with a gray bevel
  that is 25 pixels wide on each side, use ::

    gm convert -mattecolor gray -frame 25x25+0+25 image.jpg image.gif
    gm convert -mattecolor gray -frame 25x25+25+0 image.jpg image.gif

I try to launch display from my window manager and it fails. What's up?
-----------------------------------------------------------------------

Display_ determines if it is executing interactively and behaves
differently depending on the result. To convince display <display.html>
you are running in an interactive environment when launching from a
window manager, use either of ::

  display logo:Untitled
  display < /dev/console

Note that this issue no longer exists as of GraphicsMagick 1.2.

How can I make Postscript text look good?
-----------------------------------------

Simple. Increase the dots-per-inch when converting and sub-sample::

   gm convert -density 288 -geometry 25% image.ps image.gif

Change the density to 144 and geometry to 50% if the above command fails
due to insufficient memory. Alternatively, see the Ghostscript
documentation about using high-quality fonts.

The -density option increases the number of pixels (or dots) generated by
Ghostscript when processing the input postscript file. However as all
other images formats are generally displayed on screens which are
typically about 72 to 100 dots per inch, the output image will be larger.

The *-geometry* option reduces the large image output of ghostscript
image back to a normal 72 dpi resolution (25% of 288 dpi gives 72 dpi)
but in the process anti-aliases (or smooths) the fonts and lines of the
image so as to remove the jaggies you would otherwise get from a normal
postscript to image conversion.

How can I annotate an image with text that is 2 to 3 inches tall?
-----------------------------------------------------------------

If you do not access to a particular named font that is large, try
scalable fonts. First see if you have any scalable fonts. Type ::

    xlsfonts -fn '*-0-0-0-0-*'

Or if you are using display_, use the font pattern above within the Font
Browser (see Image Edit->Annotate). Next substitute the appropriate
resolution. Keep in mind that a scalable font must be fully qualified to
work. That is, all 14 fields must be specified. Here is one example where
we annotate an image with large *Helvetica* text::

    gm convert -font '-*-helvetica-*-*-*--300-300-*-*-*-*-iso8859-1' \
      -fill green -draw 'text 50,300 Magick' image.gif annotated.gif

If you have the FreeType support built into GraphicsMagick, just
increase your pointsize and/or density::

    gm convert -font Helvetica -pointsize 100 -density 300 ...

How can I convert my GIF animation sequence to individual image files?
----------------------------------------------------------------------

Use the scene embedded file format with convert_::

    gm convert animation.gif +adjoin frame%02d.gif 

The resulting image files are titled frame01.gif, frame02.gif,
frame03.gif, etc.

How can I remove the background that prints around my image when I display it with Firefox?
-------------------------------------------------------------------------------------------

Use the +page option of the convert_ command::

    gm convert +page alpha.gif beta.gif

GIF allows for a page offset relative to some background. The page
offset information may have been in your GIF image already or it
could have been introduced by GraphicsMagick. Either way, +page
removes the unwanted page offset and FireFox should behave as
expected.

How do I create a GIF or PNG image with Web safe colors?
--------------------------------------------------------

Web safe colors are not normally needed any more since almost all
computers now have true color displays. However, this FAQ may still be
useful since it demonstrates how a colormap from an image may be
applied to another image..

Use the -map option of the convert_ command::

    gm convert -map netscape: alpha.gif beta.gif 

Netscape predefines 216 colors for colormapped displays. Use the above
command to ensure only these predefined colors are used. Otherwise
Netscape dithers your image with varying degrees of image fidelity.

How can I add a matte layer to my image?
----------------------------------------

One way is to use a bitmap as your transparency mask First, use the
-matte option to add an all-opaque opacity channel, then use the
composite utility to copy the graylevel samples from the mask file
into your new opacity channel::

    gm convert image.gif -matte temp.miff
    gm composite -compose CopyOpacity mask.xbm temp.miff transparent.gif

Note, GIF is limited to one transparent color. If your mask has
variable opacity, use a format like MIFF, TIFF, or PNG as your output
image format.

How can I draw with text using 'convert' under Windows NT?
----------------------------------------------------------

The problem is that NT interprets the command line differently than
Unix does, causing the documented command to fail. The following
command has been reported to work correctly (all on one line)::

    gm convert -font Arial -fill blue -draw "text 10,10 'your text here'" \
       d:\test.tif png:d:\test.png 

and here is another example which is reported to work (which relies
on Ghostscript's 'gs' program to installed)::

    gm convert.exe -pointsize 18 -draw "text 0,0 "This is my text!"" \
      C:\blank.gif c:\text.gif

Why are my JPEG files larger than expected?
-------------------------------------------

Your JPEG files may contain embedded "profiles" such as Exif or IPTC,
or they may contain uncompressed thumbnails. You can use the `+profile
"*"` commandline option or an equivalent API method to remove them.

How do I extract a single image from a multi-image file?
--------------------------------------------------------

Use a square-bracket syntax to indicate which frame or frames you
want. For example, ::

    gm convert "Image.gif[0]" first.gif

Will extract the first image (scene 0) from a GIF animation. Be sure
to surround the file specification with quotation marks, to prevent
the shell from interpreting the square brackets.

How can I extract and combine CMYK channels in a CMYK image?
------------------------------------------------------------

GraphicsMagick 1.2.2 and later support combining multiple channels in of
any image file format in order to create a CMYK image file.  For example,
starting with a CMYK JPEG file, we can create a set of separate files
with one file per channel::

  gm convert cmyk.jpg -channel cyan cyan.tiff
  gm convert cmyk.jpg -channel magenta magenta.tiff
  gm convert cmyk.jpg -channel yellow yellow.tiff
  gm convert cmyk.jpg -channel black black.tiff

and then we can join them back together::

  gm composite -compose CopyMagenta magenta.tiff cyan.tiff result.tiff
  gm composite -compose CopyYellow yellow.tiff result.tiff result.tiff
  gm composite -compose CopyBlack black.tiff result.tiff result.tiff

Perhaps it is possible to accomplish this in one composite command. We
will leave that as an exercise for later. Note that it is possible to
extract just one channel, manipulate it, and then insert it back into the
original image file.

There is also built-in support for a *Partition* interlace format which
can split to a set of files, and join a set of files, but the only
supported format is a raw format which is not easy to deal with.

How can I create a solid or patterned canvas image?
---------------------------------------------------

Canvas images may be created using the 'XC:' or 'TILE:' pseudo-image
formats. XC produces solid color images based on an color you specify
while TILE produces a tiled image based on an an image you specify. Both
of these require that the desired size be specified, and the desired
image type may be specified as well.

To create a solid red canvas image::

  gm convert -size 640x480 xc:red canvas.tiff

or using hex syntax to specify the color::

  gm convert -size 640x480 "xc:#f00" canvas.tiff

To create a solid red canvas image using truecolor pixels::

  gm convert -size 640x480 -type TrueColor xc:red canvas.tiff

To create a patterned canvas image using a built-in crosshatch pattern::

  gm convert -size 640x480 tile:image:CROSSHATCH45 canvas.tiff

To create a patterned canvas image using a user-supplied image::

  gm convert -size 640x480 tile:myimage.tiff canvas.tiff

What does `identify` report?
----------------------------

GraphicsMagick is sophisticated image handling software supporting
perhaps a hundred formats, and hundreds of subformats, yet it needs to
be able to apply image processing algorithms in a consistent way, and
save to many output formats.  Some image types are radically different
from others.  Due to this, GraphicsMagick normalizes images it reads
into only nine internal in-memory formats (see the description of
-type) which are a specialization of just two internal formats.  The
`identify` command reports the characteristics of one of those
internal formats rather than the genuine properties of the input image
file.  In fact, if the properties of all the input files were to be
reported, it would require a book to describe all of the properties.
For example, the WMF vector-drawing format is much different from the
PNG image file format, yet GraphicsMagick can read both.

Only limited original properties are preserved, and it depends on how
GraphicsMagick was built.  For example, a "Q8" build can only report
up to a depth of 8 bits, but a "Q32" build can report a depth of up to
32 bits.  The original image depth is reported if it is at least 8
bits, and equal to or less than the build depth.

By default `identify` reports only the most basic properties::

  % gm identify seaworld.jpg
  seaworld.jpg JPEG 1530x1020+0+0 DirectClass 8-bit 204.0K 0.000u 0:01

but with -verbose it can report on a large number of properties.

--------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2015
