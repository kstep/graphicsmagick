#!/usr/local/bin/perl
#
# Test reading PNG images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#

BEGIN { $| = 1; $test=1; print "1..6\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/png' || die 'Cd failed';

#
# 1) Test Black-and-white, bit_depth=1 PNG
# 
print( "1-bit grayscale PNG ...\n" );
testRead( 'input_bw.png',
  'ebab9db9d3310e1d98b3403596724094b349fbb631f38deb9139f6b38242c85a' );

#
# 2) Test Monochrome PNG
# 
++$test;
print( "8-bit grayscale PNG ...\n" );
testRead( 'input_mono.png',
  'f10cb256abcf1042d51d4738c2362faafef13a99008d5051bff76da5ac1a91b2' );

#
# 3) Test 16-bit Portable Network Graphics
# 
++$test;
print( "16-bit grayscale PNG ...\n" );
testRead( 'input_16.png',
  '21b59be11e5dd15dbe5ff4a127eb90c2e7fb1bd687e3a3b6d3b829414e5d928b',
  '07696b623faf33d67921b80b29ff905ff9d546c445a54026a32d263adbf5f6b9' );
#
# 4) Test 256 color pseudocolor PNG
# 
++$test;
print( "8-bit indexed-color PNG ...\n" );
testRead( 'input_256.png',
  'bab21ad121ab95cb8db2df82ee400e992cc79861cce679c50bf533814b59b840' );

#
# 5) Test TrueColor PNG
# 
++$test;
print( "24-bit Truecolor PNG ...\n" );
testRead( 'input_truecolor.png',
  '86754b683ab208cd9cb596bbf5db8cdbb8806bbee0503a5ab287be475c345492' );

#
# 6) Test Multiple-image Network Graphics
# 
++$test;
print( "MNG with 24-bit Truecolor PNGs...\n" );
testRead( 'input.mng',
  'e3224a33eb05532dd5be635a45976a760c6e0f7501bda47bab18e25b36179f6b' );

