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
               '562640e0c3044629a29a4f77913337df' );

#
# Create an MPEG (Version 2)
#
++$test;
testReadWrite( 'M2V:input.m2v',
               'M2V:output.m2v',
               q//,
               '14d779490ad401d17df02284b8dac0f0' );

1;
