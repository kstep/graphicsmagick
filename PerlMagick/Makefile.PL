#
# Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated
# to making software imaging solutions freely available.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files ("ImageMagick"),
# to deal in ImageMagick without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of ImageMagick, and to permit persons to whom the
# ImageMagick is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of ImageMagick.

# The software is provided "as is", without warranty of any kind, express or
# implied, including but not limited to the warranties of merchantability,
# fitness for a particular purpose and noninfringement.  In no event shall
# ImageMagick Studio be liable for any claim, damages or other liability,
# whether in an action of contract, tort or otherwise, arising from, out of
# or in connection with ImageMagick or the use or other dealings in
# ImageMagick.
#
# Except as contained in this notice, the name of the ImageMagick Studio
# shall not be used in advertising or otherwise to promote the sale, use or
# other dealings in ImageMagick without prior written authorization from the
# ImageMagick Studio.
#

use ExtUtils::MakeMaker;
use Config;

# Compute test specification
my $delegate_tests='t/*.t';
my $delegate;
foreach $delegate (qw/ bzlib fpx jp2 jpeg lcms mpeg png ps tiff ttf x xfig zlib/) {
  if( -d "t/$delegate" ) {
    $delegate_tests .= " t/$delegate/*.t";
  }
}

# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
<<<<<<< Makefile.PL
WriteMakefile(
    'NAME'	=> 'Image::Magick',
    'VERSION_FROM' => 'Magick.pm', # finds $VERSION
<<<<<<< Makefile.PL
    'LIBS'	=> ['-L/cfs/groups/tp/tools/SunOS5/lib -R/cfs/groups/tp/tools/SunOS5/lib -L/cfs/groups/tp/tools/SunOS5/lib -L/usr/openwin/lib -R/usr/openwin/lib -L/cfs/groups/tp/tools/SunOS5/lib -lMagick -ljbig   -ltiff -lfreetype -ljpeg -lgs -lpng  -ldpstk -ldps -lXext -lXt  -lSM -lICE -lX11 -lsocket  -lnsl -lbz2 -lxml2 -lz -lpthread  -lm'],
=======
    'LIBS'	=> ['-L/usr/lib -L/usr/local/lib -L/usr/X11R6/lib -L/home/cristy/ImageMagick/magick -lMagick    -ltiff -lfreetype -ljpeg -ljasper  -lpng  -ldpstk -ldps -lXext -lXt  -lSM -lICE -lX11  -lbz2  -lz -lpthread  -lm'],
>>>>>>> 1.65
    'DEFINE'	=> '-DHAVE_CONFIG_H',     # e.g., '-DHAVE_SOMETHING' 
<<<<<<< Makefile.PL
<<<<<<< Makefile.PL
    'INC'	=> '-I../ -I.. -I/cfs/groups/tp/tools/SunOS5/include/freetype2 -I/cfs/groups/tp/tools/SunOS5/include/freetype2 -D_REENTRANT -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -I/cfs/groups/tp/tools/SunOS5/include -I/usr/openwin/include -I/usr/openwin/include/X11',
=======
    'INC'	=> '-I../ -I.. -I/usr/include/freetype2 -I/usr/include/freetype2 -D_REENTRANT -D_FILE_OFFSET_BITS=64 -I/usr/local/include -I/X11',
>>>>>>> 1.65
=======
    'INC'	=> '-I../ -I.. -I/usr/include/freetype2 -D_REENTRANT -D_FILE_OFFSET_BITS=64 -I/usr/local/include -I/X11',
>>>>>>> 1.66
    ($Config{'archname'} =~ /-object$/i ? ('CAPI' => 'TRUE') : ()),
);
=======
WriteMakefile
  (
   # Module description
   'ABSTRACT'	=> 'ImageMagick PERL Extension',

   # Set compiler flags other than INC and OPTIMIZE
   # 'CCFLAGS'	=> '-g -O2 -Wall',

   # Preprocessor defines
   'DEFINE'	=> '-DHAVE_CONFIG_H',     # e.g., '-DHAVE_SOMETHING' 

   # Header search specfication and preprocessor flags
   'INC'	=> '-I../ -I.. -I/usr/include/libxml2 -I/usr/include/freetype2 -D_REENTRANT -D_FILE_OFFSET_BITS=64 -I/usr/local/include -I/usr/X11R6/include -I/usr/X11R6/include/X11',

   # Install PerlMagick binary into ImageMagick bin directory
   'INSTALLBIN'	=> '${exec_prefix}/bin',

   # Library specification
   'LIBS'	=> ['-L/usr/lib -L/usr/lib -L/usr/local/lib -L/usr/X11R6/lib -L/home/cristy/ImageMagick/magick -lMagick -llcms -ltiff -lfreetype -ljasper -ljpeg -lpng -lfpx -ldpstk -ldps -lXext -lXt -lSM -lICE -lX11 -lbz2 -lxml2 -lz -lpthread -lm'],

   # Perl binary name (if a Perl binary is built)
   'MAP_TARGET'	=> 'PerlMagick',

   # Perl module name is Image::Magick
   'NAME'	=> 'Image::Magick',

   # Let CFLAGS drive optimization flags by setting OPTIMIZE to empty
   # 'OPTIMIZE'	=> '',

   # Use same compiler as ImageMagick
   # 'PERLMAINCC'	=> 'gcc',

   # Set Perl installation prefix to ImageMagick installation prefix
#   'PREFIX'	=> '/usr/local',

   # File to extract PerlMagick version from
   'VERSION_FROM' => 'Magick.pm',
>>>>>>> 1.67

<<<<<<< Makefile.PL
#
# Modify the MakeMaker test fragment
#
sub MY::test
{
   #
   # List any ImageMagick features derived from add-on libraries
   # or programs you would like to test.
   #
   # Valid choices are:
   #
   #  Feature   Formats Tested		Prerequisites
   #  =======   ======================	======================================
   #   bzlib    BZip compression        BZip library
   #   cgm	CGM format		'ralcgm' program
   #   fpx      FlashPix format         FlashPix library
   #   hdf	HDF format		HDF library
   #   jbig	JBIG format		JBIG library
   #   jpeg 	JPEG v1 format		libjpeg
   #   jp2 	JPEG v2 format		libjasper
   #   mpeg	MPEG format		'mpeg2decode' & 'mpeg2encode' programs
   #   png	PNG format		PNG and Zlib libraries
   #   ps	Postscript format	'gs' program and/or DPS library
   #   rad	Radiance format		'ra_ppm' program
   #   tiff	TIFF format		TIFF library
   #   ttf	TrueType font format	FreeType library
   #   x	X11 support		X-windows libraries and server
   #   xfig	Xfig format		'transfig' program
   #   zlib	Zip compression		Zlib library
   #
   # e.g. my @DELEGATES = qw/jpeg tiff/;
   #
<<<<<<< Makefile.PL
   my @DELEGATES = qw/ bzlib cgm hpgl jbig jpeg png ps tiff ttf wmf x xfig zlib/;
=======
   my @DELEGATES = qw/ bzlib jp2 jpeg mpeg png ps tiff ttf x xfig zlib/;
>>>>>>> 1.65

   package MY; # so that "SUPER" works right
   my $inherited = shift->SUPER::test(@_);
   my $delegate_tests='';
   my $delegate;
   foreach $delegate (@DELEGATES) {
     if( -d "t/$delegate" ) {
       $delegate_tests .= " t/$delegate/*.t";
    }
   }
   $inherited =~ s:^TEST_FILES =.*:TEST_FILES = t/*.t ${delegate_tests}:m;
   #
   # Only test installed PerlMagick due to portability problems
   # and because PerlMagick does not link against the uninstalled
   # ImageMagick library.
   #
   $inherited =~ s:PERL_DL_NONLAZY=1 ::mg;
   $inherited;
}
=======
   # Include delegate directories in tests
   test		=> { TESTS	=>	$delegate_tests},
>>>>>>> 1.67

   ($Config{'archname'} =~ /-object$/i ? ('CAPI' => 'TRUE') : ()),
);
