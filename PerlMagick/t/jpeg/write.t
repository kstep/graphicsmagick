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
  '49a015984c1a31f03eff69a120a7d2cf9b3a6ea203b351cd625ffd1f0ce03f46',
  '5ceb0fb09ebda1887a23985f16970b691a15a697b339cb9fb1d6f2fc9944fed2' );

#
# 2) Test with plane-interlaced image
#
++$test;
print( "Plane-interlaced JPEG ...\n" );
testReadWrite( 'input_plane.jpg', 'output_plane_tmp.jpg',
  q/quality=>80, interlace=>'Plane'/,
  '49a015984c1a31f03eff69a120a7d2cf9b3a6ea203b351cd625ffd1f0ce03f46',
  '5ceb0fb09ebda1887a23985f16970b691a15a697b339cb9fb1d6f2fc9944fed2' );

