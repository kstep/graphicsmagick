#!/usr/local/bin/perl
#
# Test reading WMF files
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..1\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/wmf' || die 'Cd failed';

testRead( 'input.wmf',
          '6d642148153c2c19f44357fa6614f4f19071c06bd9fcd9e10ff5caa61fbd262d',
          '0e98bbbbfcc61dab4e3a816c0f71c05465932ac5b5b1aed3702f16be57fb91c9' );



