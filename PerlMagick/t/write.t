#!/usr/local/bin/perl
#
# Test writing formats supported directly by ImageMagick
#
# Currently supported tests are for formats that ImageMagick
# knows how to both read and write.
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.

BEGIN { $| = 1; $test=1; print "1..33\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

print("AVS X image file ...\n");
testReadWrite( 'AVS:input.avs',
               'AVS:output.avs',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testReadWrite( 'BMP:input.bmp',
               'BMP:output.bmp',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testReadWrite( 'BMP:input.bmp24',
               'BMP:output.bmp24',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced');


print("ZSoft IBM PC multi-page Paintbrush file ...\n");
++$test;
testReadWrite( 'DCX:input.dcx',
               'DCX:output.dcx',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testReadWrite( 'DIB:input.dib',
               'DIB:output.dib',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Flexible Image Transport System ...\n");
++$test;
testReadWrite( 'FITS:input.fits',
               'FITS:output.fits',
               q//,
               '27b8cea85c011f60024ff559096bd11f',
               '62907fa59892a6241e81fa10cd513ef2' );

print("CompuServe graphics interchange format ...\n");
++$test;
testReadWrite( 'GIF:input.gif',
               'GIF:output.gif',
               q//,
               '45e5c63ac641eda9bedd4f42a7f6b4cd');

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testReadWrite( 'GIF87:input.gif87',
               'GIF87:output.gif87',
               q//,
               '45e5c63ac641eda9bedd4f42a7f6b4cd');


print("Magick image file format ...\n");
++$test;
testReadWrite( 'MIFF:input.miff',
               'MIFF:output.miff',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced');

print("MTV Raytracing image format ...\n");
++$test;
testReadWrite( 'MTV:input.mtv',
               'MTV:output.mtv',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Xv's visual schnauzer format ...\n");
++$test;
testReadWrite( 'P7:input_p7.p7',
               'P7:output_p7.p7',
               q//,
               '8ac1386b0ec8dc9bcdcef80379eb3551',
               '3e539a3d9b2054805f2231a952bf70a7' );


print("Portable bitmap format (black and white), ASCII format ...\n");
++$test;
testReadWrite( 'PBM:input_p1.pbm',
               'PBM:output_p1.pbm',
               q/compress=>'None'/,
               'b2cc6cad5c66aae9140c96b74437c591');


print("Portable bitmap format (black and white), binary format ...\n");
++$test;
testReadWrite( 'PBM:input_p4.pbm',
               'PBM:output_p4.pbm',
               q//,
               'b2cc6cad5c66aae9140c96b74437c591');

print("ZSoft IBM PC Paintbrush file ...\n");
++$test;
testReadWrite( 'PCX:input.pcx',
               'PCX:output.pcx',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Portable graymap format (gray scale), ASCII format ...\n");
++$test;
testReadWrite( 'PGM:input_p2.pgm',
               'PGM:output_p2.pgm',
               q/compress=>'None'/,
               'c49de394a410034d4682fb48384b5c1a',
	       'f7943cb6bd610629d2314b75ad3c1218');

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testReadWrite( 'PICT:input.pict',
               'PICT:output.pict',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Portable pixmap format (color), ASCII format ...\n");
++$test;
testReadWrite( 'PPM:input_p3.ppm',
               'PPM:output_p3.ppm',
               q/compress=>'None'/,
               'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Portable graymap format (gray scale), binary format ...\n");
++$test;
testReadWrite( 'PGM:input_p5.pgm',
               'PGM:output_p5.pgm',
               q//,
               'c49de394a410034d4682fb48384b5c1a',
	       'f7943cb6bd610629d2314b75ad3c1218');

print("Portable pixmap format (color), binary format ...\n");
++$test;
testReadWrite( 'PPM:input_p6.ppm',
               'PPM:output_p6.ppm',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Adobe Photoshop bitmap file ...\n");
++$test;
testReadWrite( 'PSD:input.psd',
               'PSD:output.psd',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced',
               '2b8f2d03bc34dcd45e24439039aedeee' );

print("Irix RGB image file ...\n");
++$test;
testReadWrite( 'SGI:input.sgi',
               'SGI:output.sgi',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced');


print("SUN 1-bit Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im1',
               'SUN:output.im1',
               q//,
               'a93499e5e2d0e9ae717abbd4ddc994e9');


print("SUN 8-bit Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im8',
               'SUN:output.im8',
               q//,
               '45e5c63ac641eda9bedd4f42a7f6b4cd');


print("SUN True-Color Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im24',
               'SUN:output.im24',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced');


print("Truevision Targa image file ...\n");
++$test;
testReadWrite( 'TGA:input.tga',
               'TGA:output.tga',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced');



print("Khoros Visualization image file ...\n");
++$test;
testReadWrite( 'VIFF:input.viff',
               'VIFF:output.viff',
               q//,
               '1135eda0cb17e0d7553452624242da47');

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testReadWrite( 'XBM:input.xbm',
               'XBM:output.xbm',
               q//,
               'a93499e5e2d0e9ae717abbd4ddc994e9');

print("X Windows system pixmap file (color) ...\n");
++$test;
testReadWrite( 'XPM:input.xpm',
               'XPM:output.xpm',
               q//,
               '45e5c63ac641eda9bedd4f42a7f6b4cd');


print("X Windows system window dump file (color) ...\n");
++$test;
testReadWrite( 'XWD:input.xwd',
               'XWD:output.xwd',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced');


print("CMYK format ...\n");
++$test;
testReadWriteSized( 'CMYK:input_70x46.cmyk',
		    'CMYK:output_70x46.cmyk',
		    '70x46',
		    8,
		    q//,
		    'eb059e77f4e55a08861e2ae8a13226c4',
                    '5a3260bf305600b1a693906fec1cac63' );


print("GRAY format ...\n");
++$test;
testReadWriteSized( 'GRAY:input_70x46.gray',
		    'GRAY:output_70x46.gray',
		    '70x46',
		    8,
		    q//,
		    '27b8cea85c011f60024ff559096bd11f',
                    '3e07d093efec1e28455d248fc68c2a06' );

print("RGB format ...\n");
++$test;
testReadWriteSized( 'RGB:input_70x46.rgb',
		    'RGB:output_70x46.rgb',
		    '70x46',
		    8,
		    q//,
		    'eb4d6e084afe2835a1ad28ad7fc12ced',
                    '35ce65f982f803221d4417a403effa2e' );


print("RGBA format ...\n");
++$test;
testReadWriteSized( 'RGBA:input_70x46.rgba',
		    'RGBA:output_70x46.rgba',
		    '70x46',
		    8,
		    q//,
		    '33faa53b5e96ee9ffc00e5e115545d75',
                    'b1dcc83f5d70ba6ca5d517028455b7e1' );

1;
