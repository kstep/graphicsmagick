#!/usr/local/bin/perl
#
# Test writing PNG images when 16bit support is enabled
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
# 1) Test pseudocolor image
#
testReadWrite( 'input_256.png',
  'output_256.png',
  q/quality=>54/,
  '852777db495289e4e42f98fd0136d4831caed8f23ef7f56503698b24d90467ce' );

#
# 2) Test truecolor image
#
++$test;
testReadWrite( 'input_truecolor.png',
  'output_truecolor.png',
  q/quality=>55/,
  'f3a02ab84dc50e6d4f57f4dbfd801d565391809e9affb95271a1a2393d2d50ef' );

#
# 3) Test monochrome image
#
++$test;
testReadWrite( 'input_mono.png',
  'output_mono.png', '',
  '064000205c670c86e2e7521c7c5a437278da6b72e66173ee2c9cd33f27626907' );

#
# 4) Test Multiple-image Network Graphics
#
++$test;
testReadWrite( 'input.mng',
  'output.mng',
  q/quality=>55/,
  '1d76d4e1f1e085147a7a3379fc8df0413b2c7072ef247f7c52024f4104068919' );

#
# 5) Test 16-bit Portable Network Graphics
# 
++$test;
testReadWrite( 'input_16.png',
  'output_16.png',
  q/quality=>55/,
  '415303615a36716dd3c7aade7b0d991b4fe04178f48c1725e01675226c1647de',
  '4eeef86b727f91d7f4bddfe8edbd7264ab835208c1b4edb199fdb40c1cdcba9f');


