#!/usr/local/bin/perl
#
# Test writing MPEG
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.

BEGIN { $| = 1; $test=1; print "1..2\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/mpeg' || die 'Cd failed';

#
# Create an MPEG (Version 2)
#
testReadWrite( 'M2V:input.m2v',
               'M2V:output.m2v',
               q//,
               '7b97cb66eee0b9ef0175527931ebef22' );

#
# Create an MPEG
#
++$test;
testReadWrite( 'mpg:input.mpg',
               'mpg:output.mpg',
               q//,
               '9e3d62e1fda462feb70c6a714d7a2d24' );

1;
