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
print( "Non-interlaced JPEG ...\n" );
testRead( 'input.jpg',
  '8a557c34eb033a3f5da2416f1cc5eae5179b50cb1d884a7956e8edbfc6ba5005' );

#
# 2) Test plane-interlaced image read
# 
++$test;
print( "Plane-interlaced JPEG ...\n" );
testRead( 'input_plane.jpg',
  '8a557c34eb033a3f5da2416f1cc5eae5179b50cb1d884a7956e8edbfc6ba5005' );

