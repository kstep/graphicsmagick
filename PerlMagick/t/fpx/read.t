#!/usr/local/bin/perl
#
# Test reading FPX images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#

BEGIN { $| = 1; $test=1; print "1..5\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/fpx' || die 'Cd failed';

#
# 1) Test Black-and-white, bit_depth=1 FPX
# 
print( "1-bit grayscale FPX ...\n" );
testRead( 'input_bw.fpx',
  '2dbce2cb797dfcc2ba56ecc98d71d8a886fd3671499bc89def53f4dfd7cb5a27' );

#
# 2) Test Monochrome FPX
# 
++$test;
print( "8-bit grayscale FPX ...\n" );
testRead( 'input_mono.fpx',
  '2dbce2cb797dfcc2ba56ecc98d71d8a886fd3671499bc89def53f4dfd7cb5a27' );

#
# 3) Test 256 color pseudocolor FPX
# 
++$test;
print( "8-bit indexed-color FPX ...\n" );
testRead( 'input_256.fpx',
  '04b59abb64be8a20693101544c92ad1db9ef62acbed03e4439daea1886176673' );

#
# 4) Test TrueColor FPX
# 
++$test;
print( "24-bit Truecolor FPX ...\n" );
testRead( 'input_truecolor.fpx',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c' );

#
# 5) Test JPEG FPX
# 
++$test;
print( "24-bit JPEG FPX ...\n" );
testRead( 'input_jpeg.fpx',
  '3b107cc41f45d34500335cf22a7aea3551d24530a214cbac995b6596ec127f03' );

