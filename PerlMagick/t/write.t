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
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testReadWrite( 'BMP:input.bmp',
  'BMP:output.bmp',
  q//,
  '5083e062c7e4d3d462944bc1e89c4f491bf1f0241352e0a93988c8854c261fdb');

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testReadWrite( 'BMP:input.bmp24',
  'BMP:output.bmp24',
  q//,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');


print("ZSoft IBM PC multi-page Paintbrush file ...\n");
++$test;
testReadWrite( 'DCX:input.dcx',
  'DCX:output.dcx',
  q//,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Microsoft Windows 3.X DIB file ...\n");
++$test;
testReadWrite( 'DIB:input.dib',
  'DIB:output.dib',
  q//,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Flexible Image Transport System ...\n");
++$test;
testReadWrite( 'FITS:input.fits',
  'FITS:output.fits',
  q//,
  '0e54b24352da9f7d0966c9b882988b9124b6b7fd7493ff266f4c03382b94a42a' );

print("CompuServe graphics interchange format ...\n");
++$test;
testReadWrite( 'GIF:input.gif',
  'GIF:output.gif',
  q//,
  'f84cb6b5817ea2f6f313c2bdd90c94af0d0e7fb144528994050b355f8d5e6a94');

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testReadWrite( 'GIF87:input.gif87',
  'GIF87:output.gif87',
  q//,
  'f84cb6b5817ea2f6f313c2bdd90c94af0d0e7fb144528994050b355f8d5e6a94');

print("Magick image file format ...\n");
++$test;
testReadWrite( 'MIFF:input.miff',
  'MIFF:output.miff',
  q//,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("MTV Raytracing image format ...\n");
++$test;
testReadWrite( 'MTV:input.mtv',
  'MTV:output.mtv',
  q//,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Xv's visual schnauzer format ...\n");
++$test;
testReadWriteCompare( 'input.miff', 'P7:output_p7.p7', 'reference/write/output_p7.miff',
                      q/depth=>8/, q/dither=>0, depth=>8/, 0.000005, 0.00011);

print("Portable bitmap format (black and white), ASCII format ...\n");
++$test;
testReadWrite( 'PBM:input_p1.pbm',
  'PBM:output_p1.pbm',
  q/compression=>'None'/,
  '2b48bcf7d93cc31a8deb3026d872f1bb0a300b0f4e177423e5301adc638179e4');

print("Portable bitmap format (black and white), binary format ...\n");
++$test;
testReadWrite( 'PBM:input_p4.pbm',
  'PBM:output_p4.pbm',
  q//,
  '2b48bcf7d93cc31a8deb3026d872f1bb0a300b0f4e177423e5301adc638179e4');

print("ZSoft IBM PC Paintbrush file ...\n");
++$test;
testReadWrite( 'PCX:input.pcx',
  'PCX:output.pcx',
  q//,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Portable graymap format (gray scale), ASCII format ...\n");
++$test;
testReadWrite( 'PGM:input_p2.pgm',
  'PGM:output_p2.pgm',
  q/compression=>'None'/,
  '04c5604964838e47aba7a355efee266a4d0f1b90f672ff56863440e5e5a29761');

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testReadWrite( 'PICT:input.pict',
  'PICT:output.pict',
  q//,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Portable pixmap format (color), ASCII format ...\n");
++$test;
testReadWrite( 'PPM:input_p3.ppm',
  'PPM:output_p3.ppm',
  q/compression=>'None'/,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Portable graymap format (gray scale), binary format ...\n");
++$test;
testReadWrite( 'PGM:input_p5.pgm',
  'PGM:output_p5.pgm',
  q//,
  '04c5604964838e47aba7a355efee266a4d0f1b90f672ff56863440e5e5a29761');

print("Portable pixmap format (color), binary format ...\n");
++$test;
testReadWrite( 'PPM:input_p6.ppm',
  'PPM:output_p6.ppm',
  q//,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Adobe Photoshop bitmap file ...\n");
++$test;
testReadWrite( 'PSD:input.psd',
  'PSD:output.psd',
  q//,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f' );

print("Irix RGB image file ...\n");
++$test;
testReadWrite( 'SGI:input.sgi',
  'SGI:output.sgi',
  q//,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("SUN 1-bit Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im1',
  'SUN:output.im1',
  q//,
  '615fa1d8bae486118b3733c1dba4e2a225fc1f4f8ff9441bcb7c3293753e4da1');

print("SUN 8-bit Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im8',
  'SUN:output.im8',
  q//,
  'f84cb6b5817ea2f6f313c2bdd90c94af0d0e7fb144528994050b355f8d5e6a94');

print("SUN True-Color Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im24',
  'SUN:output.im24',
  q//,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Truevision Targa image file ...\n");
++$test;
testReadWrite( 'TGA:input.tga',
  'TGA:output.tga',
  q//,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("Khoros Visualization image file ...\n");
++$test;
testReadWrite( 'VIFF:input.viff',
  'VIFF:output.viff',
  q//,
  '9c01e510e4aa7c369895db51389ae917a0d4f9004ffb4edf81e5e2302cab98a3');

print("WBMP (Wireless Bitmap (level 0) image) ...\n");
++$test;
testReadWrite( 'WBMP:input.wbmp',
  'WBMP:output.wbmp',
  q//,
  'ccde39dba836a36bbc076e7e6f20f06b034dab0eb2613b350b17c2759533b2ca');

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testReadWrite( 'XBM:input.xbm',
  'XBM:output.xbm',
  q//,
  '615fa1d8bae486118b3733c1dba4e2a225fc1f4f8ff9441bcb7c3293753e4da1');

print("X Windows system pixmap file (color) ...\n");
++$test;
testReadWrite( 'XPM:input.xpm',
  'XPM:output.xpm',
  q//,
  'f84cb6b5817ea2f6f313c2bdd90c94af0d0e7fb144528994050b355f8d5e6a94');


print("X Windows system window dump file (color) ...\n");
++$test;
testReadWrite( 'XWD:input.xwd',
  'XWD:output.xwd',
  q//,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

print("CMYK format ...\n");
++$test;
testReadWriteSized( 'CMYK:input_70x46.cmyk',
  'CMYK:output_70x46.cmyk',
  '70x46',
  8,
  q//,
  '397a9f8411f7bb065f4b6e56731734ddad8ea1a7d4c488745545e17dbe575903',
  '1a1b99a35b962941e2a1cfc01e34907539f6d6ddc4a8df09fc024ef58e16d4e2' );

print("GRAY format ...\n");
++$test;
testReadWriteSized( 'GRAY:input_70x46.gray',
  'GRAY:output_70x46.gray',
  '70x46',
  8,
  q//,
  '0e54b24352da9f7d0966c9b882988b9124b6b7fd7493ff266f4c03382b94a42a' );

print("RGB format ...\n");
++$test;
testReadWriteSized( 'RGB:input_70x46.rgb',
  'RGB:output_70x46.rgb',
  '70x46',
  8,
  q//,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f' );


print("RGBA format ...\n");
++$test;
testReadWriteSized( 'RGBA:input_70x46.rgba',
  'RGBA:output_70x46.rgba',
  '70x46',
  8,
  q//,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f' );

1;
