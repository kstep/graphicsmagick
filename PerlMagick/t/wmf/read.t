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
          'ad68c7f9603996844e956d0de048676574f75f428adfaedb3ce98fe40ba34d32',
          '5686ec552c34b022e08af38d5e371e9d1a46dd7543d618db82c6c8c5076df58c' );
++$test;
testRead( 'clock.wmf',
          '474b042b7965c290036cf8cc31b6e868f65eb7404e8afd521055b2139c0210b8',
          'd017f0eebdd40d1b3548a5b231f63fadc3294a1337ecb8b87e7c2a24ec111a75' );


