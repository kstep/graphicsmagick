#!/usr/local/bin/perl
#
# Test writing Radiance file format
#
# Currently supported tests are for formats that ImageMagick
# knows how to both read and write.
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.

BEGIN { $| = 1; $test=1; print "1..1\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/rad' || die 'Cd failed';

testReadWrite( 'RAD:input.rad',
  'MIFF:output.rad',
  q//,
  '703aff535f3822d56d9fc91ba39629bd4939bbfe65786e25c267993e7ab32829' );

1;
