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
  'b7ff35dedb72b47cd178219ed30a2380da9f900306ffbe79222827259382be54' );

#
# 2) Test reading PseudoColor
#
++$test;
print("PseudoColor ...\n");
testRead( 'input_256.tiff',
  '23ff59c59294aa8e8ee53c9769a0f09127968478dcbacc44b7a77bab44bf5afc' );

#
# 3) Test Reading TrueColor
# 
++$test;
print("TrueColor image ...\n");
testRead( 'input_truecolor.tiff',
  '1144b91e9575def0065c904809a952e9a412203dff46d8d02b68060c9bf580cc' );

