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
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Microsoft Windows bitmap image file ...\n");
++$test;
testReadWrite( 'BMP:input.bmp',
  'BMP:output.bmp',
  q//,
  '68a906a1f50c866db9d115b20bea1f008f68da87034caeaa78be40d41d529106');

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testReadWrite( 'BMP:input.bmp24',
  'BMP:output.bmp24',
  q//,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');


print("ZSoft IBM PC multi-page Paintbrush file ...\n");
++$test;
testReadWrite( 'DCX:input.dcx',
  'DCX:output.dcx',
  q//,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Microsoft Windows 3.X DIB file ...\n");
++$test;
testReadWrite( 'DIB:input.dib',
  'DIB:output.dib',
  q//,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Flexible Image Transport System ...\n");
++$test;
testReadWrite( 'FITS:input.fits',
  'FITS:output.fits',
  q//,
  '7ecdf2c1ae36cbf8a82c336c854b47da8683006d27e5ae5049c98d111dd927d2' );

print("CompuServe graphics interchange format ...\n");
++$test;
testReadWrite( 'GIF:input.gif',
  'GIF:output.gif',
  q//,
  'e8063197764e173ac9f4c903c46fe0b2d1360f522bff1dea49f2e049523d71c2');

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testReadWrite( 'GIF87:input.gif87',
  'GIF87:output.gif87',
  q//,
  'e8063197764e173ac9f4c903c46fe0b2d1360f522bff1dea49f2e049523d71c2');

print("Magick image file format ...\n");
++$test;
testReadWrite( 'MIFF:input.miff',
  'MIFF:output.miff',
  q//,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("MTV Raytracing image format ...\n");
++$test;
testReadWrite( 'MTV:input.mtv',
  'MTV:output.mtv',
  q//,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Xv's visual schnauzer format ...\n");
++$test;
testReadWrite( 'P7:input_p7.p7',
  'P7:output_p7.p7',
  q//,
  'f0fecd60e8ba349659787d8715c639ef8863ad6f8e49675ef0fea51063d7e6ba',
  '83934fd2053634dece6c41a8565e134989839c5d4868d01b28a92996581e034f' );

print("Portable bitmap format (black and white), ASCII format ...\n");
++$test;
testReadWrite( 'PBM:input_p1.pbm',
  'PBM:output_p1.pbm',
  q/compression=>'None'/,
  '62311570ee3d0c0fe50e9aadd7922d6c836839c70d092b1136880c9411e9b39e');


print("Portable bitmap format (black and white), binary format ...\n");
++$test;
testReadWrite( 'PBM:input_p4.pbm',
  'PBM:output_p4.pbm',
  q//,
  '62311570ee3d0c0fe50e9aadd7922d6c836839c70d092b1136880c9411e9b39e');

print("ZSoft IBM PC Paintbrush file ...\n");
++$test;
testReadWrite( 'PCX:input.pcx',
  'PCX:output.pcx',
  q//,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Portable graymap format (gray scale), ASCII format ...\n");
++$test;
testReadWrite( 'PGM:input_p2.pgm',
  'PGM:output_p2.pgm',
  q/compression=>'None'/,
  '64add5cd91de531222fd83c4b073bcda42f42fcf9985b409510e78aedf0c68d1');

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testReadWrite( 'PICT:input.pict',
  'PICT:output.pict',
  q//,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Portable pixmap format (color), ASCII format ...\n");
++$test;
testReadWrite( 'PPM:input_p3.ppm',
  'PPM:output_p3.ppm',
  q/compression=>'None'/,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Portable graymap format (gray scale), binary format ...\n");
++$test;
testReadWrite( 'PGM:input_p5.pgm',
  'PGM:output_p5.pgm',
  q//,
  '64add5cd91de531222fd83c4b073bcda42f42fcf9985b409510e78aedf0c68d1');

print("Portable pixmap format (color), binary format ...\n");
++$test;
testReadWrite( 'PPM:input_p6.ppm',
  'PPM:output_p6.ppm',
  q//,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Adobe Photoshop bitmap file ...\n");
++$test;
testReadWrite( 'PSD:input.psd',
  'PSD:output.psd',
  q//,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca' );

print("Irix RGB image file ...\n");
++$test;
testReadWrite( 'SGI:input.sgi',
  'SGI:output.sgi',
  q//,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("SUN 1-bit Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im1',
  'SUN:output.im1',
  q//,
  '36093b62470f3c0b85f2854b654df378f3ba1a3f74a09af9d73ec9b7177cbcd9');

print("SUN 8-bit Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im8',
  'SUN:output.im8',
  q//,
  'e8063197764e173ac9f4c903c46fe0b2d1360f522bff1dea49f2e049523d71c2');

print("SUN True-Color Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im24',
  'SUN:output.im24',
  q//,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Truevision Targa image file ...\n");
++$test;
testReadWrite( 'TGA:input.tga',
  'TGA:output.tga',
  q//,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("Khoros Visualization image file ...\n");
++$test;
testReadWrite( 'VIFF:input.viff',
  'VIFF:output.viff',
  q//,
  '8e900b95196d534527e9f09cf5153f41b1b5978f5de51e7dad5012f174c14eba');

print("WBMP (Wireless Bitmap (level 0) image) ...\n");
++$test;
testReadWrite( 'WBMP:input.wbmp',
  'WBMP:output.wbmp',
  q//,
  '02f153e943159ecf2a50316d7a649eca74c2f52082617249531e5ba05cc89bbd');

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testReadWrite( 'XBM:input.xbm',
  'XBM:output.xbm',
  q//,
  '36093b62470f3c0b85f2854b654df378f3ba1a3f74a09af9d73ec9b7177cbcd9');

print("X Windows system pixmap file (color) ...\n");
++$test;
testReadWrite( 'XPM:input.xpm',
  'XPM:output.xpm',
  q//,
  'e8063197764e173ac9f4c903c46fe0b2d1360f522bff1dea49f2e049523d71c2');


print("X Windows system window dump file (color) ...\n");
++$test;
testReadWrite( 'XWD:input.xwd',
  'XWD:output.xwd',
  q//,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

print("CMYK format ...\n");
++$test;
testReadWriteSized( 'CMYK:input_70x46.cmyk',
  'CMYK:output_70x46.cmyk',
  '70x46',
  8,
  q//,
  '397a9f8411f7bb065f4b6e56731734ddad8ea1a7d4c488745545e17dbe575903',
  '8934892c866bded94a5568f03d02f36e48a8244c2e05cad1cf7780de792ab856' );

print("GRAY format ...\n");
++$test;
testReadWriteSized( 'GRAY:input_70x46.gray',
  'GRAY:output_70x46.gray',
  '70x46',
  8,
  q//,
  '7ecdf2c1ae36cbf8a82c336c854b47da8683006d27e5ae5049c98d111dd927d2' );

print("RGB format ...\n");
++$test;
testReadWriteSized( 'RGB:input_70x46.rgb',
  'RGB:output_70x46.rgb',
  '70x46',
  8,
  q//,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca' );


print("RGBA format ...\n");
++$test;
testReadWriteSized( 'RGBA:input_70x46.rgba',
  'RGBA:output_70x46.rgba',
  '70x46',
  8,
  q//,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca' );

1;
