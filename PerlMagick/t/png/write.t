#!/usr/local/bin/perl
#
# Test writing PNG images
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
testReadWrite( 'input_bw.png', 'output_bw.png', q/quality=>95/,
  '552ee0b3b9159ceedc4ce30125e9db38d2045d53b941844a72ac5f5f38335454');

#
# 2) Test monochrome image
#
++$test;
print( "8-bit grayscale PNG ...\n" );
testReadWrite( 'input_mono.png',
  'output_mono.png', '',
  '19da3123692c66d023074dd1e272119ab2357b9a34855a393ad18c88af01ae86');
#
# 3) Test 16-bit Portable Network Graphics
# 
++$test;
print( "16-bit grayscale PNG ...\n" );
testReadWrite( 'input_16.png',
  'output_16.png',
  q/quality=>55/,
  'a6c3f3ce8772cd6aa973ae1f3ebd94ee65292112d3f1e8e86171bf4d931bd181',
  'c87cc12715f3e0619d6fe871bd8132f88facffb3d38dd8869cb262ec6b9c4cef' );
#
# 4) Test pseudocolor image
#
++$test;
print( "8-bit indexed-color PNG ...\n" );
testReadWrite( 'input_256.png',
  'output_256.png',
  q/quality=>54/,
  'd1340caccc7351834f1ca9693489ee780ac326a0d6bb660cd550c5c28711b2fe' );
#
# 5) Test truecolor image
#
++$test;
print( "24-bit Truecolor PNG ...\n" );
testReadWrite( 'input_truecolor.png',
  'output_truecolor.png',
  q/quality=>55/,
  'b84c31bbe78d84d0432c7414b25c58599484aa2c7174af8332509966d07f265b' );
#
# 6) Test Multiple-image Network Graphics
#
++$test;
print( "MNG with 24-bit Truecolor PNGs ...\n" );
testReadWrite( 'input.mng',
  'output.mng',
  q/quality=>55/,
  'eb089161ebc5ab3964cdec1b72628c4d1c29ebd78f333a3d4a0d47c614fb3897' );
