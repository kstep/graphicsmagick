#!/usr/local/bin/perl
#
# Test read image method on non-interlaced JPEG.
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..2\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/jpeg' || die 'Cd failed';

#
# 1) Test non-interlaced image read
# 
testRead( 'input.jpg', '67646163ce13f2ee08c2cfce772386a7' );

#
# 2) Test plane-interlaced image read
# 
++$test;
testRead( 'input_plane.jpg', '67646163ce13f2ee08c2cfce772386a7' );


