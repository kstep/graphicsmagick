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
print( "Non-interlaced JPEG ...\n" );
testReadWrite( 'input.jpg',
	       'output_tmp.jpg',
	       q/quality=>80, interlace=>'None'/,
	       'a5bfbd2c2af802715b2259a28beabf97',
               '5ae2cf680c8560979a8d717698cfae93' );

#
# 2) Test with plane-interlaced image
#
++$test;
print( "Plane-interlaced JPEG ...\n" );
testReadWrite( 'input_plane.jpg', 'output_plane_tmp.jpg',
	       q/quality=>80, interlace=>'Plane'/,
	       '5ae2cf680c8560979a8d717698cfae93',
               'a5bfbd2c2af802715b2259a28beabf97' );


