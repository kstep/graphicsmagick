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
# Create an MPEG
#
testReadWrite( 'MPEG:input.mpg',
               'MPEG:output.mpg',
               q//,
               '893aa3867a357a95ee2b991b38e9f47b' );

#
# Create an MPEG (Version 2)
#
++$test;
testReadWrite( 'M2V:input.m2v',
               'M2V:output.m2v',
               q//,
               '4d63b90e33df433d520e660e46956eae' );

1;
