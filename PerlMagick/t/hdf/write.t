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
   'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1' );

#
# 2) Test truecolor image
#
++$test;
testReadWrite( 'input_truecolor.hdf',
   'output_truecolor.hdf',
   q/quality=>55/,
   'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1' );
