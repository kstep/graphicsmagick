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
  '78bc2f06645698747e64873639abaed3ccf715a6ee5efcec023368dd6aefdf86' );

#
# Motion Picture Experts Group file interchange format
#
++$test;
testRead( 'input.mpg',
  'b8704fece2a1f348b1a8f273ea6554b1790f5cce06840c3512262056c7b92d81' );

1;
