#!/usr/local/bin/perl
#
# Test reading HDF images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#

BEGIN { $| = 1; $test=1; print "1..2\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/hdf' || die 'Cd failed';

#
# 1) Test 256 color pseudocolor HDF
# 
testRead( 'input_256.hdf', '8de05f913b3269ce6acafbfba975837f' );

#
# 2) Test TrueColor HDF
# 
++$test;
testRead( 'input_truecolor.hdf', '02434255ffa016599b9f33031c2884e9' );
