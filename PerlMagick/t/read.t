#!/usr/local/bin/perl
#
# Test reading formats supported directly by ImageMagick
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..44\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';


print("AVS X image file ...\n");
testRead( 'input.avs', 'e9b162b8431709942886f79b7b7c11bb',
                       '627082c105f9538d6ec9c70c8983da70' );

print("Microsoft Windows bitmap image file ...\n");
++$test;
testRead( 'input.bmp', '7785d161ea66bd39394bb74ef704a0aa',
                       '1fd7098db7d9cffe97164874cb2e2cfa' );

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testRead( 'input.bmp24', 'e9b162b8431709942886f79b7b7c11bb',
                         '627082c105f9538d6ec9c70c8983da70' );

print("ZSoft IBM PC multi-page Paintbrush file ...\n");
++$test;
testRead( 'input.dcx', 'e9b162b8431709942886f79b7b7c11bb',
                       '627082c105f9538d6ec9c70c8983da70' );

print("Microsoft Windows bitmap image file ...\n");
++$test;
testRead( 'input.dib', 'e9b162b8431709942886f79b7b7c11bb',
                       '627082c105f9538d6ec9c70c8983da70' );

print("Flexible Image Transport System ...\n");
++$test;
testRead( 'input.fits', '7ddf23e85fe26456e87ad7347ee35f49',
                        'dafe0baf222917bc87be9e9b02c06dae' );

print("CompuServe graphics interchange format ...\n");
++$test;
testRead( 'input.gif', 'd5a97a3cec301d8848102f513ceb3f3c',
                       'c64d8a3ef2a63a2898d38bb8ccc0c7e1' );

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testRead( 'input.gif87', 'd5a97a3cec301d8848102f513ceb3f3c',
                         'c64d8a3ef2a63a2898d38bb8ccc0c7e1' );

print("GRADATION (gradual passing from one shade to another) ...\n");
#
++$test;
testRead( 'gradation:red-blue', '120b84fd035a74aac8769761e21a16b2',
                                '75b6562175926bf1fdad2ed6cca2fc57' );

print("GRANITE (granite texture) ...\n");
#
++$test;
testRead( 'granite:', '30086994324339ccb531dbb82e46128b',
                      '3d0ebb040f4ad5e2ad2df95e4238c4c5' );

print("Microsoft icon ...\n");
++$test;
testRead( 'input.ico', '8353269a2be9c1f13055b7fa147a6351',
                       'c28b9e379c673ac3c4959f1dd17d16a2' );

print("Magick image file format ...\n");
++$test;
testRead( 'input.miff', '7785d161ea66bd39394bb74ef704a0aa',
                        'cb118e74a5d2552fe254ef039d3364f8' );

print("MTV Raytracing image format ...\n");
++$test;
testRead( 'input.mtv', 'e9b162b8431709942886f79b7b7c11bb',
                       '627082c105f9538d6ec9c70c8983da70' );

print("Xv's visual schnauzer format. ...\n");
++$test;
testRead( 'input_p7.p7', '3d772946085f47ed244c548d08e36579',
                         '15bb535ee9e8461c819c9af01e14466b' );

print("NULL (white image) ...\n");
++$test;
testRead( 'NULL:white', 'ab0b2d4302f691199ab052eeb9de155b',
                        '0cc305797ca5751fe8de793fbcc2ab62' );

print("NULL (black image) ...\n");
++$test;
testRead( 'NULL:black', 'cb18a5d28e77522dfec6a6255bc3847e',
                        '6811c482ead27c0b1165ecfbe996c2b4' );

print("NULL (DarkOrange image) ...\n");
++$test;
testRead( 'NULL:DarkOrange', 'cbcd8c4b1122863d2e89149074881e15',
                             '8b77b9d48fce2121e3e4da9f03bc0e3c' );

print("Portable bitmap format (black and white), ASCII format ...\n");
++$test;
testRead( 'input_p1.pbm', '61ed9c1e08dac993e5f020de8195e009',
                          '1e335d8041cf17bbd1a337d2e03fed16' );


print("Portable bitmap format (black and white), binary format ...\n");
++$test;
testRead( 'input_p4.pbm', '61ed9c1e08dac993e5f020de8195e009',
                          '1e335d8041cf17bbd1a337d2e03fed16' );

print("ZSoft IBM PC Paintbrush file ...\n");
++$test;
testRead( 'input.pcx', 'e9b162b8431709942886f79b7b7c11bb',
                       '627082c105f9538d6ec9c70c8983da70' );

print("Portable graymap format (gray scale), ASCII format ...\n");
++$test;
testRead( 'input_p2.pgm', '4d21ec5c0ed543eeb6d478515c66ff14',
                          '3a41888bd38d053c44b4ea20905916d8' );

print("Portable graymap format (gray scale), binary format ...\n");
++$test;
testRead( 'input_p5.pgm', '4d21ec5c0ed543eeb6d478515c66ff14',
                          '3a41888bd38d053c44b4ea20905916d8' );

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testRead( 'input.pict', '7785d161ea66bd39394bb74ef704a0aa',
                        '1fd7098db7d9cffe97164874cb2e2cfa' );

print("Alias/Wavefront RLE image format ...\n");
++$test;
testRead( 'input.rle', 'e9b162b8431709942886f79b7b7c11bb',
                       '627082c105f9538d6ec9c70c8983da70' );

print("Portable pixmap format (color), ASCII format ...\n");
++$test;
testRead( 'input_p3.ppm', 'e9b162b8431709942886f79b7b7c11bb',
                          '627082c105f9538d6ec9c70c8983da70' );

print("Portable pixmap format (color), binary format ...\n");
++$test;
testRead( 'input_p6.ppm', 'e9b162b8431709942886f79b7b7c11bb',
                          '627082c105f9538d6ec9c70c8983da70' );

print("Adobe Photoshop bitmap file ...\n");
++$test;
testRead( 'input.psd', '7785d161ea66bd39394bb74ef704a0aa',
                       '1fd7098db7d9cffe97164874cb2e2cfa' );

print("Irix RGB image file ...\n");
++$test;
testRead( 'input.sgi', 'e9b162b8431709942886f79b7b7c11bb',
                       '627082c105f9538d6ec9c70c8983da70' );

print("SUN 1-bit Rasterfile ...\n");
++$test;
testRead( 'input.im1', '61ed9c1e08dac993e5f020de8195e009',
                       '1e335d8041cf17bbd1a337d2e03fed16' );
print("SUN 8-bit Rasterfile ...\n");
++$test;
testRead( 'input.im8', '7785d161ea66bd39394bb74ef704a0aa',
                       '1fd7098db7d9cffe97164874cb2e2cfa' );
print("SUN TrueColor Rasterfile ...\n");
++$test;
testRead( 'input.im24', 'e9b162b8431709942886f79b7b7c11bb',
                        '627082c105f9538d6ec9c70c8983da70' );

print("Truevision Targa image file ...\n");
++$test;
testRead( 'input.tga', 'e9b162b8431709942886f79b7b7c11bb',
                       '627082c105f9538d6ec9c70c8983da70' );

print("PSX TIM file ...\n");
++$test;
testRead( 'input.tim', '5f53f9f45afef1060449dd4f09c3706c',
                       '5293a3810c61ab481b5282b8fc2dd148' );

print("Khoros Visualization image file ...\n");
++$test;
testRead( 'input.viff', '7785d161ea66bd39394bb74ef704a0aa',
                       '1fd7098db7d9cffe97164874cb2e2cfa' );

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testRead( 'input.xbm', '61ed9c1e08dac993e5f020de8195e009',
                       '1e335d8041cf17bbd1a337d2e03fed16' );

print("XC: Constant image of X server color ...\n");
++$test;
testRead( 'xc:black', 'cb18a5d28e77522dfec6a6255bc3847e',
                      '6811c482ead27c0b1165ecfbe996c2b4' );

print("X Windows system pixmap file (color) ...\n");
++$test;
testRead( 'input.xpm', '7785d161ea66bd39394bb74ef704a0aa',
                       '1fd7098db7d9cffe97164874cb2e2cfa' );

print("X Windows system window dump file (color) ...\n");
++$test;
testRead( 'input.xwd', 'e9b162b8431709942886f79b7b7c11bb',
                       '627082c105f9538d6ec9c70c8983da70' );


print("TILE (Tile image with a texture) ...\n");
# This is an internal generated format
# We will tile using the default image and a MIFF file
#
++$test;
testReadSized( 'TILE:input.miff', '140x92', 'ea3df20e9ae6e1b6fc948d1ee64b571c',
                                            'e63461fdb9532fd82473404c6883fa78');

print("GRAY format ...\n");
++$test;
testReadSized( 'input_70x46.gray', '70x46', '17e5358049cdf0613e54f0f2eb42701d',
                                            '041485656f97c539a7e2b9f7ebd7a7f9' );

print("RGB format ...\n");
++$test;
testReadSized( 'input_70x46.rgb', '70x46', 'e9b162b8431709942886f79b7b7c11bb',
                                           '80350402e42b5b55dca500bfc1a23d5a' );

print("RGBA format ...\n");
++$test;
testReadSized( 'input_70x46.rgba', '70x46', 'f3e5ab88e328da3044c1fc177a7e2978',
                                            '97558e5d40363309da60696286664df1' );


print("CMYK format ...\n");
++$test;
testReadSized( 'input_70x46.cmyk', '70x46', 'cf80501135a580d67542a4ec1a4ff524',
                                            '840c11bcfa1e39cb97589813dc139c28' );


print("UYVY format ...\n");
++$test;
testReadSized( 'input_70x46.uyvy', '70x46', '81990d39685ec85b67d606a4bd35fc9c',
                                            '082de377cb33ad807560666967172dec' );
