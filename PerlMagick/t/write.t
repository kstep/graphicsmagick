#!/usr/local/bin/perl
#
# Test writing formats supported directly by ImageMagick
#
# Currently supported tests are for formats that ImageMagick
# knows how to both read and write.
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.

BEGIN { $| = 1; $test=1; print "1..36\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

#
# AVS X image file
#
testReadWrite( 'AVS:input.avs',
               'AVS:output.avs',
               q//,
               '93bd185340123477f0a0e6622392a5b4' );
#
# Joint Bi-level Image experts Group file interchange format
#
++$test;
testReadWrite( 'BIE:input.bie',
               'BIE:output.bie',
               q//,
               '1a8d5855273e27302856c727934c9f9a' );

#
# Microsoft Windows bitmap image file
#
++$test;
testReadWrite( 'BMP:input.bmp',
               'BMP:output.bmp',
               q//,
               '25c550da420eeb782f57dd88c689e819' );

#
# Microsoft Windows 24-bit bitmap image file
#
++$test;
testReadWrite( 'BMP24:input.bmp24',
               'BMP24:output.bmp24',
               q//,
               '3a17784c61a87a98f8e25f1977b9d363' );

#
# ZSoft IBM PC multi-page Paintbrush file
#
++$test;
testReadWrite( 'DCX:input.dcx',
               'DCX:output.dcx',
               q//,
               '25c550da420eeb782f57dd88c689e819' );

#
# Microsoft Windows bitmap image file
#
++$test;
testReadWrite( 'DIB:input.dib',
               'DIB:output.dib',
               q//,
               '5cb2c2f3d73b57302059da592506be22' );

#
# Flexible Image Transport System
#
++$test;
testReadWrite( 'FITS:input.fits',
               'FITS:output.fits',
               q//,
               '21e7890c853c980821f28d233386f872' );

#
# CompuServe graphics interchange format
#
++$test;
testReadWrite( 'GIF:input.gif',
               'GIF:output.gif',
               q//,
               '25c550da420eeb782f57dd88c689e819' );

#
# CompuServe graphics interchange format (1987)
#
++$test;
testReadWrite( 'GIF87:input.gif87',
               'GIF87:output.gif87',
               q//,
               '25c550da420eeb782f57dd88c689e819' );

#
# Magick image file format
#
++$test;
testReadWrite( 'MIFF:input.miff',
               'MIFF:output.miff',
               q//,
               '25c550da420eeb782f57dd88c689e819' );

#
# MTV Raytracing image format
#
++$test;
testReadWrite( 'MTV:input.mtv',
               'MTV:output.mtv',
               q//,
               '3a17784c61a87a98f8e25f1977b9d363' );

#
# Xv's visual schnauzer format.
#
++$test;
testReadWrite( 'P7:input_p7.p7',
               'P7:output_p7.p7',
               q//,
               'ce0023b15e3ae5f140d4fc5f1bdfe6ab' );

#
# Portable bitmap format (black and white), ASCII format
#
++$test;
testReadWrite( 'input_p1.pbm',
               'output_p1.pbm',
               q/compress=>'None'/,
               '5ee57461fb1810e09ed6ddc6ffb11c7d' );

#
# Portable bitmap format (black and white), binary format
#
++$test;
testReadWrite( 'input_p4.pbm',
               'output_p4.pbm',
               q//,
               '5ee57461fb1810e09ed6ddc6ffb11c7d' );

#
# ZSoft IBM PC Paintbrush file
#
++$test;
testReadWrite( 'PCX:input.pcx',
               'PCX:output.pcx',
               q//,
               '25c550da420eeb782f57dd88c689e819' );

#
# Portable graymap format (gray scale), ASCII format
#
++$test;
testReadWrite( 'input_p2.pgm',
               'output_p2.pgm',
               q/compress=>'None'/,
               '2cf0b0506d5464a70978557fbb02ff22' );

#
# Portable graymap format (gray scale), binary format
#
++$test;
testReadWrite( 'input_p5.pgm',
               'output_p5.pgm',
               q//,
               '2cf0b0506d5464a70978557fbb02ff22' );

#
# Apple Macintosh QuickDraw/PICT file
#
++$test;
testReadWrite( 'PICT:input.pict',
               'PICT:output.pict',
               q//,
               'a166b7ae4fdb91cf89f612761f22317e' );

#
# Portable pixmap format (color), ASCII format
#
++$test;
testReadWrite( 'input_p3.ppm',
               'output_p3.ppm',
               q/compress=>'None'/,
               'd5fdeba9d1e28db7c07f1485d0d64697' );

#
# Portable pixmap format (color), binary format
#
++$test;
testReadWrite( 'input_p6.ppm',
               'output_p6.ppm',
               q//,
               'd5fdeba9d1e28db7c07f1485d0d64697' );

#
# Adobe Photoshop bitmap file
#
++$test;
testReadWrite( 'PSD:input.psd',
               'PSD:output.psd',
               q//,
               '25c550da420eeb782f57dd88c689e819' );

#
# Irix RGB image file
#
++$test;
testReadWrite( 'SGI:input.sgi',
               'SGI:output.sgi',
               q//,
               'a05c5863b2d073046329b0b71d5e85e9' );

#
# SUN 1-bit Rasterfile
#
++$test;
testReadWrite( 'SUN:input.im1',
               'SUN:output.im1',
               q//,
               '5ee57461fb1810e09ed6ddc6ffb11c7d' );

#
# SUN 8-bit Rasterfile
#
++$test;
testReadWrite( 'SUN:input.im8',
               'SUN:output.im8',
               q//,
               '25c550da420eeb782f57dd88c689e819' );

#
# SUN True-Color Rasterfile
#
++$test;
testReadWrite( 'SUN:input.im24',
               'SUN:output.im24',
               q//,
               '5cb2c2f3d73b57302059da592506be22' );

#
# Truevision Targa image file
#
++$test;
testReadWrite( 'TGA:input.tga',
               'TGA:output.tga',
               q//,
               '25c550da420eeb782f57dd88c689e819' );


#
# Khoros Visualization image file
#
++$test;
testReadWrite( 'VIFF:input.viff',
               'VIFF:output.viff',
               q//,
               '5cb2c2f3d73b57302059da592506be22' );
#
# X Windows system bitmap (black and white only)
#
++$test;
testReadWrite( 'XBM:input.xbm',
               'XBM:output.xbm',
               q//,
               '1a8d5855273e27302856c727934c9f9a' );

#
# X Windows system pixmap file (color)
#
++$test;
testReadWrite( 'XPM:input.xpm',
               'XPM:output.xpm',
               q//,
               '8c6c6cb0311fb938c42bf9c2d169b9e8' );

#
# X Windows system window dump file (color)
#
++$test;
testReadWrite( 'XWD:input.xwd',
               'XWD:output.xwd',
               q//,
               '25c550da420eeb782f57dd88c689e819' );

#
# GRAY format
#
++$test;
testReadWriteSized( 'GRAY:input_70x46.gray',
		    'GRAY:output_70x46.gray',
		    '70x46',
		    q//,
		    '42129d46fc552e41c3e23977e7b6e09f' );

#
# RGB format
#
++$test;
testReadWriteSized( 'RGB:input_70x46.rgb',
		    'RGB:output_70x46.rgb',
		    '70x46',
		    q//,
		    '5cb2c2f3d73b57302059da592506be22' );

#
# RGBA format
#
++$test;
testReadWriteSized( 'RGBA:input_70x46.rgba',
		    'RGBA:output_70x46.rgba',
		    '70x46',
		    q//,
		    '5cb2c2f3d73b57302059da592506be22' );

#
# CMYK format
#
++$test;
testReadWriteSized( 'CMYK:input_70x46.cmyk',
		    'CMYK:output_70x46.cmyk',
		    '70x46',
		    q//,
		    '5cb2c2f3d73b57302059da592506be22' );

#
# UYVY format
#
++$test;
testReadWriteSized( 'UYVY:input_70x46.uyvy',
		    'UYVY:output_70x46.uyvy',
		    '70x46',
		    q//,
		    '515bb4d72a3219513d5dd4c82f0aa3c4' );

#
# YUV format
#
++$test;
testReadWriteSized( 'YUV:input_70x46.yuv',
		    'YUV:output_70x46.yuv',
		    '70x46',
		    q//,
		    '0b6a70f64d39a96e3c6823f27c28bdbd' );


1;
