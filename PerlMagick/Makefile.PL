# Generated automatically from Makefile.PL.in by configure.
use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    'NAME'	=> 'Image::Magick',
    'VERSION_FROM' => 'Magick.pm', # finds $VERSION
    'LIBS'	=> ['-L/usr/local/lib -L/usr/openwin/lib -R/usr/openwin/lib -L/usr/local/lib -L/usr/local/lib -lMagick  -lX11 -lsocket -lnsl -lXext -lXt -ldpstk -ldps -lm'],
    'DEFINE'	=> '',     # e.g., '-DHAVE_SOMETHING' 
    'INC'	=> '-I../magick -I/usr/local/include -I/usr/openwin/include -I/usr/openwin/include/X11',
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
   #   IM	ImageMagick built-ins	None
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
   # e.g. my @PLUGINS = qw/IM jpeg tiff/;
   #
   my @PLUGINS = qw/IM cgm mpeg ps rad x xfig/;

   package MY; # so that "SUPER" works right
   my $inherited = shift->SUPER::test(@_);
   my $plugintests='';
   my $plugin;
   foreach $plugin (@PLUGINS) {
     if( -d "t/$plugin" ) {
       $plugintests .= " t/$plugin/*.t";
    }
   }
   $inherited =~ s:^TEST_FILES =.*:TEST_FILES = t/*.t ${plugintests}:m;
   $inherited;
}

