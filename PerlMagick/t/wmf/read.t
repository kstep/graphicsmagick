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
          '89edd67a8edb6c1625d3c4e13d8fbce978261c0825e31c93e26906910285f319' );
++$test;
testRead( 'clock.wmf',
          '5a71ebef41ddcf702d1d2a63d6114acb9ccfd5e718215b705e0769f074fb4303',
          '7477f68dfc34248c58ba19d9f5276ac5e520f62e6206f33c5638270be44e5c15' );


