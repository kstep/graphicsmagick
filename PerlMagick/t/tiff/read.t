#!/usr/local/bin/perl
# Copyright (C) 2003, 2004, 2005 GraphicsMagick Group
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
# Written by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..27\n"; }
END {print "not ok $test\n" unless $loaded;}

use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/tiff' || die 'Cd failed';

#
# 1) Test Reading Monochrome
# 
print("Monochrome (min-is-white) ...\n");
testRead ( 'input_gray_01bit_minwhite.tiff',
  '2b48bcf7d93cc31a8deb3026d872f1bb0a300b0f4e177423e5301adc638179e4' );

#
# 2) Test reading PseudoColor (16 color)
#
++$test;
print("PseudoColor (16 color)...\n");
testRead( 'input_palette_16.tiff',
  'a0313f6235a3158655412e1480c2f37549d89a5cacf40abc854811af1b757159' );

#
# 3) Test reading PseudoColor (16 color + matte channel)
#
++$test;
print("PseudoColor (16 color + matte channel) ...\n");
testRead( 'input_palette_16_matte.tiff',
  '631c0997280217f3f430336513f1e51fe03e04f552d49a848449a438c100aa20' );

#
# 4) Test reading PseudoColor (256 color)
#
++$test;
print("PseudoColor (256 color) ...\n");
testRead( 'input_palette_256.tiff',
  'a570df6d4c214d08bccaaee856f617883fee4f96494af6ef5e25c76d86ea0e0a' );

#
# 5) Test reading PseudoColor (256 color + matte channel)
#
++$test;
print("PseudoColor (256 color + matte channel) ...\n");
testRead( 'input_palette_256_matte.tiff',
  'a3f865320dc8b7953850babbd22d468d821c0103cc14063d6459159506a910d6' );

#
# 6) Test reading PseudoColor using contiguous planar packing
#
++$test;
print("PseudoColor (256 color) contiguous planes ...\n");
testRead( 'input_palette_256_planar_contig.tiff',
  'a570df6d4c214d08bccaaee856f617883fee4f96494af6ef5e25c76d86ea0e0a' );

#
# 7) Test reading PseudoColor using seperate planes
#
++$test;
print("PseudoColor (256 color) seperate planes ...\n");
testRead( 'input_palette_256_planar_separate.tiff',
  'a570df6d4c214d08bccaaee856f617883fee4f96494af6ef5e25c76d86ea0e0a' );

#
# 8) Test Reading TrueColor (8-bit)
# 
++$test;
print("TrueColor (8-bit) image ...\n");
testRead( 'input_truecolor_08.tiff',
  '326599218925e7aa27c735d482332744cccf41f460a1ba2d276a2d172f2b3de0' );

#
# 9) Test Reading TrueColor (8-bit + matte channel)
# 
++$test;
print("TrueColor (8-bit) image with alpha channel ...\n");
testRead( 'input_truecolor_08_matte.tiff',
  'e4f197c0de1bd1e9c43c3659953a4af81dda6d56433840c6ce508e3a2bb757e4' );

#
# 10) Test Reading TrueColor (10-bit)
# 
++$test;
print("TrueColor (10-bit) image ...\n");
testRead( 'input_truecolor_10.tiff',
  'e70080518fa43632e7896300d6614a4d1c39ad02efd3d6ab8c70a35f63364193',
  'f38f0d191bce9aebd4e99229dfbe07e82fc68d8b95d9049abb53c532f471f5e0',
  '59178f8b7f9df6dd59b2965161100db5b11fd4d46d0a790c7393c82f8b6a4f84' );

#
# 11) Test Reading TrueColor (12-bit)
# 
++$test;
print("TrueColor (12-bit) image ...\n");
testRead( 'input_truecolor_12.tiff',
  'e70080518fa43632e7896300d6614a4d1c39ad02efd3d6ab8c70a35f63364193',
  'db9e9a4bc0b315f4dea136fbfd90181909cefb9bbdafd7f47d9c267b54f9197c',
  '56ff48dbfbe3602198010130717523664d83068cd41cf27422414bd4385abda0' );

#
# 12) Test Reading TrueColor (14-bit)
# 
++$test;
print("TrueColor (14-bit) image ...\n");
testRead( 'input_truecolor_14.tiff',
  'e70080518fa43632e7896300d6614a4d1c39ad02efd3d6ab8c70a35f63364193',
  '8a897778184d453e5e681943e286f6cd34c8d5004a00132e81613debcd333f8f',
  '412b3f0188113505c79dc226eef901b25e5f66d8bae28eb832e48c89eb139e81' );

#
# 13) Test Reading TrueColor (16-bit)
#
++$test;
print("TrueColor (16-bit) image ...\n");
testRead( 'input_truecolor_16.tiff',
  '1030dc73bdc72c846831d10b661539e7a9e9c24a170f24ffdffaeae1a8aa769d',
  '72fd047e4f5942b4edc96e8b16444580e9eb1f265017dd6bb11795a842e58d58' );

#
# 14) Test Reading TrueColor (32-bit)
#
++$test;
print("TrueColor (32-bit) image ...\n");
testRead( 'input_truecolor_32.tiff',
  '2f32110a0c8e1c360420e49e02bea23de84e5772581326185b38629b69fc5d2a',
  '845c70b06bea5c43360fe50b8e6e70009631bb761ce5e96e970a8587fee60cb5',
  '8fa932366715755135be018e7da7b4417b2628f582a1ba5b371a80a840f5b791');

#
# 15) Test Reading 8-bit TrueColor Tiled (32x32 tiles)
#
++$test;
print("TrueColor (8-bit) tiled image, 32x32 tiles ...\n");
testRead( 'input_truecolor_08_tiled32x32.tiff',
  '326599218925e7aa27c735d482332744cccf41f460a1ba2d276a2d172f2b3de0' );

#
# 16) Test Reading 8-bit TrueColor Planar
#
++$test;
print("TrueColor (8-bit) planar image ...\n");
testRead( 'input_truecolor_08_planar.tiff',
  '326599218925e7aa27c735d482332744cccf41f460a1ba2d276a2d172f2b3de0' );

#
# 17) Test Reading 8-bit TrueColor Stripped (8 rows per strip)
# 
++$test;
print("TrueColor (8-bit) stripped image, 8 rows per strip ...\n");
testRead( 'input_truecolor_08_stripped.tiff',
  '326599218925e7aa27c735d482332744cccf41f460a1ba2d276a2d172f2b3de0' );

#
# 18) Test Reading Grayscale 4-bit
#
++$test;
print("Grayscale (4-bit) ...\n");
testRead( 'input_gray_04bit.tiff',
  '45c0655a417852480336d5e12d7bb16b9647d911b11589e9d000a05dd1813570');

#
# 19) Test Reading Grayscale 4-bit + matte
#
++$test;
print("Grayscale (4-bit + matte) ...\n");
testRead( 'input_gray_04bit_matte.tiff',
  '1e4ca36f33b13d8799bcda20108d20c1a2da913c27eb7872d95a6928cb8b0b48');

#
# 20) Test Reading Grayscale 8-bit
# 
++$test;
print("Grayscale (8-bit) ...\n");
testRead( 'input_gray_08bit.tiff',
  '00f6147d547f5a654369a5b41d18f0b44b38f9cc753c773f384808b9a18cac92');

#
# 21) Test Reading Grayscale 8-bit + matte
# 
++$test;
print("Grayscale (8-bit + matte) ...\n");
testRead( 'input_gray_08bit_matte.tiff',
  '70e1e096e53af6a312ec6aa54890a59b35d3c6932dfc052d6b355f62733635be');

#
# 22) Test Reading Grayscale 10-bit
# 
++$test;
print("Grayscale (10-bit) ...\n");
testRead( 'input_gray_10bit.tiff',
  'c52de8e70c226c7872e1384e119532791ea9955a01485ae68bbc1236d3f9f9ee',
  'b278e43185a407d0419768dc57f2e714597cdb8f3b6c80278a6307af68a0a5f1',
  'b5be0d42f6e491343f86744af8fcd4b51ef70373fad69593046a27ad2d805788');

#
# 23) Test Reading Grayscale 12-bit
# 
++$test;
print("Grayscale (12-bit) ...\n");
testRead( 'input_gray_12bit.tiff',
  'c52de8e70c226c7872e1384e119532791ea9955a01485ae68bbc1236d3f9f9ee',
  'df4cf35c2408487c2727ed288908d563ecb758260d09ce8605839394db930505',
  '4d382632a407732017ec16d6cbcb92a81cdb56150e9aedce1b987ac9b68d5948');

#
# 24) Test Reading Grayscale 14-bit
# 
++$test;
print("Grayscale (14-bit) ...\n");
testRead( 'input_gray_14bit.tiff',
  'c52de8e70c226c7872e1384e119532791ea9955a01485ae68bbc1236d3f9f9ee',
  'a8ecb9b559640e7db2505543cd85c7a614460bed067cf197e3d2116246b1fd05',
  'e61921b0de711a9a109cb97920a4487852f0aea9fdb255c357b8290be408ebaa');

#
# 25) Test Reading Grayscale 16-bit
# 
++$test;
print("Grayscale (16-bit) ...\n");
testRead( 'input_gray_16bit.tiff',
  '00f6147d547f5a654369a5b41d18f0b44b38f9cc753c773f384808b9a18cac92',
  '1db9216ffb84dab419b4924e6aba945e922f728c0dca8ce24c3d7f4c47c604b9' );

#
# 26) Test Reading Grayscale 32-bit
# 
++$test;
print("Grayscale (32-bit) ...\n");
testRead( 'input_gray_32bit.tiff',
  '3317287375433c8a85db4e5f922d6cf8f5597e337cfed3da3d7c090638296f50',
  'e42d156f5c3278c3083409b6d2995b37efca165e1c437b2e25f4c67b4248a574',
  'b82c3738aadf26ee25b28d17eeb9cb185d6c7794d7e676692a2cacd90642973e');

#
# 27) Test Reading CALS Type 1 (Group4-based)
# 
++$test;
print("CALS Type 1...\n");
testRead ( 'input_gray_01bit_minwhite.cals',
  '2b48bcf7d93cc31a8deb3026d872f1bb0a300b0f4e177423e5301adc638179e4' );
