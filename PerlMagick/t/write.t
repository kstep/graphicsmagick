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
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testReadWrite( 'BMP:input.bmp',
  'BMP:output.bmp',
  q//,
  '9068e42e75a84b5b170673fc8dc1dc861adb0a29340b48a48fe44f5d168b1da2');

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testReadWrite( 'BMP:input.bmp24',
  'BMP:output.bmp24',
  q//,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');


print("ZSoft IBM PC multi-page Paintbrush file ...\n");
++$test;
testReadWrite( 'DCX:input.dcx',
  'DCX:output.dcx',
  q//,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Microsoft Windows 3.X DIB file ...\n");
++$test;
testReadWrite( 'DIB:input.dib',
  'DIB:output.dib',
  q//,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Flexible Image Transport System ...\n");
++$test;
testReadWrite( 'FITS:input.fits',
  'FITS:output.fits',
  q//,
  'dae103b644eea34734b5a4d61d559dc40ae607f1a89bf92c8a019c1c637ab7ea' );

print("CompuServe graphics interchange format ...\n");
++$test;
testReadWrite( 'GIF:input.gif',
  'GIF:output.gif',
  q//,
  'a5dfb0e883c637131dfad3124b7b5d372989a86bbd6ed7065945e2fe164525be');

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testReadWrite( 'GIF87:input.gif87',
  'GIF87:output.gif87',
  q//,
  'a5dfb0e883c637131dfad3124b7b5d372989a86bbd6ed7065945e2fe164525be');

print("Magick image file format ...\n");
++$test;
testReadWrite( 'MIFF:input.miff',
  'MIFF:output.miff',
  q//,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("MTV Raytracing image format ...\n");
++$test;
testReadWrite( 'MTV:input.mtv',
  'MTV:output.mtv',
  q//,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Xv's visual schnauzer format ...\n");
++$test;
testReadWrite( 'P7:input_p7.p7',
  'P7:output_p7.p7',
  q//,
  'c3f1b150ffa4ea550ec2473199f19ce7f8a06b68f9d9d5fa7cc007bd69ff5c1d',
  '83934fd2053634dece6c41a8565e134989839c5d4868d01b28a92996581e034f' );

print("Portable bitmap format (black and white), ASCII format ...\n");
++$test;
testReadWrite( 'PBM:input_p1.pbm',
  'PBM:output_p1.pbm',
  q/compression=>'None'/,
  'b7ff35dedb72b47cd178219ed30a2380da9f900306ffbe79222827259382be54');


print("Portable bitmap format (black and white), binary format ...\n");
++$test;
testReadWrite( 'PBM:input_p4.pbm',
  'PBM:output_p4.pbm',
  q//,
  'b7ff35dedb72b47cd178219ed30a2380da9f900306ffbe79222827259382be54');

print("ZSoft IBM PC Paintbrush file ...\n");
++$test;
testReadWrite( 'PCX:input.pcx',
  'PCX:output.pcx',
  q//,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Portable graymap format (gray scale), ASCII format ...\n");
++$test;
testReadWrite( 'PGM:input_p2.pgm',
  'PGM:output_p2.pgm',
  q/compression=>'None'/,
  'b31e1f69a00cfcbaaed0e4a0104a13e60b80d10a3e2b765a2f19fe885a1138d5');

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testReadWrite( 'PICT:input.pict',
  'PICT:output.pict',
  q//,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Portable pixmap format (color), ASCII format ...\n");
++$test;
testReadWrite( 'PPM:input_p3.ppm',
  'PPM:output_p3.ppm',
  q/compression=>'None'/,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Portable graymap format (gray scale), binary format ...\n");
++$test;
testReadWrite( 'PGM:input_p5.pgm',
  'PGM:output_p5.pgm',
  q//,
  'b31e1f69a00cfcbaaed0e4a0104a13e60b80d10a3e2b765a2f19fe885a1138d5');

print("Portable pixmap format (color), binary format ...\n");
++$test;
testReadWrite( 'PPM:input_p6.ppm',
  'PPM:output_p6.ppm',
  q//,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Adobe Photoshop bitmap file ...\n");
++$test;
testReadWrite( 'PSD:input.psd',
  'PSD:output.psd',
  q//,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c' );

print("Irix RGB image file ...\n");
++$test;
testReadWrite( 'SGI:input.sgi',
  'SGI:output.sgi',
  q//,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("SUN 1-bit Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im1',
  'SUN:output.im1',
  q//,
  '1677a897f0c936df13a0d78b851066507b7a481f08dc25223e674bf35a3694ca');

print("SUN 8-bit Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im8',
  'SUN:output.im8',
  q//,
  'a5dfb0e883c637131dfad3124b7b5d372989a86bbd6ed7065945e2fe164525be');

print("SUN True-Color Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im24',
  'SUN:output.im24',
  q//,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Truevision Targa image file ...\n");
++$test;
testReadWrite( 'TGA:input.tga',
  'TGA:output.tga',
  q//,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("Khoros Visualization image file ...\n");
++$test;
testReadWrite( 'VIFF:input.viff',
  'VIFF:output.viff',
  q//,
  '027aa3d382066940eebe76a772ad34cfe0c82c411d0bcc203384f8cdbe190fc1');

print("WBMP (Wireless Bitmap (level 0) image) ...\n");
++$test;
testReadWrite( 'WBMP:input.wbmp',
  'WBMP:output.wbmp',
  q//,
  'a2f2141d597af36e551c196a208b64907621389fdcbed3a5ed0144bdb97795fa');

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testReadWrite( 'XBM:input.xbm',
  'XBM:output.xbm',
  q//,
  '1677a897f0c936df13a0d78b851066507b7a481f08dc25223e674bf35a3694ca');

print("X Windows system pixmap file (color) ...\n");
++$test;
testReadWrite( 'XPM:input.xpm',
  'XPM:output.xpm',
  q//,
  'a5dfb0e883c637131dfad3124b7b5d372989a86bbd6ed7065945e2fe164525be');


print("X Windows system window dump file (color) ...\n");
++$test;
testReadWrite( 'XWD:input.xwd',
  'XWD:output.xwd',
  q//,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

print("CMYK format ...\n");
++$test;
testReadWriteSized( 'CMYK:input_70x46.cmyk',
  'CMYK:output_70x46.cmyk',
  '70x46',
  8,
  q//,
  '397a9f8411f7bb065f4b6e56731734ddad8ea1a7d4c488745545e17dbe575903',
  'a50e140a216b6d71153485e4d1c7b6be734b69f4656f9c32f99e91ef560dbba6' );

print("GRAY format ...\n");
++$test;
testReadWriteSized( 'GRAY:input_70x46.gray',
  'GRAY:output_70x46.gray',
  '70x46',
  8,
  q//,
  'dae103b644eea34734b5a4d61d559dc40ae607f1a89bf92c8a019c1c637ab7ea' );

print("RGB format ...\n");
++$test;
testReadWriteSized( 'RGB:input_70x46.rgb',
  'RGB:output_70x46.rgb',
  '70x46',
  8,
  q//,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c' );


print("RGBA format ...\n");
++$test;
testReadWriteSized( 'RGBA:input_70x46.rgba',
  'RGBA:output_70x46.rgba',
  '70x46',
  8,
  q//,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c' );

1;
