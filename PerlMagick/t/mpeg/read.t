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
  '9310d39e4960ba9a15deda5bc22d25fa820636272f9aecadd010042c59d1e02e' );

#
# Motion Picture Experts Group file interchange format
#
++$test;
testRead( 'input.mpg',
  '80fb37d218a2383d0c93092625dc5a648d41d05c9c6cb4fdff9d9e7a7df411aa' );

1;
