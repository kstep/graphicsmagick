#!/usr/local/bin/perl
#
# Test reading PNG images when 16bit support is enabled
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#

BEGIN { $| = 1; $test=1; print "1..5\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/png' || die 'Cd failed';

#
# 1) Test Monochrome PNG
# 
testRead( 'input_mono.png',
  'f10cb256abcf1042d51d4738c2362faafef13a99008d5051bff76da5ac1a91b2' );

#
# 2) Test 256 color pseudocolor PNG
# 
++$test;
testRead( 'input_256.png',
  'bab21ad121ab95cb8db2df82ee400e992cc79861cce679c50bf533814b59b840' );

#
# 3) Test TrueColor PNG
# 
++$test;
testRead( 'input_truecolor.png',
  '86754b683ab208cd9cb596bbf5db8cdbb8806bbee0503a5ab287be475c345492' );

#
# 4) Test Multiple-image Network Graphics
# 
++$test;
testRead( 'input.mng',
  'e3224a33eb05532dd5be635a45976a760c6e0f7501bda47bab18e25b36179f6b' );

#
# 5) Test 16-bit Portable Network Graphics
# 
++$test;
testRead( 'input_16.png',
  '4eeef86b727f91d7f4bddfe8edbd7264ab835208c1b4edb199fdb40c1cdcba9f',
  '07696b623faf33d67921b80b29ff905ff9d546c445a54026a32d263adbf5f6b9');

