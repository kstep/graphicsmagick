#!/usr/local/bin/perl
#
# Test writing formats supported directly by ImageMagick
#
# Currently supported tests are for formats that ImageMagick
# knows how to both read and write.
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.

BEGIN { $| = 1; $test=1; print "1..32\n"; }
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
               '12643c99ecf96cf1d7e8f5cc62b13c78');

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testReadWrite( 'BMP24:input.bmp24',
               'BMP24:output.bmp24',
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
               'ec5b1420c419973d9ca3c723ae265c98',
               '4bbb09705c7a86e64d9e175be6623dd4' );

print("CompuServe graphics interchange format ...\n");
++$test;
testReadWrite( 'GIF:input.gif',
               'GIF:output.gif',
               q//,
               '863061c238e6d59e7926772d83c36611');

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testReadWrite( 'GIF87:input.gif87',
               'GIF87:output.gif87',
               q//,
               '863061c238e6d59e7926772d83c36611');


print("Magick image file format ...\n");
++$test;
testReadWrite( 'MIFF:input.miff',
               'MIFF:output.miff',
               q//,
               '12643c99ecf96cf1d7e8f5cc62b13c78');

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
               'bf5884d4a711e3f7a66d53b92bba301f' );


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
               'c49de394a410034d4682fb48384b5c1a');

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testReadWrite( 'PICT:input.pict',
               'PICT:output.pict',
               q//,
               '12643c99ecf96cf1d7e8f5cc62b13c78');

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
               'c49de394a410034d4682fb48384b5c1a');

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
               '12643c99ecf96cf1d7e8f5cc62b13c78',
               '5e5437a2685e456b429c72686e5d93e4' );

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
               'b2cc6cad5c66aae9140c96b74437c591');


print("SUN 8-bit Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im8',
               'SUN:output.im8',
               q//,
               '12643c99ecf96cf1d7e8f5cc62b13c78');


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
               '12643c99ecf96cf1d7e8f5cc62b13c78');

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testReadWrite( 'XBM:input.xbm',
               'XBM:output.xbm',
               q//,
               'b2cc6cad5c66aae9140c96b74437c591');

print("X Windows system pixmap file (color) ...\n");
++$test;
testReadWrite( 'XPM:input.xpm',
               'XPM:output.xpm',
               q//,
               '12643c99ecf96cf1d7e8f5cc62b13c78');


print("X Windows system window dump file (color) ...\n");
++$test;
testReadWrite( 'XWD:input.xwd',
               'XWD:output.xwd',
               q//,
               'eb4d6e084afe2835a1ad28ad7fc12ced');


print("GRAY format ...\n");
++$test;
testReadWriteSized( 'GRAY:input_70x46.gray',
		    'GRAY:output_70x46.gray',
		    '70x46',
		    q//,
		    'f0ad0e33df6ed019b9e15ee5e1741d16',
                    'e0b0d2607784a1b831024d66b9565b2c' );

print("RGB format ...\n");
++$test;
testReadWriteSized( 'RGB:input_70x46.rgb',
		    'RGB:output_70x46.rgb',
		    '70x46',
		    q//,
		    'eb4d6e084afe2835a1ad28ad7fc12ced',
                    '35ce65f982f803221d4417a403effa2e' );


print("RGBA format ...\n");
++$test;
testReadWriteSized( 'RGBA:input_70x46.rgba',
		    'RGBA:output_70x46.rgba',
		    '70x46',
		    q//,
		    'eb4d6e084afe2835a1ad28ad7fc12ced',
                    '97558e5d40363309da60696286664df1' );

1;
