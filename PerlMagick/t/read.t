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
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testRead( 'input.bmp',
  '5083e062c7e4d3d462944bc1e89c4f491bf1f0241352e0a93988c8854c261fdb');

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testRead( 'bmp:input.bmp24',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("ZSoft IBM PC multi-page Paintbrush file ...\n");
++$test;
testRead( 'input.dcx',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testRead( 'input.dib',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Flexible Image Transport System ...\n");
++$test;
testRead( 'input.fits',
  '0e54b24352da9f7d0966c9b882988b9124b6b7fd7493ff266f4c03382b94a42a' );

print("CompuServe graphics interchange format ...\n");
++$test;
testRead( 'input.gif',
  'f84cb6b5817ea2f6f313c2bdd90c94af0d0e7fb144528994050b355f8d5e6a94');

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testRead( 'input.gif87',
  'f84cb6b5817ea2f6f313c2bdd90c94af0d0e7fb144528994050b355f8d5e6a94');

print("Gradient (gradual passing from one shade to another) ...\n");
++$test;
testRead( 'gradient:red-blue',
  '7f708830d2766a485e07526173008906a7154ac50088d2ac58be41a0bbf987c1',
  '455f985c3755024be2a0165774166d8dcee66edb9b5381b9569605cb29b102d5' );

print("GRANITE (granite texture) ...\n");
++$test;
testRead( 'granite:',
  '6e7317a9cb3abb1fe090d2d43c7645be455b6116ef8e93ca193d1df421447602');

print("Microsoft icon ...\n");
++$test;
testRead( 'input.ico',
  'a8b1aae465606b8273caf1035f95b7c1262bcb1f72fcb73bfe2d3640ff6b45ef' );

print("Magick image file format ...\n");
++$test;
testRead( 'input.miff',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("MTV Raytracing image format ...\n");
++$test;
testRead( 'input.mtv',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Xv's visual schnauzer format. ...\n");
++$test;
testReadCompare('input_p7.p7', 'reference/read/input_p7.miff', q//,
                0.000005, 0.00005);

print("NULL (white image) ...\n");
++$test;
testRead( 'NULL:white', '3c907c7fc6edfcf15767c67e22c25cf9f907ba8ee59811aaae79dafd7c8ff198',
  'b57ca51881682b98a084a42beb8325895fdbc90d6f874ad8cdea7858d4907845');

print("NULL (black image) ...\n");
++$test;
testRead( 'NULL:black',
  '8851aa72e74300d3a50cb246c69d03a7d6c7cc53be91d894e232053f1b859513');

print("NULL (DarkOrange image) ...\n");
++$test;
testRead( 'NULL:DarkOrange',
  '295732fb13da5439fa0f08fc1d1a27d9a24f28c6f5c86d6dfb07ac73bd4d10cd');

print("Portable bitmap format (black and white), ASCII format ...\n");
++$test;
testRead( 'input_p1.pbm',
  '2b48bcf7d93cc31a8deb3026d872f1bb0a300b0f4e177423e5301adc638179e4');

print("Portable bitmap format (black and white), binary format ...\n");
++$test;
testRead( 'input_p4.pbm',
  '2b48bcf7d93cc31a8deb3026d872f1bb0a300b0f4e177423e5301adc638179e4');

print("ZSoft IBM PC Paintbrush file ...\n");
++$test;
testRead( 'input.pcx',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Portable graymap format (gray scale), ASCII format ...\n");
++$test;
testRead( 'input_p2.pgm',
  '04c5604964838e47aba7a355efee266a4d0f1b90f672ff56863440e5e5a29761');

print("Portable graymap format (gray scale), binary format ...\n");
++$test;
testRead( 'input_p5.pgm',
  '04c5604964838e47aba7a355efee266a4d0f1b90f672ff56863440e5e5a29761');

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testRead( 'input.pict',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Alias/Wavefront RLE image format ...\n");
++$test;
testRead( 'input.rle',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Portable pixmap format (color), ASCII format ...\n");
++$test;
testRead( 'input_p3.ppm',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Portable pixmap format (color), binary format ...\n");
++$test;
testRead( 'input_p6.ppm',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Adobe Photoshop bitmap file ...\n");
++$test;
testRead( 'input.psd',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Irix RGB image file ...\n");
++$test;
testRead( 'input.sgi',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("SUN 1-bit Rasterfile ...\n");
++$test;
testRead( 'input.im1',
  '615fa1d8bae486118b3733c1dba4e2a225fc1f4f8ff9441bcb7c3293753e4da1');

print("SUN 8-bit Rasterfile ...\n");
++$test;
testRead( 'input.im8',
  'f84cb6b5817ea2f6f313c2bdd90c94af0d0e7fb144528994050b355f8d5e6a94');

print("SUN TrueColor Rasterfile ...\n");
++$test;
testRead( 'sun:input.im24',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Truevision Targa image file ...\n");
++$test;
testRead( 'input.tga',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("PSX TIM file ...\n");
++$test;
testRead( 'input.tim',
  '403a78aa326ce7fae09e350bc36344b3263e9aede1a16bb8ea6e9b923b27b374');

print("Khoros Visualization image file ...\n");
++$test;
testRead( 'input.viff',
  '9c01e510e4aa7c369895db51389ae917a0d4f9004ffb4edf81e5e2302cab98a3');

print("WBMP (Wireless Bitmap (level 0) image) ...\n");
++$test;
testRead( 'input.wbmp',
  'ccde39dba836a36bbc076e7e6f20f06b034dab0eb2613b350b17c2759533b2ca');

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testRead( 'input.xbm',
  '615fa1d8bae486118b3733c1dba4e2a225fc1f4f8ff9441bcb7c3293753e4da1');

print("XC: Constant image of X server color ...\n");
++$test;
testRead( 'xc:black',
  '8851aa72e74300d3a50cb246c69d03a7d6c7cc53be91d894e232053f1b859513');

print("X Windows system pixmap file (color) ...\n");
++$test;
testRead( 'input.xpm',
  'f84cb6b5817ea2f6f313c2bdd90c94af0d0e7fb144528994050b355f8d5e6a94');

print("X Windows system window dump file (color) ...\n");
++$test;
testRead( 'input.xwd',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("TILE (Tile image with a texture) ...\n");
# This is an internal generated format
# We will tile using the default image and a MIFF file
#
++$test;
testReadSized( 'TILE:input.miff',
  '140x92',
  8,
  'e1384a6f9f75d4a86f6291c47171a9039972718d9e5092294746cca57fd7b02e');


print("CMYK format ...\n");
++$test;
testReadSized( 'input_70x46.cmyk',
   '70x46',
   8,
   '1a1b99a35b962941e2a1cfc01e34907539f6d6ddc4a8df09fc024ef58e16d4e2');

print("GRAY format ...\n");
++$test;
testReadSized( 'input_70x46.gray',
   '70x46',
   8,
   '0e54b24352da9f7d0966c9b882988b9124b6b7fd7493ff266f4c03382b94a42a' );

print("RGB format ...\n");
++$test;
testReadSized( 'input_70x46.rgb',
   '70x46',
   8,
   '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f' );

print("RGBA format ...\n");
++$test;
testReadSized( 'input_70x46.rgba',
   '70x46',
   8,
   '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f' );


print("UYVY format ...\n");
++$test;
testReadSized( 'input_70x46.uyvy',
   '70x46',
   0,
   'd88b2fcee63384f58ad68eb114b10dab96957f9a56a09738cc90e85ecd028c16',
   '03d13172c954470b8da3c817afb2b4f1c1c365dafefb86227d44d40240f95393' );
