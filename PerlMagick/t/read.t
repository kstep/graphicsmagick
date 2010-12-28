#!/usr/local/bin/perl
# Copyright (C) 2003 - 2009 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1991-1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#
# Test reading formats supported directly by GraphicsMagick
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..77\n"; }
END {print "not ok $test\n" unless $loaded;}
use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

print("AVS X image file ...\n");
testReadCompare('input.avs', 'reference/read/input_avs.miff', q//, 0, 0);

print("Microsoft Windows bitmap image file ...\n");
++$test;
testReadCompare('input.bmp', 'reference/read/input_bmp.miff', q//, 0, 0);

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testReadCompare('input.bmp24', 'reference/read/input_bmp24.miff', q//, 0, 0);

print("Cineon Gray image file ...\n");
++$test;
testReadCompare('input_gray.cin', 'reference/read/input_gray_cin.miff', q//, 0, 0);

print("Cineon RGB image file ...\n");
++$test;
testReadCompare('input_rgb.cin', 'reference/read/input_rgb_cin.miff', q//, 0, 0);

print("ZSoft IBM PC multi-page Paintbrush file ...\n");
++$test;
testReadCompare('input.dcx', 'reference/read/input_dcx.miff', q//, 0, 0);

print("Microsoft Windows bitmap image file ...\n");
++$test;
testReadCompare('input.dib', 'reference/read/input_dib.miff', q//, 0, 0);

print("Flexible Image Transport System 8-bit ...\n");
++$test;
testReadCompare('input_gray_08bit.fits', 'reference/read/input_gray_08bit_fits.miff', q//, 0, 0);

print("Flexible Image Transport System LSB 16-bit ...\n");
++$test;
testReadCompare('input_gray_16bit.fits', 'reference/read/input_gray_16bit_fits.miff', q//, 0.002, 0.004);

print("Flexible Image Transport System LSB 32-bit ...\n");
++$test;
testReadCompare('input_gray_32bit.fits', 'reference/read/input_gray_32bit_fits.miff', q//, 0.002, 0.004);

print("Flexible Image Transport System LSB double ...\n");
++$test;
testReadCompare('input_gray_lsb_double.fits', 'reference/read/input_gray_lsb_double_fits.miff', q//, 0.002, 0.004);

print("Flexible Image Transport System MSB 16-bit ...\n");
++$test;
testReadCompare('input_gray_msb_16bit.fits', 'reference/read/input_gray_msb_16bit_fits.miff', q//, 0.002, 0.004);

print("Flexible Image Transport System MSB 32-bit ...\n");
++$test;
testReadCompare('input_gray_msb_32bit.fits', 'reference/read/input_gray_msb_32bit_fits.miff', q//, 0.002, 0.004);

print("Flexible Image Transport System MSB 64-bit ...\n");
++$test;
testReadCompare('input_gray_msb_64bit.fits', 'reference/read/input_gray_msb_64bit_fits.miff', q//, 0.002, 0.004);

print("Flexible Image Transport System MSB float ...\n");
++$test;
testReadCompare('input_gray_msb_float.fits', 'reference/read/input_gray_msb_float_fits.miff', q//, 0.002, 0.004);

print("Flexible Image Transport System MSB double ...\n");
++$test;
testReadCompare('input_gray_msb_double.fits', 'reference/read/input_gray_msb_double_fits.miff', q//, 0.002, 0.004);

print("CompuServe graphics interchange format ...\n");
++$test;
testReadCompare('input.gif', 'reference/read/input_gif.miff', q//, 0, 0);

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testReadCompare('input.gif87', 'reference/read/input_gif87.miff', q//, 0, 0);

print("Gradient (gradual passing from one shade to another) ...\n");
++$test;
testReadCompare('gradient:red-blue', 'reference/read/gradient.miff',
                q/size=>"70x46"/, 0.003, 0.004);

print("GRANITE (granite texture) ...\n");
++$test;
testReadCompare('granite:', 'reference/read/granite.miff', q/size=>"70x46"/, 0, 0);

print("HRZ Slow scan TV ...\n");
++$test;
testReadCompare('input.hrz', 'reference/read/input_hrz.miff', q//, 0, 0);

print("MacPaint ...\n");
++$test;
testReadCompare('input.mac', 'reference/read/input_mac.bmp', q//, 0, 0);

print("MAT (MatLab logical 8-bit LSB integer) ...\n");
++$test;
testReadCompare('input_logical_lsb_08bit.mat', 'reference/read/input_logical_lsb_08bit_mat.miff', q//, 0, 0);

print("MAT (MatLab gray 8-bit LSB integer) ...\n");
++$test;
testReadCompare('input_gray_lsb_08bit.mat', 'reference/read/input_gray_lsb_08bit_mat.miff', q//, 0, 0);

print("MAT (MatLab gray 8-bit MSB integer) ...\n");
++$test;
testReadCompare('input_gray_msb_08bit.mat', 'reference/read/input_gray_msb_08bit_mat.miff', q//, 0, 0);

print("MAT (MatLab gray 16-bit LSB integer) ...\n");
++$test;
testReadCompare('input_gray_lsb_16bit.mat', 'reference/read/input_gray_lsb_16bit_mat.miff', q//, 0, 0);

print("MAT (MatLab gray 32-bit LSB integer) ...\n");
++$test;
testReadCompare('input_gray_lsb_32bit.mat', 'reference/read/input_gray_lsb_32bit_mat.miff', q//, 0, 0);

print("MAT (MatLab gray 32-bit LSB float) ...\n");
++$test;
testReadCompare('input_gray_lsb_float.mat', 'reference/read/input_gray_lsb_float_mat.miff', q//, 0.002, 0.004);

print("MAT (MatLab gray 64-bit LSB double) ...\n");
++$test;
testReadCompare('input_gray_lsb_double.mat', 'reference/read/input_gray_lsb_double_mat.miff', q//, 0.002, 0.004);

print("MAT (MatLab RGB 8-bit LSB integer) ...\n");
++$test;
testReadCompare('input_rgb_lsb_08bit.mat', 'reference/read/input_rgb_lsb_08bit_mat.miff', q//, 0, 0);

print("Microsoft icon ...\n");
++$test;
testReadCompare('input.ico', 'reference/read/input_ico.miff', q//, 0, 0);

print("Magick image file format ...\n");
++$test;
testReadCompare('input.miff', 'reference/read/input_miff.miff', q//, 0, 0);

print("MTV Raytracing image format ...\n");
++$test;
testReadCompare('input.mtv', 'reference/read/input_mtv.miff', q//, 0, 0);

print("Xv's visual schnauzer format. ...\n");
++$test;
testReadCompare('input_p7.p7', 'reference/read/input_p7.miff', q//,
                0.002, 0.004);

print("NULL (white image) ...\n");
++$test;
testReadCompare('NULL:white', 'reference/read/input_null_white.miff', q/size=>"70x46"/, 0, 0);

print("NULL (black image) ...\n");
++$test;
testReadCompare('NULL:black', 'reference/read/input_null_black.miff', q/size=>"70x46"/, 0, 0);

print("NULL (DarkOrange image) ...\n");
++$test;
testReadCompare('NULL:DarkOrange', 'reference/read/input_null_DarkOrange.miff', q/size=>"70x46"/, 0, 0);

print("Portable bitmap format (black and white), ASCII format ...\n");
++$test;
testReadCompare('input_p1.pbm', 'reference/read/input_pbm_p1.miff', q//, 0, 0);

print("Portable bitmap format (black and white), binary format ...\n");
++$test;
testReadCompare('input_p4.pbm', 'reference/read/input_pbm_p4.miff', q//, 0, 0);

print("ZSoft IBM PC Paintbrush file ...\n");
++$test;
testReadCompare('input.pcx', 'reference/read/input_pcx.miff', q//, 0, 0);

print("Portable graymap format (gray scale), ASCII format ...\n");
++$test;
testReadCompare('input_p2.pgm', 'reference/read/input_pgm_p2.miff', q//, 0, 0);

print("Portable graymap format (gray scale), binary format ...\n");
++$test;
testReadCompare('input_p5.pgm', 'reference/read/input_pgm_p5.miff', q//, 0, 0);

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testReadCompare('input.pict', 'reference/read/input_pict.miff', q//, 0, 0);

print("Alias/Wavefront RLA image format (gray scale) ...\n");
++$test;
testReadCompare('input_gray.rla', 'reference/read/input_gray_rla.miff', q//, 0, 0);

print("Alias/Wavefront RLA image format (RGB) ...\n");
++$test;
testReadCompare('input_rgb.rla', 'reference/read/input_rgb_rla.miff', q//, 0, 0);

print("Utah Raster Toolkit (URT) RLE image format (gray scale) ...\n");
++$test;
testReadCompare('input_gray.rle', 'reference/read/input_gray_rle.miff', q//, 0, 0);

print("Utah Raster Toolkit (URT) RLE image format (RGB) ...\n");
++$test;
testReadCompare('input.rle', 'reference/read/input_rle.miff', q//, 0, 0);

print("Portable pixmap format (color), ASCII format ...\n");
++$test;
testReadCompare('input_p3.ppm', 'reference/read/input_ppm_p3.miff', q//, 0, 0);

print("Portable pixmap format (color), binary format ...\n");
++$test;
testReadCompare('input_p6.ppm', 'reference/read/input_ppm_p6.miff', q//, 0, 0);

print("Adobe Photoshop bitmap file ...\n");
++$test;
testReadCompare('input.psd', 'reference/read/input_psd.miff', q//, 0, 0);

print("Irix RGB image file ...\n");
++$test;
testReadCompare('input.sgi', 'reference/read/input_sgi.miff', q//, 0, 0);

print("SUN 1-bit Rasterfile ...\n");
++$test;
testReadCompare('input.im1', 'reference/read/input_im1.miff', q//, 0, 0);

print("SUN 8-bit Rasterfile ...\n");
++$test;
testReadCompare('input.im8', 'reference/read/input_im8.miff', q//, 0, 0);

print("SUN TrueColor Rasterfile ...\n");
++$test;
testReadCompare('sun:input.im24', 'reference/read/input_im24.miff', q//, 0, 0);

print("Topol Type 1 ...\n");
++$test;
testReadCompare('topol:topol_1.ras', 'reference/read/topol_1.miff', q//, 0, 0);

print("Topol Type 2 ...\n");
++$test;
testReadCompare('topol:topol_2.ras', 'reference/read/topol_2.miff', q//, 0, 0);

print("Topol Type 3 ...\n");
++$test;
testReadCompare('topol:topol_3.ras', 'reference/read/topol_3.miff', q//, 0, 0);

print("Topol Type 4 ...\n");
++$test;
testReadCompare('topol:topol_4.ras', 'reference/read/topol_4.miff', q//, 0, 0);

print("Topol Type 5 ...\n");
++$test;
testReadCompare('topol:topol_5.ras', 'reference/read/topol_5.miff', q//, 0, 0);

print("Topol Type 7 ...\n");
++$test;
testReadCompare('topol:topol_7.ras', 'reference/read/topol_7.miff', q//, 0, 0);

print("Truevision Targa image file ...\n");
++$test;
testReadCompare('input.tga', 'reference/read/input_tga.miff', q//, 0, 0);

print("PSX TIM file ...\n");
++$test;
testReadCompare('input.tim', 'reference/read/input_tim.miff', q//, 0, 0);

print("Khoros Visualization image file ...\n");
++$test;
testReadCompare('input.viff', 'reference/read/input_viff.miff', q//, 0, 0);

print("WBMP (Wireless Bitmap (level 0) image) ...\n");
++$test;
testReadCompare('input.wbmp', 'reference/read/input_wbmp.miff', q//, 0, 0);

print("WPG (Word Perfect Graphics image, 4 bit depth WPG level 1) ...\n");
++$test;
testReadCompare('input1_4.wpg', 'reference/read/input1_4_wpg.miff', q//, 0, 0);

print("WPG (Word Perfect Graphics image, 8 bit depth WPG level 1) ...\n");
++$test;
testReadCompare('input1_8_1.wpg', 'reference/read/input1_8_1_wpg.miff', q//, 0, 0);

print("WPG (Word Perfect Graphics image, 1 bit depth + 24 bit depth WPG level 2) ...\n");
++$test;
testReadCompare('input2_TC1.wpg', 'reference/read/input2_TC1_wpg.miff', q//, 0, 0);

print("WPG (Word Perfect Graphics image, 8 bit depth WPG level 2) ...\n");
++$test;
testReadCompare('input2_8.wpg', 'reference/read/input2_8_wpg.miff', q//, 0, 0);

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testReadCompare('input.xbm', 'reference/read/input_xbm.miff', q//, 0, 0);

print("XC: Constant image of X server color ...\n");
++$test;
testReadCompare('xc:black', 'reference/read/input_xc_black.miff', q/size=>"70x46",, depth=>8/, 0, 0);

print("X Windows system pixmap file (color) ...\n");
++$test;
testReadCompare('input.xpm', 'reference/read/input_xpm.miff', q//, 0, 0);
# Q:32 mean-error=0.23551931713272, maximum-error=0.989543041912839

#print("X Windows system window dump file (color) ...\n");
#++$test;
#testReadCompare('input.xwd', 'reference/read/input_xwd.miff', q//, 0, 0);

print("TILE (Tile image with a texture) ...\n");
# This is an internal generated format
# We will tile using the default image and a MIFF file
#
++$test;
testReadCompare('TILE:input.miff', 'reference/read/input_tile.miff',
                q/size=>"140x92", depth=>8/, 0, 0);

print("CMYK format ...\n");
++$test;
testReadCompare('input_70x46.cmyk', 'reference/read/input_cmyk.miff',
                q/size=>"70x46", depth=>8/, 0, 0);

print("GRAY format ...\n");
++$test;
testReadCompare('input_70x46.gray', 'reference/read/input_gray.miff',
                q/size=>"70x46", depth=>8/, 0, 0);

print("RGB format ...\n");
++$test;
testReadCompare('input_70x46.rgb', 'reference/read/input_rgb.miff',
                q/size=>"70x46", depth=>8/, 0, 0);

print("RGBA format ...\n");
++$test;
testReadCompare('input_70x46.rgba', 'reference/read/input_rgba.miff',
                q/size=>"70x46", depth=>8/, 0, 0);

print("UYVY format ...\n");
++$test;
testReadCompare('input_70x46.uyvy', 'reference/read/input_uyvy.miff',
                q/size=>"70x46", depth=>8/, 0.006, 0.008);

