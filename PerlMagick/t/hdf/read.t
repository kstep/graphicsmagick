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
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f' );

#
# 2) Test TrueColor HDF
# 
++$test;
testRead( 'input_truecolor.hdf',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f' );
