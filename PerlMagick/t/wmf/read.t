#!/usr/local/bin/perl
#
# Test reading WMF files
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..1\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/wmf' || die 'Cd failed';

testRead( 'input.wmf',
          'a298bfb1e44be8c970ad9ba14c164b988ad836dab97e9a4f27c6acc85cfa186b',
          '480f282710faad850cb384c0bad2c60cba3bcabebc2fea4f5d3a283b9820a859' );



