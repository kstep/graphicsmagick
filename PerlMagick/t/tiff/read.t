#!/usr/local/bin/perl
#
# Test reading TIFF images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..3\n"; }
END {print "not ok $test\n" unless $loaded;}

use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/tiff' || die 'Cd failed';

#
# 1) Test Reading Monochrome
# 
print("Monochrome ...\n");
testRead ( 'input_mono.tiff',
  '62311570ee3d0c0fe50e9aadd7922d6c836839c70d092b1136880c9411e9b39e' );

#
# 2) Test reading PseudoColor
#
++$test;
print("PseudoColor ...\n");
testRead( 'input_256.tiff',
  '66ead3a3deaff3fedc98d0b4b0a0a4c4e5138d435932444363a62ace8882b263' );

#
# 3) Test Reading TrueColor
# 
++$test;
print("TrueColor image ...\n");
testRead( 'input_truecolor.tiff',
  '0a573e36c8e3da16a3184513b72a3ebb1978c182ac1aa7c1ecb1326880bf1dd0' );

