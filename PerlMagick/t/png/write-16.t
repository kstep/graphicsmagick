#!/usr/local/bin/perl
#
# Test writing PNG images when 16bit support is enabled
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..5\n"; }
END {print "not ok $test\n" unless $loaded;}

use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/png' || die 'Cd failed';

#
# 1) Test pseudocolor image
#
testReadWrite( 'input_256.png',
  'output_256.png',
  q/quality=>54/,
  '123c57a0f57f7be7ffcfb1bd428f568c23f939f5812112a986ffbbc19a9f6c0f' );

#
# 2) Test truecolor image
#
++$test;
testReadWrite( 'input_truecolor.png',
  'output_truecolor.png',
  q/quality=>55/,
  '5b48d510d17bd4c68b875a9689e8ea17caacbe7f3d566c162016e24f2ef875fd' );

#
# 3) Test monochrome image
#
++$test;
testReadWrite( 'input_mono.png',
  'output_mono.png', '',
  '98688672bd9fa81731c8d442d24e655004d11ef17e6b36ceb70de5c01b683e81' );

#
# 4) Test Multiple-image Network Graphics
#
++$test;
testReadWrite( 'input.mng',
  'output.mng',
  q/quality=>55/,
  'f1467cb3a1ed311e5e8ddf7b8bfde62353709991e2f1c7fc9a06db8d413ba336' );

#
# 5) Test 16-bit Portable Network Graphics
# 
++$test;
testReadWrite( 'input_16.png',
  'output_16.png',
  q/quality=>55/,
  '72b2c72d4e90436dd45adb77f992e1ed3c26e327a5ce5b068bd515b49039b6df',
  '07696b623faf33d67921b80b29ff905ff9d546c445a54026a32d263adbf5f6b9');


