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

testRead( 'input.cgm', '63ef31cd059d24bf42dad792c7ea0127',
                       '78240dd4983c777c3a2f707e01754df5' );



