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
testRead( 'input_256.hdf', '0acbc70bac502726c1b72d3c3ff4d0fe',
                           '8d953a68ec2c8b00f024f88c2ef6ac35' );

#
# 2) Test TrueColor HDF
# 
++$test;
testRead( 'input_truecolor.hdf', '3ada2b040cb9b94b2426b2681cd39106',
                                 '286a8f3b24cfe7c7999593d9f2ab777d' );
