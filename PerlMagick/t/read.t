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
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testRead( 'input.bmp',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testRead( 'bmp:input.bmp24',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("ZSoft IBM PC multi-page Paintbrush file ...\n");
++$test;
testRead( 'input.dcx',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testRead( 'input.dib',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Flexible Image Transport System ...\n");
++$test;
testRead( 'input.fits',
  '583db7cc54677dde2f8bb87e767bbcdc1bafabbf895d015558dc835050f9714f',
  '583db7cc54677dde2f8bb87e767bbcdc1bafabbf895d015558dc835050f9714f' );

print("CompuServe graphics interchange format ...\n");
++$test;
testRead( 'input.gif',
  '40c77daa2489a83a948b7a96ff9c9c48424ddc22326510017fbbce5c0702acaa');

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testRead( 'input.gif87',
  '40c77daa2489a83a948b7a96ff9c9c48424ddc22326510017fbbce5c0702acaa');

print("Gradient (gradual passing from one shade to another) ...\n");
#
++$test;
testRead( 'gradient:red-blue',
  '50ad77d93de608bc61f31bc743713f67f82e09af467f6bd130f25203214c9940',
  '82743c736c9dd32298ed634b319629dada58e8c79122dd257cb853f62c25e6b3' );

print("GRANITE (granite texture) ...\n");
#
++$test;
testRead( 'granite:',
  '28f15c650899cfed812e151edd680b0de62a98369f5fac2619fb8562451e46e3');

print("Microsoft icon ...\n");
++$test;
testRead( 'input.ico',
  '09ac557be3f55027130cbb8a9e4d0449f468ffe0384706791631e4150a2564f9',
  '09ac557be3f55027130cbb8a9e4d0449f468ffe0384706791631e4150a2564f9' );

print("Magick image file format ...\n");
++$test;
testRead( 'input.miff',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("MTV Raytracing image format ...\n");
++$test;
testRead( 'input.mtv',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Xv's visual schnauzer format. ...\n");
++$test;
testRead( 'input_p7.p7',
  'c1f50b0a1e01fd851c23ab6d1bb135b8572b2a63bba1cc28fdf058e391e5da5f',
  '695d593f146dd8f1be86cf79a5148bfc700c6701f13d4eee2df76f1c94d7f17e' );

print("NULL (white image) ...\n");
++$test;
testRead( 'NULL:white', '0f01427889bc25ae6f2a049a6fd00f17',
  'b57ca51881682b98a084a42beb8325895fdbc90d6f874ad8cdea7858d4907845');

print("NULL (black image) ...\n");
++$test;
testRead( 'NULL:black',
  'a4ad65b2a1e475b76e6300cbb9a1f8b281f3c554eb2bf6d42b87378498e4ceee');

print("NULL (DarkOrange image) ...\n");
++$test;
testRead( 'NULL:DarkOrange',
  'a8ae7f39c715f185b6e76747ed679b87fea7c6dcf5c443c57fab3a3ffd43ea3f');

print("Portable bitmap format (black and white), ASCII format ...\n");
++$test;
testRead( 'input_p1.pbm',
  '88f1ac676d66c7a027328809a3c596ffe9eda97304ccfe41ffcca887150a6d42');


print("Portable bitmap format (black and white), binary format ...\n");
++$test;
testRead( 'input_p4.pbm',
  '88f1ac676d66c7a027328809a3c596ffe9eda97304ccfe41ffcca887150a6d42');

print("ZSoft IBM PC Paintbrush file ...\n");
++$test;
testRead( 'input.pcx',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Portable graymap format (gray scale), ASCII format ...\n");
++$test;
testRead( 'input_p2.pgm',
  '7e1dbdbbdcf325d5f8225b2cce331d68366c74eacc7504143a1a6d6944901656');

print("Portable graymap format (gray scale), binary format ...\n");
++$test;
testRead( 'input_p5.pgm',
  '7e1dbdbbdcf325d5f8225b2cce331d68366c74eacc7504143a1a6d6944901656');

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testRead( 'input.pict',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a',
  'ca09fc4171ad27787da936baf4cddf5a' );

print("Alias/Wavefront RLE image format ...\n");
++$test;
testRead( 'input.rle',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Portable pixmap format (color), ASCII format ...\n");
++$test;
testRead( 'input_p3.ppm',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Portable pixmap format (color), binary format ...\n");
++$test;
testRead( 'input_p6.ppm',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Adobe Photoshop bitmap file ...\n");
++$test;
testRead( 'input.psd',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a',
  '2b8f2d03bc34dcd45e24439039aedeee' );

print("Irix RGB image file ...\n");
++$test;
testRead( 'input.sgi',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("SUN 1-bit Rasterfile ...\n");
++$test;
testRead( 'input.im1',
  'e81fc328ca75ad6a8430936a42d5d12eeac4d76d4a1a4a3b53e2c5d8f9ee9581');

print("SUN 8-bit Rasterfile ...\n");
++$test;
testRead( 'input.im8',
  '40c77daa2489a83a948b7a96ff9c9c48424ddc22326510017fbbce5c0702acaa');

print("SUN TrueColor Rasterfile ...\n");
++$test;
testRead( 'sun:input.im24',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Truevision Targa image file ...\n");
++$test;
testRead( 'input.tga',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("PSX TIM file ...\n");
++$test;
testRead( 'input.tim',
  'c5543026315930cb641737a5fcae30c5105345b9c081b9a8a5e0a661fb4af4a6');

print("Khoros Visualization image file ...\n");
++$test;
testRead( 'input.viff',
  'a145899b463ca4fcc5da184fc5c8e17a7f2885920b407eda37f5d11270aafb43');

print("WBMP (Wireless Bitmap (level 0) image) ...\n");
++$test;
testRead( 'input.wbmp',
  '21365de2b0d692a79e0cdbe5b455ebadc31cd58e44ab256f868740354300801b');

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testRead( 'input.xbm',
  'e81fc328ca75ad6a8430936a42d5d12eeac4d76d4a1a4a3b53e2c5d8f9ee9581');

print("XC: Constant image of X server color ...\n");
++$test;
testRead( 'xc:black',
  'a4ad65b2a1e475b76e6300cbb9a1f8b281f3c554eb2bf6d42b87378498e4ceee');

print("X Windows system pixmap file (color) ...\n");
++$test;
testRead( 'input.xpm',
  '40c77daa2489a83a948b7a96ff9c9c48424ddc22326510017fbbce5c0702acaa');

print("X Windows system window dump file (color) ...\n");
++$test;
testRead( 'input.xwd',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');


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
   '583db7cc54677dde2f8bb87e767bbcdc1bafabbf895d015558dc835050f9714f' );

print("RGB format ...\n");
++$test;
testReadSized( 'input_70x46.rgb',
   '70x46',
   8,
   '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a' );

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
   '330151ca95674ab334dc9adf0957a0d45fe176293dabdc4e649efc478e7c93e8',
   '9fef5433f387ca90c297beab1ccad854265cb32fa24bc753ade260dd0b3776a8' );
