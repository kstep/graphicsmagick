#!/usr/local/bin/perl
#
# Test reading TIFF images
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
# 1) Test Reading Monochrome
# 
print("Monochrome ...\n");
testRead ( 'input_mono.tiff',
  'e09c8eb1ccbf32e3a4062bad04b96dbc9209bc7897b0a63ac8dc2c8e0ccc3d92' );

#
# 2) Test reading PseudoColor
#
++$test;
print("PseudoColor ...\n");
testRead( 'input_256.tiff',
  'e62f8aa77283eaf46c0bdcc8feb34d16bf200b2f964ff6cff725b573f1a6b039' );

#
# 3) Test Reading TrueColor
# 
++$test;
print("TrueColor image ...\n");
testRead( 'input_truecolor.tiff',
  'c754de02d6e61cb9859fda13603b97fb51213aa56ea72beaf0cb553f31a1c5e2' );

