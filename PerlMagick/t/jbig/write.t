#!/usr/local/bin/perl
#
# Test write image method on JBIG image
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..1\n"; }
END {print "not \n" unless $loaded;}

use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/jbig' || die 'Cd failed';

testReadWrite( 'input.jbig',
  'output.jbig',
  '',
  'f0ae406bc61e37ad3d9110633aaf9befde1d69599b7488caa472e9c330f89fee' );

$test=0; # Keep perl from complaining
