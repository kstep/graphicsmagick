.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=================
ImageMagickObject
=================

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
.. _`utility documentation` : utilities.html
.. _`Windows installation package` : INSTALL-windows.html

Overview
========

The *ImageMagickObject* is a COM+ compatible component that may be
invoked from any language capable of using COM objects. The intended use
is for Windows Scripting Host VBS scripts and Visual Basic, but it can
also be used from C++, ASP, and other languages like Delphi, Perl and PHP.

The strategy with the *ImageMagickObject* COM+ component is not to the
same as with PerlMagick interface provided with ImageMagick. PerlMagick
is a low level API based system that defines a whole new way of scripting
IM operations. The IM COM+ component simply provides access to the
`convert`_, composite, `mogrify`_, `identify`_, and `montage`_ tools,
efficiently executing them as part of your process, rather than as
external programs. The way you use it is exactly the same. You pass it a
list of strings including filenames and various options and it does the
job. In fact, you can take any existing batch scripts that use the
command line tools and translate them into the equivalent calls to the
COM+ object in a matter of minutes. Beyond that, there is also a way to
pass in and retrieve images in memory in the form of standard smart
arrays (byte arrays). Samples are provided, to show both the simple and
more elaborate forms of access.

*GraphicsMagick* provides a statically-built ImageMagick object as part of
its "utils" `Windows installation package`_. When this package is
installed, *ImageMagickObject* and its sample programs are installed to

::

  c:\Program Files\GraphicsMagick-1.3-Q16\ImageMagickObject

by default (path shown is for the Q:16 build). *ImageMagickObject* is
registered if the checkbox for "Register ImageMagickObject" is checked at
install time.

You may execute the sample program from the Windows Command Shell like::

  cscript SimpleTest.vbs

Since the *GraphicsMagick* utility command line parsers are incorporated
within *ImageMagickObject*, please refer to the command line `utility
documentation`_ to learn how to use it. The sample VBS scripts show how
the object should be called and used and have lots of comments.

For C++ programmers - have a look at the MagickCMD.cpp command line
utility for an example of how to call the object from C++. This is a bit
complex because the object requires a variable size list of BSTR's to
emulate the command line argc, argv style calling conventions of the COM
component which is more complex in C++ then in VBS or VB.

Other goodies...
================

MagickCMD is a C++ sample, but it can also server as a replacement for
all the other command line utilities in most applications. Instead of
using "``convert xxxx yyyy``" you can use "``MagickCMD convert xxxx
yyyy``" instead. MagickCMD calls the COM object to get the job done. This
small tight combination replaces the entire usual binary distribution in
just a few megabytes.

Building ImageMagickObject From Source
======================================

The source code for *ImageMagickObject* is available from *GraphicsMagick*
CVS, or as part of the *GraphicsMagick* Windows source package. Once the
source code has been retrieved and extracted, the source for
*ImageMagickObject* may be found hidden in the directory
"``GraphicsMagick\contrib\win32\ATL7ImageMagickObject``", however,
*GraphicsMagick* itself must be built using the "static-multithread"
(VisualStaticMT) build configuration. Building *ImageMagickObject*
requires Microsoft Visual C++ 7.0 as delivered with Microsoft's Visual
Studio .net package. See the Windows compilation instructions in order to
get *GraphicsMagick* itself built before building *ImageMagickObject*.

Once the VisualStaticMT project has been built, *ImageMagickObject* may
be built by following the procedure::

  cd  GraphicsMagick\contrib\win32\ATL7ImageMagickObject
  BuildImageMagickObject release

This procedure assumes that the VisualStaticMT project has been built
using the "release" setting for an optimized build. If the "debug"
setting was used for a debug build, then specify the argument "debug"
instead.

To register the DLL as a COM+ server use

::

  regsvr32 /c /s ImageMagickObject.dll

To unregister the DLL use

::

  regsvr32 /u /s ImageMagickObject.dll

The MagickCMD sample program operates similarly to the gm.exe program,
and is a great way to exercise *ImageMagickObject* to verify that it is
working.

Sometime in the future, MagickCMD may assume the place of gm.exe in the
"utils" distribution in order to decrease the overall package size.

-------------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2015

