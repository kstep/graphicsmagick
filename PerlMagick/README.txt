# Copyright (C) 2003-2009 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1998,1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
===========================================================================

Introduction 

    PerlMagick, is an objected-oriented Perl interface to GraphicsMagick.
    Use the module to read, manipulate, or write an image or image
    sequence from within a Perl script. This makes it suitable for Web
    CGI scripts. You must have GraphicsMagick 1.2 or above installed on
    your system for this module to work properly.

    See

        http://www.graphicsmagick.org/www/perl.html

    for additional information about PerlMagick.  See

        http://www.graphicsmagick.org/

    for instructions about installing GraphicsMagick.


Installation 

    PerlMagick is configured by default by GraphicsMagick in order to
    create a starting Makefile.PL.  Additional edits to Makefile.PL
    may be required.  GraphicsMagick does not provide a seperate
    distribution of PerlMagick.  Please follow the applicable steps
    described here in order to complete the installation of
    PerlMagick.

    Get the GraphicsMagick distribution and type the following: 

        gunzip -c GraphicsMagick-1.2.tar.gz | tar -xvf -
        cd GraphicsMagick
        ./configure --enable-shared
        make
        su root (if necessary)
        make install
        cd PerlMagick

    If you used GraphicsMagick configure then Makefile.PL should already
    be prepared for use. If not, or you want to change a setting, then
    edit Makefile.PL and change LIBS and INC to include the appropriate
    path information to the required libGraphicsMagick library. You will
    also need library search paths (-L) to JPEG, PNG, TIFF, etc.
    libraries if they were included with your installed version of
    GraphicsMagick. If an extension library is built as a shared library
    but not installed in the system's default library search path, you
    may need to add run-path information (often -R or -rpath)
    corresponding to the equivalent library search path option so that
    the library can be located at run-time.

    To create and install the dymamically-loaded version of
    PerlMagick (the preferred way), execute
        
        perl Makefile.PL
        make
        su root (if necessary)
        make install

    [ Note that the following procedure for building a static
      PerlMagick seems to work only for perl 5.8.8 and earlier ]

    To create and install a new 'perl' executable (replacing your
    existing PERL interpreter!) with PerlMagick statically linked
    (but other libraries linked statically or dynamically according
    to system linker default), execute

        perl Makefile.PL
        make perl
        make -f Makefile.aperl inst_perl
	
    or to create and install a new PERL interpreter with a
    different name than 'perl' (e.g. 'PerlMagick') and with
    PerlMagick statically linked

        perl Makefile.PL MAP_TARGET=PerlMagick
	make PerlMagick
        make -f Makefile.aperl inst_perl

    See the ExtUtils::MakeMaker(3) manual page for more information on
    building PERL extensions (like PerlMagick).

    For Windows systems, type

        perl Makefile.nt
        nmake install

    For Unix, you typically need to be root to install the software.
    There are ways around this.  Consult the Perl manual pages for more
    information. You are now ready to utilize the PerlMagick routines
    from within your Perl scripts.


Testing PerlMagick

    Before PerlMagick is installed, you may want to execute
    
        make test

    to verify that PERL can load the PerlMagick extension ok.  Chances
    are some of the tests will fail if you do not have the proper
    delegates installed for formats like JPEG, TIFF, etc.  If 'make
    test' fails in some gruesome way (e.g. many tests fail), then it
    is advised not to install PerlMagick until the problem is
    resolved.

    To see a number of PerlMagick demonstration scripts, type
    
        cd demo
        make


Example Perl Magick Script 

    Here is an example script to get you started: 

        #!/usr/bin/perl
        use Graphics::Magick;

        $q = Graphics::Magick->new;
        $x = $q->Read("model.gif", "logo.gif", "rose.gif");
        warn "$x" if $x;

        $x = $q->Crop(geom=>'100x100+100+100');
        warn "$x" if $x;

        $x = $q->Write("x.gif");
        warn "$x" if $x;

    The script reads three images, crops them, and writes a single
    image as a GIF animation sequence.
