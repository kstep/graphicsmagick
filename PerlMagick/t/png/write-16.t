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
  'c20a56cc7d1735770bcdc4079e9aaba42dd05fc22af5243e5c1abd8aa6b038a2' );

#
# 2) Test truecolor image
#
++$test;
testReadWrite( 'input_truecolor.png',
  'output_truecolor.png',
  q/quality=>55/,
  '4c8d525a63adb1badde07442350d8d1d26e7567698df7dfd0c0a02c217dc3f5c' );

#
# 3) Test monochrome image
#
++$test;
testReadWrite( 'input_mono.png',
  'output_mono.png', '',
  '0f3293f35f2ffcec30b424669caa5c3ff7f5286448f7ae6bffdabffffc345d39' );

#
# 4) Test Multiple-image Network Graphics
#
++$test;
testReadWrite( 'input.mng',
  'output.mng',
  q/quality=>55/,
  'ab33f4c8a7b53b966123a745c08596f9ae0cc5c6a1e72d07e1c4fe0ce2fa08c2' );

#
# 5) Test 16-bit Portable Network Graphics
# 
++$test;
testReadWrite( 'input_16.png',
  'output_16.png',
  q/quality=>55/,
  'cf6951d0b2d210567c715369bd2ea80b468f5bbec1d1ef26bf5cf4751dc8ec16',
  '4eeef86b727f91d7f4bddfe8edbd7264ab835208c1b4edb199fdb40c1cdcba9f');


