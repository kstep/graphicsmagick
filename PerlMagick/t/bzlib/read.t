#!/usr/local/bin/perl
#
# Test reading an image which uses BZip compression
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#

BEGIN { $| = 1; $test=1; print "1..1\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/bzlib' || die 'Cd failed';

#
# Test reading BZip compressed MIFF
# 
testRead( 'input.miff', '12643c99ecf96cf1d7e8f5cc62b13c78' );
