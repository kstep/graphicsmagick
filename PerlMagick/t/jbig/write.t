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
               'd93259fc2d6fddfe939c0d3601a78f4a',
               '9967d7d781c6e3792b2f63128ff6369c' );

$test=0; # Keep perl from complaining
