#!/usr/local/bin/perl
#
# Test reading WMF files
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..2\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/wmf' || die 'Cd failed';

testRead( 'wizard.wmf',
          'a8df30a0ab1dff15315bb2a3024548bc257cf3f2c0a542045d5b69ab0a988268',
          '4358b4dba2c3213a58135b2d2271f6000371095e6d6135aa2163ab456f6cbcf3' );
++$test;
testRead( 'clock.wmf',
          '78813895830e7ffa1f7fda91be23b997ef404fb79756f969dda6db5d3086b441',
          'ebc0b5f23995f6d732600ca91378576c4a7cfbd2c21f1712b584bbf6eef03a26' );


