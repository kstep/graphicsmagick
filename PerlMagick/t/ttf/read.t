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
  'a8bb1524423597338799d61c9787786e0f53355ea694e5da11e8ec3f3870026f',
  '4082650fe3956d93b20629436c7847384e305cad5c8ee8d8c0b37737a5f6a00b' );

#
# 2) Test drawing text using font
#
++$test;
print("Draw text using font ...\n");
$infile = 'input.ttf';
$md5    = '705493c0e9095a3d2afc33337a1eece37ed23a897a3a0ad2547903dd9a29b062';
$md5_16 = '4c580faec96a3478385755ca7e97f86e3ac891ec998acef01f8f78c455cc6512';

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
$md5    = 'cca7437bc80e1003beb6b8c86118f1c85cbc072d4a0160972a208bea70c43159';
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

