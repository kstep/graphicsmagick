#!/usr/local/bin/perl
#
# Test writing formats supported directly by ImageMagick
#
# Currently supported tests are for formats that ImageMagick
# knows how to both read and write.
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.

BEGIN { $| = 1; $test=1; print "1..32\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

#
# AVS X image file
#
testReadWrite( 'AVS:input.avs',
               'AVS:output.avs',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

#
# Microsoft Windows bitmap image file
#
++$test;
testReadWrite( 'BMP:input.bmp',
               'BMP:output.bmp',
               q//,
               '7785d161ea66bd39394bb74ef704a0aa',
               '1fd7098db7d9cffe97164874cb2e2cfa' );

#
# Microsoft Windows 24-bit bitmap image file
#
++$test;
testReadWrite( 'BMP24:input.bmp24',
               'BMP24:output.bmp24',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70');

#
# ZSoft IBM PC multi-page Paintbrush file
#
++$test;
testReadWrite( 'DCX:input.dcx',
               'DCX:output.dcx',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

#
# Microsoft Windows bitmap image file
#
++$test;
testReadWrite( 'DIB:input.dib',
               'DIB:output.dib',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

#
# Flexible Image Transport System
#
++$test;
testReadWrite( 'FITS:input.fits',
               'FITS:output.fits',
               q//,
               '7ddf23e85fe26456e87ad7347ee35f49',
               'dafe0baf222917bc87be9e9b02c06dae' );

#
# CompuServe graphics interchange format
#
++$test;
testReadWrite( 'GIF:input.gif',
               'GIF:output.gif',
               q//,
               'd5a97a3cec301d8848102f513ceb3f3c',
               'c64d8a3ef2a63a2898d38bb8ccc0c7e1' );

#
# CompuServe graphics interchange format (1987)
#
++$test;
testReadWrite( 'GIF87:input.gif87',
               'GIF87:output.gif87',
               q//,
               'd5a97a3cec301d8848102f513ceb3f3c',
               'c64d8a3ef2a63a2898d38bb8ccc0c7e1' );

#
# Magick image file format
#
++$test;
testReadWrite( 'MIFF:input.miff',
               'MIFF:output.miff',
               q//,
               '7785d161ea66bd39394bb74ef704a0aa',
               '9a1040b983f2685f9a1c957a3cbcb687' );

#
# MTV Raytracing image format
#
++$test;
testReadWrite( 'MTV:input.mtv',
               'MTV:output.mtv',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

#
# Xv's visual schnauzer format.
#
++$test;
testReadWrite( 'P7:input_p7.p7',
               'P7:output_p7.p7',
               q//,
               '511e64f36f12cb8283b48bc3153aa98a',
               '5ddc4ec40e51da6156dde2434fc3d177' );

#
# Portable bitmap format (black and white), ASCII format
#
++$test;
testReadWrite( 'PBM:input_p1.pbm',
               'PBM:output_p1.pbm',
               q/compress=>'None'/,
               '61ed9c1e08dac993e5f020de8195e009',
               '1e335d8041cf17bbd1a337d2e03fed16' );

#
# Portable bitmap format (black and white), binary format
#
++$test;
testReadWrite( 'PBM:input_p4.pbm',
               'PBM:output_p4.pbm',
               q//,
               '61ed9c1e08dac993e5f020de8195e009',
               '1e335d8041cf17bbd1a337d2e03fed16' );

#
# ZSoft IBM PC Paintbrush file
#
++$test;
testReadWrite( 'PCX:input.pcx',
               'PCX:output.pcx',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

#
# Portable graymap format (gray scale), ASCII format
#
++$test;
testReadWrite( 'PGM:input_p2.pgm',
               'PGM:output_p2.pgm',
               q/compress=>'None'/,
               '4d21ec5c0ed543eeb6d478515c66ff14',
               '3a41888bd38d053c44b4ea20905916d8' );

#
# Apple Macintosh QuickDraw/PICT file
#
++$test;
testReadWrite( 'PICT:input.pict',
               'PICT:output.pict',
               q//,
               '7785d161ea66bd39394bb74ef704a0aa',
               '1fd7098db7d9cffe97164874cb2e2cfa' );

#
# Portable pixmap format (color), ASCII format
#
++$test;
testReadWrite( 'PPM:input_p3.ppm',
               'PPM:output_p3.ppm',
               q/compress=>'None'/,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

#
# Portable graymap format (gray scale), binary format
#
++$test;
testReadWrite( 'PGM:input_p5.pgm',
               'PGM:output_p5.pgm',
               q//,
               '4d21ec5c0ed543eeb6d478515c66ff14',
               '3a41888bd38d053c44b4ea20905916d8' );

#
# Portable pixmap format (color), binary format
#
++$test;
testReadWrite( 'PPM:input_p6.ppm',
               'PPM:output_p6.ppm',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

#
# Adobe Photoshop bitmap file
#
++$test;
testReadWrite( 'PSD:input.psd',
               'PSD:output.psd',
               q//,
               '7785d161ea66bd39394bb74ef704a0aa',
               '68f7f773127ee624d2ab96fb90f46eb7' );

#
# Irix RGB image file
#
++$test;
testReadWrite( 'SGI:input.sgi',
               'SGI:output.sgi',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

#
# SUN 1-bit Rasterfile
#
++$test;
testReadWrite( 'SUN:input.im1',
               'SUN:output.im1',
               q//,
               '61ed9c1e08dac993e5f020de8195e009',
               '1e335d8041cf17bbd1a337d2e03fed16' );

#
# SUN 8-bit Rasterfile
#
++$test;
testReadWrite( 'SUN:input.im8',
               'SUN:output.im8',
               q//,
               '7785d161ea66bd39394bb74ef704a0aa',
               '1fd7098db7d9cffe97164874cb2e2cfa' );

#
# SUN True-Color Rasterfile
#
++$test;
testReadWrite( 'SUN:input.im24',
               'SUN:output.im24',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

#
# Truevision Targa image file
#
++$test;
testReadWrite( 'TGA:input.tga',
               'TGA:output.tga',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );


#
# Khoros Visualization image file
#
++$test;
testReadWrite( 'VIFF:input.viff',
               'VIFF:output.viff',
               q//,
               '7785d161ea66bd39394bb74ef704a0aa',
               '1fd7098db7d9cffe97164874cb2e2cfa' );
#
# X Windows system bitmap (black and white only)
#
++$test;
testReadWrite( 'XBM:input.xbm',
               'XBM:output.xbm',
               q//,
               '61ed9c1e08dac993e5f020de8195e009',
               '1e335d8041cf17bbd1a337d2e03fed16' );

#
# X Windows system pixmap file (color)
#
++$test;
testReadWrite( 'XPM:input.xpm',
               'XPM:output.xpm',
               q//,
               '7785d161ea66bd39394bb74ef704a0aa',
               '1fd7098db7d9cffe97164874cb2e2cfa' );

#
# X Windows system window dump file (color)
#
++$test;
testReadWrite( 'XWD:input.xwd',
               'XWD:output.xwd',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

#
# GRAY format
#
++$test;
testReadWriteSized( 'GRAY:input_70x46.gray',
		    'GRAY:output_70x46.gray',
		    '70x46',
		    q//,
		    '17e5358049cdf0613e54f0f2eb42701d',
                    '041485656f97c539a7e2b9f7ebd7a7f9' );

#
# RGB format
#
++$test;
testReadWriteSized( 'RGB:input_70x46.rgb',
		    'RGB:output_70x46.rgb',
		    '70x46',
		    q//,
		    'e9b162b8431709942886f79b7b7c11bb',
               'cba01fe4b4934f844248d887681f775b' );

#
# RGBA format
#
++$test;
testReadWriteSized( 'RGBA:input_70x46.rgba',
		    'RGBA:output_70x46.rgba',
		    '70x46',
		    q//,
		    'f3e5ab88e328da3044c1fc177a7e2978',
                    '02d15ba5c3a07f87306af970d82786d9' );

1;
