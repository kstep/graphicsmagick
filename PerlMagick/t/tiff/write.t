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
	       '07babaaf9f2622e1176a2ab7caf4012d',
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
               '4d3c3e472ace236f15b27c7af6e3d464' );

#
# 3) Test monochrome image
#
++$test;
print("Monochrome image ...\n");
testReadWrite(  'input_mono.tiff',
		'output_mono.tiff',
		q//,
		'8023e6a23a15ab1240b48ec6c085ed40',
		'ae8254aa2869a317f789629aa4276f0a' );
