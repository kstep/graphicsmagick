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
testRead( 'input.mpg', '71d945c8ce8de757816d68823110c6ca' );

#
# Motion Picture Experts Group file interchange format (version 2)
#
++$test;
testRead( 'input.m2v', '4ba485b7228a515095f9406c99e5fab2' );
