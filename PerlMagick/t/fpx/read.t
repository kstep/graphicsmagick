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
  'c5ae39a246e55bb2469ef04e94172bd3d96cf8c6aa3f8f181b06b5e2a85028e7' );

#
# 2) Test Monochrome FPX
# 
++$test;
print( "8-bit grayscale FPX ...\n" );
testRead( 'input_mono.fpx',
  'c5ae39a246e55bb2469ef04e94172bd3d96cf8c6aa3f8f181b06b5e2a85028e7' );

#
# 3) Test 256 color pseudocolor FPX
# 
++$test;
print( "8-bit indexed-color FPX ...\n" );
testRead( 'input_256.fpx',
  'da7222056cafc58e05788c5b91fa67d24bc73a9f027ee285548d8d7c11e18895' );

#
# 4) Test TrueColor FPX
# 
++$test;
print( "24-bit Truecolor FPX ...\n" );
testRead( 'input_truecolor.fpx',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca' );

#
# 5) Test JPEG FPX
# 
++$test;
print( "24-bit JPEG FPX ...\n" );
testRead( 'input_jpeg.fpx',
  '0b0df9e4487c60a0af32663a6785e0c1a71e98a9351612d4783abf2d1c6aeac5' );

