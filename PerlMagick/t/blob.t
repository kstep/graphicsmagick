#!/usr/local/bin/perl
#
# Test image blobs.
#
BEGIN { $| = 1; $test=1, print "1..1\n"; }
END {print "not ok 1\n" unless $loaded;}
use Image::Magick;
$loaded=1;

chdir 't' || die 'Cd failed';

$image = new Image::Magick;
$image->Read( 'input.miff' );
@blob = $image->ImageToBlob();
undef $image;

$image=Image::Magick->new( magick=>'MIFF' );
$image->BlobToImage( @blob );

if ($image->Get('signature') ne 
    'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c')
  { print "not ok $test\n"; }
else
  { print "ok $test\n"; }

1;
