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
testRead( 'input.ttf',
  '0d48023028fe3b4d20d23e1edcaa4ddf7cc1b641c6e12de07ca874145aa261ef',
  '77a47616d9ce6e822bb5770ed1f64471b22822f1c449d8992e9ec37b3d1f736b' );

#
# 2) Test drawing text using font
#
++$test;
print("Draw text using font ...\n");
$infile = 'input.ttf';
$md5    = '6b25c4cb314066f828e32f08bb6f306aaef1dacb1ddb22d01c992e4534966040';
$md5_16 = 'add120b8a25cbf8729e7dab873c445ddf5af24316136b70790b9e1fd7daaf69c';

$image=Image::Magick->new;
$status=$image->Set(font=>"\@$infile", pen=>'#0000FF', pointsize=>14);
if( "$status" ) {
  print "$status\n";
  print "not ok $test\n";
} else {
  $status=$image->ReadImage('label:The quick brown fox jumps over the lazy dog.');
  if( "$status" ) {
    print "ReadImage failed!";
    print "not ok $test\n";
  } else {
    $signature=$image->Get('signature');
    if ( $signature ne $md5 && $signature ne $md5_16 ) {
      print "Image: $infile, signatures do not match.\n";
      print "       Computed: $signature\n";
      print "       Expected: $md5\n";
      if ( $md5 ne $md5_16 ) {
	print "      if 16-bit: $md5_16\n";
      }
      #$image->Display();
      print "not ok $test\n";
    } else {
      print "ok $test\n";
    }
  }
}
undef $image;

#
# 3) Test drawing text using annotate
#
++$test;
print("Draw text using annotate ...\n");
$infile = 'input.ttf';
$md5    = '7f810ec092134bd9bd36c7aa18ad9f94d7859bf43cc2710df0a23aec3e12a0ba';
$md5_16 = 'e1c44b19a3d007160a5d86ea1a01fd08010a9bbed25b7681c7432c6304e6c510';

$image=Image::Magick->new;
$status = $image->Set(size=>'330x30');
if( "$status" ) {
  print "Set size failed!";
  print "not ok $test\n";
} else {
  $status=$image->ReadImage("xc:#FFFFFF");
  if( "$status" ) {
    print "ReadImage failed!";
    print "not ok $test\n";
  } else {
    $status = $image->Annotate(text=>'The quick brown fox jumps over the lazy dog.',
			       geometry=>'+4+6',
			       font=>"\@$infile",
			       fill=>"#000000",
			       pointsize=>14);
    if ( $status ) {
      print "Annotate failed!";
      print "not ok $test\n";
    } else {
      $signature=$image->Get('signature');
      if ( $signature ne $md5  && $signature ne $md5_16 ) {
	print "Image: $infile, signatures do not match.\n";
	print "       Computed: $signature\n";
	print "       Expected: $md5\n";
	if ( $md5 ne $md5_16 ) {
	  print "      if 16-bit: $md5_16\n";
	}
	#$image->Display();
	print "not ok $test\n";
      } else {
	#$image->Display();
	print( "ok $test\n" );
      }
    }
  }
}

undef $image;

