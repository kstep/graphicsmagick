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
  '560d7aff7bb087553080fcaad9186bba564c7f631d447bce067d51f492c5296b');
