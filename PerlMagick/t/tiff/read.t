#!/usr/local/bin/perl
#
# Test reading TIFF images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..3\n"; }
END {print "not ok $test\n" unless $loaded;}

use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/tiff' || die 'Cd failed';

#
# 1) Test Reading Monochrome
# 
testRead ( 'input_mono.tiff', '17fc11e2b072769329e2e8df4f9e6fe1' );

#
# 2) Test reading PseudoColor
#
++$test;
testRead( 'input_256.tiff', 'e026b9f6e8d28422055dda3013e6e579' );

#
# 3) Test Reading TrueColor
# 
++$test;
testRead( 'input_truecolor.tiff', '3774342c000dde64d8c7c309061251d6' );

