#!/usr/local/bin/perl
#
# Test reading formats supported directly by ImageMagick
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..45\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';


print("AVS X image file ...\n");
testRead( 'input.avs',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testRead( 'input.bmp',
  '68a906a1f50c866db9d115b20bea1f008f68da87034caeaa78be40d41d529106');

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testRead( 'bmp:input.bmp24',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("ZSoft IBM PC multi-page Paintbrush file ...\n");
++$test;
testRead( 'input.dcx',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testRead( 'input.dib',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Flexible Image Transport System ...\n");
++$test;
testRead( 'input.fits',
  '7ecdf2c1ae36cbf8a82c336c854b47da8683006d27e5ae5049c98d111dd927d2' );

print("CompuServe graphics interchange format ...\n");
++$test;
testRead( 'input.gif',
  'e8063197764e173ac9f4c903c46fe0b2d1360f522bff1dea49f2e049523d71c2');

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testRead( 'input.gif87',
  'e8063197764e173ac9f4c903c46fe0b2d1360f522bff1dea49f2e049523d71c2');

print("Gradient (gradual passing from one shade to another) ...\n");
++$test;
testRead( 'gradient:red-blue',
  '2ec487d1f4abbf14e181bcb165a79afe3989f7212449cfb740e82277b2a13693',
  '4bc1c8bd802be60bbf9633b2e058efb3a2815fcf4195f1b07e319df29f2efafc' );

print("GRANITE (granite texture) ...\n");
++$test;
testRead( 'granite:',
  'af6f0cf964c00e46430bd1acaa29da13de5a20f20708e92f4ea161e45e3180ca');

print("Microsoft icon ...\n");
++$test;
testRead( 'input.ico',
  '07e0d73200e38f1b2c84bcf043540204f5237c896e7ad59af3ffdfe5455832d1' );

print("Magick image file format ...\n");
++$test;
testRead( 'input.miff',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("MTV Raytracing image format ...\n");
++$test;
testRead( 'input.mtv',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Xv's visual schnauzer format. ...\n");
++$test;
testRead( 'input_p7.p7',
  'd6451845a386a8ec189f1bb3b96ed599f1d69cf68404168616a1868afa5b2585',
  '5adfa668a99c5b9112d27fd0d9de55a0a49ea35e62d6f3ae9c5348d25a7a62b1' );

print("NULL (white image) ...\n");
++$test;
testRead( 'NULL:white', 'd673295f1b96d192003062a86c293d796d7baf02ccebc84cf1b419c22a6137c0',
  'b57ca51881682b98a084a42beb8325895fdbc90d6f874ad8cdea7858d4907845');

print("NULL (black image) ...\n");
++$test;
testRead( 'NULL:black',
  '8851aa72e74300d3a50cb246c69d03a7d6c7cc53be91d894e232053f1b859513');

print("NULL (DarkOrange image) ...\n");
++$test;
testRead( 'NULL:DarkOrange',
  '275ae63055761e40358752e8ece96615b7e03b431c224e24d3e46b65e8353921');

print("Portable bitmap format (black and white), ASCII format ...\n");
++$test;
testRead( 'input_p1.pbm',
  '62311570ee3d0c0fe50e9aadd7922d6c836839c70d092b1136880c9411e9b39e');

print("Portable bitmap format (black and white), binary format ...\n");
++$test;
testRead( 'input_p4.pbm',
  '62311570ee3d0c0fe50e9aadd7922d6c836839c70d092b1136880c9411e9b39e');

print("ZSoft IBM PC Paintbrush file ...\n");
++$test;
testRead( 'input.pcx',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Portable graymap format (gray scale), ASCII format ...\n");
++$test;
testRead( 'input_p2.pgm',
  '64add5cd91de531222fd83c4b073bcda42f42fcf9985b409510e78aedf0c68d1');

print("Portable graymap format (gray scale), binary format ...\n");
++$test;
testRead( 'input_p5.pgm',
  '64add5cd91de531222fd83c4b073bcda42f42fcf9985b409510e78aedf0c68d1');

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testRead( 'input.pict',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Alias/Wavefront RLE image format ...\n");
++$test;
testRead( 'input.rle',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Portable pixmap format (color), ASCII format ...\n");
++$test;
testRead( 'input_p3.ppm',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Portable pixmap format (color), binary format ...\n");
++$test;
testRead( 'input_p6.ppm',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Adobe Photoshop bitmap file ...\n");
++$test;
testRead( 'input.psd',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Irix RGB image file ...\n");
++$test;
testRead( 'input.sgi',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("SUN 1-bit Rasterfile ...\n");
++$test;
testRead( 'input.im1',
  '36093b62470f3c0b85f2854b654df378f3ba1a3f74a09af9d73ec9b7177cbcd9');

print("SUN 8-bit Rasterfile ...\n");
++$test;
testRead( 'input.im8',
  'e8063197764e173ac9f4c903c46fe0b2d1360f522bff1dea49f2e049523d71c2');

print("SUN TrueColor Rasterfile ...\n");
++$test;
testRead( 'sun:input.im24',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Truevision Targa image file ...\n");
++$test;
testRead( 'input.tga',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("PSX TIM file ...\n");
++$test;
testRead( 'input.tim',
  '3f19e474bcc4d9cee5d9063e9ad74bc1ae2577b052299d85358f5ba063c561d9');

print("Khoros Visualization image file ...\n");
++$test;
testRead( 'input.viff',
  '8e900b95196d534527e9f09cf5153f41b1b5978f5de51e7dad5012f174c14eba');

print("WBMP (Wireless Bitmap (level 0) image) ...\n");
++$test;
testRead( 'input.wbmp',
  '02f153e943159ecf2a50316d7a649eca74c2f52082617249531e5ba05cc89bbd');

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testRead( 'input.xbm',
  '36093b62470f3c0b85f2854b654df378f3ba1a3f74a09af9d73ec9b7177cbcd9');

print("XC: Constant image of X server color ...\n");
++$test;
testRead( 'xc:black',
  '8851aa72e74300d3a50cb246c69d03a7d6c7cc53be91d894e232053f1b859513');

print("X Windows system pixmap file (color) ...\n");
++$test;
testRead( 'input.xpm',
  'e8063197764e173ac9f4c903c46fe0b2d1360f522bff1dea49f2e049523d71c2');

print("X Windows system window dump file (color) ...\n");
++$test;
testRead( 'input.xwd',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("TILE (Tile image with a texture) ...\n");
# This is an internal generated format
# We will tile using the default image and a MIFF file
#
++$test;
testReadSized( 'TILE:input.miff',
  '140x92',
  8,
  '6f9627cb26c0c9ef2751600e18f461a99b898c8ace930b31c9efb048eb0b0e10');


print("CMYK format ...\n");
++$test;
testReadSized( 'input_70x46.cmyk',
   '70x46',
   8,
   '8934892c866bded94a5568f03d02f36e48a8244c2e05cad1cf7780de792ab856');

print("GRAY format ...\n");
++$test;
testReadSized( 'input_70x46.gray',
   '70x46',
   8,
   '7ecdf2c1ae36cbf8a82c336c854b47da8683006d27e5ae5049c98d111dd927d2' );

print("RGB format ...\n");
++$test;
testReadSized( 'input_70x46.rgb',
   '70x46',
   8,
   'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca' );

print("RGBA format ...\n");
++$test;
testReadSized( 'input_70x46.rgba',
   '70x46',
   8,
   'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca' );


print("UYVY format ...\n");
++$test;
testReadSized( 'input_70x46.uyvy',
   '70x46',
   0,
   '64fcd72ae4ef47be861381e24c5f436f681f9bc33aa43c59536a9893af2b7281',
   'a478a8ce322c110281c2e5a388a81b063dbdb726b09222c9bf3a32f174d88dfc' );
