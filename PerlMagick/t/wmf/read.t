#!/usr/local/bin/perl
#
# Test reading WMF files
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..2\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/wmf' || die 'Cd failed';

testRead( 'wizard.wmf',
          '9ca87d864406234af188dea8a3f5c7db7b1c1802ceb45725747a2379deefd03f',
          '5686ec552c34b022e08af38d5e371e9d1a46dd7543d618db82c6c8c5076df58c' );
++$test;
testRead( 'clock.wmf',
          '5a71ebef41ddcf702d1d2a63d6114acb9ccfd5e718215b705e0769f074fb4303',
          'a5ad75b745e73039f4bd43da8f26f9d082032a45dc4f2bfe9d19a14761a3a01f' );


