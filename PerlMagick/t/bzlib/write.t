#!/usr/local/bin/perl
#
# Test writing files using bzlib-based compression
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..1\n"; }
END {print "not ok $test\n" unless $loaded;}

use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/bzlib' || die 'Cd failed';

#
# Test writing BZip-compressed MIFF
#

testReadWrite( 'input.miff',
  'output.miff',
  q/compress=>'BZip'/,
  'bc6bf6d18ff76a1f422d913671437c595f0bb475ce5b0ffd8c915179d6918298' );

$test = 0;  # Quench PERL compliaint

