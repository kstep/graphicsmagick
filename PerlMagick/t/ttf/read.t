#!/usr/local/bin/perl
#
# Test read image method on TrueType font
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
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
testRead( 'input.ttf', 'aa30fdc71b5453685e1e4311906a38e0',
	               '9e2748a627f82a7b27e3d16f2894b879' );

#
# 2) Test drawing text using font
#
++$test;
print("Draw text using font ...\n");
$infile = 'input.ttf';
$md5    = 'cf591f265408d670dac77c53dd009086';
$md5_16 = '73ea848cb6318a7249281eb5e0c15527';

$image=Image::Magick->new;
$status=$image->Set(font=>"\@$infile", fill=>'#0000FF', pointsize=>14);
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
$md5    = '2ee382bbb6e18866b05ba14b99dceed1';
$md5_16 = '16a2f543d7290211717cffe96333ea66';

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

