#!/usr/local/bin/perl
#
# Test reading PNG images when 16bit support is enabled
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
# 1) Test Monochrome PNG
# 
testRead( 'input_mono.png', 'a1f6cc3e45f2d8c5d01b79355aa90e05' );

#
# 2) Test 256 color pseudocolor PNG
# 
++$test;
testRead( 'input_256.png', '38b595352bc43057502f53acaef76ba8' );

#
# 3) Test TrueColor PNG
# 
++$test;
testRead( 'input_truecolor.png', 'bf73230f28bb7b54be7f59bd0106937c' );

#
# 4) Test Multiple-image Network Graphics
# 
++$test;
testRead( 'input.mng', 'd072eb3997b849f84172d162c266389a' );

#
# 5) Test 16-bit Portable Network Graphics
# 
++$test;
testRead( 'input_16.png', 'f20dcdb4be59aa17f5e0996d9f2fc789',
			  '283037a3703baa1595b6c146a4187baa');


