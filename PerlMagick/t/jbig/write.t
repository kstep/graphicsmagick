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
  'cfa3fbe44a1f62cee0d91a5263be1e30ba9a45d35b029826a405fde94d0bc435' );

$test=0; # Keep perl from complaining
