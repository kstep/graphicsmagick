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
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca' );

#
# 2) Test TrueColor HDF
# 
++$test;
testRead( 'input_truecolor.hdf',
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca' );
