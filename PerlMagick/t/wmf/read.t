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
          'b477366dd4e4f892eac47297e9a0835efe55dc1df5e218ed344270b6d08d5671',
          'a5ad75b745e73039f4bd43da8f26f9d082032a45dc4f2bfe9d19a14761a3a01f' );


