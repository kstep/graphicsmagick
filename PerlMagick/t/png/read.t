#!/usr/local/bin/perl
#
# Test reading PNG images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#

BEGIN { $| = 1; $test=1; print "1..6\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/png' || die 'Cd failed';

#
# 1) Test Black-and-white, bit_depth=1 PNG
# 
print( "1-bit grayscale PNG ...\n" );
testRead( 'input_bw.png',
  '72262d15571bba86bf35454447e0299869678ea4e523688c66b1970e0735cc43' );

#
# 2) Test Monochrome PNG
# 
++$test;
print( "8-bit grayscale PNG ...\n" );
testRead( 'input_mono.png',
  '064000205c670c86e2e7521c7c5a437278da6b72e66173ee2c9cd33f27626907' );

#
# 3) Test 16-bit Portable Network Graphics
# 
++$test;
print( "16-bit grayscale PNG ...\n" );
testRead( 'input_16.png',
  '415303615a36716dd3c7aade7b0d991b4fe04178f48c1725e01675226c1647de',
  '4eeef86b727f91d7f4bddfe8edbd7264ab835208c1b4edb199fdb40c1cdcba9f' );
#
# 4) Test 256 color pseudocolor PNG
# 
++$test;
print( "8-bit indexed-color PNG ...\n" );
testRead( 'input_256.png',
  '852777db495289e4e42f98fd0136d4831caed8f23ef7f56503698b24d90467ce' );

#
# 5) Test TrueColor PNG
# 
++$test;
print( "24-bit Truecolor PNG ...\n" );
testRead( 'input_truecolor.png',
  'f3a02ab84dc50e6d4f57f4dbfd801d565391809e9affb95271a1a2393d2d50ef' );

#
# 6) Test Multiple-image Network Graphics
# 
++$test;
print( "MNG with 24-bit Truecolor PNGs...\n" );
testRead( 'input.mng',
  '1d76d4e1f1e085147a7a3379fc8df0413b2c7072ef247f7c52024f4104068919' );

