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
  'f3afd8f931e3be3ddd7e99c43bd5d854257dee6726cdc546962d60c5d9405047' );

#
# 2) Test Monochrome FPX
# 
++$test;
print( "8-bit grayscale FPX ...\n" );
testRead( 'input_mono.fpx',
  'f3afd8f931e3be3ddd7e99c43bd5d854257dee6726cdc546962d60c5d9405047' );

#
# 3) Test 256 color pseudocolor FPX
# 
++$test;
print( "8-bit indexed-color FPX ...\n" );
testRead( 'input_256.fpx',
  'd120f2f0f295b506a92070051dd8e7b4a83f777909fd4e73d75bd55c53a55a95' );

#
# 4) Test TrueColor FPX
# 
++$test;
print( "24-bit Truecolor FPX ...\n" );
testRead( 'input_truecolor.fpx',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f' );

#
# 5) Test JPEG FPX
# 
++$test;
print( "24-bit JPEG FPX ...\n" );
testRead( 'input_jpeg.fpx',
  '45a41e0ef7ad4b2ab890978457f1c51919139d6036bbf27f3e0adfb7732c0b48' );

