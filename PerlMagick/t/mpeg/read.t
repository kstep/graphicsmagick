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
testRead( 'input.m2v', '609295813031c19f8ecd8d6c1da7c398',
                       'e178d3e510a0f41cbc69a9ac2d27ae36' );

#
# Motion Picture Experts Group file interchange format
#
++$test;
testRead( 'input.mpg', '5592f8bd639be51bb3dc2deee6bb6cd2',
                       'c255ba7f9d75587314c8d057ea36a8f0' );

1;
