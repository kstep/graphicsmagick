#!/usr/local/bin/perl
#
# Test read image method on JBIG image
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..1\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/jbig' || die 'Cd failed';

testRead( 'input.jbig',
  'f0ae406bc61e37ad3d9110633aaf9befde1d69599b7488caa472e9c330f89fee');
