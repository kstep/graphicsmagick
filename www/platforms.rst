.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

==============
Platform Notes
==============

.. contents::
  :local:

Overview
========

This file provides platform specific portability notes which don't
belong in README.txt. Usually GraphicsMagick compiles fine according to the
procedures described in README.txt, but sometimes a platform related
oddity causes a failure.

Apple OS X (Intel)
==================

  A GraphicsMagick user submitted these steps for how to get
  GraphicsMagick up and going on an Intel Mac:

  The first few steps should be done as an administrator, or with
  proper use of "sudo"
  
  1. Install Fink (a package manager). The default setup will create a
     /sw directory on your main hard drive. It will also make sure
     /sw/bin is in your path.
  
  2. Install the latest X-Code from Apple.
  
  3. Make a link in /Developer/SDKs/MacOSX10.4u.sdk/ to /sw:

     "cd /Developer/SDKs/MacOSX10.4u.sdk; ln -s sw /sw"
  
  4. Use Fink, or FinkCommander to install libjpeg and libtiff and any
     other libraries you might want.
  
  As a regular user or administrator:
  
  5. Download the GM snapshot.
  
  6. Unpack and cd into the source directory.
  
  7. Set your CFLAGS environment variable to (command varies by shell):

     "-O -g -isysroot /Developer/SDKs/MacOSX10.4u.sdk/ -arch i386 -I/sw/include/"
  
  8. Set your LDFLAGS environment variable to:

     "-Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk/,-L/sw/lib/"
  
  9. I used the following ./configure line:

    ./configure --prefix=/sw --with-quantum-depth=16 --disable-dependency-tracking \\
    --with-x=yes --x-includes=/usr/X11R6/include --x-libraries=/usr/X11R6/lib/ --without-perl
  
  10. Issue make
  
  11. Test the binaries as necessary and when satisfied:

    sudo make install
  
  12. start X11 and in a new shell: (or source your .profile or .login)

    gm display


Cygwin
======

  Problem

    PerlMagick fails to link.

  Indications

    libperl.a is not found.

  Solution

    Configure like

      ./configure --enable-shared

    or

      ./configure --enable-shared --with-modules

    Cygwin doesn't normally come with a libperl.a but it does come with the
    equivalent DLL.  Using a shared build gets PerlMagick over the hurdle.
    

SGI Irix
=========

  Problem

    How to successfully configure GraphicsMagick using SGI's commercial
    compiler (e.g. IRIX C/C++ v7.3.1.X)?

  Indications

    Compiling C++ code fails with the error:
    #error directive:  This header file requires the -LANG:std option

  Solution

    Configure like

      ./configure CC=cc CFLAGS="-O2 -OPT:Olimit=0" CXX=CC \
      CXXFLAGS="-LANG:std -Wl,-woff,84 -O2" CXXCPP="CC -LANG:std -E" \
      --with-modules --with-threads ...

    with the following in /etc/compiler.defaults

      -DEFAULT:abi=n32:isa=mips3

Linux 6.1
=========

  Problem

    When '-rpath /somedir' is added to LDFLAGS in order to tell the
    linker to apply a run-time linker path, configure fails.

  Indications

    The configure script prints the error message

      checking for executable suffix...
                                  configure: error: cannot compute EXEEXT:

    and the error message

      gcc: unrecognized option '-rpath'

    is written to config.log

  Solution

    Use 'LDFLAGS=-Wl,-rpath,/somedir' instead to pass the options through
    to the linker.

FreeBSD 3.X and 4.X
===================

  Problem
  
    GraphicsMagick does not run after it is installed. The system does not
    see an installed shared library until the 'ldconfig' command has
    been executed (as root) using the correct options.

  Indications
  
    System fails to find shared library needed by GraphicsMagick.

  Solution
  
    Either reboot the system (which automatically runs ldconfig) or
    execute

      /sbin/ldconfig -m ${PREFIX}/lib

    where ${PREFIX} is the prefix used when configuring GraphicsMagick
    (default /usr/local).

Digital Unix and OSF/1
======================

  Problem
  
    Digital Unix provides an outdated JPEG shared library as part of the
    system.
    
  Indications

    GraphicsMagick fails to link.

  Solution
  
    Ensure that the JPEG library you installed is used. Be sure to
    install the JPEG library as a shared library.

Solaris 2.X
============

  Problem
  
    An outdated delegate library is used rather than the one just
    installed.
    
  Indications
  
    * Failure to link due to unresolved symbols
    * Failure to run properly
    * The command 'ldd `which convert`' lists the wrong library.
  
  Solution

    If the problem library is a shared library (.so extension) then
    install your own library as a shared library. Then add the required
    -L and -R options to find your shared library at both link and run
    time. For example, if your library is is installed in /usr/local/lib:
      
        configure LDFLAGS='-L/usr/local/lib -R/usr/local/lib'

  Problem

    An outdated libtiff.so (libtiff.so.3) is installed in
    /usr/openwin/lib. This library may be used by accident rather than
    the intended libtiff.so, or libtiff.a. In particular, the linker
    generally considers linking against a shared library before it
    considers static libraries, so the system libtiff.so may be used
    even though there is an installed libtiff.a earlier in the linker
    search path.

  Indications
  
    GraphicsMagick fails to link or load TIFF files.

  Solution

    * Install libtiff as a shared library, and make sure that LDFLAGS
      contains the required -L and -R options to find your shared
      library at both link and run time. For example, if libtiff.so is
      installed in /usr/local/lib:
      
      configure LDFLAGS='-L/usr/local/lib -R/usr/local/lib'

    or
      
    * Remove /usr/openwin/lib/libtiff.so.3. Note that if you take this
      step, some tools like 'imagetool' and 'pageview' will stop working.

  Problem
  
    When using gcc 3.0.X, configure decides that the compiler is insufficient
    to compile Magick++.
    
  Indications
  
    Fails ISO C++ test.
  
  Solution

    The problem is due to gcc provided headers undefining
    Solaris-provided defines for 64-bit versions of stdio functions. The
    solution is to either configure with --disable-largefile or edit the
    installed g++-v3/bits/std_cstdio.h to bracket the offending undefs
    (for fgetpos, fopen, freopen, fsetpos, & tmpfile) like::

      #if _FILE_OFFSET_BITS != 64
      #undef fopen
      #endif

    or

    Install gcc 3.1 (or later) instead.

  Problem

    A static library was used in a shared library build.

  Indications

    Linker prints a warning similar to:

    "ld: fatal: relocations remain against allocatable but non-writable
    sections"

  Solution

    All libraries used in a shared library build *must* also be shared
    libraries. Check all libraries referenced in the libtool link
    command line and verify that all of the referenced libraries are
    indeed shared libraries. All system libraries are available as
    shared libraries so it is likely that any static library is locally
    installed.


---------------------------------------------------------------------------

| Copyright (C) 2003 - 2008 GraphicsMagick Group
| Copyright (C) 2002 ImageMagick Studio
| Copyright (C) 1999 E. I. du Pont de Nemours and Company

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.


