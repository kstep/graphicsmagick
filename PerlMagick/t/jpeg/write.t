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
  '90c0ff3b78757873c25c5c788c877c08b6af1ce6f815fe48c69e50ea3b523169',
  'a32dfb4848776c8693bb83727cac288abf9c6a8c084d82a517112d7341429bdb' );

#
# 2) Test with plane-interlaced image
#
++$test;
print( "Plane-interlaced JPEG ...\n" );
testReadWrite( 'input_plane.jpg', 'output_plane_tmp.jpg',
  q/quality=>80, interlace=>'Plane'/,
  '90c0ff3b78757873c25c5c788c877c08b6af1ce6f815fe48c69e50ea3b523169',
  'a32dfb4848776c8693bb83727cac288abf9c6a8c084d82a517112d7341429bdb' );

