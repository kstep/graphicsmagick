#!/usr/local/bin/perl
#
# Test writing PNG images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..6\n"; }
END {print "not ok $test\n" unless $loaded;}

use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/png' || die 'Cd failed';

#
# 1) Test Black-and-white, bit_depth=1 PNG
# 
print( "1-bit grayscale PNG ...\n" );
testReadWrite( 'input_bw.png', 'output_bw.png', q/quality=>95/,
               'dbb89a80821a79ddd91bc3e994306f72',
	       '3b69a5385ee461dee7b6faf5c6c0b922');

#
# 2) Test monochrome image
#
++$test;
print( "8-bit grayscale PNG ...\n" );
testReadWrite( 'input_mono.png',
	       'output_mono.png', '',
	       'a1f6cc3e45f2d8c5d01b79355aa90e05',
	       '3a25beb452133a9b53e812ac409e44ee');
#
# 3) Test 16-bit Portable Network Graphics
# 
++$test;
print( "16-bit grayscale PNG ...\n" );
testReadWrite( 'input_16.png',
	       'output_16.png',
	       q/quality=>55/,
               'e29e5f11479a4a07f035d932cac1f66f',
               '283037a3703baa1595b6c146a4187baa' );
#
# 4) Test pseudocolor image
#
++$test;
print( "8-bit indexed-color PNG ...\n" );
testReadWrite( 'input_256.png',
	       'output_256.png',
	       q/quality=>54/,
	       '38b595352bc43057502f53acaef76ba8' );
#
# 5) Test truecolor image
#
++$test;
print( "24-bit Truecolor PNG ...\n" );
testReadWrite( 'input_truecolor.png',
	       'output_truecolor.png',
	       q/quality=>55/,
	       'bf73230f28bb7b54be7f59bd0106937c' );
#
# 6) Test Multiple-image Network Graphics
#
++$test;
print( "MNG with 24-bit Truecolor PNGs ...\n" );
testReadWrite( 'input.mng',
	       'output.mng',
	       q/quality=>55/,
	       'd072eb3997b849f84172d162c266389a' );
