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
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testRead( 'input.bmp',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testRead( 'bmp:input.bmp24',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("ZSoft IBM PC multi-page Paintbrush file ...\n");
++$test;
testRead( 'input.dcx',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testRead( 'input.dib',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("Flexible Image Transport System ...\n");
++$test;
testRead( 'input.fits',
  '49a8c7d81e9717e3ef5646bc5fc2c93a1cfc0cffe46a32424c6fa46bdf41a6f7' );

print("CompuServe graphics interchange format ...\n");
++$test;
testRead( 'input.gif',
  'a666c9982869ea440170dcb7fbf6cff56acaced9550341694e1e10c739c92b20');

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testRead( 'input.gif87',
  'a666c9982869ea440170dcb7fbf6cff56acaced9550341694e1e10c739c92b20');

print("Gradient (gradual passing from one shade to another) ...\n");
#
++$test;
testRead( 'gradient:red-blue',
  'b59b8d1863f6d7c5f70a870d13b4c93f0f0ce78ef64634d774ef3fea28becda3',
  '1b36b81f78e0355e6aef8d4573959abad03938b86f4ed51dfcf6733ba64745b7' );

print("GRANITE (granite texture) ...\n");
#
++$test;
testRead( 'granite:',
  'b2dce9b61086d997b10c7f8c3762608b47219d084ce8ae3cfeb443faca05d376');

print("Microsoft icon ...\n");
++$test;
testRead( 'input.ico',
  '09ac557be3f55027130cbb8a9e4d0449f468ffe0384706791631e4150a2564f9' );

print("Magick image file format ...\n");
++$test;
testRead( 'input.miff',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("MTV Raytracing image format ...\n");
++$test;
testRead( 'input.mtv',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("Xv's visual schnauzer format. ...\n");
++$test;
testRead( 'input_p7.p7',
  '301a99abfa2c6a362734dc2aef62ba89275a26af7d2826ebdb1d95f74c2bb7cb',
  '5adfa668a99c5b9112d27fd0d9de55a0a49ea35e62d6f3ae9c5348d25a7a62b1' );

print("NULL (white image) ...\n");
++$test;
testRead( 'NULL:white', '141d68e6b0288a9d1c9c05e35bf8d7483238487290445578d77c14a2caeba35f',
  'b57ca51881682b98a084a42beb8325895fdbc90d6f874ad8cdea7858d4907845');

print("NULL (black image) ...\n");
++$test;
testRead( 'NULL:black',
  '5db60a10594f7e38eb165d74a44c637128fd3ad2378de23432fece2f97e6a229');

print("NULL (DarkOrange image) ...\n");
++$test;
testRead( 'NULL:DarkOrange',
  '5356ef2a65782ead5e97463edca96b7152db4c29695f68a0bb9d1a21a5abc832');

print("Portable bitmap format (black and white), ASCII format ...\n");
++$test;
testRead( 'input_p1.pbm',
  'e09c8eb1ccbf32e3a4062bad04b96dbc9209bc7897b0a63ac8dc2c8e0ccc3d92');

print("Portable bitmap format (black and white), binary format ...\n");
++$test;
testRead( 'input_p4.pbm',
  'e09c8eb1ccbf32e3a4062bad04b96dbc9209bc7897b0a63ac8dc2c8e0ccc3d92');

print("ZSoft IBM PC Paintbrush file ...\n");
++$test;
testRead( 'input.pcx',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("Portable graymap format (gray scale), ASCII format ...\n");
++$test;
testRead( 'input_p2.pgm',
  '3e549a63afb5d2c4db5b442aca8bb973b6fcd1190bb6b1d45a61507e3bf4097e');

print("Portable graymap format (gray scale), binary format ...\n");
++$test;
testRead( 'input_p5.pgm',
  '3e549a63afb5d2c4db5b442aca8bb973b6fcd1190bb6b1d45a61507e3bf4097e');

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testRead( 'input.pict',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("Alias/Wavefront RLE image format ...\n");
++$test;
testRead( 'input.rle',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("Portable pixmap format (color), ASCII format ...\n");
++$test;
testRead( 'input_p3.ppm',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("Portable pixmap format (color), binary format ...\n");
++$test;
testRead( 'input_p6.ppm',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("Adobe Photoshop bitmap file ...\n");
++$test;
testRead( 'input.psd',
  'a58c9cc55b8ae969351efc5c023d8705306fac0449d49b7b5b15aba2bc2ba1de');

print("Irix RGB image file ...\n");
++$test;
testRead( 'input.sgi',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("SUN 1-bit Rasterfile ...\n");
++$test;
testRead( 'input.im1',
  '3487e83e2fc55c0129588353886a15caf63a3a426202c546b816bf2c8d58c80d');

print("SUN 8-bit Rasterfile ...\n");
++$test;
testRead( 'input.im8',
  'a666c9982869ea440170dcb7fbf6cff56acaced9550341694e1e10c739c92b20');

print("SUN TrueColor Rasterfile ...\n");
++$test;
testRead( 'sun:input.im24',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("Truevision Targa image file ...\n");
++$test;
testRead( 'input.tga',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("PSX TIM file ...\n");
++$test;
testRead( 'input.tim',
  '89c5c8f348df791815e752967fac346e3ca3030fdb71fdc739a26526df7b5791');

print("Khoros Visualization image file ...\n");
++$test;
testRead( 'input.viff',
  '0195c123e000026a28a1bb2a3a27d700ff9cc29ca6a2a547e9be04ed43feeeb7');

print("WBMP (Wireless Bitmap (level 0) image) ...\n");
++$test;
testRead( 'input.wbmp',
  '174494f95737ab0f7de848e914ce707093ddeb735bd5d87ac8f241a64abe3929');

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testRead( 'input.xbm',
  '3487e83e2fc55c0129588353886a15caf63a3a426202c546b816bf2c8d58c80d');

print("XC: Constant image of X server color ...\n");
++$test;
testRead( 'xc:black',
  '5db60a10594f7e38eb165d74a44c637128fd3ad2378de23432fece2f97e6a229');

print("X Windows system pixmap file (color) ...\n");
++$test;
testRead( 'input.xpm',
  'a666c9982869ea440170dcb7fbf6cff56acaced9550341694e1e10c739c92b20');

print("X Windows system window dump file (color) ...\n");
++$test;
testRead( 'input.xwd',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

print("TILE (Tile image with a texture) ...\n");
# This is an internal generated format
# We will tile using the default image and a MIFF file
#
++$test;
testReadSized( 'TILE:input.miff',
  '140x92',
  8,
  'f1c1bd749e77b9604026e9c29549c9723f56f5744885bbfa444a8a4b23a99f19');


print("CMYK format ...\n");
++$test;
testReadSized( 'input_70x46.cmyk',
   '70x46',
   8,
   '619c8c2a9f1684f1a18ff87524666303615310d19ef8c67d26401703c56e7476');

print("GRAY format ...\n");
++$test;
testReadSized( 'input_70x46.gray',
   '70x46',
   8,
   '49a8c7d81e9717e3ef5646bc5fc2c93a1cfc0cffe46a32424c6fa46bdf41a6f7' );

print("RGB format ...\n");
++$test;
testReadSized( 'input_70x46.rgb',
   '70x46',
   8,
   'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1' );

print("RGBA format ...\n");
++$test;
testReadSized( 'input_70x46.rgba',
   '70x46',
   8,
   'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1' );


print("UYVY format ...\n");
++$test;
testReadSized( 'input_70x46.uyvy',
   '70x46',
   0,
   '6224cda325acd0633c5d6dbfe7665ba068f5e7ffa3c9ed527f9d0b636d2f7562',
   'a478a8ce322c110281c2e5a388a81b063dbdb726b09222c9bf3a32f174d88dfc' );
