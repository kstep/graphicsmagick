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
  '14728f2a66b423f570809643aa21565be566c3e8e4b158c31a98c492561b5fc3' );

#
# 2) Test Monochrome PNG
# 
++$test;
print( "8-bit grayscale PNG ...\n" );
testRead( 'input_mono.png',
  '0f3293f35f2ffcec30b424669caa5c3ff7f5286448f7ae6bffdabffffc345d39' );

#
# 3) Test 16-bit Portable Network Graphics
# 
++$test;
print( "16-bit grayscale PNG ...\n" );
testRead( 'input_16.png',
  'cf6951d0b2d210567c715369bd2ea80b468f5bbec1d1ef26bf5cf4751dc8ec16',
  '4eeef86b727f91d7f4bddfe8edbd7264ab835208c1b4edb199fdb40c1cdcba9f' );
#
# 4) Test 256 color pseudocolor PNG
# 
++$test;
print( "8-bit indexed-color PNG ...\n" );
testRead( 'input_256.png',
  'c20a56cc7d1735770bcdc4079e9aaba42dd05fc22af5243e5c1abd8aa6b038a2' );

#
# 5) Test TrueColor PNG
# 
++$test;
print( "24-bit Truecolor PNG ...\n" );
testRead( 'input_truecolor.png',
  '4c8d525a63adb1badde07442350d8d1d26e7567698df7dfd0c0a02c217dc3f5c' );

#
# 6) Test Multiple-image Network Graphics
# 
++$test;
print( "MNG with 24-bit Truecolor PNGs...\n" );
testRead( 'input.mng',
  'ab33f4c8a7b53b966123a745c08596f9ae0cc5c6a1e72d07e1c4fe0ce2fa08c2' );

