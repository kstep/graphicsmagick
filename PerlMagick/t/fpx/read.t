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
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1' );

#
# 2) Test Monochrome FPX
# 
++$test;
print( "8-bit grayscale FPX ...\n" );
testRead( 'input_mono.fpx',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1' );

#
# 3) Test 256 color pseudocolor FPX
# 
++$test;
print( "8-bit indexed-color FPX ...\n" );
testRead( 'input_256.fpx',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1' );

#
# 4) Test TrueColor FPX
# 
++$test;
print( "24-bit Truecolor FPX ...\n" );
testRead( 'input_truecolor.fpx',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1' );

#
# 5) Test JPEG FPX
# 
++$test;
print( "24-bit JPEG FPX ...\n" );
testRead( 'input_jpeg.fpx',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1' );

