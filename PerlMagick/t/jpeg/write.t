#!/usr/local/bin/perl
#
# Test reading JPEG images
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
# 1) Test with non-interlaced image
#
testReadWrite( 'input.jpg',
	       'output_tmp.jpg',
	       q/quality=>80, interlace=>'None'/,
	       'ae69e28821c16a74c42cea4bef2496f4' );

#
# 2) Test with plane-interlaced image
#
++$test;
testReadWrite( 'input_plane.jpg', 'output_plane_tmp.jpg',
	       q/quality=>80, interlace=>'Plane'/,
	       'ae69e28821c16a74c42cea4bef2496f4' );


