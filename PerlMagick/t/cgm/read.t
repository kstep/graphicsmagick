#!/usr/local/bin/perl
#
# Test reading CGM files
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..1\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/cgm' || die 'Cd failed';

testRead( 'input.cgm', '894f200455b3a05b92e6a87c4b61f214',
                       '78240dd4983c777c3a2f707e01754df5' );



