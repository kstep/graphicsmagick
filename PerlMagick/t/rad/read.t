#!/usr/local/bin/perl
#
# Test reading Radiance file format
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..1\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/rad' || die 'Cd failed';

testRead( 'input.rad',
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a' );
