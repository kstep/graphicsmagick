#!/usr/local/bin/perl
#
# Test writing PNG images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..5\n"; }
END {print "not ok $test\n" unless $loaded;}

use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/png' || die 'Cd failed';

#
# 1) Test pseudocolor image
#
testReadWrite( 'input_256.png',
	       'output_256.png',
	       q/quality=>54/,
	       '0acbc70bac502726c1b72d3c3ff4d0fe', 'same' );

#
# 2) Test truecolor image
#
++$test;
testReadWrite( 'input_truecolor.png',
	       'output_truecolor.png',
	       q/quality=>55/,
	       '3ada2b040cb9b94b2426b2681cd39106', 'same' );

#
# 3) Test monochrome image
#
++$test;
testReadWrite( 'input_mono.png',
	       'output_mono.png', '',
	       '8d63ef0e910116f485da5a8b6a3cc7a9', 'same' );

#
# 4) Test Multiple-image Network Graphics
#
++$test;
testReadWrite( 'input.mng',
	       'output.mng',
	       q/quality=>55/,
	       '2ab9b274742f981c30ff745cfc355ad8', 'same' );

#
# 5) Test 16-bit Portable Network Graphics
# 
++$test;
testReadWrite( 'input_16.png',
	       'output_16.png',
	       q/quality=>55/,
               '7a3e933e0130b1fff2b1884f6e65a50f',
               '7ffc115d68c1299b12c78772714af23f' );



#
# 6) Test Black-and-white, bit_depth=1 PNG
# 
++$test;
testReadWrite( 'input_bw.png', output_bw.png, q/quality=>95/,
               '8703ee7acfb2e12fea0ba8519b5f6d15', 'same' );
