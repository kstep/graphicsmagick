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
  '52a69597e38f21fd52d38b9b7dab606caf9ed9b5a0658fec0e8d80056f349053',
  '07e0a40fb427b89a21003752f68af79616b39cc73ad83c7aa8d75d010394d58c' );

#
# 2) Test drawing text using font
#
++$test;
print("Draw text using font ...\n");
$infile = 'input.ttf';
$md5    = 'f85f556f418a80a8e30bd01254b1735fa4b730174706ddab39e676014e69e1ee';
$md5_16 = 'd6d3fe1140984b60ae545394e72cade03faf639086b6f264b708f5c51419fd24';

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
$md5    = 'ebda8f8d290e9d77a3016b1c829aeb38d4830ad99a27069ea7fac87485206411';
$md5_16 = 'eeca51a05a87b2b251500c38255f8c80aa368ac6255fe08914dcd2dbb451028c';

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

