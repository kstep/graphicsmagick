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
WriteMakefile
  (
   # Module description
   'ABSTRACT'	=> 'ImageMagick PERL Extension',

   # Module author
   'AUTHOR'    => 'ImageMagick Studio',

   # Set compiler flags other than INC and OPTIMIZE
   # 'CCFLAGS'	=> '-g -O2 -Wall',

   # Preprocessor defines
   'DEFINE'	=> '-DHAVE_CONFIG_H',     # e.g., '-DHAVE_SOMETHING' 

   # Header search specfication and preprocessor flags
   'INC'	=> '-I../ -I.. -D_REENTRANT -D_FILE_OFFSET_BITS=64 -I/usr/X11R6/include -I/usr/X11R6/include/X11 -I/usr/include/freetype2 -I/usr/include/libxml2',

   # Install PerlMagick binary into ImageMagick bin directory
   'INSTALLBIN'	=> '/usr/local/bin',

   # Library specification
   'LIBS'	=> ['-L/usr/X11R6/lib -L/usr/lib -L/usr/lib -L/usr/local/lib -lMagick -llcms -lfreetype -lXext -lXt -lSM -lICE -lX11 -lz -lpthread -lm'],

   # Perl binary name (if a Perl binary is built)
   'MAP_TARGET'	=> 'PerlMagick',

   # Perl module name is Image::Magick
   'NAME'	=> 'Image::Magick',

   # Let CFLAGS drive optimization flags by setting OPTIMIZE to empty
   # 'OPTIMIZE'	=> '',

   # Use same compiler as ImageMagick
   # 'PERLMAINCC'	=> 'gcc -L/home/cristy/ImageMagick-5.4.8/ttf/objs',

   # Set Perl installation prefix to ImageMagick installation prefix
#   'PREFIX'	=> '/usr/local',

   # File to extract PerlMagick version from
   'VERSION_FROM' => 'Magick.pm',

   # Include delegate directories in tests
   test		=> { TESTS	=>	$delegate_tests},

   ($Config{'archname'} =~ /-object$/i ? ('CAPI' => 'TRUE') : ()),
);
