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
          '25ee5c13994a8d14f5d6dc000716d15460befbe32a2191c82ab29c53af98435e',
          '9c2ccd635040e672c17b3e68efabf3b65ef3797bb23fbe94a471d31242d139e0' );


