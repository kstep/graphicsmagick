#!/usr/local/bin/perl
#
# Test writing TIFF images
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
# 1) Test pseudocolor image
#
print("PsuedoColor image ...\n");
testReadWrite( 'input_256.tiff',
  'output_256.tiff',
  q/quality=>54/,
  'a570df6d4c214d08bccaaee856f617883fee4f96494af6ef5e25c76d86ea0e0a');

#
# 2) Test truecolor image
#
++$test;
print("TrueColor image ...\n");
testReadWrite( 'input_truecolor.tiff',
  'output_truecolor.tiff',
  q/quality=>55/,
  '326599218925e7aa27c735d482332744cccf41f460a1ba2d276a2d172f2b3de0' );

#
# 3) Test monochrome image
#
++$test;
print("Monochrome image ...\n");
testReadWrite(  'input_mono.tiff',
  'output_mono.tiff',
  q//,
  'dff1c15caf8bc7f460cfa14d4422e77a32569ae7510c881d9b7d907b0d4077dd' );
