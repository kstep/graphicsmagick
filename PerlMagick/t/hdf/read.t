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
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1' );

#
# 2) Test TrueColor HDF
# 
++$test;
testRead( 'input_truecolor.hdf',
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1' );
