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
  '5346bdbf1815b28e9843bbc0a72a2fdc1c946eecd294e5cf864de0b6a92e3917' );

#
# Motion Picture Experts Group file interchange format
#
++$test;
testRead( 'input.mpg',
  '1a61d9e518d47f5e939064ea403d25139c19a4e98446de2a466d381f018a49e0' );

1;
