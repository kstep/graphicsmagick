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
  '19da3123692c66d023074dd1e272119ab2357b9a34855a393ad18c88af01ae86' );

#
# 2) Test 256 color pseudocolor PNG
# 
++$test;
testRead( 'input_256.png',
  'd1340caccc7351834f1ca9693489ee780ac326a0d6bb660cd550c5c28711b2fe' );

#
# 3) Test TrueColor PNG
# 
++$test;
testRead( 'input_truecolor.png',
  'b84c31bbe78d84d0432c7414b25c58599484aa2c7174af8332509966d07f265b' );

#
# 4) Test Multiple-image Network Graphics
# 
++$test;
testRead( 'input.mng',
  'eb089161ebc5ab3964cdec1b72628c4d1c29ebd78f333a3d4a0d47c614fb3897' );

#
# 5) Test 16-bit Portable Network Graphics
# 
++$test;
testRead( 'input_16.png',
  'a6c3f3ce8772cd6aa973ae1f3ebd94ee65292112d3f1e8e86171bf4d931bd181',
  'c87cc12715f3e0619d6fe871bd8132f88facffb3d38dd8869cb262ec6b9c4cef');

