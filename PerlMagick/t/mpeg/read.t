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
testRead( 'input.mpg', '6f169497f04bc2517f6ffb73b6f42cf5' );

#
# Motion Picture Experts Group file interchange format (version 2)
#
++$test;
testRead( 'input.m2v', '0a023e5e73c3dcf29b10daf856c9d052' );
