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
               '8b2a25ef2b5d9da5fc433fdc336a4d11' );

#
# Create an MPEG
#
++$test;
testReadWrite( 'mpg:input.mpg',
               'mpg:output.mpg',
               q//,
               '63c4fb1ca7a82594894981deb2611951' );

1;
