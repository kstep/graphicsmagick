use ExtUtils::MakeMaker;
use Config;

# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    'NAME'	=> 'Image::Magick',
    'VERSION_FROM' => 'Magick.pm', # finds $VERSION
    'LIBS'	=> ['-L/usr/local/lib -L/usr/X11R6/lib -L/home/cristy/ImageMagick/magick -lMagick    -ltiff  -ljpeg -lpng -lfpx  -lXext -lXt  -lSM -lICE -lX11  -lbz2  -lz -lpthread  -lm'],
    'DEFINE'	=> '-DHAVE_CONFIG_H',     # e.g., '-DHAVE_SOMETHING' 
    'INC'	=> '-I../ -I.. -D_REENTRANT -D_FILE_OFFSET_BITS=64 -I/usr/local/include -I/usr/X11R6/include',
    ($Config{'archname'} =~ /-object$/i ? ('CAPI' => 'TRUE') : ()),
);

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
   #   jpeg 	JPEG format		JPEG library
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
   my @DELEGATES = qw/ bzlib fpx jpeg mpeg png ps tiff x zlib/;

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

