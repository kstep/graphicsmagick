#!/usr/local/bin/perl
#
# Test reading PNG images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#

BEGIN { $| = 1; $test=1; print "1..4\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/png' || die 'Cd failed';

#
# 1) Test Monochrome PNG
# 
testRead( 'input_mono.png', 'd6a0c3ad0b6b8677f877290da26bda20' );

#
# 2) Test 256 color pseudocolor PNG
# 
++$test;
testRead( 'input_256.png', '8de05f913b3269ce6acafbfba975837f' );

#
# 3) Test TrueColor PNG
# 
++$test;
testRead( 'input_truecolor.png', '4449bf016e98250d673074583bc5b14f' );

#
# 4) Test Multiple-image Network Graphics
# 
++$test;
testRead( 'input.mng', 'b1e79584c38f54343f8723d9818a74a5' );

