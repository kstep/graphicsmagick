#!/usr/local/bin/perl
#
# Test writing TIFF images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..3\n"; }
END {print "not ok $test\n" unless $loaded;}

use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/tiff' || die 'Cd failed';

#
# 1) Test pseudocolor image
#
print("PsuedoColor image ...\n");
testReadWrite( 'input_256.tiff',
  'output_256.tiff',
  q/quality=>54/,
  'a7e9a0f0db24b03fc68ea1776b7418f5411fe0143b063cdd547749e2918d53e9');

#
# 2) Test truecolor image
#
++$test;
print("TrueColor image ...\n");
testReadWrite( 'input_truecolor.tiff',
  'output_truecolor.tiff',
  q/quality=>55/,
  'cb5a792d4a12e00363bf3fb73dc7687af3417716b2231e6d2ba04f7874e7a760' );

#
# 3) Test monochrome image
#
++$test;
print("Monochrome image ...\n");
testReadWrite(  'input_mono.tiff',
  'output_mono.tiff',
  q//,
  '88f1ac676d66c7a027328809a3c596ffe9eda97304ccfe41ffcca887150a6d42' );
