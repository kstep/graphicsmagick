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
print("PsuedoColor image ...\n");
testReadWrite( 'input_256.tiff',
	       'output_256.tiff',
	       q/quality=>54/,
	       '6c73bcb104e206db81ea91208753f4a7',
               'f90f998ebc41c7c9833ea2a454791d70');

#
# 2) Test truecolor image
#
++$test;
print("TrueColor image ...\n");
testReadWrite( 'input_truecolor.tiff',
	       'output_truecolor.tiff',
	       q/quality=>55/,
	       '8023e6a23a15ab1240b48ec6c085ed40',
               '7877671f634ee3e8b3bc09bc7d2d21c8' );

#
# 3) Test monochrome image
#
++$test;
print("Monochrome image ...\n");
testReadWrite(  'input_mono.tiff',
		'output_mono.tiff',
		q//,
		'782b370d660dff407e883221b20ffbcc',
		'8023e6a23a15ab1240b48ec6c085ed40' );
