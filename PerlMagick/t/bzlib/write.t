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
	       'eb4d6e084afe2835a1ad28ad7fc12ced' );

$test = 0;  # Quench PERL compliaint

