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
          '0eac1bba3997f623e5288a525a861c86c1af51f3e292d71343b18f600af30105',
          '89edd67a8edb6c1625d3c4e13d8fbce978261c0825e31c93e26906910285f319' );
++$test;
testRead( 'clock.wmf',
          '09f8d11e9b255a7668187854b3762798dd1dcad759c5d9632190f883200c194d',
          '9c2ccd635040e672c17b3e68efabf3b65ef3797bb23fbe94a471d31242d139e0' );


