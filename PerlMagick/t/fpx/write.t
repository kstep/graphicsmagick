#!/usr/local/bin/perl
#
# Test writing FPX images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..4\n"; }
END {print "not ok $test\n" unless $loaded;}

use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/fpx' || die 'Cd failed';

#
# 1) Test Black-and-white, bit_depth=1 FPX
# 
print( "1-bit grayscale FPX ...\n" );
testReadWrite( 'input_bw.fpx', 'output_bw.fpx', q/quality=>95/,
   'f3afd8f931e3be3ddd7e99c43bd5d854257dee6726cdc546962d60c5d9405047');

#
# 2) Test monochrome image
#
++$test;
print( "8-bit grayscale FPX ...\n" );
testReadWrite( 'input_mono.fpx',
   'output_mono.fpx', '',
   'f3afd8f931e3be3ddd7e99c43bd5d854257dee6726cdc546962d60c5d9405047');
#
# 3) Test pseudocolor image
#
++$test;
print( "8-bit indexed-color FPX ...\n" );
testReadWrite( 'input_256.fpx',
   'output_256.fpx',
   q/quality=>54/,
   'd120f2f0f295b506a92070051dd8e7b4a83f777909fd4e73d75bd55c53a55a95' );
#
# 4) Test truecolor image
#
++$test;
print( "24-bit Truecolor FPX ...\n" );
testReadWrite( 'input_truecolor.fpx',
   'output_truecolor.fpx',
   q/quality=>55/,
   '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f' );
