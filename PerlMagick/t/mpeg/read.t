#!/usr/local/bin/perl
#
# Test reading MPEG files
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..2\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/mpeg' || die 'Cd failed';

#
# Motion Picture Experts Group file nterchange format
#
testRead( 'input.mpg', '7feb98ed7a2f465a178a49e197a75a8c' );

#
# Motion Picture Experts Group file interchange format (version 2)
#
++$test;
testRead( 'input.m2v', '9f57a5315d5f7d8f01a1d826e09d1ba9' );
