#!/usr/local/bin/perl
# Copyright (C) 2003 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1991-1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#
# Test reading TIFF images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..11\n"; }
END {print "not ok $test\n" unless $loaded;}

use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/tiff' || die 'Cd failed';

#
# 1) Test Reading Monochrome
# 
print("Monochrome ...\n");
testRead ( 'input_mono.tiff',
  '2b48bcf7d93cc31a8deb3026d872f1bb0a300b0f4e177423e5301adc638179e4' );

#
# 2) Test reading PseudoColor
#
++$test;
print("PseudoColor ...\n");
testRead( 'input_256.tiff',
  'a570df6d4c214d08bccaaee856f617883fee4f96494af6ef5e25c76d86ea0e0a' );

#
# 3) Test reading PseudoColor using contiguous planar packing
#
++$test;
print("PseudoColor contiguous planes ...\n");
testRead( 'input_256_planar_contig.tiff',
  'a570df6d4c214d08bccaaee856f617883fee4f96494af6ef5e25c76d86ea0e0a' );

#
# 4) Test reading PseudoColor using seperate planes
#
++$test;
print("PseudoColor seperate planes ...\n");
testRead( 'input_256_planar_separate.tiff',
  'a570df6d4c214d08bccaaee856f617883fee4f96494af6ef5e25c76d86ea0e0a' );

#
# 5) Test Reading TrueColor (8-bit)
# 
++$test;
print("TrueColor (8-bit) image ...\n");
testRead( 'input_truecolor.tiff',
  '326599218925e7aa27c735d482332744cccf41f460a1ba2d276a2d172f2b3de0' );

#
# 6) Test Reading TrueColor (16-bit)
#
++$test;
print("TrueColor (16-bit) image ...\n");
testRead( 'input_truecolor_16.tiff',
  '1030dc73bdc72c846831d10b661539e7a9e9c24a170f24ffdffaeae1a8aa769d',
  '72fd047e4f5942b4edc96e8b16444580e9eb1f265017dd6bb11795a842e58d58' );

#
# 7) Test Reading 8-bit TrueColor Tiled (32x32 tiles)
# 
++$test;
print("TrueColor (8-bit) tiled image, 32x32 tiles ...\n");
testRead( 'input_truecolor_tiled32x32.tiff',
  '326599218925e7aa27c735d482332744cccf41f460a1ba2d276a2d172f2b3de0' );

#
# 8) Test Reading 8-bit TrueColor Tiled (8 rows per strip)
# 
++$test;
print("TrueColor (8-bit) stripped, image, 8 rows per strip ...\n");
testRead( 'input_truecolor_stripped.tiff',
  '326599218925e7aa27c735d482332744cccf41f460a1ba2d276a2d172f2b3de0' );

#
# 9) Test Reading Grayscale 8-bit
# 
++$test;
print("Grayscale (8-bit) ...\n");
testRead( 'input_gray_8bit.tiff',
  '00f6147d547f5a654369a5b41d18f0b44b38f9cc753c773f384808b9a18cac92');

#
# 10) Test Reading Grayscale 12-bit
# 
++$test;
print("Grayscale (12-bit) ...\n");
testRead( 'input_gray_12bit.tiff',
  'c644742ce63461958e441f0f7e5fae596bb5381b2c5d41f6c524b3f68f5d0d97',
  'd401344f52d6e85f63aa98467fef7c5c1408905322b56fed20aa982417537d36',
  '43e45a7fa6ac0166e689eefde058354a23cc2cf5d80c985ace279391bdb1b64c');

#
# 11) Test Reading Grayscale 16-bit
# 
++$test;
print("Grayscale (16-bit) ...\n");
testRead( 'input_gray_16bit.tiff',
  '00f6147d547f5a654369a5b41d18f0b44b38f9cc753c773f384808b9a18cac92',
  '1db9216ffb84dab419b4924e6aba945e922f728c0dca8ce24c3d7f4c47c604b9' );
