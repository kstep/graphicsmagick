#!/usr/local/bin/perl
#
# Test getting attributes.
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1, print "1..25\n"; }
END {print "not ok 1\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

testGetAttribute('input.miff','base-columns','70');

++$test;
testGetAttribute('input.miff','base-filename','input.miff');

++$test;
testGetAttribute('input.miff','base-rows','46');

++$test;
testGetAttribute('input.miff','class','DirectClass');

++$test;
testGetAttribute('input.miff','colors','3019');

++$test;
testGetAttribute('input.miff','columns','70');

++$test;
testGetAttribute('input.miff','directory',undef);

++$test;
testGetAttribute('input.miff','gamma','0');

++$test;
testGetAttribute('input.miff','geometry',undef);

++$test;
testGetAttribute('input.miff','height','46');

++$test;
# Returns undef
testGetAttribute('input.miff','label',undef);

++$test;
testGetAttribute('input.miff','matte','0');

++$test;
testGetAttribute('input.miff','error','0');

++$test;
testGetAttribute('input.miff','montage',undef);

++$test;
testGetAttribute('input.miff','maximum-error','0');

++$test;
testGetAttribute('input.miff','mean-error','0');

++$test;
testGetAttribute('input.miff','rows','46');

++$test;
testGetAttribute('input.miff','signature',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

++$test;
testGetAttribute('input.miff','texture',undef);

++$test;
testGetAttribute('input.miff','type','TrueColorMatte');

++$test;
testGetAttribute('input.miff','units','undefined units');

++$test;
testGetAttribute('input.miff','view',undef);

++$test;
testGetAttribute('input.miff','width','70');

++$test;
testGetAttribute('input.miff','x-resolution','0');

++$test;
testGetAttribute('input.miff','y-resolution','0');

1;
