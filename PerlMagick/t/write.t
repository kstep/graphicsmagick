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

print("AVS X image file ...\n");
testReadWrite( 'AVS:input.avs',
               'AVS:output.avs',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

print("Microsoft Windows bitmap image file ...\n");
++$test;
testReadWrite( 'BMP:input.bmp',
               'BMP:output.bmp',
               q//,
               '7785d161ea66bd39394bb74ef704a0aa',
               '1fd7098db7d9cffe97164874cb2e2cfa' );

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testReadWrite( 'BMP24:input.bmp24',
               'BMP24:output.bmp24',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70');


print("ZSoft IBM PC multi-page Paintbrush file ...\n");
++$test;
testReadWrite( 'DCX:input.dcx',
               'DCX:output.dcx',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

print("Microsoft Windows bitmap image file ...\n");
++$test;
testReadWrite( 'DIB:input.dib',
               'DIB:output.dib',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

print("Flexible Image Transport System ...\n");
++$test;
testReadWrite( 'FITS:input.fits',
               'FITS:output.fits',
               q//,
               '7ddf23e85fe26456e87ad7347ee35f49',
               'dafe0baf222917bc87be9e9b02c06dae' );

print("CompuServe graphics interchange format ...\n");
++$test;
testReadWrite( 'GIF:input.gif',
               'GIF:output.gif',
               q//,
               'd5a97a3cec301d8848102f513ceb3f3c',
               'c64d8a3ef2a63a2898d38bb8ccc0c7e1' );

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testReadWrite( 'GIF87:input.gif87',
               'GIF87:output.gif87',
               q//,
               'd5a97a3cec301d8848102f513ceb3f3c',
               'c64d8a3ef2a63a2898d38bb8ccc0c7e1' );


print("Magick image file format ...\n");
++$test;
testReadWrite( 'MIFF:input.miff',
               'MIFF:output.miff',
               q//,
               '7785d161ea66bd39394bb74ef704a0aa',
               '9a1040b983f2685f9a1c957a3cbcb687' );

print("MTV Raytracing image format ...\n");
++$test;
testReadWrite( 'MTV:input.mtv',
               'MTV:output.mtv',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

print("Xv's visual schnauzer format ...\n");
++$test;
testReadWrite( 'P7:input_p7.p7',
               'P7:output_p7.p7',
               q//,
               '511e64f36f12cb8283b48bc3153aa98a',
               '5ddc4ec40e51da6156dde2434fc3d177' );


print("Portable bitmap format (black and white), ASCII format ...\n");
++$test;
testReadWrite( 'PBM:input_p1.pbm',
               'PBM:output_p1.pbm',
               q/compress=>'None'/,
               '61ed9c1e08dac993e5f020de8195e009',
               '1e335d8041cf17bbd1a337d2e03fed16' );


print("Portable bitmap format (black and white), binary format ...\n");
++$test;
testReadWrite( 'PBM:input_p4.pbm',
               'PBM:output_p4.pbm',
               q//,
               '61ed9c1e08dac993e5f020de8195e009',
               '1e335d8041cf17bbd1a337d2e03fed16' );

print("ZSoft IBM PC Paintbrush file ...\n");
++$test;
testReadWrite( 'PCX:input.pcx',
               'PCX:output.pcx',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

print("Portable graymap format (gray scale), ASCII format ...\n");
++$test;
testReadWrite( 'PGM:input_p2.pgm',
               'PGM:output_p2.pgm',
               q/compress=>'None'/,
               '4d21ec5c0ed543eeb6d478515c66ff14',
               '3a41888bd38d053c44b4ea20905916d8' );

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testReadWrite( 'PICT:input.pict',
               'PICT:output.pict',
               q//,
               '7785d161ea66bd39394bb74ef704a0aa',
               '1fd7098db7d9cffe97164874cb2e2cfa' );

print("Portable pixmap format (color), ASCII format ...\n");
++$test;
testReadWrite( 'PPM:input_p3.ppm',
               'PPM:output_p3.ppm',
               q/compress=>'None'/,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

print("Portable graymap format (gray scale), binary format ...\n");
++$test;
testReadWrite( 'PGM:input_p5.pgm',
               'PGM:output_p5.pgm',
               q//,
               '4d21ec5c0ed543eeb6d478515c66ff14',
               '3a41888bd38d053c44b4ea20905916d8' );

print("Portable pixmap format (color), binary format ...\n");
++$test;
testReadWrite( 'PPM:input_p6.ppm',
               'PPM:output_p6.ppm',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );

print("Adobe Photoshop bitmap file ...\n");
++$test;
testReadWrite( 'PSD:input.psd',
               'PSD:output.psd',
               q//,
               '7785d161ea66bd39394bb74ef704a0aa',
               '1fd7098db7d9cffe97164874cb2e2cfa' );

print("Irix RGB image file ...\n");
++$test;
testReadWrite( 'SGI:input.sgi',
               'SGI:output.sgi',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );


print("SUN 1-bit Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im1',
               'SUN:output.im1',
               q//,
               '61ed9c1e08dac993e5f020de8195e009',
               '1e335d8041cf17bbd1a337d2e03fed16' );


print("SUN 8-bit Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im8',
               'SUN:output.im8',
               q//,
               '7785d161ea66bd39394bb74ef704a0aa',
               '1fd7098db7d9cffe97164874cb2e2cfa' );


print("SUN True-Color Rasterfile ...\n");
++$test;
testReadWrite( 'SUN:input.im24',
               'SUN:output.im24',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );


print("Truevision Targa image file ...\n");
++$test;
testReadWrite( 'TGA:input.tga',
               'TGA:output.tga',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );



print("Khoros Visualization image file ...\n");
++$test;
testReadWrite( 'VIFF:input.viff',
               'VIFF:output.viff',
               q//,
               '7785d161ea66bd39394bb74ef704a0aa',
               '1fd7098db7d9cffe97164874cb2e2cfa' );

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testReadWrite( 'XBM:input.xbm',
               'XBM:output.xbm',
               q//,
               '61ed9c1e08dac993e5f020de8195e009',
               '1e335d8041cf17bbd1a337d2e03fed16' );

print("X Windows system pixmap file (color) ...\n");
++$test;
testReadWrite( 'XPM:input.xpm',
               'XPM:output.xpm',
               q//,
               '7785d161ea66bd39394bb74ef704a0aa',
               '1fd7098db7d9cffe97164874cb2e2cfa' );


print("X Windows system window dump file (color) ...\n");
++$test;
testReadWrite( 'XWD:input.xwd',
               'XWD:output.xwd',
               q//,
               'e9b162b8431709942886f79b7b7c11bb',
               '627082c105f9538d6ec9c70c8983da70' );


print("GRAY format ...\n");
++$test;
testReadWriteSized( 'GRAY:input_70x46.gray',
		    'GRAY:output_70x46.gray',
		    '70x46',
		    q//,
		    '17e5358049cdf0613e54f0f2eb42701d',
                    '041485656f97c539a7e2b9f7ebd7a7f9' );

print("RGB format ...\n");
++$test;
testReadWriteSized( 'RGB:input_70x46.rgb',
		    'RGB:output_70x46.rgb',
		    '70x46',
		    q//,
		    'e9b162b8431709942886f79b7b7c11bb',
                    '80350402e42b5b55dca500bfc1a23d5a' );


print("RGBA format ...\n");
++$test;
testReadWriteSized( 'RGBA:input_70x46.rgba',
		    'RGBA:output_70x46.rgba',
		    '70x46',
		    q//,
		    'f3e5ab88e328da3044c1fc177a7e2978',
                    '97558e5d40363309da60696286664df1' );

1;
