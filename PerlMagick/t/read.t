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
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testRead( 'input.bmp',
  '9068e42e75a84b5b170673fc8dc1dc861adb0a29340b48a48fe44f5d168b1da2');

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testRead( 'bmp:input.bmp24',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("ZSoft IBM PC multi-page Paintbrush file ...\n");
++$test;
testRead( 'input.dcx',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testRead( 'input.dib',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Flexible Image Transport System ...\n");
++$test;
testRead( 'input.fits',
  'dae103b644eea34734b5a4d61d559dc40ae607f1a89bf92c8a019c1c637ab7ea' );

print("CompuServe graphics interchange format ...\n");
++$test;
testRead( 'input.gif',
  'a5dfb0e883c637131dfad3124b7b5d372989a86bbd6ed7065945e2fe164525be');

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testRead( 'input.gif87',
  'a5dfb0e883c637131dfad3124b7b5d372989a86bbd6ed7065945e2fe164525be');

print("Gradient (gradual passing from one shade to another) ...\n");
++$test;
testRead( 'gradient:red-blue',
  '0b213daf9048c6039820d1b1c1f1a50fd2ddf1401b1b6e05beac807785ae840f',
  '4bc1c8bd802be60bbf9633b2e058efb3a2815fcf4195f1b07e319df29f2efafc' );

print("GRANITE (granite texture) ...\n");
++$test;
testRead( 'granite:',
  '5826aae4d39a64b922fd6d1feb989058581a435f377181a524085f8bcac8e905');

print("Microsoft icon ...\n");
++$test;
testRead( 'input.ico',
  '606e8fd3ea17caa6bdd7d6778fd7ad0150e687fb7dd9e2d2ac306f8dfcddf347' );

print("Magick image file format ...\n");
++$test;
testRead( 'input.miff',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("MTV Raytracing image format ...\n");
++$test;
testRead( 'input.mtv',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Xv's visual schnauzer format. ...\n");
++$test;
testRead( 'input_p7.p7',
  'dcc32088dcd6f77a4027069d426dde61d8f2c090feb9df2051f756bb2891f696',
  '5adfa668a99c5b9112d27fd0d9de55a0a49ea35e62d6f3ae9c5348d25a7a62b1' );

print("NULL (white image) ...\n");
++$test;
testRead( 'NULL:white', '5ccce948576fbbb7a2a47426b6cc269f7710e00355f8ecc57b1d81f9d05b1e0d',
  'b57ca51881682b98a084a42beb8325895fdbc90d6f874ad8cdea7858d4907845');

print("NULL (black image) ...\n");
++$test;
testRead( 'NULL:black',
  '8851aa72e74300d3a50cb246c69d03a7d6c7cc53be91d894e232053f1b859513');

print("NULL (DarkOrange image) ...\n");
++$test;
testRead( 'NULL:DarkOrange',
  '19616225e71810cc9487892d55a32c0db222c7604376baf4b8b44d8354d44c1d');

print("Portable bitmap format (black and white), ASCII format ...\n");
++$test;
testRead( 'input_p1.pbm',
  'b7ff35dedb72b47cd178219ed30a2380da9f900306ffbe79222827259382be54');

print("Portable bitmap format (black and white), binary format ...\n");
++$test;
testRead( 'input_p4.pbm',
  'b7ff35dedb72b47cd178219ed30a2380da9f900306ffbe79222827259382be54');

print("ZSoft IBM PC Paintbrush file ...\n");
++$test;
testRead( 'input.pcx',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Portable graymap format (gray scale), ASCII format ...\n");
++$test;
testRead( 'input_p2.pgm',
  'b31e1f69a00cfcbaaed0e4a0104a13e60b80d10a3e2b765a2f19fe885a1138d5');

print("Portable graymap format (gray scale), binary format ...\n");
++$test;
testRead( 'input_p5.pgm',
  'b31e1f69a00cfcbaaed0e4a0104a13e60b80d10a3e2b765a2f19fe885a1138d5');

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testRead( 'input.pict',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Alias/Wavefront RLE image format ...\n");
++$test;
testRead( 'input.rle',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Portable pixmap format (color), ASCII format ...\n");
++$test;
testRead( 'input_p3.ppm',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Portable pixmap format (color), binary format ...\n");
++$test;
testRead( 'input_p6.ppm',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Adobe Photoshop bitmap file ...\n");
++$test;
testRead( 'input.psd',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Irix RGB image file ...\n");
++$test;
testRead( 'input.sgi',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("SUN 1-bit Rasterfile ...\n");
++$test;
testRead( 'input.im1',
  '1677a897f0c936df13a0d78b851066507b7a481f08dc25223e674bf35a3694ca');

print("SUN 8-bit Rasterfile ...\n");
++$test;
testRead( 'input.im8',
  'a5dfb0e883c637131dfad3124b7b5d372989a86bbd6ed7065945e2fe164525be');

print("SUN TrueColor Rasterfile ...\n");
++$test;
testRead( 'sun:input.im24',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Truevision Targa image file ...\n");
++$test;
testRead( 'input.tga',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("PSX TIM file ...\n");
++$test;
testRead( 'input.tim',
  'e2018af831a8f31f5736c72700b99b4a37396b33a299da17c373c36560aaa6f2');

print("Khoros Visualization image file ...\n");
++$test;
testRead( 'input.viff',
  '027aa3d382066940eebe76a772ad34cfe0c82c411d0bcc203384f8cdbe190fc1');

print("WBMP (Wireless Bitmap (level 0) image) ...\n");
++$test;
testRead( 'input.wbmp',
  'a2f2141d597af36e551c196a208b64907621389fdcbed3a5ed0144bdb97795fa');

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testRead( 'input.xbm',
  '1677a897f0c936df13a0d78b851066507b7a481f08dc25223e674bf35a3694ca');

print("XC: Constant image of X server color ...\n");
++$test;
testRead( 'xc:black',
  '8851aa72e74300d3a50cb246c69d03a7d6c7cc53be91d894e232053f1b859513');

print("X Windows system pixmap file (color) ...\n");
++$test;
testRead( 'input.xpm',
  'a5dfb0e883c637131dfad3124b7b5d372989a86bbd6ed7065945e2fe164525be');

print("X Windows system window dump file (color) ...\n");
++$test;
testRead( 'input.xwd',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("TILE (Tile image with a texture) ...\n");
# This is an internal generated format
# We will tile using the default image and a MIFF file
#
++$test;
testReadSized( 'TILE:input.miff',
  '140x92',
  8,
  'd7407b16ca79e74a15814fcc32e8b36ab48de6452c2dafad1849576da599e7da');


print("CMYK format ...\n");
++$test;
testReadSized( 'input_70x46.cmyk',
   '70x46',
   8,
   'a50e140a216b6d71153485e4d1c7b6be734b69f4656f9c32f99e91ef560dbba6');

print("GRAY format ...\n");
++$test;
testReadSized( 'input_70x46.gray',
   '70x46',
   8,
   'dae103b644eea34734b5a4d61d559dc40ae607f1a89bf92c8a019c1c637ab7ea' );

print("RGB format ...\n");
++$test;
testReadSized( 'input_70x46.rgb',
   '70x46',
   8,
   'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c' );

print("RGBA format ...\n");
++$test;
testReadSized( 'input_70x46.rgba',
   '70x46',
   8,
   'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c' );


print("UYVY format ...\n");
++$test;
testReadSized( 'input_70x46.uyvy',
   '70x46',
   0,
   'a75e0539c3780133773570333b5ce1f29586c2a154f0aa7efeb7e3fd78d031d1',
   'a478a8ce322c110281c2e5a388a81b063dbdb726b09222c9bf3a32f174d88dfc' );
