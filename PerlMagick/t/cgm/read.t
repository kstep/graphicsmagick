#!/usr/local/bin/perl
#
# Test reading CGM files
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..1\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/cgm' || die 'Cd failed';

testRead( 'input.cgm', 'b4680bea0efb73d2e9ed875e71e590f2' );

