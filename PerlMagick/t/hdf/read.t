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
testRead( 'input_256.hdf',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c' );

#
# 2) Test TrueColor HDF
# 
++$test;
testRead( 'input_truecolor.hdf',
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c' );
