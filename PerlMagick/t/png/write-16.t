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
  'd1340caccc7351834f1ca9693489ee780ac326a0d6bb660cd550c5c28711b2fe' );

#
# 2) Test truecolor image
#
++$test;
testReadWrite( 'input_truecolor.png',
  'output_truecolor.png',
  q/quality=>55/,
  'b84c31bbe78d84d0432c7414b25c58599484aa2c7174af8332509966d07f265b' );

#
# 3) Test monochrome image
#
++$test;
testReadWrite( 'input_mono.png',
  'output_mono.png', '',
  '19da3123692c66d023074dd1e272119ab2357b9a34855a393ad18c88af01ae86' );

#
# 4) Test Multiple-image Network Graphics
#
++$test;
testReadWrite( 'input.mng',
  'output.mng',
  q/quality=>55/,
  'eb089161ebc5ab3964cdec1b72628c4d1c29ebd78f333a3d4a0d47c614fb3897' );

#
# 5) Test 16-bit Portable Network Graphics
# 
++$test;
testReadWrite( 'input_16.png',
  'output_16.png',
  q/quality=>55/,
  'a6c3f3ce8772cd6aa973ae1f3ebd94ee65292112d3f1e8e86171bf4d931bd181',
  'c87cc12715f3e0619d6fe871bd8132f88facffb3d38dd8869cb262ec6b9c4cef');


