#
# Copyright (C) 2002 GraphicsMagick Group, an organization dedicated
# to making software imaging solutions freely available.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files ("Open
# ImageMagick"), to deal in GraphicsMagick without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of ImageMagick, and
# to permit persons to whom GraphicsMagick is furnished to do so,
# subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of GraphicsMagick.
#
# The software is provided "as is", without warranty of any kind, express
# or implied, including but not limited to the warranties of
# merchantability, fitness for a particular purpose and noninfringement.
# In no event shall GraphicsMagick Group be liable for any claim,
# damages or other liability, whether in an action of contract, tort or
# otherwise, arising from, out of or in connection with GraphicsMagick
# or the use or other dealings in GraphicsMagick.
#
# Except as contained in this notice, the name of the GraphicsMagick
# Group shall not be used in advertising or otherwise to promote the
# sale, use or other dealings in GraphicsMagick without prior written
# authorization from the GraphicsMagick Group.
#

use ExtUtils::MakeMaker;
use Config;

# Compute test specification
my $delegate_tests='t/*.t';
my $delegate;
foreach $delegate (qw/ bzlib cgm fpx hpgl jbig jp2 jpeg lcms mpeg png ps rad tiff ttf wmf x xfig zlib/) {
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
   # 'CCFLAGS'	=> '-O2 -Wall',

   # Preprocessor defines
   'DEFINE'	=> '-DHAVE_CONFIG_H',     # e.g., '-DHAVE_SOMETHING' 

   # Header search specfication and preprocessor flags
   'INC'	=> '-I../ -I.. -I/usr/local/include/freetype2 -D_REENTRANT -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -I/usr/local/include/libxml2',

   # Install PerlMagick binary into ImageMagick bin directory
   'INSTALLBIN'	=> '/usr/local/bin',

   # Library specification
   'LIBS'	=> ['-L/usr/local/lib -R/usr/local/lib -L/usr/local/lib -L/usr/local/lib -L/home/glennrp/CVS/ImageMagick/magick -lGraphicsMagick -ljbig -llcms -ltiff -lfreetype -ljasper -ljpeg -lgs -lpng -lfpx -lwmflite -lXext -lSM -lICE -lX11 -lsocket -lnsl -lbz2 -lxml2 -lz -lpthread -lm'],

   # Perl binary name (if a Perl binary is built)
   'MAP_TARGET'	=> 'PerlMagick',

   # Perl module name is Graphics::Magick
   'NAME'	=> 'Graphics::Magick',

   # Let CFLAGS drive optimization flags by setting OPTIMIZE to empty
   # 'OPTIMIZE'	=> '',

   # Use same compiler as ImageMagick
   # 'PERLMAINCC'	=> 'gcc',

   # Set Perl installation prefix to ImageMagick installation prefix
#   'PREFIX'	=> '/usr/local',

   # File to extract PerlMagick version from
   'VERSION_FROM' => 'Magick.pm',

   # Include delegate directories in tests
   test		=> { TESTS	=>	$delegate_tests},

   ($Config{'archname'} =~ /-object$/i ? ('CAPI' => 'TRUE') : ()),
);
