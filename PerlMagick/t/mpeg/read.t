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
testRead( 'input.m2v',
  '0b200f8c03ce5be229a31ee1a77fed26dab139eb891ad691265f4b8bb4f8c12c' );

#
# Motion Picture Experts Group file interchange format
#
++$test;
testRead( 'input.mpg',
  'cd129210ed436f452349cc9c306cce1d8203d95f836f0a43ad35a90587cfafc7' );

1;
