#!/usr/local/bin/perl
#
# Test read image method on TrueType font
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#

BEGIN { $| = 1; $test=1; print "1..2\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/ttf' || die 'Cd failed';

#
# 1) Test default ImageMagick read operation on font
#
testRead( 'input.ttf', 'baaf49ac19e459bd9cdf877024a9ffb8' );

#
# 2) Test drawing text using font
#
++$test;
$infile = 'input.ttf';
$md5    = '27dcff73649bcde79c674bcd47a03157';

$image=Image::Magick->new;
$x=$image->Set(font=>"\@$infile", pen=>'#0000FF', pointsize=>14);
if( "$x" ) {
  print "$x\n";
  print "not ok $test\n";
} else {
  $x=$image->ReadImage('label:The quick brown fox jumps over the lazy dog.');
  if( "$x" ) {
    print "ReadImage failed!";
    print "not ok $test\n";
  } else {
    $signature=$image->Get('signature');
    if ( $signature ne $md5 ) {
      print "Got: $signature, expected: $md5\n";
      print "not ok $test\n";
    } else {
      print "ok $test\n";
    }
  }
}
undef $image;


