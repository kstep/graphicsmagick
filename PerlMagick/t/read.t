#!/usr/local/bin/perl
#
# Test reading formats supported directly by ImageMagick
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..44\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';


print("AVS X image file ...\n");
testRead( 'input.avs', 'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testRead( 'input.bmp', 'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testRead( 'bmp:input.bmp24', 'eb4d6e084afe2835a1ad28ad7fc12ced');

print("ZSoft IBM PC multi-page Paintbrush file ...\n");
++$test;
testRead( 'input.dcx', 'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testRead( 'input.dib', 'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Flexible Image Transport System ...\n");
++$test;
testRead( 'input.fits', '27b8cea85c011f60024ff559096bd11f',
                        'f7aacb0efcd13dbd976fcee78568b2fc' );

print("CompuServe graphics interchange format ...\n");
++$test;
testRead( 'input.gif', '45e5c63ac641eda9bedd4f42a7f6b4cd');

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testRead( 'input.gif87', '45e5c63ac641eda9bedd4f42a7f6b4cd');

print("Gradient (gradual passing from one shade to another) ...\n");
#
++$test;
testRead( 'gradient:red-blue', 'ce3e346956f17cc6131dfe7213d5e11a',
                                '826cc95b15955517fcd9bc8c6bbe6490' );

print("GRANITE (granite texture) ...\n");
#
++$test;
testRead( 'granite:', 'c80c00aa59794ef7731a9117d9222d80');

print("Microsoft icon ...\n");
++$test;
testRead( 'input.ico', '9214d6f1fe694fb2642e332830167475',
                       'c28b9e379c673ac3c4959f1dd17d16a2' );

print("Magick image file format ...\n");
++$test;
testRead( 'input.miff', 'eb4d6e084afe2835a1ad28ad7fc12ced');

print("MTV Raytracing image format ...\n");
++$test;
testRead( 'input.mtv', 'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Xv's visual schnauzer format. ...\n");
++$test;
testRead( 'input_p7.p7', '471ddc0d5b34bc239a58b38c32881cea',
                         'b27023a53881e081e539143602bf1785' );

print("NULL (white image) ...\n");
++$test;
testRead( 'NULL:white', '0f01427889bc25ae6f2a049a6fd00f17',
			'b23b5d09162b92c0284923a7f628d2a5');

print("NULL (black image) ...\n");
++$test;
testRead( 'NULL:black', '9ced1f90ef7fb266a7472cb70b7e6528');

print("NULL (DarkOrange image) ...\n");
++$test;
testRead( 'NULL:DarkOrange', '31be3ed0fa6a7b6d42556ff56e2a0f8a');

print("Portable bitmap format (black and white), ASCII format ...\n");
++$test;
testRead( 'input_p1.pbm', 'b2cc6cad5c66aae9140c96b74437c591');


print("Portable bitmap format (black and white), binary format ...\n");
++$test;
testRead( 'input_p4.pbm', 'b2cc6cad5c66aae9140c96b74437c591');

print("ZSoft IBM PC Paintbrush file ...\n");
++$test;
testRead( 'input.pcx', 'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Portable graymap format (gray scale), ASCII format ...\n");
++$test;
testRead( 'input_p2.pgm', 'c49de394a410034d4682fb48384b5c1a');

print("Portable graymap format (gray scale), binary format ...\n");
++$test;
testRead( 'input_p5.pgm', 'c49de394a410034d4682fb48384b5c1a');

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testRead( 'input.pict', 'eb4d6e084afe2835a1ad28ad7fc12ced',
                        'ca09fc4171ad27787da936baf4cddf5a' );

print("Alias/Wavefront RLE image format ...\n");
++$test;
testRead( 'input.rle', 'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Portable pixmap format (color), ASCII format ...\n");
++$test;
testRead( 'input_p3.ppm', 'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Portable pixmap format (color), binary format ...\n");
++$test;
testRead( 'input_p6.ppm', 'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Adobe Photoshop bitmap file ...\n");
++$test;
testRead( 'input.psd', 'eb4d6e084afe2835a1ad28ad7fc12ced',
                       '2b8f2d03bc34dcd45e24439039aedeee' );

print("Irix RGB image file ...\n");
++$test;
testRead( 'input.sgi', 'eb4d6e084afe2835a1ad28ad7fc12ced');

print("SUN 1-bit Rasterfile ...\n");
++$test;
testRead( 'input.im1', 'a93499e5e2d0e9ae717abbd4ddc994e9');
print("SUN 8-bit Rasterfile ...\n");
++$test;
testRead( 'input.im8', '45e5c63ac641eda9bedd4f42a7f6b4cd');
print("SUN TrueColor Rasterfile ...\n");
++$test;
testRead( 'sun:input.im24', 'eb4d6e084afe2835a1ad28ad7fc12ced');

print("Truevision Targa image file ...\n");
++$test;
testRead( 'input.tga', 'eb4d6e084afe2835a1ad28ad7fc12ced');

print("PSX TIM file ...\n");
++$test;
testRead( 'input.tim', 'b8ebf628b12284b891fcb9992c1539ee');

print("Khoros Visualization image file ...\n");
++$test;
testRead( 'input.viff', '1135eda0cb17e0d7553452624242da47');

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testRead( 'input.xbm', 'a93499e5e2d0e9ae717abbd4ddc994e9');

print("XC: Constant image of X server color ...\n");
++$test;
testRead( 'xc:black', '9ced1f90ef7fb266a7472cb70b7e6528');

print("X Windows system pixmap file (color) ...\n");
++$test;
testRead( 'input.xpm', '45e5c63ac641eda9bedd4f42a7f6b4cd');

print("X Windows system window dump file (color) ...\n");
++$test;
testRead( 'input.xwd', 'eb4d6e084afe2835a1ad28ad7fc12ced');


print("TILE (Tile image with a texture) ...\n");
# This is an internal generated format
# We will tile using the default image and a MIFF file
#
++$test;
testReadSized( 'TILE:input.miff', '140x92', 8, '4aa6f50cf3005b464eee2f0fbb9c0e46');


print("CMYK format ...\n");
++$test;
testReadSized( 'input_70x46.cmyk',
	       '70x46',
	       8,
	       'eb059e77f4e55a08861e2ae8a13226c4',
	       'ec3f9a01db1de264d2bb8ab4687d3645');

print("GRAY format ...\n");
++$test;
testReadSized( 'input_70x46.gray',
	       '70x46',
	       8,
	       '27b8cea85c011f60024ff559096bd11f',
	       'e0b0d2607784a1b831024d66b9565b2c' );

print("RGB format ...\n");
++$test;
testReadSized( 'input_70x46.rgb',
	       '70x46',
	       8,
	       'eb4d6e084afe2835a1ad28ad7fc12ced',
	       '35ce65f982f803221d4417a403effa2e' );

print("RGBA format ...\n");
++$test;
testReadSized( 'input_70x46.rgba',
	       '70x46',
	       8,
	       '33faa53b5e96ee9ffc00e5e115545d75',
	       '0cb37a232fc38af9c00b730d057c68f7' );


print("UYVY format ...\n");
++$test;
testReadSized( 'input_70x46.uyvy',
	       '70x46',
	       0,
	       '0088a910bc01fb37d659251f55f89570',
	       '7ddde0b3f0b4df199973e851b4493d16' );
