#!/usr/local/bin/perl
#
# Test writing formats supported directly by ImageMagick
#
# Currently supported tests are for formats that ImageMagick
# knows how to both read and write.
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.

BEGIN { $| = 1; $test=1; print "1..34\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

print("AVS X image file ...\n");
testReadWrite( 'AVS:input.avs',
  'AVS:output.avs',
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testReadWrite( 'BMP:input.bmp',
  'BMP:output.bmp',
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testReadWrite( 'BMP:input.bmp24',
  'BMP:output.bmp24',
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');


print("ZSoft IBM PC multi-page Paintbrush file ...\n");
++$test;
testReadWrite( 'DCX:input.dcx',
  'DCX:output.dcx',
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testReadWrite( 'DIB:input.dib',
  'DIB:output.dib',
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Flexible Image Transport System ...\n");
++$test;
testReadWrite( 'FITS:input.fits',
  'FITS:output.fits',
  q//,
  '583db7cc54677dde2f8bb87e767bbcdc1bafabbf895d015558dc835050f9714f',
  '583db7cc54677dde2f8bb87e767bbcdc1bafabbf895d015558dc835050f9714f' );

print("CompuServe graphics interchange format ...\n");
++$test;
testReadWrite( 'GIF:input.gif',
  'GIF:output.gif',
  q//,
  '40c77daa2489a83a948b7a96ff9c9c48424ddc22326510017fbbce5c0702acaa');

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testReadWrite( 'GIF87:input.gif87',
  'GIF87:output.gif87',
  q//,
  '40c77daa2489a83a948b7a96ff9c9c48424ddc22326510017fbbce5c0702acaa');

print("Magick image file format ...\n");
++$test;
testReadWrite( 'MIFF:input.miff',
  'MIFF:output.miff',
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("MTV Raytracing image format ...\n");
++$test;
testReadWrite( 'MTV:input.mtv',
  'MTV:output.mtv',
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Xv's visual schnauzer format ...\n");
++$test;
testReadWrite( 'P7:input_p7.p7',
  'P7:output_p7.p7',
  q//,
  '656b4a500b4ff4ef8e04159b32abea7a1aa574fdfa960d244590464a5afd9e95',
  '11bcbd58063ca1f558272e48c1a831d361b28dc7bac5d8e752aaaba4d0d5b234' );


print("Portable bitmap format (black and white), ASCII format ...\n");
++$test;
testReadWrite( 'PBM:input_p1.pbm',
  'PBM:output_p1.pbm',
  q/compress=>'None'/,
  '88f1ac676d66c7a027328809a3c596ffe9eda97304ccfe41ffcca887150a6d42');


print("Portable bitmap format (black and white), binary format ...\n");
++$test;
testReadWrite( 'PBM:input_p4.pbm',
  'PBM:output_p4.pbm',
  q//,
  '88f1ac676d66c7a027328809a3c596ffe9eda97304ccfe41ffcca887150a6d42');

print("ZSoft IBM PC Paintbrush file ...\n");
++$test;
testReadWrite( 'PCX:input.pcx',
  'PCX:output.pcx',
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Portable graymap format (gray scale), ASCII format ...\n");
++$test;
testReadWrite( 'PGM:input_p2.pgm',
  'PGM:output_p2.pgm',
  q/compress=>'None'/,
  '7e1dbdbbdcf325d5f8225b2cce331d68366c74eacc7504143a1a6d6944901656');

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testReadWrite( 'PICT:input.pict',
  'PICT:output.pict',
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Portable pixmap format (color), ASCII format ...\n");
++$test;
testReadWrite( 'PPM:input_p3.ppm',
  'PPM:output_p3.ppm',
  q/compress=>'None'/,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Portable graymap format (gray scale), binary format ...\n");
++$test;
testReadWrite( 'PGM:input_p5.pgm',
  'PGM:output_p5.pgm',
  q//,
  '7e1dbdbbdcf325d5f8225b2cce331d68366c74eacc7504143a1a6d6944901656');

print("Portable pixmap format (color), binary format ...\n");
++$test;
testReadWrite( 'PPM:input_p6.ppm',
  'PPM:output_p6.ppm',
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Adobe Photoshop bitmap file ...\n");
++$test;
testReadWrite( 'PSD:input.psd',
  'PSD:output.psd',
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a' );

print("Irix RGB image file ...\n");
++$test;
testReadWrite( 'SGI:input.sgi',
  'SGI:output.sgi',
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');


print("SUN 1-bit Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im1',
  'SUN:output.im1',
  q//,
  'e81fc328ca75ad6a8430936a42d5d12eeac4d76d4a1a4a3b53e2c5d8f9ee9581');


print("SUN 8-bit Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im8',
  'SUN:output.im8',
  q//,
  '40c77daa2489a83a948b7a96ff9c9c48424ddc22326510017fbbce5c0702acaa');

print("SUN True-Color Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im24',
  'SUN:output.im24',
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Truevision Targa image file ...\n");
++$test;
testReadWrite( 'TGA:input.tga',
  'TGA:output.tga',
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

print("Khoros Visualization image file ...\n");
++$test;
testReadWrite( 'VIFF:input.viff',
  'VIFF:output.viff',
  q//,
  'a145899b463ca4fcc5da184fc5c8e17a7f2885920b407eda37f5d11270aafb43');

print("WBMP (Wireless Bitmap (level 0) image) ...\n");
++$test;
testReadWrite( 'WBMP:input.wbmp',
  'WBMP:output.wbmp',
  q//,
  '21365de2b0d692a79e0cdbe5b455ebadc31cd58e44ab256f868740354300801b');

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testReadWrite( 'XBM:input.xbm',
  'XBM:output.xbm',
  q//,
  'e81fc328ca75ad6a8430936a42d5d12eeac4d76d4a1a4a3b53e2c5d8f9ee9581');

print("X Windows system pixmap file (color) ...\n");
++$test;
testReadWrite( 'XPM:input.xpm',
  'XPM:output.xpm',
  q//,
  '40c77daa2489a83a948b7a96ff9c9c48424ddc22326510017fbbce5c0702acaa');


print("X Windows system window dump file (color) ...\n");
++$test;
testReadWrite( 'XWD:input.xwd',
  'XWD:output.xwd',
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');


print("CMYK format ...\n");
++$test;
testReadWriteSized( 'CMYK:input_70x46.cmyk',
  'CMYK:output_70x46.cmyk',
  '70x46',
  8,
  q//,
  '397a9f8411f7bb065f4b6e56731734ddad8ea1a7d4c488745545e17dbe575903',
  '04da6486cc4ddf7435994e8bac2804fc0ed1630410e963ba78b14b2d7943ea1d' );


print("GRAY format ...\n");
++$test;
testReadWriteSized( 'GRAY:input_70x46.gray',
  'GRAY:output_70x46.gray',
  '70x46',
  8,
  q//,
  '583db7cc54677dde2f8bb87e767bbcdc1bafabbf895d015558dc835050f9714f' );

print("RGB format ...\n");
++$test;
testReadWriteSized( 'RGB:input_70x46.rgb',
  'RGB:output_70x46.rgb',
  '70x46',
  8,
  q//,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a' );


print("RGBA format ...\n");
++$test;
testReadWriteSized( 'RGBA:input_70x46.rgba',
  'RGBA:output_70x46.rgba',
  '70x46',
  8,
  q//,
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1' );

1;
