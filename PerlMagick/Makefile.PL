# Generated automatically from Makefile.PL.in by configure.
use ExtUtils::MakeMaker;
use Config;

# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    'NAME'	=> 'Image::Magick',
    'VERSION_FROM' => 'Magick.pm', # finds $VERSION
    'LIBS'	=> ['-L/pathos/home/cristy/ImageMagick/bzlib -L/pathos/home/cristy/ImageMagick/hdf/hdf/src -L/pathos/home/cristy/ImageMagick/jbig/libjbig -L/pathos/home/cristy/ImageMagick/jpeg -L/pathos/home/cristy/ImageMagick/zlib -L/pathos/home/cristy/ImageMagick/png -L/pathos/home/cristy/ImageMagick/tiff/libtiff -L/pathos/home/cristy/ImageMagick/ttf -L/pathos/home/cristy/ImageMagick/ttf/lib -L/pathos/home/cristy/ImageMagick/ttf/lib/.libs -L/usr/local/lib -L/usr/openwin/lib -R/usr/openwin/lib -L/usr/local/lib -lMagick  -ljbig -ltiff -lttf -ljpeg -lpng -ldpstk -ldps -lXext -lXt -lX11 -lsocket -lnsl -lbz2 -lz -lm'],
    'DEFINE'	=> '',     # e.g., '-DHAVE_SOMETHING' 
    'INC'	=> '-I.. -I/pathos/home/cristy/ImageMagick/bzlib -I/pathos/home/cristy/ImageMagick/hdf/hdf/src -I/pathos/home/cristy/ImageMagick/jbig/libjbig -I/pathos/home/cristy/ImageMagick/jpeg -I/pathos/home/cristy/ImageMagick/zlib -I/pathos/home/cristy/ImageMagick/png -I/pathos/home/cristy/ImageMagick/tiff/libtiff -I/pathos/home/cristy/ImageMagick/ttf -I/pathos/home/cristy/ImageMagick/ttf/lib -I/pathos/home/cristy/ImageMagick/ttf/lib/.libs -I/usr/local/include -I/usr/openwin/include -I/usr/openwin/include/X11',
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
   my @DELEGATES = qw/ bzlib cgm hpgl jbig jpeg mpeg png ps rad tiff ttf x xfig zlib/;

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
   $inherited;
}

