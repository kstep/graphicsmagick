#!/usr/local/bin/perl
#
# Test writing TIFF images
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
# 1) Test pseudocolor image
#
testReadWrite( 'input_256.tiff',
	       'output_256.tiff',
	       q/quality=>54/,
	       'e026b9f6e8d28422055dda3013e6e579' );

#
# 2) Test truecolor image
#
++$test;
testReadWrite( 'input_truecolor.tiff',
	       'output_truecolor.tiff',
	       q/quality=>55/,
	       '3774342c000dde64d8c7c309061251d6' );

#
# 3) Test monochrome image
#
++$test;
testReadWrite(  'input_mono.tiff',
		'output_mono.tiff',
		q//,
		'17fc11e2b072769329e2e8df4f9e6fe1' );
