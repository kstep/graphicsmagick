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
testRead( 'input_256.hdf', '38b595352bc43057502f53acaef76ba8',
                           '8d953a68ec2c8b00f024f88c2ef6ac35' );

#
# 2) Test TrueColor HDF
# 
++$test;
testRead( 'input_truecolor.hdf', 'bf73230f28bb7b54be7f59bd0106937c',
                                 '286a8f3b24cfe7c7999593d9f2ab777d' );
