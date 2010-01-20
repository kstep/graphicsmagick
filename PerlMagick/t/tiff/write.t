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
# Test writing TIFF images
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
# 1) Test 4-bit pseudocolor image
#
print("PsuedoColor image (4 bits/sample) ...\n");
testReadWrite( 'input_palette_16.tiff',
  'output_palette_16.tiff',
  q//,
  'a0313f6235a3158655412e1480c2f37549d89a5cacf40abc854811af1b757159');

#
# 2) Test 8-bit pseudocolor image
#
++$test;
print("PsuedoColor image (8 bits/sample) ...\n");
testReadWrite( 'input_palette_256.tiff',
  'output_palette_256.tiff',
  q//,
  'a570df6d4c214d08bccaaee856f617883fee4f96494af6ef5e25c76d86ea0e0a');

#
# 3) Test 4-bit pseudocolor + matte channel image
#
++$test;
print("PsuedoColor image (4 bits/sample + matte channel) ...\n");
testReadWrite( 'input_palette_16_matte.tiff',
  'output_palette_16_matte.tiff',
  q//,
  '631c0997280217f3f430336513f1e51fe03e04f552d49a848449a438c100aa20');

#
# 4) Test 8-bit pseudocolor + matte channel image
#
++$test;
print("PsuedoColor image (8 bits/sample + matte channel) ...\n");
testReadWrite( 'input_palette_256_matte.tiff',
  'output_palette_256_matte.tiff',
  q//,
  'a3f865320dc8b7953850babbd22d468d821c0103cc14063d6459159506a910d6');

#
# 5) Test truecolor image
#
++$test;
print("TrueColor image (8 bits/sample) ...\n");
testReadWrite( 'input_truecolor_08.tiff',
  'output_truecolor_08.tiff',
  q/quality=>55/,
  '326599218925e7aa27c735d482332744cccf41f460a1ba2d276a2d172f2b3de0' );

#
# 6) Test monochrome image
#
++$test;
print("Gray image (1 bit per sample) ...\n");
testReadWrite(  'input_gray_01bit_minwhite.tiff',
  'output_gray_01bit_minwhite.tiff',
  q//,
  '2b48bcf7d93cc31a8deb3026d872f1bb0a300b0f4e177423e5301adc638179e4' );

#
# 7) Test gray 4 bit image
#
++$test;
print("Gray image (4 bits per sample) ...\n");
testReadWrite(  'input_gray_04bit.tiff',
  'output_gray_04bit.tiff',
  q//,
  '45c0655a417852480336d5e12d7bb16b9647d911b11589e9d000a05dd1813570' );

#
# 8) Test gray 8 bit image
#
++$test;
print("Gray image (8 bits per sample) ...\n");
testReadWrite(  'input_gray_08bit.tiff',
  'output_gray_08bit.tiff',
  q//,
  '00f6147d547f5a654369a5b41d18f0b44b38f9cc753c773f384808b9a18cac92' );

#
# 9) Test gray 4 bit image (with matte channel)
#
++$test;
print("Gray image (4 bits per sample + matte channel) ...\n");
testReadWrite(  'input_gray_04bit_matte.tiff',
  'output_gray_04bit_matte.tiff',
  q//,
  '1e4ca36f33b13d8799bcda20108d20c1a2da913c27eb7872d95a6928cb8b0b48' );

#
# 10) Test gray 8 bit image (with matte channel)
#
++$test;
print("Gray image (8 bits per sample + matte channel) ...\n");
testReadWrite(  'input_gray_08bit_matte.tiff',
  'output_gray_08bit_matte.tiff',
  q//,
  '70e1e096e53af6a312ec6aa54890a59b35d3c6932dfc052d6b355f62733635be' );

#
# 11) CALS Type 1
#
++$test;
print("CALS Type 1 ...\n");
testReadWrite(  'input_gray_01bit_minwhite.cals',
  'output_gray_01bit_minwhite.cals',
  q//,
  '2b48bcf7d93cc31a8deb3026d872f1bb0a300b0f4e177423e5301adc638179e4' );
