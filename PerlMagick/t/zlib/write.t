#!/usr/local/bin/perl
#
# Test writing files using zlib-based compression
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..1\n"; }
END {print "not ok $test\n" unless $loaded;}

use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/zlib' || die 'Cd failed';

#
# 1) Test writing Zip-compressed MIFF
#

testReadWrite( 'input.miff',
  'output.miff',
  q/compress=>'Zip'/,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a' );

$test = 0;  # Quench PERL compliaint

