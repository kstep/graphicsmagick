#!/usr/local/bin/perl
#
# Test reading formats supported directly by ImageMagick
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..46\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

#
# AVS X image file
#
testRead( 'input.avs', '93bd185340123477f0a0e6622392a5b4' );

#
# Joint Bi-level Image experts Group file interchange format
#
++$test;
testRead( 'input.bie', '1a8d5855273e27302856c727934c9f9a' );

#
# Microsoft Windows bitmap image file
#
++$test;
testRead( 'input.bmp', '25c550da420eeb782f57dd88c689e819' );

#
# Microsoft Windows 24-bit bitmap image file
#
++$test;
testRead( 'input.bmp24', '3a17784c61a87a98f8e25f1977b9d363' );

#
# ZSoft IBM PC multi-page Paintbrush file
#
++$test;
testRead( 'input.dcx', '25c550da420eeb782f57dd88c689e819' );

#
# Microsoft Windows bitmap image file
#
++$test;
testRead( 'input.dib', '5cb2c2f3d73b57302059da592506be22' );

#
# Flexible Image Transport System
#
++$test;
testRead( 'input.fits', '78f4ac7dab43c94375d9cc4476d77af8' );

#
# CompuServe graphics interchange format
#
++$test;
testRead( 'input.gif', '25c550da420eeb782f57dd88c689e819' );

#
# CompuServe graphics interchange format (1987)
#
++$test;
testRead( 'input.gif87', '25c550da420eeb782f57dd88c689e819' );

#
# GRADATION (gradual passing from one shade to another)
# This is an internal, computed, format
#
++$test;
testRead( 'gradation:red-blue', '2c618bf0ff60556b36128fb4ab32047b' );

#
# GRANITE (granite texture)
# This is an internal, computed, format
#
++$test;
testRead( 'granite:', 'f331cdb24a6db87f7add31128d78243e' );

#
# Microsoft icon
#
++$test;
testRead( 'input.ico', 'd6fb93a149f45c8002358d58cd2db2c7' );

#
# Magick image file format
#
++$test;
testRead( 'input.miff', '25c550da420eeb782f57dd88c689e819' );

#
# MTV Raytracing image format
#
++$test;
testRead( 'input.mtv', '3a17784c61a87a98f8e25f1977b9d363' );

#
# Xv's visual schnauzer format.
#
++$test;
testRead( 'input_p7.p7', 'dd54e111da0dbf1d2aabf0359c4a4e0e' );

#
# NULL (white image)
# This is an internal generated format
#
++$test;
testRead( 'NULL:white', 'aacc74d1f14965f019465e2b33e295e7' );

#
# NULL (black image)
#
++$test;
testRead( 'NULL:black', 'b6d81b360a5672d80c27430f39153e2c' );

#
# NULL (DarkOrange image)
#
++$test;
testRead( 'NULL:DarkOrange', '387ccff83df1906bc248c25122c28922' );

#
# Portable bitmap format (black and white), ASCII format
#
++$test;
testRead( 'input_p1.pbm', '5ee57461fb1810e09ed6ddc6ffb11c7d' );


#
# Portable bitmap format (black and white), binary format
#
++$test;
testRead( 'input_p4.pbm', '5ee57461fb1810e09ed6ddc6ffb11c7d' );

#
# ZSoft IBM PC Paintbrush file
#
++$test;
testRead( 'input.pcx', '25c550da420eeb782f57dd88c689e819' );

#
# Portable graymap format (gray scale), ASCII format
#
++$test;
testRead( 'input_p2.pgm', '2cf0b0506d5464a70978557fbb02ff22' );

#
# Portable graymap format (gray scale), binary format
#
++$test;
testRead( 'input_p5.pgm', '2cf0b0506d5464a70978557fbb02ff22' );

#
# Apple Macintosh QuickDraw/PICT file
#
++$test;
testRead( 'input.pict', '8063e2ddf92f55b8bc3d46528865235c' );

#
# Alias/Wavefront RLE image format
#
++$test;
testRead( 'input.miff', '25c550da420eeb782f57dd88c689e819' );

#
# Portable pixmap format (color), ASCII format
#
++$test;
testRead( 'input_p3.ppm', 'd5fdeba9d1e28db7c07f1485d0d64697' );

#
# Portable pixmap format (color), binary format
#
++$test;
testRead( 'input_p6.ppm', 'd5fdeba9d1e28db7c07f1485d0d64697' );

#
# Adobe Photoshop bitmap file
#
++$test;
testRead( 'input.psd', '25c550da420eeb782f57dd88c689e819' );

#
# Irix RGB image file
#
++$test;
testRead( 'input.sgi', 'a05c5863b2d073046329b0b71d5e85e9' );

#
# SUN 1-bit Rasterfile
#
++$test;
testRead( 'input.im1', '5ee57461fb1810e09ed6ddc6ffb11c7d' );

# SUN 8-bit Rasterfile
#
++$test;
testRead( 'input.im8', '25c550da420eeb782f57dd88c689e819' );

# SUN TrueColor Rasterfile
#
++$test;
testRead( 'input.im24', '5cb2c2f3d73b57302059da592506be22' );

#
# Truevision Targa image file
#
++$test;
testRead( 'input.tga', '25c550da420eeb782f57dd88c689e819' );

#
# TILE (Tile image with a texture)
# This is an internal generated format
# We will tile using the default image and a MIFF file
#
++$test;
testRead( 'TILE:input.miff', '900791f1c3d52b6b817ee7e77236dc43' );

#
# PSX TIM file
#
++$test;
testRead( 'input.tim', 'cf0a2de70431838d2f88225b571337d2' );

#
# Khoros Visualization image file
#
++$test;
testRead( 'input.viff', '5cb2c2f3d73b57302059da592506be22' );

#
# X Windows system bitmap (black and white only)
#
++$test;
testRead( 'input.xbm', '1a8d5855273e27302856c727934c9f9a' );

#
# XC: Constant image of X server color
#
++$test;
testRead( 'xc:black', 'b6d81b360a5672d80c27430f39153e2c' );

#
# X Windows system pixmap file (color)
#
++$test;
testRead( 'input.xpm', '8c6c6cb0311fb938c42bf9c2d169b9e8' );

#
# X Windows system window dump file (color)
#
++$test;
testRead( 'input.xwd', '25c550da420eeb782f57dd88c689e819' );

#
# GRAY format
#
++$test;
testReadSized( 'input_70x46.gray', '70x46', '42129d46fc552e41c3e23977e7b6e09f' );

#
# RGB format
#
++$test;
testReadSized( 'input_70x46.rgb', '70x46', '5cb2c2f3d73b57302059da592506be22' );

#
# RGBA format
#
++$test;
testReadSized( 'input_70x46.rgba', '70x46', '5cb2c2f3d73b57302059da592506be22' );


#
# CMYK format
#
++$test;
testReadSized( 'input_70x46.cmyk', '70x46', '5cb2c2f3d73b57302059da592506be22' );


#
# UYVY format
#
++$test;
testReadSized( 'input_70x46.uyvy', '70x46', '02177f54ec75464ef7b4ed2c5e52112d' );

#
# YUV CCIR 601 4:1:1 file
#
++$test;
testReadSized( 'input_70x46.yuv', '70x46', '62efc3ca42a68548682bee6a74347cee' );


