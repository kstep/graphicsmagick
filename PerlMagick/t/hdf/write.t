#!/usr/local/bin/perl
#
# Test writing HDF images
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
# 1) Test pseudocolor image
#
testReadWrite( 'input_256.hdf',
   'output_256.hdf',
   q/quality=>54/,
   'bc6bf6d18ff76a1f422d913671437c595f0bb475ce5b0ffd8c915179d6918298' );

#
# 2) Test truecolor image
#
++$test;
testReadWrite( 'input_truecolor.hdf',
   'output_truecolor.hdf',
   q/quality=>55/,
   'bc6bf6d18ff76a1f422d913671437c595f0bb475ce5b0ffd8c915179d6918298' );
