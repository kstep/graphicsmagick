#!/usr/local/bin/perl
#
# Test writing PNG images when 16bit support is enabled
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
	       '38b595352bc43057502f53acaef76ba8' );

#
# 2) Test truecolor image
#
++$test;
testReadWrite( 'input_truecolor.png',
	       'output_truecolor.png',
	       q/quality=>55/,
	       'bf73230f28bb7b54be7f59bd0106937c' );

#
# 3) Test monochrome image
#
++$test;
testReadWrite( 'input_mono.png',
	       'output_mono.png', '',
	       'a1f6cc3e45f2d8c5d01b79355aa90e05' );

#
# 4) Test Multiple-image Network Graphics
#
++$test;
testReadWrite( 'input.mng',
	       'output.mng',
	       q/quality=>55/,
	       'd072eb3997b849f84172d162c266389a' );

#
# 5) Test 16-bit Portable Network Graphics
# 
++$test;
testReadWrite( 'input_16.png',
	       'output_16.png',
	       q/quality=>55/,
               'f20dcdb4be59aa17f5e0996d9f2fc789',
	       '890af6c2fae2ed79795143815900e950');


