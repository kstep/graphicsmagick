.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=====================
Related Links
=====================

.. contents::

GraphicsMagick Topics
=====================

`Free Software Foundation <http://directory.fsf.org/project/GraphicsMagick/>`_ GraphicsMagick Entry.

`FreeBSD port <http://www.freebsd.org/cgi/cvsweb.cgi/ports/graphics/GraphicsMagick13/>`_ for GraphicsMagick.

`NetBSD/pkgsrc port <http://pkgsrc.se/graphics/GraphicsMagick/>`_ for GraphicsMagick.

`Free (code) <http://freecode.com/projects/graphicsmagick>`_ GraphicsMagick Entry. (Frozen Site).

`Gmane <http://dir.gmane.org/gmane.comp.video.graphicsmagick.announce>`_ GraphicsMagick Announce List.

`Gentoo Sunrise <http://bugs.gentoo.org/show_bug.cgi?id=190372>`_ Bug Tracker Entry.

`Red Hat Linux <https://bugzilla.redhat.com/buglist.cgi?component=GraphicsMagick&product=Fedora>`_ GraphicsMagick related bugs.

Debian `GraphicsMagick packages <http://packages.debian.org/search?keywords=GraphicsMagick&amp;searchon=names&amp;suite=all&amp;section=main>`_.

`WikiPedia <http://en.wikipedia.org/wiki/GraphicsMagick>`_ GraphicsMagick Entry.

`IceWalkers <http://www.icewalkers.com/Linux/Software/519400/GraphicsMagick.html>`_ GraphicsMagick Entry.

`Black Duck | Open HUB <http://www.openhub.net/p/GraphicsMagick>`_ GraphicsMagick Entry.

`SourceForge <http://sourceforge.net/projects/graphicsmagick/>`_ GraphicsMagick Entry.

`GraphicsMagick on MinGW <http://ascendwiki.cheme.cmu.edu/Building_GraphicsMagick_on_MinGW>`_.

`GraphicsMagick questions on StackOverflow <http://stackoverflow.com/questions/tagged/graphicsmagick?>`_.


GraphicsMagick Vulnerabilities
==============================

Search the CVE site at MITRE for `GraphicsMagick`__ vulnerabilities.

.. _CVE_GraphicsMagick : http://cve.mitre.org/cgi-bin/cvekey.cgi?keyword=GraphicsMagick

__ CVE_GraphicsMagick_

Search Secunia for `GraphicsMagick`__ vulnerabilities.

.. _Secunia_GraphicsMagick : http://secunia.com/advisories/search/?search=GraphicsMagick

__ Secunia_GraphicsMagick_

Seach SecurityFocus Bugtraq for `GraphicsMagick`__ vulnerabilities (select Vendor 'GraphicsMagick').

__ Bugtraq_GraphicsMagick_

.. _Bugtraq_GraphicsMagick : http://www.securityfocus.com/bid


Image Processing Topics
============================

`HyperMedia Image Processing Reference <http://homepages.inf.ed.ac.uk/rbf/HIPR2/>`_,
A guide to image processing algorithms, many of which are supported by GraphicsMagick.


Color Technology Related Topics
======================================

Charles Poynton's `Color technology FAQs <http://www.poynton.com/Poynton-color.html>`_,
very useful documentation on color technologies.

Richard Kirk's excellent `Standard Colour Spaces (FL-TL-TN-0282-StdColourSpaces.pdf) <http://www.filmlight.ltd.uk/services_support/library/truelight>`_ at `FilmLight <http://www.filmlight.ltd.uk/>`_.

`Computer Graphics Colorspace <http://cs.fit.edu/~wds/classes/cse5255/cse5255/davis/>`_,
tutorials (with figures) describing the operation of common colorspaces.

`International Color Consortium <http://www.color.org/>`_,
the organization responsible for color profile standards.

`Argyll Color Management System <http://www.argyllcms.com/>`_,
a free experimental color management system.

`Littlecms <http://www.littlecms.com/>`_,
a free commercial-grade colormanagement engine in 100K (and used by GraphicsMagick).

`SCARSE Profile Library <http://www.scarse.org/goodies/profiles/>`_,
a useful collection of CMS profiles built using the SCARSE ICC profile builder.

`Bruce Lindbloom's Web Site <http://www.brucelindbloom.com/>`_,
offering interesting information related to color science and working spaces.

`sRGB <http://www.w3.org/Graphics/Color/sRGB.html>`_,
A standard default color space for the Internet.

`High Dynamic Range Image Encodings <http://www.anyhere.com/gward/hdrenc/hdr_encodings.html>`_,
an analysis by Greg Ward of various HDR encodings.

Gamma Related Topics
======================================

While most computer images are encoded with a gamma of 2.2 (really 2.2
to 2.6), GraphicsMagick does not attempt to convert images to
linear-light before applying image processing operations since it is
not possible to know for sure to know how to do so.  Some algorithms
such as resize, blur, and composition, will produce more accurate
results when performed on images encoded in a linear-light scaled
space.

For a typical sRGB image encoded in a gamma-corrected space with gamma
2.2, the option ``-gamma 0.45`` (1/2.2 = 0.45) will remove that
encoding for subsequent algorithms so that they are done in
linear-light space.  When processing is completed, then ``-gamma 2.2``
will restore gamma-correction for viewing.  It is recommended to use a
Q16 or Q32 build of GraphicsMagick when doing this since linear-light
space encoding is not efficient and will lose accuracy if stored with
less than 14 bits per sample.

The following documents and pages provide interesting information on
gamma-related topics:

`Charles Poynton's Gamma FAQ <http://www.poynton.com/GammaFAQ.html>`_,
provides an excellent description of what gamma is, why it is good,
and when you don't want it.

`Interpolation and Gamma Correction
<http://www.all-in-one.ee/~dersch/gamma/gamma.html>`_, provides a
discussion of how affine transforms on gamma-corrected images can
cause distortion.

`Gamma error in picture scaling
<http://www.4p8.com/eric.brasseur/gamma.html>`_, provides a discussion
of how image resize on gamma-corrected images can cause distortion.


TIFF Related Topics
============================

`LibTIFF <http://www.remotesensing.org/libtiff/>`_,
Libtiff library and TIFF format mailing list.

AWare Systems `TIFF <http://www.awaresystems.be/imaging/tiff.html>`_ site.
Detailed TIFF-related information which goes beyond the TIFF specification,
list archives for the libtiff mailing list, and information regarding the emerging Big TIFF format.

`Digital Negative (DNG) <http://www.adobe.com/products/dng/index.html>`_,
Adobe TIFF specification for digital camera raw images.

`LogLuv Encoding for TIFF Images <http://www.anyhere.com/gward/pixformat/tiffluv.html>`_,
A way to store HDR images using TIFF.

JPEG Related Topics
==========================

`Independent JPEG Group <http://www.ijg.org/>`_ (home of IJG JPEG library).

`Guido Vollbeding's JPEG site <http://jpegclub.org/>`_, including various patches to IJG JPEG release 6b.

DICOM Related Topics
============================

`David Clunie's Medical Image Format Site <http://www.dclunie.com/>`_,
information about medical images.

Metadata (Associated Data) Topics
=========================================

`Extensible Metadata Platform (XMP) <http://www.adobe.com/products/xmp/index.html>`_,
Adobe's XML-based embedded metadata format.

`EXIF <http://www.exif.org/>`_,
Format for metadata in images, particularly JPEG files from digital cameras.

High Dynamic Range Topics
==========================

`High Dynamic Range Image Encodings <http://www.anyhere.com/gward/hdrenc/hdr_encodings.html>`_,
An analsys by Greg Ward of various HDR encodings.

`LogLuv Encoding for TIFF Images <http://www.anyhere.com/gward/pixformat/tiffluv.html>`_,
A way to store HDR images using TIFF.

`OpenEXR <http://www.openexr.com/>`_,
library and sample tools for dealing with high dynamic-range (HDR) images.

Motion Picture Links
=========================

`Light Illusion <http://www.lightillusion.com/home.htm>`_,
white papers by Steve Shaw regarding HD video cameras, log color spaces, and digital intermediate.

`Digital Intermediates <http://www.digitalintermediates.org/>`_,
site by Jack James dedicated to the digital intermediate industry.

`Digital Cinema Initiatives <http://www.dcimovies.com/>`_,
DCI offers the first complete specification for digital cinema delivery.

`Ingex <http://ingex.sourceforge.net/index.html>`_ Tapeless video &
audio capture, transcoding and network file serving.  From the BBC.

Video Topics
=============

`Video Codecs and Pixel Formats <http://www.fourcc.org/>`_, offers a summary of YUV encoding formats.

Other Software Packages
========================

`DMMD Visere <http://www.dmmd.net/>`_,
truly outstanding image viewing/browsing software for Microsoft Windows.
Visere is based on GraphicsMagick.

`CinePaint <http://www.cinepaint.org/>`_,
GIMP-derived software targeted for the motion picture industry, and for other
applications which require high color resolution.

`eLynx lab <http://elynxlab.free.fr/en/index.html>`_ High resolution image processing tool.

The `GIMP <http://www.gimp.org/>`_, interactive image editing software (like Photoshop).

`ImageMagick <http://www.imagemagick.org/>`_, the ancestor of GraphicsMagick.

`VIPS <http://www.vips.ecs.soton.ac.uk/index.php?title=VIPS>`_, an image processing system also useful with
large images, and which comes with an unusual GUI.

`FreeImage <http://freeimage.sourceforge.net/index.html>`_,
a free image processing library.

`ImageJ <http://rsbweb.nih.gov/ij/>`_ Image Processing and Analysis in Java.

`VIGRA <http://hci.iwr.uni-heidelberg.de/vigra/>`_,
an image processing library in C++ based on generic algorithms.

`Pstoedit <http://www.pstoedit.net/>`_,
A Postscript to editable vector translation utility.

`UFRaw <http://ufraw.sourceforge.net/>`_,
a utility to read and manipulate raw images from digital cameras.

`LPROF <http://lprof.sourceforge.net/index.html>`_,
an open source ICC profiler with graphical user interface.

`Gallery <http://gallery.menalto.com/>`_,
a facinating web-based photo album organizer.  Works with GraphicsMagick!.

`DJV Imaging <http://djv.sourceforge.net/>`_, professional movie
playback and image processing software for the film and computer
animation industries.

`OpenImageIO <https://sites.google.com/site/openimageio/>`_ library
for reading and writing images, and a bunch of related classes,
utilities, and applications.

Stock Photos
=============

`MorgueFile <http://www.morguefile.com/>`_, Free high-resolution stock photo images.

--------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2015
