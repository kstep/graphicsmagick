#!/usr/local/bin/perl
#
# Test read image method on TrueType font
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us
#
# Signatures computed using freetype 2.0.4
#

BEGIN { $| = 1; $test=1; print "1..3\n"; }
END {print "not ok $test\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/ttf' || die 'Cd failed';

#
# 1) Test default ImageMagick read operation on font
#
print("Default ImageMagick read ...\n");
#testRead( 'input.ttf',
#  'ce3a927eacd81d1caa832d36f60a15fc5c531755a07687729d103efb5c4bab85',
#  '6c5a788824db7c89db7609b96108da8392076227136ce45ad1fe21d9c5e5d7bd' );

testReadCompare('input.ttf', '../reference/ttf/read.miff', q/size=>'512x512'/, 0, 0, 0);

#
# 2) Test drawing text using font
#
++$test;
print("Draw text using font ...\n");
testReadCompare(q/label:The quick brown fox jumps over the lazy dog./,
                q!../reference/ttf/label.miff!,
                q!font=>'input.ttf', fill=>'#0000FF', pointsize=>14!,
                0, 0, 0);

#$infile = 't/input.ttf';
#$md5    = 'f85f556f418a80a8e30bd01254b1735fa4b730174706ddab39e676014e69e1ee';
#$md5_16 = 'b114c9d1bea88fd75d88094f0b69ef4714c3e7d74c3e3dd158fd5aea7f5f6aea';

#$image=Image::Magick->new;
#$status=$image->Set(font=>"$infile", pen=>'#0000FF', pointsize=>14);
#if( "$status" ) {
#  print "$status\n";
#  print "not ok $test\n";
#} else {
#  $status=$image->ReadImage('label:The quick brown fox jumps over the lazy dog.');
#  if( "$status" ) {
#    print "ReadImage failed!";
#    print "not ok $test\n";
#  } else {
#    $signature=$image->Get('signature');
#    if ( $signature ne $md5 && $signature ne $md5_16 ) {
#      print "Image: $infile, signatures do not match.\n";
#      print "       Computed: $signature\n";
#      print "       Expected: $md5\n";
#      if ( $md5 ne $md5_16 ) {
#	print "      if 16-bit: $md5_16\n";
#      }
#      #$image->Display();
#      print "not ok $test\n";
#    } else {
#      print "ok $test\n";
#    }
#  }
#}
#undef $image;

#
# 3) Test drawing text using annotate
#
++$test;
print("Draw text using annotate ...\n");
testFilterCompare('xc:#FFFFFF',
                  q!size=>'250x20'!,
                  q!../reference/ttf/annotate.miff!,
                  'Annotate',
                  q!text=>'The quick brown fox jumps over the lazy dog.',
                  gravity=>'NorthWest',
                  geometry=>'+6+14',
                  font=>'input.ttf',
                  fill=>'#FF0000',
                  pointsize=>14!,
                  0, 0, 0);

#$infile = 'input.ttf';
#$md5    = 'ebda8f8d290e9d77a3016b1c829aeb38d4830ad99a27069ea7fac87485206411';
#$md5_16 = '0b7bac508473eac587e39da285ae9540d2769263ac1d89dc00c9557a80a3c4e1';

#$image=Image::Magick->new;
#$status = $image->Set(size=>'330x30');
#if( "$status" ) {
#  print "Set size failed!";
#  print "not ok $test\n";
#} else {
#  $status=$image->ReadImage("xc:#FFFFFF");
#  if( "$status" ) {
#    print "ReadImage failed!";
#    print "not ok $test\n";
#  } else {
#    $status = $image->Annotate(text=>'The quick brown fox jumps over the lazy dog.',
#			       geometry=>'+4+6',
#			       font=>"$infile",
#			       fill=>"#000000",
#			       pointsize=>14);
#    if ( $status ) {
#      print "Annotate failed!";
#      print "not ok $test\n";
#    } else {
#      $signature=$image->Get('signature');
#      if ( $signature ne $md5  && $signature ne $md5_16 ) {
#	print "Image: $infile, signatures do not match.\n";
#	print "       Computed: $signature\n";
#	print "       Expected: $md5\n";
#	if ( $md5 ne $md5_16 ) {
#	  print "      if 16-bit: $md5_16\n";
#	}
#	#$image->Display();
#	print "not ok $test\n";
#      } else {
#	#$image->Display();
#	print( "ok $test\n" );
#      }
#    }
#  }
#}

#undef $image;

1;
