#!/usr/local/bin/perl
#
# Test writing FPX images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..4\n"; }
END {print "not ok $test\n" unless $loaded;}

use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/fpx' || die 'Cd failed';

#
# 1) Test Black-and-white, bit_depth=1 FPX
# 
print( "1-bit grayscale FPX ...\n" );
testReadWrite( 'input_bw.fpx', 'output_bw.fpx', q/quality=>95/,
   'bc6bf6d18ff76a1f422d913671437c595f0bb475ce5b0ffd8c915179d6918298');

#
# 2) Test monochrome image
#
++$test;
print( "8-bit grayscale FPX ...\n" );
testReadWrite( 'input_mono.fpx',
   'output_mono.fpx', '',
   'bc6bf6d18ff76a1f422d913671437c595f0bb475ce5b0ffd8c915179d6918298');
#
# 3) Test pseudocolor image
#
++$test;
print( "8-bit indexed-color FPX ...\n" );
testReadWrite( 'input_256.fpx',
   'output_256.fpx',
   q/quality=>54/,
   'bc6bf6d18ff76a1f422d913671437c595f0bb475ce5b0ffd8c915179d6918298' );
#
# 4) Test truecolor image
#
++$test;
print( "24-bit Truecolor FPX ...\n" );
testReadWrite( 'input_truecolor.fpx',
   'output_truecolor.fpx',
   q/quality=>55/,
   'bc6bf6d18ff76a1f422d913671437c595f0bb475ce5b0ffd8c915179d6918298' );
