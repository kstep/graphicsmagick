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
          '0eac1bba3997f623e5288a525a861c86c1af51f3e292d71343b18f600af30105',
          '26924d63993e9066507fa180d1c84a1b71fd02b0c5a0a951c52b9868bbadd06a' );
++$test;
testRead( 'clock.wmf',
          '3ba1e718dbec9f9ac296175828c276fdca7f8c4e7b6f05562a7a043e7a827fa4',
          'c6a6d9fb10256e8234490a30493ddc89e55287a85fc1341377b7dccddf75a021' );


