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
  '508bd5bdf3be32f2be07ab2493ba5ad8e61ae917174a7cb15ece971e51a92195',
  '685a7c5c7cca57fcea3b4800e5babfe809a5bf9ba918175c05fd20ef996abbf2' );

#
# 2) Test drawing text using font
#
++$test;
print("Draw text using font ...\n");
$infile = 'input.ttf';
$md5    = '63da735482d91fcb5959e08849cba86250b5206c81437a9941cda69d11705f6d';
$md5_16 = '4d1b042e54b8b5c4716445e7c7d512fec7af5263bb4ea8ccea1203d2ba96d7dd';

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
$md5    = '3aa0c3a2d6cc1d8720220afd1f3749db25749dad84cb882d23c3440579aeeb12';
$md5_16 = 'da52bcb5a2e070774ae22d8bb2acb0216363ca3bfa87f0bb10482df80ef2daaf';

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

