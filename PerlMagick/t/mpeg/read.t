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
# Motion Picture Experts Group file interchange format (version 2)
#
testRead( 'input.m2v', 'e238246a6910d65a99ceb0b23135c986' );

#
# Motion Picture Experts Group file nterchange format
#
++$test;
testRead( 'input.mpg', '2ab9b274742f981c30ff745cfc355ad8' );

1;
